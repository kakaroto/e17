/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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
#include <X11/Xutil.h>
#include <X11/Xresource.h>

typedef struct _exid
{
   Window              parent;
   Window              win;
   int                 x, y, w, h;
   char                mapped;
   int                 num_rect;
   int                 ord;
   XRectangle         *rects;
   int                 depth;
   Pixmap              bgpmap;
   int                 bgcol;
}
EXID;

static XContext     xid_context = 0;

static EXID        *
NewXID(void)
{
   EXID               *xid;

   xid = Emalloc(sizeof(EXID));
   xid->parent = 0;
   xid->win = 0;
   xid->x = 0;
   xid->y = 0;
   xid->w = 0;
   xid->h = 0;
   xid->num_rect = 0;
   xid->ord = 0;
   xid->rects = NULL;
   xid->depth = 0;
   xid->mapped = 0;
   xid->bgpmap = 0;
   xid->bgcol = 0;

   return xid;
}

static void
AddXID(EXID * xid)
{
   if (!xid_context)
      xid_context = XUniqueContext();
   XSaveContext(disp, xid->win, xid_context, (XPointer) xid);
   AddItem(xid, "", xid->win, LIST_TYPE_XID);
}

static EXID        *
FindXID(Window win)
{
   EXID               *xid = NULL;

   if (xid_context == 0)
      xid_context = XUniqueContext();
   if (XFindContext(disp, win, xid_context, (XPointer *) & xid) == XCNOENT)
      xid = NULL;
   return xid;
}

static void
DelXID(Window win)
{
   EXID               *xid;

   if (xid_context == 0)
      xid_context = XUniqueContext();
   xid = RemoveItem("", win, LIST_FINDBY_ID, LIST_TYPE_XID);
   if (xid)
     {
	XDeleteContext(disp, win, xid_context);
	if (xid->rects)
	   XFree(xid->rects);
	Efree(xid);
     }
}

static void
SetXID(Window win, Window parent, int x, int y, int w, int h, int depth)
{
   EXID               *xid;

   xid = NewXID();
   xid->parent = parent;
   xid->win = win;
   xid->x = x;
   xid->y = y;
   xid->w = w;
   xid->h = h;
   xid->depth = root.depth;
   AddXID(xid);
}

Pixmap
ECreatePixmap(Display * display, Drawable d, unsigned int width,
	      unsigned int height, unsigned depth)
{
   Pixmap              pm;

   pm = XCreatePixmap(display, d, width, height, depth);
   return pm;
}

void
EFreePixmap(Display * display, Pixmap pixmap)
{
   XFreePixmap(display, pixmap);
}

Window
ECreateWindow(Window parent, int x, int y, int w, int h, int saveunder)
{
   Window              win;
   XSetWindowAttributes attr;

   EDBUG(6, "ECreateWindow");
   attr.backing_store = NotUseful;
   attr.override_redirect = True;
   attr.colormap = root.cmap;
   attr.border_pixel = 0;
/*   attr.background_pixel = 0; */
   attr.background_pixmap = None;
   if ((saveunder == 1) && (mode.save_under))
      attr.save_under = True;
   else if (saveunder == 2)
      attr.save_under = True;
   else
      attr.save_under = False;
   win =
      XCreateWindow(disp, parent, x, y, w, h, 0, root.depth, InputOutput,
		    root.vis,
		    CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		    CWColormap | CWBackPixmap | CWBorderPixel, &attr);
   SetXID(win, parent, x, y, w, h, root.depth);

   EDBUG_RETURN(win);
}

void
EMoveWindow(Display * d, Window win, int x, int y)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if ((x != xid->x) || (y != xid->y))
	  {
	     xid->x = x;
	     xid->y = y;
	     XMoveWindow(d, win, x, y);
	  }
     }
   else
      XMoveWindow(d, win, x, y);
}

void
EResizeWindow(Display * d, Window win, int w, int h)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if ((w != xid->w) || (h != xid->h))
	  {
	     xid->w = w;
	     xid->h = h;
	     XResizeWindow(d, win, w, h);
	  }
     }
   else
      XResizeWindow(d, win, w, h);
}

