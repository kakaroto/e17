/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)os_abs.c	10.9 (Sleepycat) 7/21/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>
#endif

#include "edb_int.h"

/*
 * __edb_os_abspath --
 *	Return if a path is an absolute path.
 *
 * PUBLIC: int __edb_os_abspath __P((const char *));
 */
int
__edb_os_abspath(path)
	const char *path;
{
#ifndef __EMX__
	return (path[0] == '/');
#else	
	return (((isalpha((int)path[0])) && (path[1]==':') && 
        ((path[2]=='\\') || (path[2]=='/'))));
#endif        
}
