
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
#include <libsc.h>
#include <libse.h>

static void*
se_worker(void *arg)
{
	ssthread *self = arg;
	se *e = self->arg;
	scworker *w = sc_workerpool_pop(&e->scheduler.wp, &e->r);
	if (ssunlikely(w == NULL))
		return NULL;
	for (;;)
	{
		int rc = se_active(e);
		if (ssunlikely(rc == 0))
			break;
		rc = sc_step(&e->scheduler, w, sx_vlsn(&e->xm));
		if (ssunlikely(rc == -1))
			break;
		if (ssunlikely(rc == 0))
			ss_sleep(10000000); /* 10ms */
	}
	sc_workerpool_push(&e->scheduler.wp, w);
	return NULL;
}

int se_service_threads(se *e, int n)
{
	/* run more threads */
	return sc_create(&e->scheduler, se_worker, e, n);
}

static int
se_open(so *o)
{
	se *e = se_cast(o, se*, SE);
	/* recover phases */
	int status = sr_status(&e->status);
	switch (e->conf.recover) {
	case SE_RECOVER_1P: break;
	case SE_RECOVER_2P:
		if (status == SR_RECOVER)
			goto online;
		break;
	case SE_RECOVER_NP:
		if (status == SR_RECOVER) {
			sr_statusset(&e->status, SR_ONLINE);
			return 0;
		}
		if (status == SR_ONLINE) {
			sr_statusset(&e->status, SR_RECOVER);
			return 0;
		}
		break;
	}
	if (status != SR_OFFLINE)
		return -1;

	/* validate configuration */
	int rc;
	rc = se_confvalidate(&e->conf);
	if (ssunlikely(rc == -1))
		return -1;
	sr_statusset(&e->status, SR_RECOVER);

	/* set memory quota (disable during recovery) */
	ss_quotaset(&e->quota, e->conf.memory_limit);
	ss_quotaenable(&e->quota, 0);

	/* repository recover */
	rc = se_recover_repository(e);
	if (ssunlikely(rc == -1))
		return -1;
	/* databases recover */
	sslist *i, *n;
	ss_listforeach_safe(&e->db.list, i, n) {
		so *o = sscast(i, so, link);
		rc = so_open(o);
		if (ssunlikely(rc == -1))
			return -1;
	}
	/* recover logpool */
	rc = se_recover(e);
	if (ssunlikely(rc == -1))
		return -1;
	if (e->conf.recover == SE_RECOVER_2P)
		return 0;

online:
	/* complete */
	ss_listforeach_safe(&e->db.list, i, n) {
		so *o = sscast(i, so, link);
		rc = so_open(o);
		if (ssunlikely(rc == -1))
			return -1;
	}
	/* enable quota */
	ss_quotaenable(&e->quota, 1);
	sr_statusset(&e->status, SR_ONLINE);

	/* run thread-pool and scheduler */
	sc_set(&e->scheduler, e->conf.anticache,
	        e->conf.backup_path);
	rc = se_service_threads(e, e->conf.threads);
	if (ssunlikely(rc == -1))
		return -1;
	return 0;
}

static int
se_destroy(so *o, int fe ssunused)
{
	se *e = se_cast(o, se*, SE);
	int rcret = 0;
	int rc;
	sr_statusset(&e->status, SR_SHUTDOWN);
	rc = sc_shutdown(&e->scheduler);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = so_listdestroy(&e->cursor, 1);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = so_listdestroy(&e->view, 1);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = so_listdestroy(&e->viewdb, 1);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = so_listdestroy(&e->tx, 1);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = so_listdestroy(&e->confcursor, 1);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = so_listdestroy(&e->db, 1);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = sl_poolshutdown(&e->lp);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = sy_close(&e->rep, &e->r);
	if (ssunlikely(rc == -1))
		rcret = -1;
	sx_managerfree(&e->xm);
	ss_vfsfree(&e->vfs);
	si_cachepool_free(&e->cachepool, &e->r);
	se_conffree(&e->conf);
	ss_quotafree(&e->quota);
	ss_mutexfree(&e->apilock);
	sr_statfree(&e->stat);
	sr_seqfree(&e->seq);
	ss_pagerfree(&e->pager);
	sr_statusfree(&e->status);
	se_mark_destroyed(&e->o);
	free(e);
	return rcret;
}

static int
se_close(so *o)
{
	return se_destroy(o, 1);
}

static void*
se_begin(so *o)
{
	se *e = se_of(o);
	return se_txnew(e);
}

