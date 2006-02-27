/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#ifdef USE_ECORE_X
#include <Ecore_X.h>
#endif
#include "xwin.h"
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#define DEBUG_XWIN 0

typedef struct
{
   EventCallbackFunc  *func;
   void               *prm;
} EventCallbackItem;

typedef struct
{
   int                 num;
   EventCallbackItem  *lst;
} EventCallbackList;

typedef struct _exid
{
   struct _exid       *next;
   struct _exid       *prev;
   EventCallbackList   cbl;
   Window              parent;
   Window              win;
   int                 x, y, w, h;
   char                mapped;
   char                in_use;
   signed char         do_del;
   char                attached;
   int                 num_rect;
   int                 ord;
   XRectangle         *rects;
   int                 depth;
   Pixmap              bgpmap;
   int                 bgcol;
} EXID;

static XContext     xid_context = 0;

static EXID        *xid_first = NULL;
static EXID        *xid_last = NULL;

static EXID        *
EXidCreate(void)
{
   EXID               *xid;

   xid = Ecalloc(1, sizeof(EXID));
   xid->bgcol = -1;

   return xid;
}

static void
EXidDestroy(EXID * xid)
{
#if DEBUG_XWIN
   Eprintf("EXidDestroy: %p %#lx\n", xid, xid->win);
#endif
   if (xid->rects)
      XFree(xid->rects);
   if (xid->cbl.lst)
      Efree(xid->cbl.lst);
   Efree(xid);
}

static void
EXidAdd(EXID * xid)
{
#if DEBUG_XWIN
   Eprintf("EXidAdd: %p %#lx\n", xid, xid->win);
#endif
   if (!xid_context)
      xid_context = XUniqueContext();

   XSaveContext(disp, xid->win, xid_context, (XPointer) xid);

   if (!xid_first)
     {
	xid_first = xid_last = xid;
     }
   else
     {
	xid->prev = xid_last;
	xid_last->next = xid;
	xid_last = xid;
     }
}

static void
EXidDel(EXID * xid)
{
#if DEBUG_XWIN
   Eprintf("EXidDel: %p %#lx\n", xid, xid->win);
#endif
   if (xid == xid_first)
     {
	if (xid == xid_last)
	  {
	     xid_first = xid_last = NULL;
	  }
	else
	  {
	     xid_first = xid->next;
	     xid->next->prev = NULL;
	  }
     }
   else if (xid == xid_last)
     {
	xid_last = xid->prev;
	xid->prev->next = NULL;
     }
   else
     {
	xid->prev->next = xid->next;
	xid->next->prev = xid->prev;
     }

   XDeleteContext(disp, xid->win, xid_context);
   if (xid->in_use)
      xid->do_del = 1;
   else
      EXidDestroy(xid);
}

static EXID        *
EXidFind(Window win)
{
   EXID               *xid;
   XPointer            xp;

   if (!xid_context)
      return NULL;

   xp = NULL;
   if (XFindContext(disp, win, xid_context, &xp) == XCNOENT)
      xp = NULL;
   xid = (EXID *) xp;

   return xid;
}

static EXID        *
EXidSet(Window win, Window parent, int x, int y, int w, int h, int depth)
{
   EXID               *xid;

   xid = EXidCreate();
   xid->parent = parent;
   xid->win = win;
   xid->x = x;
   xid->y = y;
   xid->w = w;
   xid->h = h;
   xid->depth = depth;
#if DEBUG_XWIN
   Eprintf("EXidSet: %#lx\n", xid->win);
#endif
   EXidAdd(xid);

   return xid;
}

void
EventCallbackRegister(Window win, int type __UNUSED__, EventCallbackFunc * func,
		      void *prm)
{
   EXID               *xid;
   EventCallbackItem  *eci;

   xid = EXidFind(win);
   if (!xid)
      ERegisterWindow(win);
   xid = EXidFind(win);
#if 0
   Eprintf("EventCallbackRegister: %p %#lx\n", xid, win);
#endif
   if (!xid)
     {
	Eprintf("EventCallbackRegister win=%#lx ???\n", win);
	return;
     }

   xid->cbl.num++;
   xid->cbl.lst =
      Erealloc(xid->cbl.lst, xid->cbl.num * sizeof(EventCallbackItem));
   eci = xid->cbl.lst + xid->cbl.num - 1;
   eci->func = func;
   eci->prm = prm;
}

void
EventCallbackUnregister(Window win, int type __UNUSED__,
			EventCallbackFunc * func, void *prm)
{
   EXID               *xid;
   EventCallbackList  *ecl;
   EventCallbackItem  *eci;
   int                 i;

   xid = EXidFind(win);
#if 0
   Eprintf("EventCallbackUnregister: %p %#lx\n", xid, win);
#endif
   if (xid == NULL)
      return;

   ecl = &xid->cbl;
   eci = ecl->lst;
   for (i = 0; i < ecl->num; i++, eci++)
      if (eci->func == func && eci->prm == prm)
	{
	   ecl->num--;
	   if (ecl->num)
	     {
		for (; i < ecl->num; i++, eci++)
		   *eci = *(eci + 1);
		xid->cbl.lst =
		   Erealloc(xid->cbl.lst, ecl->num * sizeof(EventCallbackItem));
	     }
	   else
	     {
		Efree(xid->cbl.lst);
		xid->cbl.lst = NULL;
	     }
	   return;
	}
}

