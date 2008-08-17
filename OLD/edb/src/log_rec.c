/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
/*
 * Copyright (c) 1995, 1996
 *	The President and Fellows of Harvard University.  All rights reserved.
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
static const char sccsid[] = "@(#)log_rec.c	10.26 (Sleepycat) 10/21/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "log.h"
#include "edb_dispatch.h"
#include "common_ext.h"

static int __log_do_open __P((DB_LOG *,
    u_int8_t *, char *, DBTYPE, u_int32_t));
static int __log_lid_to_fname __P((DB_LOG *, u_int32_t, FNAME **));
static int __log_open_file __P((DB_LOG *, __log_register_args *));

/*
 * PUBLIC: int __log_register_recover
 * PUBLIC:     __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
 */
int
__log_register_recover(logp, edbtp, lsnp, redo, info)
	DB_LOG *logp;
	DBT *edbtp;
	DB_LSN *lsnp;
	int redo;
	void *info;
{
	DB_ENTRY *edbe;
	__log_register_args *argp;
	int ret;

#ifdef DEBUG_RECOVER
	__log_register_print(logp, edbtp, lsnp, redo, info);
#endif
	COMPQUIET(info, NULL);
	COMPQUIET(lsnp, NULL);

	F_SET(logp, DBC_RECOVER);

	if ((ret = __log_register_read(edbtp->data, &argp)) != 0)
		goto out;

	if ((argp->opcode == LOG_CHECKPOINT && redo == TXN_OPENFILES) ||
	    (argp->opcode == LOG_OPEN &&
	    (redo == TXN_REDO || redo == TXN_OPENFILES ||
	     redo == TXN_FORWARD_ROLL)) ||
	    (argp->opcode == LOG_CLOSE &&
	    (redo == TXN_UNDO || redo == TXN_BACKWARD_ROLL))) {
		/*
		 * If we are redoing an open or undoing a close, then we need
		 * to open a file.
		 */
		ret = __log_open_file(logp, argp);
		if (ret == ENOENT) {
			if (redo == TXN_OPENFILES)
				__edb_err(logp->edbenv, "warning: %s: %s",
				    argp->name.data, strerror(ENOENT));
			ret = 0;
		}
	} else if (argp->opcode != LOG_CHECKPOINT &&
	    argp->opcode != LOG_CLOSE) {
		/*
		 * If we are undoing an open, then we need to close the file.
		 * Note that we do *not* close the file if we are redoing a
		 * close, because we do not log the reference counts on log
		 * files and we may have had the file open multiple times,
		 * and therefore, this close should just dec a reference
		 * count.  However, since we only do one open during a
		 * checkpoint, this will inadvertently close the file.
		 *
  		 * If the file is deleted, then we can just ignore this close.
 		 * Otherwise, we should usually have a valid edbp we should
  		 * close or whose reference count should be decremented.
 		 * However, if we shut down without closing a file, we
		 * may, in fact, not have the file open, and that's OK.
		 */
		LOCK_LOGTHREAD(logp);
		if (argp->id < logp->edbentry_cnt) {
			edbe = &logp->edbentry[argp->id];
			if (edbe->edbp != NULL && --edbe->refcount == 0) {
				ret = edbe->edbp->close(edbe->edbp, 0);
				if (edbe->name != NULL) {
					__edb_os_freestr(edbe->name);
					edbe->name = NULL;
				}
				(void)__log_rem_logid(logp, argp->id);
			}
		}
		UNLOCK_LOGTHREAD(logp);
 	} else if (argp->opcode == LOG_CHECKPOINT && redo == TXN_UNDO &&
 	    (argp->id >= logp->edbentry_cnt ||
 	    (!logp->edbentry[argp->id].deleted &&
 	    logp->edbentry[argp->id].edbp == NULL))) {
 		/*
 		 * It's a checkpoint and we are rolling backward.  It
 		 * is possible that the system was shut down and thus
 		 * ended with a stable checkpoint; this file was never
 		 * closed and has therefore not been reopened yet.  If
 		 * so, we need to try to open it.
 		 */
 		ret = __log_open_file(logp, argp);
 		if (ret == ENOENT) {
 			__edb_err(logp->edbenv, "warning: %s: %s",
			    argp->name.data, strerror(ENOENT));
 			ret = 0;
 		}
	}

out:	F_CLR(logp, DBC_RECOVER);
	if (argp != NULL)
		__edb_os_free(argp, 0);
	return (ret);
}

