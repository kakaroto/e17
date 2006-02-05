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
#include "eobj.h"
#include "ewins.h"
#include "hiwin.h"
#include "iclass.h"
#include "xwin.h"

#define DEBUG_HIWIN 0

struct _hiwin
{
   EObj                o;
   EWin               *ewin;
   int                 zoom;
   int                 xo, yo, wo, ho;
   void                (*evcb) (XEvent * ev, void *data);
   void               *data;
   char                animate;
   GC                  gc;
};

typedef struct
{
   void                (*init) (Hiwin * phi);
   void                (*draw) (Hiwin * phi);
   void                (*fini) (Hiwin * phi, int shown);
} HiwinRender;

static ImageClass  *hiwin_ic = NULL;

static void
HiwinRenderImageInit(Hiwin * phi)
{
   EWin               *ewin = phi->ewin;
   Imlib_Image        *im;
   Pixmap              pmap;

   pmap = EoGetPixmap(ewin);
   if (pmap)
     {
	imlib_context_set_drawable(pmap);
	im = imlib_create_image_from_drawable(0, 0, 0,
					      EoGetW(ewin), EoGetH(ewin), 0);
	/* Skip zoom effect if composite is active */
	phi->animate = 0;
     }
   else if (phi->zoom > 2 && EwinIsOnScreen(ewin))
     {
	imlib_context_set_drawable(EoGetWin(ewin));
	im = imlib_create_image_from_drawable(0, 0, 0,
					      EoGetW(ewin), EoGetH(ewin), 0);
     }
   else
     {
	imlib_context_set_drawable(ewin->mini_pmm.pmap);
	im = imlib_create_image_from_drawable(0, 0, 0,
					      ewin->mini_w, ewin->mini_h, 0);
     }

   imlib_context_set_image(im);
   imlib_context_set_drawable(EoGetWin(phi));
   ESetWindowBackgroundPixmap(EoGetWin(phi), None);
}

static void
HiwinRenderImageDrawX(Hiwin * phi, Drawable draw)
{
   imlib_context_set_drawable(draw);
   imlib_render_image_on_drawable_at_size(0, 0, EoGetW(phi), EoGetH(phi));
}

static void
HiwinRenderImageDraw(Hiwin * phi)
{
   HiwinRenderImageDrawX(phi, EoGetWin(phi));
}

static void
HiwinRenderImageFini(Hiwin * phi, int shown)
{
   Pixmap              pmap;

   if (shown)
     {
	pmap =
	   ECreatePixmap(EoGetWin(phi), EoGetW(phi), EoGetH(phi), VRoot.depth);
	ESetWindowBackgroundPixmap(EoGetWin(phi), pmap);
	HiwinRenderImageDrawX(phi, pmap);
	EFreePixmap(pmap);
	EClearWindow(EoGetWin(phi));
     }
   imlib_free_image_and_decache();
}

#if USE_COMPOSITE
static void
HiwinRenderImageUpdate(Hiwin * phi)
{
   Imlib_Image        *im;
   Pixmap              pmap;
   EWin               *ewin = phi->ewin;

   pmap = EoGetPixmap(ewin);
   if (pmap == None)
      return;

   imlib_context_set_drawable(pmap);
   im = imlib_create_image_from_drawable(0, 0, 0,
					 EoGetW(ewin), EoGetH(ewin), 0);

   imlib_context_set_image(im);
   ESetWindowBackgroundPixmap(EoGetWin(phi), None);
   HiwinRenderImageDrawX(phi, EoGetWin(phi));
   imlib_free_image_and_decache();
}
#endif

static const HiwinRender HiwinRenderImage = {
   HiwinRenderImageInit, HiwinRenderImageDraw, HiwinRenderImageFini
};

static void
HiwinRenderIclassInit(Hiwin * phi __UNUSED__)
{
}

static void
HiwinRenderIclassDraw(Hiwin * phi)
{
   ImageclassApply(hiwin_ic, EoGetWin(phi), EoGetW(phi), EoGetH(phi), 0, 0,
		   STATE_NORMAL, 0, ST_PAGER);
   EClearWindow(EoGetWin(phi));
}