void
EventCallbacksProcess(XEvent * ev)
{
   EXID               *xid;
   EventCallbackList  *ecl;
   EventCallbackItem  *eci;
   int                 i;

   xid = EXidFind(ev->xany.window);
   if (xid == NULL)
      return;

   xid->in_use = 1;
   ecl = &xid->cbl;
   eci = ecl->lst;
   for (i = 0; i < ecl->num; i++, eci++)
     {
	if (EventDebug(EDBUG_TYPE_DISPATCH))
	   Eprintf("EventDispatch: type=%d win=%#lx func=%p prm=%p\n",
		   ev->type, ev->xany.window, eci->func, eci->prm);
	eci->func(ev, eci->prm);
	if (xid->do_del)
	  {
	     EXidDestroy(xid);
	     return;
	  }
     }
   xid->in_use = 0;
}

Window
ECreateWindow(Window parent, int x, int y, int w, int h, int saveunder)
{
   Window              win;
   XSetWindowAttributes attr;

   attr.backing_store = NotUseful;
   attr.override_redirect = False;
   attr.colormap = VRoot.cmap;
   attr.border_pixel = 0;
/*   attr.background_pixel = 0; */
   attr.background_pixmap = None;
   if ((saveunder == 1) && (Conf.save_under))
      attr.save_under = True;
   else if (saveunder == 2)
      attr.save_under = True;
   else
      attr.save_under = False;
   win = XCreateWindow(disp, parent, x, y, w, h, 0,
		       VRoot.depth, InputOutput, VRoot.vis,
		       CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		       CWColormap | CWBackPixmap | CWBorderPixel, &attr);
   EXidSet(win, parent, x, y, w, h, VRoot.depth);

   return win;
}

/* Creates a window, but takes the visual, depth and the colormap from c_attr. */
Window
ECreateVisualWindow(Window parent, int x, int y, int w, int h, int saveunder,
		    XWindowAttributes * c_attr)
{
   Window              win;
   XSetWindowAttributes attr;

   attr.backing_store = NotUseful;
   attr.override_redirect = False;
   attr.border_pixel = 0;
   attr.colormap = c_attr->colormap;
/*   attr.background_pixel = 0; */
   attr.background_pixmap = None;
   if ((saveunder == 1) && (Conf.save_under))
      attr.save_under = True;
   else if (saveunder == 2)
      attr.save_under = True;
   else
      attr.save_under = False;
   win = XCreateWindow(disp, parent, x, y, w, h, 0,
		       c_attr->depth, InputOutput, c_attr->visual,
		       CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		       CWColormap | CWBackPixmap | CWBorderPixel, &attr);
   EXidSet(win, parent, x, y, w, h, VRoot.depth);

   return win;
}

Window
ECreateEventWindow(Window parent, int x, int y, int w, int h)
{
   Window              win;
   XSetWindowAttributes attr;

   attr.override_redirect = False;
   win = XCreateWindow(disp, parent, x, y, w, h, 0, 0, InputOnly,
		       CopyFromParent, CWOverrideRedirect, &attr);
   EXidSet(win, parent, x, y, w, h, VRoot.depth);

   return win;
}

#if 0				/* Not used */
/*
 * create a window which will accept the keyboard focus when no other 
 * windows have it
 */
Window
ECreateFocusWindow(Window parent, int x, int y, int w, int h)
{
   Window              win;
   XSetWindowAttributes attr;

   attr.backing_store = NotUseful;
   attr.override_redirect = False;
   attr.colormap = VRoot.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = False;
   attr.event_mask = KeyPressMask | FocusChangeMask;

   win = XCreateWindow(disp, parent, x, y, w, h, 0, 0, InputOnly,
		       CopyFromParent,
		       CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		       CWColormap | CWBackPixel | CWBorderPixel | CWEventMask,
		       &attr);

   XSetWindowBackground(disp, win, 0);
   XMapWindow(disp, win);
   XSetInputFocus(disp, win, RevertToParent, CurrentTime);

   return win;
}
#endif

void
EMoveWindow(Window win, int x, int y)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
#if 0
	Eprintf("EMoveWindow: %p %#lx: %d,%d %dx%d -> %d,%d\n",
		xid, xid->win, xid->x, xid->y, xid->w, xid->h, x, y);
#endif
	if ((x != xid->x) || (y != xid->y))
	  {
	     xid->x = x;
	     xid->y = y;
	     XMoveWindow(disp, win, x, y);
	  }
     }
   else
      XMoveWindow(disp, win, x, y);
}

