#ifndef SI_PLANNER_H_
#define SI_PLANNER_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct siplanner siplanner;
typedef struct siplan siplan;

struct siplanner {
	void *i;
	ssrq branch;
	ssrq compact;
	ssrq temp;
};

/* plan */
#define SI_BRANCH        1
#define SI_AGE           2
#define SI_COMPACT       4
#define SI_COMPACT_INDEX 8
#define SI_CHECKPOINT    16
#define SI_GC            32
#define SI_TEMP          64
#define SI_BACKUP        128
#define SI_BACKUPEND     256
#define SI_SHUTDOWN      512
#define SI_DROP          1024

/* explain */
#define SI_ENONE         0
#define SI_ERETRY        1
#define SI_EINDEX_SIZE   2
#define SI_EINDEX_AGE    3
#define SI_EBRANCH_COUNT 4
#define SI_ETEMP         5

struct siplan {
	int explain;
	int plan;
	/* branch:
	 *   a: index_size
	 * age:
	 *   a: ttl
	 *   b: ttl_wm
	 *   c:
	 * compact:
	 *   a: branches
	 *   b: mode
	 *   c:
	 * compact_index:
	 *   a: index_size
	 * checkpoint:
	 *   a: lsn
	 *   b:
	 *   c:
	 * gc:
	 *   a: lsn
	 *   b: percent
	 *   c:
	 * temperature:
	 * backup:
	 *   a: bsn
	 *   b:
	 *   c:
	 * shutdown:
	 * drop:
	 */
	uint64_t a, b, c;
	sinode *node;
};

int si_planinit(siplan*);
int si_plannerinit(siplanner*, ssa*, void*);
int si_plannerfree(siplanner*, ssa*);
int si_plannertrace(siplan*, sstrace*);
int si_plannerupdate(siplanner*, int, sinode*);
int si_plannerremove(siplanner*, int, sinode*);
int si_planner(siplanner*, siplan*);

#endif
