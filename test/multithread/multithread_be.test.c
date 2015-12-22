
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

#include <assert.h>
#include <time.h>
#include <sys/time.h>

static inline void
print_current(int i) {
	if (i > 0 && (i % 100000) == 0) {
		fprintf(st_r.output, " %.1fM", i / 1000000.0);
		fflush(st_r.output);
	}
}

static void
mt_set_checkpoint_get(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setint(env, "log.sync", 0) == 0 );
	t( sp_setint(env, "log.rotate_sync", 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );

	char value[100];
	memset(value, 0, sizeof(value));
	uint32_t n = 300000;
	uint32_t i, k;
	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		*(uint32_t*)value = k;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		t( sp_setstring(o, "value", value, sizeof(value)) == 0 );
		t( sp_set(db, o) == 0 );
		print_current(i);
	}
	t( sp_setint(env, "log.rotate", 0) == 0 );
	t( sp_setint(env, "scheduler.checkpoint", 0) == 0 );
	fprintf(st_r.output, " (checkpoint..");
	fflush(st_r.output);
	for (;;) {
		int active = sp_getint(env, "scheduler.checkpoint_active");
		if (!active)
			break;
	}
	fprintf(st_r.output, "done)");

	/* This works only with thread = 1.
	 *
	 * Real data flush can happed before index got
	 * collected and any other worker trigger
	 * checkpoint complete.
	*/
	t( sp_setint(env, "log.gc", 0) == 0 );
	t( sp_getint(env, "log.files") == 1 );

	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		o = sp_get(db, o);
		t( o != NULL );
		t( *(uint32_t*)sp_getstring(o, "value", NULL) == k );
		sp_destroy(o);
		print_current(i);
	}

	t( sp_destroy(env) == 0 );
}

static void
mt_set_snapshot_recover_get(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setint(env, "log.sync", 0) == 0 );
	t( sp_setint(env, "log.rotate_sync", 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );

	char value[100];
	memset(value, 0, sizeof(value));
	uint32_t n = 300000;
	uint32_t i, k;
	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		*(uint32_t*)value = k;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		t( sp_setstring(o, "value", value, sizeof(value)) == 0 );
		t( sp_set(db, o) == 0 );
		print_current(i);
	}
	t( sp_setint(env, "log.rotate", 0) == 0 );
	t( sp_setint(env, "scheduler.snapshot", 0) == 0 );
	fprintf(st_r.output, " (snapshot..");
	fflush(st_r.output);
	for (;;) {
		int active = sp_getint(env, "scheduler.snapshot_active");
		if (!active)
			break;
	}
	fprintf(st_r.output, "done)");
	t( sp_destroy(env) == 0 );

	fprintf(st_r.output, "(recover..");
	fflush(st_r.output);
	env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setint(env, "log.sync", 0) == 0 );
	t( sp_setint(env, "log.rotate_sync", 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );
	fprintf(st_r.output, "done)");

	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		o = sp_get(db, o);
		t( o != NULL );
		t( *(uint32_t*)sp_getstring(o, "value", NULL) == k );
		sp_destroy(o);
		print_current(i);
	}

	t( sp_destroy(env) == 0 );
}

static void
mt_set_delete_get(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_open(env) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(db) == 0 );

	char value[100];
	memset(value, 0, sizeof(value));
	uint32_t n = 700000;
	uint32_t i, k;
	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		*(uint32_t*)value = k;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		t( sp_setstring(o, "value", value, sizeof(value)) == 0 );
		t( sp_set(db, o) == 0 );
		print_current(i);
	}
	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		*(uint32_t*)value = k;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		t( sp_setstring(o, "value", value, sizeof(value)) == 0 );
		t( sp_delete(db, o) == 0 );
		print_current(i);
	}
	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		o = sp_get(db, o);
		t( o == NULL );
		print_current(i);
	}
	t( sp_destroy(env) == 0 );
}

