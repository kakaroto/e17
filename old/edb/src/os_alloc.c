/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)os_alloc.c	10.10 (Sleepycat) 10/12/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "edb_int.h"
#include "os_jump.h"

/*
 * !!!
 * Correct for systems that return NULL when you allocate 0 bytes of memory.
 * There are several places in DB where we allocate the number of bytes held
 * by the key/data item, and it can be 0.  Correct here so that malloc never
 * returns a NULL for that reason (which behavior is permitted by ANSI).  We
 * could make these calls macros on non-Alpha architectures (that's where we
 * saw the problem), but it's probably not worth the autoconf complexity.
 *
 * !!!
 * Correct for systems that don't set errno when malloc and friends fail.
 *
 *	Out of memory.
 *	We wish to hold the whole sky,
 *	But we never will.
 */

/*
 * __edb_os_strdup --
 *	The strdup(3) function for DB.
 *
 * PUBLIC: int __edb_os_strdup __P((const char *, void *));
 */
int
__edb_os_strdup(str, storep)
	const char *str;
	void *storep;
{
	size_t size;
	int ret;
	void *p;

	*(void **)storep = NULL;

	size = strlen(str) + 1;
	if ((ret = __edb_os_malloc(size, NULL, &p)) != 0)
		return (ret);

	memcpy(p, str, size);

	*(void **)storep = p;
	return (0);
}

/*
 * __edb_os_calloc --
 *	The calloc(3) function for DB.
 *
 * PUBLIC: int __edb_os_calloc __P((size_t, size_t, void *));
 */
int
__edb_os_calloc(num, size, storep)
	size_t num, size;
	void *storep;
{
	void *p;
	int ret;

	size *= num;
	if ((ret = __edb_os_malloc(size, NULL, &p)) != 0)
		return (ret);

	memset(p, 0, size);
	*(void **)storep = p;

	return (0);
}

/*
 * __edb_os_malloc --
 *	The malloc(3) function for DB.
 *
 * PUBLIC: int __edb_os_malloc __P((size_t, void *(*)(size_t), void *));
 */
int
__edb_os_malloc(size, edb_malloc, storep)
	size_t size;
	void *(*edb_malloc) __P((size_t)), *storep;
{
	void *p;

	*(void **)storep = NULL;

	/* Never allocate 0 bytes -- some C libraries don't like it. */
	if (size == 0)
		++size;

	/* Some C libraries don't correctly set errno when malloc(3) fails. */
	errno = 0;
	if (edb_malloc != NULL)
		p = edb_malloc(size);
	else if (__edb_jump.j_malloc != NULL)
		p = __edb_jump.j_malloc(size);
	else
		p = malloc(size);
	if (p == NULL) {
		if (errno == 0)
			errno = ENOMEM;
		return (errno);
	}

#ifdef DIAGNOSTIC
	memset(p, 0xedb, size);
#endif
	*(void **)storep = p;

	return (0);
}

/*
 * __edb_os_realloc --
 *	The realloc(3) function for DB.
 *
 * PUBLIC: int __edb_os_realloc __P((void *, size_t));
 */
int
__edb_os_realloc(storep, size)
	void *storep;
	size_t size;
{
	void *p, *ptr;

	ptr = *(void **)storep;

	/* If we haven't yet allocated anything yet, simply call malloc. */
	if (ptr == NULL)
		return (__edb_os_malloc(size, NULL, storep));

	/* Never allocate 0 bytes -- some C libraries don't like it. */
	if (size == 0)
		++size;

	/*
	 * Some C libraries don't correctly set errno when realloc(3) fails.
	 *
	 * Don't overwrite the original pointer, there are places in DB we
	 * try to continue after realloc fails.
	 */
	errno = 0;
	if (__edb_jump.j_realloc != NULL)
		p = __edb_jump.j_realloc(ptr, size);
	else
		p = realloc(ptr, size);
	if (p == NULL) {
		if (errno == 0)
			errno = ENOMEM;
		return (errno);
	}

	*(void **)storep = p;

	return (0);
}

/*
 * __edb_os_free --
 *	The free(3) function for DB.
 *
 * PUBLIC: void __edb_os_free __P((void *, size_t));
 */
void
__edb_os_free(ptr, size)
	void *ptr;
	size_t size;
{
#ifdef DIAGNOSTIC
	if (size != 0)
		memset(ptr, 0xedb, size);
#endif

	if (__edb_jump.j_free != NULL)
		__edb_jump.j_free(ptr);
	else
		free(ptr);
}

/*
 * __edb_os_freestr --
 *	The free(3) function for DB, freeing a string.
 *
 * PUBLIC: void __edb_os_freestr __P((void *));
 */
void
__edb_os_freestr(ptr)
	void *ptr;
{
#ifdef DIAGNOSTIC
	memset(ptr, 0xedb, strlen(ptr) + 1);
#endif

	if (__edb_jump.j_free != NULL)
		__edb_jump.j_free(ptr);
	else
		free(ptr);
}
