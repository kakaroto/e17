/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
/*
 * Copyright (c) 1990, 1993, 1994, 1995, 1996
 *	Keith Bostic.  All rights reserved.
 */
/*
 * Copyright (c) 1990, 1993, 1994, 1995
 *	The Regents of the University of California.  All rights reserved.
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
static const char sccsid[] = "@(#)edb.c	10.75 (Sleepycat) 12/3/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "edb_page.h"
#include "edb_shash.h"
#include "edb_swap.h"
#include "btree.h"
#include "hash.h"
#include "mp.h"
#include "edb_am.h"
#include "common_ext.h"

/*
 * If the metadata page has the flag set, set the local flag.  If the page
 * does NOT have the flag set, return EINVAL if the user's edbinfo argument
 * caused us to already set the local flag.
 */
#define	DBINFO_FCHK(edbp, fn, meta_flags, m_name, edbp_name) {		\
	if ((meta_flags) & (m_name))					\
		F_SET(edbp, edbp_name);					\
	else								\
		if (F_ISSET(edbp, edbp_name)) {				\
			__edb_err(edbenv,					\
	    "%s: %s specified in dbinfo argument but not set in file",	\
			    fname, fn);					\
			goto einval;					\
		}							\
}

/*
 * edb_open --
 *	Main library interface to the DB access methods.
 */