static void*
se_poll(so *o)
{
	se *e = se_cast(o, se*, SE);
	so *result;
	if (e->conf.event_on_backup) {
		int event = sc_ctl_backup_event(&e->scheduler);
		if (event) {
			sedocument *doc;
			result = se_document_new(e, &e->o, NULL, 1);
			if (ssunlikely(result == NULL))
				return NULL;
			doc = (sedocument*)result;
			doc->async_operation = 1;
			return result;
		}
	}
	scread *r = (scread*)sc_readpool_popready(&e->scheduler.rp);
	if (r == NULL)
		return NULL;
	result = se_dbresult(e, r, 1);
	so_destroy(&r->o, 1);
	return result;
}

static int
se_error(so *o)
{
	se *e = se_cast(o, se*, SE);
	int status = sr_errorof(&e->error);
	if (status == SR_ERROR_MALFUNCTION)
		return 1;
	status = sr_status(&e->status);
	if (status == SR_MALFUNCTION)
		return 1;
	return 0;
}

static void*
se_cursor(so *o)
{
	se *e = se_cast(o, se*, SE);
	return se_cursornew(e, UINT64_MAX);
}

static soif seif =
{
	.open         = se_open,
	.close        = se_close,
	.destroy      = se_destroy,
	.error        = se_error,
	.document     = NULL,
	.poll         = se_poll,
	.drop         = NULL,
	.setstring    = se_confset_string,
	.setint       = se_confset_int,
	.getobject    = se_confget_object,
	.getstring    = se_confget_string,
	.getint       = se_confget_int,
	.set          = NULL,
	.upsert       = NULL,
	.del          = NULL,
	.get          = NULL,
	.begin        = se_begin,
	.prepare      = NULL,
	.commit       = NULL,
	.cursor       = se_cursor,
};

int se_service(so *o)
{
	se *e = se_cast(o, se*, SE);
	return sc_ctl_call(&e->scheduler, sx_vlsn(&e->xm));
}

so *se_new(void)
{
	se *e = malloc(sizeof(*e));
	if (ssunlikely(e == NULL))
		return NULL;
	memset(e, 0, sizeof(*e));
	so_init(&e->o, &se_o[SE], &seif, &e->o, &e->o /* self */);
	sr_statusinit(&e->status);
	sr_statusset(&e->status, SR_OFFLINE);
	ss_vfsinit(&e->vfs, &ss_stdvfs);
	ss_pagerinit(&e->pager, &e->vfs, 10, 8192);
	ss_aopen(&e->a, &ss_stda);
	ss_aopen(&e->a_ref, &ss_stda);
	ss_aopen(&e->a_document, &ss_slaba, &e->pager, sizeof(sedocument));
	ss_aopen(&e->a_cursor, &ss_slaba, &e->pager, sizeof(secursor));
	ss_aopen(&e->a_view, &ss_slaba, &e->pager, sizeof(seview));
	ss_aopen(&e->a_viewdb, &ss_slaba, &e->pager, sizeof(seviewdb));
	ss_aopen(&e->a_cachebranch, &ss_slaba, &e->pager, sizeof(sicachebranch));
	ss_aopen(&e->a_cache, &ss_slaba, &e->pager, sizeof(sicache));
	ss_aopen(&e->a_confcursor, &ss_slaba, &e->pager, sizeof(seconfcursor));
	ss_aopen(&e->a_confkv, &ss_slaba, &e->pager, sizeof(seconfkv));
	ss_aopen(&e->a_tx, &ss_slaba, &e->pager, sizeof(setx));
	ss_aopen(&e->a_sxv, &ss_slaba, &e->pager, sizeof(sxv));
	int rc;
	rc = se_confinit(&e->conf, &e->o);
	if (ssunlikely(rc == -1))
		goto error;
	so_listinit(&e->db);
	so_listinit(&e->cursor);
	so_listinit(&e->viewdb);
	so_listinit(&e->view);
	so_listinit(&e->tx);
	so_listinit(&e->confcursor);
	ss_mutexinit(&e->apilock);
	ss_quotainit(&e->quota);
	sr_seqinit(&e->seq);
	sr_errorinit(&e->error);
	sr_statinit(&e->stat);
	sscrcf crc = ss_crc32c_function();
	sr_init(&e->r, &e->status, &e->error, &e->a, &e->a_ref, &e->vfs, &e->quota,
	        &e->conf.zones, &e->seq, SF_KV, SF_SRAW, NULL,
	        &e->conf.scheme, &e->ei, &e->stat, crc);
	sy_init(&e->rep);
	sl_poolinit(&e->lp, &e->r);
	sx_managerinit(&e->xm, &e->r, &e->a_sxv);
	si_cachepool_init(&e->cachepool, &e->a_cache, &e->a_cachebranch);
	sc_init(&e->scheduler, &e->r, &e->conf.on_event, &e->lp);
	return &e->o;
error:
	sr_statusfree(&e->status);
	ss_pagerfree(&e->pager);
	free(e);
	return NULL;
}
