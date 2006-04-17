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
#ifndef _XWIN_H_
#define _XWIN_H_

Display            *EDisplayOpen(const char *dstr, int scr);
void                EDisplayClose(void);
void                EDisplayDisconnect(void);
void                EGrabServer(void);
void                EUngrabServer(void);
int                 EServerIsGrabbed(void);
void                EFlush(void);
void                ESync(void);
Time                EGetTimestamp(void);

void                ERegisterWindow(Window win);
void                EUnregisterWindow(Window win);
typedef void        (EventCallbackFunc) (XEvent * ev, void *prm);
void                EventCallbackRegister(Window win, int type,
					  EventCallbackFunc * func, void *prm);
void                EventCallbackUnregister(Window win, int type,
					    EventCallbackFunc * func,
					    void *prm);
void                EventCallbacksProcess(XEvent * ev);

Window              ECreateWindow(Window parent, int x, int y, int w, int h,
				  int saveunder);
Window              ECreateVisualWindow(Window parent, int x, int y, int w,
					int h, int saveunder,
					XWindowAttributes * child_attr);
Window              ECreateEventWindow(Window parent, int x, int y, int w,
				       int h);
Window              ECreateFocusWindow(Window parent, int x, int y, int w,
				       int h);
void                EWindowSync(Window win);
void                EWindowSetMapped(Window win, int mapped);
Window              EWindowGetParent(Window win);
void                ESelectInputAdd(Window win, long mask);

void                EMoveWindow(Window win, int x, int y);
void                EResizeWindow(Window win, int w, int h);
void                EMoveResizeWindow(Window win, int x, int y, int w, int h);
void                EDestroyWindow(Window win);
void                EMapWindow(Window win);
void                EMapRaised(Window win);
void                EUnmapWindow(Window win);
void                EReparentWindow(Window win, Window parent, int x, int y);
int                 EGetGeometry(Window win, Window * root_return,
				 int *x, int *y, int *w, int *h, int *bw,
				 int *depth);
void                EConfigureWindow(Window win, unsigned int mask,
				     XWindowChanges * wc);
void                ESetWindowBackgroundPixmap(Window win, Pixmap pmap);
void                ESetWindowBackground(Window win, int col);
int                 ETranslateCoordinates(Window src_w, Window dst_w,
					  int src_x, int src_y,
					  int *dest_x_return,
					  int *dest_y_return,
					  Window * child_return);
void                EWarpPointer(Window win, int x, int y);
Bool                EQueryPointer(Window win, int *px, int *py, Window * pchild,
				  unsigned int *pmask);
int                 EDrawableCheck(Drawable draw, int grab);

#define ESelectInput(win, mask) XSelectInput(disp, win, mask)
#define EGetWindowAttributes(win, attr) XGetWindowAttributes(disp, win, attr)
#define EChangeWindowAttributes(win, mask, attr) XChangeWindowAttributes(disp, win, mask, attr)
#define ESetWindowBorderWidth(win, bw) XSetWindowBorderWidth(disp, win, bw)
#define ERaiseWindow(win) XRaiseWindow(disp, win)
#define ELowerWindow(win) XLowerWindow(disp, win)
#define EClearWindow(win) XClearWindow(disp, win)
#define EClearArea(win, x, y, w, h, exp) XClearArea(disp, win, x, y, w, h, exp)

void                EShapeCombineMask(Window win, int dest, int x, int y,
				      Pixmap pmap, int op);
void                EShapeCombineMaskTiled(Window win, int dest, int x, int y,
					   Pixmap pmap, int op, int w, int h);
void                EShapeCombineRectangles(Window win, int dest, int x, int y,
					    XRectangle * rect, int n_rects,
					    int op, int ordering);
void                EShapeCombineShape(Window win, int dest, int x, int y,
				       Window src_win, int src_kind, int op);
XRectangle         *EShapeGetRectangles(Window win, int dest, int *rn,
					int *ord);
int                 EShapeCopy(Window dst, Window src);
int                 EShapePropagate(Window win);
int                 EShapeCheck(Window win);
Pixmap              EWindowGetShapePixmap(Window win);

#define ECreatePixmap(draw, w, h, depth) XCreatePixmap(disp, draw, w, h, depth)
#define EFreePixmap(pmap) XFreePixmap(disp, pmap)
Pixmap              ECreatePixmapCopy(Pixmap src, unsigned int w,
				      unsigned int h, unsigned int depth);
void                ECopyArea(Drawable src, Drawable dst, int sx, int sy,
			      unsigned int w, unsigned int h, int dx, int dy);

GC                  EXCreateGC(Drawable draw, unsigned long mask,
			       XGCValues * val);
int                 EXFreeGC(GC gc);

#define EAllocColor(pxc) \
	XAllocColor(disp, VRoot.cmap, pxc)
void                ESetColor(XColor * pxc, int r, int g, int b);
void                EGetColor(const XColor * pxc, int *pr, int *pg, int *pb);

void                EDrawableDumpImage(Drawable draw, const char *txt);

typedef struct
{
   char                type;
   Pixmap              pmap;
   Pixmap              mask;
   int                 w, h;
} PmapMask;

void                FreePmapMask(PmapMask * pmm);

#endif /* _XWIN_H_ */
