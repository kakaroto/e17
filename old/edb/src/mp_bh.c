/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)mp_bh.c	10.45 (Sleepycat) 11/25/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "edb_shash.h"
#include "mp.h"
#include "common_ext.h"

static int __memp_upgrade __P((DB_MPOOL *, DB_MPOOLFILE *, MPOOLFILE *));

/*
 * __memp_bhwrite --
 *	Write the page associated with a given bucket header.
 *
 * PUBLIC: int __memp_bhwrite
 * PUBLIC:     __P((DB_MPOOL *, MPOOLFILE *, BH *, int *, int *));
 */
int
__memp_bhwrite(edbmp, mfp, bhp, restartp, wrotep)
	DB_MPOOL *edbmp;
	MPOOLFILE *mfp;
	BH *bhp;
	int *restartp, *wrotep;
{
	DB_MPOOLFILE *edbmfp;
	DB_MPREG *mpreg;
	int incremented, ret;

	if (restartp != NULL)
		*restartp = 0;
	if (wrotep != NULL)
		*wrotep = 0;
	incremented = 0;

	/*
	 * Walk the process' DB_MPOOLFILE list and find a file descriptor for
	 * the file.  We also check that the descriptor is open for writing.
	 * If we find a descriptor on the file that's not open for writing, we
	 * try and upgrade it to make it writeable.  If that fails, we're done.
	 */
	LOCKHANDLE(edbmp, edbmp->mutexp);
	for (edbmfp = TAILQ_FIRST(&edbmp->edbmfq);
	    edbmfp != NULL; edbmfp = TAILQ_NEXT(edbmfp, q))
		if (edbmfp->mfp == mfp) {
			if (F_ISSET(edbmfp, MP_READONLY) &&
			    __memp_upgrade(edbmp, edbmfp, mfp)) {
				UNLOCKHANDLE(edbmp, edbmp->mutexp);
				return (0);
			}

			/*
			 * Increment the reference count -- see the comment in
			 * memp_fclose().
			 */
			++edbmfp->ref;
			incremented = 1;
			break;
		}
	UNLOCKHANDLE(edbmp, edbmp->mutexp);
	if (edbmfp != NULL)
		goto found;

	/*
	 * It's not a page from a file we've opened.  If the file requires
	 * input/output processing, see if this process has ever registered
	 * information as to how to write this type of file.  If not, there's
	 * nothing we can do.
	 */
	if (mfp->ftype != 0) {
		LOCKHANDLE(edbmp, edbmp->mutexp);
		for (mpreg = LIST_FIRST(&edbmp->edbregq);
		    mpreg != NULL; mpreg = LIST_NEXT(mpreg, q))
			if (mpreg->ftype == mfp->ftype)
				break;
		UNLOCKHANDLE(edbmp, edbmp->mutexp);
		if (mpreg == NULL)
			return (0);
	}

	/*
	 * Try and open the file, attaching to the underlying shared area.
	 *
	 * XXX
	 * Don't try to attach to temporary files.  There are two problems in
	 * trying to do that.  First, if we have different privileges than the
	 * process that "owns" the temporary file, we might create the backing
	 * disk file such that the owning process couldn't read/write its own
	 * buffers, e.g., memp_trickle() running as root creating a file owned
	 * as root, mode 600.  Second, if the temporary file has already been
	 * created, we don't have any way of finding out what its real name is,
	 * and, even if we did, it was already unlinked (so that it won't be
	 * left if the process dies horribly).  This decision causes a problem,
	 * however: if the temporary file consumes the entire buffer cache,
	 * and the owner doesn't flush the buffers to disk, we could end up
	 * with resource starvation, and the memp_trickle() thread couldn't do
	 * anything about it.  That's a pretty unlikely scenario, though.
	 *
	 * XXX
	 * There's no negative cache, so we may repeatedly try and open files
	 * that we have previously tried (and failed) to open.
	 *
	 * Ignore any error, assume it's a permissions problem.
	 */
	if (F_ISSET(mfp, MP_TEMP))
		return (0);

	if (__memp_fopen(edbmp, mfp, R_ADDR(edbmp, mfp->path_off),
	    0, 0, mfp->stat.st_pagesize, 0, NULL, &edbmfp) != 0)
		return (0);

found:	ret = __memp_pgwrite(edbmfp, bhp, restartp, wrotep);

	if (incremented) {
		LOCKHANDLE(edbmp, edbmp->mutexp);
		--edbmfp->ref;
		UNLOCKHANDLE(edbmp, edbmp->mutexp);
	}

	return (ret);
}