int
edb_open(fname, type, flags, mode, edbenv, edbinfo, edbpp)
	const char *fname;
	DBTYPE type;
	u_int32_t flags;
	int mode;
	DB_ENV *edbenv;
	DB_INFO *edbinfo;
	DB **edbpp;
{
	BTMETA *btm;
	DB *edbp;
	DBT pgcookie;
	DB_ENV *envp, t_edbenv;
	DB_MPOOL_FINFO finfo;
	DB_PGINFO pginfo;
	HASHHDR *hashm;
	size_t cachesize;
	ssize_t nr;
	u_int32_t iopsize;
	int fd, ftype, need_fileid, restore, ret, retry_cnt, swapped;
	char *real_name, mbuf[512];

	/* Validate arguments. */
#ifdef HAVE_SPINLOCKS
#define	OKFLAGS	(DB_CREATE | DB_FCNTL_LOCKING | DB_NOMMAP | DB_RDONLY | DB_THREAD | DB_TRUNCATE)
#else
#define	OKFLAGS	(DB_CREATE | DB_FCNTL_LOCKING | DB_NOMMAP | DB_RDONLY | DB_TRUNCATE)
#endif
	if ((ret = __edb_fchk(edbenv, "db_open", flags, OKFLAGS)) != 0)
		return (ret);

	if (edbenv != NULL) {
		/*
		 * You can't specify threads during the edb_open() if the
		 * environment wasn't configured with them.
		 */
		if (LF_ISSET(DB_THREAD) && !F_ISSET(edbenv, DB_ENV_THREAD)) {
			__edb_err(edbenv,
			    "environment not created using DB_THREAD");
			return (EINVAL);
		}

		/*
		 * Specifying a cachesize to edb_open(3), after creating an
		 * environment with DB_INIT_MPOOL, is a common mistake.
		 */
		if (edbenv->mp_info != NULL &&
		    edbinfo != NULL && edbinfo->edb_cachesize != 0) {
			__edb_err(edbenv,
			    "cachesize will be ignored if environment exists");
			return (EINVAL);
		}
	}

	/* Allocate the DB structure, reference the DB_ENV structure. */
	if ((ret = __edb_os_calloc(1, sizeof(DB), &edbp)) != 0)
		return (ret);
	edbp->edbenv = edbenv;

	/* Initialize for error return. */
	edbp->saved_open_fd = fd = -1;
	need_fileid = 1;
	real_name = NULL;

	/* Random initialization. */
	TAILQ_INIT(&edbp->free_queue);
	TAILQ_INIT(&edbp->active_queue);
	if ((ret = __edb_init_wrapper(edbp)) != 0)
		goto err;

	/* Convert the edb_open(3) flags. */
	if (LF_ISSET(DB_RDONLY))
		F_SET(edbp, DB_AM_RDONLY);
	if (LF_ISSET(DB_THREAD))
		F_SET(edbp, DB_AM_THREAD);

	/* Convert the edbinfo structure flags. */
	if (edbinfo != NULL) {
		/*
		 * !!!
		 * We can't check for illegal flags until we know what type
		 * of open we're doing.
		 */
		if (F_ISSET(edbinfo, DB_DELIMITER))
			F_SET(edbp, DB_RE_DELIMITER);
		if (F_ISSET(edbinfo, DB_DUP))
			F_SET(edbp, DB_AM_DUP);
		if (F_ISSET(edbinfo, DB_FIXEDLEN))
			F_SET(edbp, DB_RE_FIXEDLEN);
		if (F_ISSET(edbinfo, DB_PAD))
			F_SET(edbp, DB_RE_PAD);
		if (F_ISSET(edbinfo, DB_RECNUM))
			F_SET(edbp, DB_BT_RECNUM);
		if (F_ISSET(edbinfo, DB_RENUMBER))
			F_SET(edbp, DB_RE_RENUMBER);
		if (F_ISSET(edbinfo, DB_SNAPSHOT))
			F_SET(edbp, DB_RE_SNAPSHOT);
	}

	/*
	 * Set based on the edbenv fields, although no logging or transactions
	 * are possible for temporary files.
	 */
	if (edbenv != NULL) {
		if (edbenv->lk_info != NULL)
			if (F_ISSET(edbenv, DB_ENV_CDB))
				F_SET(edbp, DB_AM_CDB);
			else
				F_SET(edbp, DB_AM_LOCKING);
		if (fname != NULL && edbenv->lg_info != NULL)
			F_SET(edbp, DB_AM_LOGGING);
	}

	/* Set the common fields. */
	if (edbinfo == NULL) {
		edbp->pgsize = 0;
		edbp->edb_malloc = NULL;
		edbp->dup_compare = NULL;
	} else {
		/*
		 * We don't want anything that's not a power-of-2, as we rely
		 * on that for alignment of various types on the pages.
		 */
		if ((edbp->pgsize = edbinfo->edb_pagesize) != 0 &&
		    (u_int32_t)1 << __edb_log2(edbp->pgsize) != edbp->pgsize) {
			__edb_err(edbenv, "page sizes must be a power-of-2");
			goto einval;
		}
		edbp->pgsize = edbinfo->edb_pagesize;
		edbp->edb_malloc = edbinfo->edb_malloc;
		if (F_ISSET(edbinfo, DB_DUPSORT)) {
			if (F_ISSET(edbinfo, DB_DUP))
				edbp->dup_compare = edbinfo->dup_compare == NULL ?
				    __bam_defcmp : edbinfo->dup_compare;
			else {
				__edb_err(edbenv, "DB_DUPSORT requires DB_DUP");
				goto einval;
			}
			F_CLR(edbinfo, DB_DUPSORT);
		}
	}

	/* Fill in the default file mode. */
	if (mode == 0)
		mode = __edb_omode("rwrw--");

	/* Check if the user wants us to swap byte order. */
	if (edbinfo != NULL)
		switch (ret = __edb_byteorder(edbenv, edbinfo->edb_lorder)) {
		case 0:
			break;
		case DB_SWAPBYTES:
			F_SET(edbp, DB_AM_SWAP);
			break;
		default:
			goto err;
		}
	edbp->byteswapped = F_ISSET(edbp, DB_AM_SWAP) ? 1 : 0;

	/*
	 * If we have a file name, try and read the first page, figure out
	 * what type of file it is, and initialize everything we can based
	 * on that file's meta-data page.
	 *
	 * XXX
	 * We don't actually expect zero-length strings as arguments.  We
	 * do the check, permitting them, because scripting languages, e.g.,
	 * the Tcl test suite, doesn't know anything about passing NULL's.
	 */
	if (fname != NULL && fname[0] != '\0') {
		/* Get the real file name. */
		if ((ret = __edb_appname(edbenv,
		     DB_APP_DATA, NULL, fname, 0, NULL, &real_name)) != 0)
			goto err;

		/*
		 * Open the backing file.  We need to make sure that multiple
		 * processes attempting to create the file at the same time
		 * are properly ordered so that only one of them creates the
		 * "unique" file id, so we open it O_EXCL and O_CREAT so two
		 * simultaneous attempts to create the region will return
		 * failure in one of the attempts.  If we're one of the ones
		 * that fail, we simply retry without the O_CREAT flag, which
		 * will require that the meta-data page exist.
		 */
		retry_cnt = 0;
open_retry:	if (LF_ISSET(DB_CREATE)) {
			if ((ret = __edb_open(real_name, flags | DB_EXCL,
			    OKFLAGS | DB_EXCL, mode, &fd)) != 0)
				if (ret == EEXIST) {
					LF_CLR(DB_CREATE);
					goto open_retry;
				} else {
					__edb_err(edbenv,
					    "%s: %s", fname, strerror(ret));
					goto err;
				}
		} else
			if ((ret = __edb_open(real_name,
			    flags, OKFLAGS, mode, &fd)) != 0) {
				__edb_err(edbenv, "%s: %s", fname, strerror(ret));
				goto err;
			}

		/*
		 * Use the optimum I/O size as the pagesize if a pagesize not
		 * specified.  Some filesystems have 64K as their optimum I/O
		 * size, but as that results in impossibly large default cache
		 * sizes, we limit the default pagesize to 16K.
		 */
		if (edbp->pgsize == 0) {
			if ((ret = __edb_os_ioinfo(real_name,
			    fd, NULL, NULL, &iopsize)) != 0) {
				__edb_err(edbenv,
				    "%s: %s", real_name, strerror(ret));
				goto err;
			}
			if (iopsize < 512)
				iopsize = 512;
			if (iopsize > 16 * 1024)
				iopsize = 16 * 1024;

			/*
			 * Sheer paranoia, but we don't want anything that's
			 * not a power-of-2, as we rely on that for alignment
			 * of various types on the pages.
			 */
			DB_ROUNDOFF(iopsize, 512);

			edbp->pgsize = iopsize;
			F_SET(edbp, DB_AM_PGDEF);
		}

		/*
		 * Try and read the first disk sector -- this code assumes
		 * that the meta-data for all access methods fits in 512
		 * bytes, and that no database will be smaller than that.
		 */
		if ((ret = __edb_os_read(fd, mbuf, sizeof(mbuf), &nr)) != 0)
			goto err;

		if (LF_ISSET(DB_FCNTL_LOCKING))
			edbp->saved_open_fd = fd;
		else
			(void)__edb_os_close(fd);
		fd = -1;

		if (nr != sizeof(mbuf)) {
			if (nr != 0) {
				__edb_err(edbenv,
				    "%s: unexpected file format", fname);
				goto einval;
			}
			/*
			 * The only way we can reach here with the DB_CREATE
			 * flag set is if we created the file.  If that's not
			 * the case, then a) someone else created the file
			 * but has not yet written out the meta-data page, or
			 * b) we truncated the file (DB_TRUNCATE) leaving it
			 * zero-length.  In the case of a), we want to sleep
			 * and give the file creator some time to write the
			 * metadata page.  In the case of b), charge forward.
			 * Note, there is a race in the case of two processes
			 * opening the file with the DB_TRUNCATE flag set at
			 * roughly the same time, and they could theoretically
			 * hurt each other, although it's pretty unlikely.
			 */
			if (retry_cnt++ < 3 &&
			    !LF_ISSET(DB_CREATE | DB_TRUNCATE)) {
				__edb_os_sleep(1, 0);
				goto open_retry;
			}
			if (type == DB_UNKNOWN) {
				__edb_err(edbenv,
				    "%s: DBTYPE of unknown with empty file",
				    fname);
				goto einval;
			}
			goto empty;
		}

		/*
		 * A found file overrides some user information.  We'll check
		 * for possible error conditions based on conflicts between
		 * the file and the user's arguments below.
		 */
		swapped = 0;
		F_CLR(edbp, DB_AM_SWAP);

retry:		switch (((BTMETA *)mbuf)->magic) {
		case DB_BTREEMAGIC:
			if (type != DB_BTREE &&
			    type != DB_RECNO && type != DB_UNKNOWN)
				goto einval;

			btm = (BTMETA *)mbuf;
			if (swapped && (ret = __bam_mswap((PAGE *)btm)) != 0)
				goto err;

			if (btm->version < DB_BTREEOLDVER ||
			    btm->version > DB_BTREEVERSION) {
				__edb_err(edbenv,
				    "%s: unsupported btree version number %lu",
				    fname, (u_long)btm->version);
				goto einval;
			}
			edbp->pgsize = btm->pagesize;
			F_CLR(edbp, DB_AM_PGDEF);

			if ((ret = __edb_fchk(edbenv,
			    "db_open", btm->flags, BTM_MASK)) != 0)
				goto err;
			DBINFO_FCHK(edbp, "DB_DUP",
			    btm->flags, BTM_DUP, DB_AM_DUP);
			if (F_ISSET(btm, BTM_RECNO)) {
				DBINFO_FCHK(edbp, "DB_FIXEDLEN",
				    btm->flags, BTM_FIXEDLEN, DB_RE_FIXEDLEN);
				DBINFO_FCHK(edbp, "DB_RENUMBER",
				    btm->flags, BTM_RENUMBER, DB_RE_RENUMBER);
				type = DB_RECNO;
			} else {
				DBINFO_FCHK(edbp, "DB_RECNUM",
				    btm->flags, BTM_RECNUM, DB_BT_RECNUM);
				type = DB_BTREE;
			}

			/* Copy the file's unique id. */
			need_fileid = 0;
			memcpy(edbp->fileid, btm->uid, DB_FILE_ID_LEN);
			break;
		case DB_HASHMAGIC:
			if (type != DB_HASH && type != DB_UNKNOWN)
				goto einval;

			hashm = (HASHHDR *)mbuf;
			if (swapped && (ret = __ham_mswap((PAGE *)hashm)) != 0)
				goto err;

			if (hashm->version < DB_HASHOLDVER ||
			    hashm->version > DB_HASHVERSION) {
				__edb_err(edbenv,
				    "%s: unsupported hash version number %lu",
				    fname, hashm->version);
				goto einval;
			}
			edbp->pgsize = hashm->pagesize;
			F_CLR(edbp, DB_AM_PGDEF);

			if ((ret = __edb_fchk(edbenv,
			    "db_open", hashm->flags, DB_HASH_DUP)) != 0)
				goto err;
			DBINFO_FCHK(edbp, "DB_DUP",
			    hashm->flags, DB_HASH_DUP, DB_AM_DUP);
			type = DB_HASH;

			/* Copy the file's unique id. */
			need_fileid = 0;
			memcpy(edbp->fileid, hashm->uid, DB_FILE_ID_LEN);
			break;
		default:
			if (swapped) {
				__edb_err(edbenv, "unrecognized file type");
				goto einval;
			}
			M_32_SWAP(((BTMETA *)mbuf)->magic);
			F_SET(edbp, DB_AM_SWAP);

			swapped = 1;
			goto retry;
		}
	} else {
		fname = real_name = NULL;

		if (type == DB_UNKNOWN) {
			__edb_err(edbenv,
			    "DBTYPE of unknown without existing file");
			goto einval;
		}
		F_SET(edbp, DB_AM_INMEM);
	}

empty:	/*
	 * By the time we get here we've either set the type or we're taking
	 * it from the user.
	 */
	edbp->type = type;

	/*
	 * Set the page size to the best value for I/O to this file.  Don't
	 * overflow the page offset type.  The page size must be edb_indx_t
	 * aligned and >= MIN_PAGE_SIZE.
	 *
	 * XXX
	 * Should we be checking for a page size that's not a multiple of 512?
	 */
	if (edbp->pgsize == 0) {
		F_SET(edbp, DB_AM_PGDEF);
		edbp->pgsize = 8 * 1024;
	}
	if (edbp->pgsize < DB_MIN_PGSIZE ||
	    edbp->pgsize > DB_MAX_PGSIZE ||
	    edbp->pgsize & (sizeof(edb_indx_t) - 1)) {
		__edb_err(edbenv, "illegal page size");
		goto einval;
	}

	/*
	 * If no mpool supplied by the application, attach to a local,
	 * created buffer pool.
	 *
	 * XXX
	 * If the user has a DB_ENV structure, we have to use a temporary
	 * one so that we don't step on their values.  If the user doesn't,
	 * we have to create one, and keep it around until the call to the
	 * memp_close() function.  This is all so the mpool functions get
	 * the error stuff right.
	 */
	if (edbenv == NULL || edbenv->mp_info == NULL) {
		F_SET(edbp, DB_AM_MLOCAL);

		if (edbenv == NULL) {
			if ((ret = __edb_os_calloc(1,
			    sizeof(DB_ENV), &edbp->mp_edbenv)) != 0)
				goto err;

			envp = edbp->mp_edbenv;
			restore = 0;
		} else {
			t_edbenv = *edbenv;

			envp = edbenv;
			restore = 1;
		}

		/*
		 * Set and/or correct the cache size; must be a multiple of
		 * the page size.
		 */
		if (edbinfo == NULL || edbinfo->edb_cachesize == 0)
			cachesize = edbp->pgsize * DB_MINCACHE;
		else {
			cachesize = edbinfo->edb_cachesize;
			if (cachesize & (edbp->pgsize - 1))
				cachesize +=
				    (~cachesize & (edbp->pgsize - 1)) + 1;
			if (cachesize < edbp->pgsize * DB_MINCACHE)
				cachesize = edbp->pgsize * DB_MINCACHE;
			if (cachesize < 20 * 1024)
				cachesize = 20 * 1024;
		}
		envp->mp_size = cachesize;

		if ((ret = memp_open(NULL, DB_CREATE | DB_MPOOL_PRIVATE |
		    (F_ISSET(edbp, DB_AM_THREAD) ? DB_THREAD : 0),
		    __edb_omode("rw----"), envp, &edbp->mp)) != 0)
			goto err;
		if (restore)
			*edbenv = t_edbenv;
	} else
		edbp->mp = edbenv->mp_info;

	/* Register DB's pgin/pgout functions. */
	if ((ret = memp_register(edbp->mp,
	    DB_FTYPE_BTREE, __bam_pgin, __bam_pgout)) != 0)
		goto err;
	if ((ret = memp_register(edbp->mp,
	    DB_FTYPE_HASH, __ham_pgin, __ham_pgout)) != 0)
		goto err;

	/*
	 * If we don't already have one, get a unique file ID.  If the file
	 * is a temporary file, then we have to create a unique file ID --
	 * no backing file will be created until the mpool cache is filled
	 * forcing it to go to disk.  The created ID must never match any
	 * potential real file ID -- we know it won't because real file IDs
	 * contain a time stamp after the dev/ino pair, and we're simply
	 * storing a 4-byte locker ID.
	 *
	 * XXX
	 * Store the file id in the locker structure -- we can get it from
	 * there as necessary, and it saves having two copies.
	 */
	if (need_fileid)
		if (fname == NULL) {
			memset(edbp->fileid, 0, DB_FILE_ID_LEN);
			if (F_ISSET(edbp, DB_AM_LOCKING) &&
			    (ret = lock_id(edbenv->lk_info,
			    (u_int32_t *)edbp->fileid)) != 0)
				goto err;
		} else
			if ((ret = __edb_os_fileid(edbenv,
			    real_name, 1, edbp->fileid)) != 0)
				goto err;

	/* No further use for the real name. */
	if (real_name != NULL)
		__edb_os_freestr(real_name);
	real_name = NULL;

	/*
	 * Open a backing file in the memory pool.
	 *
	 * If we need to process the file's pages on I/O, set the file type.
	 * If it's a hash file, always call pgin and pgout routines.  This
	 * means that hash files can never be mapped into process memory.  If
	 * it's a btree file and requires swapping, we need to page the file
	 * in and out.  This has to be right -- we can't mmap files that are
	 * being paged in and out.
	 */
	if (type == DB_HASH)
		ftype = DB_FTYPE_HASH;
	else
		ftype = F_ISSET(edbp, DB_AM_SWAP) ? DB_FTYPE_BTREE : 0;
	pginfo.edb_pagesize = edbp->pgsize;
	pginfo.needswap = F_ISSET(edbp, DB_AM_SWAP);
	pgcookie.data = &pginfo;
	pgcookie.size = sizeof(DB_PGINFO);

	/*
	 * Set up additional memp_fopen information.
	 */
	memset(&finfo, 0, sizeof(finfo));
	finfo.ftype = ftype;
	finfo.pgcookie = &pgcookie;
	finfo.fileid = edbp->fileid;
	finfo.lsn_offset = 0;
	finfo.clear_len = DB_PAGE_CLEAR_LEN;
	if ((ret = memp_fopen(edbp->mp, fname,
	    F_ISSET(edbp, DB_AM_RDONLY) ? DB_RDONLY : 0,
	    0, edbp->pgsize, &finfo, &edbp->mpf)) != 0)
		goto err;

	/*
	 * XXX
	 * We need a per-thread mutex that lives in shared memory -- HP-UX
	 * can't allocate mutexes in malloc'd memory.  Allocate it from the
	 * shared memory region, since it's the only one that is guaranteed
	 * to exist.
	 */
	if (F_ISSET(edbp, DB_AM_THREAD)) {
		if ((ret = __memp_reg_alloc(edbp->mp,
		    sizeof(edb_mutex_t), NULL, &edbp->mutexp)) != 0)
			goto err;
		/*
		 * Since we only get here if DB_THREAD was specified, we know
		 * we have spinlocks and no file offset argument is needed.
		 */
		(void)__edb_mutex_init(edbp->mutexp, 0);
	}

	/* Get a log file id. */
	if (F_ISSET(edbp, DB_AM_LOGGING) &&
	    (ret = log_register(edbenv->lg_info,
	    edbp, fname, type, &edbp->log_fileid)) != 0)
		goto err;

	/* Call the real open function. */
	switch (type) {
	case DB_BTREE:
		if (edbinfo != NULL && (ret = __edb_fchk(edbenv,
		    "db_open", edbinfo->flags, DB_RECNUM | DB_DUP)) != 0)
			goto err;
		if (edbinfo != NULL && (ret = __edb_fcchk(edbenv,
		    "db_open", edbinfo->flags, DB_DUP, DB_RECNUM)) != 0)
			goto err;
		if ((ret = __bam_open(edbp, edbinfo)) != 0)
			goto err;
		break;
	case DB_HASH:
		if (edbinfo != NULL && (ret = __edb_fchk(edbenv,
		    "db_open", edbinfo->flags, DB_DUP)) != 0)
			goto err;
		if ((ret = __ham_open(edbp, edbinfo)) != 0)
			goto err;
		break;
	case DB_RECNO:
#define	DB_INFO_FLAGS \
	(DB_DELIMITER | DB_FIXEDLEN | DB_PAD | DB_RENUMBER | DB_SNAPSHOT)
		if (edbinfo != NULL && (ret = __edb_fchk(edbenv,
		    "db_open", edbinfo->flags, DB_INFO_FLAGS)) != 0)
			goto err;
		if ((ret = __ram_open(edbp, edbinfo)) != 0)
			goto err;
		break;
	default:
		abort();
	}

	*edbpp = edbp;
	return (0);

einval:	ret = EINVAL;
err:	/* Close the file descriptor. */
	if (fd != -1)
		(void)__edb_os_close(fd);

	/* Discard the log file id. */
	if (edbp->log_fileid != 0)
		(void)log_unregister(edbenv->lg_info, edbp->log_fileid);

	/* Close the memory pool file. */
	if (edbp->mpf != NULL)
		(void)memp_fclose(edbp->mpf);

	/* If the memory pool was local, close it. */
	if (F_ISSET(edbp, DB_AM_MLOCAL) && edbp->mp != NULL)
		(void)memp_close(edbp->mp);

	/* If we allocated a DB_ENV, discard it. */
	if (edbp->mp_edbenv != NULL)
		__edb_os_free(edbp->mp_edbenv, sizeof(DB_ENV));

	if (real_name != NULL)
		__edb_os_freestr(real_name);
	if (edbp != NULL)
		__edb_os_free(edbp, sizeof(DB));

	return (ret);
}

