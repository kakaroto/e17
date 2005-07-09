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
#include "icons.h"

static int
NetwmIconFindBestSize(unsigned int *val, unsigned int len, int size)
{
   unsigned int        i, j, sj, sbest, sz;
   int                 k = -1;

   sz = (unsigned int)size;
   sj = sbest = 0;
   for (i = 0; i < len;)
     {
	j = i;
	i += 2 + val[i] * val[i + 1];
	if (i > len)
	   break;
	/* Valid */
	sj = val[j];
	if (sj == sz)
	  {
	     k = j;
	     break;		/* First exact match */
	  }
	if (sj > sz)
	  {
	     if (sbest > sz && sj >= sbest)
		continue;
	  }
	else
	  {
	     if (sj <= sbest)
		continue;
	  }
	k = j;
	sbest = sj;
     }

   return k;
}

static Imlib_Image *
IB_SnapEWin(EWin * ewin, int size)
{
   /* Make snapshot of window */
   int                 w, h, ww, hh, scale;
   Imlib_Image        *im;
   Drawable            draw;

   if (!EwinIsMapped(ewin))
      return NULL;

   ww = EoGetW(ewin);
   hh = EoGetH(ewin);
   if (ww <= 0 || hh <= 0)
      return NULL;

   if (ewin->state.shaded)
      EwinInstantUnShade(ewin);
   RaiseEwin(ewin);

   w = h = size;

   /* Oversample for nicer snapshots */
   scale = 4;
   w *= scale;
   h *= scale;

   if (ww > hh)
      h = (w * hh) / ww;
   else
      w = (h * ww) / hh;
   if (w < 4)
      w = 4;
   if (h < 4)
      h = 4;
   if (w > ww || h > hh)
     {
	w = ww;
	h = hh;
     }

#if USE_COMPOSITE
   draw = EoGetPixmap(ewin);
   if (draw != None)
     {
	Pixmap              mask;

	mask = EWindowGetShapePixmap(EoGetWin(ewin));
	imlib_context_set_drawable(draw);
	im = imlib_create_scaled_image_from_drawable(mask, 0, 0, ww, hh,
						     w, h, !EServerIsGrabbed(),
						     0);
	imlib_context_set_image(im);
	imlib_image_set_has_alpha(1);	/* Should be set by imlib? */
     }
   else
#endif
     {
	draw = EoGetWin(ewin);
	imlib_context_set_drawable(draw);
	im = imlib_create_scaled_image_from_drawable(None, 0, 0, ww, hh,
						     w, h, !EServerIsGrabbed(),
						     1);
	imlib_context_set_image(im);
	imlib_image_set_has_alpha(1);	/* Should be set by imlib? */
     }

   return im;
}

static Imlib_Image *
IB_GetAppIcon(EWin * ewin, int size)
{
   /* Get the applications icon pixmap/mask */
   int                 x, y;
   unsigned int        w, h, depth, bw;
   Window              rt;
   Imlib_Image         im;

   if (ewin->ewmh.wm_icon)
     {
	x = NetwmIconFindBestSize(ewin->ewmh.wm_icon, ewin->ewmh.wm_icon_len,
				  size);
	if (x >= 0)
	  {
	     im = imlib_create_image_using_copied_data(ewin->ewmh.wm_icon[x],
						       ewin->ewmh.wm_icon[x +
									  1],
						       ewin->ewmh.wm_icon + x +
						       2);
	     imlib_context_set_image(im);
	     imlib_image_set_has_alpha(1);
	     return im;
	  }
     }

   if (!ewin->client.icon_pmap)
      return NULL;

   w = 0;
   h = 0;
   XGetGeometry(disp, ewin->client.icon_pmap, &rt, &x, &y, &w, &h, &bw, &depth);

   if (w < 1 || h < 1)
      return NULL;

   imlib_context_set_colormap(None);
   imlib_context_set_drawable(ewin->client.icon_pmap);
   im = imlib_create_image_from_drawable(ewin->client.icon_mask, 0, 0, w, h,
					 !EServerIsGrabbed());
   imlib_context_set_image(im);
   imlib_image_set_has_alpha(1);	/* Should be set by imlib? */
   imlib_context_set_colormap(VRoot.cmap);
   imlib_context_set_drawable(VRoot.win);

   return im;
}

static Imlib_Image *
IB_GetEIcon(EWin * ewin)
{
   /* get the icon defined for this window in E's iconf match file */
   const char         *file;
   Imlib_Image        *im;

   file = WindowMatchEwinIcon(ewin);
   if (!file)
      return NULL;

   im = ELoadImage(file);

   return im;
}

#define N_MODES 5
#define N_TYPES 3
static const char   ewin_icon_modes[N_MODES][N_TYPES] = {
   {EWIN_ICON_TYPE_SNAP, EWIN_ICON_TYPE_APP, EWIN_ICON_TYPE_IMG},
   {EWIN_ICON_TYPE_APP, EWIN_ICON_TYPE_IMG, EWIN_ICON_TYPE_SNAP},
   {EWIN_ICON_TYPE_IMG, EWIN_ICON_TYPE_SNAP, EWIN_ICON_TYPE_APP},
   {EWIN_ICON_TYPE_IMG, EWIN_ICON_TYPE_APP, EWIN_ICON_TYPE_NONE},
   {EWIN_ICON_TYPE_APP, EWIN_ICON_TYPE_IMG, EWIN_ICON_TYPE_NONE},
};

Imlib_Image        *
EwinIconImageGet(EWin * ewin, int size, int mode)
{
   Imlib_Image        *im = NULL;
   int                 i, type;

   if (mode < 0 || mode >= N_MODES)
      mode = 4;

   for (i = 0; i < N_TYPES; i++)
     {
	type = ewin_icon_modes[mode][i];
	if (type >= N_TYPES)
	   continue;

	switch (type)
	  {
	  default:
	     goto done;

	  case EWIN_ICON_TYPE_SNAP:
	     im = IB_SnapEWin(ewin, size);
	     break;

	  case EWIN_ICON_TYPE_APP:
	     im = IB_GetAppIcon(ewin, size);
	     break;

	  case EWIN_ICON_TYPE_IMG:
	     im = IB_GetEIcon(ewin);
	     break;
	  }
	if (im)
	   goto done;
     }

 done:
   return im;
}
