/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)mp_open.c	10.27 (Sleepycat) 10/1/98";
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
 * memp_open --
 *	Initialize and/or join a memory pool.
 */
int
memp_open(path, flags, mode, edbenv, retp)
	const char *path;
	u_int32_t flags;
	int mode;
	DB_ENV *edbenv;
	DB_MPOOL **retp;
{
	DB_MPOOL *edbmp;
	size_t cachesize;
	int is_private, ret;

	/* Validate arguments. */
#ifdef HAVE_SPINLOCKS
#define	OKFLAGS	(DB_CREATE | DB_MPOOL_PRIVATE | DB_NOMMAP | DB_THREAD)
#else
#define	OKFLAGS	(DB_CREATE | DB_MPOOL_PRIVATE | DB_NOMMAP)
#endif
	if ((ret = __edb_fchk(edbenv, "memp_open", flags, OKFLAGS)) != 0)
		return (ret);

	/* Extract fields from DB_ENV structure. */
	cachesize = edbenv == NULL ? 0 : edbenv->mp_size;

	/* Create and initialize the DB_MPOOL structure. */
	if ((ret = __edb_os_calloc(1, sizeof(DB_MPOOL), &edbmp)) != 0)
		return (ret);
	LIST_INIT(&edbmp->edbregq);
	TAILQ_INIT(&edbmp->edbmfq);

	edbmp->edbenv = edbenv;

	/* Decide if it's possible for anyone else to access the pool. */
	is_private =
	    (edbenv == NULL && path == NULL) || LF_ISSET(DB_MPOOL_PRIVATE);

	/*
	 * Map in the region.  We do locking regardless, as portions of it are
	 * implemented in common code (if we put the region in a file, that is).
	 */
	F_SET(edbmp, MP_LOCKREGION);
	if ((ret = __memp_ropen(edbmp,
	    path, cachesize, mode, is_private, LF_ISSET(DB_CREATE))) != 0)
		goto err;
	F_CLR(edbmp, MP_LOCKREGION);

	/*
	 * If there's concurrent access, then we have to lock the region.
	 * If it's threaded, then we have to lock both the handles and the
	 * region, and we need to allocate a mutex for that purpose.
	 */
	if (!is_private)
		F_SET(edbmp, MP_LOCKREGION);
	if (LF_ISSET(DB_THREAD)) {
		F_SET(edbmp, MP_LOCKHANDLE | MP_LOCKREGION);
		LOCKREGION(edbmp);
		ret = __memp_alloc(edbmp,
		    sizeof(edb_mutex_t), NULL, &edbmp->mutexp);
		UNLOCKREGION(edbmp);
		if (ret != 0) {
			(void)memp_close(edbmp);
			goto err;
		}
		LOCKINIT(edbmp, edbmp->mutexp);
	}

	*retp = edbmp;
	return (0);

err:	if (edbmp != NULL)
		__edb_os_free(edbmp, sizeof(DB_MPOOL));
	return (ret);
}

/*
 * memp_close --
 *	Close a memory pool.
 */
int
memp_close(edbmp)
	DB_MPOOL *edbmp;
{
	DB_MPOOLFILE *edbmfp;
	DB_MPREG *mpreg;
	int ret, t_ret;

	ret = 0;

	MP_PANIC_CHECK(edbmp);

	/* Discard DB_MPREGs. */
	while ((mpreg = LIST_FIRST(&edbmp->edbregq)) != NULL) {
		LIST_REMOVE(mpreg, q);
		__edb_os_free(mpreg, sizeof(DB_MPREG));
	}

	/* Discard DB_MPOOLFILEs. */
	while ((edbmfp = TAILQ_FIRST(&edbmp->edbmfq)) != NULL)
		if ((t_ret = memp_fclose(edbmfp)) != 0 && ret == 0)
			ret = t_ret;

	/* Discard thread mutex. */
	if (F_ISSET(edbmp, MP_LOCKHANDLE)) {
		LOCKREGION(edbmp);
		__edb_shalloc_free(edbmp->addr, edbmp->mutexp);
		UNLOCKREGION(edbmp);
	}

	/* Close the region. */
	if ((t_ret = __edb_rdetach(&edbmp->reginfo)) != 0 && ret == 0)
		ret = t_ret;

	if (edbmp->reginfo.path != NULL)
		__edb_os_freestr(edbmp->reginfo.path);
	__edb_os_free(edbmp, sizeof(DB_MPOOL));

	return (ret);
}

/*
 * __memp_panic --
 *	Panic a memory pool.
 *
 * PUBLIC: void __memp_panic __P((DB_ENV *));
 */
void
__memp_panic(edbenv)
	DB_ENV *edbenv;
{
	if (edbenv->mp_info != NULL)
		edbenv->mp_info->mp->rlayout.panic = 1;
}

/*
 * memp_unlink --
 *	Exit a memory pool.
 */
int
memp_unlink(path, force, edbenv)
	const char *path;
	int force;
	DB_ENV *edbenv;
{
	REGINFO reginfo;
	int ret;

	memset(&reginfo, 0, sizeof(reginfo));
	reginfo.edbenv = edbenv;
	reginfo.appname = DB_APP_NONE;
	if (path != NULL && (ret = __edb_os_strdup(path, &reginfo.path)) != 0)
		return (ret);
	reginfo.file = DB_DEFAULT_MPOOL_FILE;
	ret = __edb_runlink(&reginfo, force);
	if (reginfo.path != NULL)
		__edb_os_freestr(reginfo.path);
	return (ret);
}

/*
 * memp_register --
 *	Register a file type's pgin, pgout routines.
 */
int
memp_register(edbmp, ftype, pgin, pgout)
	DB_MPOOL *edbmp;
	int ftype;
	int (*pgin) __P((edb_pgno_t, void *, DBT *));
	int (*pgout) __P((edb_pgno_t, void *, DBT *));
{
	DB_MPREG *mpr;
	int ret;

	MP_PANIC_CHECK(edbmp);

	if ((ret = __edb_os_malloc(sizeof(DB_MPREG), NULL, &mpr)) != 0)
		return (ret);

	mpr->ftype = ftype;
	mpr->pgin = pgin;
	mpr->pgout = pgout;

	/*
	 * Insert at the head.  Because we do a linear walk, we'll find
	 * the most recent registry in the case of multiple entries, so
	 * we don't have to check for multiple registries.
	 */
	LOCKHANDLE(edbmp, edbmp->mutexp);
	LIST_INSERT_HEAD(&edbmp->edbregq, mpr, q);
	UNLOCKHANDLE(edbmp, edbmp->mutexp);

	return (0);
}
