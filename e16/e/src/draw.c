/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#include <sys/ipc.h>
#include <sys/shm.h>

/* Hmmm... */
#define HIQ Conf.pagers.hiq

void
HandleDrawQueue()
{
   DrawQueue         **lst = NULL, *dq;
   int                 i, num;
   char                already, p_queue;

   EDBUG(4, "HandleDrawQueue");
   switch (Mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	if (Conf.movemode > 0)
	   EDBUG_RETURN_;
	break;
     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	if (Conf.resizemode > 0)
	   EDBUG_RETURN_;
	break;
     }

   p_queue = queue_up;
   queue_up = 0;
   num = 0;
   /* find all DRAW queue entries most recent first and add them to the */
   /* end of the draw list array if there are no previous entries for that */
   /* draw type and that window in the array */
   while ((dq =
	   (DrawQueue *) RemoveItem(NULL, 0, LIST_FINDBY_NONE, LIST_TYPE_DRAW)))
     {
	already = 0;

	if (dq->shape_propagate)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->win == dq->win) && (lst[i]->shape_propagate))
		    {
		       already = 1;
		       break;
		    }
	       }
	  }
	else if (dq->text)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->win == dq->win) && (lst[i]->text))
		    {
		       already = 1;
		       break;
		    }
	       }
	  }
	else if (dq->iclass)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->win == dq->win) && (!lst[i]->shape_propagate)
		      && (!lst[i]->text))
		    {
		       already = 1;
		       break;
		    }
	       }
	  }
	else if (dq->pager)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->win == dq->win) && (lst[i]->pager))
		    {
		       already = 1;
		       break;
		    }
	       }
	  }
	else if (dq->d)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->d == dq->d) && DialogItem(dq->d) &&
		      /*(dq->d->item == dq->di) && */ (lst[i]->di == dq->di))
		    {
		       if (dq->x < lst[i]->x)
			 {
			    lst[i]->w += (lst[i]->x - dq->x);
			    lst[i]->x = dq->x;
			 }
		       if ((lst[i]->x + lst[i]->w) < (dq->x + dq->w))
			  lst[i]->w +=
			     (dq->x + dq->w) - (lst[i]->x + lst[i]->w);
		       if (dq->y < lst[i]->y)
			 {
			    lst[i]->h += (lst[i]->y - dq->y);
			    lst[i]->y = dq->y;
			 }
		       if ((lst[i]->y + lst[i]->h) < (dq->y + dq->h))
			  lst[i]->h +=
			     (dq->y + dq->h) - (lst[i]->y + lst[i]->h);
		       already = 1;
		       break;
		    }
	       }
	  }
	else if (dq->redraw_pager)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->win == dq->win) && (lst[i]->redraw_pager))
		    {
		       switch (lst[i]->newbg)
			 {
			 case 0:
			    if (dq->newbg == 1)
			       lst[i]->newbg = 1;
			    else if (dq->newbg == 2)
			       lst[i]->newbg = 1;
			    break;
			 case 1:
			    break;
			 case 2:
			    if (dq->newbg == 1)
			       lst[i]->newbg = 1;
			    else if (dq->newbg == 0)
			       lst[i]->newbg = 1;
			    break;
			 case 3:
			    if (dq->newbg == 1)
			       lst[i]->newbg = 1;
			    else if (dq->newbg == 0)
			       lst[i]->newbg = 0;
			    else if (dq->newbg == 2)
			       lst[i]->newbg = 2;
			    break;
			 default:
			    break;
			 }
		       already = 1;
		       break;
		    }
	       }
	  }

	if (already)
	  {
	     if (dq)
	       {
		  if (dq->iclass)
		     dq->iclass->ref_count--;
		  if (dq->tclass)
		     dq->tclass->ref_count--;
		  if (dq->text)
		     Efree(dq->text);
		  Efree(dq);
	       }
	  }
	else
	  {
	     num++;
	     lst = Erealloc(lst, num * sizeof(DrawQueue *));
	     lst[num - 1] = dq;
	  }
     }
   /* go thru the list in chronological order (ie reverse) and do the draws */
   if (lst)
     {
	for (i = num - 1; i >= 0; i--)
	  {
	     dq = lst[i];
	     if (dq->shape_propagate)
	       {
/*            printf("S %x\n", dq->win); */
		  if (WinExists(dq->win))
		     PropagateShapes(dq->win);
	       }
	     else if (dq->text)
	       {
/*            printf("T %x\n", dq->win); */
		  if (WinExists(dq->win))
		     TclassApply(dq->iclass, dq->win, dq->w, dq->h, dq->active,
				 dq->sticky, dq->state, dq->expose, dq->tclass,
				 dq->text);
		  Efree(dq->text);
	       }
	     else if (dq->iclass)
	       {
/*            printf("I %x\n", dq->win); */
		  if (WinExists(dq->win))
		     IclassApply(dq->iclass, dq->win, dq->w, dq->h, dq->active,
				 dq->sticky, dq->state, 0);
	       }
	     else if (dq->pager)
	       {
/*            printf("P %x\n", dq->win); */
		  if (FindItem
		      ((char *)(dq->pager), 0, LIST_FINDBY_POINTER,
		       LIST_TYPE_PAGER))
		     PagerForceUpdate(dq->pager);
	       }
	     else if (dq->d)
	       {
/*            printf("D %x\n", dq->d->ewin->client.win); */
		  if (FindItem
		      ((char *)(dq->d), 0, LIST_FINDBY_POINTER,
		       LIST_TYPE_DIALOG))
		     DialogDrawItems(dq->d, dq->di, dq->x, dq->y, dq->w, dq->h);
	       }
	     else if (dq->redraw_pager)
	       {
/*            printf("p %x\n", dq->win); */
		  if (FindItem
		      ((char *)(dq->redraw_pager), 0, LIST_FINDBY_POINTER,
		       LIST_TYPE_PAGER))
		     PagerRedraw(dq->redraw_pager, dq->newbg);
	       }
	     if (dq->iclass)
		dq->iclass->ref_count--;
	     if (dq->tclass)
		dq->tclass->ref_count--;
	     Efree(dq);
	  }
	Efree(lst);
     }

   queue_up = p_queue;
   EDBUG_RETURN_;
}

char
IsPropagateEwinOnQueue(EWin * ewin)
{
   EDBUG(6, "IsPropagateOnQueue");

   if (FindItem(NULL, ewin->win, LIST_FINDBY_ID, LIST_TYPE_DRAW))
      EDBUG_RETURN(1);
   EDBUG_RETURN(0);
}

void
EFillPixmap(Window win, Pixmap pmap, int x, int y, int w, int h)
{
   XGCValues           gcv;
   GC                  gc;

   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
   XCopyArea(disp, win, pmap, gc, x, y, w, h, x, y);
   XFreeGC(disp, gc);
}

void
EPastePixmap(Window win, Pixmap pmap, int x, int y, int w, int h)
{
   XGCValues           gcv;
   GC                  gc;

   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
   XCopyArea(disp, pmap, win, gc, x, y, w, h, x, y);
   XFreeGC(disp, gc);
}