void
EResizeWindow(Window win, int w, int h)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
	if ((w != xid->w) || (h != xid->h))
	  {
	     xid->w = w;
	     xid->h = h;
	     XResizeWindow(disp, win, w, h);
	  }
     }
   else
      XResizeWindow(disp, win, w, h);
}

void
EMoveResizeWindow(Window win, int x, int y, int w, int h)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
#if 0
	Eprintf("EMoveResizeWindow: %p %#lx: %d,%d %dx%d -> %d,%d %dx%d\n",
		xid, xid->win, xid->x, xid->y, xid->w, xid->h, x, y, w, h);
#endif
	if ((w != xid->w) || (h != xid->h) || (x != xid->x) || (y != xid->y))
	  {
	     xid->x = x;
	     xid->y = y;
	     xid->w = w;
	     xid->h = h;
	     XMoveResizeWindow(disp, win, x, y, w, h);
	  }
     }
   else
      XMoveResizeWindow(disp, win, x, y, w, h);
}

static int
ExDelTree(EXID * xid)
{
   Window              win;
   int                 nsub;

   xid->do_del = -1;

   nsub = 0;
   win = xid->win;
   for (xid = xid_first; xid; xid = xid->next)
     {
	if (xid->parent != win)
	   continue;
	ExDelTree(xid);
	nsub++;
     }

   return nsub;
}

static void
ExDestroyWindow(EXID * xid)
{
   EXID               *next;
   int                 nsub;

#if DEBUG_XWIN
   Eprintf("ExDestroyWindow: %p %#lx\n", xid, xid->win);
#endif
   if (xid->parent != None)
      XDestroyWindow(disp, xid->win);

   /* Mark the ones to be deleted */
   nsub = ExDelTree(xid);
   if (nsub == 0)
     {
	/* No children */
	EXidDel(xid);
	return;
     }

   /* Delete entire tree */
   for (xid = xid_first; xid; xid = next)
     {
	next = xid->next;
	if (xid->do_del < 0)
	   EXidDel(xid);
     }
}

void
EDestroyWindow(Window win)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
      ExDestroyWindow(xid);
   else
      XDestroyWindow(disp, win);
}

void
EWindowSync(Window win)
{
   EXID               *xid;
   Window              rr;
   int                 x, y;
   unsigned int        w, h, bw, depth;

   xid = EXidFind(win);
   if (!xid)
      return;

   XGetGeometry(disp, win, &rr, &x, &y, &w, &h, &bw, &depth);
#if 0
   Eprintf("EWindowSync: %p %#lx: %d,%d %dx%d -> %d,%d %dx%d\n",
	   xid, xid->win, xid->x, xid->y, xid->w, xid->h, x, y, w, h);
#endif
   xid->x = x;
   xid->y = y;
   xid->w = w;
   xid->h = h;
   xid->depth = depth;
}

void
EWindowSetMapped(Window win, int mapped)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (!xid)
      return;

   xid->mapped = mapped;
}

Window
EWindowGetParent(Window win)
{
   EXID               *xid;
   Window              parent, rt;
   Window             *pch = NULL;
   unsigned int        nch = 0;

   parent = None;
   if (!XQueryTree(disp, win, &rt, &parent, &pch, &nch))
      parent = None;
   else if (pch)
      XFree(pch);

   xid = EXidFind(win);
   if (xid)
      xid->parent = parent;

   return parent;
}

void
ERegisterWindow(Window win)
{
   EXID               *xid;
   Window              rr;
   int                 x, y;
   unsigned int        w, h, bw, depth;

   xid = EXidFind(win);
   if (xid)
      return;

   XGetGeometry(disp, win, &rr, &x, &y, &w, &h, &bw, &depth);
#if 0
   Eprintf("ERegisterWindow %#lx %d+%d %dx%d\n", win, x, y, w, h);
#endif
   xid = EXidSet(win, None, x, y, w, h, depth);
   xid->attached = 1;
}

void
EUnregisterWindow(Window win)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
      EXidDel(xid);
}

void
EMapWindow(Window win)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
	if (!xid->mapped)
	  {
	     xid->mapped = 1;
	     XMapWindow(disp, win);
	  }
     }
   else
      XMapWindow(disp, win);
}

void
EUnmapWindow(Window win)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
	if (xid->mapped)
	  {
	     xid->mapped = 0;
	     XUnmapWindow(disp, win);
	  }
     }
   else
      XUnmapWindow(disp, win);
}

void
EReparentWindow(Window win, Window parent, int x, int y)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
#if 0
	Eprintf
	   ("EReparentWindow: %p %#lx: %d %#lx->%#lx %d,%d %dx%d -> %d,%d\n",
	    xid, xid->win, xid->mapped, xid->parent, parent, xid->x, xid->y,
	    xid->w, xid->h, x, y);
#endif
	if (parent == xid->parent)
	  {
	     if ((x != xid->x) || (y != xid->y))
	       {
		  xid->x = x;
		  xid->y = y;
		  XMoveWindow(disp, win, x, y);
	       }
	  }
	else
	  {
	     xid->parent = parent;
	     xid->x = x;
	     xid->y = y;
	     XReparentWindow(disp, win, parent, x, y);
	  }
     }
   else
      XReparentWindow(disp, win, parent, x, y);
}

