/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)bt_curadj.c	10.69 (Sleepycat) 12/2/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <stdlib.h>
#endif

#include "edb_int.h"
#include "edb_page.h"
#include "btree.h"

#ifdef DEBUG
/*
 * __bam_cprint --
 *	Display the current cursor list.
 *
 * PUBLIC: int __bam_cprint __P((DB *));
 */
int
__bam_cprint(edbp)
	DB *edbp;
{
	CURSOR *cp;
	DBC *edbc;

	DB_THREAD_LOCK(edbp);
	for (edbc = TAILQ_FIRST(&edbp->active_queue);
	    edbc != NULL; edbc = TAILQ_NEXT(edbc, links)) {
		cp = (CURSOR *)edbc->internal;
		fprintf(stderr,
	    "%#0x->%#0x: page: %lu index: %lu dpage %lu dindex: %lu recno: %lu",
		    (u_int)edbc, (u_int)cp, (u_long)cp->pgno, (u_long)cp->indx,
		    (u_long)cp->dpgno, (u_long)cp->dindx, (u_long)cp->recno);
		if (F_ISSET(cp, C_DELETED))
			fprintf(stderr, " (deleted)");
		fprintf(stderr, "\n");
	}
	DB_THREAD_UNLOCK(edbp);

	return (0);
}
#endif /* DEBUG */

/*
 * __bam_ca_delete --
 *	Update the cursors when items are deleted and when already deleted
 *	items are overwritten.  Return the number of relevant cursors found.
 *
 * PUBLIC: int __bam_ca_delete __P((DB *, edb_pgno_t, u_int32_t, int));
 */
int
__bam_ca_delete(edbp, pgno, indx, delete)
	DB *edbp;
	edb_pgno_t pgno;
	u_int32_t indx;
	int delete;
{
	DBC *edbc;
	CURSOR *cp;
	int count;		/* !!!: Has to contain max number of cursors. */

	/* Recno is responsible for its own adjustments. */
	if (edbp->type == DB_RECNO)
		return (0);

	/*
	 * Adjust the cursors.  We don't have to review the cursors for any
	 * thread of control other than the current one, because we have the
	 * page write locked at this point, and any other thread of control
	 * had better be using a different locker ID, meaning only cursors in
	 * our thread of control can be on the page.
	 *
	 * It's possible for multiple cursors within the thread to have write
	 * locks on the same page, but, cursors within a thread must be single
	 * threaded, so all we're locking here is the cursor linked list.
	 */
	DB_THREAD_LOCK(edbp);
	for (count = 0, edbc = TAILQ_FIRST(&edbp->active_queue);
	    edbc != NULL; edbc = TAILQ_NEXT(edbc, links)) {
		cp = (CURSOR *)edbc->internal;

		if ((cp->pgno == pgno && cp->indx == indx) ||
		    (cp->dpgno == pgno && cp->dindx == indx)) {
			if (delete)
				F_SET(cp, C_DELETED);
			else
				F_CLR(cp, C_DELETED);
			++count;
		}
	}
	DB_THREAD_UNLOCK(edbp);

	return (count);
}

/*
 * __bam_ca_di --
 *	Adjust the cursors during a delete or insert.
 *
 * PUBLIC: void __bam_ca_di __P((DB *, edb_pgno_t, u_int32_t, int));
 */
void
__bam_ca_di(edbp, pgno, indx, adjust)
	DB *edbp;
	edb_pgno_t pgno;
	u_int32_t indx;
	int adjust;
{
	CURSOR *cp;
	DBC *edbc;

	/* Recno is responsible for its own adjustments. */
	if (edbp->type == DB_RECNO)
		return;

	/*
	 * Adjust the cursors.  See the comment in __bam_ca_delete().
	 */
	DB_THREAD_LOCK(edbp);
	for (edbc = TAILQ_FIRST(&edbp->active_queue);
	    edbc != NULL; edbc = TAILQ_NEXT(edbc, links)) {
		cp = (CURSOR *)edbc->internal;
		if (cp->pgno == pgno && cp->indx >= indx)
			cp->indx += adjust;
		if (cp->dpgno == pgno && cp->dindx >= indx)
			cp->dindx += adjust;
	}
	DB_THREAD_UNLOCK(edbp);
}

