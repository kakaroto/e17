/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "piximg.h"
#include "xwin.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

void
EFillPixmap(Window win, Pixmap pmap, int x, int y, int w, int h)
{
   XGCValues           gcv;
   GC                  gc;

   gcv.subwindow_mode = IncludeInferiors;
   gc = EXCreateGC(win, GCSubwindowMode, &gcv);
   XCopyArea(disp, win, pmap, gc, x, y, w, h, x, y);
   EXFreeGC(gc);
}

void
EPastePixmap(Window win, Pixmap pmap, int x, int y, int w, int h)
{
   XGCValues           gcv;
   GC                  gc;

   gcv.subwindow_mode = IncludeInferiors;
   gc = EXCreateGC(win, GCSubwindowMode, &gcv);
   XCopyArea(disp, pmap, win, gc, x, y, w, h, x, y);
   EXFreeGC(gc);
}

PixImg             *
ECreatePixImg(Window win, int w, int h)
{
   XGCValues           gcv;
   PixImg             *pi;

   pi = EMALLOC(PixImg, 1);
   if (!pi)
      return NULL;

   pi->shminfo = EMALLOC(XShmSegmentInfo, 1);
   if (pi->shminfo)
     {
	pi->xim = XShmCreateImage(disp, WinGetVisual(VROOT),
				  WinGetDepth(VROOT), ZPixmap, NULL,
				  pi->shminfo, w, h);
	if (pi->xim)
	  {
	     pi->shminfo->shmid =
		shmget(IPC_PRIVATE, pi->xim->bytes_per_line * pi->xim->height,
		       IPC_CREAT | 0666);
	     if (pi->shminfo->shmid >= 0)
	       {
		  pi->shminfo->shmaddr = pi->xim->data =
		     (char *)shmat(pi->shminfo->shmid, 0, 0);
		  if (pi->shminfo->shmaddr != (void *)-1)
		    {
		       pi->shminfo->readOnly = False;
		       Dpy.last_error_code = 0;
		       XShmAttach(disp, pi->shminfo);
		       ESync(0);
		       if (Dpy.last_error_code == 0)
			 {
			    pi->pmap =
			       XShmCreatePixmap(disp, win, pi->shminfo->shmaddr,
						pi->shminfo, w, h,
						WinGetDepth(VROOT));
			    if (pi->pmap)
			      {
				 gcv.subwindow_mode = IncludeInferiors;
				 pi->gc =
				    EXCreateGC(win, GCSubwindowMode, &gcv);
				 if (pi->gc)
				    return pi;

				 EFreePixmap(pi->pmap);
			      }
			    XShmDetach(disp, pi->shminfo);
			 }
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

void
EDestroyPixImg(PixImg * pi)
{
   if (!pi)
      return;
   ESync(0);
   XShmDetach(disp, pi->shminfo);
   shmdt(pi->shminfo->shmaddr);
   shmctl(pi->shminfo->shmid, IPC_RMID, 0);
   XDestroyImage(pi->xim);
   Efree(pi->shminfo);
   EFreePixmap(pi->pmap);
   EXFreeGC(pi->gc);
   Efree(pi);
}

void
EBlendRemoveShape(Win win, Pixmap pmap, int x, int y)
{
   static GC           gc = 0;
   Window              root = WinGetXwin(VROOT);
   int                 w, h;

   if (!win)
     {
	if (gc)
	   EXFreeGC(gc);
	gc = 0;
	return;
     }

   if (win->num_rect <= 0)
      return;

   w = WinGetW(win);
   h = WinGetH(win);

   if (!gc)
     {
	XGCValues           gcv;
	GC                  gcm;
	Pixmap              mask;
	XRectangle         *rl;
	int                 i;

	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(root, GCSubwindowMode, &gcv);

	mask = XCreatePixmap(disp, root, w, h, 1);
	gcm = EXCreateGC(mask, 0, NULL);
	XSetForeground(disp, gcm, 1);
	XFillRectangle(disp, mask, gcm, 0, 0, w, h);
	XSetForeground(disp, gcm, 0);
	rl = win->rects;
	for (i = 0; i < win->num_rect; i++)
	   XFillRectangle(disp, mask, gcm, rl[i].x, rl[i].y, rl[i].width,
			  rl[i].height);
	XSetClipMask(disp, gc, mask);
	EXFreeGC(gcm);
	XFreePixmap(disp, mask);
     }

   XSetClipOrigin(disp, gc, x, y);
   XCopyArea(disp, pmap, root, gc, x, y, w, h, x, y);
}

void
EBlendPixImg(Win win, PixImg * s1, PixImg * s2, PixImg * dst, int x, int y,
	     int w, int h)
{
   static GC           gc = 0;
   Window              root = WinGetXwin(VROOT);
   int                 i, j, ox, oy;

   if (!win)
     {
	if (gc)
	   EXFreeGC(gc);
	gc = 0;
	return;
     }

   if (!gc)
     {
	XGCValues           gcv;

	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(root, GCSubwindowMode, &gcv);
	if (win->rects)
	   XSetClipRectangles(disp, gc, x, y, win->rects, win->num_rect,
			      win->ord);
     }
   else
      XSetClipOrigin(disp, gc, x, y);

   ox = 0;
   oy = 0;
   if ((x >= WinGetW(VROOT)) || (y >= WinGetH(VROOT)))
      return;
   if (x + w > WinGetW(VROOT))
      w -= ((x + w) - WinGetW(VROOT));
   if (x < 0)
     {
	ox = -x;
	w -= ox;
	x = 0;
     }
   if (y + h > WinGetH(VROOT))
      h -= ((y + h) - WinGetH(VROOT));
   if (y < 0)
     {
	oy = -y;
	h -= oy;
	y = 0;
     }
   if ((w <= 0) || (h <= 0))
      return;

   ESync(0);

   switch (dst->xim->bits_per_pixel)
     {
     case 32:
	for (j = 0; j < h; j++)
	  {
	     unsigned int       *ptr1, *ptr2, *ptr3;

	     ptr1 =
		(unsigned int *)(s1->xim->data +
				 (x * ((s1->xim->bits_per_pixel) >> 3)) +
				 ((j + y) * s1->xim->bytes_per_line));
	     ptr2 =
		(unsigned int *)(s2->xim->data +
				 (ox * ((s2->xim->bits_per_pixel) >> 3)) +
				 ((j + oy) * s2->xim->bytes_per_line));
	     ptr3 =
		(unsigned int *)(dst->xim->data +
				 (ox * ((dst->xim->bits_per_pixel) >> 3)) +
				 ((j + oy) * dst->xim->bytes_per_line));
	     for (i = 0; i < w; i++)
	       {
		  unsigned int        p1, p2;

		  p1 = *ptr1++;
		  p2 = *ptr2++;
		  *ptr3++ = ((p1 >> 1) & 0x7f7f7f7f) +
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
	if (WinGetDepth(VROOT) != 15)
	  {
	     for (j = 0; j < h; j++)
	       {
		  unsigned int       *ptr1, *ptr2, *ptr3;

		  ptr1 =
		     (unsigned int *)(s1->xim->data +
				      (x * ((s1->xim->bits_per_pixel) >> 3)) +
				      ((j + y) * s1->xim->bytes_per_line));
		  ptr2 =
		     (unsigned int *)(s2->xim->data +
				      (ox * ((s2->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * s2->xim->bytes_per_line));
		  ptr3 =
		     (unsigned int *)(dst->xim->data +
				      (ox * ((dst->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * dst->xim->bytes_per_line));
		  if (!(w & 0x1))
		    {
		       for (i = 0; i < w; i += 2)
			 {
			    unsigned int        p1, p2;

			    p1 = *ptr1++;
			    p2 = *ptr2++;
			    *ptr3++ =
			       ((p1 >> 1) &
				((0x78 << 8) | (0x7c << 3) | (0x78 >> 3) |
				 (0x78 << 24) | (0x7c << 19) | (0x78 << 13)))
			       +
			       ((p2 >> 1) &
				((0x78 << 8) | (0x7c << 3) | (0x78 >> 3) |
				 (0x78 << 24) | (0x7c << 19) | (0x78 << 13)))
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
				((0x78 << 8) | (0x7c << 3) | (0x78 >> 3) |
				 (0x78 << 24) | (0x7c << 19) | (0x78 << 13)))
			       +
			       ((p2 >> 1) &
				((0x78 << 8) | (0x7c << 3) | (0x78 >> 3) |
				 (0x78 << 24) | (0x7c << 19) | (0x78 << 13)))
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
			     + (pp1 & pp2 & ((0x1 << 11) | (0x1 << 5) | (0x1)));
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
				      (x * (s1->xim->bits_per_pixel >> 3)) +
				      ((j + y) * s1->xim->bytes_per_line));
		  ptr2 =
		     (unsigned int *)(s2->xim->data +
				      (ox * ((s2->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * s2->xim->bytes_per_line));
		  ptr3 =
		     (unsigned int *)(dst->xim->data +
				      (ox * ((dst->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * dst->xim->bytes_per_line));
		  if (!(w & 0x1))
		    {
		       for (i = 0; i < w; i += 2)
			 {
			    unsigned int        p1, p2;

			    p1 = *ptr1++;
			    p2 = *ptr2++;
			    *ptr3++ =
			       ((p1 >> 1) &
				((0x78 << 7) | (0x78 << 2) | (0x78 >> 3) |
				 (0x78 << 23) | (0x78 << 18) | (0x78 << 13)))
			       +
			       ((p2 >> 1) &
				((0x78 << 7) | (0x78 << 2) | (0x78 >> 3) |
				 (0x78 << 23) | (0x78 << 18) | (0x78 << 13)))
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
				((0x78 << 7) | (0x78 << 2) | (0x78 >> 3) |
				 (0x78 << 23) | (0x78 << 18) | (0x78 << 13)))
			       +
			       ((p2 >> 1) &
				((0x78 << 7) | (0x78 << 2) | (0x78 >> 3) |
				 (0x78 << 23) | (0x78 << 18) | (0x78 << 13)))
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
			     + (pp1 & pp2 & ((0x1 << 10) | (0x1 << 5) | (0x1)));
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
/* I dont believe it - you cannot do this to a shared pixmaps to the screen */
/* XCopyArea(disp, dst->pmap, root, dst->gc, x, y, w, h, x, y); */
}