void
EMapRaised(Window win)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
	if (xid->mapped)
	   XRaiseWindow(disp, win);
	else
	  {
	     xid->mapped = 1;
	     XMapRaised(disp, win);
	  }
     }
   else
      XMapRaised(disp, win);
}

int
EGetGeometry(Window win, Window * root_return, int *x, int *y,
	     int *w, int *h, int *bw, int *depth)
{
   int                 ok;
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
	if (x)
	   *x = xid->x;
	if (y)
	   *y = xid->y;
	if (w)
	   *w = xid->w;
	if (h)
	   *h = xid->h;
	if (bw)
	   *bw = 0;
	if (depth)
	   *depth = xid->depth;
	if (root_return)
	   *root_return = VRoot.win;
	ok = 1;
     }
   else
     {
	Window              rr;
	int                 xx, yy;
	unsigned int        ww, hh, bb, dd;

	ok = XGetGeometry(disp, win, &rr, &xx, &yy, &ww, &hh, &bb, &dd);
	if (ok)
	  {
	     if (root_return)
		*root_return = rr;
	     if (x)
		*x = xx;
	     if (y)
		*y = yy;
	     if (w)
		*w = ww;
	     if (h)
		*h = hh;
	     if (bw)
		*bw = bb;
	     if (depth)
		*depth = dd;
	  }
     }
#if 0				/* Debug */
   if (!ok)
      Eprintf("EGetGeometry win=%#x, error %d\n", (unsigned)win, ok);
#endif
   return ok;
}

void
EConfigureWindow(Window win, unsigned int mask, XWindowChanges * wc)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
	char                doit = 0;

	if ((mask & CWX) && (wc->x != xid->x))
	  {
	     xid->x = wc->x;
	     doit = 1;
	  }
	if ((mask & CWY) && (wc->y != xid->y))
	  {
	     xid->y = wc->y;
	     doit = 1;
	  }
	if ((mask & CWWidth) && (wc->width != xid->w))
	  {
	     xid->w = wc->width;
	     doit = 1;
	  }
	if ((mask & CWHeight) && (wc->height != xid->h))
	  {
	     xid->h = wc->height;
	     doit = 1;
	  }
	if ((doit) || (mask & (CWBorderWidth | CWSibling | CWStackMode)))
	   XConfigureWindow(disp, win, mask, wc);
     }
   else
     {
	XConfigureWindow(disp, win, mask, wc);
     }
}

void
ESetWindowBackgroundPixmap(Window win, Pixmap pmap)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
	xid->bgpmap = pmap;
	xid->bgcol = 0xffffffff;	/* Hmmm.. */
	XSetWindowBackgroundPixmap(disp, win, pmap);
     }
   else
      XSetWindowBackgroundPixmap(disp, win, pmap);
}

void
ESetWindowBackground(Window win, int col)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
     {
	if (xid->bgpmap)
	  {
	     xid->bgpmap = 0;
	     xid->bgcol = col;
	     XSetWindowBackground(disp, win, col);
	  }
	else if (xid->bgcol != col)
	  {
	     xid->bgcol = col;
	     XSetWindowBackground(disp, win, col);
	  }
     }
   else
      XSetWindowBackground(disp, win, col);
}

int
ETranslateCoordinates(Window src_w, Window dst_w,
		      int src_x, int src_y,
		      int *dest_x_return,
		      int *dest_y_return, Window * child_return)
{
   Window              child;

   if (!child_return)
      child_return = &child;

   return XTranslateCoordinates(disp, src_w, dst_w, src_x, src_y,
				dest_x_return, dest_y_return, child_return);
}

void
EWarpPointer(Window win, int x, int y)
{
   XWarpPointer(disp, None, win, 0, 0, 0, 0, x, y);
}

Bool
EQueryPointer(Window win, int *px, int *py, Window * pchild,
	      unsigned int *pmask)
{
   Window              root, child;
   int                 root_x, root_y;
   unsigned int        mask;

   if (win == None)
      win = VRoot.win;
   if (!px)
      px = &root_x;
   if (!py)
      py = &root_y;
   if (!pchild)
      pchild = &child;
   if (!pmask)
      pmask = &mask;

   return XQueryPointer(disp, win, &root, pchild, &root_x, &root_y, px, py,
			pmask);
}

void
ESelectInputAdd(Window win, long mask)
{
   XWindowAttributes   xwa;

   XGetWindowAttributes(disp, win, &xwa);
   xwa.your_event_mask |= mask;
   XSelectInput(disp, win, xwa.your_event_mask);
}

