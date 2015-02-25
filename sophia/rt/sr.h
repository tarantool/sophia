#ifndef SR_H_
#define SR_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct sr sr;

struct sr {
	srerror *e;
	srcomparator *cmp;
	srseq *seq;
	sra *a;
	srinjection *i;
	srcrcf crc;
};

static inline void
sr_init(sr *r,
        srerror *e,
        sra *a,
        srseq *seq,
        srcomparator *cmp,
        srinjection *i,
        srcrcf crc)
{
	r->e   = e;
	r->a   = a;
	r->seq = seq;
	r->cmp = cmp;
	r->i   = i;
	r->crc = crc;
}

#endif
