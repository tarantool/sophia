
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
#include <libsl.h>
#include <libsd.h>
#include <libsi.h>
#include <libsx.h>
#include <libsy.h>
#include <libse.h>

static int
se_cursordestroy(so *o)
{
	secursor *c = se_cast(o, secursor*, SECURSOR);
	se *e = se_of(&c->o);
	sx_rollback(&c->t);
	uint32_t id = c->t.id;
	if (c->cache)
		si_cachepool_push(c->cache);
	so_listdel(&e->cursor, &c->o);
	se_dbunbind(e, id);
	sr_statcursor(&e->stat, c->start,
	              c->read_disk,
	              c->read_cache,
	              c->ops);
	se_mark_destroyed(&c->o);
	ss_free(&e->a_cursor, c);
	return 0;
}

static void*
se_cursorget(so *o, so *v)
{
	secursor *c = se_cast(o, secursor*, SECURSOR);
	sev *key = se_cast(v, sev*, SEV);
	sedb *db = se_cast(v->parent, sedb*, SEDB);
	ssorder order = key->order;
	if (ssunlikely(! key->orderset))
		order = SS_GTE;
	/* this statistics might be not complete, because
	 * last statement is not accounted here */
	c->read_disk  += key->read_disk;
	c->read_cache += key->read_cache;
	c->ops++;
	return se_dbread(db, key, &c->t, 0, c->cache, order);
}

static soif secursorif =
{
	.open         = NULL,
	.destroy      = se_cursordestroy,
	.error        = NULL,
	.object       = NULL,
	.poll         = NULL,
	.drop         = NULL,
	.setobject    = NULL,
	.setstring    = NULL,
	.setint       = NULL,
	.getobject    = NULL,
	.getstring    = NULL,
	.getint       = NULL,
	.set          = NULL,
	.update       = NULL,
	.del          = NULL,
	.get          = se_cursorget,
	.batch        = NULL,
	.begin        = NULL,
	.prepare      = NULL,
	.commit       = NULL,
	.cursor       = NULL,
};

so *se_cursornew(se *e, uint64_t vlsn)
{
	secursor *c = ss_malloc(&e->a_cursor, sizeof(secursor));
	if (ssunlikely(c == NULL)) {
		sr_oom(&e->error);
		return NULL;
	}
	so_init(&c->o, &se_o[SECURSOR], &secursorif, &e->o, &e->o);
	sx_init(&e->xm, &c->t);
	c->start = ss_utime();
	c->ops = 0;
	c->read_disk = 0;
	c->read_cache = 0;
	c->t.state = SXUNDEF;
	c->cache = si_cachepool_pop(&e->cachepool);
	if (ssunlikely(c->cache == NULL)) {
		ss_free(&e->a_cursor, c);
		return NULL;
	}
	sx_begin(&e->xm, &c->t, SXRO, vlsn);
	se_dbbind(e);
	so_listadd(&e->cursor, &c->o);
	return &c->o;
}