int
EDrawableCheck(Drawable draw, int grab)
{
   int                 ok;

   if (draw == None)
      return 0;

   if (grab)
      EGrabServer();

   ok = EGetGeometry(draw, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

   if (grab && !ok)
      EUngrabServer();

   return ok;
}

#define DEBUG_SHAPE_OPS 0
#define DEBUG_SHAPE_PROPAGATE 0

#if DEBUG_SHAPE_OPS
static void
EShapeShow(const char *txt, XRectangle * pr, int nr)
{
   int                 i;

   Eprintf("%s nr=%d\n", txt, nr);
   for (i = 0; i < nr; i++)
      Eprintf(" %d - %4d,%4d %4dx%4d\n", i,
	      pr[i].x, pr[i].y, pr[i].width, pr[i].height);
}
#endif

static void
ExShapeUpdate(EXID * xid)
{
   if (xid->rects)
     {
	XFree(xid->rects);
	xid->num_rect = 0;
     }

   xid->rects =
      XShapeGetRectangles(disp, xid->win, ShapeBounding, &(xid->num_rect),
			  &(xid->ord));
   if (xid->rects)
     {
	if (xid->num_rect == 1)
	  {
	     if ((xid->rects[0].x == 0) && (xid->rects[0].y == 0)
		 && (xid->rects[0].width == xid->w)
		 && (xid->rects[0].height == xid->h))
	       {
		  xid->num_rect = 0;
		  XFree(xid->rects);
		  xid->rects = NULL;
		  XShapeCombineMask(disp, xid->win, ShapeBounding, 0, 0,
				    None, ShapeSet);
	       }
	  }
	else if (xid->num_rect > 4096)
	  {
	     Eprintf("*** ExShapeUpdate: nrect=%d - Not likely, ignoring.\n",
		     xid->num_rect);
	     XShapeCombineMask(disp, xid->win, ShapeBounding, 0, 0, None,
			       ShapeSet);
	     xid->num_rect = 0;
	     XFree(xid->rects);
	     xid->rects = NULL;
	  }
     }
   else
     {
	xid->num_rect = -1;
     }
#if DEBUG_SHAPE_OPS
   EShapeShow("ExShapeUpdate", xid->rects, xid->num_rect);
#endif
}

static void
ExShapeCombineMask(EXID * xid, int dest, int x, int y, Pixmap pmap, int op)
{
   char                wasshaped = 0;

   if (xid->rects)
     {
	xid->num_rect = 0;
	XFree(xid->rects);
	xid->rects = NULL;
	wasshaped = 1;
     }
#if DEBUG_SHAPE_OPS
   Eprintf("ExShapeCombineMask %#lx %d,%d %dx%d mask=%#lx wassh=%d\n",
	   xid->win, xid->x, xid->y, xid->w, xid->h, pmap, wasshaped);
#endif
   if (pmap)
     {
	XShapeCombineMask(disp, xid->win, dest, x, y, pmap, op);
	ExShapeUpdate(xid);
     }
   else if (wasshaped)
      XShapeCombineMask(disp, xid->win, dest, x, y, pmap, op);
}

void
EShapeCombineMaskTiled(Window win, int dest, int x, int y,
		       Pixmap pmap, int op, int w, int h)
{
   XGCValues           gcv;
   GC                  gc;
   Window              tm;

   gcv.fill_style = FillTiled;
   gcv.tile = pmap;
   gcv.ts_x_origin = 0;
   gcv.ts_y_origin = 0;
   tm = ECreatePixmap(win, w, h, 1);
   gc = ECreateGC(tm, GCFillStyle | GCTile |
		  GCTileStipXOrigin | GCTileStipYOrigin, &gcv);
   XFillRectangle(disp, tm, gc, 0, 0, w, h);
   EFreeGC(gc);
   EShapeCombineMask(win, dest, x, y, tm, op);
   EFreePixmap(tm);
}

static void
ExShapeCombineRectangles(EXID * xid, int dest, int x, int y,
			 XRectangle * rect, int n_rects, int op, int ordering)
{
#if DEBUG_SHAPE_OPS
   Eprintf("ExShapeCombineRectangles %#lx %d\n", xid->win, n_rects);
#endif

   if (n_rects == 1 && op == ShapeSet)
     {
	if ((rect[0].x == 0) && (rect[0].y == 0) &&
	    (rect[0].width == xid->w) && (rect[0].height == xid->h))
	  {
	     xid->num_rect = 0;
	     XFree(xid->rects);
	     xid->rects = NULL;
	     XShapeCombineMask(disp, xid->win, dest, x, y, None, op);
	     return;
	  }
     }
   XShapeCombineRectangles(disp, xid->win, dest, x, y, rect, n_rects, op,
			   ordering);
   if (n_rects > 1)
     {
	/* Limit shape to window extents */
	XRectangle          r;

	r.x = r.y = 0;
	r.width = xid->w;
	r.height = xid->h;
	XShapeCombineRectangles(disp, xid->win, ShapeBounding, 0, 0, &r,
				1, ShapeIntersect, Unsorted);
     }
   ExShapeUpdate(xid);
}

static void
ExShapeCombineShape(EXID * xdst, int dest, int x, int y,
		    EXID * xsrc, int src_kind, int op)
{
   XShapeCombineShape(disp, xdst->win, dest, x, y, xsrc->win, src_kind, op);
   ExShapeUpdate(xdst);
}

XRectangle         *
EShapeGetRectangles(Window win, int dest, int *rn, int *ord)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid && !xid->attached)
     {
	XRectangle         *r;

#if DEBUG_SHAPE_OPS
	Eprintf("EShapeGetRectangles-A %#lx nr=%d\n", win, xid->num_rect);
#endif
	*rn = xid->num_rect;
	*ord = xid->ord;
	if (xid->num_rect > 0)
	  {
	     r = Emalloc(sizeof(XRectangle) * xid->num_rect);
	     if (!r)
		return NULL;
	     memcpy(r, xid->rects, sizeof(XRectangle) * xid->num_rect);
	     return r;
	  }
     }
   else
     {
	XRectangle         *r, *rr;

#if DEBUG_SHAPE_OPS
	Eprintf("EShapeGetRectangles-B %#lx nr=%d\n", win, xid->num_rect);
#endif
	r = XShapeGetRectangles(disp, win, dest, rn, ord);
	if (r)
	  {
	     rr = Emalloc(sizeof(XRectangle) * *rn);
	     if (!rr)
		return NULL;
	     memcpy(rr, r, sizeof(XRectangle) * *rn);
	     XFree(r);
	     return rr;
	  }
     }
   return NULL;
}

