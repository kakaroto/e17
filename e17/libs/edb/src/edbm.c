/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
/*
 * Copyright (c) 1990, 1993
 *	Margo Seltzer.  All rights reserved.
 */
/*
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Margo Seltzer.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)edbm.c	10.23 (Sleepycat) 11/22/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#endif

#define	DB_DBM_HSEARCH	1
#include "edb_int.h"

#include "edb_page.h"
#include "hash.h"

/*
 *
 * This package provides edbm and nedbm compatible interfaces to DB.
 *
 * The DBM routines, which call the NDBM routines.
 */
static DBM *__cur_edb;

static void __edb_no_open __P((void));

int
__edb_edbm_init(file)
	char *file;
{
	if (__cur_edb != NULL)
		(void)edbm_close(__cur_edb);
	if ((__cur_edb =
	    edbm_open(file, O_CREAT | O_RDWR, __edb_omode("rw----"))) != NULL)
		return (0);
	if ((__cur_edb = edbm_open(file, O_RDONLY, 0)) != NULL)
		return (0);
	return (-1);
}

int
__edb_edbm_close()
{
	if (__cur_edb != NULL) {
		edbm_close(__cur_edb);
		__cur_edb = NULL;
	}
	return (0);
}

datum
__edb_edbm_fetch(key)
	datum key;
{
	datum item;

	if (__cur_edb == NULL) {
		__edb_no_open();
		item.dptr = 0;
		return (item);
	}
	return (edbm_fetch(__cur_edb, key));
}

datum
__edb_edbm_firstkey()
{
	datum item;

	if (__cur_edb == NULL) {
		__edb_no_open();
		item.dptr = 0;
		return (item);
	}
	return (edbm_firstkey(__cur_edb));
}

datum
__edb_edbm_nextkey(key)
	datum key;
{
	datum item;

	COMPQUIET(key.dsize, 0);

	if (__cur_edb == NULL) {
		__edb_no_open();
		item.dptr = 0;
		return (item);
	}
	return (edbm_nextkey(__cur_edb));
}

int
__edb_edbm_delete(key)
	datum key;
{
	if (__cur_edb == NULL) {
		__edb_no_open();
		return (-1);
	}
	return (edbm_delete(__cur_edb, key));
}

int
__edb_edbm_store(key, dat)
	datum key, dat;
{
	if (__cur_edb == NULL) {
		__edb_no_open();
		return (-1);
	}
	return (edbm_store(__cur_edb, key, dat, DBM_REPLACE));
}

static void
__edb_no_open()
{
	(void)fprintf(stderr, "edbm: no open database.\n");
}

/*
 * This package provides edbm and nedbm compatible interfaces to DB.
 *
 * The NDBM routines, which call the DB routines.
 */
/*
 * Returns:
 * 	*DBM on success
 *	 NULL on failure
 */
DBM *
__edb_nedbm_open(file, oflags, mode)
	const char *file;
	int oflags, mode;
{
	DB *edbp;
	DBC *edbc;
	DB_INFO edbinfo;
	int sv_errno;
	char path[MAXPATHLEN];

	memset(&edbinfo, 0, sizeof(edbinfo));
	edbinfo.edb_pagesize = 4096;
	edbinfo.h_ffactor = 40;
	edbinfo.h_nelem = 1;

	/*
	 * XXX
	 * Don't use sprintf(3)/snprintf(3) -- the former is dangerous, and
	 * the latter isn't standard, and we're manipulating strings handed
	 * us by the application.
	 */
	if (strlen(file) + strlen(DBM_SUFFIX) + 1 > sizeof(path)) {
		errno = ENAMETOOLONG;
		return (NULL);
	}
	(void)strcpy(path, file);
	(void)strcat(path, DBM_SUFFIX);
	if ((errno = edb_open(path,
	    DB_HASH, __edb_oflags(oflags), mode, NULL, &edbinfo, &edbp)) != 0)
		return (NULL);

	if ((errno = edbp->cursor(edbp, NULL, &edbc, 0)) != 0) {
		sv_errno = errno;
		(void)edbp->close(edbp, 0);
		errno = sv_errno;
		return (NULL);
	}

	return ((DBM *)edbc);
}

/*
 * Returns:
 *	Nothing.
 */
void
__edb_nedbm_close(edbm)
	DBM *edbm;
{
	DBC *edbc;

	edbc = (DBC *)edbm;

	(void)edbc->edbp->close(edbc->edbp, 0);
}

/*
 * Returns:
 *	DATUM on success
 *	NULL on failure
 */
datum
__edb_nedbm_fetch(edbm, key)
	DBM *edbm;
	datum key;
{
	DBC *edbc;
	DBT _key, _data;
	datum data;
	int ret;

	edbc = (DBC *)edbm;

	memset(&_key, 0, sizeof(DBT));
	memset(&_data, 0, sizeof(DBT));
	_key.size = key.dsize;
	_key.data = key.dptr;

	/*
	 * Note that we can't simply use the edbc we have to do a c_get/SET,
	 * because that cursor is the one used for sequential iteration and
	 * it has to remain stable in the face of intervening gets and puts.
	 */
	if ((ret = edbc->edbp->get(edbc->edbp, NULL, &_key, &_data, 0)) == 0) {
		data.dptr = _data.data;
		data.dsize = _data.size;
	} else {
		data.dptr = NULL;
		data.dsize = 0;
		if (ret == DB_NOTFOUND)
			errno = ENOENT;
		else {
			errno = ret;
			F_SET(edbc->edbp, DB_DBM_ERROR);
		}
	}
	return (data);
}

