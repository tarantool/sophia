
/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

#include <libsr.h>
#include <libsv.h>
#include <libsd.h>
#include <libst.h>
#include <sophia.h>

static void
addv(sdbuild *b, uint64_t lsn, uint8_t flags, int *key)
{
	svlocal l;
	l.lsn         = lsn;
	l.flags       = flags;
	l.key         = key;
	l.keysize     = sizeof(int);
	l.value       = NULL;
	l.valuesize   = 0;
	sv lv;
	svinit(&lv, &sv_localif, &l, NULL);
	sd_buildadd(b, &lv, flags & SVDUP);
}

static void
sdpageiter_lte_empty(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);

	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &i, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	sv *v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &j, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &k, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_eq0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);

	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &i, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &j, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &k, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_eq1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);

	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &i, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &j, sizeof(int), 1ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &k, sizeof(int), 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_eq2(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srcomparator cmp = { sr_cmpu32, NULL };
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);

	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &i, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	sv *v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &j, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &k, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_minmax0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srcomparator cmp = { sr_cmpu32, NULL };
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);

	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int min = 6;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &min, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	t( sr_iterof(&it) == NULL);

	int max = 16;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_minmax1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srcomparator cmp = { sr_cmpu32, NULL };
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);

	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int z = 2;
	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 4, SVSET, &z);
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int min = 6;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &min, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == z);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &min, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	int max = 16;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_minmax2(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);

	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int z = 2;
	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 1, SVSET, &z);
	addv(&b, 2, SVSET, &i);
	addv(&b, 3, SVSET, &j);
	addv(&b, 4, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int max = 16;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == z);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_mid0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);

	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 8;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	p = 10;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	p = 555;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_mid1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 8;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 2ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	p = 10;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 1ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	p = 555;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_iterate0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_iterate1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &k, sizeof(k), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lt_eq(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, &i, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	sv *v = sr_iterof(&it);
	t( v == NULL);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, &j, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, &k, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lt_minmax(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int min = 7;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, &min, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	t( sr_iterof(&it) == NULL);

	int max = 16;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lt_mid(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 8;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	p = 10;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	p = 555;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lt_iterate0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, NULL, 0, 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lt_iterate1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LT, &k, sizeof(k), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_dup_eq(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int j = 4;
	int i = 7;
	addv(&b, 0, SVSET, &j);
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET|SVDUP, &i);
	addv(&b, 1, SVSET|SVDUP, &i);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 3 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 2 ); sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 1 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 0ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 0 );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_dup_mid(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 5, SVSET, &i);
	addv(&b, 4, SVSET, &j);
	addv(&b, 3, SVSET|SVDUP, &j);
	addv(&b, 2, SVSET|SVDUP, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 8;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	sv *v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	p = 10;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 2 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 3 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 4 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 10ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 4 );

	p = 8;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 8ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i );
	t( svlsn(v) == 5 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_dup_mid_gt(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 90, SVSET, &i);
	addv(&b, 80, SVSET, &j);
	addv(&b, 70, SVSET|SVDUP, &j);
	addv(&b, 60, SVSET|SVDUP, &j);
	addv(&b, 50, SVSET, &k);
	addv(&b, 40, SVSET|SVDUP, &k);
	addv(&b, 30, SVSET|SVDUP, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 16;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	sv *v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 30ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 38ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 40ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 40);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 50ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 50);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &j, sizeof(int), 90ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 80);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_dup_mid_lt(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 3;
	addv(&b, 50, SVSET, &k);
	addv(&b, 40, SVSET|SVDUP, &k);
	addv(&b, 30, SVSET|SVDUP, &k);
	addv(&b, 90, SVSET, &i);
	addv(&b, 80, SVSET, &j);
	addv(&b, 70, SVSET|SVDUP, &j);
	addv(&b, 60, SVSET|SVDUP, &j);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 6;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 30ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 38ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 40ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 40);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 50ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 50);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &j, sizeof(int), 90ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 80);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_dup_iterate0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 90, SVSET, &i);
	addv(&b, 80, SVSET, &j);
	addv(&b, 70, SVSET|SVDUP, &j);
	addv(&b, 60, SVSET|SVDUP, &j);
	addv(&b, 50, SVSET, &k);
	addv(&b, 40, SVSET|SVDUP, &k);
	addv(&b, 30, SVSET|SVDUP, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 100;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 100ULL);

	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 50);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 80);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 90);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_lte_dup_iterate1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 42, SVSET, &i);
	addv(&b, 80, SVSET, &j);
	addv(&b, 60, SVSET|SVDUP, &j);
	addv(&b, 41, SVSET|SVDUP, &j);
	addv(&b, 50, SVSET, &k);
	addv(&b, 40, SVSET|SVDUP, &k);
	addv(&b, 30, SVSET|SVDUP, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 100;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 30ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);
	sr_iternext(&it);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, &p, sizeof(int), 42ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 40);
	sr_iternext(&it);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 41);
	sr_iternext(&it);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 42);
	sr_iternext(&it);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_LTE, NULL, 0, 42ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 40);
	sr_iternext(&it);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 41);
	sr_iternext(&it);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 42);
	sr_iternext(&it);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_eq0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &i, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &j, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &k, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_eq1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &i, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &j, sizeof(int), 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &k, sizeof(int), 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_eq2(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &i, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	sv *v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &j, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &k, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_minmax0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int min = 6;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &min, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	int max = 16;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_minmax1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int z = 2;
	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 4, SVSET, &z);
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int min = 6;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &min, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &min, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );


	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &min, sizeof(int), 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &min, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &min, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	int max = 16;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &max, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_minmax2(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int z = 2;
	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 4, SVSET, &z);
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int max = 2;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == z);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &max, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &max, sizeof(int), 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &max, sizeof(int), 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &max, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_mid0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 8;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	p = 10;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	p = 2;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);

	p = 555;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_mid1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 8;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	p = 10;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	p = 1;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_iterate0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_iterate1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &i, sizeof(k), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gt_eq(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &i, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( v != NULL);
	t( *(int*)svkey(v) == j);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &j, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &k, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gt_minmax(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 8;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int min = 7;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &min, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	int max = 15;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &max, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gt_mid(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 8;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);

	p = 10;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	p = 555;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gt_iterate0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, NULL, 0, 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gt_iterate1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GT, &i, sizeof(i), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_dup_eq(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int j = 4;
	int i = 7;
	addv(&b, 4, SVSET, &j);
	addv(&b, 3, SVSET, &i);
	addv(&b, 2, SVSET|SVDUP, &i);
	addv(&b, 1, SVSET|SVDUP, &i);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 4 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 3ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 3 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 2 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 1 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 0ULL);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_dup_mid(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 5, SVSET, &i);
	addv(&b, 4, SVSET, &j);
	addv(&b, 3, SVSET|SVDUP, &j);
	addv(&b, 2, SVSET|SVDUP, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 8;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 4 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 1ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);

	p = 10;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k );
	t( svlsn(v) == 1 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k );
	t( svlsn(v) == 1 );

	p = 8;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 8ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 4 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 3ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 3 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 2ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 2 );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 6ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_dup_mid_gt(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 90, SVSET, &i);
	addv(&b, 80, SVSET, &j);
	addv(&b, 70, SVSET|SVDUP, &j);
	addv(&b, 60, SVSET|SVDUP, &j);
	addv(&b, 50, SVSET, &k);
	addv(&b, 40, SVSET|SVDUP, &k);
	addv(&b, 30, SVSET|SVDUP, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 6;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 4ULL);
	t( sr_iterhas(&it) == 0 );
	sv *v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 30ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 38ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 40ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 40);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 50ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 50);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &j, sizeof(int), 90ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 80);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_dup_mid_lt(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int k = 7;
	int i = 8;
	int j = 9;
	addv(&b, 50, SVSET, &k);
	addv(&b, 40, SVSET|SVDUP, &k);
	addv(&b, 30, SVSET|SVDUP, &k);
	addv(&b, 90, SVSET, &i);
	addv(&b, 80, SVSET, &j);
	addv(&b, 70, SVSET|SVDUP, &j);
	addv(&b, 60, SVSET|SVDUP, &j);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 6;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 30ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 38ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 40ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 40);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 50ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 50);

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &j, sizeof(int), 90ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 80);
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_dup_iterate0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 90, SVSET, &i);
	addv(&b, 80, SVSET, &j);
	addv(&b, 70, SVSET|SVDUP, &j);
	addv(&b, 60, SVSET|SVDUP, &j);
	addv(&b, 50, SVSET, &k);
	addv(&b, 40, SVSET|SVDUP, &k);
	addv(&b, 30, SVSET|SVDUP, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 1;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 100ULL);

	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 90);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 80);
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 50);
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_gte_dup_iterate1(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 42, SVSET, &i);
	addv(&b, 80, SVSET, &j);
	addv(&b, 60, SVSET|SVDUP, &j);
	addv(&b, 41, SVSET|SVDUP, &j);
	addv(&b, 50, SVSET, &k);
	addv(&b, 40, SVSET|SVDUP, &k);
	addv(&b, 30, SVSET|SVDUP, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	int p = 1;
	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 30ULL);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 30);
	sr_iternext(&it);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, &p, sizeof(int), 42ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 42);
	sr_iternext(&it);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 41);
	sr_iternext(&it);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 40);
	sr_iternext(&it);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );

	sr_iterinit(&it, &sd_pageiter, &r);
	sr_iteropen(&it, &page, SR_GTE, NULL, 0, 60ULL);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == i);
	t( svlsn(v) == 42);
	sr_iternext(&it);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j);
	t( svlsn(v) == 60);
	sr_iternext(&it);
	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k);
	t( svlsn(v) == 50);
	sr_iternext(&it);
	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_update0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);
	int i = 0;
	for (; i < 10; i++)
		addv(&b, i, SVSET, &i);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiter, &r);
	i = 5;
	t( sr_iteropen(&it, &page, SR_UPDATE, &i, sizeof(i), (uint64_t)i) == 0 );
	sr_iterclose(&it);

	sr_iterinit(&it, &sd_pageiter, &r);
	i = 5;
	t( sr_iteropen(&it, &page, SR_UPDATE, &i, sizeof(i), (uint64_t)(i - 1)) == 1 );
	sr_iterclose(&it);

	sr_iterinit(&it, &sd_pageiter, &r);
	i = 5;
	t( sr_iteropen(&it, &page, SR_UPDATE, &i, sizeof(i), (uint64_t)(i + 1)) == 0 );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_random0(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srcomparator cmp = { sr_cmpu32, NULL };
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);
	int i = 0;
	for (; i < 100; i++)
		addv(&b, i, SVSET, &i);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	srand(2341);
	for (; i < 1000; i++) {
		uint32_t rnd = rand() % 100;
		sriter it;
		sr_iterinit(&it, &sd_pageiter, &r);
		sr_iteropen(&it, &page, SR_RANDOM, &rnd, sizeof(rnd), UINT64_MAX);
		t( sr_iterhas(&it) != 0 );
		sv *v = sr_iterof(&it);
		t( v != NULL );
		int k = *(int*)svkey(v);
		t( k >= 0 && k < 100 );
		i++;
		sr_iterclose(&it);
	}
	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