static int
ExShapeCopy(EXID * xdst, EXID * xsrc)
{
   XRectangle         *rl;
   int                 rn;

   if (xsrc->attached)
      ExShapeUpdate(xsrc);

   rn = xsrc->num_rect;
   rl = xsrc->rects;

   if (rn < 0)
     {
	/* Source has empty shape */
	ExShapeCombineShape(xdst, ShapeBounding, 0, 0,
			    xsrc, ShapeBounding, ShapeSet);
     }
   else if (rn == 0)
     {
	/* Source has default shape (no shape) */
	ExShapeCombineMask(xdst, ShapeBounding, 0, 0, None, ShapeSet);
     }
   else if (rn == 1)
     {
	if ((rl[0].x <= 0) && (rl[0].y <= 0) && (rl[0].width >= xsrc->w)
	    && (rl[0].height >= xsrc->h))
	  {
	     rn = 0;
	     ExShapeCombineMask(xdst, ShapeBounding, 0, 0, None, ShapeSet);
	  }
	else
	  {
	     ExShapeCombineShape(xdst, ShapeBounding, 0, 0,
				 xsrc, ShapeBounding, ShapeSet);
	  }
     }
   else
     {
	ExShapeCombineShape(xdst, ShapeBounding, 0, 0,
			    xsrc, ShapeBounding, ShapeSet);
     }

   return rn != 0;
}

static int
ExShapePropagate(EXID * xid)
{
   EXID               *xch;
   Window              rt, par, *list = NULL;
   unsigned int        i, num, num_rects;
   int                 k, rn;
   int                 x, y, w, h;
   XRectangle         *rects, *rl;
   XWindowAttributes   att;

   if (!xid || xid->w <= 0 || xid->h <= 0)
      return 0;

#if DEBUG_SHAPE_PROPAGATE
   Eprintf("ExShapePropagate %#lx %d,%d %dx%d\n", win, xid->x, xid->y, xid->w,
	   xid->h);
#endif

   XQueryTree(disp, xid->win, &rt, &par, &list, &num);
   if (!list)
      return 0;

   num_rects = 0;
   rects = NULL;

   /* go through all child windows and create/inset spans */
   for (i = 0; i < num; i++)
     {
	xch = EXidFind(list[i]);
	if (!xch)
	   continue;		/* Should never happen */
	XGetWindowAttributes(disp, list[i], &att);
#if DEBUG_SHAPE_PROPAGATE > 1
	Eprintf("%3d %#lx(%d): %4d,%4d %4dx%4d\n", i, list[i], att.map_state,
		att.x, att.y, att.width, att.height);
#endif
	if ((att.class != InputOutput) || (att.map_state == IsUnmapped))
	   continue;

	x = xch->x;
	y = xch->y;
	w = xch->w;
	h = xch->h;
	if (x >= xid->w || y >= xid->h || x + w < 0 || y + h < 0)
	   continue;

	rn = xch->num_rect;

	if (rn > 0)
	  {
	     rl = xch->rects;
	     rects = Erealloc(rects, (num_rects + rn) * sizeof(XRectangle));
	     /* go through all clip rects in thsi window's shape */
	     for (k = 0; k < rn; k++)
	       {
		  /* for each clip rect, add it to the rect list */
		  rects[num_rects + k].x = x + rl[k].x;
		  rects[num_rects + k].y = y + rl[k].y;
		  rects[num_rects + k].width = rl[k].width;
		  rects[num_rects + k].height = rl[k].height;
#if DEBUG_SHAPE_PROPAGATE > 1
		  Eprintf(" - %d: %4d,%4d %4dx%4d\n", k,
			  rects[num_rects + k].x,
			  rects[num_rects + k].y, rects[num_rects + k].width,
			  rects[num_rects + k].height);
#endif
	       }
	     num_rects += rn;
	  }
	else if (rn == 0)
	  {
	     /* Unshaped */
	     rects = Erealloc(rects, (num_rects + 1) * sizeof(XRectangle));

	     rects[num_rects].x = x;
	     rects[num_rects].y = y;
	     rects[num_rects].width = w;
	     rects[num_rects].height = h;
	     num_rects++;
	  }
     }
   XFree(list);

#if DEBUG_SHAPE_PROPAGATE > 1
   Eprintf("ExShapePropagate %#lx nr=%d\n", win, num_rects);
   for (i = 0; i < num_rects; i++)
      Eprintf("%3d %4d,%4d %4dx%4d\n", i, rects[i].x, rects[i].y,
	      rects[i].width, rects[i].height);
#endif

   /* set the rects as the shape mask */
   if (rects)
     {
	ExShapeCombineRectangles(xid, ShapeBounding, 0, 0, rects,
				 num_rects, ShapeSet, Unsorted);
	Efree(rects);
     }
   else
     {
	/* Empty shape */
	ExShapeCombineRectangles(xid, ShapeBounding, 0, 0, NULL, 0, ShapeSet,
				 Unsorted);
     }

   return xid->num_rect;
}