/*
 * __memp_pgread --
 *	Read a page from a file.
 *
 * PUBLIC: int __memp_pgread __P((DB_MPOOLFILE *, BH *, int));
 */
int
__memp_pgread(edbmfp, bhp, can_create)
	DB_MPOOLFILE *edbmfp;
	BH *bhp;
	int can_create;
{
	DB_IO edb_io;
	DB_MPOOL *edbmp;
	MPOOLFILE *mfp;
	size_t len, pagesize;
	ssize_t nr;
	int created, ret;

	edbmp = edbmfp->edbmp;
	mfp = edbmfp->mfp;
	pagesize = mfp->stat.st_pagesize;

	F_SET(bhp, BH_LOCKED | BH_TRASH);
	LOCKBUFFER(edbmp, bhp);
	UNLOCKREGION(edbmp);

	/*
	 * Temporary files may not yet have been created.  We don't create
	 * them now, we create them when the pages have to be flushed.
	 */
	nr = 0;
	if (edbmfp->fd == -1)
		ret = 0;
	else {
		/*
		 * Ignore read errors if we have permission to create the page.
		 * Assume that the page doesn't exist, and that we'll create it
		 * when we write it out.
		 */
		edb_io.fd_io = edbmfp->fd;
		edb_io.fd_lock = edbmp->reginfo.fd;
		edb_io.mutexp =
		    F_ISSET(edbmp, MP_LOCKHANDLE) ? edbmfp->mutexp : NULL;
		edb_io.pagesize = edb_io.bytes = pagesize;
		edb_io.pgno = bhp->pgno;
		edb_io.buf = bhp->buf;

		ret = __edb_os_io(&edb_io, DB_IO_READ, &nr);
	}

	created = 0;
	if (nr < (ssize_t)pagesize)
		if (can_create)
			created = 1;
		else {
			/* If we had a short read, ret may be 0. */
			if (ret == 0)
				ret = EIO;
			__edb_err(edbmp->edbenv,
			    "%s: page %lu doesn't exist, create flag not set",
			    __memp_fn(edbmfp), (u_long)bhp->pgno);
			goto err;
		}

	/*
	 * Clear any bytes we didn't read that need to be cleared.  If we're
	 * running in diagnostic mode, smash any bytes on the page that are
	 * unknown quantities for the caller.
	 */
	if (nr != (ssize_t)pagesize) {
		len = mfp->clear_len == 0 ? pagesize : mfp->clear_len;
		if (nr < (ssize_t)len)
			memset(bhp->buf + nr, 0, len - nr);
#ifdef DIAGNOSTIC
		if (nr > (ssize_t)len)
			len = nr;
		if (len < pagesize)
			memset(bhp->buf + len, 0xedb, pagesize - len);
#endif
	}

	/* Call any pgin function. */
	ret = mfp->ftype == 0 ? 0 : __memp_pg(edbmfp, bhp, 1);

	/* Unlock the buffer and reacquire the region lock. */
err:	UNLOCKBUFFER(edbmp, bhp);
	LOCKREGION(edbmp);

	/*
	 * If no errors occurred, the data is now valid, clear the BH_TRASH
	 * flag; regardless, clear the lock bit and let other threads proceed.
	 */
	F_CLR(bhp, BH_LOCKED);
	if (ret == 0) {
		F_CLR(bhp, BH_TRASH);

		/* Update the statistics. */
		if (created) {
			++edbmp->mp->stat.st_page_create;
			++mfp->stat.st_page_create;
		} else {
			++edbmp->mp->stat.st_page_in;
			++mfp->stat.st_page_in;
		}
	}

	return (ret);
}

