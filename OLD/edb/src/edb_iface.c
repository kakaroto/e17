/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)edb_iface.c	10.40 (Sleepycat) 12/19/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#endif

#include "edb_int.h"
#include "edb_page.h"
#include "edb_auto.h"
#include "edb_ext.h"
#include "common_ext.h"

static int __edb_keyempty __P((const DB_ENV *));
static int __edb_rdonly __P((const DB_ENV *, const char *));
static int __edbt_ferr __P((const DB *, const char *, const DBT *, int));

/*
 * __edb_cdelchk --
 *	Common cursor delete argument checking routine.
 *
 * PUBLIC: int __edb_cdelchk __P((const DB *, u_int32_t, int, int));
 */
int
__edb_cdelchk(edbp, flags, isrdonly, isvalid)
	const DB *edbp;
	u_int32_t flags;
	int isrdonly, isvalid;
{
	/* Check for changes to a read-only tree. */
	if (isrdonly)
		return (__edb_rdonly(edbp->edbenv, "c_del"));

	/* Check for invalid function flags. */
	switch (flags) {
	case 0:
		break;
	default:
		return (__edb_ferr(edbp->edbenv, "DBcursor->c_del", 0));
	}

	/*
	 * The cursor must be initialized, return -1 for an invalid cursor,
	 * otherwise 0.
	 */
	return (isvalid ? 0 : EINVAL);
}

/*
 * __edb_cgetchk --
 *	Common cursor get argument checking routine.
 *
 * PUBLIC: int __edb_cgetchk __P((const DB *, DBT *, DBT *, u_int32_t, int));
 */
int
__edb_cgetchk(edbp, key, data, flags, isvalid)
	const DB *edbp;
	DBT *key, *data;
	u_int32_t flags;
	int isvalid;
{
	int key_einval, key_flags, ret;

	key_einval = key_flags = 0;

	/* Check for invalid function flags. */
	LF_CLR(DB_RMW);
	switch (flags) {
	case DB_NEXT_DUP:
		if (edbp->type == DB_RECNO)
			goto err;
		/* FALLTHROUGH */
	case DB_CURRENT:
	case DB_FIRST:
	case DB_LAST:
	case DB_NEXT:
	case DB_PREV:
		key_flags = 1;
		break;
	case DB_GET_BOTH:
	case DB_SET_RANGE:
		key_einval = key_flags = 1;
		break;
	case DB_SET:
		key_einval = 1;
		break;
	case DB_GET_RECNO:
		if (!F_ISSET(edbp, DB_BT_RECNUM))
			goto err;
		break;
	case DB_SET_RECNO:
		if (!F_ISSET(edbp, DB_BT_RECNUM))
			goto err;
		key_einval = key_flags = 1;
		break;
	default:
err:		return (__edb_ferr(edbp->edbenv, "DBcursor->c_get", 0));
	}

	/* Check for invalid key/data flags. */
	if ((ret = __edbt_ferr(edbp, "key", key, 0)) != 0)
		return (ret);
	if ((ret = __edbt_ferr(edbp, "data", data, 0)) != 0)
		return (ret);

	/* Check for missing keys. */
	if (key_einval && (key->data == NULL || key->size == 0))
		return (__edb_keyempty(edbp->edbenv));

	/*
	 * The cursor must be initialized for DB_CURRENT, return -1 for an
	 * invalid cursor, otherwise 0.
	 */
	return (isvalid || flags != DB_CURRENT ? 0 : EINVAL);
}

/*
 * __edb_cputchk --
 *	Common cursor put argument checking routine.
 *
 * PUBLIC: int __edb_cputchk __P((const DB *,
 * PUBLIC:    const DBT *, DBT *, u_int32_t, int, int));
 */
