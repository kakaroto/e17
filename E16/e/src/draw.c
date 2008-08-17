/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2007-2008 Kim Woelders
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
#include "eobj.h"
#include "ewins.h"
#include "piximg.h"
#include "xwin.h"

#if 0
#include <X11/bitmaps/gray>
#include <X11/bitmaps/gray3>
#else
/* Include contents of X11/bitmaps/gray+gray3.
 * (avoid build failure if x11 bitmap package isn't installed) */

#define gray_width 2
#define gray_height 2
static const char   gray_bits[] = { 0x01, 0x02 };

#define gray3_width 4
#define gray3_height 4
static const char   gray3_bits[] = { 0x01, 0x00, 0x04, 0x00 };
#endif

typedef struct {
   EObj                o;
   Pixmap              mask;
   GC                  gc;
} ShapeWin;

static Font         font = None;	/* Used in mode 1 (technical) */

#define DRAW_H_ARROW(_dr, _gc, x1, x2, y1) \
    if (((x2) - (x1)) >= 12) \
      { \
        XDrawLine(disp, _dr, _gc, (x1), (y1), (x1) + 6, (y1) - 3); \
        XDrawLine(disp, _dr, _gc, (x1), (y1), (x1) + 6, (y1) + 3); \
        XDrawLine(disp, _dr, _gc, (x2), (y1), (x2) - 6, (y1) - 3); \
        XDrawLine(disp, _dr, _gc, (x2), (y1), (x2) - 6, (y1) + 3); \
      } \
    if ((x2) >= (x1)) \
      { \
        XDrawLine(disp, _dr, _gc, (x1), (y1), (x2), (y1)); \
        Esnprintf(str, sizeof(str), "%i", (x2) - (x1) + 1); \
        XDrawString(disp, _dr, _gc, ((x1) + (x2)) / 2, (y1) - 10, str, strlen(str)); \
      }
#define DRAW_V_ARROW(_dr, _gc, y1, y2, x1) \
    if (((y2) - (y1)) >= 12) \
      { \
        XDrawLine(disp, _dr, _gc, (x1), (y1), (x1) + 3, (y1) + 6); \
        XDrawLine(disp, _dr, _gc, (x1), (y1), (x1) - 3, (y1) + 6); \
        XDrawLine(disp, _dr, _gc, (x1), (y2), (x1) + 3, (y2) - 6); \
        XDrawLine(disp, _dr, _gc, (x1), (y2), (x1) - 3, (y2) - 6); \
      } \
    if ((y2) >= (y1)) \
      { \
        XDrawLine(disp, _dr, _gc, (x1), (y1), (x1), (y2)); \
        Esnprintf(str, sizeof(str), "%i", (y2) - (y1) + 1); \
        XDrawString(disp, _dr, _gc, (x1) + 10, ((y1) + (y2)) / 2, str, strlen(str)); \
      }