/*
 * Returns:
 *	DATUM on success
 *	NULL on failure
 */
datum
__edb_nedbm_firstkey(edbm)
	DBM *edbm;
{
	DBC *edbc;
	DBT _key, _data;
	datum key;
	int ret;

	edbc = (DBC *)edbm;

	memset(&_key, 0, sizeof(DBT));
	memset(&_data, 0, sizeof(DBT));

	if ((ret = edbc->c_get(edbc, &_key, &_data, DB_FIRST)) == 0) {
		key.dptr = _key.data;
		key.dsize = _key.size;
	} else {
		key.dptr = NULL;
		key.dsize = 0;
		if (ret == DB_NOTFOUND)
			errno = ENOENT;
		else {
			errno = ret;
			F_SET(edbc->edbp, DB_DBM_ERROR);
		}
	}
	return (key);
}

/*
 * Returns:
 *	DATUM on success
 *	NULL on failure
 */
datum
__edb_nedbm_nextkey(edbm)
	DBM *edbm;
{
	DBC *edbc;
	DBT _key, _data;
	datum key;
	int ret;

	edbc = (DBC *)edbm;

	memset(&_key, 0, sizeof(DBT));
	memset(&_data, 0, sizeof(DBT));

	if ((ret = edbc->c_get(edbc, &_key, &_data, DB_NEXT)) == 0) {
		key.dptr = _key.data;
		key.dsize = _key.size;
	} else {
		key.dptr = NULL;
		key.dsize = 0;
		if (ret == DB_NOTFOUND)
			errno = ENOENT;
		else {
			errno = ret;
			F_SET(edbc->edbp, DB_DBM_ERROR);
		}
	}
	return (key);
}

/*
 * Returns:
 *	 0 on success
 *	<0 failure
 */
int
__edb_nedbm_delete(edbm, key)
	DBM *edbm;
	datum key;
{
	DBC *edbc;
	DBT _key;
	int ret;

	edbc = (DBC *)edbm;

	memset(&_key, 0, sizeof(DBT));
	_key.data = key.dptr;
	_key.size = key.dsize;

	if ((ret = edbc->edbp->del(edbc->edbp, NULL, &_key, 0)) == 0)
		return (0);

	if (ret == DB_NOTFOUND)
		errno = ENOENT;
	else {
		errno = ret;
		F_SET(edbc->edbp, DB_DBM_ERROR);
	}
	return (-1);
}

/*
 * Returns:
 *	 0 on success
 *	<0 failure
 *	 1 if DBM_INSERT and entry exists
 */
int
__edb_nedbm_store(edbm, key, data, flags)
	DBM *edbm;
	datum key, data;
	int flags;
{
	DBC *edbc;
	DBT _key, _data;
	int ret;

	edbc = (DBC *)edbm;

	memset(&_key, 0, sizeof(DBT));
	_key.data = key.dptr;
	_key.size = key.dsize;

	memset(&_data, 0, sizeof(DBT));
	_data.data = data.dptr;
	_data.size = data.dsize;

	if ((ret = edbc->edbp->put(edbc->edbp, NULL,
	    &_key, &_data, flags == DBM_INSERT ? DB_NOOVERWRITE : 0)) == 0)
		return (0);

	if (ret == DB_KEYEXIST)
		return (1);

	errno = ret;
	F_SET(edbc->edbp, DB_DBM_ERROR);
	return (-1);
}

int
__edb_nedbm_error(edbm)
	DBM *edbm;
{
	DBC *edbc;

	edbc = (DBC *)edbm;

	return (F_ISSET(edbc->edbp, DB_DBM_ERROR));
}

int
__edb_nedbm_clearerr(edbm)
	DBM *edbm;
{
	DBC *edbc;

	edbc = (DBC *)edbm;

	F_CLR(edbc->edbp, DB_DBM_ERROR);
	return (0);
}

/*
 * Returns:
 *	1 if read-only
 *	0 if not read-only
 */
int
__edb_nedbm_rdonly(edbm)
	DBM *edbm;
{
	DBC *edbc;

	edbc = (DBC *)edbm;

	return (F_ISSET(edbc->edbp, DB_AM_RDONLY) ? 1 : 0);
}

/*
 * XXX
 * We only have a single file descriptor that we can return, not two.  Return
 * the same one for both files.  Hopefully, the user is using it for locking
 * and picked one to use at random.
 */
int
__edb_nedbm_dirfno(edbm)
	DBM *edbm;
{
	return (edbm_pagfno(edbm));
}

int
__edb_nedbm_pagfno(edbm)
	DBM *edbm;
{
	DBC *edbc;
	int fd;

	edbc = (DBC *)edbm;

	(void)edbc->edbp->fd(edbc->edbp, &fd);
	return (fd);
}
