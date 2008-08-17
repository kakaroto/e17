/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)xa_edb.c	10.6 (Sleepycat) 12/19/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "edb_page.h"
#include "xa.h"
#include "xa_ext.h"
#include "edb_am.h"
#include "edb_ext.h"
#include "common_ext.h"

static int __xa_c_close __P((DBC *));
static int __xa_c_del __P((DBC *, u_int32_t));
static int __xa_c_get __P((DBC *, DBT *, DBT *, u_int32_t));
static int __xa_c_put __P((DBC *, DBT *, DBT *, u_int32_t));
static int __xa_close __P((DB *, u_int32_t));
static int __xa_cursor __P((DB *, DB_TXN *, DBC **, u_int32_t));
static int __xa_del __P((DB *, DB_TXN *, DBT *, u_int32_t));
static int __xa_fd __P((DB *, int *));
static int __xa_get __P((DB *, DB_TXN *, DBT *, DBT *, u_int32_t));
static int __xa_put __P((DB *, DB_TXN *, DBT *, DBT *, u_int32_t));
static int __xa_stat __P((DB *, void *, void *(*)(size_t), u_int32_t));
static int __xa_sync __P((DB *, u_int32_t));

int
edb_xa_open(fname, type, flags, mode, edbinfo, edbpp)
	const char *fname;
	DBTYPE type;
	u_int32_t flags;
	int mode;
	DB_INFO *edbinfo;
	DB **edbpp;
{
	DB *edbp, *real_edbp;
	DB_ENV *edbenv;
	struct __rmname *rp;
	int ret;

	/*
	 * First try to open up the underlying DB.
	 *
	 * !!!
	 * The edbenv argument is taken from the global list of environments.
	 * When the transaction manager called xa_start() (__edb_xa_start()),
	 * the "current" DB environment was moved to the start of the list.
	 * However, if we were called in a tpsvrinit function (which is
	 * entirely plausible), then it's possible that xa_open was called
	 * (which simply recorded the name of the environment to open) and
	 * this is the next call into DB.  In that case, we still have to
	 * open the environment.
	 *
	 * The way that we know that xa_open and nothing else was called
	 * is because the nameq is not NULL.
	 */
	if ((rp = TAILQ_FIRST(&DB_GLOBAL(edb_nameq))) != NULL &&
	    (ret = __edb_rmid_to_env(rp->rmid, &edbenv, 1)) != 0)
		return (ret);

	edbenv = TAILQ_FIRST(&DB_GLOBAL(edb_envq));
	if ((ret = edb_open(fname,
	    type, flags, mode, edbenv, edbinfo, &real_edbp)) != 0)
		return (ret);

	/*
	 * Allocate our own DB handle, and copy the exported fields and
	 * function pointers into it.  The internal pointer references
	 * the real underlying DB handle.
	 */
	if ((ret = __edb_os_calloc(1, sizeof(DB), &edbp)) != 0) {
		(void)real_edbp->close(real_edbp, 0);
		return (ret);
	}
	edbp->type = real_edbp->type;
	edbp->byteswapped = real_edbp->byteswapped;
	edbp->edbenv = edbenv;
	edbp->internal = real_edbp;
	TAILQ_INIT(&edbp->active_queue);
	TAILQ_INIT(&edbp->free_queue);
	edbp->close = __xa_close;
	edbp->cursor = __xa_cursor;
	edbp->del = __xa_del;
	edbp->fd = __xa_fd;
	edbp->get = __xa_get;
	edbp->join = real_edbp->join;
	edbp->put = __xa_put;
	edbp->stat = __xa_stat;
	edbp->sync = __xa_sync;

	*edbpp = edbp;
	return (0);
}

static int
__xa_close(edbp, flags)
	DB *edbp;
	u_int32_t flags;
{
	DB *real_edbp;
	DBC *edbc;
	int ret;

	/* Close any associated cursors. */
	while ((edbc = TAILQ_FIRST(&edbp->active_queue)) != NULL)
		(void)edbc->c_close(edbc);

	/* Close the DB handle. */
	real_edbp = (DB *)edbp->internal;
	ret = real_edbp->close(real_edbp, flags);

	__edb_os_free(edbp, sizeof(DB));
	return (ret);
}

