/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)mp_fset.c	10.16 (Sleepycat) 9/27/98";
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
 * memp_fset --
 *	Mpool page set-flag routine.
 */
int
memp_fset(edbmfp, pgaddr, flags)
	DB_MPOOLFILE *edbmfp;
	void *pgaddr;
	u_int32_t flags;
{
	BH *bhp;
	DB_MPOOL *edbmp;
	MPOOL *mp;
	int ret;

	edbmp = edbmfp->edbmp;
	mp = edbmp->mp;

	MP_PANIC_CHECK(edbmp);

	/* Validate arguments. */
	if (flags == 0)
		return (__edb_ferr(edbmp->edbenv, "memp_fset", 1));

	if ((ret = __edb_fchk(edbmp->edbenv, "memp_fset", flags,
	    DB_MPOOL_DIRTY | DB_MPOOL_CLEAN | DB_MPOOL_DISCARD)) != 0)
		return (ret);
	if ((ret = __edb_fcchk(edbmp->edbenv, "memp_fset",
	    flags, DB_MPOOL_CLEAN, DB_MPOOL_DIRTY)) != 0)
		return (ret);

	if (LF_ISSET(DB_MPOOL_DIRTY) && F_ISSET(edbmfp, MP_READONLY)) {
		__edb_err(edbmp->edbenv,
		    "%s: dirty flag set for readonly file page",
		    __memp_fn(edbmfp));
		return (EACCES);
	}

	/* Convert the page address to a buffer header. */
	bhp = (BH *)((u_int8_t *)pgaddr - SSZA(BH, buf));

	LOCKREGION(edbmp);

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

	UNLOCKREGION(edbmp);
	return (0);
}