PixImg             *
ECreatePixImg(Window win, int w, int h)
{
   XGCValues           gcv;
   int                 bpp;
   PixImg             *pi;

   if (root.depth <= 8)
      bpp = 1;
   else if (root.depth <= 16)
      bpp = 2;
   else if (root.depth <= 24)
      bpp = 3;
   else
      bpp = 4;
#if !USE_IMLIB2
   if ((pImlib_Context->max_shm) && ((bpp * w * h) > pImlib_Context->max_shm))
      return NULL;
   if ((!pImlib_Context->x.shm) || (!pImlib_Context->x.shmp))
      return NULL;
#endif

   pi = Emalloc(sizeof(PixImg));
   if (!pi)
      return NULL;
   pi->shminfo = Emalloc(sizeof(XShmSegmentInfo));
   if (!pi->shminfo)
     {
	Efree(pi);
	return NULL;
     }
   pi->xim =
      XShmCreateImage(disp, root.vis, root.depth, ZPixmap, NULL, pi->shminfo,
		      w, h);
   if (!pi->xim)
     {
	Efree(pi->shminfo);
	Efree(pi);
	return NULL;
     }
   pi->shminfo->shmid =
      shmget(IPC_PRIVATE, pi->xim->bytes_per_line * pi->xim->height,
	     IPC_CREAT | 0666);
   if (pi->shminfo->shmid < 0)
     {
	XDestroyImage(pi->xim);
	Efree(pi->shminfo);
	Efree(pi);
	return NULL;
     }
   pi->shminfo->shmaddr = pi->xim->data = shmat(pi->shminfo->shmid, 0, 0);
   if (!pi->shminfo->shmaddr)
     {
	shmctl(pi->shminfo->shmid, IPC_RMID, 0);
	XDestroyImage(pi->xim);
	Efree(pi->shminfo);
	Efree(pi);
	return NULL;
     }
   pi->shminfo->readOnly = False;
   XShmAttach(disp, pi->shminfo);
   pi->pmap =
      XShmCreatePixmap(disp, win, pi->shminfo->shmaddr, pi->shminfo, w, h,
		       root.depth);
   if (!pi->pmap)
     {
	XShmDetach(disp, pi->shminfo);
	shmdt(pi->shminfo->shmaddr);
	shmctl(pi->shminfo->shmid, IPC_RMID, 0);
	XDestroyImage(pi->xim);
	Efree(pi->shminfo);
	Efree(pi);
	return NULL;
     }
   gcv.subwindow_mode = IncludeInferiors;
   pi->gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
   if (!pi->gc)
     {
	XShmDetach(disp, pi->shminfo);
	shmdt(pi->shminfo->shmaddr);
	shmctl(pi->shminfo->shmid, IPC_RMID, 0);
	XDestroyImage(pi->xim);
	Efree(pi->shminfo);
	XFreePixmap(disp, pi->pmap);
	Efree(pi);
	return NULL;
     }
   return pi;
}

void
EDestroyPixImg(PixImg * pi)
{
   if (!pi)
      return;
   XSync(disp, False);
   XShmDetach(disp, pi->shminfo);
   shmdt(pi->shminfo->shmaddr);
   shmctl(pi->shminfo->shmid, IPC_RMID, 0);
   XDestroyImage(pi->xim);
   Efree(pi->shminfo);
   XFreePixmap(disp, pi->pmap);
   XFreeGC(disp, pi->gc);
   Efree(pi);
}

void
EBlendRemoveShape(EWin * ewin, Pixmap pmap, int x, int y)
{
   XGCValues           gcv;
   int                 i, w, h;
   static GC           gc = 0, gcm = 0;
   static int          rn, ord;
   static XRectangle  *rl = NULL;
   static Pixmap       mask = 0;

   if (!ewin)
     {
	if (rl)
	   XFree(rl);
	if (gc)
	   XFreeGC(disp, gc);
	if (gcm)
	   XFreeGC(disp, gcm);
	if (mask)
	   EFreePixmap(disp, mask);
	mask = 0;
	gc = 0;
	gcm = 0;
	rl = NULL;
	return;
     }

   w = ewin->w;
   h = ewin->h;
   if (!rl)
     {
	rl = EShapeGetRectangles(disp, ewin->win, ShapeBounding, &rn, &ord);
	if (rn < 1)
	   return;
	else if (rn == 1)
	  {
	     if ((rl[0].x == 0) && (rl[0].y == 0) && (rl[0].width == ewin->w)
		 && (rl[0].height == ewin->h))
	       {
		  if (rl)
		     XFree(rl);
		  rl = NULL;
		  return;
	       }
	  }
     }
   if (!mask)
      mask = ECreatePixmap(disp, root.win, w, h, 1);
   if (!gcm)
      gcm = XCreateGC(disp, mask, 0, &gcv);
   if (!gc)
     {
	gcv.subwindow_mode = IncludeInferiors;
	gc = XCreateGC(disp, root.win, GCSubwindowMode, &gcv);
	XSetForeground(disp, gcm, 1);
	XFillRectangle(disp, mask, gcm, 0, 0, w, h);
	XSetForeground(disp, gcm, 0);
	for (i = 0; i < rn; i++)
	   XFillRectangle(disp, mask, gcm, rl[i].x, rl[i].y, rl[i].width,
			  rl[i].height);
	XSetClipMask(disp, gc, mask);
     }
   XSetClipOrigin(disp, gc, x, y);
   XCopyArea(disp, pmap, root.win, gc, x, y, w, h, x, y);
}

