
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <libss.h>
#include <libsf.h>
#include <libsr.h>
#include <libsv.h>
#include <libsd.h>
#include <libsi.h>

static inline sibranch*
si_branchcreate(si *index, sdc *c, sinode *parent, svindex *vindex, uint64_t vlsn)
{
	sr *r = index->r;
	sibranch *branch = NULL;

	/* in-memory mode blob */
	int rc;
	ssblob copy, *blob = NULL;
	if (index->scheme->in_memory) {
		ss_blobinit(&copy);
		rc = ss_blobensure(&copy, 10ULL * 1024 * 1024);
		if (ssunlikely(rc == -1)) {
			sr_oom_malfunction(r->e);
			return NULL;
		}
		blob = &copy;
	}

	svmerge vmerge;
	sv_mergeinit(&vmerge);
	rc = sv_mergeprepare(&vmerge, r, 1);
	if (ssunlikely(rc == -1))
		return NULL;
	svmergesrc *s = sv_mergeadd(&vmerge, NULL);
	ss_iterinit(sv_indexiter, &s->src);
	ss_iteropen(sv_indexiter, &s->src, r, vindex, SS_GTE, NULL, 0);
	ssiter i;
	ss_iterinit(sv_mergeiter, &i);
	ss_iteropen(sv_mergeiter, &i, r, &vmerge, SS_GTE);

	/* merge iter is not used */
	sdmergeconf mergeconf = {
		.size_stream     = UINT32_MAX,
		.size_node       = UINT64_MAX,
		.size_page       = index->scheme->node_page_size,
		.checksum        = index->scheme->node_page_checksum,
		.compression     = index->scheme->compression,
		.compression_key = index->scheme->compression_key,
		.vlsn            = vlsn,
		.save_delete     = 1,
		.save_update     = 1
	};
	sdmerge merge;
	sd_mergeinit(&merge, r, &i, &c->build, &c->update, &mergeconf);

	while ((rc = sd_merge(&merge)) > 0)
	{
		/* write open seal */
		uint64_t seal = parent->file.size;
		rc = sd_writeseal(r, &parent->file, blob);
		if (ssunlikely(rc == -1))
			goto e0;

		/* write pages */
		uint64_t offset = parent->file.size;
		while ((rc = sd_mergepage(&merge, offset)) == 1)
		{
			rc = sd_writepage(r, &parent->file, blob, merge.build);
			if (ssunlikely(rc == -1))
				goto e0;
			offset = parent->file.size;
		}
		if (ssunlikely(rc == -1))
			goto e0;
		sdid id = {
			.parent = parent->self.id.id,
			.flags  = SD_IDBRANCH,
			.id     = sr_seq(r->seq, SR_NSNNEXT)
		};
		rc = sd_mergecommit(&merge, &id, parent->file.size);
		if (ssunlikely(rc == -1))
			goto e0;

		/* write index */
		rc = sd_writeindex(r, &parent->file, blob, &merge.index);
		if (ssunlikely(rc == -1))
			goto e0;
		if (index->scheme->sync) {
			rc = ss_filesync(&parent->file);
			if (ssunlikely(rc == -1)) {
				sr_malfunction(r->e, "file '%s' sync error: %s",
				               parent->file.file, strerror(errno));
				goto e0;
			}
		}

		SS_INJECTION(r->i, SS_INJECTION_SI_BRANCH_0,
		             sd_mergefree(&merge);
		             sr_malfunction(r->e, "%s", "error injection");
		             return NULL);

		/* seal the branch */
		rc = sd_seal(r, &parent->file, blob, &merge.index, seal);
		if (ssunlikely(rc == -1))
			goto e0;
		if (index->scheme->sync == 2) {
			rc = ss_filesync(&parent->file);
			if (ssunlikely(rc == -1)) {
				sr_malfunction(r->e, "file '%s' sync error: %s",
				               parent->file.file, strerror(errno));
				goto e0;
			}
		}

		/* create new branch object */
		branch = si_branchnew(r);
		if (ssunlikely(branch == NULL))
			goto e0;
		si_branchset(branch, &merge.index);
	}
	sv_mergefree(&vmerge, r->a);

	if (ssunlikely(rc == -1)) {
		sr_oom_malfunction(r->e);
		goto e0;
	}
	assert(branch != NULL);

	/* in-memory mode support */
	if (blob) {
		rc = ss_blobfit(blob);
		if (ssunlikely(rc == -1)) {
			ss_blobfree(blob);
			goto e1;
		}
		branch->copy = copy;
	}
	/* mmap support */
	if (index->scheme->mmap) {
		ss_mmapinit(&parent->map_swap);
		rc = ss_mmap(&parent->map_swap, parent->file.fd,
		              parent->file.size, 1);
		if (ssunlikely(rc == -1)) {
			sr_malfunction(r->e, "db file '%s' mmap error: %s",
			               parent->file.file, strerror(errno));
			goto e1;
		}
	}
	return branch;
e0:
	sd_mergefree(&merge);
	if (blob)
		ss_blobfree(blob);
	return NULL;
e1:
	si_branchfree(branch, r);
	return NULL;
}

