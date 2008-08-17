/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
/*
 * Copyright (c) 1990, 1993, 1994
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
static const char sccsid[] = "@(#)hash_dup.c	10.27 (Sleepycat) 12/6/98";
#endif /* not lint */

/*
 * PACKAGE:  hashing
 *
 * DESCRIPTION:
 *      Manipulation of duplicates for the hash package.
 *
 * ROUTINES:
 *
 * External
 *      __add_dup
 * Internal
 */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "edb_page.h"
#include "hash.h"
#include "btree.h"

static int __ham_check_move __P((DBC *, int32_t));
static int __ham_dup_convert __P((DBC *));
static int __ham_make_dup __P((const DBT *, DBT *d, void **, u_int32_t *));

/*
 * Called from hash_access to add a duplicate key. nval is the new
 * value that we want to add.  The flags correspond to the flag values
 * to cursor_put indicating where to add the new element.
 * There are 4 cases.
 * Case 1: The existing duplicate set already resides on a separate page.
 *	   We can use common code for this.
 * Case 2: The element is small enough to just be added to the existing set.
 * Case 3: The element is large enough to be a big item, so we're going to
 *	   have to push the set onto a new page.
 * Case 4: The element is large enough to push the duplicate set onto a
 *	   separate page.
 *
 * PUBLIC: int __ham_add_dup __P((DBC *, DBT *, u_int32_t));
 */
