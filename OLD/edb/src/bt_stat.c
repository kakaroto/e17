/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)bt_stat.c	10.27 (Sleepycat) 11/25/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <string.h>
#endif

#include "edb_int.h"
#include "edb_page.h"
#include "btree.h"

/*
 * __bam_stat --
 *	Gather/print the btree statistics
 *
 * PUBLIC: int __bam_stat __P((DB *, void *, void *(*)(size_t), u_int32_t));
 */
int
__bam_stat(edbp, spp, edb_malloc, flags)
	DB *edbp;
	void *spp;
	void *(*edb_malloc) __P((size_t));
	u_int32_t flags;
{
	BTMETA *meta;
	BTREE *t;
	DBC *edbc;
	DB_BTREE_STAT *sp;
	DB_LOCK lock;
	PAGE *h;
	edb_pgno_t lastpgno, pgno;
	int ret, t_ret;

	DB_PANIC_CHECK(edbp);

	/* Check for invalid flags. */
	if ((ret = __edb_statchk(edbp, flags)) != 0)
		return (ret);

	if ((ret = edbp->cursor(edbp, NULL, &edbc, 0)) != 0)
		return (ret);

	DEBUG_LWRITE(edbc, NULL, "bam_stat", NULL, NULL, flags);

	t = edbp->internal;

	if (spp == NULL)
		return (0);

	/* Allocate and clear the structure. */
	if ((ret = __edb_os_malloc(sizeof(*sp), edb_malloc, &sp)) != 0)
		goto err;
	memset(sp, 0, sizeof(*sp));

	/* If the app just wants the record count, make it fast. */
	if (flags == DB_RECORDCOUNT) {
		pgno = PGNO_ROOT;
		if ((ret = __bam_lget(edbc, 0, pgno, DB_LOCK_READ, &lock)) != 0)
			goto err;
		if ((ret = memp_fget(edbp->mpf, &pgno, 0, (PAGE **)&h)) != 0)
			goto err;

		sp->bt_nrecs = RE_NREC(h);

		(void)memp_fput(edbp->mpf, h, 0);
		(void)__BT_LPUT(edbc, lock);
		goto done;
	}

	/* Get the meta-data page. */
	pgno = PGNO_METADATA;
	if ((ret = __bam_lget(edbc, 0, pgno, DB_LOCK_READ, &lock)) != 0)
		goto err;
	if ((ret = memp_fget(edbp->mpf, &pgno, 0, (PAGE **)&meta)) != 0)
		goto err;

	/* Translate the metadata flags. */
	if (F_ISSET(meta, BTM_DUP))
		sp->bt_flags |= DB_DUP;
	if (F_ISSET(meta, BTM_FIXEDLEN))
		sp->bt_flags |= DB_FIXEDLEN;
	if (F_ISSET(meta, BTM_RECNUM))
		sp->bt_flags |= DB_RECNUM;
	if (F_ISSET(meta, BTM_RENUMBER))
		sp->bt_flags |= DB_RENUMBER;

	/* Get the remaining metadata fields. */
	sp->bt_minkey = meta->minkey;
	sp->bt_maxkey = meta->maxkey;
	sp->bt_re_len = meta->re_len;
	sp->bt_re_pad = meta->re_pad;
	sp->bt_magic = meta->magic;
	sp->bt_version = meta->version;

	/* Get the page size from the DB. */
	sp->bt_pagesize = edbp->pgsize;

	/* Walk the free list, counting pages. */
	for (sp->bt_free = 0, pgno = meta->free; pgno != PGNO_INVALID;) {
		++sp->bt_free;

		if ((ret = memp_fget(edbp->mpf, &pgno, 0, &h)) != 0) {
			(void)memp_fput(edbp->mpf, meta, 0);
			(void)__BT_TLPUT(edbc, lock);
			goto err;
		}
		pgno = h->next_pgno;
		(void)memp_fput(edbp->mpf, h, 0);
	}

	/* Discard the meta-data page. */
	(void)memp_fput(edbp->mpf, meta, 0);
	(void)__BT_TLPUT(edbc, lock);

	/* Determine the last page of the database. */
	if ((ret = memp_fget(edbp->mpf, &lastpgno, DB_MPOOL_LAST, &h)) != 0)
		goto err;
	(void)memp_fput(edbp->mpf, h, 0);

	/* Get the root page. */
	pgno = PGNO_ROOT;
	if ((ret = __bam_lget(edbc, 0, PGNO_ROOT, DB_LOCK_READ, &lock)) != 0)
		goto err;
	if ((ret = memp_fget(edbp->mpf, &pgno, 0, &h)) != 0) {
		(void)__BT_LPUT(edbc, lock);
		goto err;
	}

	/* Get the levels from the root page. */
	sp->bt_levels = h->level;

	/* Walk the page list, counting things. */
	for (;;) {
		switch (TYPE(h)) {
		case P_INVALID:
			break;
		case P_IBTREE:
		case P_IRECNO:
			++sp->bt_int_pg;
			sp->bt_int_pgfree += HOFFSET(h) - LOFFSET(h);
			break;
		case P_LBTREE:
			++sp->bt_leaf_pg;
			sp->bt_leaf_pgfree += HOFFSET(h) - LOFFSET(h);
			sp->bt_nrecs += NUM_ENT(h) / P_INDX;
			break;
		case P_LRECNO:
			++sp->bt_leaf_pg;
			sp->bt_leaf_pgfree += HOFFSET(h) - LOFFSET(h);
			sp->bt_nrecs += NUM_ENT(h);
			break;
		case P_DUPLICATE:
			++sp->bt_dup_pg;
			/* XXX MARGO: sp->bt_dup_pgfree; */
			break;
		case P_OVERFLOW:
			++sp->bt_over_pg;
			/* XXX MARGO: sp->bt_over_pgfree; */
			break;
		default:
			(void)memp_fput(edbp->mpf, h, 0);
			(void)__BT_LPUT(edbc, lock);
			return (__edb_pgfmt(edbp, pgno));
		}

		(void)memp_fput(edbp->mpf, h, 0);
		(void)__BT_LPUT(edbc, lock);

		if (++pgno > lastpgno)
			break;
		if (__bam_lget(edbc, 0, pgno, DB_LOCK_READ, &lock))
			break;
		if (memp_fget(edbp->mpf, &pgno, 0, &h) != 0) {
			(void)__BT_LPUT(edbc, lock);
			break;
		}
	}

done:	*(DB_BTREE_STAT **)spp = sp;
	ret = 0;

err:	if ((t_ret = edbc->c_close(edbc)) != 0 && ret == 0)
		ret = t_ret;
	return (ret);
}
