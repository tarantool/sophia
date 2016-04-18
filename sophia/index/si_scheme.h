#ifndef SI_SCHEME_H_
#define SI_SCHEME_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct sischeme sischeme;

typedef enum {
	SI_SCACHE,
	SI_SANTI_CACHE,
	SI_SIN_MEMORY
} sistorage;

struct sischeme {
	uint32_t    id;
	char       *name;
	char       *path;
	uint32_t    path_fail_on_exists;
	uint32_t    path_fail_on_drop;
	char       *path_backup;
	uint32_t    mmap;
	sistorage   storage;
	char       *storage_sz;
	uint32_t    sync;
	uint64_t    node_size;
	uint32_t    node_page_size;
	uint32_t    node_page_checksum;
	uint32_t    node_compact_load;
	uint32_t    expire;
	uint32_t    compression;
	char       *compression_sz;
	ssfilterif *compression_if;
	uint32_t    compression_branch;
	char       *compression_branch_sz;
	ssfilterif *compression_branch_if;
	uint32_t    compression_key;
	uint32_t    temperature;
	uint32_t    amqf;
	uint64_t    lru;
	uint32_t    lru_step;
	uint32_t    buf_gc_wm;
	sfstorage   fmt_storage;
	sfupsert    fmt_upsert;
	sfscheme    scheme;
	srversion   version;
	srversion   version_storage;
};

void si_schemeinit(sischeme*);
void si_schemefree(sischeme*, sr*);
int  si_schemedeploy(sischeme*, sr*);
int  si_schemerecover(sischeme*, sr*);

#endif
