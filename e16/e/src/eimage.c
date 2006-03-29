/*
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
#include "eimage.h"
#include "xwin.h"
#include <Imlib2.h>

void
EImageInit(Display * dpy)
{
   imlib_set_cache_size(2048 * 1024);
   imlib_set_font_cache_size(512 * 1024);

   imlib_set_color_usage(128);

   imlib_context_set_display(dpy);
   imlib_context_set_visual(DefaultVisual(dpy, DefaultScreen(dpy)));
   imlib_context_set_colormap(DefaultColormap(dpy, DefaultScreen(dpy)));

   imlib_context_set_dither(1);
}

EImage             *
EImageLoad(const char *file)
{
   EImage             *im;
   char               *f;

   if (!file)
      return NULL;

   if (file[0] == '/')
     {
	im = imlib_load_image(file);
	return im;
     }

   f = ThemeFileFind(file, 0);
   if (f)
     {
	im = imlib_load_image(f);
	Efree(f);
	return im;
     }

   return NULL;
}

void
EImageSave(EImage * im, const char *file)
{
   imlib_context_set_image(im);
   imlib_image_set_format("png");
   imlib_save_image(file);
}

EImage             *
EImageCreate(int w, int h)
{
   EImage             *im;

   im = imlib_create_image(w, h);

   return im;
}

EImage             *
EImageCreateFromData(int w, int h, unsigned int *data)
{
   EImage             *im;

   im = imlib_create_image_using_copied_data(w, h, data);

   return im;
}

EImage             *
EImageCreateScaled(EImage * im, int sx, int sy, int sw, int sh, int dw, int dh)
{
   imlib_context_set_image(im);
   if (sw <= 0)
      sw = imlib_image_get_width();
   if (sh <= 0)
      sh = imlib_image_get_height();
   return imlib_create_cropped_scaled_image(sx, sy, sw, sh, dw, dh);
}

void
EImageFree(EImage * im)
{
   imlib_context_set_image(im);
   imlib_free_image();
}

void
EImageDecache(EImage * im)
{
   imlib_context_set_image(im);
   imlib_free_image_and_decache();
}

static int
_EImageCheckAlpha(void)
{
   static const short  oink = 3;	/* For endianness checking */
   unsigned char      *pb, *pe;

   if (!imlib_image_has_alpha())
      return 0;

   pb = (unsigned char *)imlib_image_get_data_for_reading_only();
   pe = pb + 4 * imlib_image_get_width() * imlib_image_get_height();
   pb += *((char *)(&oink));
   for (; pb < pe; pb += 4)
      if (*pb != 0xff)
	 return 1;

   return 0;
}

void
EImageCheckAlpha(EImage * im)
{
   imlib_context_set_image(im);

   if (imlib_image_has_alpha() && !_EImageCheckAlpha())
     {
#if 0
	Eprintf("Alpha set but no shape %s\n", is->real_file);
#endif
	imlib_image_set_has_alpha(0);
     }
}

void
EImageSetHasAlpha(EImage * im, int has_alpha)
{
   imlib_context_set_image(im);
   imlib_image_set_has_alpha(has_alpha);
}

void
EImageSetBorder(EImage * im, EImageBorder * border)
{
   Imlib_Border        ib;

   ib.left = border->left;
   ib.right = border->right;
   ib.top = border->top;
   ib.bottom = border->bottom;
   imlib_context_set_image(im);
   imlib_image_set_border(&ib);
}

int
EImageHasAlpha(EImage * im)
{
   imlib_context_set_image(im);
   return imlib_image_has_alpha();
}

void
EImageGetSize(EImage * im, int *pw, int *ph)
{
   imlib_context_set_image(im);
   *pw = imlib_image_get_width();
   *ph = imlib_image_get_height();
}

void
EImageFill(EImage * im, int x, int y, int w, int h, int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_context_set_blend(0);
   imlib_image_fill_rectangle(x, y, w, h);
}

void
EImageOrientate(EImage * im, int orientation)
{
   imlib_context_set_image(im);
   imlib_image_orientate(orientation);
}

void
EImageBlend(EImage * im, EImage * src, int blend,
	    int sx, int sy, int sw, int sh,
	    int dx, int dy, int dw, int dh, int merge_alpha, int anti_alias)
{
   imlib_context_set_image(im);
   if (anti_alias)
      imlib_context_set_anti_alias(1);
   if (blend)
      imlib_context_set_blend(1);
   imlib_blend_image_onto_image(src, merge_alpha, sx, sy, sw, sh,
				dx, dy, dw, dh);
   if (blend)
      imlib_context_set_blend(0);
   if (anti_alias)
      imlib_context_set_anti_alias(0);
}

void
EImageBlendCM(EImage * im, EImage * src, EImageColorModifier * icm)
{
   int                 w, h, iw, ih;

   imlib_context_set_image(src);
   iw = imlib_image_get_width();
   ih = imlib_image_get_height();
   imlib_context_set_image(im);
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   imlib_context_set_blend(1);
   if (icm)
      imlib_context_set_color_modifier(icm);
   imlib_context_set_operation(IMLIB_OP_COPY);
   imlib_blend_image_onto_image(src, 0, 0, 0, iw, ih, 0, 0, w, h);
   imlib_context_set_blend(0);
   if (icm)
      imlib_context_set_color_modifier(NULL);
}

