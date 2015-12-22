
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <sophia.h>
#include <libss.h>
#include <libsf.h>
#include <libsr.h>
#include <libsv.h>
#include <libsd.h>
#include <libst.h>

static void
upsert_no_operator(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	int i = 0;
	while ( i < 100 ) {
		void *o = sp_document(db);
		t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
		t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
		t( sp_upsert(db, o) == 0 );
		i++;
	}

	i = 0;
	void *o = sp_document(db);
	t( o != NULL );
	void *cur = sp_cursor(env);
	while ((o = sp_get(cur, o))) {
		t( *(int*)sp_getstring(o, "key", NULL) == i );
		t( *(int*)sp_getstring(o, "value", NULL) == i );
		i++;
	}
	sp_destroy(cur);

	t( sp_destroy(env) == 0 );
}

static int upsert_ops = 0;

static int
upsert_operator_orphan(char **result,
                       char **key, int *key_size, int key_count,
                       char *src, int src_size,
                       char *upsert, int upsert_size,
                       void *arg)
{
	(void)key;
	(void)key_size;
	(void)key_count;
	assert(src == NULL);
	assert(src_size == 0);
	assert(upsert != NULL);
	(void)arg;
	char *c = malloc(upsert_size);
	memcpy(c, upsert, upsert_size);
	*result = c;
	upsert_ops++;
	return upsert_size;
}

static int
upsert_operator(char **result,
                char **key, int *key_size, int key_count,
                char *src, int src_size,
                char *upsert, int upsert_size,
                void *arg)
{
	(void)key;
	(void)key_size;
	(void)key_count;
	assert(src != NULL);
	assert(upsert != NULL);
	(void)arg;
	char *c = malloc(upsert_size);
	memcpy(c, upsert, upsert_size);
	*result = c;
	upsert_ops++;
	return upsert_size;
}