static void
sdpageiter_iterate_raw(stc *cx srunused)
{
	sra a;
	sr_allocopen(&a, &sr_astd);
	srcomparator cmp = { sr_cmpu32, NULL };
	srinjection ij;
	memset(&ij, 0, sizeof(ij));
	srerror error;
	sr_errorinit(&error);
	sr r;
	srcrcf crc = sr_crc32c_function();
	sr_init(&r, &error, &a, NULL, &cmp, &ij, crc);
	sdbuild b;
	sd_buildinit(&b, &r);
	t( sd_buildbegin(&b) == 0);

	int i = 7;
	int j = 9;
	int k = 15;
	addv(&b, 5, SVSET, &i);
	addv(&b, 4, SVSET, &j);
	addv(&b, 3, SVSET|SVDUP, &j);
	addv(&b, 2, SVSET|SVDUP, &j);
	addv(&b, 1, SVSET, &k);
	sd_buildend(&b);

	srbuf buf;
	sr_bufinit(&buf);
	t( sd_buildwritepage(&b, &buf) == 0 );
	sdpageheader *h = (sdpageheader*)buf.s;
	sdpage page;
	sd_pageinit(&page, h);

	sriter it;
	sr_iterinit(&it, &sd_pageiterraw, &r);
	sr_iteropen(&it, &page);
	t( sr_iterhas(&it) != 0 );
	sv *v = sr_iterof(&it);
	t( *(int*)svkey(v) == i );
	t( svlsn(v) == 5 );
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 4 );
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 3 );
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == j );
	t( svlsn(v) == 2 );
	sr_iternext(&it);

	t( sr_iterhas(&it) != 0 );
	v = sr_iterof(&it);
	t( *(int*)svkey(v) == k );
	t( svlsn(v) == 1 );
	sr_iternext(&it);

	t( sr_iterhas(&it) == 0 );
	v = sr_iterof(&it);
	t( v == NULL );
	sr_iterclose(&it);

	sd_buildfree(&b);
	sr_buffree(&buf, &a);
}

