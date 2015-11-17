#ifndef SE_DB_H_
#define SE_DB_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct sedb sedb;

struct sedb {
	so         o;
	sestatus   status;
	uint32_t   created;
	uint32_t   scheduled;
	uint32_t   dropped;
	siprofiler rtp;
	sdc        dc;
	sischeme   scheme;
	si         index;
	sxindex    coindex;
	ssspinlock reflock;
	uint32_t   ref;
	uint32_t   ref_be;
	uint64_t   txn_min;
	uint64_t   txn_max;
	sr         r;
};

static inline int
se_dbactive(sedb *o) {
	return se_statusactive(&o->status);
}

so       *se_dbnew(se*, char*);
so       *se_dbmatch(se*, char*);
so       *se_dbmatch_id(se*, uint32_t);
void     *se_dbread(sedb*, sedocument*, sx*, int, sicache*, ssorder);
void      se_dbref(sedb*, int);
uint32_t  se_dbunref(sedb*, int);
uint32_t  se_dbrefof(sedb*, int);
int       se_dbgarbage(sedb*);
int       se_dbvisible(sedb*, uint64_t);
void      se_dbbind(se*);
void      se_dbunbind(se*, uint64_t);
int       se_dbmalfunction(sedb*);
int       se_dbv(sedb*, sedocument*, int, svv**);
int       se_dbvprefix(sedb*, sedocument*, svv**);

#endif
