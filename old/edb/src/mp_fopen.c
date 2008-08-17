/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)mp_fopen.c	10.60 (Sleepycat) 1/1/99";
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

static int __memp_mf_close __P((DB_MPOOL *, DB_MPOOLFILE *));
static int __memp_mf_open __P((DB_MPOOL *,
    const char *, size_t, edb_pgno_t, DB_MPOOL_FINFO *, MPOOLFILE **));

/*
 * memp_fopen --
 *	Open a backing file for the memory pool.
 */
int
memp_fopen(edbmp, path, flags, mode, pagesize, finfop, retp)
	DB_MPOOL *edbmp;
	const char *path;
	u_int32_t flags;
	int mode;
	size_t pagesize;
	DB_MPOOL_FINFO *finfop;
	DB_MPOOLFILE **retp;
{
	int ret;

	MP_PANIC_CHECK(edbmp);

	/* Validate arguments. */
	if ((ret = __edb_fchk(edbmp->edbenv,
	    "memp_fopen", flags, DB_CREATE | DB_NOMMAP | DB_RDONLY)) != 0)
		return (ret);

	/* Require a non-zero pagesize. */
	if (pagesize == 0) {
		__edb_err(edbmp->edbenv, "memp_fopen: pagesize not specified");
		return (EINVAL);
	}
	if (finfop != NULL && finfop->clear_len > pagesize)
		return (EINVAL);

	return (__memp_fopen(edbmp,
	    NULL, path, flags, mode, pagesize, 1, finfop, retp));
}

/*
 * __memp_fopen --
 *	Open a backing file for the memory pool; internal version.
 *
 * PUBLIC: int __memp_fopen __P((DB_MPOOL *, MPOOLFILE *, const char *,
 * PUBLIC:    u_int32_t, int, size_t, int, DB_MPOOL_FINFO *, DB_MPOOLFILE **));
 */
