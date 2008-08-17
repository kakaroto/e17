/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)xa_map.c	10.4 (Sleepycat) 10/20/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "txn.h"

/*
 * This file contains all the mapping information that we need to support
 * the DB/XA interface.
 */

/*
 * __edb_rmid_to_env
 *	Return the environment associated with a given XA rmid.
 *
 * PUBLIC: int __edb_rmid_to_env __P((int rmid, DB_ENV **envp, int open_ok));
 */
int
__edb_rmid_to_env(rmid, envp, open_ok)
	int rmid;
	DB_ENV **envp;
	int open_ok;
{
	DB_ENV *env;
	char *edbhome;

	env = TAILQ_FIRST(&DB_GLOBAL(edb_envq));
	if (env != NULL && env->xa_rmid == rmid) {
		*envp = env;
		return (0);
	}

	/*
	 * When we map an rmid, move that environment to be the first one in
	 * the list of environments, so we pass the correct environment from
	 * the upcoming edb_xa_open() call into edb_open().
	 */
	for (; env != NULL; env = TAILQ_NEXT(env, links))
		if (env->xa_rmid == rmid) {
			TAILQ_REMOVE(&DB_GLOBAL(edb_envq), env, links);
			TAILQ_INSERT_HEAD(&DB_GLOBAL(edb_envq), env, links);
			*envp = env;
			return (0);
		}

	/*
	 * We have not found the rmid on the environment list.  If we
	 * are allowed to do an open, search for the rmid on the name
	 * list and, if we find it, then open it.
	 */
	if (!open_ok)
		return (1);

	if (__edb_rmid_to_name(rmid, &edbhome) != 0)
		return (1);
#undef XA_FLAGS
#define	XA_FLAGS \
	DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN

	if (__edb_os_calloc(1, sizeof(DB_ENV), &env) != 0)
		return (1);

	if (edb_appinit(edbhome, NULL, env, XA_FLAGS) != 0) 
		goto err;

	if (__edb_map_rmid(rmid, env) != 0)
		goto err1;

	__edb_unmap_rmid_name(rmid);

	*envp = env;
	return (0);

err1:	(void)edb_appexit(env);
err:	__edb_os_free(env, sizeof(DB_ENV));
	return (1);
}

/*
 * __edb_xid_to_txn
 *	Return the txn that corresponds to this XID.
 *
 * PUBLIC: int __edb_xid_to_txn __P((DB_ENV *, XID *, size_t *));
 */
int
__edb_xid_to_txn(edbenv, xid, offp)
	DB_ENV *edbenv;
	XID *xid;
	size_t *offp;
{
	DB_TXNREGION *tmr;
	struct __txn_detail *td;

	/*
	 * Search the internal active transaction table to find the
	 * matching xid.  If this is a performance hit, then we
	 * can create a hash table, but I doubt it's worth it.
	 */
	tmr = edbenv->tx_info->region;

	LOCK_TXNREGION(edbenv->tx_info);
	for (td = SH_TAILQ_FIRST(&tmr->active_txn, __txn_detail);
	    td != NULL;
	    td = SH_TAILQ_NEXT(td, links, __txn_detail))
		if (memcmp(xid->data, td->xid, XIDDATASIZE) == 0)
			break;
	UNLOCK_TXNREGION(edbenv->tx_info);

	if (td == NULL)
		return (EINVAL);

	*offp = (u_int8_t *)td - (u_int8_t *)tmr;
	return (0);
}

/*
 * __edb_map_rmid
 *	Create a mapping between the specified rmid and environment.
 *
 * PUBLIC: int __edb_map_rmid __P((int, DB_ENV *));
 */
int
__edb_map_rmid(rmid, env)
	int rmid;
	DB_ENV *env;
{
	if (__edb_os_calloc(1, sizeof(DB_TXN), &env->xa_txn) != 0)
		return (XAER_RMERR);
	env->xa_txn->txnid = TXN_INVALID;
	env->xa_rmid = rmid;
	TAILQ_INSERT_HEAD(&DB_GLOBAL(edb_envq), env, links);
	return (XA_OK);
}

/*
 * __edb_unmap_rmid
 *	Destroy the mapping for the given rmid.
 *
 * PUBLIC: int __edb_unmap_rmid __P((int));
 */
