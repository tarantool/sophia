#ifndef SE_CURSOR_H_
#define SE_CURSOR_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct secursor secursor;

struct secursor {
	so o;
	sx t;
	uint64_t start;
	int ops;
	int read_disk;
	int read_cache;
	int read_commited;
	sicache *cache;
};

so *se_cursornew(se*, uint64_t);

#endif