static void
upsert_upsert_get_index(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_orphan, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int up = 777;
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_upsert_get_branch0(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_orphan, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int up = 777;
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );
	t( upsert_ops == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_upsert_get_compact(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_orphan, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int up = 777;
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );
	t( sp_setint(env, "db.test.compact", 0) == 0 );

	t( upsert_ops == 1 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_get_index(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_get_branch0(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_get_branch1(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_get_compact(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );
	t( sp_setint(env, "db.test.compact", 0) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_upsert_get_index(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up1 );
	sp_destroy(o);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_upsert_get_branch0(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up1 );
	sp_destroy(o);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_upsert_get_branch1(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up1 );
	sp_destroy(o);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_upsert_get_branch2(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up1 );
	sp_destroy(o);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_upsert_get_compact(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );
	t( sp_setint(env, "db.test.compact", 0) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up1 );
	sp_destroy(o);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_upsert_upsert_get_cursor(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	t( o != NULL );
	void *cur = sp_cursor(env);
	o = sp_get(cur, o);
	t( *(int*)sp_getstring(o, "value", NULL) == up1 );
	o = sp_get(cur, o);
	t( o == NULL );
	sp_destroy(cur);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static int
upsert_operator_delete(char **result,
                       char **key, int *key_size, int key_count,
                       char *src, int src_size,
                       char *upsert, int upsert_size,
                       void *arg)
{
	(void)key;
	(void)key_size;
	(void)key_count;
	assert(src == NULL);
	assert(upsert != NULL);
	(void)arg;
	char *c = malloc(upsert_size);
	memcpy(c, upsert, upsert_size);
	*result = c;
	upsert_ops++;
	return upsert_size;
}

static void
upsert_set_delete_upsert_get_index(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_delete, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_delete_upsert_get_branch0(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_delete, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_delete_upsert_get_branch1(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_delete, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_set_delete_upsert_get_compact(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_delete, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );
	t( sp_setint(env, "db.test.compact", 0) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_delete_upsert_get_index(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_delete, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_delete_upsert_get_branch0(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_delete, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_delete_upsert_get_branch1(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_delete, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_delete_upsert_get_compact(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_delete, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );
	t( sp_setint(env, "db.test.compact", 0) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	o = sp_get(db, o);
	t( o != NULL );
	t( *(int*)sp_getstring(o, "value", NULL) == up );
	sp_destroy(o);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_sx_set_upsert_get(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *tx = sp_begin(env);

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(tx, o) == 0 );

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(tx, o) == -1 );

	t( sp_commit(tx) == 0 );

	t( upsert_ops == 0 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_sx_upsert_upsert(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *tx = sp_begin(env);

	void *o;
	int i = 0;

	o = sp_document(db);
	int up = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(tx, o) == 0 );

	o = sp_document(db);
	up = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up, sizeof(up)) == 0 );
	t( sp_upsert(tx, o) == -1 );

	t( sp_commit(tx) == 0 );

	t( upsert_ops == 0 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_cursor0(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( o != NULL );
	void *cur = sp_cursor(env);
	o = sp_get(cur, o);
	t( *(int*)sp_getstring(o, "value", NULL) == up1 );
	o = sp_get(cur, o);
	t( o == NULL );
	sp_destroy(cur);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_cursor1(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	void *cur = sp_cursor(env);

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( o != NULL );
	o = sp_get(cur, o);
	t( *(int*)sp_getstring(o, "value", NULL) == up0 );
	o = sp_get(cur, o);
	t( o == NULL );
	sp_destroy(cur);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_cursor2(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	void *cur = sp_cursor(env);

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	t( o != NULL );
	o = sp_get(cur, o);
	t( *(int*)sp_getstring(o, "value", NULL) == i );
	o = sp_get(cur, o);
	t( o == NULL );
	sp_destroy(cur);

	t( upsert_ops == 0 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_cursor3(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator_orphan, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	void *cur = sp_cursor(env);
	o = sp_document(db);
	t( o != NULL );
	o = sp_get(cur, o);
	t( *(int*)sp_getstring(o, "value", NULL) == up1 );
	o = sp_get(cur, o);
	t( o == NULL );
	sp_destroy(cur);

	t( upsert_ops == 1 );

	t( sp_destroy(env) == 0 );
}

static int
upsert_operator2(char **result,
                 char **key, int *key_size, int key_count,
                 char *src, int src_size,
                 char *upsert, int upsert_size,
                 void *arg)
{
	(void)key;
	(void)key_size;
	(void)key_count;
	assert(upsert != NULL);
	(void)arg;
	(void)src;
	(void)src_size;
	char *c = malloc(upsert_size);
	memcpy(c, upsert, upsert_size);
	*result = c;
	upsert_ops++;
	return upsert_size;
}

static void
upsert_cursor4(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator2, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	void *cur = sp_cursor(env);
	o = sp_document(db);
	t( o != NULL );
	o = sp_get(cur, o);
	t( *(int*)sp_getstring(o, "value", NULL) == up0 );
	o = sp_get(cur, o);
	t( o == NULL );
	sp_destroy(cur);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_cursor5(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator2, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_delete(db, o) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );
	t( sp_setint(env, "db.test.compact", 0) == 0 );

	void *cur = sp_cursor(env);
	o = sp_document(db);
	t( o != NULL );
	o = sp_get(cur, o);
	t( *(int*)sp_getstring(o, "value", NULL) == up0 );
	o = sp_get(cur, o);
	t( o == NULL );
	sp_destroy(cur);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

static void
upsert_cursor6(void)
{
	upsert_ops = 0;

	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 0) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.upsert", upsert_operator2, 0) == 0 );
	t( sp_open(env) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );

	void *o = sp_document(db);
	int i = 0;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &i, sizeof(i)) == 0 );
	t( sp_set(db, o) == 0 );

	o = sp_document(db);
	int up1 = 778;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up1, sizeof(up1)) == 0 );
	t( sp_upsert(db, o) == 0 );

	o = sp_document(db);
	int up0 = 777;
	t( sp_setstring(o, "key", &i, sizeof(i)) == 0 );
	t( sp_setstring(o, "value", &up0, sizeof(up0)) == 0 );
	t( sp_upsert(db, o) == 0 );

	t( sp_setint(env, "db.test.branch", 0) == 0 );
	t( sp_setint(env, "db.test.compact", 0) == 0 );

	void *cur = sp_cursor(env);
	o = sp_document(db);
	t( o != NULL );
	o = sp_get(cur, o);
	t( *(int*)sp_getstring(o, "value", NULL) == up0 );
	o = sp_get(cur, o);
	t( o == NULL );
	sp_destroy(cur);

	t( upsert_ops == 2 );

	t( sp_destroy(env) == 0 );
}

stgroup *upsert_group(void)
{
	stgroup *group = st_group("upsert");
	st_groupadd(group, st_test("no_operator", upsert_no_operator));
	st_groupadd(group, st_test("upsert_get_index", upsert_upsert_get_index));
	st_groupadd(group, st_test("upsert_get_branch0", upsert_upsert_get_branch0));
	st_groupadd(group, st_test("upsert_get_compact", upsert_upsert_get_compact));
	st_groupadd(group, st_test("set_upsert_get_index", upsert_set_upsert_get_index));
	st_groupadd(group, st_test("set_upsert_get_branch0", upsert_set_upsert_get_branch0));
	st_groupadd(group, st_test("set_upsert_get_branch1", upsert_set_upsert_get_branch1));
	st_groupadd(group, st_test("set_upsert_get_compact", upsert_set_upsert_get_compact));
	st_groupadd(group, st_test("set_upsert_upsert_get_index", upsert_set_upsert_upsert_get_index));
	st_groupadd(group, st_test("set_upsert_upsert_get_branch0", upsert_set_upsert_upsert_get_branch0));
	st_groupadd(group, st_test("set_upsert_upsert_get_branch1", upsert_set_upsert_upsert_get_branch1));
	st_groupadd(group, st_test("set_upsert_upsert_get_branch2", upsert_set_upsert_upsert_get_branch2));
	st_groupadd(group, st_test("set_upsert_upsert_get_compact", upsert_set_upsert_upsert_get_compact));
	st_groupadd(group, st_test("set_upsert_upsert_get_cursor", upsert_set_upsert_upsert_get_cursor));
	st_groupadd(group, st_test("set_delete_upsert_get_index", upsert_set_delete_upsert_get_index));
	st_groupadd(group, st_test("set_delete_upsert_get_branch0", upsert_set_delete_upsert_get_branch0));
	st_groupadd(group, st_test("set_delete_upsert_get_branch1", upsert_set_delete_upsert_get_branch1));
	st_groupadd(group, st_test("set_delete_upsert_get_compact", upsert_set_delete_upsert_get_compact));
	st_groupadd(group, st_test("delete_upsert_get_index", upsert_delete_upsert_get_index));
	st_groupadd(group, st_test("delete_upsert_get_branch0", upsert_delete_upsert_get_branch0));
	st_groupadd(group, st_test("delete_upsert_get_branch1", upsert_delete_upsert_get_branch1));
	st_groupadd(group, st_test("delete_upsert_get_compact", upsert_delete_upsert_get_compact));
	st_groupadd(group, st_test("sx_set_upsert_get", upsert_sx_set_upsert_get));
	st_groupadd(group, st_test("sx_upsert_upsert", upsert_sx_upsert_upsert));
	st_groupadd(group, st_test("upsert_cursor0", upsert_cursor0));
	st_groupadd(group, st_test("upsert_cursor1", upsert_cursor1));
	st_groupadd(group, st_test("upsert_cursor2", upsert_cursor2));
	st_groupadd(group, st_test("upsert_cursor3", upsert_cursor3));
	st_groupadd(group, st_test("upsert_cursor4", upsert_cursor4));
	st_groupadd(group, st_test("upsert_cursor5", upsert_cursor5));
	st_groupadd(group, st_test("upsert_cursor6", upsert_cursor6));
	return group;
}
