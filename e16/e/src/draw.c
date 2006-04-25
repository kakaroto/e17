/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "desktops.h"
#include "ewins.h"
#include "hints.h"
#include "xwin.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

static void
EFillPixmap(Window win, Pixmap pmap, int x, int y, int w, int h)
{
   XGCValues           gcv;
   GC                  gc;

   gcv.subwindow_mode = IncludeInferiors;
   gc = EXCreateGC(win, GCSubwindowMode, &gcv);
   XCopyArea(disp, win, pmap, gc, x, y, w, h, x, y);
   EXFreeGC(gc);
}

static void
EPastePixmap(Window win, Pixmap pmap, int x, int y, int w, int h)
{
   XGCValues           gcv;
   GC                  gc;

   gcv.subwindow_mode = IncludeInferiors;
   gc = EXCreateGC(win, GCSubwindowMode, &gcv);
   XCopyArea(disp, pmap, win, gc, x, y, w, h, x, y);
   EXFreeGC(gc);
}

typedef struct _PixImg
{
   XImage             *xim;
   XShmSegmentInfo    *shminfo;
   Pixmap              pmap;
   GC                  gc;
}
PixImg;

static PixImg      *
ECreatePixImg(Window win, int w, int h)
{
   XGCValues           gcv;
   PixImg             *pi;

   pi = Emalloc(sizeof(PixImg));
   if (!pi)
      return NULL;

   pi->shminfo = Emalloc(sizeof(XShmSegmentInfo));
   if (pi->shminfo)
     {
	pi->xim = XShmCreateImage(disp, VRoot.vis, VRoot.depth, ZPixmap, NULL,
				  pi->shminfo, w, h);
	if (pi->xim)
	  {
	     pi->shminfo->shmid =
		shmget(IPC_PRIVATE, pi->xim->bytes_per_line * pi->xim->height,
		       IPC_CREAT | 0666);
	     if (pi->shminfo->shmid >= 0)
	       {
		  pi->shminfo->shmaddr = pi->xim->data =
		     shmat(pi->shminfo->shmid, 0, 0);
		  if (pi->shminfo->shmaddr)
		    {
		       pi->shminfo->readOnly = False;
		       XShmAttach(disp, pi->shminfo);
		       pi->pmap =
			  XShmCreatePixmap(disp, win, pi->shminfo->shmaddr,
					   pi->shminfo, w, h, VRoot.depth);
		       if (pi->pmap)
			 {
			    gcv.subwindow_mode = IncludeInferiors;
			    pi->gc = EXCreateGC(win, GCSubwindowMode, &gcv);
			    if (pi->gc)
			       return pi;

			    EFreePixmap(pi->pmap);
			 }
		       XShmDetach(disp, pi->shminfo);
		       shmdt(pi->shminfo->shmaddr);
		    }
		  shmctl(pi->shminfo->shmid, IPC_RMID, 0);
	       }
	     XDestroyImage(pi->xim);
	  }
	Efree(pi->shminfo);
     }
   Efree(pi);
   return NULL;
}

static void
EDestroyPixImg(PixImg * pi)
{
   if (!pi)
      return;
   ESync();
   XShmDetach(disp, pi->shminfo);
   shmdt(pi->shminfo->shmaddr);
   shmctl(pi->shminfo->shmid, IPC_RMID, 0);
   XDestroyImage(pi->xim);
   Efree(pi->shminfo);
   EFreePixmap(pi->pmap);
   EXFreeGC(pi->gc);
   Efree(pi);
}

