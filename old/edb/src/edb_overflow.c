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
 * This code is derived from software contributed to Berkeley by
 * Mike Olson.
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
static const char sccsid[] = "@(#)edb_overflow.c	10.21 (Sleepycat) 9/27/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "edb_page.h"
#include "edb_am.h"
#include "common_ext.h"

/*
 * Big key/data code.
 *
 * Big key and data entries are stored on linked lists of pages.  The initial
 * reference is a structure with the total length of the item and the page
 * number where it begins.  Each entry in the linked list contains a pointer
 * to the next page of data, and so on.
 */

/*
 * __edb_goff --
 *	Get an offpage item.
 *
 * PUBLIC: int __edb_goff __P((DB *, DBT *,
 * PUBLIC:     u_int32_t, edb_pgno_t, void **, u_int32_t *));
 */
int
__edb_goff(edbp, edbt, tlen, pgno, bpp, bpsz)
	DB *edbp;
	DBT *edbt;
	u_int32_t tlen;
	edb_pgno_t pgno;
	void **bpp;
	u_int32_t *bpsz;
{
	PAGE *h;
	edb_indx_t bytes;
	u_int32_t curoff, needed, start;
	u_int8_t *p, *src;
	int ret;

	/*
	 * Check if the buffer is big enough; if it is not and we are
	 * allowed to malloc space, then we'll malloc it.  If we are
	 * not (DB_DBT_USERMEM), then we'll set the edbt and return
	 * appropriately.
	 */
	if (F_ISSET(edbt, DB_DBT_PARTIAL)) {
		start = edbt->doff;
		needed = edbt->dlen;
	} else {
		start = 0;
		needed = tlen;
	}

	/* Allocate any necessary memory. */
	if (F_ISSET(edbt, DB_DBT_USERMEM)) {
		if (needed > edbt->ulen) {
			edbt->size = needed;
			return (ENOMEM);
		}
	} else if (F_ISSET(edbt, DB_DBT_MALLOC)) {
		if ((ret =
		    __edb_os_malloc(needed, edbp->edb_malloc, &edbt->data)) != 0)
			return (ret);
	} else if (*bpsz == 0 || *bpsz < needed) {
		if ((ret = __edb_os_realloc(bpp, needed)) != 0)
			return (ret);
		*bpsz = needed;
		edbt->data = *bpp;
	} else
		edbt->data = *bpp;

	/*
	 * Step through the linked list of pages, copying the data on each
	 * one into the buffer.  Never copy more than the total data length.
	 */
	edbt->size = needed;
	for (curoff = 0, p = edbt->data; pgno != P_INVALID && needed > 0;) {
		if ((ret = memp_fget(edbp->mpf, &pgno, 0, &h)) != 0) {
			(void)__edb_pgerr(edbp, pgno);
			return (ret);
		}
		/* Check if we need any bytes from this page. */
		if (curoff + OV_LEN(h) >= start) {
			src = (u_int8_t *)h + P_OVERHEAD;
			bytes = OV_LEN(h);
			if (start > curoff) {
				src += start - curoff;
				bytes -= start - curoff;
			}
			if (bytes > needed)
				bytes = needed;
			memcpy(p, src, bytes);
			p += bytes;
			needed -= bytes;
		}
		curoff += OV_LEN(h);
		pgno = h->next_pgno;
		memp_fput(edbp->mpf, h, 0);
	}
	return (0);
}

/*
 * __edb_poff --
 *	Put an offpage item.
 *
 * PUBLIC: int __edb_poff __P((DBC *, const DBT *, edb_pgno_t *,
 * PUBLIC:     int (*)(DBC *, u_int32_t, PAGE **)));
 */
