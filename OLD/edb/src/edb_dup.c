/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)edb_dup.c	10.35 (Sleepycat) 12/2/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "edb_page.h"
#include "btree.h"
#include "edb_am.h"

static int __edb_addpage __P((DBC *,
    PAGE **, edb_indx_t *, int (*)(DBC *, u_int32_t, PAGE **)));
static int __edb_dsplit __P((DBC *,
    PAGE **, edb_indx_t *, u_int32_t, int (*)(DBC *, u_int32_t, PAGE **)));

/*
 * __edb_dput --
 *	Put a duplicate item onto a duplicate page at the given index.
 *
 * PUBLIC: int __edb_dput __P((DBC *, DBT *,
 * PUBLIC:    PAGE **, edb_indx_t *, int (*)(DBC *, u_int32_t, PAGE **)));
 */
int
__edb_dput(edbc, edbt, pp, indxp, newfunc)
	DBC *edbc;
	DBT *edbt;
	PAGE **pp;
	edb_indx_t *indxp;
	int (*newfunc) __P((DBC *, u_int32_t, PAGE **));
{
	BOVERFLOW bo;
	DBT *data_edbtp, hdr_edbt, *hdr_edbtp;
	PAGE *pagep;
	edb_indx_t size, isize;
	edb_pgno_t pgno;
	int ret;

	/*
	 * We need some access method independent threshold for when we put
	 * a duplicate item onto an overflow page.
	 */
	if (edbt->size > 0.25 * edbc->edbp->pgsize) {
		if ((ret = __edb_poff(edbc, edbt, &pgno, newfunc)) != 0)
			return (ret);
		UMRW(bo.unused1);
		B_TSET(bo.type, B_OVERFLOW, 0);
		UMRW(bo.unused2);
		bo.tlen = edbt->size;
		bo.pgno = pgno;
		hdr_edbt.data = &bo;
		hdr_edbt.size = isize = BOVERFLOW_SIZE;
		hdr_edbtp = &hdr_edbt;
		size = BOVERFLOW_PSIZE;
		data_edbtp = NULL;
	} else {
		size = BKEYDATA_PSIZE(edbt->size);
		isize = BKEYDATA_SIZE(edbt->size);
		hdr_edbtp = NULL;
		data_edbtp = edbt;
	}

	pagep = *pp;
	if (size > P_FREESPACE(pagep)) {
		if (*indxp == NUM_ENT(*pp) && NEXT_PGNO(*pp) == PGNO_INVALID)
			ret = __edb_addpage(edbc, pp, indxp, newfunc);
		else
			ret = __edb_dsplit(edbc, pp, indxp, isize, newfunc);
		if (ret != 0)
			/*
			 * XXX
			 * Pages not returned to free list.
			 */
			return (ret);
		pagep = *pp;
	}

	/*
	 * Now, pagep references the page on which to insert and indx is the
	 * the location to insert.
	 */
	if ((ret = __edb_pitem(edbc,
	    pagep, (u_int32_t)*indxp, isize, hdr_edbtp, data_edbtp)) != 0)
		return (ret);

	(void)memp_fset(edbc->edbp->mpf, pagep, DB_MPOOL_DIRTY);
	return (0);
}

/*
 * __edb_drem --
 *	Remove a duplicate at the given index on the given page.
 *
 * PUBLIC: int __edb_drem __P((DBC *,
 * PUBLIC:    PAGE **, u_int32_t, int (*)(DBC *, PAGE *)));
 */
