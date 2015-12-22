#ifndef SI_TRACK_H_
#define SI_TRACK_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct sitrack sitrack;

struct sitrack {
	ssrb i;
	int count;
	uint64_t nsn;
	uint64_t lsn;
};

static inline void
si_trackinit(sitrack *t) {
	ss_rbinit(&t->i);
	t->count = 0;
	t->nsn = 0;
	t->lsn = 0;
}

ss_rbtruncate(si_tracktruncate,
              si_nodefree(sscast(n, sinode, node), (sr*)arg, 0))

static inline void
si_trackfree(sitrack *t, sr *r) {
	if (t->i.root)
		si_tracktruncate(t->i.root, r);
}

static inline void
si_trackmetrics(sitrack *t, sinode *n)
{
	sibranch *b = n->branch;
	while (b) {
		sdindexheader *h = b->index.h;
		if (b->id.parent > t->nsn)
			t->nsn = b->id.parent;
		if (b->id.id > t->nsn)
			t->nsn = b->id.id;
		if (h->lsnmin != UINT64_MAX && h->lsnmin > t->lsn)
			t->lsn = h->lsnmin;
		if (h->lsnmax > t->lsn)
			t->lsn = h->lsnmax;
		b = b->next;
	}
}

static inline void
si_tracknsn(sitrack *t, uint64_t nsn)
{
	if (t->nsn < nsn)
		t->nsn = nsn;
}

ss_rbget(si_trackmatch, ss_cmp((sscast(n, sinode, node))->self.id.id, sscastu64(key)))

static inline void
si_trackset(sitrack *t, sinode *n)
{
	ssrbnode *p = NULL;
	int rc = si_trackmatch(&t->i, NULL, (char*)&n->self.id.id,
	                       sizeof(n->self.id.id), &p);
	assert(! (rc == 0 && p));
	ss_rbset(&t->i, p, rc, &n->node);
	t->count++;
}

static inline sinode*
si_trackget(sitrack *t, uint64_t id)
{
	ssrbnode *p = NULL;
	int rc = si_trackmatch(&t->i, NULL, (char*)&id, sizeof(id), &p);
	if (rc == 0 && p)
		return sscast(p, sinode, node);
	return NULL;
}

static inline void
si_trackreplace(sitrack *t, sinode *o, sinode *n)
{
	ss_rbreplace(&t->i, &o->node, &n->node);
}

static inline void
si_trackremove(sitrack *t, sinode *n)
{
	ss_rbremove(&t->i, &n->node);
	t->count--;
}

#endif