int
__memp_fopen(edbmp, mfp, path, flags, mode, pagesize, needlock, finfop, retp)
	DB_MPOOL *edbmp;
	MPOOLFILE *mfp;
	const char *path;
	u_int32_t flags;
	int mode, needlock;
	size_t pagesize;
	DB_MPOOL_FINFO *finfop;
	DB_MPOOLFILE **retp;
{
	DB_ENV *edbenv;
	DB_MPOOLFILE *edbmfp;
	DB_MPOOL_FINFO finfo;
	edb_pgno_t last_pgno;
	size_t maxmap;
	u_int32_t mbytes, bytes;
	int ret;
	u_int8_t iedbuf[DB_FILE_ID_LEN];
	char *rpath;

	edbenv = edbmp->edbenv;
	ret = 0;
	rpath = NULL;

	/*
	 * If mfp is provided, we take the DB_MPOOL_FINFO information from
	 * the mfp.  We don't bother initializing everything, because some
	 * of them are expensive to acquire.  If no mfp is provided and the
	 * finfop argument is NULL, we default the values.
	 */
	if (finfop == NULL) {
		memset(&finfo, 0, sizeof(finfo));
		if (mfp != NULL) {
			finfo.ftype = mfp->ftype;
			finfo.pgcookie = NULL;
			finfo.fileid = NULL;
			finfo.lsn_offset = mfp->lsn_off;
			finfo.clear_len = mfp->clear_len;
		} else {
			finfo.ftype = 0;
			finfo.pgcookie = NULL;
			finfo.fileid = NULL;
			finfo.lsn_offset = -1;
			finfo.clear_len = 0;
		}
		finfop = &finfo;
	}

	/* Allocate and initialize the per-process structure. */
	if ((ret = __edb_os_calloc(1, sizeof(DB_MPOOLFILE), &edbmfp)) != 0)
		return (ret);
	edbmfp->edbmp = edbmp;
	edbmfp->fd = -1;
	edbmfp->ref = 1;
	if (LF_ISSET(DB_RDONLY))
		F_SET(edbmfp, MP_READONLY);

	if (path == NULL) {
		if (LF_ISSET(DB_RDONLY)) {
			__edb_err(edbenv,
			    "memp_fopen: temporary files can't be readonly");
			ret = EINVAL;
			goto err;
		}
		last_pgno = 0;
	} else {
		/* Get the real name for this file and open it. */
		if ((ret = __edb_appname(edbenv,
		    DB_APP_DATA, NULL, path, 0, NULL, &rpath)) != 0)
			goto err;
		if ((ret = __edb_open(rpath,
		   LF_ISSET(DB_CREATE | DB_RDONLY),
		   DB_CREATE | DB_RDONLY, mode, &edbmfp->fd)) != 0) {
			__edb_err(edbenv, "%s: %s", rpath, strerror(ret));
			goto err;
		}

		/*
		 * Don't permit files that aren't a multiple of the pagesize,
		 * and find the number of the last page in the file, all the
		 * time being careful not to overflow 32 bits.
		 *
		 * !!!
		 * We can't use off_t's here, or in any code in the mainline
		 * library for that matter.  (We have to use them in the os
		 * stubs, of course, as there are system calls that take them
		 * as arguments.)  The reason is that some customers build in
		 * environments where an off_t is 32-bits, but still run where
		 * offsets are 64-bits, and they pay us a lot of money.
		 */
		if ((ret = __edb_os_ioinfo(rpath,
		    edbmfp->fd, &mbytes, &bytes, NULL)) != 0) {
			__edb_err(edbenv, "%s: %s", rpath, strerror(ret));
			goto err;
		}

		/* Page sizes have to be a power-of-two, ignore mbytes. */
		if (bytes % pagesize != 0) {
			__edb_err(edbenv,
			    "%s: file size not a multiple of the pagesize",
			    rpath);
			ret = EINVAL;
			goto err;
		}

		last_pgno = mbytes * (MEGABYTE / pagesize);
		last_pgno += bytes / pagesize;

		/* Correction: page numbers are zero-based, not 1-based. */
		if (last_pgno != 0)
			--last_pgno;

		/*
		 * Get the file id if we weren't given one.  Generated file id's
		 * don't use timestamps, otherwise there'd be no chance of any
		 * other process joining the party.
		 */
		if (finfop->fileid == NULL) {
			if ((ret = __edb_os_fileid(edbenv, rpath, 0, iedbuf)) != 0)
				goto err;
			finfop->fileid = iedbuf;
		}
	}

	/*
	 * If we weren't provided an underlying shared object to join with,
	 * find/allocate the shared file objects.  Also allocate space for
	 * for the per-process thread lock.
	 */
	if (needlock)
		LOCKREGION(edbmp);

	if (mfp == NULL)
		ret = __memp_mf_open(edbmp,
		    path, pagesize, last_pgno, finfop, &mfp);
	else {
		++mfp->ref;
		ret = 0;
	}
	if (ret == 0 &&
	    F_ISSET(edbmp, MP_LOCKHANDLE) && (ret =
	    __memp_alloc(edbmp, sizeof(edb_mutex_t), NULL, &edbmfp->mutexp)) == 0)
		LOCKINIT(edbmp, edbmfp->mutexp);

	if (needlock)
		UNLOCKREGION(edbmp);
	if (ret != 0)
		goto err;

	edbmfp->mfp = mfp;

	/*
	 * If a file:
	 *	+ is read-only
	 *	+ isn't temporary
	 *	+ doesn't require any pgin/pgout support
	 *	+ the DB_NOMMAP flag wasn't set
	 *	+ and is less than mp_mmapsize bytes in size
	 *
	 * we can mmap it instead of reading/writing buffers.  Don't do error
	 * checking based on the mmap call failure.  We want to do normal I/O
	 * on the file if the reason we failed was because the file was on an
	 * NFS mounted partition, and we can fail in buffer I/O just as easily
	 * as here.
	 *
	 * XXX
	 * We'd like to test to see if the file is too big to mmap.  Since we
	 * don't know what size or type off_t's or size_t's are, or the largest
	 * unsigned integral type is, or what random insanity the local C
	 * compiler will perpetrate, doing the comparison in a portable way is
	 * flatly impossible.  Hope that mmap fails if the file is too large.
	 */
#define	DB_MAXMMAPSIZE	(10 * 1024 * 1024)	/* 10 Mb. */
	if (F_ISSET(mfp, MP_CAN_MMAP)) {
		if (!F_ISSET(edbmfp, MP_READONLY))
			F_CLR(mfp, MP_CAN_MMAP);
		if (path == NULL)
			F_CLR(mfp, MP_CAN_MMAP);
		if (finfop->ftype != 0)
			F_CLR(mfp, MP_CAN_MMAP);
		if (LF_ISSET(DB_NOMMAP))
			F_CLR(mfp, MP_CAN_MMAP);
		maxmap = edbenv == NULL || edbenv->mp_mmapsize == 0 ?
		    DB_MAXMMAPSIZE : edbenv->mp_mmapsize;
		if (mbytes > maxmap / MEGABYTE ||
		    (mbytes == maxmap / MEGABYTE && bytes >= maxmap % MEGABYTE))
			F_CLR(mfp, MP_CAN_MMAP);
	}
	edbmfp->addr = NULL;
	if (F_ISSET(mfp, MP_CAN_MMAP)) {
		edbmfp->len = (size_t)mbytes * MEGABYTE + bytes;
		if (__edb_mapfile(rpath,
		    edbmfp->fd, edbmfp->len, 1, &edbmfp->addr) != 0) {
			edbmfp->addr = NULL;
			F_CLR(mfp, MP_CAN_MMAP);
		}
	}
	if (rpath != NULL)
		__edb_os_freestr(rpath);

	LOCKHANDLE(edbmp, edbmp->mutexp);
	TAILQ_INSERT_TAIL(&edbmp->edbmfq, edbmfp, q);
	UNLOCKHANDLE(edbmp, edbmp->mutexp);

	*retp = edbmfp;
	return (0);

err:	/*
	 * Note that we do not have to free the thread mutex, because we
	 * never get to here after we have successfully allocated it.
	 */
	if (rpath != NULL)
		__edb_os_freestr(rpath);
	if (edbmfp->fd != -1)
		(void)__edb_os_close(edbmfp->fd);
	if (edbmfp != NULL)
		__edb_os_free(edbmfp, sizeof(DB_MPOOLFILE));
	return (ret);
}