static int
__xa_cursor(edbp, txn, edbcp, flags)
	DB *edbp;
	DB_TXN *txn;
	DBC **edbcp;
	u_int32_t flags;
{
	DB *real_edbp;
	DBC *real_edbc, *edbc;
	int ret;

	real_edbp = (DB *)edbp->internal;
	txn = edbp->edbenv->xa_txn;

	if ((ret = real_edbp->cursor(real_edbp, txn, &real_edbc, flags)) != 0)
		return (ret);

	/*
	 * Allocate our own DBC handle, and copy the exported fields and
	 * function pointers into it.  The internal pointer references
	 * the real underlying DBC handle.
	 */
	if ((ret = __edb_os_calloc(1, sizeof(DBC), &edbc)) != 0) {
		(void)real_edbc->c_close(real_edbc);
		return (ret);
	}
	edbc->edbp = edbp;
	edbc->c_close = __xa_c_close;
	edbc->c_del = __xa_c_del;
	edbc->c_get = __xa_c_get;
	edbc->c_put = __xa_c_put;
	edbc->internal = real_edbc;
	TAILQ_INSERT_TAIL(&edbp->active_queue, edbc, links);

	*edbcp = edbc;
	return (0);
}

static int
__xa_fd(edbp, fdp)
	DB *edbp;
	int *fdp;
{
	DB *real_edbp;

	COMPQUIET(fdp, NULL);

	real_edbp = (DB *)edbp->internal;
	return (__edb_eopnotsup(real_edbp->edbenv));
}

static int
__xa_del(edbp, txn, key, flags)
	DB *edbp;
	DB_TXN *txn;
	DBT *key;
	u_int32_t flags;
{
	DB *real_edbp;

	real_edbp = (DB *)edbp->internal;
	txn = edbp->edbenv->xa_txn;

	return (real_edbp->del(real_edbp, txn, key, flags));
}

static int
__xa_get(edbp, txn, key, data, flags)
	DB *edbp;
	DB_TXN *txn;
	DBT *key;
	DBT *data;
	u_int32_t flags;
{
	DB *real_edbp;

	real_edbp = (DB *)edbp->internal;
	txn = edbp->edbenv->xa_txn;

	return (real_edbp->get(real_edbp, txn, key, data, flags));
}

static int
__xa_put(edbp, txn, key, data, flags)
	DB *edbp;
	DB_TXN *txn;
	DBT *key;
	DBT *data;
	u_int32_t flags;
{
	DB *real_edbp;

	real_edbp = (DB *)edbp->internal;
	txn = edbp->edbenv->xa_txn;

	return (real_edbp->put(real_edbp, txn, key, data, flags));
}

static int
__xa_stat(edbp, spp, edb_malloc, flags)
	DB *edbp;
	void *spp;
	void *(*edb_malloc) __P((size_t));
	u_int32_t flags;
{
	DB *real_edbp;

	real_edbp = (DB *)edbp->internal;
	return (real_edbp->stat(real_edbp, spp, edb_malloc, flags));
}

static int
__xa_sync(edbp, flags)
	DB *edbp;
	u_int32_t flags;
{
	DB *real_edbp;

	real_edbp = (DB *)edbp->internal;
	return (real_edbp->sync(real_edbp, flags));
}

static int
__xa_c_close(edbc)
	DBC *edbc;
{
	DBC *real_edbc;
	int ret;

	real_edbc = (DBC *)edbc->internal;

	ret = real_edbc->c_close(real_edbc);

	TAILQ_REMOVE(&edbc->edbp->active_queue, edbc, links);
	__edb_os_free(edbc, sizeof(DBC));

	return (ret);
}

static int
__xa_c_del(edbc, flags)
	DBC *edbc;
	u_int32_t flags;
{
	DBC *real_edbc;

	real_edbc = (DBC *)edbc->internal;
	return (real_edbc->c_del(real_edbc, flags));
}

static int
__xa_c_get(edbc, key, data, flags)
	DBC *edbc;
	DBT *key;
	DBT *data;
	u_int32_t flags;
{
	DBC *real_edbc;

	real_edbc = (DBC *)edbc->internal;
	return (real_edbc->c_get(real_edbc, key, data, flags));
}

static int
__xa_c_put(edbc, key, data, flags)
	DBC *edbc;
	DBT *key;
	DBT *data;
	u_int32_t flags;
{
	DBC *real_edbc;

	real_edbc = (DBC *)edbc->internal;
	return (real_edbc->c_put(real_edbc, key, data, flags));
}