int
__edb_drem(edbc, pp, indx, freefunc)
	DBC *edbc;
	PAGE **pp;
	u_int32_t indx;
	int (*freefunc) __P((DBC *, PAGE *));
{
	PAGE *pagep;
	int ret;

	pagep = *pp;

	/* Check if we are freeing a big item. */
	if (B_TYPE(GET_BKEYDATA(pagep, indx)->type) == B_OVERFLOW) {
		if ((ret = __edb_doff(edbc,
		    GET_BOVERFLOW(pagep, indx)->pgno, freefunc)) != 0)
			return (ret);
		ret = __edb_ditem(edbc, pagep, indx, BOVERFLOW_SIZE);
	} else
		ret = __edb_ditem(edbc, pagep, indx,
		    BKEYDATA_SIZE(GET_BKEYDATA(pagep, indx)->len));
	if (ret != 0)
		return (ret);

	if (NUM_ENT(pagep) == 0) {
		/*
		 * If the page is emptied, then the page is freed and the pp
		 * parameter is set to reference the next, locked page in the
		 * duplicate chain, if one exists.  If there was no such page,
		 * then it is set to NULL.
		 *
		 * !!!
		 * __edb_relink will set the dirty bit for us.
		 */
		if ((ret = __edb_relink(edbc, DB_REM_PAGE, pagep, pp, 0)) != 0)
			return (ret);
		if ((ret = freefunc(edbc, pagep)) != 0)
			return (ret);
	} else
		(void)memp_fset(edbc->edbp->mpf, pagep, DB_MPOOL_DIRTY);

	return (0);
}

/*
 * __edb_dend --
 *	Find the last page in a set of offpage duplicates.
 *
 * PUBLIC: int __edb_dend __P((DBC *, edb_pgno_t, PAGE **));
 */
int
__edb_dend(edbc, pgno, pp)
	DBC *edbc;
	edb_pgno_t pgno;
	PAGE **pp;
{
	DB *edbp;
	PAGE *h;
	int ret;

	edbp = edbc->edbp;

	/*
	 * This implements DB_KEYLAST.  The last page is returned in pp; pgno
	 * should be the page number of the first page of the duplicate chain.
	 *
	 * *pp may be non-NULL -- if given a valid page use it.
	 */
	if (*pp != NULL)
		goto started;
	for (;;) {
		if ((ret = memp_fget(edbp->mpf, &pgno, 0, pp)) != 0) {
			(void)__edb_pgerr(edbp, pgno);
			return (ret);
		}
started:	h = *pp;

		if ((pgno = NEXT_PGNO(h)) == PGNO_INVALID)
			break;

		if ((ret = memp_fput(edbp->mpf, h, 0)) != 0)
			return (ret);
	}
	return (0);
}

/*
 * __edb_dsplit --
 *	Split a page of duplicates, calculating the split point based
 *	on an element of size "size" being added at "*indxp".
 *	On entry hp contains a pointer to the page-pointer of the original
 *	page.  On exit, it returns a pointer to the page containing "*indxp"
 *	and "indxp" has been modified to reflect the index on the new page
 *	where the element should be added.  The function returns with
 *	the page on which the insert should happen, not yet put.
 */
