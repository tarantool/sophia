#ifndef SR_SEQ_H_
#define SR_SEQ_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef enum {
	SR_DSN,
	SR_DSNNEXT,
	SR_NSN,
	SR_NSNNEXT,
	SR_ASN,
	SR_ASNNEXT,
	SR_SSN,
	SR_SSNNEXT,
	SR_BSN,
	SR_BSNNEXT,
	SR_LSN,
	SR_LSNNEXT,
	SR_LFSN,
	SR_LFSNNEXT,
	SR_TSN,
	SR_TSNNEXT,
	SR_RSN,
	SR_RSNNEXT
} srseqop;

typedef struct {
	ssspinlock lock;
	uint64_t lsn;
	uint64_t tsn;
	uint64_t nsn;
	uint64_t ssn;
	uint64_t asn;
	uint64_t rsn;
	uint64_t lfsn;
	uint32_t dsn;
	uint32_t bsn;
} srseq;

static inline void
sr_seqinit(srseq *n) {
	memset(n, 0, sizeof(*n));
	ss_spinlockinit(&n->lock);
}

static inline void
sr_seqfree(srseq *n) {
	ss_spinlockfree(&n->lock);
}

static inline void
sr_seqlock(srseq *n) {
	ss_spinlock(&n->lock);
}

static inline void
sr_sequnlock(srseq *n) {
	ss_spinunlock(&n->lock);
}

static inline uint64_t
sr_seqdo(srseq *n, srseqop op)
{
	uint64_t v = 0;
	switch (op) {
	case SR_LSN:       v = n->lsn;
		break;
	case SR_LSNNEXT:   v = ++n->lsn;
		break;
	case SR_TSN:       v = n->tsn;
		break;
	case SR_TSNNEXT:   v = ++n->tsn;
		break;
	case SR_RSN:       v = n->rsn;
		break;
	case SR_RSNNEXT:   v = ++n->rsn;
		break;
	case SR_NSN:       v = n->nsn;
		break;
	case SR_NSNNEXT:   v = ++n->nsn;
		break;
	case SR_LFSN:      v = n->lfsn;
		break;
	case SR_LFSNNEXT:  v = ++n->lfsn;
		break;
	case SR_SSN:       v = n->ssn;
		break;
	case SR_SSNNEXT:   v = ++n->ssn;
		break;
	case SR_ASN:       v = n->asn;
		break;
	case SR_ASNNEXT:   v = ++n->asn;
		break;
	case SR_BSN:       v = n->bsn;
		break;
	case SR_BSNNEXT:   v = ++n->bsn;
		break;
	case SR_DSN:       v = n->dsn;
		break;
	case SR_DSNNEXT:   v = ++n->dsn;
		break;
	}
	return v;
}

static inline uint64_t
sr_seq(srseq *n, srseqop op)
{
	sr_seqlock(n);
	uint64_t v = sr_seqdo(n, op);
	sr_sequnlock(n);
	return v;
}

#endif