void
EBlendPixImg(EWin * ewin, PixImg * s1, PixImg * s2, PixImg * dst, int x, int y,
	     int w, int h)
{
   int                 ox, oy;
   int                 i, j;
   XGCValues           gcv;
   static int          rn, ord;
   static XRectangle  *rl = NULL;
   static GC           gc = 0;

   if (!s1)
     {
	if (gc)
	   XFreeGC(disp, gc);
	if (rl > (XRectangle *) 1)
	   XFree(rl);
	gc = 0;
	rl = NULL;
	return;
     }
   if (!gc)
     {
	gcv.subwindow_mode = IncludeInferiors;
	gc = XCreateGC(disp, root.win, GCSubwindowMode, &gcv);
     }
   if (!rl)
     {
	rl = EShapeGetRectangles(disp, ewin->win, ShapeBounding, &rn, &ord);
	if (rl)
	   XSetClipRectangles(disp, gc, x, y, rl, rn, ord);
	if (!rl)
	   rl = (XRectangle *) 1;
     }
   else
      XSetClipOrigin(disp, gc, x, y);
   ox = 0;
   oy = 0;
   if ((x >= root.w) || (y >= root.h))
      return;
   if (x + w > root.w)
      w -= ((x + w) - root.w);
   if (x < 0)
     {
	ox = -x;
	w -= ox;
	x = 0;
     }
   if (y + h > root.h)
      h -= ((y + h) - root.h);
   if (y < 0)
     {
	oy = -y;
	h -= oy;
	y = 0;
     }
   if ((w <= 0) || (h <= 0))
      return;
   XSync(disp, False);
   if (dst)
     {
	switch (dst->xim->bits_per_pixel)
	  {
	  case 32:
	     for (j = 0; j < h; j++)
	       {
		  unsigned int       *ptr1, *ptr2, *ptr3;

		  ptr1 =
		     (unsigned int *)(s1->xim->data +
				      ((x) *
				       ((s1->xim->bits_per_pixel) >> 3)) +
				      ((j + y) * s1->xim->bytes_per_line));
		  ptr2 =
		     (unsigned int *)(s2->xim->data +
				      ((ox) *
				       ((s2->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * s2->xim->bytes_per_line));
		  ptr3 =
		     (unsigned int *)(dst->xim->data +
				      ((ox) *
				       ((dst->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * dst->xim->bytes_per_line));
		  for (i = 0; i < w; i++)
		    {
		       unsigned int        p1, p2;

		       p1 = *ptr1++;
		       p2 = *ptr2++;
		       *ptr3++ =
			  ((p1 >> 1) & 0x7f7f7f7f) +
			  ((p2 >> 1) & 0x7f7f7f7f) + (p1 & p2 & 0x01010101);
		    }
	       }
	     break;
	  case 24:
	     for (j = 0; j < h; j++)
	       {
		  for (i = 0; i < w; i++)
		    {
		       unsigned int        p1, p2;

		       p1 = XGetPixel(s1->xim, (i + x), (j + y));
		       p2 = XGetPixel(s2->xim, (i + ox), (j + oy));
		       XPutPixel(dst->xim, (i + ox), (j + oy),
				 (((p1 >> 1) & 0x7f7f7f7f) +
				  ((p2 >> 1) & 0x7f7f7f7f) +
				  (p1 & p2 & 0x01010101)));
		    }
	       }
	     break;
	  case 16:
	     if (IC_RenderDepth() != 15)
	       {
		  for (j = 0; j < h; j++)
		    {
		       unsigned int       *ptr1, *ptr2, *ptr3;

		       ptr1 =
			  (unsigned int *)(s1->xim->data +
					   ((x) *
					    ((s1->xim->bits_per_pixel) >> 3)) +
					   ((j + y) * s1->xim->bytes_per_line));
		       ptr2 =
			  (unsigned int *)(s2->xim->data +
					   ((ox) *
					    ((s2->xim->bits_per_pixel) >> 3)) +
					   ((j +
					     oy) * s2->xim->bytes_per_line));
		       ptr3 =
			  (unsigned int *)(dst->xim->data +
					   ((ox) *
					    ((dst->xim->bits_per_pixel) >> 3)) +
					   ((j +
					     oy) * dst->xim->bytes_per_line));
		       if (!(w & 0x1))
			 {
			    for (i = 0; i < w; i += 2)
			      {
				 unsigned int        p1, p2;

				 p1 = *ptr1++;
				 p2 = *ptr2++;
				 *ptr3++ =
				    ((p1 >> 1) &
				     ((0x78 << 8) | (0x7c << 3) | (0x78 >> 3)
				      | (0x78 << 24) | (0x7c << 19) | (0x78
								       <<
								       13)))
				    +
				    ((p2 >> 1) &
				     ((0x78 << 8) | (0x7c << 3) | (0x78 >> 3)
				      | (0x78 << 24) | (0x7c << 19) | (0x78
								       <<
								       13)))
				    +
				    (p1 & p2 &
				     ((0x1 << 11) | (0x1 << 5) | (0x1) |
				      (0x1 << 27) | (0x1 << 21) | (0x1 << 16)));
			      }
			 }
		       else
			 {
			    for (i = 0; i < (w - 1); i += 2)
			      {
				 unsigned int        p1, p2;

				 p1 = *ptr1++;
				 p2 = *ptr2++;
				 *ptr3++ =
				    ((p1 >> 1) &
				     ((0x78 << 8) | (0x7c << 3) | (0x78 >> 3)
				      | (0x78 << 24) | (0x7c << 19) | (0x78
								       <<
								       13)))
				    +
				    ((p2 >> 1) &
				     ((0x78 << 8) | (0x7c << 3) | (0x78 >> 3)
				      | (0x78 << 24) | (0x7c << 19) | (0x78
								       <<
								       13)))
				    +
				    (p1 & p2 &
				     ((0x1 << 11) | (0x1 << 5) | (0x1) |
				      (0x1 << 27) | (0x1 << 21) | (0x1 << 16)));
			      }
			    {
			       unsigned short     *pptr1, *pptr2, *pptr3;
			       unsigned short      pp1, pp2;

			       pptr1 = (unsigned short *)ptr1;
			       pptr2 = (unsigned short *)ptr2;
			       pptr3 = (unsigned short *)ptr3;
			       pp1 = *pptr1;
			       pp2 = *pptr2;
			       *pptr3 =
				  ((pp1 >> 1) &
				   ((0x78 << 8) | (0x7c << 3) | (0x78 >> 3)))
				  +
				  ((pp2 >> 1) &
				   ((0x78 << 8) | (0x7c << 3) | (0x78 >> 3)))
				  +
				  (pp1 & pp2 &
				   ((0x1 << 11) | (0x1 << 5) | (0x1)));
			    }
			 }
		    }
	       }
	     else
	       {
		  for (j = 0; j < h; j++)
		    {
		       unsigned int       *ptr1, *ptr2, *ptr3;

		       ptr1 =
			  (unsigned int *)(s1->xim->data +
					   ((x) *
					    ((s1->xim->
					      bits_per_pixel) >> 3)) + ((j +
									 y) *
									s1->
									xim->
									bytes_per_line));
		       ptr2 =
			  (unsigned int *)(s2->xim->data +
					   ((ox) *
					    ((s2->xim->
					      bits_per_pixel) >> 3)) + ((j +
									 oy)
									*
									s2->
									xim->
									bytes_per_line));
		       ptr3 =
			  (unsigned int *)(dst->xim->data +
					   ((ox) *
					    ((dst->xim->
					      bits_per_pixel) >> 3)) + ((j +
									 oy)
									*
									dst->
									xim->
									bytes_per_line));
		       if (!(w & 0x1))
			 {
			    for (i = 0; i < w; i += 2)
			      {
				 unsigned int        p1, p2;

				 p1 = *ptr1++;
				 p2 = *ptr2++;
				 *ptr3++ =
				    ((p1 >> 1) &
				     ((0x78 << 7) | (0x78 << 2) | (0x78 >> 3)
				      | (0x78 << 23) | (0x78 << 18) | (0x78
								       <<
								       13)))
				    +
				    ((p2 >> 1) &
				     ((0x78 << 7) | (0x78 << 2) | (0x78 >> 3)
				      | (0x78 << 23) | (0x78 << 18) | (0x78
								       <<
								       13)))
				    +
				    (p1 & p2 &
				     ((0x1 << 10) | (0x1 << 5) | (0x1) |
				      (0x1 << 26) | (0x1 << 20) | (0x1 << 16)));
			      }
			 }
		       else
			 {
			    for (i = 0; i < (w - 1); i += 2)
			      {
				 unsigned int        p1, p2;

				 p1 = *ptr1++;
				 p2 = *ptr2++;
				 *ptr3++ =
				    ((p1 >> 1) &
				     ((0x78 << 7) | (0x78 << 2) | (0x78 >> 3)
				      | (0x78 << 23) | (0x78 << 18) | (0x78
								       <<
								       13)))
				    +
				    ((p2 >> 1) &
				     ((0x78 << 7) | (0x78 << 2) | (0x78 >> 3)
				      | (0x78 << 23) | (0x78 << 18) | (0x78
								       <<
								       13)))
				    +
				    (p1 & p2 &
				     ((0x1 << 10) | (0x1 << 5) | (0x1) |
				      (0x1 << 26) | (0x1 << 20) | (0x1 << 16)));
			      }
			    {
			       unsigned short     *pptr1, *pptr2, *pptr3;
			       unsigned short      pp1, pp2;

			       pptr1 = (unsigned short *)ptr1;
			       pptr2 = (unsigned short *)ptr2;
			       pptr3 = (unsigned short *)ptr3;
			       pp1 = *pptr1;
			       pp2 = *pptr2;
			       *pptr3++ =
				  ((pp1 >> 1) &
				   ((0x78 << 7) | (0x78 << 2) | (0x78 >> 3)))
				  +
				  ((pp2 >> 1) &
				   ((0x78 << 7) | (0x78 << 2) | (0x78 >> 3)))
				  +
				  (pp1 & pp2 &
				   ((0x1 << 10) | (0x1 << 5) | (0x1)));
			    }
			 }
		    }
	       }
	     break;
	  default:
	     for (j = 0; j < h; j++)
	       {
		  unsigned char      *ptr1, *ptr2, *ptr3;

		  ptr1 =
		     (unsigned char *)(s1->xim->data +
				       ((x) *
					((s1->xim->bits_per_pixel) >> 3)) +
				       ((j + y) * s1->xim->bytes_per_line));
		  ptr2 =
		     (unsigned char *)(s2->xim->data +
				       ((ox) *
					((s2->xim->bits_per_pixel) >> 3)) +
				       ((j + oy) * s2->xim->bytes_per_line));
		  ptr3 =
		     (unsigned char *)(dst->xim->data +
				       ((ox) *
					((dst->xim->bits_per_pixel) >> 3)) +
				       ((j + oy) * dst->xim->bytes_per_line));
		  if (!(w & 0x1))
		    {
		       if (j & 0x1)
			 {
			    ptr2++;
			    for (i = 0; i < w; i += 2)
			      {
				 unsigned char       p1;

				 p1 = *ptr1;
				 ptr1 += 2;
				 *ptr3++ = p1;
				 p1 = *ptr2;
				 ptr2 += 2;
				 *ptr3++ = p1;
			      }
			 }
		       else
			 {
			    ptr1++;
			    for (i = 0; i < w; i += 2)
			      {
				 unsigned char       p1;

				 p1 = *ptr2;
				 ptr2 += 2;
				 *ptr3++ = p1;
				 p1 = *ptr1;
				 ptr1 += 2;
				 *ptr3++ = p1;
			      }
			 }
		    }
		  else
		    {
		       if (j & 0x1)
			 {
			    ptr2++;
			    for (i = 0; i < (w - 1); i += 2)
			      {
				 unsigned char       p1;

				 p1 = *ptr1;
				 ptr1 += 2;
				 *ptr3++ = p1;
				 p1 = *ptr2;
				 ptr2 += 2;
				 *ptr3++ = p1;
			      }
			    *ptr3 = *ptr1;
			 }
		       else
			 {
			    ptr1++;
			    for (i = 0; i < (w - 1); i += 2)
			      {
				 unsigned char       p1;

				 p1 = *ptr2;
				 ptr2 += 2;
				 *ptr3++ = p1;
				 p1 = *ptr1;
				 ptr1 += 2;
				 *ptr3++ = p1;
			      }
			    *ptr3 = *ptr2;
			 }
		    }
	       }
	     break;
	  }
/* workaround since XCopyArea doesnt always work with shared pixmaps */
	XShmPutImage(disp, root.win, gc, dst->xim, ox, oy, x, y, w, h, False);
/*      XCopyArea(disp, dst->pmap, root.win, gc, ox, oy, w, h, x, y); */
     }
/* I dont believe it - you cannot do this to a shared pixmaps to the screen */
/* XCopyArea(disp, dst->pmap, root.win, dst->gc, x, y, w, h, x, y); */
}

void
ScaleLine(Pixmap dest, Window src, int dx, int dy, int sw, int pw, int sy,
	  int sh)
{
   static GC           gc = 0;
   XGCValues           gcv;
   int                 x, x2;
   PixImg             *p_grab = NULL, *p_grab2 = NULL, *p_buf = NULL;
   XImage             *px_grab = NULL, *px_grab2 = NULL, *px_buf = NULL;

   if (!gc)
     {
	gcv.subwindow_mode = IncludeInferiors;
	gc = XCreateGC(disp, src, GCSubwindowMode, &gcv);
     }

   p_grab = ECreatePixImg(dest, sw, 1);
   if (p_grab)
     {
	if (HIQ)
	  {
	     p_grab2 = ECreatePixImg(dest, sw, 1);
	     if (!p_grab2)
	       {
		  EDestroyPixImg(p_grab);
		  p_grab = NULL;
	       }
	  }

	if (p_grab)
	  {
	     p_buf = ECreatePixImg(dest, pw, 1);
	     if (p_buf)
	       {
		  XCopyArea(disp, src, p_grab->pmap, gc, 0, sy, sw, 1, 0, 0);
		  if (HIQ)
		     XCopyArea(disp, src, p_grab2->pmap, gc, 0, sy + (sh / 2),
			       sw, 1, 0, 0);
		  XSync(disp, False);
	       }
	  }
	else
	  {
	     EDestroyPixImg(p_grab);
	     if (p_grab2)
		EDestroyPixImg(p_grab2);
	     p_grab = NULL;
	  }
     }

   if (!p_grab)
     {
	px_grab = XGetImage(disp, src, 0, sy, sw, 1, 0xffffffff, ZPixmap);
	if (!px_grab)
	   return;

	if (HIQ)
	   px_grab2 =
	      XGetImage(disp, src, 0, sy + (sh / 2), sw, 1, 0xffffffff,
			ZPixmap);
	if (!px_grab2)
	  {
	     XDestroyImage(px_grab);
	     return;
	  }

	px_buf =
	   XCreateImage(disp, root.vis, root.depth, ZPixmap, 0, NULL, pw, 1,
			32, 0);
	if (!px_buf)
	  {
	     XDestroyImage(px_grab);
	     XDestroyImage(px_grab2);
	     return;
	  }

	px_buf->data = malloc(px_buf->bytes_per_line * px_buf->height);
	if (!px_buf->data)
	  {
	     XDestroyImage(px_grab);
	     XDestroyImage(px_grab2);
	     XDestroyImage(px_buf);
	     return;
	  }
     }

   if (HIQ)
     {
	int                 v1, v2, v3, v4, difx;
	XImage             *xim1 = NULL, *xim2 = NULL, *xim3 = NULL;

	if (p_grab)
	  {
	     xim1 = p_grab->xim;
	     xim2 = p_grab2->xim;
	     xim3 = p_buf->xim;
	  }
	else
	  {
	     xim1 = px_grab;
	     xim2 = px_grab2;
	     xim3 = px_buf;
	  }

	difx = (sw / pw) / 2;
	switch (IC_RenderDepth())
	  {
	  case 24:
	  case 32:
	     for (x = 0; x < pw; x++)
	       {
		  x2 = (sw * x) / pw;
		  v1 = XGetPixel(xim1, x2, 0);
		  v2 = XGetPixel(xim1, x2 + difx, 0);
		  v3 = XGetPixel(xim2, x2, 0);
		  v4 = XGetPixel(xim2, x2 + difx, 0);
		  v1 = ((v1 >> 2) & 0x3f3f3f3f) + ((v2 >> 2) & 0x3f3f3f3f) +
		     ((v3 >> 2) & 0x3f3f3f3f) + ((v4 >> 2) & 0x3f3f3f3f) +
		     (v1 & v2 & v3 & v4 & 0x03030303);
		  XPutPixel(xim3, x, 0, v1);
	       }
	     break;
	  case 16:
	     for (x = 0; x < pw; x++)
	       {
		  x2 = (sw * x) / pw;
		  v1 = XGetPixel(xim1, x2, 0);
		  v2 = XGetPixel(xim1, x2 + difx, 0);
		  v3 = XGetPixel(xim2, x2, 0);
		  v4 = XGetPixel(xim2, x2 + difx, 0);
		  v1 = ((v1 >> 2) & ((0x70 << 7) | (0x78 << 2) | (0x70 >> 4)))
		     +
		     ((v2 >> 2) & ((0x70 << 7) | (0x78 << 2) | (0x70 >> 4)))
		     +
		     ((v3 >> 2) & ((0x70 << 7) | (0x78 << 2) | (0x70 >> 4)))
		     +
		     ((v4 >> 2) & ((0x70 << 7) | (0x78 << 2) | (0x70 >> 4)))
		     + (v1 & v2 & v3 & v4 & ((0x3 << 11) | (0x3 << 5) | (0x3)));
		  XPutPixel(xim3, x, 0, v1);
	       }
	     break;
	  case 15:
	     /*        break; */
	  default:
	     for (x = 0; x < pw; x++)
	       {
		  x2 = (sw * x) / pw;
		  XPutPixel(xim3, x, 0, XGetPixel(xim1, x2, 0));
	       }
	     break;
	  }
     }
   else
     {
	XImage             *xim1 = NULL, *xim3 = NULL;

	if (p_grab)
	  {
	     xim1 = p_grab->xim;
	     xim3 = p_buf->xim;
	  }
	else
	  {
	     xim1 = px_grab;
	     xim3 = px_buf;
	  }

	for (x = 0; x < pw; x++)
	  {
	     x2 = (sw * x) / pw;
	     XPutPixel(xim3, x, 0, XGetPixel(xim1, x2, 0));
	  }
     }

   if (p_buf)
     {
	XShmPutImage(disp, dest, gc, p_buf->xim, 0, 0, dx, dy, pw, 1, False);
	XSync(disp, False);
	if (p_grab)
	   EDestroyPixImg(p_grab);
	if (p_grab2)
	   EDestroyPixImg(p_grab2);
	if (p_buf)
	   EDestroyPixImg(p_buf);
     }
   else
     {
	XPutImage(disp, dest, gc, px_buf, 0, 0, dx, dy, pw, 1);
	if (px_grab)
	   XDestroyImage(px_grab);
	if (px_grab2)
	   XDestroyImage(px_grab2);
	if (px_buf)
	   XDestroyImage(px_buf);
     }
}

void
ScaleRect(Pixmap dest, Window src, int sx, int sy, int dx, int dy, int sw,
	  int sh, int dw, int dh)
{
   static GC           gc = 0, gc2 = 0;
   XGCValues           gcv;
   int                 y, y2, x, x2;
   PixImg             *p_grab = NULL, *p_buf = NULL;
   XImage             *px_grab = NULL, *px_buf = NULL;

   if (sw > root.w * 2)
      return;
   if (sh > root.h * 2)
      return;
   if (dw > root.w * 2)
      return;
   if (dh > root.h * 2)
      return;

   if (!gc)
     {
	gcv.subwindow_mode = IncludeInferiors;
	gc = XCreateGC(disp, src, GCSubwindowMode, &gcv);
	gc2 = XCreateGC(disp, src, 0, &gcv);
     }

   if (HIQ)
      p_grab = ECreatePixImg(src, sw, dh * 2);
   else
      p_grab = ECreatePixImg(src, sw, dh);

   if (p_grab)
     {
	p_buf = ECreatePixImg(dest, dw, dh);
	if (p_buf)
	  {
	     if (HIQ)
	       {
		  for (y = 0; y < (dh * 2); y++)
		    {
		       y2 = (sh * y) / (dh * 2);
		       XCopyArea(disp, src, p_grab->pmap, gc, sx, sy + y2, sw,
				 1, 0, y);
		    }
	       }
	     else
	       {
		  for (y = 0; y < dh; y++)
		    {
		       y2 = (sh * y) / dh;
		       XCopyArea(disp, src, p_grab->pmap, gc, sx, sy + y2, sw,
				 1, 0, y);
		    }
	       }
	     XSync(disp, False);
	  }
	else
	  {
	     EDestroyPixImg(p_grab);
	     p_grab = NULL;
	  }
     }

   if (!p_grab)
     {
	if (HIQ)
	  {
	     Pixmap              pmap;

	     pmap = ECreatePixmap(disp, src, sw, dh * 2, root.depth);
	     for (y = 0; y < (dh * 2); y++)
	       {
		  y2 = (sh * y) / (dh * 2);
		  XCopyArea(disp, src, pmap, gc, sx, sy + y2, sw, 1, 0, y);
	       }

	     px_grab =
		XGetImage(disp, pmap, 0, 0, sw, dh * 2, 0xffffffff, ZPixmap);
	     EFreePixmap(disp, pmap);
	     if (!px_grab)
		return;
	  }
	else
	  {
	     Pixmap              pmap;

	     pmap = ECreatePixmap(disp, src, sw, dh, root.depth);
	     for (y = 0; y < dh; y++)
	       {
		  y2 = (sh * y) / dh;
		  XCopyArea(disp, src, pmap, gc, sx, sy + y2, sw, 1, 0, y);
	       }

	     px_grab = XGetImage(disp, pmap, 0, 0, sw, dh, 0xffffffff, ZPixmap);
	     EFreePixmap(disp, pmap);
	     if (!px_grab)
		return;
	  }

	px_buf =
	   XCreateImage(disp, root.vis, root.depth, ZPixmap, 0, NULL, dw, dh,
			32, 0);
	if (!px_buf)
	  {
	     XDestroyImage(px_grab);
	     return;
	  }

	px_buf->data = malloc(px_buf->bytes_per_line * px_buf->height);
	if (!px_buf->data)
	  {
	     XDestroyImage(px_buf);
	     XDestroyImage(px_grab);
	     return;
	  }
     }

   if (HIQ)
     {
	int                 v1, v2, v3, v4, difx;
	XImage             *xim1 = NULL, *xim3 = NULL;

	if (p_grab)
	  {
	     xim1 = p_grab->xim;
	     xim3 = p_buf->xim;
	  }
	else
	  {
	     xim1 = px_grab;
	     xim3 = px_buf;
	  }

	difx = (sw / dw) / 2;
	switch (root.depth)
	  {
	  case 24:
	  case 32:
	     for (y = 0; y < dh; y++)
	       {
		  for (x = 0; x < dw; x++)
		    {
		       y2 = (y * 2);
		       x2 = (sw * x) / dw;
		       v1 = XGetPixel(xim1, x2, y2);
		       v2 = XGetPixel(xim1, x2 + difx, y2);
		       v3 = XGetPixel(xim1, x2, y2 + 1);
		       v4 = XGetPixel(xim1, x2 + difx, y2 + 1);
		       v1 = ((v1 >> 2) & 0x3f3f3f3f) +
			  ((v2 >> 2) & 0x3f3f3f3f) +
			  ((v3 >> 2) & 0x3f3f3f3f) +
			  ((v4 >> 2) & 0x3f3f3f3f) +
			  (v1 & v2 & v3 & v4 & 0x03030303);
		       XPutPixel(xim3, x, y, v1);
		    }
	       }
	     break;
	  case 16:
	     for (y = 0; y < dh; y++)
	       {
		  for (x = 0; x < dw; x++)
		    {
		       y2 = (y * 2);
		       x2 = (sw * x) / dw;
		       v1 = XGetPixel(xim1, x2, y2);
		       v2 = XGetPixel(xim1, x2 + difx, y2);
		       v3 = XGetPixel(xim1, x2, y2 + 1);
		       v4 = XGetPixel(xim1, x2 + difx, y2 + 1);
		       v1 = ((v1 >> 2) &
			     ((0x70 << 7) | (0x78 << 2) | (0x70 >> 4))) +
			  ((v2 >> 2) &
			   ((0x70 << 7) | (0x78 << 2) | (0x70 >> 4))) +
			  ((v3 >> 2) &
			   ((0x70 << 7) | (0x78 << 2) | (0x70 >> 4))) +
			  ((v4 >> 2) &
			   ((0x70 << 7) | (0x78 << 2) | (0x70 >> 4))) +
			  (v1 & v2 & v3 & v4 &
			   ((0x3 << 11) | (0x3 << 5) | (0x3)));
		       XPutPixel(xim3, x, y, v1);
		    }
	       }
	     break;
	  case 15:
	     /*        break; */
	  default:
	     for (y = 0; y < dh; y++)
	       {
		  for (x = 0; x < dw; x++)
		    {
		       y2 = (y * 2);
		       x2 = (sw * x) / dw;
		       XPutPixel(xim3, x, y, XGetPixel(xim1, x2, y2));
		    }
	       }
	     break;
	  }
     }
   else
     {
	XImage             *xim1 = NULL, *xim3 = NULL;

	if (p_grab)
	  {
	     xim1 = p_grab->xim;
	     xim3 = p_buf->xim;
	  }
	else
	  {
	     xim1 = px_grab;
	     xim3 = px_buf;
	  }

	for (y = 0; y < dh; y++)
	  {
	     for (x = 0; x < dw; x++)
	       {
		  x2 = (sw * x) / dw;
		  XPutPixel(xim3, x, y, XGetPixel(xim1, x2, y));
	       }
	  }
     }

   if (p_buf)
     {
	XShmPutImage(disp, dest, gc2, p_buf->xim, 0, 0, dx, dy, dw, dh, False);
	XSync(disp, False);
	if (p_grab)
	   EDestroyPixImg(p_grab);
	if (p_buf)
	   EDestroyPixImg(p_buf);
     }
   else
     {
	XPutImage(disp, dest, gc, px_buf, 0, 0, dx, dy, dw, dh);
	if (px_grab)
	   XDestroyImage(px_grab);
	if (px_buf)
	   XDestroyImage(px_buf);
     }
}

#include <X11/bitmaps/flipped_gray>
#include <X11/bitmaps/gray>
#include <X11/bitmaps/gray3>

void
DrawEwinShape(EWin * ewin, int md, int x, int y, int w, int h, char firstlast)
{
   static GC           gc = 0;
   XGCValues           gcv;
   int                 x1, y1, w1, h1, i, j, pw, ph;
   static Pixmap       b1 = 0, b2 = 0, b3 = 0;
   static Font         font = 0;
   int                 bpp;
   char                str[32], pq;
   char                check_move = 0;

   EDBUG(4, "DrawEwinShape");

   for (i = 0; i < ewin->num_groups; i++)
     {
	check_move |= ewin->groups[i]->cfg.move;
	if (check_move)
	   break;
     }

   if ((Mode.mode == MODE_RESIZE) || (Mode.mode == MODE_RESIZE_H)
       || (Mode.mode == MODE_RESIZE_V))
     {
	w1 = ewin->client.w;
	h1 = ewin->client.h;
	ewin->client.w = w;
	ewin->client.h = h;
	ICCCM_MatchSize(ewin);
	i = (x - ewin->x) / ewin->client.w_inc;
	j = (y - ewin->y) / ewin->client.h_inc;
	x = ewin->x + (i * ewin->client.w_inc);
	y = ewin->y + (j * ewin->client.h_inc);
	ewin->client.w = w1;
	ewin->client.h = h1;
     }
   if ((md == 5)
       && ((Mode.mode == MODE_RESIZE) || (Mode.mode == MODE_RESIZE_H)
	   || (Mode.mode == MODE_RESIZE_V) || (ewin->groups && check_move)))
      md = 0;
   if (md == 5)
     {
	if (root.depth <= 8)
	   bpp = 1;
	else if (root.depth <= 16)
	   bpp = 2;
	else if (root.depth <= 24)
	   bpp = 3;
	else
	   bpp = 4;
#if !USE_IMLIB2
	if ((prImlib_Context) ||
	    (pImlib_Context->max_shm &&
	     ((bpp * w * h) > pImlib_Context->max_shm)) ||
	    ((!pImlib_Context->x.shm) || (!pImlib_Context->x.shmp)))
	   md = 0;
#endif
     }
   pw = w;
   ph = h;
   pq = queue_up;
   queue_up = 0;
   switch (md)
     {
     case 0:
	MoveResizeEwin(ewin, x, y, w, h);
	if (Mode.mode != MODE_NONE)
	   SetCoords(ewin);
	break;
     case 1:
     case 2:
     case 3:
     case 4:
     case 5:
	if (!b1)
	   b1 = XCreateBitmapFromData(disp, root.win, flipped_gray_bits,
				      flipped_gray_width, flipped_gray_height);
	if (!b2)
	   b2 = XCreateBitmapFromData(disp, root.win, gray_bits, gray_width,
				      gray_height);
	if (!b3)
	   b3 = XCreateBitmapFromData(disp, root.win, gray3_bits, gray3_width,
				      gray3_height);
	x1 = ewin->x + desks.desk[ewin->desktop].x;
	y1 = ewin->y + desks.desk[ewin->desktop].y;
	w1 = ewin->w - (ewin->border->border.left + ewin->border->border.right);
	h1 = ewin->h - (ewin->border->border.top + ewin->border->border.bottom);
	ewin->x = x;
	ewin->y = y;
	ewin->reqx = x;
	ewin->reqy = y;
	x = ewin->x + desks.desk[ewin->desktop].x;
	y = ewin->y + desks.desk[ewin->desktop].y;
	if ((w != ewin->client.w) || (h != ewin->client.h))
	  {
	     ewin->client.w = w;
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     ewin->w =
		ewin->client.w + ewin->border->border.left +
		ewin->border->border.right;
	     ewin->h =
		ewin->client.h + ewin->border->border.top +
		ewin->border->border.bottom;
	  }
	w = ewin->w - (ewin->border->border.left + ewin->border->border.right);
	h = ewin->h - (ewin->border->border.top + ewin->border->border.bottom);
	if (!gc)
	  {
	     gcv.function = GXxor;
	     gcv.foreground = WhitePixel(disp, root.scr);
	     if (gcv.foreground == 0)
		gcv.foreground = BlackPixel(disp, root.scr);
	     gcv.subwindow_mode = IncludeInferiors;
	     gc = XCreateGC(disp, root.win,
			    GCFunction | GCForeground | GCSubwindowMode, &gcv);
	  }
#define DRAW_H_ARROW(x1, x2, y1) \
      if (((x2) - (x1)) >= 12) \
        { \
          XDrawLine(disp, root.win, gc, x1, y1, (x1) + 6, (y1) - 3); \
          XDrawLine(disp, root.win, gc, x1, y1, (x1) + 6, (y1) + 3); \
          XDrawLine(disp, root.win, gc, x2, y1, (x2) - 6, (y1) - 3); \
          XDrawLine(disp, root.win, gc, x2, y1, (x2) - 6, (y1) + 3); \
        } \
      if ((x2) >= (x1)) \
        { \
          XDrawLine(disp, root.win, gc, x1, y1, x2, y1); \
          Esnprintf(str, sizeof(str), "%i", (x2) - (x1) + 1); \
          XDrawString(disp, root.win, gc, ((x1) + (x2)) / 2, (y1) - 10, str, strlen(str)); \
        }
#define DRAW_V_ARROW(y1, y2, x1) \
      if (((y2) - (y1)) >= 12) \
        { \
          XDrawLine(disp, root.win, gc, x1, y1, (x1) + 3, (y1) + 6); \
          XDrawLine(disp, root.win, gc, x1, y1, (x1) - 3, (y1) + 6); \
          XDrawLine(disp, root.win, gc, x1, y2, (x1) + 3, (y2) - 6); \
          XDrawLine(disp, root.win, gc, x1, y2, (x1) - 3, (y2) - 6); \
        } \
      if ((y2) >= (y1)) \
        { \
          XDrawLine(disp, root.win, gc, x1, y1, x1, y2); \
          Esnprintf(str, sizeof(str), "%i", (y2) - (y1) + 1); \
          XDrawString(disp, root.win, gc, x1 + 10, ((y1) + (y2)) / 2, str, strlen(str)); \
        }
#define DO_DRAW_MODE_1(aa, bb, cc, dd) \
      if (!font) \
        font = XLoadFont(disp, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"); \
      XSetFont(disp, gc, font); \
      if (cc < 3) cc = 3; \
      if (dd < 3) dd = 3; \
      DRAW_H_ARROW(aa + ewin->border->border.left, \
                   aa + ewin->border->border.left + cc - 1, \
                   bb + ewin->border->border.top + dd - 16); \
      DRAW_H_ARROW(0, \
                   aa - 1, \
                   bb + ewin->border->border.top + (dd / 2)); \
      DRAW_H_ARROW(aa + cc + ewin->border->border.left + ewin->border->border.right, \
                   root.w - 1, \
                   bb + ewin->border->border.top + (dd / 2)); \
      DRAW_V_ARROW(bb + ewin->border->border.top, \
                   bb + ewin->border->border.top + dd - 1, \
                   aa + ewin->border->border.left + 16); \
      DRAW_V_ARROW(0, \
                   bb - 1, \
                   aa + ewin->border->border.left + (cc / 2)); \
      DRAW_V_ARROW(bb + dd + ewin->border->border.top + ewin->border->border.bottom, \
                   root.h - 1, \
                   aa + ewin->border->border.left + (cc / 2)); \
      XDrawLine(disp, root.win, gc, aa, 0, aa, root.h); \
      XDrawLine(disp, root.win, gc, \
		aa + cc + ewin->border->border.left + \
		ewin->border->border.right - 1, 0, \
		aa + cc + ewin->border->border.left + \
		ewin->border->border.right - 1, root.h); \
      XDrawLine(disp, root.win, gc, 0, bb, root.w, bb); \
      XDrawLine(disp, root.win, gc, 0, \
		bb + dd + ewin->border->border.top + \
		ewin->border->border.bottom - 1, root.w, \
		bb + dd + ewin->border->border.top + \
		ewin->border->border.bottom - 1); \
      XDrawRectangle(disp, root.win, gc, aa + ewin->border->border.left + 1, \
		     bb + ewin->border->border.top + 1, cc - 3, dd - 3);

#define DO_DRAW_MODE_2(aa, bb, cc, dd) \
      if (cc < 3) cc = 3; \
      if (dd < 3) dd = 3; \
      XDrawRectangle(disp, root.win, gc, aa, bb, \
                     cc + ewin->border->border.left + \
                     ewin->border->border.right - 1, \
                     dd + ewin->border->border.top + \
                     ewin->border->border.bottom - 1); \
      XDrawRectangle(disp, root.win, gc, aa + ewin->border->border.left + 1, \
		     bb + ewin->border->border.top + 1, cc - 3, dd - 3);

#define DO_DRAW_MODE_3(aa, bb, cc, dd) \
      XSetFillStyle(disp, gc, FillStippled); \
      XSetStipple(disp, gc, b2); \
      if ((cc + ewin->border->border.left + ewin->border->border.right > 0) && \
          (ewin->border->border.top > 0)) \
      XFillRectangle(disp, root.win, gc, aa, bb, \
                     cc + ewin->border->border.left + \
                     ewin->border->border.right, \
                     ewin->border->border.top); \
      if ((cc + ewin->border->border.left + ewin->border->border.right > 0) && \
          (ewin->border->border.bottom > 0)) \
      XFillRectangle(disp, root.win, gc, aa, bb + dd + \
                     ewin->border->border.top, \
                     cc + ewin->border->border.left + \
                     ewin->border->border.right, \
                     ewin->border->border.bottom); \
      if ((dd > 0) && (ewin->border->border.left > 0)) \
      XFillRectangle(disp, root.win, gc, aa, bb + ewin->border->border.top, \
                     ewin->border->border.left, \
                     dd); \
      if ((dd > 0) && (ewin->border->border.right > 0)) \
      XFillRectangle(disp, root.win, gc, aa + cc + ewin->border->border.left, \
                     bb + ewin->border->border.top, \
                     ewin->border->border.right, \
                     dd); \
      XSetStipple(disp, gc, b3); \
      if ((cc > 0) && (dd > 0)) \
        XFillRectangle(disp, root.win, gc, aa + ewin->border->border.left + 1, \
  		       bb + ewin->border->border.top + 1, cc - 3, dd - 3);

#define DO_DRAW_MODE_4(aa, bb, cc, dd) \
      XSetFillStyle(disp, gc, FillStippled); \
      XSetStipple(disp, gc, b2); \
      XFillRectangle(disp, root.win, gc, aa, bb, \
                     cc + ewin->border->border.left + \
                     ewin->border->border.right, \
                     dd + ewin->border->border.top + \
                     ewin->border->border.bottom);
	if (md == 1)
	  {
	     if (firstlast > 0)
	       {
		  DO_DRAW_MODE_1(x1, y1, w1, h1);
	       }
	     if ((Mode.mode != MODE_NONE)
		 && (!ewin->groups || (ewin->groups && !check_move)))
		SetCoords(ewin);
	     if (firstlast < 2)
	       {
		  DO_DRAW_MODE_1(x, y, w, h);
	       }
	  }
	else if (md == 2)
	  {
	     if (firstlast > 0)
	       {
		  DO_DRAW_MODE_2(x1, y1, w1, h1);
	       }
	     if ((Mode.mode != MODE_NONE)
		 && (!ewin->groups || (ewin->groups && !check_move)))
		SetCoords(ewin);
	     if (firstlast < 2)
	       {
		  DO_DRAW_MODE_2(x, y, w, h);
	       }
	  }
	else if (md == 3)
	  {
	     if (firstlast > 0)
	       {
		  DO_DRAW_MODE_3(x1, y1, w1, h1);
	       }
	     if ((Mode.mode != MODE_NONE)
		 && (!ewin->groups || (ewin->groups && !check_move)))
		SetCoords(ewin);
	     if (firstlast < 2)
	       {
		  DO_DRAW_MODE_3(x, y, w, h);
	       }
	  }
	else if (md == 4)
	  {
	     if (firstlast > 0)
	       {
		  DO_DRAW_MODE_4(x1, y1, w1, h1);
	       }
	     if (firstlast < 2)
		if ((Mode.mode != MODE_NONE)
		    && (!ewin->groups || (ewin->groups && !check_move)))
		   SetCoords(ewin);
	     if (firstlast < 2)
	       {
		  DO_DRAW_MODE_4(x, y, w, h);
	       }
	  }
	else if (md == 5)
	  {
	     static PixImg      *ewin_pi = NULL;
	     static PixImg      *root_pi = NULL;
	     static PixImg      *draw_pi = NULL;

	     if (firstlast == 0)
	       {
		  XGCValues           gcv2;
		  GC                  gc2;

		  if (ewin_pi)
		     EDestroyPixImg(ewin_pi);
		  if (root_pi)
		     EDestroyPixImg(root_pi);
		  if (draw_pi)
		     EDestroyPixImg(draw_pi);
		  EBlendRemoveShape(NULL, 0, 0, 0);
		  EBlendPixImg(NULL, NULL, NULL, NULL, 0, 0, 0, 0);
		  ewin_pi = NULL;
		  root_pi = NULL;
		  draw_pi = NULL;
		  root_pi = ECreatePixImg(root.win, root.w, root.h);
		  ewin_pi = ECreatePixImg(root.win, ewin->w, ewin->h);
		  draw_pi = ECreatePixImg(root.win, ewin->w, ewin->h);
		  if ((!root_pi) || (!ewin_pi) || (!draw_pi))
		    {
		       Conf.movemode = 0;
		       UngrabX();
		       DrawEwinShape(ewin, Conf.movemode, x, y, w, h,
				     firstlast);
		       goto done;
		    }
		  EFillPixmap(root.win, root_pi->pmap, x1, y1, ewin->w,
			      ewin->h);
		  gc2 = XCreateGC(disp, root_pi->pmap, 0, &gcv2);
		  XCopyArea(disp, root_pi->pmap, ewin_pi->pmap, gc2, x1, y1,
			    ewin->w, ewin->h, 0, 0);
		  XFreeGC(disp, gc2);
		  EBlendPixImg(ewin, root_pi, ewin_pi, draw_pi, x, y, ewin->w,
			       ewin->h);
	       }
	     else if (firstlast == 1)
	       {
		  int                 dx, dy, wt, ht;
		  int                 adx, ady;

		  dx = x - x1;
		  dy = y - y1;
		  if (dx < 0)
		     adx = -dx;
		  else
		     adx = dx;
		  if (dy < 0)
		     ady = -dy;
		  else
		     ady = dy;
		  wt = ewin->w;
		  ht = ewin->h;
		  if ((adx <= wt) && (ady <= ht))
		    {
		       if (dx < 0)
			  EFillPixmap(root.win, root_pi->pmap, x, y, -dx, ht);
		       else if (dx > 0)
			  EFillPixmap(root.win, root_pi->pmap, x + wt - dx, y,
				      dx, ht);
		       if (dy < 0)
			  EFillPixmap(root.win, root_pi->pmap, x, y, wt, -dy);
		       else if (dy > 0)
			  EFillPixmap(root.win, root_pi->pmap, x, y + ht - dy,
				      wt, dy);
		    }
		  else
		     EFillPixmap(root.win, root_pi->pmap, x, y, wt, ht);
		  if ((adx <= wt) && (ady <= ht))
		    {
		       EBlendPixImg(ewin, root_pi, ewin_pi, draw_pi, x, y,
				    ewin->w, ewin->h);
		       if (dx > 0)
			  EPastePixmap(root.win, root_pi->pmap, x1, y1, dx, ht);
		       else if (dx < 0)
			  EPastePixmap(root.win, root_pi->pmap, x1 + wt + dx,
				       y1, -dx, ht);
		       if (dy > 0)
			  EPastePixmap(root.win, root_pi->pmap, x1, y1, wt, dy);
		       else if (dy < 0)
			  EPastePixmap(root.win, root_pi->pmap, x1,
				       y1 + ht + dy, wt, -dy);
		    }
		  else
		    {
		       EPastePixmap(root.win, root_pi->pmap, x1, y1, wt, ht);
		       EBlendPixImg(ewin, root_pi, ewin_pi, draw_pi, x, y,
				    ewin->w, ewin->h);
		    }
		  EBlendRemoveShape(ewin, root_pi->pmap, x, y);
	       }
	     else if (firstlast == 2)
	       {
		  EPastePixmap(root.win, root_pi->pmap, x1, y1, ewin->w,
			       ewin->h);
		  if (ewin_pi)
		     EDestroyPixImg(ewin_pi);
		  if (root_pi)
		     EDestroyPixImg(root_pi);
		  if (draw_pi)
		     EDestroyPixImg(draw_pi);
		  EBlendRemoveShape(NULL, 0, 0, 0);
		  EBlendPixImg(NULL, NULL, NULL, NULL, 0, 0, 0, 0);
		  ewin_pi = NULL;
		  root_pi = NULL;
		  draw_pi = NULL;
	       }
	     else if (firstlast == 3)
	       {
		  EPastePixmap(root.win, root_pi->pmap, x, y, ewin->w, ewin->h);
		  if (root_pi)
		     EDestroyPixImg(root_pi);
		  root_pi->pmap = 0;
	       }
	     else if (firstlast == 4)
	       {
		  int                 wt, ht;

		  wt = ewin->w;
		  ht = ewin->h;
		  root_pi = ECreatePixImg(root.win, root.w, root.h);
		  EFillPixmap(root.win, root_pi->pmap, x, y, wt, ht);
		  EBlendPixImg(ewin, root_pi, ewin_pi, draw_pi, x, y, ewin->w,
			       ewin->h);
	       }
	     else if (firstlast == 5)
	       {
		  if (root_pi)
		     EDestroyPixImg(root_pi);
		  root_pi->pmap = 0;
	       }
	     if (Mode.mode != MODE_NONE)
		SetCoords(ewin);
	  }
	if (firstlast == 2)
	  {
	     /* If we're moving a group, don't do this,
	      * otherwise we have a lot of garbage onscreen */
	     if (!ewin->floating || !ewin->groups
		 || (ewin->groups && !check_move))
	       {
		  if (ewin->shaded)
		     MoveEwin(ewin, ewin->x, ewin->y);
		  else
		     MoveResizeEwin(ewin, ewin->x, ewin->y, pw, ph);
	       }
	     XFreeGC(disp, gc);
	     gc = 0;
	  }
	break;
     default:
	break;
     }

 done:
   queue_up = pq;
   EDBUG_RETURN_;
}

Imlib_Image        *
ELoadImage(char *file)
{
   Imlib_Image        *im;
   char               *f = NULL;

   EDBUG(5, "ELoadImage");

   if (!file)
      EDBUG_RETURN(NULL);

   if (file[0] == '/')
     {
	im = imlib_load_image(file);
	EDBUG_RETURN(im);
     }

   f = FindFile(file);
   if (f)
     {
	im = imlib_load_image(f);
	Efree(f);
	EDBUG_RETURN(im);
     }

   EDBUG_RETURN(NULL);
}

void
PropagateShapes(Window win)
{
   Window              rt, par, *list = NULL;
   int                 k, i, num = 0, num_rects = 0, rn = 0, ord;
   int                 x, y;
   unsigned int        ww, hh, w, h, d;
   XRectangle         *rects = NULL, *rl = NULL;
   XWindowAttributes   att;

   EDBUG(6, "PropagateShapes");
   if (queue_up)
     {
	DrawQueue          *dq;

	dq = Emalloc(sizeof(DrawQueue));
	dq->win = win;
	dq->iclass = NULL;
	dq->w = 0;
	dq->h = 0;
	dq->active = 0;
	dq->sticky = 0;
	dq->state = 0;
	dq->expose = 0;
	dq->tclass = NULL;
	dq->text = NULL;
	dq->shape_propagate = 1;
	dq->pager = NULL;
	dq->redraw_pager = NULL;
	dq->d = NULL;
	dq->di = NULL;
	dq->x = 0;
	dq->y = 0;
	AddItem(dq, "DRAW", dq->win, LIST_TYPE_DRAW);
	EDBUG_RETURN_;
     }
   EGetGeometry(disp, win, &rt, &x, &y, &w, &h, &d, &d);
   if ((w <= 0) || (h <= 0))
      EDBUG_RETURN_;

   ww = w;
   hh = h;

   XQueryTree(disp, win, &rt, &par, &list, (unsigned int *)&num);
   if (list)
     {
	/* go through all child windows and create/inset spans */
	for (i = 0; i < num; i++)
	  {
	     XGetWindowAttributes(disp, list[i], &att);
	     x = att.x;
	     y = att.y;
	     w = att.width;
	     h = att.height;
	     if ((att.class == InputOutput) && (att.map_state != IsUnmapped))
	       {
		  rl = NULL;
		  rl = EShapeGetRectangles(disp, list[i], ShapeBounding, &rn,
					   &ord);
		  if (rl)
		    {
		       num_rects += rn;
		       if (rn > 0)
			 {
			    rects =
			       Erealloc(rects, num_rects * sizeof(XRectangle));
			    /* go through all clip rects in thsi window's shape */
			    for (k = 0; k < rn; k++)
			      {
				 /* for each clip rect, add it to the rect list */
				 rects[num_rects - rn + k].x = x + rl[k].x;
				 rects[num_rects - rn + k].y = y + rl[k].y;
				 rects[num_rects - rn + k].width = rl[k].width;
				 rects[num_rects - rn + k].height =
				    rl[k].height;
			      }
			 }
		       Efree(rl);
		    }
		  else
		    {
		       num_rects++;
		       rects = Erealloc(rects, num_rects * sizeof(XRectangle));

		       rects[num_rects - 1].x = x;
		       rects[num_rects - 1].y = y;
		       rects[num_rects - 1].width = w;
		       rects[num_rects - 1].height = h;
		    }
	       }
	  }
	/* set the rects as the shape mask */
	if (rects)
	  {
	     EShapeCombineRectangles(disp, win, ShapeBounding, 0, 0, rects,
				     num_rects, ShapeSet, Unsorted);
	     Efree(rects);
	     rl = NULL;
	     rl = EShapeGetRectangles(disp, win, ShapeBounding, &rn, &ord);
	     if (rl)
	       {
		  if (rn < 1)
		     EShapeCombineMask(disp, win, ShapeBounding, 0, 0, None,
				       ShapeSet);
		  else if (rn == 1)
		    {
		       if ((rl[0].x == 0) && (rl[0].y == 0)
			   && (rl[0].width == ww) && (rl[0].height == hh))
			  EShapeCombineMask(disp, win, ShapeBounding, 0, 0,
					    None, ShapeSet);
		    }
		  Efree(rl);
	       }
	     else
		EShapeCombineMask(disp, win, ShapeBounding, 0, 0, None,
				  ShapeSet);
	  }
	XFree(list);
     }
   EDBUG_RETURN_;
}
