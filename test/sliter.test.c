
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <libsr.h>
#include <libsv.h>
#include <libsl.h>
#include <libsd.h>
#include <libst.h>
#include <sophia.h>

static void
alloclogv(svlog *log, sra *a, uint64_t lsn, uint8_t flags, int key)
{
	svlocal l;
	l.lsn         = lsn;
	l.flags       = flags;
	l.key         = &key;
	l.keysize     = sizeof(int);
	l.value       = NULL;
	l.valuesize   = 0;
	sv lv;
	sv_init(&lv, &sv_localif, &l, NULL);
	svv *v = sv_valloc(a, &lv);
	svlogv logv;
	logv.id = 0;
	logv.next = 0;
	sv_init(&logv.v, &sv_vif, v, NULL);
	sv_logadd(log, a, &logv, NULL);
}

static void
freelog(svlog *log, sr *c)
{
	sriter i;
	sr_iterinit(sr_bufiter, &i, c);
	sr_iteropen(sr_bufiter, &i, &log->buf, sizeof(svlogv));
	for (; sr_iteratorhas(&i); sr_iteratornext(&i)) {
		svlogv *v = sr_iteratorof(&i);
		sr_free(c->a, v->v.v);
	}
	sv_logfree(log, c->a);
}

static void
sliter_tx(stc *cx)
{
	sra a;
	sr_aopen(&a, &sr_stda);
	srcomparator cmp = { sr_cmpu32, NULL };
	srseq seq;
	sr_seqinit(&seq);
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, &seq, &cmp, NULL, crc, NULL);
	slconf conf = {
		.path     = cx->suite->logdir,
		.enable   = 1,
		.rotatewm = 1000
	};
	slpool lp;
	t( sl_poolinit(&lp, &r) == 0 );
	t( sl_poolopen(&lp, &conf) == 0 );
	t( sl_poolrotate(&lp) == 0 );

	svlog log;
	sv_loginit(&log);

	alloclogv(&log, &a, 0, SVSET, 7);

	sltx ltx;
	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );

	freelog(&log, &r);
	t( sl_poolshutdown(&lp) == 0 );
}

static void
sliter_tx_read_empty(stc *cx)
{
	sra a;
	sr_aopen(&a, &sr_stda);
	srcomparator cmp = { sr_cmpu32, NULL };
	srseq seq;
	sr_seqinit(&seq);
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, &seq, &cmp, NULL, crc, NULL);
	slconf conf = {
		.path     = cx->suite->logdir,
		.enable   = 1,
		.rotatewm = 1000
	};
	slpool lp;
	t( sl_poolinit(&lp, &r) == 0 );
	t( sl_poolopen(&lp, &conf) == 0 );
	t( sl_poolrotate(&lp) == 0 );
	svlog log;
	sv_loginit(&log);
	freelog(&log, &r);

	sl *current = srcast(lp.list.prev, sl, link);
	sriter li;
	sr_iterinit(sl_iter, &li, &r);
	t( sr_iteropen(sl_iter, &li, &current->file, 1) == 0 );
	for (;;) {
		// begin
		while (sr_iteratorhas(&li)) {
			sv *v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 7 );
			sr_iteratornext(&li);
		}
		t( sl_iter_error(&li) == 0 );
		// commit
		if (! sl_iter_continue(&li) )
			break;
	}
	sr_iteratorclose(&li);

	t( sl_poolshutdown(&lp) == 0 );
}

static void
sliter_tx_read0(stc *cx)
{
	sra a;
	sr_aopen(&a, &sr_stda);
	srcomparator cmp = { sr_cmpu32, NULL };
	srseq seq;
	sr_seqinit(&seq);
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, &seq, &cmp, NULL, crc, NULL);
	slconf conf = {
		.path     = cx->suite->logdir,
		.enable   = 1,
		.rotatewm = 1000
	};
	slpool lp;
	t( sl_poolinit(&lp, &r) == 0 );
	t( sl_poolopen(&lp, &conf) == 0 );
	t( sl_poolrotate(&lp) == 0 );
	svlog log;
	sv_loginit(&log);
	alloclogv(&log, &a, 0, SVSET, 7);
	sltx ltx;
	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );
	freelog(&log, &r);

	sl *current = srcast(lp.list.prev, sl, link);
	sriter li;
	sr_iterinit(sl_iter, &li, &r);
	t( sr_iteropen(sl_iter, &li, &current->file, 1) == 0 );
	for (;;) {
		// begin
		while (sr_iteratorhas(&li)) {
			sv *v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 7 );
			sr_iteratornext(&li);
		}
		t( sl_iter_error(&li) == 0 );
		// commit
		if (! sl_iter_continue(&li) )
			break;
	}
	sr_iteratorclose(&li);

	t( sl_poolshutdown(&lp) == 0 );
}

