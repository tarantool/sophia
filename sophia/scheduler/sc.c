
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
#include <libsd.h>
#include <libsl.h>
#include <libsi.h>
#include <libsy.h>
#include <libsc.h>

int sc_init(sc *s, sr *r, sstrigger *on_event, slpool *lp)
{
	uint64_t now = ss_utime();
	ss_mutexinit(&s->lock);
	s->checkpoint_lsn           = 0;
	s->checkpoint_lsn_last      = 0;
	s->checkpoint               = 0;
	s->age                      = 0;
	s->age_time                 = now;
	s->backup_bsn               = 0;
	s->backup_bsn_last          = 0;
	s->backup_bsn_last_complete = 0;
	s->backup_events            = 0;
	s->backup                   = 0;
	s->anticache_asn            = 0;
	s->anticache_asn_last       = 0;
	s->anticache_storage        = 0;
	s->anticache_time           = now;
	s->anticache                = 0;
	s->anticache_limit          = 0;
	s->snapshot_ssn             = 0;
	s->snapshot_ssn_last        = 0;
	s->snapshot_time            = now;
	s->snapshot                 = 0;
	s->gc                       = 0;
	s->gc_time                  = now;
	s->lru                      = 0;
	s->lru_time                 = now;
	s->shutdown_pending         = 0;
	s->rotate                   = 0;
	s->read                     = 0;
	s->i                        = NULL;
	s->count                    = 0;
	s->rr                       = 0;
	s->r                        = r;
	s->on_event                 = on_event;
	s->backup_path              = NULL;
	s->lp                       = lp;
	ss_threadpool_init(&s->tp);
	sc_workerpool_init(&s->wp);
	sc_readpool_init(&s->rp, r);
	return 0;
}

int sc_set(sc *s, uint64_t anticache, char *backup_path)
{
	s->anticache_limit = anticache;
	s->backup_path = backup_path;
	return 0;
}

int sc_create(sc *s, ssthreadf function, void *arg, int n)
{
	return ss_threadpool_new(&s->tp, s->r->a, n, function, arg);
}

int sc_shutdown(sc *s)
{
	sr *r = s->r;
	sc_readpool_wakeup(&s->rp);
	int rcret = 0;
	int rc = ss_threadpool_shutdown(&s->tp, r->a);
	if (ssunlikely(rc == -1))
		rcret = -1;
	rc = sc_workerpool_free(&s->wp, r);
	if (ssunlikely(rc == -1))
		rcret = -1;
	sc_readpool_free(&s->rp);
	/* destroy databases which are ready for
	 * shutdown or drop */
	if (s->i) {
		int i = 0;
		while (i < s->count) {
			scdb *db = &s->i[i];
			int status = sr_status(&db->index->status);
			if (status == SR_SHUTDOWN ||
			    status == SR_DROP) {
				so_destroy(db->db, 0);
			}
			i++;
		}
		ss_free(r->a, s->i);
		s->i = NULL;
	}
	ss_mutexfree(&s->lock);
	return rcret;
}

int sc_add(sc *s, so *dbo, si *index)
{
	ss_mutexlock(&s->lock);
	int count = s->count + 1;
	scdb *i = ss_malloc(s->r->a, count * sizeof(scdb));
	if (ssunlikely(i == NULL)) {
		ss_mutexunlock(&s->lock);
		return -1;
	}
	memcpy(i, s->i, s->count * sizeof(scdb));
	scdb *db = &i[s->count];
	db->db = dbo;
	db->index = index;
	memset(db->workers, 0, sizeof(db->workers));
	void *iprev = s->i;
	s->i = i;
	s->count = count;
	ss_mutexunlock(&s->lock);
	if (iprev)
		ss_free(s->r->a, iprev);
	return 0;
}

int sc_del(sc *s, so *dbo, int lock)
{
	if (ssunlikely(s->i == NULL))
		return 0;
	if (lock)
		ss_mutexlock(&s->lock);
	int count = s->count - 1;
	if (ssunlikely(count == 0)) {
		s->count = 0;
		ss_free(s->r->a, s->i);
		s->i = NULL;
		ss_mutexunlock(&s->lock);
		return 0;
	}
	scdb *i = ss_malloc(s->r->a, count * sizeof(scdb));
	if (ssunlikely(i == NULL)) {
		ss_mutexunlock(&s->lock);
		return -1;
	}
	int j = 0;
	int k = 0;
	while (j < s->count) {
		if (s->i[j].db == dbo) {
			j++;
			continue;
		}
		i[k] = s->i[j];
		k++;
		j++;
	}
	void *iprev = s->i;
	s->i = i;
	s->count = count;
	if (ssunlikely(s->rr >= s->count))
		s->rr = 0;
	if (lock)
		ss_mutexunlock(&s->lock);
	ss_free(s->r->a, iprev);
	return 0;
}