void
EShapeCombineMask(Window win, int dest, int x, int y, Pixmap pmap, int op)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
      ExShapeCombineMask(xid, dest, x, y, pmap, op);
   else
      XShapeCombineMask(disp, win, dest, x, y, pmap, op);
}

void
EShapeCombineRectangles(Window win, int dest, int x, int y,
			XRectangle * rect, int n_rects, int op, int ordering)
{
   EXID               *xid;

   xid = EXidFind(win);
   if (xid)
      ExShapeCombineRectangles(xid, dest, x, y, rect, n_rects, op, ordering);
   else
      XShapeCombineRectangles(disp, win, dest, x, y, rect, n_rects, op,
			      ordering);
}

void
EShapeCombineShape(Window win, int dest, int x, int y,
		   Window src_win, int src_kind, int op)
{
   EXID               *xid;

   XShapeCombineShape(disp, win, dest, x, y, src_win, src_kind, op);
   xid = EXidFind(win);
   if (xid)
      ExShapeUpdate(xid);
}

int
EShapeCopy(Window dst, Window src)
{
   EXID               *xsrc, *xdst;

   xsrc = EXidFind(src);
   xdst = EXidFind(dst);
   if (!xsrc || !xdst)
      return 0;

   return ExShapeCopy(xdst, xsrc);
}

int
EShapePropagate(Window win)
{
   EXID               *xid;

   xid = EXidFind(win);

   return ExShapePropagate(xid);
}

int
EShapeCheck(Window win)
{
   EXID               *xid;

   xid = EXidFind(win);

   return xid->num_rect;
}

Pixmap
ECreatePixmapCopy(Pixmap src, unsigned int w, unsigned int h,
		  unsigned int depth)
{
   Pixmap              pmap;
   GC                  gc;

   pmap = ECreatePixmap(src, w, h, depth);
   gc = ECreateGC(src, 0, NULL);
   XCopyArea(disp, src, pmap, gc, 0, 0, w, h, 0, 0);
   EFreeGC(gc);

   return pmap;
}

GC
ECreateGC(Drawable d, unsigned long mask, XGCValues * val)
{
   XGCValues           xgcv;

   if (val)
     {
	mask |= GCGraphicsExposures;
	val->graphics_exposures = False;
     }
   else
     {
	mask = GCGraphicsExposures;
	val = &xgcv;
	val->graphics_exposures = False;
     }
   return XCreateGC(disp, d, mask, val);
}

int
EFreeGC(GC gc)
{
   return XFreeGC(disp, gc);
}

void
ESetColor(XColor * pxc, int r, int g, int b)
{
   pxc->red = (r << 8) | r;
   pxc->green = (g << 8) | g;
   pxc->blue = (b << 8) | b;
}

void
EGetColor(const XColor * pxc, int *pr, int *pg, int *pb)
{
   *pr = pxc->red >> 8;
   *pg = pxc->green >> 8;
   *pb = pxc->blue >> 8;
}

/* Build mask from window shape rects */
/* Snatched from imlib_create_scaled_image_from_drawable() */
Pixmap
EWindowGetShapePixmap(Window win)
{
   Pixmap              mask;
   GC                  gc;
   XRectangle         *rect;
   int                 i, w, h;
   int                 rect_num, rect_ord;

   EGetGeometry(win, NULL, NULL, NULL, &w, &h, NULL, NULL);
   mask = ECreatePixmap(win, w, h, 1);

   gc = XCreateGC(disp, mask, 0, NULL);
   XSetForeground(disp, gc, 0);

   rect = XShapeGetRectangles(disp, win, ShapeBounding, &rect_num, &rect_ord);
   XFillRectangle(disp, mask, gc, 0, 0, w, h);
   if (rect)
     {
	XSetForeground(disp, gc, 1);
	for (i = 0; i < rect_num; i++)
	   XFillRectangle(disp, mask, gc, rect[i].x, rect[i].y,
			  rect[i].width, rect[i].height);
	XFree(rect);
     }

   XFreeGC(disp, gc);

   return mask;
}

