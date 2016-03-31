#ifndef SL_V_H_
#define SL_V_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct slv slv;

struct slv {
	uint32_t crc;
	uint64_t lsn;
	uint32_t dsn;
	uint32_t size;
	uint32_t timestamp;
	uint8_t  flags;
} sspacked;

extern svif sl_vif;

static inline uint32_t
sl_vdsn(sv *v) {
	return ((slv*)v->v)->dsn;
}

static inline uint32_t
sl_vtimestamp(sv *v) {
	return ((slv*)v->v)->timestamp;
}

#endif
