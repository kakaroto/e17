/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)mp_fput.c	10.24 (Sleepycat) 9/27/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "edb_shash.h"
#include "mp.h"
#include "common_ext.h"

/*
 * memp_fput --
 *	Mpool file put function.
 */
int
memp_fput(edbmfp, pgaddr, flags)
	DB_MPOOLFILE *edbmfp;
	void *pgaddr;
	u_int32_t flags;
{
	BH *bhp;
	DB_MPOOL *edbmp;
	MPOOL *mp;
	int wrote, ret;

	edbmp = edbmfp->edbmp;
	mp = edbmp->mp;

	MP_PANIC_CHECK(edbmp);

	/* Validate arguments. */
	if (flags) {
		if ((ret = __edb_fchk(edbmp->edbenv, "memp_fput", flags,
		    DB_MPOOL_CLEAN | DB_MPOOL_DIRTY | DB_MPOOL_DISCARD)) != 0)
			return (ret);
		if ((ret = __edb_fcchk(edbmp->edbenv, "memp_fput",
		    flags, DB_MPOOL_CLEAN, DB_MPOOL_DIRTY)) != 0)
			return (ret);

		if (LF_ISSET(DB_MPOOL_DIRTY) && F_ISSET(edbmfp, MP_READONLY)) {
			__edb_err(edbmp->edbenv,
			    "%s: dirty flag set for readonly file page",
			    __memp_fn(edbmfp));
			return (EACCES);
		}
	}

	LOCKREGION(edbmp);

	/* Decrement the pinned reference count. */
	if (edbmfp->pinref == 0)
		__edb_err(edbmp->edbenv,
		    "%s: put: more blocks returned than retrieved",
		    __memp_fn(edbmfp));
	else
		--edbmfp->pinref;

	/*
	 * If we're mapping the file, there's nothing to do.  Because we can
	 * stop mapping the file at any time, we have to check on each buffer
	 * to see if the address we gave the application was part of the map
	 * region.
	 */
	if (edbmfp->addr != NULL && pgaddr >= edbmfp->addr &&
	    (u_int8_t *)pgaddr <= (u_int8_t *)edbmfp->addr + edbmfp->len) {
		UNLOCKREGION(edbmp);
		return (0);
	}

	/* Convert the page address to a buffer header. */
	bhp = (BH *)((u_int8_t *)pgaddr - SSZA(BH, buf));

	/* Set/clear the page bits. */
	if (LF_ISSET(DB_MPOOL_CLEAN) && F_ISSET(bhp, BH_DIRTY)) {
		++mp->stat.st_page_clean;
		--mp->stat.st_page_dirty;
		F_CLR(bhp, BH_DIRTY);
	}
	if (LF_ISSET(DB_MPOOL_DIRTY) && !F_ISSET(bhp, BH_DIRTY)) {
		--mp->stat.st_page_clean;
		++mp->stat.st_page_dirty;
		F_SET(bhp, BH_DIRTY);
	}
	if (LF_ISSET(DB_MPOOL_DISCARD))
		F_SET(bhp, BH_DISCARD);

	/*
	 * Check for a reference count going to zero.  This can happen if the
	 * application returns a page twice.
	 */
	if (bhp->ref == 0) {
		__edb_err(edbmp->edbenv, "%s: page %lu: unpinned page returned",
		    __memp_fn(edbmfp), (u_long)bhp->pgno);
		UNLOCKREGION(edbmp);
		return (EINVAL);
	}

	/*
	 * If more than one reference to the page, we're done.  Ignore the
	 * discard flags (for now) and leave it at its position in the LRU
	 * chain.  The rest gets done at last reference close.
	 */
	if (--bhp->ref > 0) {
		UNLOCKREGION(edbmp);
		return (0);
	}

	/* Move the buffer to the head/tail of the LRU chain. */
	SH_TAILQ_REMOVE(&mp->bhq, bhp, q, __bh);
	if (F_ISSET(bhp, BH_DISCARD))
		SH_TAILQ_INSERT_HEAD(&mp->bhq, bhp, q, __bh);
	else
		SH_TAILQ_INSERT_TAIL(&mp->bhq, bhp, q);

	/*
	 * If this buffer is scheduled for writing because of a checkpoint, we
	 * need to write it (if we marked it dirty), or update the checkpoint
	 * counters (if we didn't mark it dirty).  If we try to write it and
	 * can't, that's not necessarily an error, but set a flag so that the
	 * next time the memp_sync function runs we try writing it there, as
	 * the checkpoint application better be able to write all of the files.
	 */
	if (F_ISSET(bhp, BH_WRITE)) {
		if (F_ISSET(bhp, BH_DIRTY)) {
			if (__memp_bhwrite(edbmp,
			    edbmfp->mfp, bhp, NULL, &wrote) != 0 || !wrote)
				F_SET(mp, MP_LSN_RETRY);
		} else {
			F_CLR(bhp, BH_WRITE);

			--edbmfp->mfp->lsn_cnt;
			--mp->lsn_cnt;
		}
	}

	UNLOCKREGION(edbmp);
	return (0);
}