int
__edb_cputchk(edbp, key, data, flags, isrdonly, isvalid)
	const DB *edbp;
	const DBT *key;
	DBT *data;
	u_int32_t flags;
	int isrdonly, isvalid;
{
	int key_einval, key_flags, ret;

	key_einval = key_flags = 0;

	/* Check for changes to a read-only tree. */
	if (isrdonly)
		return (__edb_rdonly(edbp->edbenv, "c_put"));

	/* Check for invalid function flags. */
	switch (flags) {
	case DB_AFTER:
	case DB_BEFORE:
		if (edbp->dup_compare != NULL)
			goto err;
		if (edbp->type == DB_RECNO && !F_ISSET(edbp, DB_RE_RENUMBER))
			goto err;
		if (edbp->type != DB_RECNO && !F_ISSET(edbp, DB_AM_DUP))
			goto err;
		break;
	case DB_CURRENT:
		/*
		 * If there is a comparison function, doing a DB_CURRENT
		 * must not change the part of the data item that is used
		 * for the comparison.
		 */
		break;
	case DB_KEYFIRST:
	case DB_KEYLAST:
		if (edbp->type == DB_RECNO)
			goto err;
		key_einval = key_flags = 1;
		break;
	default:
err:		return (__edb_ferr(edbp->edbenv, "DBcursor->c_put", 0));
	}

	/* Check for invalid key/data flags. */
	if (key_flags && (ret = __edbt_ferr(edbp, "key", key, 0)) != 0)
		return (ret);
	if ((ret = __edbt_ferr(edbp, "data", data, 0)) != 0)
		return (ret);

	/* Check for missing keys. */
	if (key_einval && (key->data == NULL || key->size == 0))
		return (__edb_keyempty(edbp->edbenv));

	/*
	 * The cursor must be initialized for anything other than DB_KEYFIRST
	 * and DB_KEYLAST, return -1 for an invalid cursor, otherwise 0.
	 */
	return (isvalid ||
	    flags == DB_KEYFIRST || flags == DB_KEYLAST ? 0 : EINVAL);
}

/*
 * __edb_closechk --
 *	DB->close flag check.
 *
 * PUBLIC: int __edb_closechk __P((const DB *, u_int32_t));
 */
int
__edb_closechk(edbp, flags)
	const DB *edbp;
	u_int32_t flags;
{
	/* Check for invalid function flags. */
	if (flags != 0 && flags != DB_NOSYNC)
		return (__edb_ferr(edbp->edbenv, "DB->close", 0));

	return (0);
}

/*
 * __edb_delchk --
 *	Common delete argument checking routine.
 *
 * PUBLIC: int __edb_delchk __P((const DB *, DBT *, u_int32_t, int));
 */
int
__edb_delchk(edbp, key, flags, isrdonly)
	const DB *edbp;
	DBT *key;
	u_int32_t flags;
	int isrdonly;
{
	/* Check for changes to a read-only tree. */
	if (isrdonly)
		return (__edb_rdonly(edbp->edbenv, "delete"));

	/* Check for invalid function flags. */
	switch (flags) {
	case 0:
		break;
	default:
		return (__edb_ferr(edbp->edbenv, "DB->del", 0));
	}

	/* Check for missing keys. */
	if (key->data == NULL || key->size == 0)
		return (__edb_keyempty(edbp->edbenv));

	return (0);
}

/*
 * __edb_getchk --
 *	Common get argument checking routine.
 *
 * PUBLIC: int __edb_getchk __P((const DB *, const DBT *, DBT *, u_int32_t));
 */
int
__edb_getchk(edbp, key, data, flags)
	const DB *edbp;
	const DBT *key;
	DBT *data;
	u_int32_t flags;
{
	int ret;

	/* Check for invalid function flags. */
	LF_CLR(DB_RMW);
	switch (flags) {
	case 0:
	case DB_GET_BOTH:
		break;
	case DB_SET_RECNO:
		if (!F_ISSET(edbp, DB_BT_RECNUM))
			goto err;
		break;
	default:
err:		return (__edb_ferr(edbp->edbenv, "DB->get", 0));
	}

	/* Check for invalid key/data flags. */
	if ((ret = __edbt_ferr(edbp, "key", key, flags == DB_SET_RECNO)) != 0)
		return (ret);
	if ((ret = __edbt_ferr(edbp, "data", data, 1)) != 0)
		return (ret);

	/* Check for missing keys. */
	if (key->data == NULL || key->size == 0)
		return (__edb_keyempty(edbp->edbenv));

	return (0);
}

/*
 * __edb_joinchk --
 *	Common join argument checking routine.
 *
 * PUBLIC: int __edb_joinchk __P((const DB *, u_int32_t));
 */
int
__edb_joinchk(edbp, flags)
	const DB *edbp;
	u_int32_t flags;
{
	if (flags != 0)
		return (__edb_ferr(edbp->edbenv, "DB->join", 0));

	return (0);
}

/*
 * __edb_putchk --
 *	Common put argument checking routine.
 *
 * PUBLIC: int __edb_putchk
 * PUBLIC:    __P((const DB *, DBT *, const DBT *, u_int32_t, int, int));
 */
