/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
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
#include "ewins.h"
#include "hiwin.h"
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
};

typedef struct
{
   void                (*init) (Hiwin * phi, void *data);
   void                (*draw) (Hiwin * phi, void *data);
   void                (*fini) (Hiwin * phi, void *data, int shown);
} HiwinRender;

static void
HiwinRenderImageInit(Hiwin * phi, void *data)
{
   EWin               *ewin = data;
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
HiwinRenderImageDraw(Hiwin * phi, void *data __UNUSED__)
{
   imlib_render_image_on_drawable_at_size(0, 0, EoGetW(phi), EoGetH(phi));
}

static void
HiwinRenderImageFini(Hiwin * phi, void *data __UNUSED__, int shown)
{
   Pixmap              pmap;

   if (shown)
     {
	pmap =
	   ECreatePixmap(EoGetWin(phi), EoGetW(phi), EoGetH(phi), VRoot.depth);
	imlib_context_set_drawable(pmap);
	imlib_render_image_on_drawable_at_size(0, 0, EoGetW(phi), EoGetH(phi));
	ESetWindowBackgroundPixmap(EoGetWin(phi), pmap);
	EFreePixmap(pmap);
	EClearWindow(EoGetWin(phi));
     }
   imlib_free_image_and_decache();
}

static const HiwinRender HiwinRenderImage = {
   HiwinRenderImageInit, HiwinRenderImageDraw, HiwinRenderImageFini
};

static void
HiwinRenderIclassInit(Hiwin * phi __UNUSED__, void *data __UNUSED__)
{
}

static void
HiwinRenderIclassDraw(Hiwin * phi, void *data)
{
   ImageclassApply(data, EoGetWin(phi), EoGetW(phi), EoGetH(phi), 0, 0,
		   STATE_NORMAL, 0, ST_PAGER);
   EClearWindow(EoGetWin(phi));
}

static void
HiwinRenderIclassFini(Hiwin * phi __UNUSED__, void *data __UNUSED__,
		      int shown __UNUSED__)
{
   if (shown)
      HiwinRenderIclassDraw(phi, data);
}

static const HiwinRender HiwinRenderIclass = {
   HiwinRenderIclassInit, HiwinRenderIclassDraw, HiwinRenderIclassFini
};

typedef struct
{
   GC                  gc;
} HiwinRenderPixmapData;

static void
HiwinRenderPixmapInit(Hiwin * phi __UNUSED__, void *data)
{
   HiwinRenderPixmapData *pd = data;

   pd->gc = ECreateGC(EoGetWin(phi), 0, NULL);
}

static void
HiwinRenderPixmapDraw(Hiwin * phi, void *data)
{
   HiwinRenderPixmapData *pd = data;

   XSetForeground(disp, pd->gc, BlackPixel(disp, VRoot.scr));
   XFillRectangle(disp, EoGetWin(phi), pd->gc, 0, 0, EoGetW(phi), EoGetH(phi));
   XSetForeground(disp, pd->gc, WhitePixel(disp, VRoot.scr));
   XFillRectangle(disp, EoGetWin(phi), pd->gc, 1, 1, EoGetW(phi) - 2,
		  EoGetH(phi) - 2);
   EClearWindow(EoGetWin(phi));
}

static void
HiwinRenderPixmapFini(Hiwin * phi __UNUSED__, void *data, int shown __UNUSED__)
{
   HiwinRenderPixmapData *pd = data;
   Pixmap              pmap;

   if (shown)
     {
	pmap =
	   ECreatePixmap(EoGetWin(phi), EoGetW(phi), EoGetH(phi), VRoot.depth);
	XSetForeground(disp, pd->gc, BlackPixel(disp, VRoot.scr));
	XFillRectangle(disp, pmap, pd->gc, 0, 0, EoGetW(phi), EoGetH(phi));
	XSetForeground(disp, pd->gc, WhitePixel(disp, VRoot.scr));
	XFillRectangle(disp, pmap, pd->gc, 1, 1, EoGetW(phi) - 2,
		       EoGetH(phi) - 2);
	ESetWindowBackgroundPixmap(EoGetWin(phi), pmap);
	EFreePixmap(pmap);
	EClearWindow(EoGetWin(phi));
     }

   EFreeGC(pd->gc);
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

Hiwin              *
HiwinCreate(void)
{
   Hiwin              *phi;

   phi = Ecalloc(1, sizeof(Hiwin));
   if (!phi)
      return NULL;

   EobjInit(EoObj(phi), EOBJ_TYPE_MISC, None, 0, 0, 3, 3, 1, "HiWin");
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
   phi->ewin = ewin;
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
   if (!ewin)
      phi->ewin = NULL;

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
	EoUnmap(phi);
     }

   phi->ewin = NULL;
   phi->data = NULL;
   phi->evcb = NULL;
}

void
HiwinShow(Hiwin * phi, EWin * ewin, int zoom, int confine)
{
   const HiwinRender  *pz;
   int                 x, y, w, h, zold;
   int                 xx, yy, ww, hh, i, i1, i2, step, px, py;
   XID                 pzd[2];
   void               *data;
   ImageClass         *ic;

   if (!ewin)
      ewin = phi->ewin;
   if (!ewin)
      return;

   if (ewin->mini_pmm.pmap)
     {
	pz = &HiwinRenderImage;
	data = ewin;
     }
   else
     {
	ic = ImageclassFind("PAGER_WIN", 0);
	if (ic)
	  {
	     pz = &HiwinRenderIclass;
	     data = ic;
	  }
	else
	  {
	     pz = &HiwinRenderPixmap;
	     data = pzd;
	  }
     }

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

   pz->init(phi, data);

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
	     pz->draw(phi, data);

	     PointerAt(&px, &py);
	     if ((px < x) || (py < y) || (px >= (x + w)) || (py >= (y + h)))
	       {
		  pz->fini(phi, data, 0);
		  EoUnmap(phi);
		  return;
	       }
	  }
     }
   else
     {
	EoMoveResize(phi, x - w / 2, y - h / 2, w, h);
     }

   GrabPointerSet(EoGetWin(phi), ECSR_ACT_MOVE, confine);

   pz->fini(phi, data, 1);
}