static int
__edb_dsplit(edbc, hp, indxp, size, newfunc)
	DBC *edbc;
	PAGE **hp;
	edb_indx_t *indxp;
	u_int32_t size;
	int (*newfunc) __P((DBC *, u_int32_t, PAGE **));
{
	PAGE *h, *np, *tp;
	BKEYDATA *bk;
	DBT page_edbt;
	DB *edbp;
	size_t pgsize;
	edb_indx_t halfbytes, i, indx, lastsum, nindex, oindex, s, sum;
	int did_indx, ret, t_ret;

	h = *hp;
	indx = *indxp;
	ret = 0;
	edbp = edbc->edbp;
	pgsize = edbp->pgsize;

	/* Create a temporary page to do compaction onto. */
	if ((ret = __edb_os_malloc(pgsize, NULL, &tp)) != 0)
		return (ret);

	/* Create new page for the split. */
	if ((ret = newfunc(edbc, P_DUPLICATE, &np)) != 0) {
		__edb_os_free(tp, pgsize);
		return (ret);
	}

	P_INIT(np, pgsize, PGNO(np), PGNO(h), NEXT_PGNO(h), 0,
	    P_DUPLICATE);
	P_INIT(tp, pgsize, PGNO(h), PREV_PGNO(h), PGNO(np), 0,
	    P_DUPLICATE);

	/* Figure out the split point */
	halfbytes = (pgsize - HOFFSET(h)) / 2;
	did_indx = 0;
	for (sum = 0, lastsum = 0, i = 0; i < NUM_ENT(h); i++) {
		if (i == indx) {
			sum += size;
			did_indx = 1;
			if (lastsum < halfbytes && sum >= halfbytes) {
				/* We've crossed the halfway point. */
				if ((edb_indx_t)(halfbytes - lastsum) <
				    (edb_indx_t)(sum - halfbytes)) {
					*hp = np;
					*indxp = 0;
				} else
					*indxp = i;
				break;
			}
			*indxp = i;
			lastsum = sum;
		}
		if (B_TYPE(GET_BKEYDATA(h, i)->type) == B_KEYDATA)
			sum += BKEYDATA_SIZE(GET_BKEYDATA(h, i)->len);
		else
			sum += BOVERFLOW_SIZE;

		if (lastsum < halfbytes && sum >= halfbytes) {
			/* We've crossed the halfway point. */
			if ((edb_indx_t)(sum - halfbytes) <
			    (edb_indx_t)(halfbytes - lastsum))
				i++;
			break;
		}
	}
	/*
	 * Check if we have set the return values of the index pointer and
	 * page pointer.
	 */
	if (!did_indx) {
		*hp = np;
		*indxp = indx - i;
	}

	if (DB_LOGGING(edbc)) {
		page_edbt.size = edbp->pgsize;
		page_edbt.data = h;
		if ((ret = __edb_split_log(edbp->edbenv->lg_info,
		    edbc->txn, &LSN(h), 0, DB_SPLITOLD, edbp->log_fileid,
		    PGNO(h), &page_edbt, &LSN(h))) != 0) {
			__edb_os_free(tp, pgsize);
			return (ret);
		}
		LSN(tp) = LSN(h);
	}

	/*
	 * If it's a btree, adjust the cursors.
	 *
	 * i is the index of the first element to move onto the new page.
	 */
	if (edbp->type == DB_BTREE)
		__bam_ca_split(edbp, PGNO(h), PGNO(h), PGNO(np), i, 0);

	for (nindex = 0, oindex = i; oindex < NUM_ENT(h); oindex++) {
		bk = GET_BKEYDATA(h, oindex);
		if (B_TYPE(bk->type) == B_KEYDATA)
			s = BKEYDATA_SIZE(bk->len);
		else
			s = BOVERFLOW_SIZE;

		np->inp[nindex++] = HOFFSET(np) -= s;
		memcpy((u_int8_t *)np + HOFFSET(np), bk, s);
		NUM_ENT(np)++;
	}

	/*
	 * Now do data compaction by copying the remaining stuff onto the
	 * temporary page and then copying it back to the real page.
	 */
	for (nindex = 0, oindex = 0; oindex < i; oindex++) {
		bk = GET_BKEYDATA(h, oindex);
		if (B_TYPE(bk->type) == B_KEYDATA)
			s = BKEYDATA_SIZE(bk->len);
		else
			s = BOVERFLOW_SIZE;

		tp->inp[nindex++] = HOFFSET(tp) -= s;
		memcpy((u_int8_t *)tp + HOFFSET(tp), bk, s);
		NUM_ENT(tp)++;
	}

	/*
	 * This page (the temporary) should be only half full, so we do two
	 * memcpy's, one for the top of the page and one for the bottom of
	 * the page.  This way we avoid copying the middle which should be
	 * about half a page.
	 */
	memcpy(h, tp, LOFFSET(tp));
	memcpy((u_int8_t *)h + HOFFSET(tp),
	    (u_int8_t *)tp + HOFFSET(tp), pgsize - HOFFSET(tp));
	__edb_os_free(tp, pgsize);

	if (DB_LOGGING(edbc)) {
		/*
		 * XXX
		 * If either of these fails, are we leaving pages pinned?
		 * Yes, but it seems like this happens in error case.
		 */
		page_edbt.size = pgsize;
		page_edbt.data = h;
		if ((ret = __edb_split_log(edbp->edbenv->lg_info,
		    edbc->txn, &LSN(h), 0, DB_SPLITNEW, edbp->log_fileid,
		    PGNO(h), &page_edbt, &LSN(h))) != 0)
			return (ret);

		page_edbt.size = pgsize;
		page_edbt.data = np;
		if ((ret = __edb_split_log(edbp->edbenv->lg_info,
		    edbc->txn, &LSN(np), 0, DB_SPLITNEW, edbp->log_fileid,
		    PGNO(np),  &page_edbt, &LSN(np))) != 0)
			return (ret);
	}

	/*
	 * Finally, if there was a next page after the page being
	 * split, fix its prev pointer.
	 */
	if (np->next_pgno != PGNO_INVALID)
	    ret = __edb_relink(edbc, DB_ADD_PAGE, np, NULL, 1);

	/*
	 * Figure out if the location we're interested in is on the new
	 * page, and if so, reset the callers' pointer.  Push the other
	 * page back to the store.
	 */
	if (*hp == h)
		t_ret = memp_fput(edbp->mpf, np, DB_MPOOL_DIRTY);
	else
		t_ret = memp_fput(edbp->mpf, h, DB_MPOOL_DIRTY);

	return (ret != 0 ? ret : t_ret);
}