static void
HiwinRenderIclassFini(Hiwin * phi, int shown)
{
   if (shown)
      HiwinRenderIclassDraw(phi);
}

static const HiwinRender HiwinRenderIclass = {
   HiwinRenderIclassInit, HiwinRenderIclassDraw, HiwinRenderIclassFini
};

static void
HiwinRenderPixmapInit(Hiwin * phi)
{
   phi->gc = ECreateGC(EoGetWin(phi), 0, NULL);
}

static void
HiwinRenderPixmapDrawX(Hiwin * phi, Drawable draw)
{
   XSetForeground(disp, phi->gc, BlackPixel(disp, VRoot.scr));
   XFillRectangle(disp, draw, phi->gc, 0, 0, EoGetW(phi), EoGetH(phi));
   XSetForeground(disp, phi->gc, WhitePixel(disp, VRoot.scr));
   XFillRectangle(disp, draw, phi->gc, 1, 1, EoGetW(phi) - 2, EoGetH(phi) - 2);
}

static void
HiwinRenderPixmapDraw(Hiwin * phi)
{
   HiwinRenderPixmapDrawX(phi, EoGetWin(phi));
   EClearWindow(EoGetWin(phi));
}

static void
HiwinRenderPixmapFini(Hiwin * phi, int shown)
{
   Pixmap              pmap;

   if (shown)
     {
	pmap =
	   ECreatePixmap(EoGetWin(phi), EoGetW(phi), EoGetH(phi), VRoot.depth);
	ESetWindowBackgroundPixmap(EoGetWin(phi), pmap);
	HiwinRenderPixmapDrawX(phi, pmap);
	EFreePixmap(pmap);
	EClearWindow(EoGetWin(phi));
     }

   EFreeGC(phi->gc);
   phi->gc = None;
}

static const HiwinRender HiwinRenderPixmap = {
   HiwinRenderPixmapInit, HiwinRenderPixmapDraw, HiwinRenderPixmapFini
};

static void
HiwinEvent(XEvent * ev, void *prm)
{
   Hiwin              *phi = prm;

   if (phi->evcb)
      phi->evcb(ev, phi->data);
}

#if USE_COMPOSITE
static void
HiwinEwinEvent(XEvent * ev, void *prm)
{
   Hiwin              *phi = prm;

#if DEBUG_HIWIN
   Eprintf("HiwinEwinEvent type=%d %s\n", ev->type, EwinGetName(phi->ewin));
#endif

   switch (ev->type)
     {
     case EX_EVENT_DAMAGE_NOTIFY:
	HiwinRenderImageUpdate(phi);
	break;
     }
}
#endif

Hiwin              *
HiwinCreate(void)
{
   Hiwin              *phi;

   phi = Ecalloc(1, sizeof(Hiwin));
   if (!phi)
      return NULL;

   EoInit(phi, EOBJ_TYPE_MISC, None, 0, 0, 3, 3, 1, "HiWin");
   EoSetShadow(phi, 0);
   EoSetFloating(phi, 1);
   EoSetLayer(phi, 19);
   EventCallbackRegister(EoGetWin(phi), 0, HiwinEvent, phi);
   ESelectInput(EoGetWin(phi),
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		EnterWindowMask | LeaveWindowMask);

   return phi;
}

void
HiwinSetGeom(Hiwin * phi, int x, int y, int w, int h)
{
   phi->xo = x;
   phi->yo = y;
   phi->wo = w;
   phi->ho = h;
}

void
HiwinInit(Hiwin * phi, EWin * ewin)
{
   if (ewin == phi->ewin)
      return;

#if USE_COMPOSITE
   if (phi->ewin)
     {
#if DEBUG_HIWIN
	Eprintf("Unregister %s\n", EwinGetName(phi->ewin));
#endif
	EventCallbackUnregister(EoGetWin(phi->ewin), 0, HiwinEwinEvent, phi);
     }
#endif

   phi->ewin = ewin;

#if USE_COMPOSITE
   if (phi->ewin)
     {
#if DEBUG_HIWIN
	Eprintf("Register %s\n", EwinGetName(phi->ewin));
#endif
	EventCallbackRegister(EoGetWin(phi->ewin), 0, HiwinEwinEvent, phi);
     }
#endif

   if (!hiwin_ic)
      hiwin_ic = ImageclassFind("PAGER_WIN", 0);
}