void
EImageTile(EImage * im, EImage * tile, int blend, int tw, int th,
	   int dx, int dy, int dw, int dh, int ox, int oy)
{
   Imlib_Image        *tim;
   int                 x, y, tx, ty, ww, hh;
   int                 sw, sh;

   imlib_context_set_image(tile);
   sw = imlib_image_get_width();
   sh = imlib_image_get_height();
   if (sw == tw && sh == th)
     {
	tim = tile;
     }
   else
     {
	tim = imlib_create_image(tw, th);
	imlib_context_set_image(tim);
	imlib_context_set_blend(0);
	imlib_blend_image_onto_image(tile, 0, 0, 0, sw, sh, 0, 0, tw, th);
     }
   imlib_context_set_image(im);

   if (ox)
     {
	ox = tw - ox;
	ox %= tw;
	if (ox < 0)
	   ox += tw;
     }
   if (oy)
     {
	oy = th - oy;
	oy %= th;
	if (oy < 0)
	   oy += th;
     }
   dw += dx;
   dh += dy;
   y = dy;
   ty = oy;
   hh = th - oy;
   for (;;)
     {
	if (y + hh >= dh)
	   hh = dh - y;
	if (hh <= 0)
	   break;
	x = dx;
	tx = ox;
	ww = tw - ox;
	for (;;)
	  {
	     if (x + ww >= dw)
		ww = dw - x;
	     if (ww <= 0)
		break;
	     imlib_blend_image_onto_image(tim, blend, tx, ty, ww, hh,
					  x, y, ww, hh);
	     tx = 0;
	     x += ww;
	     ww = tw;
	  }
	ty = 0;
	y += hh;
	hh = th;
     }
   if (tim != tile)
     {
	imlib_context_set_image(tim);
	imlib_free_image();
	imlib_context_set_image(im);	/* FIXME - Remove */
     }
}

EImage             *
EImageGrabDrawable(Drawable draw, Pixmap mask, int x, int y, int w, int h,
		   int grab)
{
   EImage             *im;

   imlib_context_set_drawable(draw);
   im = imlib_create_image_from_drawable(mask, x, y, w, h, grab);

   return im;
}

EImage             *
EImageGrabDrawableScaled(Drawable draw, Pixmap mask, int x, int y, int w, int h,
			 int iw, int ih, int grab, int get_mask_from_shape)
{
   EImage             *im;

   imlib_context_set_drawable(draw);
   im =
      imlib_create_scaled_image_from_drawable(mask, x, y, w, h, iw, ih, grab,
					      get_mask_from_shape);

   return im;
}

void
EImageRenderOnDrawable(EImage * im, Drawable draw, int x, int y, int w, int h,
		       int blend)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(draw);
   if (blend)
      imlib_context_set_blend(1);
   imlib_render_image_on_drawable_at_size(x, y, w, h);
   if (blend)
      imlib_context_set_blend(0);
}

void
EImageRenderPixmaps(EImage * im, Drawable draw, Pixmap * pmap, Pixmap * mask,
		    int w, int h)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(draw);

   *pmap = *mask = None;
   if (w <= 0 || h <= 0)
      imlib_render_pixmaps_for_whole_image(pmap, mask);
   else
      imlib_render_pixmaps_for_whole_image_at_size(pmap, mask, w, h);
}

void
EImagePixmapFree(Pixmap pmap)
{
   imlib_free_pixmap_and_mask(pmap);
}

void
ScaleRect(Window src, Pixmap dst, Pixmap * pdst, int sx, int sy, int sw, int sh,
	  int dx, int dy, int dw, int dh, int scale)
{
   Imlib_Image        *im;
   Pixmap              pmap, mask;

   scale = (scale) ? 2 : 1;

   imlib_context_set_drawable(src);
   im = imlib_create_scaled_image_from_drawable(None, sx, sy, sw, sh,
						scale * dw, scale * dh, 0, 0);
   imlib_context_set_image(im);
   imlib_context_set_anti_alias(1);
   if (pdst)
     {
	imlib_render_pixmaps_for_whole_image_at_size(&pmap, &mask, dw, dh);
	*pdst = pmap;
     }
   else
     {
	imlib_context_set_drawable(dst);
	imlib_render_image_on_drawable_at_size(dx, dy, dw, dh);
     }
   imlib_free_image();
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

EImageColorModifier *
EImageColorModifierCreate(unsigned char *r, unsigned char *g, unsigned char *b,
			  unsigned char *a)
{
   EImageColorModifier *icm;

   icm = imlib_create_color_modifier();
   if (!icm)
      return NULL;
   imlib_context_set_color_modifier(icm);
#if 0				/* Useful in this context? */
   imlib_modify_color_modifier_gamma(0.5);
   imlib_modify_color_modifier_brightness(0.5);
   imlib_modify_color_modifier_contrast(0.5);
#endif
   imlib_set_color_modifier_tables(r, g, b, a);
   imlib_context_set_color_modifier(NULL);

   return icm;
}