/*
 * __memp_mf_open --
 *	Open an MPOOLFILE.
 */
static int
__memp_mf_open(edbmp, path, pagesize, last_pgno, finfop, retp)
	DB_MPOOL *edbmp;
	const char *path;
	size_t pagesize;
	edb_pgno_t last_pgno;
	DB_MPOOL_FINFO *finfop;
	MPOOLFILE **retp;
{
	MPOOLFILE *mfp;
	int ret;
	void *p;

#define	ISTEMPORARY	(path == NULL)

	/*
	 * Walk the list of MPOOLFILE's, looking for a matching file.
	 * Temporary files can't match previous files.
	 */
	if (!ISTEMPORARY)
		for (mfp = SH_TAILQ_FIRST(&edbmp->mp->mpfq, __mpoolfile);
		    mfp != NULL; mfp = SH_TAILQ_NEXT(mfp, q, __mpoolfile)) {
			if (F_ISSET(mfp, MP_TEMP))
				continue;
			if (!memcmp(finfop->fileid,
			    R_ADDR(edbmp, mfp->fileid_off), DB_FILE_ID_LEN)) {
				if (finfop->clear_len != mfp->clear_len ||
				    finfop->ftype != mfp->ftype ||
				    pagesize != mfp->stat.st_pagesize) {
					__edb_err(edbmp->edbenv,
			    "%s: ftype, clear length or pagesize changed",
					    path);
					return (EINVAL);
				}

				/* Found it: increment the reference count. */
				++mfp->ref;
				*retp = mfp;
				return (0);
			}
		}

	/* Allocate a new MPOOLFILE. */
	if ((ret = __memp_alloc(edbmp, sizeof(MPOOLFILE), NULL, &mfp)) != 0)
		return (ret);
	*retp = mfp;

	/* Initialize the structure. */
	memset(mfp, 0, sizeof(MPOOLFILE));
	mfp->ref = 1;
	mfp->ftype = finfop->ftype;
	mfp->lsn_off = finfop->lsn_offset;
	mfp->clear_len = finfop->clear_len;

	/*
	 * If the user specifies DB_MPOOL_LAST or DB_MPOOL_NEW on a memp_fget,
	 * we have to know the last page in the file.  Figure it out and save
	 * it away.
	 */
	mfp->stat.st_pagesize = pagesize;
	mfp->orig_last_pgno = mfp->last_pgno = last_pgno;

	if (ISTEMPORARY)
		F_SET(mfp, MP_TEMP);
	else {
		/* Copy the file path into shared memory. */
		if ((ret = __memp_alloc(edbmp,
		    strlen(path) + 1, &mfp->path_off, &p)) != 0)
			goto err;
		memcpy(p, path, strlen(path) + 1);

		/* Copy the file identification string into shared memory. */
		if ((ret = __memp_alloc(edbmp,
		    DB_FILE_ID_LEN, &mfp->fileid_off, &p)) != 0)
			goto err;
		memcpy(p, finfop->fileid, DB_FILE_ID_LEN);

		F_SET(mfp, MP_CAN_MMAP);
	}

	/* Copy the page cookie into shared memory. */
	if (finfop->pgcookie == NULL || finfop->pgcookie->size == 0) {
		mfp->pgcookie_len = 0;
		mfp->pgcookie_off = 0;
	} else {
		if ((ret = __memp_alloc(edbmp,
		    finfop->pgcookie->size, &mfp->pgcookie_off, &p)) != 0)
			goto err;
		memcpy(p, finfop->pgcookie->data, finfop->pgcookie->size);
		mfp->pgcookie_len = finfop->pgcookie->size;
	}

	/* Prepend the MPOOLFILE to the list of MPOOLFILE's. */
	SH_TAILQ_INSERT_HEAD(&edbmp->mp->mpfq, mfp, q, __mpoolfile);

	if (0) {
err:		if (mfp->path_off != 0)
			__edb_shalloc_free(edbmp->addr,
			    R_ADDR(edbmp, mfp->path_off));
		if (mfp->fileid_off != 0)
			__edb_shalloc_free(edbmp->addr,
			    R_ADDR(edbmp, mfp->fileid_off));
		if (mfp != NULL)
			__edb_shalloc_free(edbmp->addr, mfp);
		mfp = NULL;
	}
	return (0);
}

