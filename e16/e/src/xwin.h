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

typedef struct _xwin *Win;

#define NoWin ((Win)0)

Win                 ELookupXwin(Window xwin);

#define Xwin(win) WinGetXwin(win)
Window              WinGetXwin(const Win win);
int                 WinGetBorderWidth(const Win win);
int                 WinGetDepth(const Win win);
Visual             *WinGetVisual(const Win win);
Colormap            WinGetCmap(const Win win);

Win                 ECreateWinFromXwin(Window xwin);
void                EDestroyWin(Win win);

Display            *EDisplayOpen(const char *dstr, int scr);
void                EDisplayClose(void);
void                EDisplayDisconnect(void);
void                EGrabServer(void);
void                EUngrabServer(void);
int                 EServerIsGrabbed(void);
void                EFlush(void);
void                ESync(void);
Time                EGetTimestamp(void);

Win                 ERegisterWindow(Window xwin, XWindowAttributes * pxwa);
void                EUnregisterWindow(Win win);
void                EUnregisterXwin(Window xwin);
typedef void        (EventCallbackFunc) (Win win, XEvent * ev, void *prm);
void                EventCallbackRegister(Win win, int type,
					  EventCallbackFunc * func, void *prm);
void                EventCallbackUnregister(Win win, int type,
					    EventCallbackFunc * func,
					    void *prm);
void                EventCallbacksProcess(Win win, XEvent * ev);

Win                 ECreateWindow(Win parent, int x, int y, int w, int h,
				  int saveunder);
Win                 ECreateVisualWindow(Win parent, int x, int y, int w,
					int h, int saveunder,
					XWindowAttributes * child_attr);
Win                 ECreateEventWindow(Win parent, int x, int y, int w, int h);
Win                 ECreateFocusWindow(Win parent, int x, int y, int w, int h);
void                EWindowSync(Win win);
void                EWindowSetMapped(Win win, int mapped);
void                ESelectInputAdd(Win win, long mask);

void                EMoveWindow(Win win, int x, int y);
void                EResizeWindow(Win win, int w, int h);
void                EMoveResizeWindow(Win win, int x, int y, int w, int h);
void                EDestroyWindow(Win win);
void                EMapWindow(Win win);
void                EMapRaised(Win win);
void                EUnmapWindow(Win win);
void                EReparentWindow(Win win, Win parent, int x, int y);
int                 EGetGeometry(Win win, Window * root_return,
				 int *x, int *y, int *w, int *h, int *bw,
				 int *depth);
void                EGetWindowAttributes(Win win, XWindowAttributes * pxwa);
void                EConfigureWindow(Win win, unsigned int mask,
				     XWindowChanges * wc);
void                ESetWindowBackgroundPixmap(Win win, Pixmap pmap);
void                ESetWindowBackground(Win win, unsigned int col);
int                 ETranslateCoordinates(Win src_w, Win dst_w,
					  int src_x, int src_y,
					  int *dest_x_return,
					  int *dest_y_return,
					  Window * child_return);
int                 EDrawableCheck(Drawable draw, int grab);

#define ESelectInput(win, event_mask) \
	XSelectInput(disp, Xwin(win), event_mask)

#define EChangeWindowAttributes(win, mask, attr) \
	XChangeWindowAttributes(disp, Xwin(win), mask, attr)
#define ESetWindowBorderWidth(win, bw) \
	XSetWindowBorderWidth(disp, Xwin(win), bw)

#define ERaiseWindow(win) \
	XRaiseWindow(disp, Xwin(win))
#define ELowerWindow(win) \
	XLowerWindow(disp, Xwin(win))

#define EClearWindow(win) \
	XClearWindow(disp, Xwin(win))
#define EClearArea(win, x, y, w, h, exp) \
	XClearArea(disp, Xwin(win), x, y, w, h, exp)

Pixmap              ECreatePixmap(Win win, unsigned int width,
				  unsigned int height, unsigned int depth);
void                EFreePixmap(Pixmap pixmap);

void                EShapeCombineMask(Win win, int dest, int x, int y,
				      Pixmap pmap, int op);
void                EShapeCombineMaskTiled(Win win, int dest, int x, int y,
					   Pixmap pmap, int op, int w, int h);
void                EShapeCombineRectangles(Win win, int dest, int x, int y,
					    XRectangle * rect, int n_rects,
					    int op, int ordering);
void                EShapeCombineShape(Win win, int dest, int x, int y,
				       Win src_win, int src_kind, int op);
XRectangle         *EShapeGetRectangles(Win win, int dest, int *rn, int *ord);
int                 EShapeCopy(Win dst, Win src);
int                 EShapePropagate(Win win);
int                 EShapeCheck(Win win);
Pixmap              EWindowGetShapePixmap(Win win);

void                EAllocColor(Colormap colormap, XColor * pxc);
void                ESetColor(XColor * pxc, int r, int g, int b);
void                EGetColor(const XColor * pxc, int *pr, int *pg, int *pb);

Window              EXWindowGetParent(Window xwin);
int                 EXGetGeometry(Window xwin, Window * root_return,
				  int *x, int *y, int *w, int *h, int *bw,
				  int *depth);
#define EXGetWindowAttributes(win, xwa) \
	XGetWindowAttributes(disp, Xwin(win), xwa)

void                EXCopyArea(Drawable src, Drawable dst, int sx, int sy,
			       unsigned int w, unsigned int h, int dx, int dy);

Bool                EXQueryPointer(Window xwin, int *px, int *py,
				   Window * pchild, unsigned int *pmask);
void                EXWarpPointer(Window xwin, int x, int y);

#define EXCreatePixmap(win, w, h, d) \
	XCreatePixmap(disp, win, w, h, d)
#define EXFreePixmap(pmap) \
	XFreePixmap(disp, pmap)
Pixmap              EXCreatePixmapCopy(Pixmap src, unsigned int w,
				       unsigned int h, unsigned int depth);

GC                  EXCreateGC(Drawable draw, unsigned long mask,
			       XGCValues * val);
int                 EXFreeGC(GC gc);

typedef struct
{
   char                type;
   Pixmap              pmap;
   Pixmap              mask;
   int                 w, h;
} PmapMask;

void                FreePmapMask(PmapMask * pmm);

#endif /* _XWIN_H_ */