/*
 * __memp_pgwrite --
 *	Write a page to a file.
 *
 * PUBLIC: int __memp_pgwrite __P((DB_MPOOLFILE *, BH *, int *, int *));
 */
int
__memp_pgwrite(edbmfp, bhp, restartp, wrotep)
	DB_MPOOLFILE *edbmfp;
	BH *bhp;
	int *restartp, *wrotep;
{
	DB_ENV *edbenv;
	DB_IO edb_io;
	DB_LOG *lg_info;
	DB_LSN lsn;
	DB_MPOOL *edbmp;
	MPOOL *mp;
	MPOOLFILE *mfp;
	ssize_t nw;
	int callpgin, dosync, ret, syncfail;
	const char *fail;

	edbmp = edbmfp->edbmp;
	edbenv = edbmp->edbenv;
	mp = edbmp->mp;
	mfp = edbmfp->mfp;

	if (restartp != NULL)
		*restartp = 0;
	if (wrotep != NULL)
		*wrotep = 0;
	callpgin = 0;

	/*
	 * Check the dirty bit -- this buffer may have been written since we
	 * decided to write it.
	 */
	if (!F_ISSET(bhp, BH_DIRTY)) {
		if (wrotep != NULL)
			*wrotep = 1;
		return (0);
	}

	LOCKBUFFER(edbmp, bhp);

	/*
	 * If there were two writers, we may have just been waiting while the
	 * other writer completed I/O on this buffer.  Check the dirty bit one
	 * more time.
	 */
	if (!F_ISSET(bhp, BH_DIRTY)) {
		UNLOCKBUFFER(edbmp, bhp);

		if (wrotep != NULL)
			*wrotep = 1;
		return (0);
	}

	F_SET(bhp, BH_LOCKED);
	UNLOCKREGION(edbmp);

	if (restartp != NULL)
		*restartp = 1;

	/* Copy the LSN off the page if we're going to need it. */
	lg_info = edbenv->lg_info;
	if (lg_info != NULL || F_ISSET(bhp, BH_WRITE))
		memcpy(&lsn, bhp->buf + mfp->lsn_off, sizeof(DB_LSN));

	/* Ensure the appropriate log records are on disk. */
	if (lg_info != NULL && (ret = log_flush(lg_info, &lsn)) != 0)
		goto err;

	/*
	 * Call any pgout function.  We set the callpgin flag so that we flag
	 * that the contents of the buffer will need to be passed through pgin
	 * before they are reused.
	 */
	if (mfp->ftype == 0)
		ret = 0;
	else {
		callpgin = 1;
		if ((ret = __memp_pg(edbmfp, bhp, 0)) != 0)
			goto err;
	}

	/* Temporary files may not yet have been created. */
	if (edbmfp->fd == -1) {
		LOCKHANDLE(edbmp, edbmfp->mutexp);
		if (edbmfp->fd == -1 && ((ret = __edb_appname(edbenv,
		    DB_APP_TMP, NULL, NULL, DB_CREATE | DB_EXCL | DB_TEMPORARY,
		    &edbmfp->fd, NULL)) != 0 || edbmfp->fd == -1)) {
			UNLOCKHANDLE(edbmp, edbmfp->mutexp);
			__edb_err(edbenv,
			    "unable to create temporary backing file");
			goto err;
		}
		UNLOCKHANDLE(edbmp, edbmfp->mutexp);
	}

	/* Write the page. */
	edb_io.fd_io = edbmfp->fd;
	edb_io.fd_lock = edbmp->reginfo.fd;
	edb_io.mutexp = F_ISSET(edbmp, MP_LOCKHANDLE) ? edbmfp->mutexp : NULL;
	edb_io.pagesize = edb_io.bytes = mfp->stat.st_pagesize;
	edb_io.pgno = bhp->pgno;
	edb_io.buf = bhp->buf;
	if ((ret = __edb_os_io(&edb_io, DB_IO_WRITE, &nw)) != 0) {
		__edb_panic(edbenv, ret);
		fail = "write";
		goto syserr;
	}
	if (nw != (ssize_t)mfp->stat.st_pagesize) {
		ret = EIO;
		fail = "write";
		goto syserr;
	}

	if (wrotep != NULL)
		*wrotep = 1;

	/* Unlock the buffer and reacquire the region lock. */
	UNLOCKBUFFER(edbmp, bhp);
	LOCKREGION(edbmp);

	/*
	 * Clean up the flags based on a successful write.
	 *
	 * If we rewrote the page, it will need processing by the pgin
	 * routine before reuse.
	 */
	if (callpgin)
		F_SET(bhp, BH_CALLPGIN);
	F_CLR(bhp, BH_DIRTY | BH_LOCKED);

	/*
	 * If we write a buffer for which a checkpoint is waiting, update
	 * the count of pending buffers (both in the mpool as a whole and
	 * for this file).  If the count for this file goes to zero, set a
	 * flag so we flush the writes.
	 */
	if (F_ISSET(bhp, BH_WRITE)) {
		F_CLR(bhp, BH_WRITE);

		--mp->lsn_cnt;
		dosync = --mfp->lsn_cnt == 0 ? 1 : 0;
	} else
		dosync = 0;

	/* Update the page clean/dirty statistics. */
	++mp->stat.st_page_clean;
	--mp->stat.st_page_dirty;

	/* Update I/O statistics. */
	++mp->stat.st_page_out;
	++mfp->stat.st_page_out;

	/*
	 * Do the sync after everything else has been updated, so any incoming
	 * checkpoint doesn't see inconsistent information.
	 *
	 * XXX:
	 * Don't lock the region around the sync, fsync(2) has no atomicity
	 * issues.
	 *
	 * XXX:
	 * We ignore errors from the sync -- it makes no sense to return an
	 * error to the calling process, so set a flag causing the checkpoint
	 * to be retried later.  There is a possibility, of course, that a
	 * subsequent checkpoint was started and that we're going to force it
	 * to fail.  That should be unlikely, and fixing it would be difficult.
	 */
	if (dosync) {
		UNLOCKREGION(edbmp);
		syncfail = __edb_os_fsync(edbmfp->fd) != 0;
		LOCKREGION(edbmp);
		if (syncfail)
			F_SET(mp, MP_LSN_RETRY);
	}

	return (0);

syserr:	__edb_err(edbenv, "%s: %s failed for page %lu",
	    __memp_fn(edbmfp), fail, (u_long)bhp->pgno);

err:	/* Unlock the buffer and reacquire the region lock. */
	UNLOCKBUFFER(edbmp, bhp);
	LOCKREGION(edbmp);

	/*
	 * Clean up the flags based on a failure.
	 *
	 * The page remains dirty but we remove our lock.  If we rewrote the
	 * page, it will need processing by the pgin routine before reuse.
	 */
	if (callpgin)
		F_SET(bhp, BH_CALLPGIN);
	F_CLR(bhp, BH_LOCKED);

	return (ret);
}