/*
 * memp_fclose --
 *	Close a backing file for the memory pool.
 */
int
memp_fclose(edbmfp)
	DB_MPOOLFILE *edbmfp;
{
	DB_MPOOL *edbmp;
	int ret, t_ret;

	edbmp = edbmfp->edbmp;
	ret = 0;

	MP_PANIC_CHECK(edbmp);

	for (;;) {
		LOCKHANDLE(edbmp, edbmp->mutexp);

		/*
		 * We have to reference count DB_MPOOLFILE structures as other
		 * threads may be using them.  The problem only happens if the
		 * application makes a bad design choice.  Here's the path:
		 *
		 * Thread A opens a database.
		 * Thread B uses thread A's DB_MPOOLFILE to write a buffer
		 *    in order to free up memory in the mpool cache.
		 * Thread A closes the database while thread B is using the
		 *    DB_MPOOLFILE structure.
		 *
		 * By opening all databases before creating the threads, and
		 * closing them after the threads have exited, applications
		 * get better performance and avoid the problem path entirely.
		 *
		 * Regardless, holding the DB_MPOOLFILE to flush a dirty buffer
		 * is a short-term lock, even in worst case, since we better be
		 * the only thread of control using the DB_MPOOLFILE structure
		 * to read pages *into* the cache.  Wait until we're the only
		 * reference holder and remove the DB_MPOOLFILE structure from
		 * the list, so nobody else can even find it.
		 */
		if (edbmfp->ref == 1) {
			TAILQ_REMOVE(&edbmp->edbmfq, edbmfp, q);
			break;
		}
		UNLOCKHANDLE(edbmp, edbmp->mutexp);

		(void)__edb_os_sleep(1, 0);
	}
	UNLOCKHANDLE(edbmp, edbmp->mutexp);

	/* Complain if pinned blocks never returned. */
	if (edbmfp->pinref != 0)
		__edb_err(edbmp->edbenv, "%s: close: %lu blocks left pinned",
		    __memp_fn(edbmfp), (u_long)edbmfp->pinref);

	/* Close the underlying MPOOLFILE. */
	(void)__memp_mf_close(edbmp, edbmfp);

	/* Discard any mmap information. */
	if (edbmfp->addr != NULL &&
	    (ret = __edb_unmapfile(edbmfp->addr, edbmfp->len)) != 0)
		__edb_err(edbmp->edbenv,
		    "%s: %s", __memp_fn(edbmfp), strerror(ret));

	/* Close the file; temporary files may not yet have been created. */
	if (edbmfp->fd != -1 && (t_ret = __edb_os_close(edbmfp->fd)) != 0) {
		__edb_err(edbmp->edbenv,
		    "%s: %s", __memp_fn(edbmfp), strerror(t_ret));
		if (ret != 0)
			t_ret = ret;
	}

	/* Free memory. */
	if (edbmfp->mutexp != NULL) {
		LOCKREGION(edbmp);
		__edb_shalloc_free(edbmp->addr, edbmfp->mutexp);
		UNLOCKREGION(edbmp);
	}

	/* Discard the DB_MPOOLFILE structure. */
	__edb_os_free(edbmfp, sizeof(DB_MPOOLFILE));

	return (ret);
}