int
__ham_add_dup(edbc, nval, flags)
	DBC *edbc;
	DBT *nval;
	u_int32_t flags;
{
	DB *edbp;
	HASH_CURSOR *hcp;
	DBT edbt, pval, tmp_val;
	u_int32_t del_len, new_size;
	int cmp, ret;
	u_int8_t *hk;

	edbp = edbc->edbp;
	hcp = (HASH_CURSOR *)edbc->internal;
	if (flags == DB_CURRENT && hcp->dpgno == PGNO_INVALID)
		del_len = hcp->dup_len;
	else
		del_len = 0;

	if ((ret = __ham_check_move(edbc,
	    (int32_t)DUP_SIZE(nval->size) - (int32_t)del_len)) != 0)
		return (ret);

	/*
	 * Check if resulting duplicate set is going to need to go
	 * onto a separate duplicate page.  If so, convert the
	 * duplicate set and add the new one.  After conversion,
	 * hcp->dndx is the first free ndx or the index of the
	 * current pointer into the duplicate set.
	 */
	hk = H_PAIRDATA(hcp->pagep, hcp->bndx);
	new_size = DUP_SIZE(nval->size) - del_len + LEN_HKEYDATA(hcp->pagep,
	    hcp->hdr->pagesize, H_DATAINDEX(hcp->bndx));

	/*
	 * We convert to off-page duplicates if the item is a big item,
	 * the addition of the new item will make the set large, or
	 * if there isn't enough room on this page to add the next item.
	 */
	if (HPAGE_PTYPE(hk) != H_OFFDUP &&
	    (HPAGE_PTYPE(hk) == H_OFFPAGE || ISBIG(hcp, new_size) ||
	    DUP_SIZE(nval->size) - del_len > P_FREESPACE(hcp->pagep))) {

		if ((ret = __ham_dup_convert(edbc)) != 0)
			return (ret);
		else
			hk = H_PAIRDATA(hcp->pagep, hcp->bndx);
	}

	/* There are two separate cases here: on page and off page. */
	if (HPAGE_PTYPE(hk) != H_OFFDUP) {
		if (HPAGE_PTYPE(hk) != H_DUPLICATE) {
			HPAGE_PTYPE(hk) = H_DUPLICATE;
			pval.flags = 0;
			pval.data = HKEYDATA_DATA(hk);
			pval.size = LEN_HDATA(hcp->pagep, edbp->pgsize,
			    hcp->bndx);
			if ((ret =
			    __ham_make_dup(&pval, &tmp_val, &edbc->rdata.data,
			    &edbc->rdata.size)) != 0 || (ret =
			    __ham_replpair(edbc, &tmp_val, 1)) != 0)
				return (ret);
		}

		/* Now make the new entry a duplicate. */
		if ((ret = __ham_make_dup(nval,
		    &tmp_val, &edbc->rdata.data, &edbc->rdata.size)) != 0)
			return (ret);

		tmp_val.dlen = 0;
		switch (flags) {			/* On page. */
		case DB_KEYFIRST:
		case DB_KEYLAST:
			if (edbp->dup_compare != NULL)
				__ham_dsearch(edbc, nval, &tmp_val.doff, &cmp);
			else if (flags == DB_KEYFIRST)
				tmp_val.doff = 0;
			else
				tmp_val.doff = LEN_HDATA(hcp->pagep,
				    hcp->hdr->pagesize, hcp->bndx);
			break;
		case DB_CURRENT:
			/*
			 * If we have a sort function, we need to verify that
			 * the new item sorts identically to the old item.
			 */
			if (edbp->dup_compare != NULL) {
				edbt.data = HKEYDATA_DATA(H_PAIRDATA(hcp->pagep,
				    hcp->bndx)) + hcp->dup_off;
				edbt.size = DUP_SIZE(hcp->dup_len);
				if (edbp->dup_compare(nval, &edbt) != 0)
					return (EINVAL);
			}
			tmp_val.doff = hcp->dup_off;
			tmp_val.dlen = DUP_SIZE(hcp->dup_len);
			break;
		case DB_BEFORE:
			tmp_val.doff = hcp->dup_off;
			break;
		case DB_AFTER:
			tmp_val.doff = hcp->dup_off + DUP_SIZE(hcp->dup_len);
			break;
		}
		/* Add the duplicate. */
		ret = __ham_replpair(edbc, &tmp_val, 0);
		if (ret == 0)
			ret = __ham_dirty_page(edbp, hcp->pagep);
		__ham_c_update(hcp, hcp->pgno, tmp_val.size, 1, 1);
		return (ret);
	}

	/* If we get here, then we're on duplicate pages. */
	if (hcp->dpgno == PGNO_INVALID) {
		memcpy(&hcp->dpgno, HOFFDUP_PGNO(hk), sizeof(edb_pgno_t));
		hcp->dndx = 0;
	}

	switch (flags) {
	case DB_KEYFIRST:
		if (edbp->dup_compare != NULL)
			goto sorted_dups;
		/*
		 * The only way that we are already on a dup page is
		 * if we just converted the on-page representation.
		 * In that case, we've only got one page of duplicates.
		 */
		if (hcp->dpagep == NULL && (ret =
		    __edb_dend(edbc, hcp->dpgno, &hcp->dpagep)) != 0)
			return (ret);
		hcp->dndx = 0;
		break;
	case DB_KEYLAST:
		if (edbp->dup_compare != NULL) {
sorted_dups:		if ((ret = __edb_dsearch(edbc, 1, nval,
			    hcp->dpgno, &hcp->dndx, &hcp->dpagep, &cmp)) != 0)
				return (ret);
			if (cmp == 0)
				hcp->dpgno = PGNO(hcp->dpagep);
		} else {
			if (hcp->dpagep == NULL && (ret =
			    __edb_dend(edbc, hcp->dpgno, &hcp->dpagep)) != 0)
				return (ret);
			hcp->dpgno = PGNO(hcp->dpagep);
			hcp->dndx = NUM_ENT(hcp->dpagep);
		}
		break;
	case DB_CURRENT:
		if (edbp->dup_compare != NULL && __bam_cmp(edbp,
		    nval, hcp->dpagep, hcp->dndx, edbp->dup_compare) != 0)
			return (EINVAL);
		switch (GET_BKEYDATA(hcp->dpagep, hcp->dndx)->type) {
		case B_KEYDATA:
			del_len = BKEYDATA_SIZE(GET_BKEYDATA(hcp->dpagep,
			    hcp->dndx)->len);
			break;
		case B_OVERFLOW:
			del_len = BOVERFLOW_SIZE;
			break;
		}
		if ((ret =
		    __edb_ditem(edbc, hcp->dpagep, hcp->dndx, del_len)) != 0)
			return (ret);
		break;
	case DB_BEFORE:	/* The default behavior is correct. */
		break;
	case DB_AFTER:
		hcp->dndx++;
		break;
	}

	ret = __edb_dput(edbc,
	    nval, &hcp->dpagep, &hcp->dndx, __ham_overflow_page);
	hcp->pgno = PGNO(hcp->pagep);
	__ham_c_update(hcp, hcp->pgno, nval->size, 1, 1);
	return (ret);
}