/* Hand coded routines. */

/*
 * Called during log_register recovery.  Make sure that we have an
 * entry in the edbentry table for this ndx.
 * Returns 0 on success, non-zero on error.
 */
static int
__log_open_file(lp, argp)
	DB_LOG *lp;
	__log_register_args *argp;
{
	DB_ENTRY *edbe;
  
	if (argp->name.size == 0)
		return(0);
  
	/*
	 * Because of reference counting, we cannot automatically close files
	 * during recovery, so when we're opening, we have to check that the
	 * name we are opening is what we expect.  If it's not, then we close
	 * the old file and open the new one.
	 */
	LOCK_LOGTHREAD(lp);
	if (argp->id < lp->edbentry_cnt)
		edbe = &lp->edbentry[argp->id];
	else
		edbe = NULL;

	if (edbe != NULL && (edbe->deleted == 1 || edbe->edbp != NULL) &&
	    edbe->name != NULL && argp->name.data != NULL &&
	    strncmp(argp->name.data, edbe->name, argp->name.size) == 0) {

		edbe->refcount++;
		UNLOCK_LOGTHREAD(lp);
		return (0);
	}
  	UNLOCK_LOGTHREAD(lp);

	if (edbe != NULL && edbe->edbp != NULL) {
                (void)edbe->edbp->close(edbe->edbp, 0);
		if (edbe->name != NULL)
			__edb_os_freestr(edbe->name);
                edbe->name = NULL;
		(void)__log_rem_logid(lp, argp->id);
        }


	return (__log_do_open(lp,
	    argp->uid.data, argp->name.data, argp->ftype, argp->id));
}

/*
 * __log_do_open --
 * 	Open files referenced in the log.  This is the part of the open that
 * is not protected by the thread mutex.
 */

static int
__log_do_open(lp, uid, name, ftype, ndx)
	DB_LOG *lp;
	u_int8_t *uid;
	char *name;
	DBTYPE ftype;
	u_int32_t ndx;
{
	DB *edbp;
	int ret;

	edbp = NULL;
	if ((ret = edb_open(name, ftype, 0, 0, lp->edbenv, NULL, &edbp)) == 0) {
		/*
		 * Verify that we are opening the same file that we were
		 * referring to when we wrote this log record.
		 */
		if (memcmp(uid, edbp->fileid, DB_FILE_ID_LEN) != 0) {
			(void)edbp->close(edbp, 0);
			edbp = NULL;
			ret = ENOENT;
		}
	}

	if (ret == 0 || ret == ENOENT)
		(void)__log_add_logid(lp, edbp, name, ndx);

	return (ret);
}

/*
 * __log_add_logid --
 *	Adds a DB entry to the log's DB entry table.
 *
 * PUBLIC: int __log_add_logid __P((DB_LOG *, DB *, const char *, u_int32_t));
 */
int
__log_add_logid(logp, edbp, name, ndx)
	DB_LOG *logp;
	DB *edbp;
	const char *name;
	u_int32_t ndx;
{
	u_int32_t i;
	int ret;

	ret = 0;

	LOCK_LOGTHREAD(logp);

	/*
	 * Check if we need to grow the table.  Note, ndx is 0-based (the
	 * index into the DB entry table) an edbentry_cnt is 1-based, the
	 * number of available slots.
	 */
	if (logp->edbentry_cnt <= ndx) {
		if ((ret = __edb_os_realloc(&logp->edbentry,
		    (ndx + DB_GROW_SIZE) * sizeof(DB_ENTRY))) != 0)
			goto err;

		/* Initialize the new entries. */
		for (i = logp->edbentry_cnt; i < ndx + DB_GROW_SIZE; i++) {
			logp->edbentry[i].edbp = NULL;
			logp->edbentry[i].deleted = 0;
			logp->edbentry[i].name = NULL;
		}

		logp->edbentry_cnt = i;
	}

	/* Make space for the name and copy it in. */
	if (name != NULL) {
		if ((ret = __edb_os_malloc(strlen(name) + 1, 
		    NULL, &logp->edbentry[ndx].name)) != 0)
			goto err;
		strcpy(logp->edbentry[ndx].name, name);
	}

	if (logp->edbentry[ndx].deleted == 0 && logp->edbentry[ndx].edbp == NULL) {
		logp->edbentry[ndx].edbp = edbp;
		logp->edbentry[ndx].refcount = 1;
		logp->edbentry[ndx].deleted = edbp == NULL;
	} else
		logp->edbentry[ndx].refcount++;


err:	UNLOCK_LOGTHREAD(logp);
	return (ret);
}