/*
 * __memp_mf_close --
 *	Close down an MPOOLFILE.
 */
static int
__memp_mf_close(edbmp, edbmfp)
	DB_MPOOL *edbmp;
	DB_MPOOLFILE *edbmfp;
{
	BH *bhp, *nbhp;
	MPOOL *mp;
	MPOOLFILE *mfp;
	size_t mf_offset;

	mp = edbmp->mp;
	mfp = edbmfp->mfp;

	LOCKREGION(edbmp);

	/* If more than a single reference, simply decrement. */
	if (mfp->ref > 1) {
		--mfp->ref;
		goto ret1;
	}

	/*
	 * Move any BH's held by the file to the free list.  We don't free the
	 * memory itself because we may be discarding the memory pool, and it's
	 * fairly expensive to reintegrate the buffers back into the region for
	 * no purpose.
	 */
	mf_offset = R_OFFSET(edbmp, mfp);
	for (bhp = SH_TAILQ_FIRST(&mp->bhq, __bh); bhp != NULL; bhp = nbhp) {
		nbhp = SH_TAILQ_NEXT(bhp, q, __bh);

#ifdef DEBUG_NO_DIRTY
		/* Complain if we find any blocks that were left dirty. */
		if (F_ISSET(bhp, BH_DIRTY))
			__edb_err(edbmp->edbenv,
			    "%s: close: pgno %lu left dirty; ref %lu",
			    __memp_fn(edbmfp),
			    (u_long)bhp->pgno, (u_long)bhp->ref);
#endif

		if (bhp->mf_offset == mf_offset) {
			if (F_ISSET(bhp, BH_DIRTY)) {
				++mp->stat.st_page_clean;
				--mp->stat.st_page_dirty;
			}
			__memp_bhfree(edbmp, mfp, bhp, 0);
			SH_TAILQ_INSERT_HEAD(&mp->bhfq, bhp, q, __bh);
		}
	}

	/* Delete from the list of MPOOLFILEs. */
	SH_TAILQ_REMOVE(&mp->mpfq, mfp, q, __mpoolfile);

	/* Free the space. */
	if (mfp->path_off != 0)
		__edb_shalloc_free(edbmp->addr, R_ADDR(edbmp, mfp->path_off));
	if (mfp->fileid_off != 0)
		__edb_shalloc_free(edbmp->addr, R_ADDR(edbmp, mfp->fileid_off));
	if (mfp->pgcookie_off != 0)
		__edb_shalloc_free(edbmp->addr, R_ADDR(edbmp, mfp->pgcookie_off));
	__edb_shalloc_free(edbmp->addr, mfp);

ret1:	UNLOCKREGION(edbmp);
	return (0);
}
