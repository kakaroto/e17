/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)edb_am.c	10.15 (Sleepycat) 12/30/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "edb_page.h"
#include "edb_shash.h"
#include "mp.h"
#include "btree.h"
#include "hash.h"
#include "edb_am.h"
#include "edb_ext.h"

static int __edb_c_close __P((DBC *));
static int __edb_cursor __P((DB *, DB_TXN *, DBC **, u_int32_t));
static int __edb_fd __P((DB *, int *));
static int __edb_get __P((DB *, DB_TXN *, DBT *, DBT *, u_int32_t));
static int __edb_put __P((DB *, DB_TXN *, DBT *, DBT *, u_int32_t));

/*
 * __edb_init_wrapper --
 *	Wrapper layer to implement generic DB functions.
 *
 * PUBLIC: int __edb_init_wrapper __P((DB *));
 */
int
__edb_init_wrapper(edbp)
	DB *edbp;
{
	edbp->close = __edb_close;
	edbp->cursor = __edb_cursor;
	edbp->del = NULL;		/* !!! Must be set by access method. */
	edbp->fd = __edb_fd;
	edbp->get = __edb_get;
	edbp->join = __edb_join;
	edbp->put = __edb_put;
	edbp->stat = NULL;		/* !!! Must be set by access method. */
	edbp->sync = __edb_sync;

	return (0);
}

/*
 * __edb_cursor --
 *	Allocate and return a cursor.
 */
static int
__edb_cursor(edbp, txn, edbcp, flags)
	DB *edbp;
	DB_TXN *txn;
	DBC **edbcp;
	u_int32_t flags;
{
	DBC *edbc, *aedbc;
	int ret;
	edb_lockmode_t mode;
	u_int32_t op;

	DB_PANIC_CHECK(edbp);

	/* Take one from the free list if it's available. */
	DB_THREAD_LOCK(edbp);
	if ((edbc = TAILQ_FIRST(&edbp->free_queue)) != NULL)
		TAILQ_REMOVE(&edbp->free_queue, edbc, links);
	else {
		DB_THREAD_UNLOCK(edbp);

		if ((ret = __edb_os_calloc(1, sizeof(DBC), &edbc)) != 0)
			return (ret);

		edbc->edbp = edbp;
		edbc->c_close = __edb_c_close;

		/* Set up locking information. */
		if (F_ISSET(edbp, DB_AM_LOCKING | DB_AM_CDB)) {
 			/*
 			 * If we are not threaded, then there is no need to
 			 * create new locker ids.  We know that no one else
 			 * is running concurrently using this DB, so we can
 			 * take a peek at any cursors on the active queue.
 			 */
 			if (!F_ISSET(edbp, DB_AM_THREAD) &&
 			    (aedbc = TAILQ_FIRST(&edbp->active_queue)) != NULL)
 				edbc->lid = aedbc->lid;
 			else
 				if ((ret = lock_id(edbp->edbenv->lk_info,
 				    &edbc->lid)) != 0)
 					goto err;
 
			memcpy(edbc->lock.fileid, edbp->fileid, DB_FILE_ID_LEN);
			if (F_ISSET(edbp, DB_AM_CDB)) {
				edbc->lock_edbt.size = DB_FILE_ID_LEN;
				edbc->lock_edbt.data = edbc->lock.fileid;
			} else {
				edbc->lock_edbt.size = sizeof(edbc->lock);
				edbc->lock_edbt.data = &edbc->lock;
			}
		}

		switch (edbp->type) {
		case DB_BTREE:
		case DB_RECNO:
			if ((ret = __bam_c_init(edbc)) != 0)
				goto err;
			break;
		case DB_HASH:
			if ((ret = __ham_c_init(edbc)) != 0)
				goto err;
			break;
		default:
			ret = EINVAL;
			goto err;
		}

		DB_THREAD_LOCK(edbp);
	}

	if ((edbc->txn = txn) == NULL)
		edbc->locker = edbc->lid;
	else
		edbc->locker = txn->txnid;

	TAILQ_INSERT_TAIL(&edbp->active_queue, edbc, links);
	DB_THREAD_UNLOCK(edbp);

	/*
	 * If this is the concurrent DB product, then we do all locking
	 * in the interface, which is right here.
	 */
	if (F_ISSET(edbp, DB_AM_CDB)) {
		op = LF_ISSET(DB_OPFLAGS_MASK);
		mode = (op == DB_WRITELOCK) ? DB_LOCK_WRITE :
		    (LF_ISSET(DB_RMW) ? DB_LOCK_IWRITE : DB_LOCK_READ);
		if ((ret = lock_get(edbp->edbenv->lk_info, edbc->locker, 0,
		    &edbc->lock_edbt, mode, &edbc->mylock)) != 0) {
			(void)__edb_c_close(edbc);
			return (EAGAIN);
		}
		if (LF_ISSET(DB_RMW))
			F_SET(edbc, DBC_RMW);
		if (op == DB_WRITELOCK)
			F_SET(edbc, DBC_WRITER);
	}

	*edbcp = edbc;
	return (0);

err:	__edb_os_free(edbc, sizeof(*edbc));
	return (ret);
}