/*
 * __memp_pg --
 *	Call the pgin/pgout routine.
 *
 * PUBLIC: int __memp_pg __P((DB_MPOOLFILE *, BH *, int));
 */
int
__memp_pg(edbmfp, bhp, is_pgin)
	DB_MPOOLFILE *edbmfp;
	BH *bhp;
	int is_pgin;
{
	DBT edbt, *edbtp;
	DB_MPOOL *edbmp;
	DB_MPREG *mpreg;
	MPOOLFILE *mfp;
	int ftype, ret;

	edbmp = edbmfp->edbmp;
	mfp = edbmfp->mfp;

	LOCKHANDLE(edbmp, edbmp->mutexp);

	ftype = mfp->ftype;
	for (mpreg = LIST_FIRST(&edbmp->edbregq);
	    mpreg != NULL; mpreg = LIST_NEXT(mpreg, q)) {
		if (ftype != mpreg->ftype)
			continue;
		if (mfp->pgcookie_len == 0)
			edbtp = NULL;
		else {
			edbt.size = mfp->pgcookie_len;
			edbt.data = R_ADDR(edbmp, mfp->pgcookie_off);
			edbtp = &edbt;
		}
		UNLOCKHANDLE(edbmp, edbmp->mutexp);

		if (is_pgin) {
			if (mpreg->pgin != NULL && (ret =
			    mpreg->pgin(bhp->pgno, bhp->buf, edbtp)) != 0)
				goto err;
		} else
			if (mpreg->pgout != NULL && (ret =
			    mpreg->pgout(bhp->pgno, bhp->buf, edbtp)) != 0)
				goto err;
		break;
	}

	if (mpreg == NULL)
		UNLOCKHANDLE(edbmp, edbmp->mutexp);

	return (0);

err:	UNLOCKHANDLE(edbmp, edbmp->mutexp);
	__edb_err(edbmp->edbenv, "%s: %s failed for page %lu",
	    __memp_fn(edbmfp), is_pgin ? "pgin" : "pgout", (u_long)bhp->pgno);
	return (ret);
}