static void
mt_set_get_kv_multipart(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setint(env, "db.test.compression_key", 1) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "string", 0) == 0 );
	t( sp_setstring(env, "db.test.index", "key_b", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key_b", "u32", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );

	uint32_t n = 500000;
	uint32_t i;

	char key_a[] = "very_long_long_key_part";
	srand(82351);
	for (i = 0; i < n; i++) {
		uint32_t key_b = rand();
		uint32_t value = key_b;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", key_a, sizeof(key_a)) == 0 );
		t( sp_setstring(o, "key_b", &key_b, sizeof(key_b)) == 0 );
		t( sp_setstring(o, "value", &value, sizeof(value)) == 0 );
		t( sp_set(db, o) == 0 );
		print_current(i);
	}
	srand(82351);
	for (i = 0; i < n; i++) {
		uint32_t key_b = rand();
		uint32_t value = key_b;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", key_a, sizeof(key_a)) == 0 );
		t( sp_setstring(o, "key_b", &key_b, sizeof(key_b)) == 0 );
		o = sp_get(db, o);
		t( o != NULL );
		int size = 0;
		t( memcmp(sp_getstring(o, "key", &size), key_a, sizeof(key_a)) == 0 );
		t( *(uint32_t*)sp_getstring(o, "key_b", &size) == key_b );
		t( *(uint32_t*)sp_getstring(o, "value", &size) == value );
		sp_destroy(o);
		print_current(i);
	}

	t( sp_destroy(env) == 0 );
}

static void
mt_set_get_document_multipart(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.format", "document", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setstring(env, "db.test.index", "key_b", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key_b", "u32", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );

	struct document {
		uint32_t value;
		char used0[89];
		uint32_t key_a;
		char used1[15];
		uint32_t key_b;
		char used2[10];
	} sspacked;
	struct document doc;
	memset(&doc, 'x', sizeof(doc));

	uint32_t n = 500000;
	uint32_t i;

	srand(82351);
	for (i = 0; i < n; i++) {
		doc.key_a = rand();
		doc.key_b = rand();
		doc.value = doc.key_a ^ doc.key_b;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &doc.key_a, sizeof(doc.key_a)) == 0 );
		t( sp_setstring(o, "key_b", &doc.key_b, sizeof(doc.key_b)) == 0 );
		t( sp_setstring(o, "value", &doc, sizeof(doc)) == 0 );
		t( sp_set(db, o) == 0 );
		print_current(i);
	}
	srand(82351);
	for (i = 0; i < n; i++) {
		doc.key_a = rand();
		doc.key_b = rand();
		doc.value = doc.key_a ^ doc.key_b;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &doc.key_a, sizeof(doc.key_a)) == 0 );
		t( sp_setstring(o, "key_b", &doc.key_b, sizeof(doc.key_b)) == 0 );
		o = sp_get(db, o);
		t( o != NULL );

		int valuesize = 0;
		struct document *ret =
			(struct document*)sp_getstring(o, "value", &valuesize);
		t( valuesize == sizeof(doc) );
		t( doc.key_a == ret->key_a );
		t( doc.key_b == ret->key_b );
		t( doc.value == (ret->key_a ^ ret->key_b) );
		sp_destroy(o);

		print_current(i);
	}

	t( sp_destroy(env) == 0 );
}

static void
mt_set_get_document_multipart_cursor(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.format", "document", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	t( sp_setstring(env, "db.test.index", "key_b", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key_b", "u32", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );

	struct document {
		uint32_t value;
		char used0[89];
		uint32_t key_a;
		char used1[15];
		uint32_t key_b;
		char used2[10];
	} sspacked;
	struct document doc;
	memset(&doc, 'x', sizeof(doc));

	uint32_t n = 500000;
	uint32_t i;

	for (i = 0; i < n; i++) {
		doc.key_a = i;
		doc.key_b = i;
		doc.value = doc.key_a ^ doc.key_b;
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &doc.key_a, sizeof(doc.key_a)) == 0 );
		t( sp_setstring(o, "key_b", &doc.key_b, sizeof(doc.key_b)) == 0 );
		t( sp_setstring(o, "value", &doc, sizeof(doc)) == 0 );
		t( sp_set(db, o) == 0 );
		print_current(i);
	}

	i = 0;
	void *o = sp_document(db);
	t( o != NULL );
	void *cursor = sp_cursor(env);
	t( cursor != NULL );
	while ((o = sp_get(cursor, o))) {
		int valuesize = 0;
		struct document *ret =
			(struct document*)sp_getstring(o, "value", &valuesize);
		t( valuesize == sizeof(doc) );
		t( ret->key_a == i );
		t( ret->key_b == i );
		print_current(i);
		i++;
	}
	sp_destroy(cursor);
	t( i == n );

	t( sp_destroy(env) == 0 );
}

static void
mt_async_read(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setint(env, "compaction.0.async", 1) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.format", "kv", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );

	int i = 0;
	while (i < 100000) {
		void *o = sp_document(db);
		assert(o != NULL);
		sp_setstring(o, "key", &i, sizeof(i));
		int rc = sp_set(db, o);
		t( rc == 0 );
		print_current(i);
		i++;
	}
	fprintf(st_r.output, " (insert done..iterate) ");

	/* trigger iteration */
	void *o = sp_document(db);
	sp_setint(o, "async", 1);
	sp_setstring(o, "order", ">=", 0);
	o = sp_get(db, o);
	t( o != NULL );
	sp_destroy(o);

	i = 0;
	while (i < 100000) {
		o = sp_poll(env);
		if (o == NULL)
			continue;
		t( strcmp(sp_getstring(o, "type", 0), "on_read") == 0 );
		t( sp_getint(o, "status") == 1 );
		t( *(int*)sp_getstring(o, "key", NULL) == i );
		o = sp_get(db, o);
		t( o != NULL );
		sp_destroy(o);
		print_current(i);
		i++;
	}
	t( i == 100000 );
	fprintf(st_r.output, "(complete)");
	t( sp_destroy(env) == 0 );
}