static void
EBlendRemoveShape(EWin * ewin, Pixmap pmap, int x, int y)
{
   static GC           gc = 0, gcm = 0;
   static int          rn, ord;
   static XRectangle  *rl = NULL;
   static Pixmap       mask = 0;
   Window              root = VRoot.win;
   XGCValues           gcv;
   int                 i, w, h;

   if (!ewin)
     {
	if (rl)
	   XFree(rl);
	if (gc)
	   EXFreeGC(gc);
	if (gcm)
	   EXFreeGC(gcm);
	if (mask)
	   EXFreePixmap(mask);
	mask = 0;
	gc = 0;
	gcm = 0;
	rl = NULL;
	return;
     }

   w = EoGetW(ewin);
   h = EoGetH(ewin);
   if (!rl)
     {
	rl = EShapeGetRectangles(EoGetWin(ewin), ShapeBounding, &rn, &ord);
	if (rn < 1)
	   return;
	else if (rn == 1)
	  {
	     if ((rl[0].x == 0) && (rl[0].y == 0)
		 && (rl[0].width == EoGetW(ewin))
		 && (rl[0].height == EoGetH(ewin)))
	       {
		  if (rl)
		     XFree(rl);
		  rl = NULL;
		  return;
	       }
	  }
     }
   if (!mask)
      mask = EXCreatePixmap(root, w, h, 1);
   if (!gcm)
      gcm = EXCreateGC(mask, 0, &gcv);
   if (!gc)
     {
	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(root, GCSubwindowMode, &gcv);
	XSetForeground(disp, gcm, 1);
	XFillRectangle(disp, mask, gcm, 0, 0, w, h);
	XSetForeground(disp, gcm, 0);
	for (i = 0; i < rn; i++)
	   XFillRectangle(disp, mask, gcm, rl[i].x, rl[i].y, rl[i].width,
			  rl[i].height);
	XSetClipMask(disp, gc, mask);
     }
   XSetClipOrigin(disp, gc, x, y);
   XCopyArea(disp, pmap, root, gc, x, y, w, h, x, y);
}

static void
EBlendPixImg(EWin * ewin, PixImg * s1, PixImg * s2, PixImg * dst, int x, int y,
	     int w, int h)
{
   static int          rn, ord;
   static XRectangle  *rl = NULL;
   static GC           gc = 0;
   Window              root = VRoot.win;
   XGCValues           gcv;
   int                 i, j, ox, oy;

   if (!s1)
     {
	if (gc)
	   EXFreeGC(gc);
	if (rl > (XRectangle *) 1)
	   XFree(rl);
	gc = 0;
	rl = NULL;
	return;
     }
   if (!gc)
     {
	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(root, GCSubwindowMode, &gcv);
     }
   if (!rl)
     {
	rl = EShapeGetRectangles(EoGetWin(ewin), ShapeBounding, &rn, &ord);
	if (rl)
	   XSetClipRectangles(disp, gc, x, y, rl, rn, ord);
	if (!rl)
	   rl = (XRectangle *) 1;
     }
   else
      XSetClipOrigin(disp, gc, x, y);
   ox = 0;
   oy = 0;
   if ((x >= VRoot.w) || (y >= VRoot.h))
      return;
   if (x + w > VRoot.w)
      w -= ((x + w) - VRoot.w);
   if (x < 0)
     {
	ox = -x;
	w -= ox;
	x = 0;
     }
   if (y + h > VRoot.h)
      h -= ((y + h) - VRoot.h);
   if (y < 0)
     {
	oy = -y;
	h -= oy;
	y = 0;
     }
   if ((w <= 0) || (h <= 0))
      return;

   ESync();

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
	     if (DefaultDepth(disp, VRoot.scr) != 15)
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
	XShmPutImage(disp, root, gc, dst->xim, ox, oy, x, y, w, h, False);
/*      XCopyArea(disp, dst->pmap, root, gc, ox, oy, w, h, x, y); */
     }
/* I dont believe it - you cannot do this to a shared pixmaps to the screen */
/* XCopyArea(disp, dst->pmap, root, dst->gc, x, y, w, h, x, y); */
}

#include <X11/bitmaps/flipped_gray>
#include <X11/bitmaps/gray>
#include <X11/bitmaps/gray3>