/*
 * __memp_bhfree --
 *	Free a bucket header and its referenced data.
 *
 * PUBLIC: void __memp_bhfree __P((DB_MPOOL *, MPOOLFILE *, BH *, int));
 */
void
__memp_bhfree(edbmp, mfp, bhp, free_mem)
	DB_MPOOL *edbmp;
	MPOOLFILE *mfp;
	BH *bhp;
	int free_mem;
{
	size_t off;

	/* Delete the buffer header from the hash bucket queue. */
	off = BUCKET(edbmp->mp, R_OFFSET(edbmp, mfp), bhp->pgno);
	SH_TAILQ_REMOVE(&edbmp->htab[off], bhp, hq, __bh);

	/* Delete the buffer header from the LRU queue. */
	SH_TAILQ_REMOVE(&edbmp->mp->bhq, bhp, q, __bh);

	/*
	 * If we're not reusing it immediately, free the buffer header
	 * and data for real.
	 */
	if (free_mem) {
		__edb_shalloc_free(edbmp->addr, bhp);
		--edbmp->mp->stat.st_page_clean;
	}
}

/*
 * __memp_upgrade --
 *	Upgrade a file descriptor from readonly to readwrite.
 */
static int
__memp_upgrade(edbmp, edbmfp, mfp)
	DB_MPOOL *edbmp;
	DB_MPOOLFILE *edbmfp;
	MPOOLFILE *mfp;
{
	int fd, ret;
	char *rpath;

	/*
	 * !!!
	 * We expect the handle to already be locked.
	 */

	/* Check to see if we've already upgraded. */
	if (F_ISSET(edbmfp, MP_UPGRADE))
		return (0);

	/* Check to see if we've already failed. */
	if (F_ISSET(edbmfp, MP_UPGRADE_FAIL))
		return (1);

	/*
	 * Calculate the real name for this file and try to open it read/write.
	 * We know we have a valid pathname for the file because it's the only
	 * way we could have gotten a file descriptor of any kind.
	 */
	if ((ret = __edb_appname(edbmp->edbenv, DB_APP_DATA,
	    NULL, R_ADDR(edbmp, mfp->path_off), 0, NULL, &rpath)) != 0)
		return (ret);
	if (__edb_open(rpath, 0, 0, 0, &fd) != 0) {
		F_SET(edbmfp, MP_UPGRADE_FAIL);
		ret = 1;
	} else {
		/* Swap the descriptors and set the upgrade flag. */
		(void)__edb_os_close(edbmfp->fd);
		edbmfp->fd = fd;
		F_SET(edbmfp, MP_UPGRADE);
		ret = 0;
	}
	__edb_os_freestr(rpath);
	return (ret);
}