void
EMoveResizeWindow(Display * d, Window win, int x, int y, int w, int h)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if ((w != xid->w) || (h != xid->h) || (x != xid->x) || (y != xid->y))
	  {
	     xid->x = x;
	     xid->y = y;
	     xid->w = w;
	     xid->h = h;
	     XMoveResizeWindow(d, win, x, y, w, h);
	  }
     }
   else
      XMoveResizeWindow(d, win, x, y, w, h);
}

void
EDestroyWindow(Display * d, Window win)
{
   EXID               *xid;

   if ((mode.slideout) && (mode.slideout->from_win == win))
      HideSlideout(mode.slideout, 0);
   xid = FindXID(win);
   if (xid)
     {
	EXID              **lst;
	int                 i, num;

	DelXID(win);
	XDestroyWindow(d, win);
	lst = (EXID **) ListItemType(&num, LIST_TYPE_XID);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (lst[i]->parent == win)
		     EDestroyWindow(d, lst[i]->win);
	       }
	     Efree(lst);
	  }
     }
   else
      XDestroyWindow(d, win);
}

void
EForgetWindow(Display * d, Window win)
{
   DelXID(win);
}

void
EMapWindow(Display * d, Window win)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if (!xid->mapped)
	  {
	     xid->mapped = 1;
	     XMapWindow(d, win);
	  }
     }
   else
      XMapWindow(d, win);
}

void
EUnmapWindow(Display * d, Window win)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if (xid->mapped)
	  {
	     xid->mapped = 0;
	     XUnmapWindow(d, win);
	  }
     }
   else
      XUnmapWindow(d, win);
}

void
EShapeCombineMask(Display * d, Window win, int dest, int x, int y, Pixmap pmap,
		  int op)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	char                wasshaped = 0;

	if (xid->rects)
	  {
	     xid->num_rect = 0;
	     XFree(xid->rects);
	     xid->rects = NULL;
	     wasshaped = 1;
	  }
	if (pmap)
	  {
	     XShapeCombineMask(d, win, dest, x, y, pmap, op);
	     xid->rects =
		XShapeGetRectangles(d, win, dest, &(xid->num_rect),
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
			 }
		    }
	       }
	  }
	else if ((!pmap) && (wasshaped))
	   XShapeCombineMask(d, win, dest, x, y, pmap, op);
     }
   else
      XShapeCombineMask(d, win, dest, x, y, pmap, op);
}

void
EShapeCombineRectangles(Display * d, Window win, int dest, int x, int y,
			XRectangle * rect, int n_rects, int op, int ordering)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if (n_rects == 1)
	  {
	     if ((rect[0].x == 0) && (rect[0].y == 0)
		 && (rect[0].width == xid->w) && (rect[0].height == xid->h))
	       {
		  xid->num_rect = 0;
		  XFree(xid->rects);
		  xid->rects = NULL;
		  XShapeCombineMask(d, win, dest, x, y, None, op);
		  return;
	       }
	  }
	xid->num_rect = 0;
	if (xid->rects)
	   XFree(xid->rects);
	XShapeCombineRectangles(d, win, dest, x, y, rect, n_rects, op,
				ordering);
	xid->rects =
	   XShapeGetRectangles(d, win, dest, &(xid->num_rect), &(xid->ord));
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
		    }
	       }
	  }
     }
   else
      XShapeCombineRectangles(d, win, dest, x, y, rect, n_rects, op, ordering);
}

void
EShapeCombineShape(Display * d, Window win, int dest, int x, int y,
		   Window src_win, int src_kind, int op)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	xid->num_rect = 0;
	if (xid->rects)
	   XFree(xid->rects);
	XShapeCombineShape(d, win, dest, x, y, src_win, src_kind, op);
	xid->rects =
	   XShapeGetRectangles(d, win, dest, &(xid->num_rect), &(xid->ord));
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
		    }
	       }
	  }
     }
   else
      XShapeCombineShape(d, win, dest, x, y, src_win, src_kind, op);
}

