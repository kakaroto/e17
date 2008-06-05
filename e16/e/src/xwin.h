/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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

#include <X11/Xlib.h>
#include "util.h"
#include "xtypes.h"

typedef struct {
   Display            *disp;
   char               *name;
   int                 screens;
   int                 screen;
   unsigned int        pixel_black;
   unsigned int        pixel_white;

   Win                 rroot;	/* Real root window */
   Win                 vroot;	/* Virtual root window */

   int                 server_grabbed;

   unsigned char       last_error_code;
} EDisplay;

__EXPORT__ extern EDisplay Dpy;

#define disp  Dpy.disp
#define RROOT Dpy.rroot
#define VROOT Dpy.vroot

void                EXInit(void);

int                 EDisplayOpen(const char *dstr, int scr);
void                EDisplayClose(void);
void                EDisplayDisconnect(void);
void                EDisplaySetErrorHandlers(void (*fatal) (void));

void                EGrabServer(void);
void                EUngrabServer(void);
int                 EServerIsGrabbed(void);
void                EFlush(void);

#define ESYNC_MAIN	0x0001
#define ESYNC_DESKS	0x0002
#define ESYNC_MENUS	0x0004
#define ESYNC_MOVRES	0x0008
#define ESYNC_FOCUS	0x0010
#define ESYNC_TLOOP	0x0040
#define ESYNC_SLIDEOUT	0x0080
#define ESYNC_STARTUP	0x0100
void                ESync(unsigned int mask);

#if USE_COMPOSITE
int                 EVisualIsARGB(Visual * vis);
Visual             *EVisualFindARGB(void);
#endif

Time                EGetTimestamp(void);

typedef void        (EventCallbackFunc) (Win win, XEvent * ev, void *prm);

#define EXPOSE_WIN 1
#if EXPOSE_WIN || DECLARE_WIN
typedef struct {
   EventCallbackFunc  *func;
   void               *prm;
} EventCallbackItem;

typedef struct {
   int                 num;
   EventCallbackItem  *lst;
} EventCallbackList;

struct _xwin {
   struct _xwin       *next;
   struct _xwin       *prev;
   EventCallbackList   cbl;
   Window              xwin;
   Win                 parent;
   int                 x, y, w, h;
   short               depth;
   unsigned short      bw;
   char                argb;
   char                mapped;
   char                in_use;
   signed char         do_del;
   char                attached;
   signed char         bg_owned;	/* bgpmap "owned" by Win */
   int                 num_rect;
   int                 ord;
   XRectangle         *rects;
   Visual             *visual;
   Colormap            cmap;
   Pixmap              bgpmap;
   unsigned int        bgcol;
};
#endif

Win                 ELookupXwin(Window xwin);

#if EXPOSE_WIN
#define             WinGetXwin(win)		((win)->xwin)
#define             WinGetPmap(win)		((win)->bgpmap)
#define             WinGetX(win)		((win)->x)
#define             WinGetY(win)		((win)->y)
#define             WinGetW(win)		((win)->w)
#define             WinGetH(win)		((win)->h)
#define             WinGetBorderWidth(win)	((win)->bw)
#define             WinGetDepth(win)		((win)->depth)
#define             WinGetVisual(win)		((win)->visual)
#define             WinGetCmap(win)		((win)->cmap)
#define             WinIsShaped(win)		((win)->num_rect != 0)
#else
Window              WinGetXwin(const Win win);
int                 WinGetX(const Win win);
int                 WinGetY(const Win win);
int                 WinGetW(const Win win);
int                 WinGetH(const Win win);
int                 WinGetBorderWidth(const Win win);
int                 WinGetDepth(const Win win);
Visual             *WinGetVisual(const Win win);
Colormap            WinGetCmap(const Win win);
#endif

Win                 ECreateWinFromXwin(Window xwin);
void                EDestroyWin(Win win);

Win                 ERegisterWindow(Window xwin, XWindowAttributes * pxwa);
void                EUnregisterWindow(Win win);
void                EUnregisterXwin(Window xwin);
void                EventCallbackRegister(Win win, int type,
					  EventCallbackFunc * func, void *prm);
void                EventCallbackUnregister(Win win, int type,
					    EventCallbackFunc * func,
					    void *prm);
void                EventCallbacksProcess(Win win, XEvent * ev);

Win                 ECreateWindow(Win parent, int x, int y, int w, int h,
				  int saveunder);
Win                 ECreateArgbWindow(Win parent, int x, int y, int w, int h,
				      Win cwin);
Win                 ECreateWindowVD(Win parent, int x, int y, int w, int h,
				    Visual * vis, unsigned int depth);
Win                 ECreateClientWindow(Win parent, int x, int y, int w, int h);