static void
sliter_tx_read1(stc *cx)
{
	sra a;
	sr_aopen(&a, &sr_stda);
	srcomparator cmp = { sr_cmpu32, NULL };
	srseq seq;
	sr_seqinit(&seq);
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, &seq, &cmp, NULL, crc, NULL);
	slconf conf = {
		.path     = cx->suite->logdir,
		.enable   = 1,
		.rotatewm = 1000
	};
	slpool lp;
	t( sl_poolinit(&lp, &r) == 0 );
	t( sl_poolopen(&lp, &conf) == 0 );
	t( sl_poolrotate(&lp) == 0 );
	svlog log;
	sv_loginit(&log);
	alloclogv(&log, &a, 0, SVSET, 7);
	alloclogv(&log, &a, 0, SVSET, 8);
	alloclogv(&log, &a, 0, SVSET, 9);
	sltx ltx;
	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );
	freelog(&log, &r);

	sl *current = srcast(lp.list.prev, sl, link);
	sriter li;
	sr_iterinit(sl_iter, &li, &r);
	t( sr_iteropen(sl_iter, &li, &current->file, 1) == 0 );
	for (;;) {
		// begin
		t( sr_iteratorhas(&li) == 1 );
		sv *v = sr_iteratorof(&li);
		t( *(int*)sv_key(v) == 7 );
		sr_iteratornext(&li);
		t( sr_iteratorhas(&li) == 1 );
		v = sr_iteratorof(&li);
		t( *(int*)sv_key(v) == 8 );
		sr_iteratornext(&li);
		t( sr_iteratorhas(&li) == 1 );
		v = sr_iteratorof(&li);
		t( *(int*)sv_key(v) == 9 );
		sr_iteratornext(&li);
		t( sr_iteratorhas(&li) == 0 );

		t( sl_iter_error(&li) == 0 );
		// commit
		if (! sl_iter_continue(&li) )
			break;
	}
	sr_iteratorclose(&li);

	t( sl_poolshutdown(&lp) == 0 );
}

static void
sliter_tx_read2(stc *cx)
{
	sra a;
	sr_aopen(&a, &sr_stda);
	srcomparator cmp = { sr_cmpu32, NULL };
	srseq seq;
	sr_seqinit(&seq);
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, &seq, &cmp, NULL, crc, NULL);
	slconf conf = {
		.path     = cx->suite->logdir,
		.enable   = 1,
		.rotatewm = 1000
	};
	slpool lp;
	t( sl_poolinit(&lp, &r) == 0 );
	t( sl_poolopen(&lp, &conf) == 0 );
	t( sl_poolrotate(&lp) == 0 );
	svlog log;
	sv_loginit(&log);
	alloclogv(&log, &a, 0, SVSET, 7);
	alloclogv(&log, &a, 0, SVSET, 8);
	alloclogv(&log, &a, 0, SVSET, 9);
	sltx ltx;
	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );

	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );
	freelog(&log, &r);

	sl *current = srcast(lp.list.prev, sl, link);
	sriter li;
	sr_iterinit(sl_iter, &li, &r);
	t( sr_iteropen(sl_iter, &li, &current->file, 1) == 0 );
	for (;;) {
		// begin
		t( sr_iteratorhas(&li) == 1 );
		sv *v = sr_iteratorof(&li);
		t( *(int*)sv_key(v) == 7 );
		sr_iteratornext(&li);
		t( sr_iteratorhas(&li) == 1 );
		v = sr_iteratorof(&li);
		t( *(int*)sv_key(v) == 8 );
		sr_iteratornext(&li);
		t( sr_iteratorhas(&li) == 1 );
		v = sr_iteratorof(&li);
		t( *(int*)sv_key(v) == 9 );
		sr_iteratornext(&li);

		t( sr_iteratorhas(&li) == 0 );
		t( sl_iter_error(&li) == 0 );
		// commit
		if (! sl_iter_continue(&li) )
			break;
	}
	sr_iteratorclose(&li);

	t( sl_poolshutdown(&lp) == 0 );
}