void
EGrabServer(void)
{
   if (Mode.grabs.server_grabbed <= 0)
     {
	if (EventDebug(EDBUG_TYPE_GRABS))
	   Eprintf("EGrabServer\n");
	XGrabServer(disp);
     }
   Mode.grabs.server_grabbed++;
}

void
EUngrabServer(void)
{
   if (Mode.grabs.server_grabbed == 1)
     {
	XUngrabServer(disp);
	XFlush(disp);
	if (EventDebug(EDBUG_TYPE_GRABS))
	   Eprintf("EUngrabServer\n");
     }
   Mode.grabs.server_grabbed--;
   if (Mode.grabs.server_grabbed < 0)
      Mode.grabs.server_grabbed = 0;
}

int
EServerIsGrabbed(void)
{
   return Mode.grabs.server_grabbed;
}

void
EFlush(void)
{
   XFlush(disp);
}

void
ESync(void)
{
   XSync(disp, False);
}

Display            *
EDisplayOpen(const char *dstr, int scr)
{
   char                dbuf[256], *s;
   Display            *dpy;

   if (scr >= 0)
     {
	/* Override screen */
	Esnprintf(dbuf, sizeof(dbuf) - 10, dstr);
	s = strchr(dbuf, ':');
	if (s)
	  {
	     s = strchr(s, '.');
	     if (s)
		*s = '\0';
	  }
	Esnprintf(dbuf + strlen(dbuf), 10, ".%d", scr);
	dstr = dbuf;
     }

#ifdef USE_ECORE_X
   ecore_x_init(dstr);
   dpy = ecore_x_display_get();
#else
   dpy = XOpenDisplay(dstr);
#endif

   return dpy;
}

void
EDisplayClose(void)
{
   if (!disp)
      return;
#ifdef USE_ECORE_X
   ecore_x_shutdown();
#else
   XCloseDisplay(disp);
#endif
   XSetErrorHandler((XErrorHandler) NULL);
   XSetIOErrorHandler((XIOErrorHandler) NULL);
   disp = NULL;
}

void
EDisplayDisconnect(void)
{
   if (!disp)
      return;
#ifdef USE_ECORE_X
   ecore_x_disconnect();
#else
   close(ConnectionNumber(disp));
#endif
   XSetErrorHandler((XErrorHandler) NULL);
   XSetIOErrorHandler((XIOErrorHandler) NULL);

   disp = NULL;
}

Time
EGetTimestamp(void)
{
   static Window       win_ts = None;
   XSetWindowAttributes attr;
   XEvent              ev;

   if (win_ts == None)
     {
	attr.override_redirect = False;
	win_ts = XCreateWindow(disp, VRoot.win, -100, -100, 1, 1, 0,
			       CopyFromParent, InputOnly, CopyFromParent,
			       CWOverrideRedirect, &attr);
	XSelectInput(disp, win_ts, PropertyChangeMask);
     }

   XChangeProperty(disp, win_ts, XA_WM_NAME, XA_STRING, 8,
		   PropModeAppend, (unsigned char *)"", 0);
   XWindowEvent(disp, win_ts, PropertyChangeMask, &ev);

   return ev.xproperty.time;
}

void
EDrawableDumpImage(Drawable draw, const char *txt)
{
   static int          seqn = 0;
   char                buf[1024];
   Imlib_Image        *im;
   int                 w, h;

   w = h = 0;
   EGetGeometry(draw, NULL, NULL, NULL, &w, &h, NULL, NULL);
   if (w <= 0 || h <= 0)
      return;
   imlib_context_set_drawable(draw);
   im = imlib_create_image_from_drawable(None, 0, 0, w, h, !EServerIsGrabbed());
   imlib_context_set_image(im);
   imlib_image_set_format("png");
   sprintf(buf, "%s-%#lx-%d.png", txt, draw, seqn++);
   Eprintf("EDrawableDumpImage: %s\n", buf);
   imlib_save_image(buf);
   imlib_free_image_and_decache();
}

void
FreePmapMask(PmapMask * pmm)
{
   /* type !=0: Created by imlib_render_pixmaps_for_whole_image... */
   if (pmm->pmap)
     {
	if (pmm->type == 0)
	   EFreePixmap(pmm->pmap);
	else
	   imlib_free_pixmap_and_mask(pmm->pmap);
	pmm->pmap = 0;
     }

   if (pmm->mask)
     {
	if (pmm->type == 0)
	   EFreePixmap(pmm->mask);
	pmm->mask = 0;
     }
}