/*
 * __bam_ca_dup --
 *	Adjust the cursors when moving items from a leaf page to a duplicates
 *	page.
 *
 * PUBLIC: void __bam_ca_dup __P((DB *,
 * PUBLIC:    edb_pgno_t, u_int32_t, u_int32_t, edb_pgno_t, u_int32_t));
 */
void
__bam_ca_dup(edbp, fpgno, first, fi, tpgno, ti)
	DB *edbp;
	edb_pgno_t fpgno, tpgno;
	u_int32_t first, fi, ti;
{
	CURSOR *cp;
	DBC *edbc;

	/* Recno is responsible for its own adjustments. */
	if (edbp->type == DB_RECNO)
		return;

	/*
	 * Adjust the cursors.  See the comment in __bam_ca_delete().
	 */
	DB_THREAD_LOCK(edbp);
	for (edbc = TAILQ_FIRST(&edbp->active_queue);
	    edbc != NULL; edbc = TAILQ_NEXT(edbc, links)) {
		cp = (CURSOR *)edbc->internal;
		/*
		 * Ignore matching entries that have already been moved,
		 * we move from the same location on the leaf page more
		 * than once.
		 */
		if (cp->dpgno == PGNO_INVALID &&
		    cp->pgno == fpgno && cp->indx == fi) {
			cp->indx = first;
			cp->dpgno = tpgno;
			cp->dindx = ti;
		}
	}
	DB_THREAD_UNLOCK(edbp);
}

/*
 * __bam_ca_rsplit --
 *	Adjust the cursors when doing reverse splits.
 *
 * PUBLIC: void __bam_ca_rsplit __P((DB *, edb_pgno_t, edb_pgno_t));
 */
void
__bam_ca_rsplit(edbp, fpgno, tpgno)
	DB *edbp;
	edb_pgno_t fpgno, tpgno;
{
	CURSOR *cp;
	DBC *edbc;

	/* Recno is responsible for its own adjustments. */
	if (edbp->type == DB_RECNO)
		return;

	/*
	 * Adjust the cursors.  See the comment in __bam_ca_delete().
	 */
	DB_THREAD_LOCK(edbp);
	for (edbc = TAILQ_FIRST(&edbp->active_queue);
	    edbc != NULL; edbc = TAILQ_NEXT(edbc, links)) {
		cp = (CURSOR *)edbc->internal;
		if (cp->pgno == fpgno)
			cp->pgno = tpgno;
	}
	DB_THREAD_UNLOCK(edbp);
}

/*
 * __bam_ca_split --
 *	Adjust the cursors when splitting a page.
 *
 * PUBLIC: void __bam_ca_split __P((DB *,
 * PUBLIC:    edb_pgno_t, edb_pgno_t, edb_pgno_t, u_int32_t, int));
 */
void
__bam_ca_split(edbp, ppgno, lpgno, rpgno, split_indx, cleft)
	DB *edbp;
	edb_pgno_t ppgno, lpgno, rpgno;
	u_int32_t split_indx;
	int cleft;
{
	DBC *edbc;
	CURSOR *cp;

	/* Recno is responsible for its own adjustments. */
	if (edbp->type == DB_RECNO)
		return;

	/*
	 * Adjust the cursors.  See the comment in __bam_ca_delete().
	 *
	 * If splitting the page that a cursor was on, the cursor has to be
	 * adjusted to point to the same record as before the split.  Most
	 * of the time we don't adjust pointers to the left page, because
	 * we're going to copy its contents back over the original page.  If
	 * the cursor is on the right page, it is decremented by the number of
	 * records split to the left page.
	 */
	DB_THREAD_LOCK(edbp);
	for (edbc = TAILQ_FIRST(&edbp->active_queue);
	    edbc != NULL; edbc = TAILQ_NEXT(edbc, links)) {
		cp = (CURSOR *)edbc->internal;
		if (cp->pgno == ppgno) {
			if (cp->indx < split_indx) {
				if (cleft)
					cp->pgno = lpgno;
			} else {
				cp->pgno = rpgno;
				cp->indx -= split_indx;
			}
		}

		if (cp->dpgno == ppgno) {
			if (cp->dindx < split_indx) {
				if (cleft)
					cp->dpgno = lpgno;
			} else {
				cp->dpgno = rpgno;
				cp->dindx -= split_indx;
			}
		}
	}
	DB_THREAD_UNLOCK(edbp);
}