/*
 * Convert an on-page set of duplicates to an offpage set of duplicates.
 */
static int
__ham_dup_convert(edbc)
	DBC *edbc;
{
	DB *edbp;
	HASH_CURSOR *hcp;
	BOVERFLOW bo;
	DBT edbt;
	HOFFPAGE ho;
	edb_indx_t dndx, i, len, off;
	int ret;
	u_int8_t *p, *pend;

	/*
	 * Create a new page for the duplicates.
	 */
	edbp = edbc->edbp;
	hcp = (HASH_CURSOR *)edbc->internal;
	if ((ret =
	    __ham_overflow_page(edbc, P_DUPLICATE, &hcp->dpagep)) != 0)
		return (ret);
	hcp->dpagep->type = P_DUPLICATE;
	hcp->dpgno = PGNO(hcp->dpagep);

	/*
	 * Now put the duplicates onto the new page.
	 */
	dndx = 0;
	edbt.flags = 0;
	switch (HPAGE_PTYPE(H_PAIRDATA(hcp->pagep, hcp->bndx))) {
	case H_KEYDATA:
		/* Simple case, one key on page; move it to dup page. */
		edbt.size =
		    LEN_HDATA(hcp->pagep, hcp->hdr->pagesize, hcp->bndx);
		edbt.data = HKEYDATA_DATA(H_PAIRDATA(hcp->pagep, hcp->bndx));
		ret = __edb_pitem(edbc, hcp->dpagep,
		    (u_int32_t)dndx, BKEYDATA_SIZE(edbt.size), NULL, &edbt);
		if (ret == 0)
			__ham_dirty_page(edbp, hcp->dpagep);
		break;
	case H_OFFPAGE:
		/* Simple case, one key on page; move it to dup page. */
		memcpy(&ho,
		    P_ENTRY(hcp->pagep, H_DATAINDEX(hcp->bndx)), HOFFPAGE_SIZE);
		UMRW(bo.unused1);
		B_TSET(bo.type, ho.type, 0);
		UMRW(bo.unused2);
		bo.pgno = ho.pgno;
		bo.tlen = ho.tlen;
		edbt.size = BOVERFLOW_SIZE;
		edbt.data = &bo;

		ret = __edb_pitem(edbc, hcp->dpagep,
		   (u_int32_t)dndx, edbt.size, &edbt, NULL);
		if (ret == 0)
			__ham_dirty_page(edbp, hcp->dpagep);
		break;
	case H_DUPLICATE:
		p = HKEYDATA_DATA(H_PAIRDATA(hcp->pagep, hcp->bndx));
		pend = p +
		    LEN_HDATA(hcp->pagep, hcp->hdr->pagesize, hcp->bndx);

		/*
		 * We need to maintain the duplicate cursor position.
		 * Keep track of where we are in the duplicate set via
		 * the offset, and when it matches the one in the cursor,
		 * set the off-page duplicate cursor index to the current
		 * index.
		 */
		for (off = 0, i = 0; p < pend; i++) {
			if (off == hcp->dup_off)
				dndx = i;
			memcpy(&len, p, sizeof(edb_indx_t));
			edbt.size = len;
			p += sizeof(edb_indx_t);
			edbt.data = p;
			p += len + sizeof(edb_indx_t);
			off += len + 2 * sizeof(edb_indx_t);
			ret = __edb_dput(edbc, &edbt,
			    &hcp->dpagep, &i, __ham_overflow_page);
			if (ret != 0)
				break;
		}
		break;
	default:
		ret = __edb_pgfmt(edbp, (u_long)hcp->pgno);
		break;
	}
	if (ret == 0) {
		/*
		 * Now attach this to the source page in place of
		 * the old duplicate item.
		 */
		__ham_move_offpage(edbc, hcp->pagep,
		    (u_int32_t)H_DATAINDEX(hcp->bndx), hcp->dpgno);

		/* Can probably just do a "put" here. */
		ret = __ham_dirty_page(edbp, hcp->pagep);
		hcp->dndx = dndx;
	} else {
		(void)__ham_del_page(edbc, hcp->dpagep);
		hcp->dpagep = NULL;
	}
	return (ret);
}

