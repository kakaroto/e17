/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)os_spin.c	10.10 (Sleepycat) 10/12/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>
#if defined(HAVE_PSTAT_GETDYNAMIC)
#include <sys/pstat.h>
#endif

#include <limits.h>
#include <unistd.h>
#endif

#include "edb_int.h"
#include "os_jump.h"

#if defined(HAVE_PSTAT_GETDYNAMIC)
/*
 * __edb_os_pstat_getdynamic --
 *	HP/UX.
 */
static int
__edb_os_pstat_getdynamic()
{
	struct pst_dynamic psd;

	return (pstat_getdynamic(&psd,
	    sizeof(psd), (size_t)1, 0) == -1 ? 1 : psd.psd_proc_cnt);
}
#endif

#if defined(HAVE_SYSCONF) && defined(_SC_NPROCESSORS_ONLN)
/*
 * __edb_os_sysconf --
 *	Solaris, Linux.
 */
static int
__edb_os_sysconf()
{
	int nproc;

	return ((nproc = sysconf(_SC_NPROCESSORS_ONLN)) > 1 ? nproc : 1);
}
#endif

/*
 * __edb_os_spin --
 *	Return the number of default spins before blocking.
 *
 * PUBLIC: int __edb_os_spin __P((void));
 */
int
__edb_os_spin()
{
	/*
	 * If the application specified a value or we've already figured it
	 * out, return it.
	 *
	 * XXX
	 * We don't want to repeatedly call the underlying function because
	 * it can be expensive (e.g., requiring multiple filesystem accesses
	 * under Debian Linux).
	 */
	if (DB_GLOBAL(edb_tsl_spins) != 0)
		return (DB_GLOBAL(edb_tsl_spins));

	DB_GLOBAL(edb_tsl_spins) = 1;
#if defined(HAVE_PSTAT_GETDYNAMIC)
	DB_GLOBAL(edb_tsl_spins) = __edb_os_pstat_getdynamic();
#endif
#if defined(HAVE_SYSCONF) && defined(_SC_NPROCESSORS_ONLN)
	DB_GLOBAL(edb_tsl_spins) = __edb_os_sysconf();
#endif

	/*
	 * Spin 50 times per processor, we have anecdotal evidence that this
	 * is a reasonable value.
	 */
	DB_GLOBAL(edb_tsl_spins) *= 50;

	return (DB_GLOBAL(edb_tsl_spins));
}

/*
 * __edb_os_yield --
 *	Yield the processor.
 *
 * PUBLIC: void __edb_os_yield __P((u_long));
 */
void
__edb_os_yield(usecs)
	u_long usecs;
{
	if (__edb_jump.j_yield != NULL && __edb_jump.j_yield() == 0)
		return;
	__edb_os_sleep(0, usecs);
}
