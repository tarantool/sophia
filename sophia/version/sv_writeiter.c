
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <libsr.h>
#include <libsv.h>

typedef struct svwriteiter svwriteiter;

struct svwriteiter {
	sriter *merge;
	uint64_t limit; 
	uint64_t size;
	uint32_t sizev;
	uint64_t vlsn;
	int save_delete;
	int next;
	uint64_t prevlsn;
	sv *v;
} srpacked;

static void
sv_writeiter_init(sriter *i)
{
	assert(sizeof(svwriteiter) <= sizeof(i->priv));
	svwriteiter *im = (svwriteiter*)i->priv;
	memset(im, 0, sizeof(*im));
}

static void sv_writeiter_next(sriter*);

static int
sv_writeiter_open(sriter *i, va_list args)
{
	svwriteiter *im = (svwriteiter*)i->priv;
	im->merge = va_arg(args, sriter*);
	im->limit = va_arg(args, uint64_t);
	im->sizev = va_arg(args, uint32_t);
	im->vlsn  = va_arg(args, uint64_t);
	im->save_delete = va_arg(args, int);
	assert(im->merge->i == &sv_mergeiter);
	sv_writeiter_next(i);
	return 0;
}

static void
sv_writeiter_close(sriter *i srunused)
{ }

static int
sv_writeiter_has(sriter *i)
{
	svwriteiter *im = (svwriteiter*)i->priv;
	return im->v != NULL;
}

static void*
sv_writeiter_of(sriter *i)
{
	svwriteiter *im = (svwriteiter*)i->priv;
	if (srunlikely(im->v == NULL))
		return NULL;
	return im->v;
}

static void
sv_writeiter_next(sriter *i)
{
	svwriteiter *im = (svwriteiter*)i->priv;
	if (im->next)
		sr_iternext(im->merge);
	im->next = 0;
	im->v = NULL;
	for (; sr_iterhas(im->merge); sr_iternext(im->merge))
	{
		sv *v = sr_iterof(im->merge);
		int dup = (svflags(v) & SVDUP) | sv_mergeisdup(im->merge);
		if (im->size >= im->limit) {
			if (! dup)
				break;
		}
		uint64_t lsn = svlsn(v);
		int kv = svkeysize(v) + svvaluesize(v);
		if (srunlikely(dup)) {
			/* keep atleast one visible version for <= vlsn */
			if (im->prevlsn <= im->vlsn)
				continue;
		} else {
			/* branched or stray deletes */
			if (! im->save_delete) {
				int del = (svflags(v) & SVDELETE) > 0;
				if (srunlikely(del && (lsn <= im->vlsn))) {
					im->prevlsn = lsn;
					continue;
				}
			}
			im->size += im->sizev + kv;
		}
		im->prevlsn = lsn;
		im->v = v;
		im->next = 1;
		break;
	}
}

sriterif sv_writeiter =
{
	.init    = sv_writeiter_init,
	.open    = sv_writeiter_open,
	.close   = sv_writeiter_close,
	.has     = sv_writeiter_has,
	.of      = sv_writeiter_of,
	.next    = sv_writeiter_next
};

int sv_writeiter_resume(sriter *i)
{
	svwriteiter *im = (svwriteiter*)i->priv;
	im->v    = sr_iterof(im->merge);
	if (srunlikely(im->v == NULL))
		return 0;
	im->next = 1;
	im->size = im->sizev + svkeysize(im->v) +
	           svvaluesize(im->v);
	return 1;
}
