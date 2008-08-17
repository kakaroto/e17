/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)edb_err.c	10.42 (Sleepycat) 11/24/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "edb_shash.h"
#include "lock.h"
#include "lock_ext.h"
#include "log.h"
#include "log_ext.h"
#include "mp.h"
#include "mp_ext.h"
#include "txn.h"
#include "txn_ext.h"
#include "common_ext.h"
#include "clib_ext.h"

/*
 * __edb_fchk --
 *	General flags checking routine.
 *
 * PUBLIC: int __edb_fchk __P((DB_ENV *, const char *, u_int32_t, u_int32_t));
 */
int
__edb_fchk(edbenv, name, flags, ok_flags)
	DB_ENV *edbenv;
	const char *name;
	u_int32_t flags, ok_flags;
{
	return (flags & ~ok_flags ?  __edb_ferr(edbenv, name, 0) : 0);
}

/*
 * __edb_fcchk --
 *	General combination flags checking routine.
 *
 * PUBLIC: int __edb_fcchk
 * PUBLIC:    __P((DB_ENV *, const char *, u_int32_t, u_int32_t, u_int32_t));
 */
int
__edb_fcchk(edbenv, name, flags, flag1, flag2)
	DB_ENV *edbenv;
	const char *name;
	u_int32_t flags, flag1, flag2;
{
	return ((flags & flag1) &&
	    (flags & flag2) ?  __edb_ferr(edbenv, name, 1) : 0);
}

/*
 * __edb_ferr --
 *	Common flag errors.
 *
 * PUBLIC: int __edb_ferr __P((const DB_ENV *, const char *, int));
 */
int
__edb_ferr(edbenv, name, iscombo)
	const DB_ENV *edbenv;
	const char *name;
	int iscombo;
{
	__edb_err(edbenv, "illegal flag %sspecified to %s",
	    iscombo ? "combination " : "", name);
	return (EINVAL);
}

/*
 * __edb_err --
 *	Standard DB error routine.
 *
 * PUBLIC: #ifdef __STDC__
 * PUBLIC: void __edb_err __P((const DB_ENV *edbenv, const char *fmt, ...));
 * PUBLIC: #else
 * PUBLIC: void __edb_err();
 * PUBLIC: #endif
 */
void
#ifdef __STDC__
__edb_err(const DB_ENV *edbenv, const char *fmt, ...)
#else
__edb_err(edbenv, fmt, va_alist)
	const DB_ENV *edbenv;
	const char *fmt;
	va_dcl
#endif
{
	va_list ap;
	char errbuf[2048];	/* XXX: END OF THE STACK DON'T TRUST SPRINTF. */

	if (edbenv == NULL)
		return;

	if (edbenv->edb_errcall != NULL) {
#ifdef __STDC__
         	va_start(ap, fmt);
#else
	        va_start(ap);
#endif
		(void)vsnprintf(errbuf, sizeof(errbuf), fmt, ap);
		edbenv->edb_errcall(edbenv->edb_errpfx, errbuf);
		va_end(ap);
	}
	if (edbenv->edb_errfile != NULL) {
		if (edbenv->edb_errpfx != NULL)
			(void)fprintf(edbenv->edb_errfile, "%s: ",
			    edbenv->edb_errpfx);
#ifdef __STDC__
         	va_start(ap, fmt);
#else
	        va_start(ap);
#endif
		(void)vfprintf(edbenv->edb_errfile, fmt, ap);
		(void)fprintf(edbenv->edb_errfile, "\n");
		(void)fflush(edbenv->edb_errfile);
		va_end(ap);
	}
}

/*
 * __edb_pgerr --
 *	Error when unable to retrieve a specified page.
 *
 * PUBLIC: int __edb_pgerr __P((DB *, edb_pgno_t));
 */
int
__edb_pgerr(edbp, pgno)
	DB *edbp;
	edb_pgno_t pgno;
{
	/*
	 * Three things are certain:
	 * Death, taxes, and lost data.
	 * Guess which has occurred.
	 */
	__edb_err(edbp->edbenv,
	    "unable to create/retrieve page %lu", (u_long)pgno);
	return (__edb_panic(edbp->edbenv, EIO));
}

/*
 * __edb_pgfmt --
 *	Error when a page has the wrong format.
 *
 * PUBLIC: int __edb_pgfmt __P((DB *, edb_pgno_t));
 */
int
__edb_pgfmt(edbp, pgno)
	DB *edbp;
	edb_pgno_t pgno;
{
	__edb_err(edbp->edbenv,
	    "page %lu: illegal page type or format", (u_long)pgno);
	return (__edb_panic(edbp->edbenv, EINVAL));
}

/*
 * __edb_panic --
 *	Lock out the tree due to unrecoverable error.
 *
 * PUBLIC: int __edb_panic __P((DB_ENV *, int));
 */
int
__edb_panic(edbenv, errval)
	DB_ENV *edbenv;
	int errval;
{
	if (edbenv != NULL) {
		edbenv->edb_panic = errval;

		(void)__log_panic(edbenv);
		(void)__memp_panic(edbenv);
		(void)__lock_panic(edbenv);
		(void)__txn_panic(edbenv);

		__edb_err(edbenv, "PANIC: %s", strerror(errval));

		if (edbenv->edb_paniccall != NULL)
			edbenv->edb_paniccall(edbenv, errval);
	}

	/*
	 * Chaos reigns within.
	 * Reflect, repent, and reboot.
	 * Order shall return.
	 */
	return (DB_RUNRECOVERY);
}
