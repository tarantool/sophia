#ifndef SF_UPSERT_H_
#define SF_UPSERT_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef int (*sfupsertf)(char **result,
                         char **key, int *key_size, int key_count,
                         char *src, int src_size,
                         char *upsert, int upsert_size,
                         void *arg);

typedef struct {
	sfupsertf function;
	void *arg;
} sfupsert;

static inline void
sf_upsertinit(sfupsert *u)
{
	memset(u, 0, sizeof(*u));
}

static inline void
sf_upsertset(sfupsert *u, sfupsertf function)
{
	u->function = function;
}

static inline void
sf_upsertset_arg(sfupsert *u, void *arg)
{
	u->arg = arg;
}

static inline int
sf_upserthas(sfupsert *u) {
	return u->function != NULL;
}

#endif