static int
__ham_make_dup(notdup, duplicate, bufp, sizep)
	const DBT *notdup;
	DBT *duplicate;
	void **bufp;
	u_int32_t *sizep;
{
	edb_indx_t tsize, item_size;
	int ret;
	u_int8_t *p;

	item_size = (edb_indx_t)notdup->size;
	tsize = DUP_SIZE(item_size);
	if ((ret = __ham_init_edbt(duplicate, tsize, bufp, sizep)) != 0)
		return (ret);

	duplicate->dlen = 0;
	duplicate->flags = notdup->flags;
	F_SET(duplicate, DB_DBT_PARTIAL);

	p = duplicate->data;
	memcpy(p, &item_size, sizeof(edb_indx_t));
	p += sizeof(edb_indx_t);
	memcpy(p, notdup->data, notdup->size);
	p += notdup->size;
	memcpy(p, &item_size, sizeof(edb_indx_t));

	duplicate->doff = 0;
	duplicate->dlen = notdup->size;

	return (0);
}

static int
__ham_check_move(edbc, add_len)
	DBC *edbc;
	int32_t add_len;
{
	DB *edbp;
	HASH_CURSOR *hcp;
	DBT k, d;
	DB_LSN new_lsn;
	PAGE *next_pagep;
	edb_pgno_t next_pgno;
	u_int32_t new_datalen, old_len, rectype;
	u_int8_t *hk;
	int ret;

	edbp = edbc->edbp;
	hcp = (HASH_CURSOR *)edbc->internal;
	/*
	 * Check if we can do whatever we need to on this page.  If not,
	 * then we'll have to move the current element to a new page.
	 */
	hk = H_PAIRDATA(hcp->pagep, hcp->bndx);

	/*
	 * If the item is already off page duplicates or an offpage item,
	 * then we know we can do whatever we need to do in-place
	 */
	if (HPAGE_PTYPE(hk) == H_OFFDUP || HPAGE_PTYPE(hk) == H_OFFPAGE)
		return (0);

	old_len =
	    LEN_HITEM(hcp->pagep, hcp->hdr->pagesize, H_DATAINDEX(hcp->bndx));
	new_datalen = old_len - HKEYDATA_SIZE(0) + add_len;

	/*
	 * We need to add a new page under two conditions:
	 * 1. The addition makes the total data length cross the BIG
	 *    threshold and the OFFDUP structure won't fit on this page.
	 * 2. The addition does not make the total data cross the
	 *    threshold, but the new data won't fit on the page.
	 * If neither of these is true, then we can return.
	 */
	if (ISBIG(hcp, new_datalen) && (old_len > HOFFDUP_SIZE ||
	    HOFFDUP_SIZE - old_len <= P_FREESPACE(hcp->pagep)))
		return (0);

	if (!ISBIG(hcp, new_datalen) &&
	    add_len <= (int32_t)P_FREESPACE(hcp->pagep))
		return (0);

	/*
	 * If we get here, then we need to move the item to a new page.
	 * Check if there are more pages in the chain.
	 */

	new_datalen = ISBIG(hcp, new_datalen) ?
	    HOFFDUP_SIZE : HKEYDATA_SIZE(new_datalen);

	next_pagep = NULL;
	for (next_pgno = NEXT_PGNO(hcp->pagep); next_pgno != PGNO_INVALID;
	    next_pgno = NEXT_PGNO(next_pagep)) {
		if (next_pagep != NULL &&
		    (ret = __ham_put_page(edbp, next_pagep, 0)) != 0)
			return (ret);

		if ((ret =
		    __ham_get_page(edbp, next_pgno, &next_pagep)) != 0)
			return (ret);

		if (P_FREESPACE(next_pagep) >= new_datalen)
			break;
	}

	/* No more pages, add one. */
	if (next_pagep == NULL && (ret = __ham_add_ovflpage(edbc,
	    hcp->pagep, 0, &next_pagep)) != 0)
		return (ret);

	/* Add new page at the end of the chain. */
	if (P_FREESPACE(next_pagep) < new_datalen && (ret =
	    __ham_add_ovflpage(edbc, next_pagep, 1, &next_pagep)) != 0)
		return (ret);

	/* Copy the item to the new page. */
	if (DB_LOGGING(hcp->edbc)) {
		rectype = PUTPAIR;
		k.flags = 0;
		d.flags = 0;
		if (HPAGE_PTYPE(
		    H_PAIRKEY(hcp->pagep, hcp->bndx)) == H_OFFPAGE) {
			rectype |= PAIR_KEYMASK;
			k.data = H_PAIRKEY(hcp->pagep, hcp->bndx);
			k.size = HOFFPAGE_SIZE;
		} else {
			k.data =
			    HKEYDATA_DATA(H_PAIRKEY(hcp->pagep, hcp->bndx));
			k.size = LEN_HKEY(hcp->pagep,
			    hcp->hdr->pagesize, hcp->bndx);
		}

		if (HPAGE_PTYPE(hk) == H_OFFPAGE) {
			rectype |= PAIR_DATAMASK;
			d.data = H_PAIRDATA(hcp->pagep, hcp->bndx);
			d.size = HOFFPAGE_SIZE;
		} else {
			d.data =
			    HKEYDATA_DATA(H_PAIRDATA(hcp->pagep, hcp->bndx));
			d.size = LEN_HDATA(hcp->pagep,
			    hcp->hdr->pagesize, hcp->bndx);
		}


		if ((ret = __ham_insdel_log(edbp->edbenv->lg_info,
		    edbc->txn, &new_lsn, 0, rectype,
		    edbp->log_fileid, PGNO(next_pagep),
		    (u_int32_t)H_NUMPAIRS(next_pagep), &LSN(next_pagep),
		    &k, &d)) != 0)
			return (ret);

		/* Move lsn onto page. */
		LSN(next_pagep) = new_lsn;	/* Structure assignment. */
	}

	__ham_copy_item(edbp->pgsize,
	    hcp->pagep, H_KEYINDEX(hcp->bndx), next_pagep);
	__ham_copy_item(edbp->pgsize,
	    hcp->pagep, H_DATAINDEX(hcp->bndx), next_pagep);

	/* Now delete the pair from the current page. */
	ret = __ham_del_pair(edbc, 0);

	(void)__ham_put_page(edbp, hcp->pagep, 1);
	hcp->pagep = next_pagep;
	hcp->pgno = PGNO(hcp->pagep);
	hcp->bndx = H_NUMPAIRS(hcp->pagep) - 1;
	F_SET(hcp, H_EXPAND);
	return (ret);
}