int
__edb_poff(edbc, edbt, pgnop, newfunc)
	DBC *edbc;
	const DBT *edbt;
	edb_pgno_t *pgnop;
	int (*newfunc) __P((DBC *, u_int32_t, PAGE **));
{
	DB *edbp;
	PAGE *pagep, *lastp;
	DB_LSN new_lsn, null_lsn;
	DBT tmp_edbt;
	edb_indx_t pagespace;
	u_int32_t sz;
	u_int8_t *p;
	int ret;

	/*
	 * Allocate pages and copy the key/data item into them.  Calculate the
	 * number of bytes we get for pages we fill completely with a single
	 * item.
	 */
	edbp = edbc->edbp;
	pagespace = P_MAXSPACE(edbp->pgsize);

	lastp = NULL;
	for (p = edbt->data,
	    sz = edbt->size; sz > 0; p += pagespace, sz -= pagespace) {
		/*
		 * Reduce pagespace so we terminate the loop correctly and
		 * don't copy too much data.
		 */
		if (sz < pagespace)
			pagespace = sz;

		/*
		 * Allocate and initialize a new page and copy all or part of
		 * the item onto the page.  If sz is less than pagespace, we
		 * have a partial record.
		 */
		if ((ret = newfunc(edbc, P_OVERFLOW, &pagep)) != 0)
			return (ret);
		if (DB_LOGGING(edbc)) {
			tmp_edbt.data = p;
			tmp_edbt.size = pagespace;
			ZERO_LSN(null_lsn);
			if ((ret = __edb_big_log(edbp->edbenv->lg_info, edbc->txn,
			    &new_lsn, 0, DB_ADD_BIG, edbp->log_fileid,
			    PGNO(pagep), lastp ? PGNO(lastp) : PGNO_INVALID,
			    PGNO_INVALID, &tmp_edbt, &LSN(pagep),
			    lastp == NULL ? &null_lsn : &LSN(lastp),
			    &null_lsn)) != 0)
				return (ret);

			/* Move lsn onto page. */
			if (lastp)
				LSN(lastp) = new_lsn;
			LSN(pagep) = new_lsn;
		}

		P_INIT(pagep, edbp->pgsize,
		    PGNO(pagep), PGNO_INVALID, PGNO_INVALID, 0, P_OVERFLOW);
		OV_LEN(pagep) = pagespace;
		OV_REF(pagep) = 1;
		memcpy((u_int8_t *)pagep + P_OVERHEAD, p, pagespace);

		/*
		 * If this is the first entry, update the user's info.
		 * Otherwise, update the entry on the last page filled
		 * in and release that page.
		 */
		if (lastp == NULL)
			*pgnop = PGNO(pagep);
		else {
			lastp->next_pgno = PGNO(pagep);
			pagep->prev_pgno = PGNO(lastp);
			(void)memp_fput(edbp->mpf, lastp, DB_MPOOL_DIRTY);
		}
		lastp = pagep;
	}
	(void)memp_fput(edbp->mpf, lastp, DB_MPOOL_DIRTY);
	return (0);
}

/*
 * __edb_ovref --
 *	Increment/decrement the reference count on an overflow page.
 *
 * PUBLIC: int __edb_ovref __P((DBC *, edb_pgno_t, int32_t));
 */
int
__edb_ovref(edbc, pgno, adjust)
	DBC *edbc;
	edb_pgno_t pgno;
	int32_t adjust;
{
	DB *edbp;
	PAGE *h;
	int ret;

	edbp = edbc->edbp;
	if ((ret = memp_fget(edbp->mpf, &pgno, 0, &h)) != 0) {
		(void)__edb_pgerr(edbp, pgno);
		return (ret);
	}

	if (DB_LOGGING(edbc))
		if ((ret = __edb_ovref_log(edbp->edbenv->lg_info, edbc->txn,
		    &LSN(h), 0, edbp->log_fileid, h->pgno, adjust,
		    &LSN(h))) != 0)
			return (ret);
	OV_REF(h) += adjust;

	(void)memp_fput(edbp->mpf, h, DB_MPOOL_DIRTY);
	return (0);
}

/*
 * __edb_doff --
 *	Delete an offpage chain of overflow pages.
 *
 * PUBLIC: int __edb_doff __P((DBC *, edb_pgno_t, int (*)(DBC *, PAGE *)));
 */
