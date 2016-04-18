
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
#include <libso.h>
#include <libsv.h>
#include <libsd.h>
#include <libsl.h>
#include <libsi.h>
#include <libsy.h>
#include <libsc.h>

int sc_write(sc *s, svlog *log, uint64_t lsn, int recover)
{
	/* write-ahead log */
	sltx tl;
	sl_begin(s->lp, &tl, lsn, recover);
	int rc = sl_write(&tl, log);
	if (ssunlikely(rc == -1)) {
		sl_rollback(&tl);
		return -1;
	}
	sl_commit(&tl);

	/* index */
	uint64_t now = ss_utime();
	svlogindex *i   = (svlogindex*)log->index.s;
	svlogindex *end = (svlogindex*)log->index.p;
	while (i < end) {
		si *index = i->ptr;
		sitx x;
		si_begin(&x, index);
		si_write(&x, log, i, now, recover);
		si_commit(&x);
		i++;
	}
	return 0;
}