int
__edb_unmap_rmid(rmid)
	int rmid;
{
	DB_ENV *e;

	for (e = TAILQ_FIRST(&DB_GLOBAL(edb_envq));
	    e->xa_rmid != rmid;
	    e = TAILQ_NEXT(e, links));

	if (e == NULL)
		return (EINVAL);

	TAILQ_REMOVE(&DB_GLOBAL(edb_envq), e, links);
	if (e->xa_txn != NULL)
		__edb_os_free(e->xa_txn, sizeof(DB_TXN));
	return (0);
}

/*
 * __edb_map_xid
 *	Create a mapping between this XID and the transaction at
 *	"off" in the shared region.
 *
 * PUBLIC: int __edb_map_xid __P((DB_ENV *, XID *, size_t));
 */
int
__edb_map_xid(env, xid, off)
	DB_ENV *env;
	XID *xid;
	size_t off;
{
	DB_TXNMGR *tm;
	TXN_DETAIL *td;

	tm = env->tx_info;
	td = (TXN_DETAIL *)((u_int8_t *)tm->region + off);

	LOCK_TXNREGION(tm);
	memcpy(td->xid, xid->data, XIDDATASIZE);
	UNLOCK_TXNREGION(tm);

	return (0);
}

/*
 * __edb_unmap_xid
 *	Destroy the mapping for the specified XID.
 *
 * PUBLIC: void __edb_unmap_xid __P((DB_ENV *, XID *, size_t));
 */

void
__edb_unmap_xid(env, xid, off)
	DB_ENV *env;
	XID *xid;
	size_t off;
{
	TXN_DETAIL *td;

	COMPQUIET(xid, NULL);

	td = (TXN_DETAIL *)((u_int8_t *)env->tx_info->region + off);
	memset(td->xid, 0, sizeof(td->xid));
}

/*
 * __edb_map_rmid_name --
 * 	Create a mapping from an rmid to a name (the xa_info argument).
 * We use this during create and then at some later point when we are
 * trying to map an rmid, we might indicate that it's OK to do an open
 * in which case, we'll get the xa_info parameter from here and then
 * free it up.
 *
 * PUBLIC: int __edb_map_rmid_name __P((int, char *));
 */

int
__edb_map_rmid_name(rmid, edbhome)
	int rmid;
	char *edbhome;
{
	struct __rmname *entry;
	int ret;

	if ((ret = __edb_os_malloc(sizeof(struct __rmname), NULL, &entry)) != 0)
		return (ret);

	if ((ret = __edb_os_strdup(edbhome, &entry->edbhome)) != 0) {
		__edb_os_free(entry, sizeof(struct __rmname));
		return (ret);
	}

	entry->rmid = rmid;

	TAILQ_INSERT_HEAD(&DB_GLOBAL(edb_nameq), entry, links);
	return (0);
}

/*
 * __edb_rmid_to_name --
 *	Given an rmid, return the name of the home directory for that
 * rmid.
 *
 * PUBLIC: int __edb_rmid_to_name __P((int, char **));
 */
int
__edb_rmid_to_name(rmid, edbhomep)
	int rmid;
	char **edbhomep;
{
	struct __rmname *np;

	for (np = TAILQ_FIRST(&DB_GLOBAL(edb_nameq)); np != NULL;
	    np = TAILQ_NEXT(np, links)) {
		if (np->rmid == rmid) {
			*edbhomep = np->edbhome;
			return (0);
		}
	}
	return (1);
}

/*
 * __edb_unmap_rmid_name --
 *	Given an rmid, remove its entry from the name list.
 *
 * PUBLIC:  void __edb_unmap_rmid_name __P((int));
 */
void
__edb_unmap_rmid_name(rmid)
	int rmid;
{
	struct __rmname *np, *next;

	for (np = TAILQ_FIRST(&DB_GLOBAL(edb_nameq)); np != NULL; np = next) {
		next = TAILQ_NEXT(np, links);
		if (np->rmid == rmid) {
			TAILQ_REMOVE(&DB_GLOBAL(edb_nameq), np, links);
			__edb_os_freestr(np->edbhome);
			__edb_os_free(np, sizeof(struct __rmname));
			return;
		}
	}
	return;
}