XRectangle         *
EShapeGetRectangles(Display * d, Window win, int dest, int *rn, int *ord)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	XRectangle         *r;

	*rn = xid->num_rect;
	*ord = xid->ord;
	if (xid->num_rect > 0)
	  {
	     r = Emalloc(sizeof(XRectangle) * xid->num_rect);
	     memcpy(r, xid->rects, sizeof(XRectangle) * xid->num_rect);
	     return r;
	  }
	else
	   return NULL;
     }
   else
     {
	XRectangle         *r, *rr;

	r = XShapeGetRectangles(d, win, dest, rn, ord);
	if (r)
	  {
	     rr = Emalloc(sizeof(XRectangle) * *rn);
	     memcpy(rr, r, sizeof(XRectangle) * *rn);
	     XFree(r);
	     return rr;
	  }
     }
   return NULL;
}

void
EReparentWindow(Display * d, Window win, Window parent, int x, int y)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if (parent == xid->parent)
	  {
	     if ((x != xid->x) || (y != xid->y))
	       {
		  xid->x = x;
		  xid->y = y;
		  XMoveWindow(d, win, x, y);
	       }
	  }
	else
	  {
	     xid->parent = parent;
	     xid->x = x;
	     xid->y = y;
	     XReparentWindow(d, win, parent, x, y);
	  }
     }
   else
      XReparentWindow(d, win, parent, x, y);
}

void
EMapRaised(Display * d, Window win)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if (xid->mapped)
	   XRaiseWindow(d, win);
	else
	  {
	     xid->mapped = 1;
	     XMapRaised(d, win);
	  }
     }
   else
      XMapRaised(d, win);
}

int
EGetGeometry(Display * d, Window win, Window * root_return, int *x, int *y,
	     unsigned int *w, unsigned int *h, unsigned int *bw,
	     unsigned int *depth)
{
   EXID               *xid;

   xid = FindXID(win);
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
	   *root_return = root.win;
	return 1;
     }
   return XGetGeometry(d, win, root_return, x, y, w, h, bw, depth);
}

void
EConfigureWindow(Display * d, Window win, unsigned int mask,
		 XWindowChanges * wc)
{
   EXID               *xid;

   xid = FindXID(win);
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
	   XConfigureWindow(d, win, mask, wc);
     }
   else
     {
	XConfigureWindow(d, win, mask, wc);
     }
}

void
ESetWindowBackgroundPixmap(Display * d, Window win, Pixmap pmap)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	xid->bgpmap = pmap;
	XSetWindowBackgroundPixmap(d, win, pmap);
     }
   else
      XSetWindowBackgroundPixmap(d, win, pmap);
}

void
ESetWindowBackground(Display * d, Window win, int col)
{
   EXID               *xid;

   xid = FindXID(win);
   if (xid)
     {
	if (xid->bgpmap)
	  {
	     xid->bgpmap = 0;
	     xid->bgcol = col;
	     XSetWindowBackground(d, win, col);
	  }
	else if (xid->bgcol != col)
	  {
	     xid->bgcol = col;
	     XSetWindowBackground(d, win, col);
	  }
     }
   else
      XSetWindowBackground(d, win, col);
}

Window
ECreateEventWindow(Window parent, int x, int y, int w, int h)
{
   Window              win;
   XSetWindowAttributes attr;

   EDBUG(6, "ECreateEventWindow");
   attr.override_redirect = False;
   win =
      XCreateWindow(disp, parent, x, y, w, h, 0, 0, InputOnly, root.vis,
		    CWOverrideRedirect, &attr);
#if 0				/* Not yet */
   SetXID(win, parent, x, y, w, h, root.depth);
#endif

   EDBUG_RETURN(win);
}

/*
 * create a window which will accept the keyboard focus when no other 
 * windows have it
 */
Window
ECreateFocusWindow(Window parent, int x, int y, int w, int h)
{
   Window              win;
   XSetWindowAttributes attr;

   EDBUG(6, "ECreateFocusWindow");

   attr.backing_store = NotUseful;
   attr.override_redirect = True;
   attr.colormap = root.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = False;
   attr.event_mask = KeyPressMask | FocusChangeMask;

   win =
      XCreateWindow(disp, parent, x, y, w, h, 0, 0, InputOnly, CopyFromParent,
		    CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		    CWColormap | CWBackPixel | CWBorderPixel | CWEventMask,
		    &attr);

   XSetWindowBackground(disp, win, 0);
   XMapWindow(disp, win);
   XSetInputFocus(disp, win, RevertToParent, CurrentTime);

   EDBUG_RETURN(win);
}