void
HiwinSetCallback(Hiwin * phi, void (*func) (XEvent * ev, void *data),
		 void *data)
{
   phi->evcb = func;
   phi->data = data;
}

void
HiwinGetXY(Hiwin * phi, int *x, int *y)
{
   *x = EoGetX(phi);
   *y = EoGetY(phi);
}

void
HiwinMove(Hiwin * phi, int x, int y)
{
   EoMove(phi, x, y);
}

EWin               *
HiwinGetEwin(Hiwin * phi, int check)
{
   EWin               *ewin;

   if (!phi)
      return NULL;
   if (!check || !phi->ewin)
      return phi->ewin;

   ewin = EwinFindByPtr(phi->ewin);

   return ewin;
}

void
HiwinHide(Hiwin * phi)
{
   if (!phi)
      return;

   if (EoIsShown(phi))
     {
	GrabPointerRelease();
	HiwinInit(phi, NULL);
	EoUnmap(phi);
     }

   phi->data = NULL;
   phi->evcb = NULL;
}

void
HiwinShow(Hiwin * phi, EWin * ewin, int zoom, int confine)
{
   const HiwinRender  *pz;
   int                 x, y, w, h, zold;
   int                 xx, yy, ww, hh, i, i1, i2, step, px, py;

   if (!ewin)
      ewin = phi->ewin;
   if (!ewin)
      return;

   if (ewin->mini_pmm.pmap)
      pz = &HiwinRenderImage;
   else if (hiwin_ic)
      pz = &HiwinRenderIclass;
   else
      pz = &HiwinRenderPixmap;

   if (phi->zoom <= 2 && zoom == 2)
     {
	phi->zoom = 1;

	x = phi->xo + phi->wo / 2;
	y = phi->yo + phi->ho / 2;
	w = zoom * phi->wo;
	h = zoom * phi->ho;

	step = zoom - phi->zoom;
     }
   else if (zoom <= 2)
     {
	x = phi->xo + phi->wo / 2;
	y = phi->yo + phi->ho / 2;
	w = zoom * phi->wo;
	h = zoom * phi->ho;
	step = 0;
     }
   else
     {
	x = VRoot.w / 2;
	y = VRoot.h / 2;
	w = zoom * EoGetW(phi->ewin) / 4;
	h = zoom * EoGetH(phi->ewin) / 4;
	step = 0;
     }

#if DEBUG_HIWIN
   Eprintf("HiwinShow %s zoom=%d->%d step=%d %d,%d %dx%d\n",
	   EoGetName(ewin), phi->zoom, zoom, step, x, y, w, h);
#endif

   zold = phi->zoom;
   phi->zoom = zoom;
   phi->animate = 1;

   pz->init(phi);

   EoMap(phi, 0);

   if (step && phi->animate)
     {
	x = phi->xo;
	y = phi->yo;
	w = phi->wo;
	h = phi->ho;

	if (w > h)
	  {
	     i1 = w * zold;
	     i2 = w * zoom;
	  }
	else
	  {
	     i1 = h * zold;
	     i2 = h * zoom;
	  }

	for (i = i1; i != i2; i += step)
	  {
	     int                 on_screen;

	     if (w > h)
	       {
		  ww = i;
		  hh = (ww * h) / w;
	       }
	     else
	       {
		  hh = i;
		  ww = (hh * w) / h;
	       }
	     xx = x + ((w - ww) / 2);
	     yy = y + ((h - hh) / 2);
	     EoMoveResize(phi, xx, yy, ww, hh);
	     pz->draw(phi);

	     on_screen = EQueryPointer(None, &px, &py, NULL, NULL);
	     if (!on_screen ||
		 (px < x) || (py < y) || (px >= (x + w)) || (py >= (y + h)))
	       {
		  pz->fini(phi, 0);
		  HiwinHide(phi);
		  return;
	       }
	  }
     }
   else
     {
	EoMoveResize(phi, x - w / 2, y - h / 2, w, h);
     }

   GrabPointerSet(EoGetWin(phi), ECSR_ACT_MOVE, confine);

   pz->fini(phi, 1);
}