stgroup *sdpageiter_group(void)
{
	stgroup *group = st_group("sdpageiter");
	st_groupadd(group, st_test("lte_empty", sdpageiter_lte_empty));
	st_groupadd(group, st_test("lte_eq0", sdpageiter_lte_eq0));
	st_groupadd(group, st_test("lte_eq1", sdpageiter_lte_eq1));
	st_groupadd(group, st_test("lte_eq2", sdpageiter_lte_eq2));
	st_groupadd(group, st_test("lte_minmax0",  sdpageiter_lte_minmax0));
	st_groupadd(group, st_test("lte_minmax1", sdpageiter_lte_minmax1));
	st_groupadd(group, st_test("lte_minmax2", sdpageiter_lte_minmax2));
	st_groupadd(group, st_test("lte_mid0", sdpageiter_lte_mid0));
	st_groupadd(group, st_test("lte_mid1", sdpageiter_lte_mid1));
	st_groupadd(group, st_test("lte_iterate0", sdpageiter_lte_iterate0));
	st_groupadd(group, st_test("lte_iterate1", sdpageiter_lte_iterate1));
	st_groupadd(group, st_test("lt_eq", sdpageiter_lt_eq));
	st_groupadd(group, st_test("lt_minmax", sdpageiter_lt_minmax));
	st_groupadd(group, st_test("lt_mid", sdpageiter_lt_mid));
	st_groupadd(group, st_test("lt_iterate0", sdpageiter_lt_iterate0));
	st_groupadd(group, st_test("lt_iterate1", sdpageiter_lt_iterate1));
	st_groupadd(group, st_test("lte_dup_eq", sdpageiter_lte_dup_eq));
	st_groupadd(group, st_test("lte_dup_mid", sdpageiter_lte_dup_mid));
	st_groupadd(group, st_test("lte_dup_mid_gt", sdpageiter_lte_dup_mid_gt));
	st_groupadd(group, st_test("lte_dup_mid_lt", sdpageiter_lte_dup_mid_lt));
	st_groupadd(group, st_test("lte_dup_iterate0", sdpageiter_lte_dup_iterate0));
	st_groupadd(group, st_test("lte_dup_iterate1", sdpageiter_lte_dup_iterate1));
	st_groupadd(group, st_test("gte_eq0", sdpageiter_gte_eq0));
	st_groupadd(group, st_test("gte_eq1", sdpageiter_gte_eq1));
	st_groupadd(group, st_test("gte_eq2", sdpageiter_gte_eq2));
	st_groupadd(group, st_test("gte_minmax0", sdpageiter_gte_minmax0));
	st_groupadd(group, st_test("gte_minmax1", sdpageiter_gte_minmax1));
	st_groupadd(group, st_test("gte_minmax2", sdpageiter_gte_minmax2));
	st_groupadd(group, st_test("gte_mid0", sdpageiter_gte_mid0));
	st_groupadd(group, st_test("gte_mid1", sdpageiter_gte_mid1));
	st_groupadd(group, st_test("gte_iterate0", sdpageiter_gte_iterate0));
	st_groupadd(group, st_test("gte_iterate1", sdpageiter_gte_iterate1));
	st_groupadd(group, st_test("gt_eq", sdpageiter_gt_eq));
	st_groupadd(group, st_test("gt_minmax", sdpageiter_gt_minmax));
	st_groupadd(group, st_test("gt_mid", sdpageiter_gt_mid));
	st_groupadd(group, st_test("gt_iterate0", sdpageiter_gt_iterate0));
	st_groupadd(group, st_test("gt_iterate1", sdpageiter_gt_iterate1));
	st_groupadd(group, st_test("gte_dup_eq", sdpageiter_gte_dup_eq));
	st_groupadd(group, st_test("gte_dup_mid", sdpageiter_gte_dup_mid));
	st_groupadd(group, st_test("gte_dup_mid_gt", sdpageiter_gte_dup_mid_gt));
	st_groupadd(group, st_test("gte_dup_mid_lt", sdpageiter_gte_dup_mid_lt));
	st_groupadd(group, st_test("gte_dup_iterate0", sdpageiter_gte_dup_iterate0));
	st_groupadd(group, st_test("gte_dup_iterate1", sdpageiter_gte_dup_iterate1));
	st_groupadd(group, st_test("update0", sdpageiter_update0));
	st_groupadd(group, st_test("random0", sdpageiter_random0));
	st_groupadd(group, st_test("iterate_raw", sdpageiter_iterate_raw));
	return group;
}
