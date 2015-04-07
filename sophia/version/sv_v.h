#ifndef SV_V_H_
#define SV_V_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct svv svv;

struct svv {
	uint64_t  lsn;
	uint32_t  valuesize;
	uint16_t  keysize;
	uint8_t   flags;
	void     *log;
	svv      *next;
	srrbnode node;
} srpacked;

extern svif sv_vif;

static inline char*
sv_vkey(svv *v) {
	return (char*)(v) + sizeof(svv);
}

static inline void*
sv_vvalue(svv *v) {
	return (char*)(v) + sizeof(svv) + v->keysize;
}

static inline svv*
sv_valloc(sra *a, sv *v)
{
	int keysize = sv_keysize(v);
	int valuesize = sv_valuesize(v);
	int size = sizeof(svv) + keysize + valuesize;
	svv *vv = sr_malloc(a, size);
	if (srunlikely(vv == NULL))
		return NULL;
	vv->keysize   = keysize; 
	vv->valuesize = valuesize;
	vv->flags     = sv_flags(v);
	vv->lsn       = sv_lsn(v);
	vv->next      = NULL;
	vv->log       = NULL;
	memset(&vv->node, 0, sizeof(vv->node));
	char *key = sv_vkey(vv);
	memcpy(key, sv_key(v), keysize);
	memcpy(key + keysize, sv_value(v), valuesize);
	return vv;
}

static inline void
sv_vfree(sra *a, svv *v)
{
	while (v) {
		svv *n = v->next;
		sr_free(a, v);
		v = n;
	}
}

static inline svv*
sv_visible(svv *v, uint64_t vlsn) {
	while (v && v->lsn > vlsn)
		v = v->next;
	return v;
}

static inline uint32_t
sv_vsize(svv *v) {
	return sizeof(svv) + v->keysize + v->valuesize;
}

static inline uint32_t
sv_vsizeof(sv *v) {
	return sizeof(svv) + sv_keysize(v) + sv_valuesize(v);
}

#endif
