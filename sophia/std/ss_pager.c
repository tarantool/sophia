
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <libss.h>

void ss_pagerinit(sspager *p, ssvfs *vfs, uint32_t pool_count, uint32_t page_size)
{
	p->page_size  = sizeof(sspage) + page_size;
	p->pool_count = pool_count;
	p->pool_size  = sizeof(sspagepool) + pool_count * p->page_size;
	p->pools      = 0;
	p->pp         = NULL;
	p->p          = NULL;
	p->vfs        = vfs;
}

void ss_pagerfree(sspager *p)
{
	sspagepool *pp_next, *pp = p->pp;
	while (pp) {
		pp_next = pp->next;
		ssmmap map = {
			.p = (char*)pp,
			.size = p->pool_size
		};
		ss_vfsmunmap(p->vfs, &map);
		pp = pp_next;
	}
}

static inline void
ss_pagerprefetch(sspager *p, sspagepool *pp)
{
	register sspage *start =
		(sspage*)((char*)pp + sizeof(sspagepool));
	register sspage *prev = start;
	register uint32_t i = 1;
	start->pool = pp;
	while (i < p->pool_count) {
		sspage *page =
			(sspage*)((char*)start + i * p->page_size);
		page->pool = pp;
		prev->next = page;
		prev = page;
		i++;
	}
	prev->next = NULL;
	p->p = start;
}

int ss_pageradd(sspager *p)
{
	ssmmap map;
	ss_mmapinit(&map);
	int rc = ss_vfsmmap_allocate(p->vfs, &map, p->pool_size);
	if (ssunlikely(rc == -1))
		return -1;
	sspagepool *pp = (sspagepool*)map.p;
	pp->used = 0;
	pp->next = p->pp;
	p->pp = pp;
	p->pools++;
	ss_pagerprefetch(p, pp);
	return 0;
}

void *ss_pagerpop(sspager *p)
{
	if (p->p)
		goto fetch;
	if (ssunlikely(ss_pageradd(p) == -1))
		return NULL;
fetch:;
	sspage *page = p->p;
	p->p = page->next;
	page->pool->used++;
	return page;
}

void ss_pagerpush(sspager *p, sspage *page)
{
	page->pool->used--;
	page->next = p->p;
	p->p = page;
}