/*
 * __edb_ditem --
 *	Remove an item from a page.
 *
 * PUBLIC:  int __edb_ditem __P((DBC *, PAGE *, u_int32_t, u_int32_t));
 */
int
__edb_ditem(edbc, pagep, indx, nbytes)
	DBC *edbc;
	PAGE *pagep;
	u_int32_t indx, nbytes;
{
	DB *edbp;
	DBT ledbt;
	edb_indx_t cnt, offset;
	int ret;
	u_int8_t *from;

	edbp = edbc->edbp;
	if (DB_LOGGING(edbc)) {
		ledbt.data = P_ENTRY(pagep, indx);
		ledbt.size = nbytes;
		if ((ret = __edb_addrem_log(edbp->edbenv->lg_info, edbc->txn,
		    &LSN(pagep), 0, DB_REM_DUP, edbp->log_fileid, PGNO(pagep),
		    (u_int32_t)indx, nbytes, &ledbt, NULL, &LSN(pagep))) != 0)
			return (ret);
	}

	/*
	 * If there's only a single item on the page, we don't have to
	 * work hard.
	 */
	if (NUM_ENT(pagep) == 1) {
		NUM_ENT(pagep) = 0;
		HOFFSET(pagep) = edbp->pgsize;
		return (0);
	}

	/*
	 * Pack the remaining key/data items at the end of the page.  Use
	 * memmove(3), the regions may overlap.
	 */
	from = (u_int8_t *)pagep + HOFFSET(pagep);
	memmove(from + nbytes, from, pagep->inp[indx] - HOFFSET(pagep));
	HOFFSET(pagep) += nbytes;

	/* Adjust the indices' offsets. */
	offset = pagep->inp[indx];
	for (cnt = 0; cnt < NUM_ENT(pagep); ++cnt)
		if (pagep->inp[cnt] < offset)
			pagep->inp[cnt] += nbytes;

	/* Shift the indices down. */
	--NUM_ENT(pagep);
	if (indx != NUM_ENT(pagep))
		memmove(&pagep->inp[indx], &pagep->inp[indx + 1],
		    sizeof(edb_indx_t) * (NUM_ENT(pagep) - indx));

	/* If it's a btree, adjust the cursors. */
	if (edbp->type == DB_BTREE)
		__bam_ca_di(edbp, PGNO(pagep), indx, -1);

	return (0);
}

/*
 * __edb_pitem --
 *	Put an item on a page.
 *
 * PUBLIC: int __edb_pitem
 * PUBLIC:     __P((DBC *, PAGE *, u_int32_t, u_int32_t, DBT *, DBT *));
 */