int si_branch(si *index, sdc *c, siplan *plan, uint64_t vlsn)
{
	sr *r = index->r;
	sinode *n = plan->node;
	assert(n->flags & SI_LOCK);

	si_lock(index);
	if (ssunlikely(n->used == 0)) {
		si_nodeunlock(n);
		si_unlock(index);
		return 0;
	}
	svindex *i;
	i = si_noderotate(n);
	si_unlock(index);

	sd_creset(c, r);
	sibranch *branch = si_branchcreate(index, c, n, i, vlsn);
	if (ssunlikely(branch == NULL))
		return -1;

	/* commit */
	si_lock(index);
	branch->next = n->branch;
	n->branch = branch;
	n->branch_count++;
	uint32_t used = sv_indexused(i);
	n->used -= used;
	ss_quota(r->quota, SS_QREMOVE, used);
	svindex swap = *i;
	si_nodeunrotate(n);
	si_nodeunlock(n);
	si_plannerupdate(&index->p, SI_BRANCH|SI_COMPACT, n);
	ssmmap swap_map = n->map;
	n->map = n->map_swap;
	memset(&n->map_swap, 0, sizeof(n->map_swap));
	si_unlock(index);

	/* gc */
	if (index->scheme->mmap) {
		int rc = ss_munmap(&swap_map);
		if (ssunlikely(rc == -1)) {
			sr_malfunction(r->e, "db file '%s' munmap error: %s",
			               n->file.file, strerror(errno));
			return -1;
		}
	}
	si_nodegc_index(r, &swap);
	return 1;
}

int si_compact(si *index, sdc *c, siplan *plan, uint64_t vlsn)
{
	sr *r = index->r;
	sinode *node = plan->node;
	assert(node->flags & SI_LOCK);

	sd_creset(c, r);
	/* prepare for compaction */
	int rc;
	rc = sd_censure(c, r, node->branch_count);
	if (ssunlikely(rc == -1))
		return sr_oom_malfunction(r->e);
	svmerge merge;
	sv_mergeinit(&merge);
	rc = sv_mergeprepare(&merge, r, node->branch_count);
	if (ssunlikely(rc == -1))
		return -1;

	/* read node file into memory */
	int use_mmap = index->scheme->mmap;
	ssmmap *map = &node->map;
	ssmmap  preload;
	if (index->scheme->node_compact_load) {
		rc = si_noderead(node, r, &c->c);
		if (ssunlikely(rc == -1))
			return -1;
		preload.p = c->c.s;
		preload.size = ss_bufused(&c->c);
		map = &preload;
		use_mmap = 1;
	}

	uint32_t size_stream = 0;
	sdcbuf *cbuf = c->head;
	sibranch *b = node->branch;
	while (b) {
		svmergesrc *s = sv_mergeadd(&merge, NULL);
		sdreadarg arg = {
			.index           = &b->index,
			.buf             = &cbuf->a,
			.buf_xf          = &cbuf->b,
			.buf_read        = &c->d,
			.index_iter      = &cbuf->index_iter,
			.page_iter       = &cbuf->page_iter,
			.use_compression = index->scheme->compression,
			.use_memory      = index->scheme->in_memory,
			.use_mmap        = use_mmap,
			.use_mmap_copy   = 0,
			.has             = 0,
			.has_vlsn        = 0,
			.o               = SS_GTE,
			.memory          = &b->copy,
			.mmap            = map,
			.file            = &node->file,
			.r               = r
		};
		ss_iterinit(sd_read, &s->src);
		int rc = ss_iteropen(sd_read, &s->src, &arg, NULL, 0);
		if (ssunlikely(rc == -1))
			return sr_oom_malfunction(r->e);
		size_stream += sd_indextotal(&b->index);
		cbuf = cbuf->next;
		b = b->next;
	}
	ssiter i;
	ss_iterinit(sv_mergeiter, &i);
	ss_iteropen(sv_mergeiter, &i, r, &merge, SS_GTE);
	rc = si_compaction(index, c, vlsn, node, &i, size_stream);
	sv_mergefree(&merge, r->a);
	return rc;
}