/*
 * __ham_move_offpage --
 *	Replace an onpage set of duplicates with the OFFDUP structure
 *	that references the duplicate page.
 *
 * XXX
 * This is really just a special case of __onpage_replace; we should
 * probably combine them.
 *
 * PUBLIC: void __ham_move_offpage __P((DBC *, PAGE *, u_int32_t, edb_pgno_t));
 */
void
__ham_move_offpage(edbc, pagep, ndx, pgno)
	DBC *edbc;
	PAGE *pagep;
	u_int32_t ndx;
	edb_pgno_t pgno;
{
	DB *edbp;
	HASH_CURSOR *hcp;
	DBT new_edbt;
	DBT old_edbt;
	HOFFDUP od;
	edb_indx_t i;
	int32_t shrink;
	u_int8_t *src;

	edbp = edbc->edbp;
	hcp = (HASH_CURSOR *)edbc->internal;
	od.type = H_OFFDUP;
	UMRW(od.unused[0]);
	UMRW(od.unused[1]);
	UMRW(od.unused[2]);
	od.pgno = pgno;

	if (DB_LOGGING(edbc)) {
		new_edbt.data = &od;
		new_edbt.size = HOFFDUP_SIZE;
		old_edbt.data = P_ENTRY(pagep, ndx);
		old_edbt.size = LEN_HITEM(pagep, hcp->hdr->pagesize, ndx);
		(void)__ham_replace_log(edbp->edbenv->lg_info,
		    edbc->txn, &LSN(pagep), 0, edbp->log_fileid,
		    PGNO(pagep), (u_int32_t)ndx, &LSN(pagep), -1,
		    &old_edbt, &new_edbt, 0);
	}

	shrink =
	    LEN_HITEM(pagep, hcp->hdr->pagesize, ndx) - HOFFDUP_SIZE;

	if (shrink != 0) {
		/* Copy data. */
		src = (u_int8_t *)(pagep) + HOFFSET(pagep);
		memmove(src + shrink, src, pagep->inp[ndx] - HOFFSET(pagep));
		HOFFSET(pagep) += shrink;

		/* Update index table. */
		for (i = ndx; i < NUM_ENT(pagep); i++)
			pagep->inp[i] += shrink;
	}

	/* Now copy the offdup entry onto the page. */
	memcpy(P_ENTRY(pagep, ndx), &od, HOFFDUP_SIZE);
}