static void
sliter_tx_read3(stc *cx)
{
	sra a;
	sr_aopen(&a, &sr_stda);
	srcomparator cmp = { sr_cmpu32, NULL };
	srseq seq;
	sr_seqinit(&seq);
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, &seq, &cmp, NULL, crc, NULL);
	slconf conf = {
		.path     = cx->suite->logdir,
		.enable   = 1,
		.rotatewm = 1000
	};
	slpool lp;
	t( sl_poolinit(&lp, &r) == 0 );
	t( sl_poolopen(&lp, &conf) == 0 );
	t( sl_poolrotate(&lp) == 0 );
	svlog log;

	sv_loginit(&log);
	alloclogv(&log, &a, 0, SVSET, 7); /* single stmt */
	sltx ltx;
	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );
	freelog(&log, &r);

	sv_loginit(&log);
	alloclogv(&log, &a, 0, SVSET, 8); /* multi stmt */
	alloclogv(&log, &a, 0, SVSET, 9);
	alloclogv(&log, &a, 0, SVSET, 10);
	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );
	freelog(&log, &r);

	sv_loginit(&log);
	alloclogv(&log, &a, 0, SVSET, 11); /* multi stmt */
	alloclogv(&log, &a, 0, SVSET, 12);
	alloclogv(&log, &a, 0, SVSET, 13);
	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );
	freelog(&log, &r);

	sv_loginit(&log);
	alloclogv(&log, &a, 0, SVSET, 14); /* single stmt */
	t( sl_begin(&lp, &ltx) == 0 );
	t( sl_write(&ltx, &log) == 0 );
	t( sl_commit(&ltx) == 0 );
	freelog(&log, &r);

	int state = 0;

	sl *current = srcast(lp.list.prev, sl, link);
	sriter li;
	sr_iterinit(sl_iter, &li, &r);
	t( sr_iteropen(sl_iter, &li, &current->file, 1) == 0 );
	for (;;) {
		sv *v;
		// begin
		switch (state) {
		case 0:
			t( sr_iteratorhas(&li) == 1 );
			v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 7 );
			sr_iteratornext(&li);
			v = sr_iteratorof(&li);
			t( v == NULL );
			break;
		case 1:
			t( sr_iteratorhas(&li) == 1 );
			v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 8 );
			sr_iteratornext(&li);
			v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 9 );
			sr_iteratornext(&li);
			v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 10 );
			sr_iteratornext(&li);
			v = sr_iteratorof(&li);
			t( v == NULL );
			break;
		case 2:
			t( sr_iteratorhas(&li) == 1 );
			v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 11 );
			sr_iteratornext(&li);
			v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 12 );
			sr_iteratornext(&li);
			v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 13 );
			sr_iteratornext(&li);
			v = sr_iteratorof(&li);
			t( v == NULL );
			break;
		case 3:
			t( sr_iteratorhas(&li) == 1 );
			v = sr_iteratorof(&li);
			t( *(int*)sv_key(v) == 14 );
			sr_iteratornext(&li);
			v = sr_iteratorof(&li);
			t( v == NULL );
			break;
		}
		// commit
		if (! sl_iter_continue(&li) )
			break;
		state++;
	}
	sr_iteratorclose(&li);
	t( state == 3);

	t( sl_poolshutdown(&lp) == 0 );
}

stgroup *sliter_group(void)
{
	stgroup *group = st_group("sliter");
	st_groupadd(group, st_test("tx", sliter_tx));
	st_groupadd(group, st_test("tx_read_empty", sliter_tx_read_empty));
	st_groupadd(group, st_test("tx_read0", sliter_tx_read0));
	st_groupadd(group, st_test("tx_read1", sliter_tx_read1));
	st_groupadd(group, st_test("tx_read2", sliter_tx_read2));
	st_groupadd(group, st_test("tx_read3", sliter_tx_read3));
	return group;
}