/*
 * __edb_fileid_to_edb --
 *	Return the DB corresponding to the specified fileid.
 *
 * PUBLIC: int __edb_fileid_to_edb __P((DB_LOG *, DB **, u_int32_t));
 */
int
__edb_fileid_to_edb(logp, edbpp, ndx)
	DB_LOG *logp;
	DB **edbpp;
	u_int32_t ndx;
{
	int ret;
	char *name;
	FNAME *fname;

	ret = 0;
	LOCK_LOGTHREAD(logp);

	/*
	 * Under XA, a process different than the one issuing DB
	 * operations may abort a transaction.  In this case, 
	 * recovery routines are run by a process that does not
	 * necessarily have the file open.  In this case, we must
	 * open the file explicitly.
	 */
	if (ndx >= logp->edbentry_cnt ||
	    (!logp->edbentry[ndx].deleted && logp->edbentry[ndx].edbp == NULL)) {
		if (__log_lid_to_fname(logp, ndx, &fname) != 0) {
			/* Couldn't find entry; this is a fatal error. */
			ret = EINVAL;
			goto err;
		}
		name = R_ADDR(logp, fname->name_off);
		/*
		 * __log_do_open is called without protection of the
		 * log thread lock.
		 */
		UNLOCK_LOGTHREAD(logp);
		/*
		 * At this point, we are not holding the thread lock, so
		 * exit directly instead of going through the exit code
		 * at the bottom.  If the __log_do_open succeeded, then
		 * we don't need to do any of the remaining error checking
		 * at the end of this routine.
		 */
		if ((ret = __log_do_open(logp,
		    fname->ufid, name, fname->s_type, ndx)) != 0)
			return (ret);
		*edbpp = logp->edbentry[ndx].edbp;
		return (0);
	}

	/*
	 * Return DB_DELETED if the file has been deleted
	 * (it's not an error).
	 */
	if (logp->edbentry[ndx].deleted) {
		ret = DB_DELETED;
		goto err;
	}

	/*
	 * Otherwise return 0, but if we don't have a corresponding DB,
	 * it's an error.
	 */
	if ((*edbpp = logp->edbentry[ndx].edbp) == NULL)
		ret = ENOENT;

err:	UNLOCK_LOGTHREAD(logp);
	return (ret);
}

/*
 * Close files that were opened by the recovery daemon.
 *
 * PUBLIC: void __log_close_files __P((DB_LOG *));
 */
void
__log_close_files(logp)
	DB_LOG *logp;
{
	u_int32_t i;

	LOCK_LOGTHREAD(logp);
	for (i = 0; i < logp->edbentry_cnt; i++)
		if (logp->edbentry[i].edbp) {
			logp->edbentry[i].edbp->close(logp->edbentry[i].edbp, 0);
			logp->edbentry[i].edbp = NULL;
			logp->edbentry[i].deleted = 0;
		}
	F_CLR(logp, DBC_RECOVER);
	UNLOCK_LOGTHREAD(logp);
}

/*
 * PUBLIC: void __log_rem_logid __P((DB_LOG *, u_int32_t));
 */
void
__log_rem_logid(logp, ndx)
	DB_LOG *logp;
	u_int32_t ndx;
{
	LOCK_LOGTHREAD(logp);
	if (--logp->edbentry[ndx].refcount == 0) {
		logp->edbentry[ndx].edbp = NULL;
		logp->edbentry[ndx].deleted = 0;
	}
	UNLOCK_LOGTHREAD(logp);
}

/*
 * __log_lid_to_fname --
 * 	Traverse the shared-memory region looking for the entry that
 * matches the passed log fileid.  Returns 0 on success; -1 on error.
 */
static int
__log_lid_to_fname(edblp, lid, fnamep)
	DB_LOG *edblp;
	u_int32_t lid;
	FNAME **fnamep;
{
	FNAME *fnp;

	for (fnp = SH_TAILQ_FIRST(&edblp->lp->fq, __fname);
	    fnp != NULL; fnp = SH_TAILQ_NEXT(fnp, q, __fname)) {
		if (fnp->ref == 0)	/* Entry not in use. */
			continue;
		if (fnp->id == lid) {
			*fnamep = fnp;
			return (0);
		}
	}
	return (-1);
}