/*
 * __ham_dsearch:
 *	Locate a particular duplicate in a duplicate set.
 *
 * PUBLIC: void __ham_dsearch __P((DBC *, DBT *, u_int32_t *, int *));
 */
void
__ham_dsearch(edbc, edbt, offp, cmpp)
	DBC *edbc;
	DBT *edbt;
	u_int32_t *offp;
	int *cmpp;
{
	DB *edbp;
	HASH_CURSOR *hcp;
	DBT cur;
	edb_indx_t i, len;
	int (*func) __P((const DBT *, const DBT *));
	u_int8_t *data;

	edbp = edbc->edbp;
	hcp = (HASH_CURSOR *)edbc->internal;
	if (edbp->dup_compare == NULL)
		func = __bam_defcmp;
	else
		func = edbp->dup_compare;

	i = F_ISSET(edbc, DBC_CONTINUE) ? hcp->dup_off: 0;
	data = HKEYDATA_DATA(H_PAIRDATA(hcp->pagep, hcp->bndx)) + i;
	while (i < LEN_HDATA(hcp->pagep, hcp->hdr->pagesize, hcp->bndx)) {
		memcpy(&len, data, sizeof(edb_indx_t));
		data += sizeof(edb_indx_t);
		cur.data = data;
		cur.size = (u_int32_t)len;
		*cmpp = func(edbt, &cur);
		if (*cmpp == 0 || (*cmpp < 0 && edbp->dup_compare != NULL))
			break;
		i += len + 2 * sizeof(edb_indx_t);
		data += len + sizeof(edb_indx_t);
	}
	*offp = i;
}