#define DO_DRAW_MODE_1(_dr, _gc, _a, _b, _c, _d) \
  do { \
    if (!font) \
      font = XLoadFont(disp, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"); \
    XSetFont(disp, _gc, font); \
    if (_c < 3) _c = 3; \
    if (_d < 3) _d = 3; \
    DRAW_H_ARROW(_dr, _gc, _a + bl, _a + bl + _c - 1, _b + bt + _d - 16); \
    DRAW_H_ARROW(_dr, _gc, 0, _a - 1, _b + bt + (_d / 2)); \
    DRAW_H_ARROW(_dr, _gc, _a + _c + bl + br, WinGetW(VROOT) - 1, _b + bt + (_d / 2)); \
    DRAW_V_ARROW(_dr, _gc, _b + bt, _b + bt + _d - 1, _a + bl + 16); \
    DRAW_V_ARROW(_dr, _gc, 0, _b - 1, _a + bl + (_c / 2)); \
    DRAW_V_ARROW(_dr, _gc, _b + _d + bt + bb, WinGetH(VROOT) - 1, _a + bl + (_c / 2)); \
    XDrawLine(disp, _dr, _gc, _a, 0, _a, WinGetH(VROOT)); \
    XDrawLine(disp, _dr, _gc, _a + _c + bl + br - 1, 0, _a + _c + bl + br - 1, WinGetH(VROOT)); \
    XDrawLine(disp, _dr, _gc, 0, _b, WinGetW(VROOT), _b); \
    XDrawLine(disp, _dr, _gc, 0, _b + _d + bt + bb - 1, WinGetW(VROOT), _b + _d + bt + bb - 1); \
    XDrawRectangle(disp, _dr, _gc, _a + bl + 1, _b + bt + 1, _c - 3, _d - 3); \
  } while(0)

#define DO_DRAW_MODE_2(_dr, _gc, _a, _b, _c, _d) \
  do { \
    if (_c < 3) _c = 3; \
    if (_d < 3) _d = 3; \
    XDrawRectangle(disp, _dr, _gc, _a, _b, _c + bl + br - 1, _d + bt + bb - 1); \
    XDrawRectangle(disp, _dr, _gc, _a + bl + 1, _b + bt + 1, _c - 3, _d - 3); \
  } while(0)

#define DO_DRAW_MODE_3(_dr, _gc, _a, _b, _c, _d) \
  do { \
    XSetFillStyle(disp, _gc, FillStippled); \
    XSetStipple(disp, _gc, b2); \
    if ((_c + bl + br > 0) && (bt > 0)) \
      XFillRectangle(disp, _dr, _gc, _a, _b, _c + bl + br, bt); \
    if ((_c + bl + br > 0) && (bb > 0)) \
      XFillRectangle(disp, _dr, _gc, _a, _b + _d + bt, _c + bl + br, bb); \
    if ((_d > 0) && (bl > 0)) \
      XFillRectangle(disp, _dr, _gc, _a, _b + bt, bl, _d); \
    if ((_d > 0) && (br > 0)) \
      XFillRectangle(disp, _dr, _gc, _a + _c + bl, _b + bt, br, _d); \
    XSetStipple(disp, _gc, b3); \
    if ((_c > 0) && (_d > 0)) \
      XFillRectangle(disp, _dr, _gc, _a + bl + 1, _b + bt + 1, _c - 3, _d - 3); \
  } while(0)

#define DO_DRAW_MODE_4(_dr, _gc, _a, _b, _c, _d) \
  do { \
    XSetFillStyle(disp, _gc, FillStippled); \
    XSetStipple(disp, _gc, b2); \
    XFillRectangle(disp, _dr, _gc, _a, _b, _c + bl + br, _d + bt + bb); \
  } while(0)

#define _SHAPE_SET_RECT(rl, _x, _y, _w, _h) \
  do { \
    rl[0].x = (_x);        rl[0].y = (_y);        rl[0].width = (_w); rl[0].height = 1; \
    rl[1].x = (_x);        rl[1].y = (_y)+(_h)-1; rl[1].width = (_w); rl[1].height = 1; \
    rl[2].x = (_x);        rl[2].y = (_y)+1;      rl[2].width = 1;    rl[2].height = (_h)-2; \
    rl[3].x = (_x)+(_w)-1; rl[3].y = (_y)+1;      rl[3].width = 1;    rl[3].height = (_h)-2; \
  } while(0)

#define _R(x) (((x) >> 16) & 0xff)
#define _G(x) (((x) >>  8) & 0xff)
#define _B(x) (((x)      ) & 0xff)

static unsigned int
_ShapeGetColor(void)
{
   static char         color_valid = 0;
   static unsigned int color_value = 0;
   static unsigned int color_pixel;
   EColor              color;

   if (color_valid && color_value == Conf.movres.color)
      goto done;

   color_value = Conf.movres.color;
   SET_COLOR(&color, _R(color_value), _G(color_value), _B(color_value));
   EAllocColor(WinGetCmap(VROOT), &color);
   color_pixel = color.pixel;
   color_valid = 1;

 done:
   return color_pixel;
}

static void
_ShapeWinDestroy(ShapeWin * sw)
{
   EoUnmap(sw);
   EoFini(sw);
   if (sw->gc)
      EXFreeGC(sw->gc);
   if (sw->mask != None)
      EFreePixmap(sw->mask);
   Efree(sw);
}

static ShapeWin    *
_ShapeWinCreate(int md)
{
   ShapeWin           *sw;

   sw = ECALLOC(ShapeWin, 1);
   if (!sw)
      return NULL;

   EoInit(sw, EOBJ_TYPE_MISC, None,
	  0, 0, WinGetW(VROOT), WinGetH(VROOT), 2, "Wires");
   if (!EoGetWin(sw))
      goto bail_out;

   EoSetFloating(sw, 1);
   EoSetLayer(sw, 18);
   ESetWindowBackground(EoGetWin(sw), _ShapeGetColor());
#ifdef ShapeInput		/* Should really check server too */
   XShapeCombineRectangles(disp, EoGetXwin(sw),
			   ShapeInput, 0, 0, NULL, 0, ShapeSet, Unsorted);
#endif

   if (md == 1)
     {
	sw->mask =
	   ECreatePixmap(EoGetWin(sw), WinGetW(VROOT), WinGetH(VROOT), 1);
	sw->gc = EXCreateGC(sw->mask, 0, NULL);
	if (sw->mask == None || !sw->gc)
	   goto bail_out;
     }

   return sw;

 bail_out:
   _ShapeWinDestroy(sw);
   return NULL;
}

static void
_ShapeSet(ShapeWin * sw, int md, int x, int y, int w, int h,
	  int bl, int br, int bt, int bb, int seqno)
{
   int                 w2, h2;

   w2 = w + bl + br;
   h2 = h + bt + bb;

   if (md == 1)
     {
	char                str[32];

	XSetForeground(disp, sw->gc, 0);
	XFillRectangle(disp, sw->mask, sw->gc,
		       0, 0, WinGetW(VROOT), WinGetH(VROOT));
	XSetForeground(disp, sw->gc, 1);
	DO_DRAW_MODE_1(sw->mask, sw->gc, x, y, w, h);
	if (seqno == 0)
	   EShapeSetMask(EoGetWin(sw), 0, 0, sw->mask);
	else
	   EShapeUnionMask(EoGetWin(sw), 0, 0, sw->mask);
     }
   else
     {
	XRectangle          rl[8];

	_SHAPE_SET_RECT((&rl[0]), x, y, w2, h2);
	w = (w > 5) ? w - 2 : 3;
	h = (h > 5) ? h - 2 : 3;
	_SHAPE_SET_RECT((&rl[4]), x + bl + 1, y + bt + 1, w, h);

	if (seqno == 0)
	   EShapeSetRects(EoGetWin(sw), 0, 0, rl, 8);
	else
	   EShapeUnionRects(EoGetWin(sw), 0, 0, rl, 8);
     }
   EoShapeUpdate(sw, 0);
}

static PixImg      *root_pi = NULL;
static PixImg      *ewin_pi = NULL;
static PixImg      *draw_pi = NULL;

static void
_PixImgsCreate(Window root, const EWin * ewin)
{
   root_pi = ECreatePixImg(root, WinGetW(VROOT), WinGetH(VROOT));
   ewin_pi = ECreatePixImg(root, EoGetW(ewin), EoGetH(ewin));
   draw_pi = ECreatePixImg(root, EoGetW(ewin), EoGetH(ewin));
}

static void
_PixImgsDestroy(void)
{
   EDestroyPixImg(root_pi);
   EDestroyPixImg(ewin_pi);
   EDestroyPixImg(draw_pi);
   EBlendRemoveShape(NULL, 0, 0, 0);
   EBlendPixImg(NULL, NULL, NULL, NULL, 0, 0, 0, 0);
   root_pi = NULL;
   ewin_pi = NULL;
   draw_pi = NULL;
}

void
DrawEwinShape(EWin * ewin, int md, int x, int y, int w, int h,
	      int firstlast, int seqno)
{
   static GC           gc = 0;
   static Pixmap       b2 = 0, b3 = 0;
   Window              root = WinGetXwin(VROOT);
   int                 x1, y1, w1, h1, dx, dy;
   int                 bl, br, bt, bb;
   char                str[32];

   /* Quit if no change */
   if (firstlast == 1 &&
       (x == ewin->shape_x && y == ewin->shape_y &&
	(ewin->state.shaded || (w == ewin->shape_w && h == ewin->shape_h))))
      return;

   switch (md)
     {
     case 0:
	EwinOpMoveResize(ewin, OPSRC_USER, x, y, w, h);
	EwinShapeSet(ewin);
	CoordsShow(ewin);
	goto done;
     case 1:
     case 2:
	break;
     case 3:
     case 4:
	if (!b2)
	   b2 = XCreateBitmapFromData(disp, root, gray_bits, gray_width,
				      gray_height);
	if (!b3)
	   b3 = XCreateBitmapFromData(disp, root, gray3_bits, gray3_width,
				      gray3_height);
	break;
     case 5:
	break;
     }

   if (firstlast == 0)
      EwinShapeSet(ewin);

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

   if (!ewin->state.shaded)
     {
	ewin->shape_w = w;
	ewin->shape_h = h;
     }
   else
     {
	w = ewin->shape_w;
	h = ewin->shape_h;
     }

   EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);

   if (md <= 2 && Conf.movres.avoid_server_grab)
     {
	static ShapeWin    *shape_win = NULL;

	if (firstlast == 0 && !shape_win)
	   shape_win = _ShapeWinCreate(md);
	if (!shape_win)
	   return;

	_ShapeSet(shape_win, md, x, y, w, h, bl, br, bt, bb, seqno);
	EoMap(shape_win, 0);

	CoordsShow(ewin);

	if (firstlast == 2)
	  {
	     _ShapeWinDestroy(shape_win);
	     shape_win = NULL;
	  }
	goto done;
     }

   if (!gc)
     {
	XGCValues           gcv;

	gcv.function = GXxor;
	gcv.foreground = Dpy.pixel_white;
	if (gcv.foreground == 0)
	   gcv.foreground = Dpy.pixel_black;
	gcv.subwindow_mode = IncludeInferiors;
	gc = EXCreateGC(root,
			GCFunction | GCForeground | GCSubwindowMode, &gcv);
     }

   switch (md)
     {
     case 1:
	if (firstlast > 0)
	   DO_DRAW_MODE_1(root, gc, x1, y1, w1, h1);
	CoordsShow(ewin);
	if (firstlast < 2)
	   DO_DRAW_MODE_1(root, gc, x, y, w, h);
	break;
     case 2:
	if (firstlast > 0)
	   DO_DRAW_MODE_2(root, gc, x1, y1, w1, h1);
	CoordsShow(ewin);
	if (firstlast < 2)
	   DO_DRAW_MODE_2(root, gc, x, y, w, h);
	break;
     case 3:
	if (firstlast > 0)
	   DO_DRAW_MODE_3(root, gc, x1, y1, w1, h1);
	CoordsShow(ewin);
	if (firstlast < 2)
	   DO_DRAW_MODE_3(root, gc, x, y, w, h);
	break;
     case 4:
	if (firstlast > 0)
	   DO_DRAW_MODE_4(root, gc, x1, y1, w1, h1);
	CoordsShow(ewin);
	if (firstlast < 2)
	   DO_DRAW_MODE_4(root, gc, x, y, w, h);
	break;
     case 5:
	{
	   if (firstlast == 0)
	     {
		GC                  gc2;

		_PixImgsDestroy();
		_PixImgsCreate(root, ewin);
		if ((!root_pi) || (!ewin_pi) || (!draw_pi))
		  {
		     _PixImgsDestroy();
		     Conf.movres.mode_move = 0;
		     EUngrabServer();
		     DrawEwinShape(ewin, Conf.movres.mode_move, x, y, w, h,
				   firstlast, seqno);
		     return;
		  }
		EFillPixmap(root, root_pi->pmap, x1, y1, EoGetW(ewin),
			    EoGetH(ewin));
		gc2 = EXCreateGC(root_pi->pmap, 0, NULL);
		XCopyArea(disp, root_pi->pmap, ewin_pi->pmap, gc2, x1, y1,
			  EoGetW(ewin), EoGetH(ewin), 0, 0);
		EXFreeGC(gc2);
		EBlendPixImg(EoGetWin(ewin), root_pi, ewin_pi, draw_pi, x, y,
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
		     EBlendPixImg(EoGetWin(ewin), root_pi, ewin_pi, draw_pi,
				  x, y, EoGetW(ewin), EoGetH(ewin));
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
		     EBlendPixImg(EoGetWin(ewin), root_pi, ewin_pi, draw_pi,
				  x, y, EoGetW(ewin), EoGetH(ewin));
		  }
		EBlendRemoveShape(EoGetWin(ewin), root_pi->pmap, x, y);
	     }
	   else if (firstlast == 2)
	     {
		EPastePixmap(root, root_pi->pmap, x1, y1, EoGetW(ewin),
			     EoGetH(ewin));
		_PixImgsDestroy();
	     }
	   else if (firstlast == 3)
	     {
		EPastePixmap(root, root_pi->pmap, x, y, EoGetW(ewin),
			     EoGetH(ewin));
		if (root_pi)
		   EDestroyPixImg(root_pi);
		root_pi = NULL;
	     }
	   else if (firstlast == 4)
	     {
		int                 wt, ht;

		wt = EoGetW(ewin);
		ht = EoGetH(ewin);
		root_pi = ECreatePixImg(root, WinGetW(VROOT), WinGetH(VROOT));
		EFillPixmap(root, root_pi->pmap, x, y, wt, ht);
		EBlendPixImg(EoGetWin(ewin), root_pi, ewin_pi, draw_pi, x, y,
			     EoGetW(ewin), EoGetH(ewin));
	     }
	   CoordsShow(ewin);
	}
	break;
     }

   if (firstlast == 2)
     {
	EXFreeGC(gc);
	gc = 0;
     }

 done:
   if (firstlast == 0 || firstlast == 2 || firstlast == 4)
     {
	ewin->req_x = ewin->shape_x;
	ewin->req_y = ewin->shape_y;
	if (firstlast == 2)
	   CoordsHide();
     }
}