static void
mt_set_get_anticache(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setint(env, "memory.anticache", 500 * 1024) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setstring(env, "db.test.storage", "anti-cache", 0) == 0 );
	t( sp_setint(env, "db.test.compression_key", 0) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setint(env, "db.test.node_size", 100 * 1024) == 0 );
	t( sp_setint(env, "db.test.page_size", 8 * 1024) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );

	uint32_t n = 700000;
	uint32_t i, k;

	char value[100];
	memset(value, 0, sizeof(value));

	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		t( sp_setstring(o, "value", value, sizeof(value)) == 0 );
		t( sp_set(db, o) == 0 );
		print_current(i);
	}

	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		o = sp_get(db, o);
		t( o != NULL );
		sp_destroy(o);
		print_current(i);
	}

	t( sp_destroy(env) == 0 );
}

static void
mt_set_lru(void)
{
	void *env = sp_env();
	t( env != NULL );
	t( sp_setstring(env, "sophia.path", st_r.conf->sophia_dir, 0) == 0 );
	t( sp_setint(env, "compaction.0.lru_prio", 3) == 0 );
	t( sp_setint(env, "compaction.0.branch_wm", 500000) == 0 );
	t( sp_setint(env, "scheduler.threads", 5) == 0 );
	t( sp_setstring(env, "log.path", st_r.conf->log_dir, 0) == 0 );
	t( sp_setstring(env, "db", "test", 0) == 0 );
	t( sp_setstring(env, "db.test.path", st_r.conf->db_dir, 0) == 0 );
	t( sp_setint(env, "db.test.compression_key", 0) == 0 );
	t( sp_setint(env, "db.test.lru", 1 * 1024 * 1024) == 0 );
	t( sp_setint(env, "db.test.sync", 0) == 0 );
	t( sp_setstring(env, "db.test.index.key", "u32", 0) == 0 );
	void *db = sp_getobject(env, "db.test");
	t( db != NULL );
	t( sp_open(env) == 0 );

	uint32_t n = 700000;
	uint32_t i, k;

	char value[100];
	memset(value, 0, sizeof(value));

	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		void *o = sp_document(db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		t( sp_setstring(o, "value", value, sizeof(value)) == 0 );
		t( sp_set(db, o) == 0 );
		print_current(i);
	}

	int64_t size = sp_getint(env, "db.test.index.size");
	fprintf(st_r.output, " (cache: 1Mb, size: %"PRIu64")", size);

	t( sp_destroy(env) == 0 );
}

stgroup *multithread_be_group(void)
{
	stgroup *group = st_group("mt_backend");
	st_groupadd(group, st_test("set_delete_get", mt_set_delete_get));
	st_groupadd(group, st_test("set_snapshot_recover_get", mt_set_snapshot_recover_get));
	st_groupadd(group, st_test("set_checkpoint_get", mt_set_checkpoint_get));
	st_groupadd(group, st_test("set_get_kv_multipart", mt_set_get_kv_multipart));
	st_groupadd(group, st_test("set_get_document_multipart", mt_set_get_document_multipart));
	st_groupadd(group, st_test("set_get_document_multipart_cursor", mt_set_get_document_multipart_cursor));
	st_groupadd(group, st_test("set_get_anticache", mt_set_get_anticache));
	st_groupadd(group, st_test("set_lru", mt_set_lru));
	st_groupadd(group, st_test("async_read", mt_async_read));
	return group;
}

static void
mt_setget(void)
{
	char value[100];
	memset(value, 0, sizeof(value));
	uint32_t n = 300000;
	uint32_t i, k;
	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		*(uint32_t*)value = k;
		void *o = sp_document(st_r.db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		t( sp_setstring(o, "value", value, sizeof(value)) == 0 );
		t( sp_set(st_r.db, o) == 0 );
		print_current(i);
	}
	srand(82351);
	for (i = 0; i < n; i++) {
		k = rand();
		void *o = sp_document(st_r.db);
		t( o != NULL );
		t( sp_setstring(o, "key", &k, sizeof(k)) == 0 );
		o = sp_get(st_r.db, o);
		t( o != NULL );
		t( *(uint32_t*)sp_getstring(o, "value", NULL) == k );
		sp_destroy(o);
		print_current(i);
	}
}

stgroup *multithread_be_multipass_group(void)
{
	stgroup *group = st_group("mt_backend_multipass");
	st_groupadd(group, st_test("setget", mt_setget));
	return group;
}
