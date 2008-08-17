/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)edb_ret.c	10.16 (Sleepycat) 10/4/98";
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

/*
 * __edb_ret --
 *	Build return DBT.
 *
 * PUBLIC: int __edb_ret __P((DB *,
 * PUBLIC:    PAGE *, u_int32_t, DBT *, void **, u_int32_t *));
 */
int
__edb_ret(edbp, h, indx, edbt, memp, memsize)
	DB *edbp;
	PAGE *h;
	u_int32_t indx;
	DBT *edbt;
	void **memp;
	u_int32_t *memsize;
{
	BKEYDATA *bk;
	HOFFPAGE ho;
	BOVERFLOW *bo;
	u_int32_t len;
	u_int8_t *hk;
	void *data;

	switch (TYPE(h)) {
	case P_HASH:
		hk = P_ENTRY(h, indx);
		if (HPAGE_PTYPE(hk) == H_OFFPAGE) {
			memcpy(&ho, hk, sizeof(HOFFPAGE));
			return (__edb_goff(edbp, edbt,
			    ho.tlen, ho.pgno, memp, memsize));
		}
		len = LEN_HKEYDATA(h, edbp->pgsize, indx);
		data = HKEYDATA_DATA(hk);
		break;
	case P_DUPLICATE:
	case P_LBTREE:
	case P_LRECNO:
		bk = GET_BKEYDATA(h, indx);
		if (B_TYPE(bk->type) == B_OVERFLOW) {
			bo = (BOVERFLOW *)bk;
			return (__edb_goff(edbp, edbt,
			    bo->tlen, bo->pgno, memp, memsize));
		}
		len = bk->len;
		data = bk->data;
		break;
	default:
		return (__edb_pgfmt(edbp, h->pgno));
	}

	return (__edb_retcopy(edbt, data, len, memp, memsize,
	    F_ISSET(edbt, DB_DBT_INTERNAL) ? NULL : edbp->edb_malloc));
}

/*
 * __edb_retcopy --
 *	Copy the returned data into the user's DBT, handling special flags.
 *
 * PUBLIC: int __edb_retcopy __P((DBT *,
 * PUBLIC:    void *, u_int32_t, void **, u_int32_t *, void *(*)(size_t)));
 */
int
__edb_retcopy(edbt, data, len, memp, memsize, edb_malloc)
	DBT *edbt;
	void *data;
	u_int32_t len;
	void **memp;
	u_int32_t *memsize;
	void *(*edb_malloc) __P((size_t));
{
	int ret;

	/* If returning a partial record, reset the length. */
	if (F_ISSET(edbt, DB_DBT_PARTIAL)) {
		data = (u_int8_t *)data + edbt->doff;
		if (len > edbt->doff) {
			len -= edbt->doff;
			if (len > edbt->dlen)
				len = edbt->dlen;
		} else
			len = 0;
	}

	/*
	 * Return the length of the returned record in the DBT size field.
	 * This satisfies the requirement that if we're using user memory
	 * and insufficient memory was provided, return the amount necessary
	 * in the size field.
	 */
	edbt->size = len;

	/*
	 * Allocate memory to be owned by the application: DB_DBT_MALLOC.
	 *
	 * !!!
	 * We always allocate memory, even if we're copying out 0 bytes. This
	 * guarantees consistency, i.e., the application can always free memory
	 * without concern as to how many bytes of the record were requested.
	 *
	 * Use the memory specified by the application: DB_DBT_USERMEM.
	 *
	 * !!!
	 * If the length we're going to copy is 0, the application-supplied
	 * memory pointer is allowed to be NULL.
	 */
	if (F_ISSET(edbt, DB_DBT_MALLOC)) {
		if ((ret = __edb_os_malloc(len, edb_malloc, &edbt->data)) != 0)
			return (ret);
	} else if (F_ISSET(edbt, DB_DBT_USERMEM)) {
		if (len != 0 && (edbt->data == NULL || edbt->ulen < len))
			return (ENOMEM);
	} else if (memp == NULL || memsize == NULL) {
		return (EINVAL);
	} else {
		if (len != 0 && (*memsize == 0 || *memsize < len)) {
			if ((ret = __edb_os_realloc(memp, len)) != 0) {
				*memsize = 0;
				return (ret);
			}
			*memsize = len;
		}
		edbt->data = *memp;
	}

	if (len != 0)
		memcpy(edbt->data, data, len);
	return (0);
}