int
__edb_pitem(edbc, pagep, indx, nbytes, hdr, data)
	DBC *edbc;
	PAGE *pagep;
	u_int32_t indx;
	u_int32_t nbytes;
	DBT *hdr, *data;
{
	DB *edbp;
	BKEYDATA bk;
	DBT thdr;
	int ret;
	u_int8_t *p;

	/*
	 * Put a single item onto a page.  The logic figuring out where to
	 * insert and whether it fits is handled in the caller.  All we do
	 * here is manage the page shuffling.  We cheat a little bit in that
	 * we don't want to copy the edbt on a normal put twice.  If hdr is
	 * NULL, we create a BKEYDATA structure on the page, otherwise, just
	 * copy the caller's information onto the page.
	 *
	 * This routine is also used to put entries onto the page where the
	 * entry is pre-built, e.g., during recovery.  In this case, the hdr
	 * will point to the entry, and the data argument will be NULL.
	 *
	 * !!!
	 * There's a tremendous potential for off-by-one errors here, since
	 * the passed in header sizes must be adjusted for the structure's
	 * placeholder for the trailing variable-length data field.
	 */
	edbp = edbc->edbp;
	if (DB_LOGGING(edbc))
		if ((ret = __edb_addrem_log(edbp->edbenv->lg_info, edbc->txn,
		    &LSN(pagep), 0, DB_ADD_DUP, edbp->log_fileid, PGNO(pagep),
		    (u_int32_t)indx, nbytes, hdr, data, &LSN(pagep))) != 0)
			return (ret);

	if (hdr == NULL) {
		B_TSET(bk.type, B_KEYDATA, 0);
		bk.len = data == NULL ? 0 : data->size;

		thdr.data = &bk;
		thdr.size = SSZA(BKEYDATA, data);
		hdr = &thdr;
	}

	/* Adjust the index table, then put the item on the page. */
	if (indx != NUM_ENT(pagep))
		memmove(&pagep->inp[indx + 1], &pagep->inp[indx],
		    sizeof(edb_indx_t) * (NUM_ENT(pagep) - indx));
	HOFFSET(pagep) -= nbytes;
	pagep->inp[indx] = HOFFSET(pagep);
	++NUM_ENT(pagep);

	p = P_ENTRY(pagep, indx);
	memcpy(p, hdr->data, hdr->size);
	if (data != NULL)
		memcpy(p + hdr->size, data->data, data->size);

	/* If it's a btree, adjust the cursors. */
	if (edbp->type == DB_BTREE)
		__bam_ca_di(edbp, PGNO(pagep), indx, 1);

	return (0);
}

/*
 * __edb_relink --
 *	Relink around a deleted page.
 *
 * PUBLIC: int __edb_relink __P((DBC *, u_int32_t, PAGE *, PAGE **, int));
 */