void
GrabX()
{
   EDBUG(6, "GrabX");
   if (mode.server_grabbed <= 0)
      XGrabServer(disp);
   mode.server_grabbed++;
   EDBUG_RETURN_;
}

void
UngrabX()
{
   EDBUG(6, "UngrabX");
   if (mode.server_grabbed == 1)
     {
	XUngrabServer(disp);
	XFlush(disp);
     }
   mode.server_grabbed--;
   if (mode.server_grabbed < 0)
      mode.server_grabbed = 0;
   EDBUG_RETURN_;
}

void
SetBG(Window win, Pixmap pmap, int color)
{
   static Atom         a = 0, aa = 0, aaa = 0;
   static Window       alive_win = 0;

   EDBUG(6, "SetBG");
   if (!a)
     {
	a = XInternAtom(disp, "_XROOTPMAP_ID", False);
	aa = XInternAtom(disp, "_XROOTCOLOR_PIXEL", False);
	aaa = XInternAtom(disp, "_XROOTWINDOW", False);
     }
   if (!alive_win)
     {
	alive_win = ECreateWindow(root.win, -100, -100, 1, 1, 0);
	XChangeProperty(disp, alive_win, aaa, XA_WINDOW, 32, PropModeReplace,
			(unsigned char *)&alive_win, 1);
	XChangeProperty(disp, root.win, aaa, XA_WINDOW, 32, PropModeReplace,
			(unsigned char *)&alive_win, 1);
     }
   XChangeProperty(disp, win, a, XA_PIXMAP, 32, PropModeReplace,
		   (unsigned char *)&pmap, 1);
   XChangeProperty(disp, win, aa, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)&color, 1);
   if (pmap)
      XSetWindowBackgroundPixmap(disp, win, pmap);
   else
      XSetWindowBackground(disp, win, color);
   XClearWindow(disp, win);
   EDBUG_RETURN_;
}

void
GetWinXY(Window win, int *x, int *y)
{
   Window              w1;
   unsigned int        w, h, b, d;

   EDBUG(7, "GetWinXY");
   EGetGeometry(disp, win, &w1, x, y, &w, &h, &b, &d);
   EDBUG_RETURN_;
}

void
GetWinWH(Window win, unsigned int *w, unsigned int *h)
{
   Window              w1;
   int                 x, y;
   unsigned int        b, d;

   EDBUG(7, "GetWinWH");
   EGetGeometry(disp, win, &w1, &x, &y, w, h, &b, &d);
   EDBUG_RETURN_;
}

int
GetWinDepth(Window win)
{
   Window              w1;
   unsigned int        w, h, b, d;
   int                 x, y;

   EDBUG(7, "GetWinDepth");
   EGetGeometry(disp, win, &w1, &x, &y, &w, &h, &b, &d);
   EDBUG_RETURN(d);
}

int
WinExists(Window win)
{
   Window              w1;
   int                 x, y;
   unsigned int        w, h;
   unsigned int        b, d;

   EDBUG(7, "WinExists");
   if (EGetGeometry(disp, win, &w1, &x, &y, &w, &h, &b, &d))
      EDBUG_RETURN(1);
   EDBUG_RETURN(0);
}

Window
WindowAtXY_0(Window base, int bx, int by, int x, int y)
{
   Window             *list = NULL;
   XWindowAttributes   att;
   Window              child = 0, parent_win = 0, root_win = 0;
   int                 i;
   unsigned int        ww, wh, num;
   int                 wx, wy;

   EDBUG(7, "WindowAtXY_0");
   if (!XGetWindowAttributes(disp, base, &att))
      EDBUG_RETURN(0);
   if (att.class == InputOnly)
      EDBUG_RETURN(0);
   if (att.map_state != IsViewable)
      EDBUG_RETURN(0);
   wx = att.x;
   wy = att.y;
   ww = att.width;
   wh = att.height;

   wx += bx;
   wy += by;

   if (!((x >= wx) && (y >= wy) && (x < (int)(wx + ww))
	 && (y < (int)(wy + wh))))
      EDBUG_RETURN(0);

   if (!XQueryTree(disp, base, &root_win, &parent_win, &list, &num))
      EDBUG_RETURN(base);
   if (list)
     {
	for (i = num - 1;; i--)
	  {
	     if ((child = WindowAtXY_0(list[i], wx, wy, x, y)) != 0)
	       {
		  XFree(list);
		  EDBUG_RETURN(child);
	       }
	     if (!i)
		break;
	  }
	XFree(list);
     }
   EDBUG_RETURN(base);
}