int
__edb_putchk(edbp, key, data, flags, isrdonly, isdup)
	const DB *edbp;
	DBT *key;
	const DBT *data;
	u_int32_t flags;
	int isrdonly, isdup;
{
	int ret;

	/* Check for changes to a read-only tree. */
	if (isrdonly)
		return (__edb_rdonly(edbp->edbenv, "put"));

	/* Check for invalid function flags. */
	switch (flags) {
	case 0:
	case DB_NOOVERWRITE:
		break;
	case DB_APPEND:
		if (edbp->type != DB_RECNO)
			goto err;
		break;
	default:
err:		return (__edb_ferr(edbp->edbenv, "DB->put", 0));
	}

	/* Check for invalid key/data flags. */
	if ((ret = __edbt_ferr(edbp, "key", key, 0)) != 0)
		return (ret);
	if ((ret = __edbt_ferr(edbp, "data", data, 0)) != 0)
		return (ret);

	/* Check for missing keys. */
	if (key->data == NULL || key->size == 0)
		return (__edb_keyempty(edbp->edbenv));

	/* Check for partial puts in the presence of duplicates. */
	if (isdup && F_ISSET(data, DB_DBT_PARTIAL)) {
		__edb_err(edbp->edbenv,
"a partial put in the presence of duplicates requires a cursor operation");
		return (EINVAL);
	}

	return (0);
}

/*
 * __edb_statchk --
 *	Common stat argument checking routine.
 *
 * PUBLIC: int __edb_statchk __P((const DB *, u_int32_t));
 */
int
__edb_statchk(edbp, flags)
	const DB *edbp;
	u_int32_t flags;
{
	/* Check for invalid function flags. */
	switch (flags) {
	case 0:
		break;
	case DB_RECORDCOUNT:
		if (edbp->type == DB_RECNO)
			break;
		if (edbp->type == DB_BTREE && F_ISSET(edbp, DB_BT_RECNUM))
			break;
		goto err;
	default:
err:		return (__edb_ferr(edbp->edbenv, "DB->stat", 0));
	}

	return (0);
}

/*
 * __edb_syncchk --
 *	Common sync argument checking routine.
 *
 * PUBLIC: int __edb_syncchk __P((const DB *, u_int32_t));
 */
int
__edb_syncchk(edbp, flags)
	const DB *edbp;
	u_int32_t flags;
{
	/* Check for invalid function flags. */
	switch (flags) {
	case 0:
		break;
	default:
		return (__edb_ferr(edbp->edbenv, "DB->sync", 0));
	}

	return (0);
}

/*
 * __edbt_ferr --
 *	Check a DBT for flag errors.
 */
static int
__edbt_ferr(edbp, name, edbt, check_thread)
	const DB *edbp;
	const char *name;
	const DBT *edbt;
	int check_thread;
{
	int ret;

	/*
	 * Check for invalid DBT flags.  We allow any of the flags to be
	 * specified to any DB or DBcursor call so that applications can
	 * set DB_DBT_MALLOC when retrieving a data item from a secondary
	 * database and then specify that same DBT as a key to a primary
	 * database, without having to clear flags.
	 */
	if ((ret = __edb_fchk(edbp->edbenv, name, edbt->flags,
	    DB_DBT_MALLOC | DB_DBT_USERMEM | DB_DBT_PARTIAL)) != 0)
		return (ret);
	if ((ret = __edb_fcchk(edbp->edbenv, name,
	    edbt->flags, DB_DBT_MALLOC, DB_DBT_USERMEM)) != 0)
		return (ret);

	if (check_thread && F_ISSET(edbp, DB_AM_THREAD) &&
	    !F_ISSET(edbt, DB_DBT_MALLOC | DB_DBT_USERMEM)) {
		__edb_err(edbp->edbenv,
		    "missing flag thread flag for %s DBT", name);
		return (EINVAL);
	}
	return (0);
}

/*
 * __edb_eopnotsup --
 *	Common operation not supported message.
 *
 * PUBLIC: int __edb_eopnotsup __P((const DB_ENV *));
 */
int
__edb_eopnotsup(edbenv)
	const DB_ENV *edbenv;
{
	__edb_err(edbenv, "operation not supported");
#ifdef EOPNOTSUPP
	return (EOPNOTSUPP);
#else
	return (EINVAL);
#endif
}

/*
 * __edb_keyempty --
 *	Common missing or empty key value message.
 */
static int
__edb_keyempty(edbenv)
	const DB_ENV *edbenv;
{
	__edb_err(edbenv, "missing or empty key value specified");
	return (EINVAL);
}

/*
 * __edb_rdonly --
 *	Common readonly message.
 */
static int
__edb_rdonly(edbenv, name)
	const DB_ENV *edbenv;
	const char *name;
{
	__edb_err(edbenv, "%s: attempt to modify a read-only tree", name);
	return (EACCES);
}