int
__edb_relink(edbc, add_rem, pagep, new_next, needlock)
	DBC *edbc;
	u_int32_t add_rem;
	PAGE *pagep, **new_next;
	int needlock;
{
	DB *edbp;
	PAGE *np, *pp;
	DB_LOCK npl, ppl;
	DB_LSN *nlsnp, *plsnp;
	int ret;

	ret = 0;
	np = pp = NULL;
	npl = ppl = LOCK_INVALID;
	nlsnp = plsnp = NULL;
	edbp = edbc->edbp;

	/*
	 * Retrieve and lock the one/two pages.  For a remove, we may need
	 * two pages (the before and after).  For an add, we only need one
	 * because, the split took care of the prev.
	 */
	if (pagep->next_pgno != PGNO_INVALID) {
		if (needlock && (ret = __bam_lget(edbc,
		    0, pagep->next_pgno, DB_LOCK_WRITE, &npl)) != 0)
			goto err;
		if ((ret = memp_fget(edbp->mpf,
		    &pagep->next_pgno, 0, &np)) != 0) {
			(void)__edb_pgerr(edbp, pagep->next_pgno);
			goto err;
		}
		nlsnp = &np->lsn;
	}
	if (add_rem == DB_REM_PAGE && pagep->prev_pgno != PGNO_INVALID) {
		if (needlock && (ret = __bam_lget(edbc,
		    0, pagep->prev_pgno, DB_LOCK_WRITE, &ppl)) != 0)
			goto err;
		if ((ret = memp_fget(edbp->mpf,
		    &pagep->prev_pgno, 0, &pp)) != 0) {
			(void)__edb_pgerr(edbp, pagep->next_pgno);
			goto err;
		}
		plsnp = &pp->lsn;
	}

	/* Log the change. */
	if (DB_LOGGING(edbc)) {
		if ((ret = __edb_relink_log(edbp->edbenv->lg_info, edbc->txn,
		    &pagep->lsn, 0, add_rem, edbp->log_fileid,
		    pagep->pgno, &pagep->lsn,
		    pagep->prev_pgno, plsnp, pagep->next_pgno, nlsnp)) != 0)
			goto err;
		if (np != NULL)
			np->lsn = pagep->lsn;
		if (pp != NULL)
			pp->lsn = pagep->lsn;
	}

	/*
	 * Modify and release the two pages.
	 *
	 * !!!
	 * The parameter new_next gets set to the page following the page we
	 * are removing.  If there is no following page, then new_next gets
	 * set to NULL.
	 */
	if (np != NULL) {
		if (add_rem == DB_ADD_PAGE)
			np->prev_pgno = pagep->pgno;
		else
			np->prev_pgno = pagep->prev_pgno;
		if (new_next == NULL)
			ret = memp_fput(edbp->mpf, np, DB_MPOOL_DIRTY);
		else {
			*new_next = np;
			ret = memp_fset(edbp->mpf, np, DB_MPOOL_DIRTY);
		}
		if (ret != 0)
			goto err;
		if (needlock)
			(void)__bam_lput(edbc, npl);
	} else if (new_next != NULL)
		*new_next = NULL;

	if (pp != NULL) {
		pp->next_pgno = pagep->next_pgno;
		if ((ret = memp_fput(edbp->mpf, pp, DB_MPOOL_DIRTY)) != 0)
			goto err;
		if (needlock)
			(void)__bam_lput(edbc, ppl);
	}
	return (0);

err:	if (np != NULL)
		(void)memp_fput(edbp->mpf, np, 0);
	if (needlock && npl != LOCK_INVALID)
		(void)__bam_lput(edbc, npl);
	if (pp != NULL)
		(void)memp_fput(edbp->mpf, pp, 0);
	if (needlock && ppl != LOCK_INVALID)
		(void)__bam_lput(edbc, ppl);
	return (ret);
}

/*
 * __edb_ddup --
 *	Delete an offpage chain of duplicates.
 *
 * PUBLIC: int __edb_ddup __P((DBC *, edb_pgno_t, int (*)(DBC *, PAGE *)));
 */
int
__edb_ddup(edbc, pgno, freefunc)
	DBC *edbc;
	edb_pgno_t pgno;
	int (*freefunc) __P((DBC *, PAGE *));
{
	DB *edbp;
	PAGE *pagep;
	DBT tmp_edbt;
	int ret;

	edbp = edbc->edbp;
	do {
		if ((ret = memp_fget(edbp->mpf, &pgno, 0, &pagep)) != 0) {
			(void)__edb_pgerr(edbp, pgno);
			return (ret);
		}

		if (DB_LOGGING(edbc)) {
			tmp_edbt.data = pagep;
			tmp_edbt.size = edbp->pgsize;
			if ((ret = __edb_split_log(edbp->edbenv->lg_info,
			    edbc->txn, &LSN(pagep), 0, DB_SPLITOLD,
			    edbp->log_fileid, PGNO(pagep), &tmp_edbt,
			    &LSN(pagep))) != 0)
				return (ret);
		}
		pgno = pagep->next_pgno;
		if ((ret = freefunc(edbc, pagep)) != 0)
			return (ret);
	} while (pgno != PGNO_INVALID);

	return (0);
}

/*
 * __edb_addpage --
 *	Create a new page and link it onto the next_pgno field of the
 *	current page.
 */
