/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)mp_region.c	10.35 (Sleepycat) 12/11/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "edb_shash.h"
#include "mp.h"
#include "common_ext.h"

/*
 * __memp_reg_alloc --
 *	Allocate some space in the mpool region, with locking.
 *
 * PUBLIC: int __memp_reg_alloc __P((DB_MPOOL *, size_t, size_t *, void *));
 */
int
__memp_reg_alloc(edbmp, len, offsetp, retp)
	DB_MPOOL *edbmp;
	size_t len, *offsetp;
	void *retp;
{
	int ret;

	LOCKREGION(edbmp);
	ret = __memp_alloc(edbmp, len, offsetp, retp);
	UNLOCKREGION(edbmp);
	return (ret);
}

/*
 * __memp_alloc --
 *	Allocate some space in the mpool region.
 *
 * PUBLIC: int __memp_alloc __P((DB_MPOOL *, size_t, size_t *, void *));
 */
int
__memp_alloc(edbmp, len, offsetp, retp)
	DB_MPOOL *edbmp;
	size_t len, *offsetp;
	void *retp;
{
	BH *bhp, *nbhp;
	MPOOL *mp;
	MPOOLFILE *mfp;
	size_t fsize, total;
	int nomore, restart, ret, wrote;
	void *p;

	mp = edbmp->mp;

	nomore = 0;
alloc:	if ((ret = __edb_shalloc(edbmp->addr, len, MUTEX_ALIGNMENT, &p)) == 0) {
		if (offsetp != NULL)
			*offsetp = R_OFFSET(edbmp, p);
		*(void **)retp = p;
		return (0);
	}
	if (nomore) {
		__edb_err(edbmp->edbenv,
	    "Unable to allocate %lu bytes from mpool shared region: %s\n",
		    (u_long)len, strerror(ret));
		return (ret);
	}

	/* Look for a buffer on the free list that's the right size. */
	for (bhp =
	    SH_TAILQ_FIRST(&mp->bhfq, __bh); bhp != NULL; bhp = nbhp) {
		nbhp = SH_TAILQ_NEXT(bhp, q, __bh);

		if (__edb_shsizeof(bhp) == len) {
			SH_TAILQ_REMOVE(&mp->bhfq, bhp, q, __bh);
			if (offsetp != NULL)
				*offsetp = R_OFFSET(edbmp, bhp);
			*(void **)retp = bhp;
			return (0);
		}
	}

	/* Discard from the free list until we've freed enough memory. */
	total = 0;
	for (bhp =
	    SH_TAILQ_FIRST(&mp->bhfq, __bh); bhp != NULL; bhp = nbhp) {
		nbhp = SH_TAILQ_NEXT(bhp, q, __bh);

		SH_TAILQ_REMOVE(&mp->bhfq, bhp, q, __bh);
		__edb_shalloc_free(edbmp->addr, bhp);
		--mp->stat.st_page_clean;

		/*
		 * Retry as soon as we've freed up sufficient space.  If we
		 * will have to coalesce memory to satisfy the request, don't
		 * try until it's likely (possible?) that we'll succeed.
		 */
		total += fsize = __edb_shsizeof(bhp);
		if (fsize >= len || total >= 3 * len)
			goto alloc;
	}

retry:	/* Find a buffer we can flush; pure LRU. */
	restart = total = 0;
	for (bhp =
	    SH_TAILQ_FIRST(&mp->bhq, __bh); bhp != NULL; bhp = nbhp) {
		nbhp = SH_TAILQ_NEXT(bhp, q, __bh);

		/* Ignore pinned or locked (I/O in progress) buffers. */
		if (bhp->ref != 0 || F_ISSET(bhp, BH_LOCKED))
			continue;

		/* Find the associated MPOOLFILE. */
		mfp = R_ADDR(edbmp, bhp->mf_offset);

		/*
		 * Write the page if it's dirty.
		 *
		 * If we wrote the page, fall through and free the buffer.  We
		 * don't have to rewalk the list to acquire the buffer because
		 * it was never available for any other process to modify it.
		 * If we didn't write the page, but we discarded and reacquired
		 * the region lock, restart the buffer list walk.  If we neither
		 * wrote the buffer nor discarded the region lock, continue down
		 * the buffer list.
		 */
		if (F_ISSET(bhp, BH_DIRTY)) {
			++bhp->ref;
			if ((ret = __memp_bhwrite(edbmp,
			    mfp, bhp, &restart, &wrote)) != 0)
				return (ret);
			--bhp->ref;

			/*
			 * It's possible that another process wants this buffer
			 * and incremented the ref count while we were writing
			 * it.
			 */
			if (bhp->ref != 0)
				goto retry;

			if (wrote)
				++mp->stat.st_rw_evict;
			else {
				if (restart)
					goto retry;
				continue;
			}
		} else
			++mp->stat.st_ro_evict;

		/*
		 * Check to see if the buffer is the size we're looking for.
		 * If it is, simply reuse it.
		 */
		total += fsize = __edb_shsizeof(bhp);
		if (fsize == len) {
			__memp_bhfree(edbmp, mfp, bhp, 0);

			if (offsetp != NULL)
				*offsetp = R_OFFSET(edbmp, bhp);
			*(void **)retp = bhp;
			return (0);
		}

		/* Free the buffer. */
		__memp_bhfree(edbmp, mfp, bhp, 1);

		/*
		 * Retry as soon as we've freed up sufficient space.  If we
		 * have to coalesce of memory to satisfy the request, don't
		 * try until it's likely (possible?) that we'll succeed.
		 */
		if (fsize >= len || total >= 3 * len)
			goto alloc;

		/* Restart the walk if we discarded the region lock. */
		if (restart)
			goto retry;
	}
	nomore = 1;
	goto alloc;
}