void
DrawEwinShape(EWin * ewin, int md, int x, int y, int w, int h, char firstlast)
{
   static GC           gc = 0;
   static Pixmap       b1 = 0, b2 = 0, b3 = 0;
   static Font         font = 0;
   Window              root = VRoot.win;
   int                 x1, y1, w1, h1, i, j, dx, dy;
   int                 bl, br, bt, bb;
   char                str[32];

   switch (md)
     {
     case 0:
	EwinOpMoveResize(ewin, OPSRC_USER, x, y, w, h);
	EwinShapeSet(ewin);
	CoordsShow(ewin);
	break;
     case 1:
     case 2:
     case 3:
     case 4:
     case 5:
	if (firstlast == 0)
	   EwinShapeSet(ewin);

	if (!b1)
	   b1 = XCreateBitmapFromData(disp, root, flipped_gray_bits,
				      flipped_gray_width, flipped_gray_height);
	if (!b2)
	   b2 = XCreateBitmapFromData(disp, root, gray_bits, gray_width,
				      gray_height);
	if (!b3)
	   b3 = XCreateBitmapFromData(disp, root, gray3_bits, gray3_width,
				      gray3_height);

	if ((Mode.mode == MODE_RESIZE) || (Mode.mode == MODE_RESIZE_H)
	    || (Mode.mode == MODE_RESIZE_V))
	  {
	     i = (x - ewin->shape_x) / ewin->icccm.w_inc;
	     j = (y - ewin->shape_y) / ewin->icccm.h_inc;
	     x = ewin->shape_x + (i * ewin->icccm.w_inc);
	     y = ewin->shape_y + (j * ewin->icccm.h_inc);
	  }

	dx = EoGetX(EoGetDesk(ewin));
	dy = EoGetY(EoGetDesk(ewin));
	x1 = ewin->shape_x + dx;
	y1 = ewin->shape_y + dy;

	w1 = ewin->shape_w;
	h1 = ewin->shape_h;

	ewin->shape_x = x;
	ewin->shape_y = y;
	x += dx;
	y += dy;

	if ((w != ewin->client.w) || (h != ewin->client.h))
	  {
	     if (!ewin->state.shaded)
		ICCCM_SizeMatch(ewin, w, h, &ewin->shape_w, &ewin->shape_h);
	  }

	w = ewin->shape_w;
	h = ewin->shape_h;

	EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);

	if (!gc)
	  {
	     XGCValues           gcv;

	     gcv.function = GXxor;
	     gcv.foreground = WhitePixel(disp, VRoot.scr);
	     if (gcv.foreground == 0)
		gcv.foreground = BlackPixel(disp, VRoot.scr);
	     gcv.subwindow_mode = IncludeInferiors;
	     gc = EXCreateGC(root,
			     GCFunction | GCForeground | GCSubwindowMode, &gcv);
	  }
#define DRAW_H_ARROW(x1, x2, y1) \
      if (((x2) - (x1)) >= 12) \
        { \
          XDrawLine(disp, root, gc, (x1), (y1), (x1) + 6, (y1) - 3); \
          XDrawLine(disp, root, gc, (x1), (y1), (x1) + 6, (y1) + 3); \
          XDrawLine(disp, root, gc, (x2), (y1), (x2) - 6, (y1) - 3); \
          XDrawLine(disp, root, gc, (x2), (y1), (x2) - 6, (y1) + 3); \
        } \
      if ((x2) >= (x1)) \
        { \
          XDrawLine(disp, root, gc, (x1), (y1), (x2), (y1)); \
          Esnprintf(str, sizeof(str), "%i", (x2) - (x1) + 1); \
          XDrawString(disp, root, gc, ((x1) + (x2)) / 2, (y1) - 10, str, strlen(str)); \
        }
#define DRAW_V_ARROW(y1, y2, x1) \
      if (((y2) - (y1)) >= 12) \
        { \
          XDrawLine(disp, root, gc, (x1), (y1), (x1) + 3, (y1) + 6); \
          XDrawLine(disp, root, gc, (x1), (y1), (x1) - 3, (y1) + 6); \
          XDrawLine(disp, root, gc, (x1), (y2), (x1) + 3, (y2) - 6); \
          XDrawLine(disp, root, gc, (x1), (y2), (x1) - 3, (y2) - 6); \
        } \
      if ((y2) >= (y1)) \
        { \
          XDrawLine(disp, root, gc, (x1), (y1), (x1), (y2)); \
          Esnprintf(str, sizeof(str), "%i", (y2) - (y1) + 1); \
          XDrawString(disp, root, gc, (x1) + 10, ((y1) + (y2)) / 2, str, strlen(str)); \
        }
#define DO_DRAW_MODE_1(_a, _b, _c, _d) \
      if (!font) \
        font = XLoadFont(disp, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"); \
      XSetFont(disp, gc, font); \
      if (_c < 3) _c = 3; \
      if (_d < 3) _d = 3; \
      DRAW_H_ARROW(_a + bl, _a + bl + _c - 1, _b + bt + _d - 16); \
      DRAW_H_ARROW(0, _a - 1, _b + bt + (_d / 2)); \
      DRAW_H_ARROW(_a + _c + bl + br, VRoot.w - 1, _b + bt + (_d / 2)); \
      DRAW_V_ARROW(_b + bt, _b + bt + _d - 1, _a + bl + 16); \
      DRAW_V_ARROW(0, _b - 1, _a + bl + (_c / 2)); \
      DRAW_V_ARROW(_b + _d + bt + bb, VRoot.h - 1, _a + bl + (_c / 2)); \
      XDrawLine(disp, root, gc, _a, 0, _a, VRoot.h); \
      XDrawLine(disp, root, gc, _a + _c + bl + br - 1, 0, _a + _c + bl + br - 1, VRoot.h); \
      XDrawLine(disp, root, gc, 0, _b, VRoot.w, _b); \
      XDrawLine(disp, root, gc, 0, _b + _d + bt + bb - 1, VRoot.w, _b + _d + bt + bb - 1); \
      XDrawRectangle(disp, root, gc, _a + bl + 1, _b + bt + 1, _c - 3, _d - 3);

#define DO_DRAW_MODE_2(_a, _b, _c, _d) \
      if (_c < 3) _c = 3; \
      if (_d < 3) _d = 3; \
      XDrawRectangle(disp, root, gc, _a, _b, _c + bl + br - 1, _d + bt + bb - 1); \
      XDrawRectangle(disp, root, gc, _a + bl + 1, _b + bt + 1, _c - 3, _d - 3);

#define DO_DRAW_MODE_3(_a, _b, _c, _d) \
      XSetFillStyle(disp, gc, FillStippled); \
      XSetStipple(disp, gc, b2); \
      if ((_c + bl + br > 0) && (bt > 0)) \
        XFillRectangle(disp, root, gc, _a, _b, _c + bl + br, bt); \
      if ((_c + bl + br > 0) && (bb > 0)) \
        XFillRectangle(disp, root, gc, _a, _b + _d + bt, _c + bl + br, bb); \
      if ((_d > 0) && (bl > 0)) \
        XFillRectangle(disp, root, gc, _a, _b + bt, bl, _d); \
      if ((_d > 0) && (br > 0)) \
        XFillRectangle(disp, root, gc, _a + _c + bl, _b + bt, br, _d); \
      XSetStipple(disp, gc, b3); \
      if ((_c > 0) && (_d > 0)) \
        XFillRectangle(disp, root, gc, _a + bl + 1, _b + bt + 1, _c - 3, _d - 3);

#define DO_DRAW_MODE_4(_a, _b, _c, _d) \
      XSetFillStyle(disp, gc, FillStippled); \
      XSetStipple(disp, gc, b2); \
      XFillRectangle(disp, root, gc, _a, _b, _c + bl + br, _d + bt + bb);

	if (md == 1)
	  {
	     if (firstlast > 0)
	       {
		  DO_DRAW_MODE_1(x1, y1, w1, h1);
	       }
	     CoordsShow(ewin);
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
	     CoordsShow(ewin);
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
	     CoordsShow(ewin);
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
	     CoordsShow(ewin);
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
		  root_pi = ECreatePixImg(root, VRoot.w, VRoot.h);
		  ewin_pi = ECreatePixImg(root, EoGetW(ewin), EoGetH(ewin));
		  draw_pi = ECreatePixImg(root, EoGetW(ewin), EoGetH(ewin));
		  if ((!root_pi) || (!ewin_pi) || (!draw_pi))
		    {
		       Conf.movres.mode_move = 0;
		       EUngrabServer();
		       DrawEwinShape(ewin, Conf.movres.mode_move, x, y, w, h,
				     firstlast);
		       return;
		    }
		  EFillPixmap(root, root_pi->pmap, x1, y1, EoGetW(ewin),
			      EoGetH(ewin));
		  gc2 = EXCreateGC(root_pi->pmap, 0, &gcv2);
		  XCopyArea(disp, root_pi->pmap, ewin_pi->pmap, gc2, x1, y1,
			    EoGetW(ewin), EoGetH(ewin), 0, 0);
		  EXFreeGC(gc2);
		  EBlendPixImg(ewin, root_pi, ewin_pi, draw_pi, x, y,
			       EoGetW(ewin), EoGetH(ewin));
	       }
	     else if (firstlast == 1)
	       {
		  int                 wt, ht;
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
		  wt = EoGetW(ewin);
		  ht = EoGetH(ewin);
		  if ((adx <= wt) && (ady <= ht))
		    {
		       if (dx < 0)
			  EFillPixmap(root, root_pi->pmap, x, y, -dx, ht);
		       else if (dx > 0)
			  EFillPixmap(root, root_pi->pmap, x + wt - dx, y,
				      dx, ht);
		       if (dy < 0)
			  EFillPixmap(root, root_pi->pmap, x, y, wt, -dy);
		       else if (dy > 0)
			  EFillPixmap(root, root_pi->pmap, x, y + ht - dy,
				      wt, dy);
		    }
		  else
		     EFillPixmap(root, root_pi->pmap, x, y, wt, ht);
		  if ((adx <= wt) && (ady <= ht))
		    {
		       EBlendPixImg(ewin, root_pi, ewin_pi, draw_pi, x, y,
				    EoGetW(ewin), EoGetH(ewin));
		       if (dx > 0)
			  EPastePixmap(root, root_pi->pmap, x1, y1, dx, ht);
		       else if (dx < 0)
			  EPastePixmap(root, root_pi->pmap, x1 + wt + dx,
				       y1, -dx, ht);
		       if (dy > 0)
			  EPastePixmap(root, root_pi->pmap, x1, y1, wt, dy);
		       else if (dy < 0)
			  EPastePixmap(root, root_pi->pmap, x1,
				       y1 + ht + dy, wt, -dy);
		    }
		  else
		    {
		       EPastePixmap(root, root_pi->pmap, x1, y1, wt, ht);
		       EBlendPixImg(ewin, root_pi, ewin_pi, draw_pi, x, y,
				    EoGetW(ewin), EoGetH(ewin));
		    }
		  EBlendRemoveShape(ewin, root_pi->pmap, x, y);
	       }
	     else if (firstlast == 2)
	       {
		  EPastePixmap(root, root_pi->pmap, x1, y1, EoGetW(ewin),
			       EoGetH(ewin));
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
		  EPastePixmap(root, root_pi->pmap, x, y, EoGetW(ewin),
			       EoGetH(ewin));
		  if (root_pi)
		     EDestroyPixImg(root_pi);
		  root_pi->pmap = 0;
	       }
	     else if (firstlast == 4)
	       {
		  int                 wt, ht;

		  wt = EoGetW(ewin);
		  ht = EoGetH(ewin);
		  root_pi = ECreatePixImg(root, VRoot.w, VRoot.h);
		  EFillPixmap(root, root_pi->pmap, x, y, wt, ht);
		  EBlendPixImg(ewin, root_pi, ewin_pi, draw_pi, x, y,
			       EoGetW(ewin), EoGetH(ewin));
	       }
	     else if (firstlast == 5)
	       {
		  if (root_pi)
		     EDestroyPixImg(root_pi);
		  root_pi->pmap = 0;
	       }
	     CoordsShow(ewin);
	  }

	if (firstlast == 2)
	  {
	     EXFreeGC(gc);
	     gc = 0;
	  }
	break;
     default:
	break;
     }

   if (firstlast == 0 || firstlast == 2 || firstlast == 4)
     {
	ewin->req_x = ewin->shape_x;
	ewin->req_y = ewin->shape_y;
	if (firstlast == 2)
	   CoordsHide();
     }
}