static int
__edb_addpage(edbc, hp, indxp, newfunc)
	DBC *edbc;
	PAGE **hp;
	edb_indx_t *indxp;
	int (*newfunc) __P((DBC *, u_int32_t, PAGE **));
{
	DB *edbp;
	PAGE *newpage;
	int ret;

	edbp = edbc->edbp;
	if ((ret = newfunc(edbc, P_DUPLICATE, &newpage)) != 0)
		return (ret);

	if (DB_LOGGING(edbc)) {
		if ((ret = __edb_addpage_log(edbp->edbenv->lg_info,
		    edbc->txn, &LSN(*hp), 0, edbp->log_fileid,
		    PGNO(*hp), &LSN(*hp), PGNO(newpage), &LSN(newpage))) != 0) {
			return (ret);
		}
		LSN(newpage) = LSN(*hp);
	}

	PREV_PGNO(newpage) = PGNO(*hp);
	NEXT_PGNO(*hp) = PGNO(newpage);

	if ((ret = memp_fput(edbp->mpf, *hp, DB_MPOOL_DIRTY)) != 0)
		return (ret);
	*hp = newpage;
	*indxp = 0;
	return (0);
}

/*
 * __edb_dsearch --
 *	Search a set of duplicates for the proper position for a new duplicate.
 *
 *	+ pgno is the page number of the page on which to begin searching.
 * 	  Since we can continue duplicate searches, it might not be the first
 * 	  page.
 *
 * 	+ If we are continuing a search, then *pp may be non-NULL in which
 * 	  case we do not have to retrieve the page.
 *
 *	+ If we are continuing a search, then *indxp contains the first
 * 	  on pgno of where we should begin the search.
 *
 * 	NOTE: if there is no comparison function, then continuing is
 * 	meaningless, and *pp should always be NULL and *indxp will be
 *	ignored.
 *
 *	3 return values::
 *
 *	+ pp is the returned page pointer of where this element should go.
 *	+ indxp is the returned index on that page
 *	+ cmpp is the returned final comparison result.
 *
 * PUBLIC: int __edb_dsearch __P((DBC *,
 * PUBLIC:     int, DBT *, edb_pgno_t, edb_indx_t *, PAGE **, int *));
 */