/*
 * __memp_ropen --
 *	Attach to, and optionally create, the mpool region.
 *
 * PUBLIC: int __memp_ropen
 * PUBLIC:    __P((DB_MPOOL *, const char *, size_t, int, int, u_int32_t));
 */
int
__memp_ropen(edbmp, path, cachesize, mode, is_private, flags)
	DB_MPOOL *edbmp;
	const char *path;
	size_t cachesize;
	int mode, is_private;
	u_int32_t flags;
{
	MPOOL *mp;
	size_t rlen;
	int defcache, ret;

	/*
	 * Unlike other DB subsystems, mpool can't simply grow the region
	 * because it returns pointers into the region to its clients.  To
	 * "grow" the region, we'd have to allocate a new region and then
	 * store a region number in the structures that reference regional
	 * objects.  It's reasonable that we fail regardless, as clients
	 * shouldn't have every page in the region pinned, so the only
	 * "failure" mode should be a performance penalty because we don't
	 * find a page in the cache that we'd like to have found.
	 *
	 * Up the user's cachesize by 25% to account for our overhead.
	 */
	defcache = 0;
	if (cachesize < DB_CACHESIZE_MIN)
		if (cachesize == 0) {
			defcache = 1;
			cachesize = DB_CACHESIZE_DEF;
		} else
			cachesize = DB_CACHESIZE_MIN;
	rlen = cachesize + cachesize / 4;

	/*
	 * Map in the region.
	 *
	 * If it's a private mpool, use malloc, it's a lot faster than
	 * instantiating a region.
	 */
	edbmp->reginfo.edbenv = edbmp->edbenv;
	edbmp->reginfo.appname = DB_APP_NONE;
	if (path == NULL)
		edbmp->reginfo.path = NULL;
	else
		if ((ret = __edb_os_strdup(path, &edbmp->reginfo.path)) != 0)
			return (ret);
	edbmp->reginfo.file = DB_DEFAULT_MPOOL_FILE;
	edbmp->reginfo.mode = mode;
	edbmp->reginfo.size = rlen;
	edbmp->reginfo.edbflags = flags;
	edbmp->reginfo.flags = 0;
	if (defcache)
		F_SET(&edbmp->reginfo, REGION_SIZEDEF);

	/*
	 * If we're creating a temporary region, don't use any standard
	 * naming.
	 */
	if (is_private) {
		edbmp->reginfo.appname = DB_APP_TMP;
		edbmp->reginfo.file = NULL;
		F_SET(&edbmp->reginfo, REGION_PRIVATE);
	}

	if ((ret = __edb_rattach(&edbmp->reginfo)) != 0) {
		if (edbmp->reginfo.path != NULL)
			__edb_os_freestr(edbmp->reginfo.path);
		return (ret);
	}

	/*
	 * The MPOOL structure is first in the region, the rest of the region
	 * is free space.
	 */
	edbmp->mp = edbmp->reginfo.addr;
	edbmp->addr = (u_int8_t *)edbmp->mp + sizeof(MPOOL);

	/* Initialize a created region. */
	if (F_ISSET(&edbmp->reginfo, REGION_CREATED)) {
		mp = edbmp->mp;
		SH_TAILQ_INIT(&mp->bhq);
		SH_TAILQ_INIT(&mp->bhfq);
		SH_TAILQ_INIT(&mp->mpfq);

		__edb_shalloc_init(edbmp->addr, rlen - sizeof(MPOOL));

		/*
		 * Assume we want to keep the hash chains with under 10 pages
		 * on each chain.  We don't know the pagesize in advance, and
		 * it may differ for different files.  Use a pagesize of 1K for
		 * the calculation -- we walk these chains a lot, they should
		 * be short.
		 */
		mp->htab_buckets =
		    __edb_tablesize((cachesize / (1 * 1024)) / 10);

		/* Allocate hash table space and initialize it. */
		if ((ret = __edb_shalloc(edbmp->addr,
		    mp->htab_buckets * sizeof(DB_HASHTAB),
		    0, &edbmp->htab)) != 0)
			goto err;
		__edb_hashinit(edbmp->htab, mp->htab_buckets);
		mp->htab = R_OFFSET(edbmp, edbmp->htab);

		ZERO_LSN(mp->lsn);
		mp->lsn_cnt = 0;

		memset(&mp->stat, 0, sizeof(mp->stat));
		mp->stat.st_cachesize = cachesize;

		mp->flags = 0;
	}

	/* Get the local hash table address. */
	edbmp->htab = R_ADDR(edbmp, edbmp->mp->htab);

	UNLOCKREGION(edbmp);
	return (0);

err:	UNLOCKREGION(edbmp);
	(void)__edb_rdetach(&edbmp->reginfo);
	if (F_ISSET(&edbmp->reginfo, REGION_CREATED))
		(void)memp_unlink(path, 1, edbmp->edbenv);

	if (edbmp->reginfo.path != NULL)
		__edb_os_freestr(edbmp->reginfo.path);
	return (ret);
}
