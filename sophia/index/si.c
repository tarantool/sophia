
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

int si_init(si *i, sr *r)
{
	int rc = si_plannerinit(&i->p, r->a, i);
	if (ssunlikely(rc == -1))
		return -1;
	ss_bufinit(&i->readbuf);
	sv_upsertinit(&i->u);
	ss_rbinit(&i->i);
	ss_mutexinit(&i->lock);
	i->scheme       = NULL;
	i->update_time  = 0;
	i->lru_run_lsn  = 0;
	i->lru_v        = 0;
	i->lru_steps    = 1;
	i->lru_intr_lsn = 0;
	i->lru_intr_sum = 0;
	i->size         = 0;
	i->read_disk    = 0;
	i->read_cache   = 0;
	i->backup       = 0;
	i->snapshot_run = 0;
	i->snapshot     = 0;
	i->destroyed    = 0;
	i->r            = r;
	return 0;
}

int si_open(si *i, sischeme *scheme)
{
	i->scheme = scheme;
	return si_recover(i);
}

ss_rbtruncate(si_truncate,
              si_nodefree(sscast(n, sinode, node), (sr*)arg, 0))

int si_close(si *i)
{
	if (i->destroyed)
		return 0;
	int rcret = 0;
	if (i->i.root)
		si_truncate(i->i.root, i->r);
	i->i.root = NULL;
	sv_upsertfree(&i->u, i->r);
	ss_buffree(&i->readbuf, i->r->a);
	si_plannerfree(&i->p, i->r->a);
	ss_mutexfree(&i->lock);
	i->destroyed = 1;
	return rcret;
}

ss_rbget(si_match,
         sr_compare(scheme,
                    sd_indexpage_min(&(sscast(n, sinode, node))->self.index,
                                     sd_indexmin(&(sscast(n, sinode, node))->self.index)),
                    sd_indexmin(&(sscast(n, sinode, node))->self.index)->sizemin,
                                key, keysize))

int si_insert(si *i, sinode *n)
{
	sdindexpage *min = sd_indexmin(&n->self.index);
	ssrbnode *p = NULL;
	int rc = si_match(&i->i, i->r->scheme,
	                  sd_indexpage_min(&n->self.index, min),
	                  min->sizemin, &p);
	assert(! (rc == 0 && p));
	ss_rbset(&i->i, p, rc, &n->node);
	i->n++;
	return 0;
}

int si_remove(si *i, sinode *n)
{
	ss_rbremove(&i->i, &n->node);
	i->n--;
	return 0;
}

int si_replace(si *i, sinode *o, sinode *n)
{
	ss_rbreplace(&i->i, &o->node, &n->node);
	return 0;
}

int si_plan(si *i, siplan *plan)
{
	si_lock(i);
	int rc = si_planner(&i->p, plan);
	si_unlock(i);
	return rc;
}

int si_execute(si *i, sdc *c, siplan *plan,
               uint64_t vlsn,
               uint64_t vlsn_lru)
{
	int rc = -1;
	switch (plan->plan) {
	case SI_CHECKPOINT:
	case SI_BRANCH:
	case SI_AGE:
		rc = si_branch(i, c, plan, vlsn);
		break;
	case SI_LRU:
	case SI_GC:
	case SI_COMPACT:
		rc = si_compact(i, c, plan, vlsn, vlsn_lru, NULL, 0);
		break;
	case SI_COMPACT_INDEX:
		rc = si_compact_index(i, c, plan, vlsn, vlsn_lru);
		break;
	case SI_ANTICACHE:
		rc = si_anticache(i, plan);
		break;
	case SI_SNAPSHOT:
		rc = si_snapshot(i, plan);
		break;
	case SI_BACKUP:
	case SI_BACKUPEND:
		rc = si_backup(i, c, plan);
		break;
	case SI_SHUTDOWN:
		rc = si_close(i);
		break;
	case SI_DROP:
		rc = si_drop(i);
		break;
	}
	/* garbage collect buffers */
	sd_cgc(c, i->r, i->scheme->buf_gc_wm);
	return rc;
}