/*
 * __edb_close --
 *	Close a DB tree.
 *
 * PUBLIC: int __edb_close __P((DB *, u_int32_t));
 */
int
__edb_close(edbp, flags)
	DB *edbp;
	u_int32_t flags;
{
	DBC *edbc;
	int ret, t_ret;

	DB_PANIC_CHECK(edbp);

	/* Validate arguments. */
	if ((ret = __edb_closechk(edbp, flags)) != 0)
		return (ret);

	/* Sync the underlying file. */
	if (flags != DB_NOSYNC &&
	    (t_ret = edbp->sync(edbp, 0)) != 0 && ret == 0)
		ret = t_ret;

	/*
	 * Go through the active cursors and call the cursor recycle routine,
	 * which resolves pending operations and moves the cursors onto the
	 * free list.  Then, walk the free list and call the cursor destroy
	 * routine.
	 */
	while ((edbc = TAILQ_FIRST(&edbp->active_queue)) != NULL)
		if ((t_ret = edbc->c_close(edbc)) != 0 && ret == 0)
			ret = t_ret;
	while ((edbc = TAILQ_FIRST(&edbp->free_queue)) != NULL)
		if ((t_ret = __edb_c_destroy(edbc)) != 0 && ret == 0)
			ret = t_ret;

	/* Call the access specific close function. */
	if ((t_ret = edbp->am_close(edbp)) != 0 && ret == 0)
		ret = t_ret;

	/* Sync the memory pool. */
	if (flags != DB_NOSYNC && (t_ret = memp_fsync(edbp->mpf)) != 0 &&
	    t_ret != DB_INCOMPLETE && ret == 0)
		ret = t_ret;

	/* Close the memory pool file. */
	if ((t_ret = memp_fclose(edbp->mpf)) != 0 && ret == 0)
		ret = t_ret;

	/* If the memory pool was local, close it. */
	if (F_ISSET(edbp, DB_AM_MLOCAL) &&
	    (t_ret = memp_close(edbp->mp)) != 0 && ret == 0)
		ret = t_ret;

	if (edbp->saved_open_fd != -1) {
		(void)__edb_os_close(edbp->saved_open_fd);
		edbp->saved_open_fd = -1;
	}

	/* Discard the log file id. */
	if (F_ISSET(edbp, DB_AM_LOGGING))
		(void)log_unregister(edbp->edbenv->lg_info, edbp->log_fileid);

	/* If we allocated a DB_ENV, discard it. */
	if (edbp->mp_edbenv != NULL)
		__edb_os_free(edbp->mp_edbenv, sizeof(DB_ENV));

	/* Free the DB. */
	__edb_os_free(edbp, sizeof(*edbp));

	return (ret);
}