#define WIN_TYPE_CLIENT     0
#define WIN_TYPE_INTERNAL   1
#define WIN_TYPE_NO_ARGB    2
#define WIN_TYPE_GLX        3
Win                 ECreateObjectWindow(Win parent, int x, int y, int w,
					int h, int saveunder, int type,
					Win cwin);
Win                 ECreateEventWindow(Win parent, int x, int y, int w, int h);
Win                 ECreateFocusWindow(Win parent, int x, int y, int w, int h);
void                EWindowSync(Win win);
void                EWindowSetGeometry(Win win, int x, int y, int w, int h,
				       int bw);
void                EWindowSetMapped(Win win, int mapped);
void                ESelectInputChange(Win win, unsigned long set,
				       unsigned long clear);

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
Pixmap              EGetWindowBackgroundPixmap(Win win);
void                EFreeWindowBackgroundPixmap(Win win);
void                ESetWindowBackground(Win win, unsigned int col);
int                 ETranslateCoordinates(Win src_w, Win dst_w,
					  int src_x, int src_y,
					  int *dest_x_return,
					  int *dest_y_return,
					  Window * child_return);
int                 EDrawableCheck(Drawable draw, int grab);

void                ESelectInput(Win win, unsigned int event_mask);
void                EChangeWindowAttributes(Win win, unsigned int mask,
					    XSetWindowAttributes * attr);
void                ESetWindowBorderWidth(Win win, unsigned int bw);
void                ERaiseWindow(Win win);
void                ELowerWindow(Win win);
void                EClearWindow(Win win);
void                EClearArea(Win win, int x, int y,
			       unsigned int w, unsigned int h);

Pixmap              ECreatePixmap(Win win, unsigned int width,
				  unsigned int height, unsigned int depth);
void                EFreePixmap(Pixmap pixmap);

void                EShapeSetMask(Win win, int x, int y, Pixmap mask);
void                EShapeUnionMask(Win win, int x, int y, Pixmap mask);
void                EShapeSetMaskTiled(Win win, int x, int y, Pixmap mask,
				       int w, int h);
void                EShapeSetRects(Win win, int x, int y,
				   XRectangle * rect, int n_rects);
void                EShapeUnionRects(Win win, int x, int y,
				     XRectangle * rect, int n_rects);
int                 EShapeSetShape(Win win, int x, int y, Win src_win);
int                 EShapePropagate(Win win);
int                 EShapeCheck(Win win);
Pixmap              EWindowGetShapePixmap(Win win);

Bool                EQueryPointer(Win win, int *px, int *py,
				  Window * pchild, unsigned int *pmask);

typedef struct {
   unsigned long       pixel;
   unsigned char       alpha, red, green, blue;
} EColor;

void                EAllocColor(Colormap cmap, EColor * pec);
void                EAllocXColor(Colormap cmap, XColor * pxc, EColor * pec);

#define SET_COLOR(xc, _r, _g, _b) \
    do { (xc)->red = _r; (xc)->green = _g; (xc)->blue = _b; } while(0)

#define GET_COLOR(xc, _r, _g, _b) \
    do { _r = (xc)->red; _g = (xc)->green; _b = (xc)->blue; } while(0)

Window              EXWindowGetParent(Window xwin);
int                 EXGetGeometry(Window xwin, Window * root_return,
				  int *x, int *y, int *w, int *h, int *bw,
				  int *depth);

void                EXRestackWindows(Window * windows, int nwindows);

void                EXCopyArea(Drawable src, Drawable dst, int sx, int sy,
			       unsigned int w, unsigned int h, int dx, int dy);
void                EXCopyAreaTiled(Drawable src, Pixmap mask, Drawable dst,
				    int sx, int sy,
				    unsigned int w, unsigned int h,
				    int dx, int dy);

void                EXWarpPointer(Window xwin, int x, int y);

Pixmap              EXCreatePixmapCopy(Pixmap src, unsigned int w,
				       unsigned int h, unsigned int depth);

GC                  EXCreateGC(Drawable draw, unsigned long mask,
			       XGCValues * val);
int                 EXFreeGC(GC gc);

void                EXSendEvent(Window xwin, long event_mask, XEvent * ev);

KeyCode             EKeysymToKeycode(KeySym keysym);
KeyCode             EKeynameToKeycode(const char *name);
const char         *EKeycodeToString(KeyCode keycode, int index);

Atom                EInternAtom(const char *name);

typedef struct {
   char                type;
   Pixmap              pmap;
   Pixmap              mask;
   int                 w, h;
} PmapMask;

void                FreePmapMask(PmapMask * pmm);

#endif /* _XWIN_H_ */