int
__edb_doff(edbc, pgno, freefunc)
	DBC *edbc;
	edb_pgno_t pgno;
	int (*freefunc) __P((DBC *, PAGE *));
{
	DB *edbp;
	PAGE *pagep;
	DB_LSN null_lsn;
	DBT tmp_edbt;
	int ret;

	edbp = edbc->edbp;
	do {
		if ((ret = memp_fget(edbp->mpf, &pgno, 0, &pagep)) != 0) {
			(void)__edb_pgerr(edbp, pgno);
			return (ret);
		}

		/*
		 * If it's an overflow page and it's referenced by more than
		 * one key/data item, decrement the reference count and return.
		 */
		if (TYPE(pagep) == P_OVERFLOW && OV_REF(pagep) > 1) {
			(void)memp_fput(edbp->mpf, pagep, 0);
			return (__edb_ovref(edbc, pgno, -1));
		}

		if (DB_LOGGING(edbc)) {
			tmp_edbt.data = (u_int8_t *)pagep + P_OVERHEAD;
			tmp_edbt.size = OV_LEN(pagep);
			ZERO_LSN(null_lsn);
			if ((ret = __edb_big_log(edbp->edbenv->lg_info, edbc->txn,
			    &LSN(pagep), 0, DB_REM_BIG, edbp->log_fileid,
			    PGNO(pagep), PREV_PGNO(pagep), NEXT_PGNO(pagep),
			    &tmp_edbt, &LSN(pagep), &null_lsn, &null_lsn)) != 0)
				return (ret);
		}
		pgno = pagep->next_pgno;
		if ((ret = freefunc(edbc, pagep)) != 0)
			return (ret);
	} while (pgno != PGNO_INVALID);

	return (0);
}

/*
 * __edb_moff --
 *	Match on overflow pages.
 *
 * Given a starting page number and a key, return <0, 0, >0 to indicate if the
 * key on the page is less than, equal to or greater than the key specified.
 * We optimize this by doing chunk at a time comparison unless the user has
 * specified a comparison function.  In this case, we need to materialize
 * the entire object and call their comparison routine.
 *
 * PUBLIC: int __edb_moff __P((DB *, const DBT *, edb_pgno_t, u_int32_t,
 * PUBLIC:     int (*)(const DBT *, const DBT *), int *));
 */
int
__edb_moff(edbp, edbt, pgno, tlen, cmpfunc, cmpp)
	DB *edbp;
	const DBT *edbt;
	edb_pgno_t pgno;
	u_int32_t tlen;
	int (*cmpfunc) __P((const DBT *, const DBT *)), *cmpp;
{
	PAGE *pagep;
	DBT local_edbt;
	void *buf;
	u_int32_t bufsize, cmp_bytes, key_left;
	u_int8_t *p1, *p2;
	int ret;

	/*
	 * If there is a user-specified comparison function, build a
	 * contiguous copy of the key, and call it.
	 */
	if (cmpfunc != NULL) {
		memset(&local_edbt, 0, sizeof(local_edbt));
		buf = NULL;
		bufsize = 0;

		if ((ret = __edb_goff(edbp,
		    &local_edbt, tlen, pgno, &buf, &bufsize)) != 0)
			return (ret);
		*cmpp = cmpfunc(&local_edbt, edbt);
		__edb_os_free(buf, bufsize);
		return (0);
	}

	/* While there are both keys to compare. */
	for (*cmpp = 0, p1 = edbt->data,
	    key_left = edbt->size; key_left > 0 && pgno != PGNO_INVALID;) {
		if ((ret = memp_fget(edbp->mpf, &pgno, 0, &pagep)) != 0)
			return (ret);

		cmp_bytes = OV_LEN(pagep) < key_left ? OV_LEN(pagep) : key_left;
		key_left -= cmp_bytes;
		for (p2 =
		    (u_int8_t *)pagep + P_OVERHEAD; cmp_bytes-- > 0; ++p1, ++p2)
			if (*p1 != *p2) {
				*cmpp = (long)*p1 - (long)*p2;
				break;
			}
		pgno = NEXT_PGNO(pagep);
		if ((ret = memp_fput(edbp->mpf, pagep, 0)) != 0)
			return (ret);
		if (*cmpp != 0)
			return (0);
	}
	if (key_left > 0)		/* DBT is longer than page key. */
		*cmpp = -1;
	else if (pgno != PGNO_INVALID)	/* DBT is shorter than page key. */
		*cmpp = 1;
	else
		*cmpp = 0;

	return (0);
}
