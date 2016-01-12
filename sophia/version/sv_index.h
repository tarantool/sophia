#ifndef SC_INDEX_H_
#define SC_INDEX_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct svindexpos svindexpos;
typedef struct svindex svindex;

struct svindexpos {
	ssrbnode *node;
	int rc;
};

struct svindex {
	ssrb i;
	uint32_t count;
	uint32_t used;
	uint64_t lsnmin;
} sspacked;

ss_rbget(sv_indexmatch,
         sr_compare(scheme, sv_vpointer((sscast(n, svref, node))->v),
                    (sscast(n, svref, node))->v->size,
                    key, keysize))

int sv_indexinit(svindex*);
int sv_indexfree(svindex*, sr*);
int sv_indexupdate(svindex*, svindexpos*, svref*);
svref *sv_indexget(svindex*, sr*, svindexpos*, svref*);

static inline int
sv_indexset(svindex *i, sr *r, svref  *v)
{
	svindexpos pos;
	sv_indexget(i, r, &pos, v);
	sv_indexupdate(i, &pos, v);
	return 0;
}

static inline uint32_t
sv_indexused(svindex *i) {
	return i->count * sizeof(svv) + i->used;
}

#endif