/*
 * __edb_c_close --
 *	Close the cursor (recycle for later use).
 */
static int
__edb_c_close(edbc)
	DBC *edbc;
{
	DB *edbp;
	int ret, t_ret;

	edbp = edbc->edbp;

	DB_PANIC_CHECK(edbp);

	ret = 0;

	/*
	 * We cannot release the lock until after we've called the
	 * access method specific routine, since btrees may have pending
	 * deletes.
	 */

	/* Remove the cursor from the active queue. */
	DB_THREAD_LOCK(edbp);
	TAILQ_REMOVE(&edbp->active_queue, edbc, links);
	DB_THREAD_UNLOCK(edbp);

	/* Call the access specific cursor close routine. */
	if ((t_ret = edbc->c_am_close(edbc)) != 0 && ret == 0)
		t_ret = ret;

	/* Release the lock. */
	if (F_ISSET(edbc->edbp, DB_AM_CDB) && edbc->mylock != LOCK_INVALID) {
		ret = lock_put(edbc->edbp->edbenv->lk_info, edbc->mylock);
		edbc->mylock = LOCK_INVALID;
	}

	/* Clean up the cursor. */
	edbc->flags = 0;

#ifdef DEBUG
	/*
	 * Check for leftover locks, unless we're running with transactions.
	 *
	 * If we're running tests, display any locks currently held.  It's
	 * possible that some applications may hold locks for long periods,
	 * e.g., conference room locks, but the DB tests should never close
	 * holding locks.
	 */
	if (F_ISSET(edbp, DB_AM_LOCKING) && edbc->lid == edbc->locker) {
		DB_LOCKREQ request;

		request.op = DB_LOCK_DUMP;
		if ((t_ret = lock_vec(edbp->edbenv->lk_info,
		    edbc->locker, 0, &request, 1, NULL)) != 0 && ret == 0)
			ret = EAGAIN;
	}
#endif
	/* Move the cursor to the free queue. */
	DB_THREAD_LOCK(edbp);
	TAILQ_INSERT_TAIL(&edbp->free_queue, edbc, links);
	DB_THREAD_UNLOCK(edbp);

	return (ret);
}

#ifdef DEBUG
/*
 * __edb_cprint --
 *	Display the current cursor list.
 *
 * PUBLIC: int __edb_cprint __P((DB *));
 */
int
__edb_cprint(edbp)
	DB *edbp;
{
	static const FN fn[] = {
		{ DBC_RECOVER, 	"recover" },
		{ DBC_RMW, 	"read-modify-write" },
		{ 0 },
	};
	DBC *edbc;

	DB_THREAD_LOCK(edbp);
	for (edbc = TAILQ_FIRST(&edbp->active_queue);
	    edbc != NULL; edbc = TAILQ_NEXT(edbc, links)) {
		fprintf(stderr,
		    "%#0x: edbp: %#0x txn: %#0x lid: %lu locker: %lu",
		    (u_int)edbc, (u_int)edbc->edbp, (u_int)edbc->txn,
		    (u_long)edbc->lid, (u_long)edbc->locker);
		__edb_prflags(edbc->flags, fn, stderr);
		fprintf(stderr, "\n");
	}
	DB_THREAD_UNLOCK(edbp);

	return (0);
}
#endif /* DEBUG */

/*
 * __edb_c_destroy --
 *	Destroy the cursor.
 *
 * PUBLIC: int __edb_c_destroy __P((DBC *));
 */