Window
WindowAtXY(int x, int y)
{
   Window             *list = NULL;
   Window              child = 0, parent_win = 0, root_win = 0;
   unsigned int        num;
   int                 i;

   EDBUG(7, "WindowAtXY");
   GrabX();
   if (!XQueryTree(disp, root.win, &root_win, &parent_win, &list, &num))
     {
	UngrabX();
	EDBUG_RETURN(root.win);
     }
   if (list)
     {
	i = num - 1;
	do
	  {
	     XWindowAttributes   xwa;

	     XGetWindowAttributes(disp, list[i], &xwa);
	     if (xwa.map_state != IsViewable)
		continue;

	     if ((child = WindowAtXY_0(list[i], 0, 0, x, y)) == 0)
		continue;

	     XFree(list);
	     UngrabX();
	     EDBUG_RETURN(child);
	  }
	while (--i > 0);
	XFree(list);
     }
   UngrabX();
   EDBUG_RETURN(root.win);
}

void
PointerAt(int *x, int *y)
{
   Window              dw;
   int                 dd;
   unsigned int        mm;

   XQueryPointer(disp, root.win, &dw, &dw, &dd, &dd, x, y, &mm);
}

void
PastePixmap(Display * d, Drawable w, Pixmap p, Mask m, int x, int y)
{
   static GC           gc = 0;
   XGCValues           gcv;
   int                 ww, hh;

   if (!gc)
      gc = XCreateGC(d, w, 0, &gcv);
   GetWinWH(p, (unsigned int *)&ww, (unsigned int *)&hh);
   XSetClipMask(disp, gc, m);
   XSetClipOrigin(disp, gc, x, y);
   XCopyArea(disp, p, w, gc, 0, 0, ww, hh, x, y);
}

void
PasteMask(Display * d, Drawable w, Pixmap p, int x, int y, int wd, int ht)
{
   static GC           gc = 0;
   XGCValues           gcv;
   int                 ww, hh;

   if (!gc)
      gc = XCreateGC(d, w, 0, &gcv);
   if (p)
     {
	GetWinWH(p, (unsigned int *)&ww, (unsigned int *)&hh);
	XSetClipMask(disp, gc, p);
	XSetClipOrigin(disp, gc, x, y);
	XCopyArea(disp, p, w, gc, 0, 0, ww, hh, x, y);
     }
   else
     {
	XSetForeground(disp, gc, 1);
	XFillRectangle(disp, w, gc, x, y, wd, ht);
     }
}

int
GetPointerScreenGeometry(int *px, int *py, int *pw, int *ph)
{
   int                 head = 0;

#ifdef HAS_XINERAMA
   if (xinerama_active)
     {
	int                 i;
	Window              rt, ch;
	XineramaScreenInfo *screens;
	int                 pointer_x, pointer_y;
	int                 num;
	int                 d;
	unsigned int        ud;

	XQueryPointer(disp, root.win, &rt, &ch, &pointer_x, &pointer_y, &d,
		      &d, &ud);
	screens = XineramaQueryScreens(disp, &num);
	for (i = 0; i < num; i++)
	  {
	     if (pointer_x >= screens[i].x_org &&
		 pointer_x <= (screens[i].width + screens[i].x_org) &&
		 pointer_y >= screens[i].y_org &&
		 pointer_y <= (screens[i].height + screens[i].y_org))
	       {
		  *px = screens[i].x_org;
		  *py = screens[i].y_org;
		  *pw = screens[i].width;
		  *ph = screens[i].height;
		  head = i;
	       }
	  }
	XFree(screens);
     }
   else
#endif
     {
	*px = 0;
	*py = 0;
	*pw = root.w;
	*ph = root.h;
     }

   return head;
}