int
__edb_dsearch(edbc, is_insert, edbt, pgno, indxp, pp, cmpp)
	DBC *edbc;
	int is_insert, *cmpp;
	DBT *edbt;
	edb_pgno_t pgno;
	edb_indx_t *indxp;
	PAGE **pp;
{
	DB *edbp;
	PAGE *h;
	edb_indx_t base, indx, lim, save_indx;
	edb_pgno_t save_pgno;
	int ret;

	edbp = edbc->edbp;

	if (edbp->dup_compare == NULL) {
		/*
		 * We may have been given a valid page, but we may not be
		 * able to use it.  The problem is that the application is
		 * doing a join and we're trying to continue the search,
		 * but since the items aren't sorted, we can't.  Discard
		 * the page if it's not the one we're going to start with
		 * anyway.
		 */
		if (*pp != NULL && (*pp)->pgno != pgno) {
			if ((ret = memp_fput(edbp->mpf, *pp, 0)) != 0)
				return (ret);
			*pp = NULL;
		}

		/*
		 * If no duplicate function is specified, just go to the end
		 * of the duplicate set.
		 */
		if (is_insert) {
			if ((ret = __edb_dend(edbc, pgno, pp)) != 0)
				return (ret);
			*indxp = NUM_ENT(*pp);
			return (0);
		}

		/*
		 * We are looking for a specific duplicate, so do a linear
		 * search.
		 */
		if (*pp != NULL)
			goto nocmp_started;
		for (;;) {
			if ((ret = memp_fget(edbp->mpf, &pgno, 0, pp)) != 0)
				goto pg_err;
nocmp_started:		h = *pp;

			for (*indxp = 0; *indxp < NUM_ENT(h); ++*indxp) {
				if ((*cmpp = __bam_cmp(edbp,
				    edbt, h, *indxp, __bam_defcmp)) != 0)
					continue;
				/*
				 * The duplicate may have already been deleted,
				 * if it's a btree page, in which case we skip
				 * it.
				 */
				if (edbp->type == DB_BTREE &&
				    B_DISSET(GET_BKEYDATA(h, *indxp)->type))
					continue;

				return (0);
			}

			if ((pgno = h->next_pgno) == PGNO_INVALID)
				break;

			if ((ret = memp_fput(edbp->mpf, h, 0)) != 0)
				return (ret);
		}
		*cmpp = 1;			/* We didn't succeed... */
		return (0);
	}

	/*
	 * We have a comparison routine, i.e., the duplicates are sorted.
	 * Walk through the chain of duplicates, checking the last entry
	 * on each page to decide if it's the page we want to search.
	 *
	 * *pp may be non-NULL -- if we were given a valid page (e.g., are
	 * in mid-search), then use the provided page.
	 */
	if (*pp != NULL)
		goto cmp_started;
	for (;;) {
		if ((ret = memp_fget(edbp->mpf, &pgno, 0, pp)) != 0)
			goto pg_err;
cmp_started:	h = *pp;

		if ((pgno = h->next_pgno) == PGNO_INVALID || __bam_cmp(edbp,
		    edbt, h, h->entries - 1, edbp->dup_compare) <= 0)
			break;
		/*
		 * Even when continuing a search, make sure we don't skip
		 * entries on a new page
		 */
		*indxp = 0;

		if ((ret = memp_fput(edbp->mpf, h, 0)) != 0)
			return (ret);
	}

	/* Next, do a binary search on the page. */
	base = F_ISSET(edbc, DBC_CONTINUE) ? *indxp : 0;
	for (lim = NUM_ENT(h) - base; lim != 0; lim >>= 1) {
		indx = base + (lim >> 1);
		if ((*cmpp = __bam_cmp(edbp,
		    edbt, h, indx, edbp->dup_compare)) == 0) {
			*indxp = indx;

			if (edbp->type != DB_BTREE ||
			    !B_DISSET(GET_BKEYDATA(h, *indxp)->type))
				return (0);
			goto check_delete;
		}
		if (*cmpp > 0) {
			base = indx + 1;
			lim--;
		}
	}

	/*
	 * Base references the smallest index larger than the supplied DBT's
	 * data item, potentially both 0 and NUM_ENT.
	 */
	*indxp = base;
	return (0);

check_delete:
	/*
	 * The duplicate may have already been deleted, if it's a btree page,
	 * in which case we wander around, hoping to find an entry that hasn't
	 * been deleted.  First, wander in a forwardly direction.
	 */
	save_pgno = (*pp)->pgno;
	save_indx = *indxp;
	for (++*indxp;;) {
		for (; *indxp < NUM_ENT(h); ++*indxp) {
			if ((*cmpp = __bam_cmp(edbp,
			    edbt, h, *indxp, edbp->dup_compare)) != 0)
				goto check_delete_rev;

			if (!B_DISSET(GET_BKEYDATA(h, *indxp)->type))
				return (0);
		}
		if ((pgno = h->next_pgno) == PGNO_INVALID)
			break;

		if ((ret = memp_fput(edbp->mpf, h, 0)) != 0)
			return (ret);

		if ((ret = memp_fget(edbp->mpf, &pgno, 0, pp)) != 0)
			goto pg_err;
		h = *pp;

		*indxp = 0;
	}

check_delete_rev:
	/* Go back to where we started, and wander in a backwardly direction. */
	if (h->pgno != save_pgno) {
		if ((ret = memp_fput(edbp->mpf, h, 0)) != 0)
			return (ret);
		if ((ret = memp_fget(edbp->mpf, &save_pgno, 0, pp)) != 0)
			goto pg_err;
		h = *pp;
	}

	for (;;) {
		while (*indxp > 0) {
			--*indxp;
			if ((*cmpp = __bam_cmp(edbp,
			    edbt, h, *indxp, edbp->dup_compare)) != 0)
				goto check_delete_fail;

			if (!B_DISSET(GET_BKEYDATA(h, *indxp)->type))
				return (0);
		}
		if ((pgno = h->prev_pgno) == PGNO_INVALID)
			break;

		if ((ret = memp_fput(edbp->mpf, h, 0)) != 0)
			return (ret);

		if ((ret = memp_fget(edbp->mpf, &pgno, 0, pp)) != 0)
			goto pg_err;
		h = *pp;

		*indxp = NUM_ENT(h);
	}

check_delete_fail:
	*cmpp = 1;			/* We didn't succeed... */
	return (0);

pg_err:	__edb_pgerr(edbp, pgno);
	return (ret);
}