int
__edb_c_destroy(edbc)
	DBC *edbc;
{
	DB *edbp;
	int ret;

	edbp = edbc->edbp;

	/* Remove the cursor from the free queue. */
	DB_THREAD_LOCK(edbp);
	TAILQ_REMOVE(&edbp->free_queue, edbc, links);
	DB_THREAD_UNLOCK(edbp);

	/* Call the access specific cursor destroy routine. */
	ret = edbc->c_am_destroy == NULL ? 0 : edbc->c_am_destroy(edbc);

	/* Free up allocated memory. */
	if (edbc->rkey.data != NULL)
		__edb_os_free(edbc->rkey.data, edbc->rkey.ulen);
	if (edbc->rdata.data != NULL)
		__edb_os_free(edbc->rdata.data, edbc->rdata.ulen);
	__edb_os_free(edbc, sizeof(*edbc));

	return (0);
}

/*
 * edb_fd --
 *	Return a file descriptor for flock'ing.
 */
static int
__edb_fd(edbp, fdp)
        DB *edbp;
	int *fdp;
{
	DB_PANIC_CHECK(edbp);

	/*
	 * XXX
	 * Truly spectacular layering violation.
	 */
	return (__mp_xxx_fd(edbp->mpf, fdp));
}

/*
 * __edb_get --
 *	Return a key/data pair.
 */
static int
__edb_get(edbp, txn, key, data, flags)
	DB *edbp;
	DB_TXN *txn;
	DBT *key, *data;
	u_int32_t flags;
{
	DBC *edbc;
	int ret, t_ret;

	DB_PANIC_CHECK(edbp);

	if ((ret = __edb_getchk(edbp, key, data, flags)) != 0)
		return (ret);

	if ((ret = edbp->cursor(edbp, txn, &edbc, 0)) != 0)
		return (ret);

	DEBUG_LREAD(edbc, txn, "__edb_get", key, NULL, flags);

	ret = edbc->c_get(edbc, key, data,
	    flags == 0 || flags == DB_RMW ? flags | DB_SET : flags);

	if ((t_ret = __edb_c_close(edbc)) != 0 && ret == 0)
		ret = t_ret;

	return (ret);
}

/*
 * __edb_put --
 *	Store a key/data pair.
 */
static int
__edb_put(edbp, txn, key, data, flags)
	DB *edbp;
	DB_TXN *txn;
	DBT *key, *data;
	u_int32_t flags;
{
	DBC *edbc;
	DBT tdata;
	int ret, t_ret;

	DB_PANIC_CHECK(edbp);

	if ((ret = __edb_putchk(edbp, key, data,
	    flags, F_ISSET(edbp, DB_AM_RDONLY), F_ISSET(edbp, DB_AM_DUP))) != 0)
		return (ret);

	if ((ret = edbp->cursor(edbp, txn, &edbc, DB_WRITELOCK)) != 0)
		return (ret);

	DEBUG_LWRITE(edbc, txn, "__edb_put", key, data, flags);

	if (flags == DB_NOOVERWRITE) {
		/*
		 * Set DB_DBT_USERMEM, this might be a threaded application and
		 * the flags checking will catch us.  We don't want the actual
		 * data, so request a partial of length 0.
		 */
		memset(&tdata, 0, sizeof(tdata));
		F_SET(&tdata, DB_DBT_USERMEM | DB_DBT_PARTIAL);
		if ((ret = edbc->c_get(edbc, key, &tdata, DB_SET | DB_RMW)) == 0)
			ret = DB_KEYEXIST;
		else if (ret == DB_NOTFOUND)
			ret = 0;
	}
	if (ret == 0)
		ret = edbc->c_put(edbc, key, data, DB_KEYLAST);

	if ((t_ret = __edb_c_close(edbc)) != 0 && ret == 0)
		ret = t_ret;

	return (ret);
}

/*
 * __edb_sync --
 *	Flush the database cache.
 *
 * PUBLIC: int __edb_sync __P((DB *, u_int32_t));
 */
int
__edb_sync(edbp, flags)
	DB *edbp;
	u_int32_t flags;
{
	int ret;

	DB_PANIC_CHECK(edbp);

	if ((ret = __edb_syncchk(edbp, flags)) != 0)
		return (ret);

	/* If it wasn't possible to modify the file, we're done. */
	if (F_ISSET(edbp, DB_AM_INMEM | DB_AM_RDONLY))
		return (0);

	/* Flush any dirty pages from the cache to the backing file. */
	if ((ret = memp_fsync(edbp->mpf)) == DB_INCOMPLETE)
		ret = 0;

	return (ret);
}
