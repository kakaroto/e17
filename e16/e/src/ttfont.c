/*
 * Copyright (C) 1999 Carsten Haitzler, Geoff Harrison and various contributors
 * *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 * *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

typedef struct _efont_color_tab EfontColorTable;

struct _efont_color_tab
{
   Colormap            cmap;

   XColor              list[256];
   unsigned char       match[8][8][8];
};

/*static EfontColorTable *color_tab = NULL; */
static unsigned char alpha_lut[5] = { 0, 64, 128, 192, 255 };
static unsigned char bounded_palette[9] = { 0, 1, 2, 3, 4, 4, 4, 4, 4 };

static TT_Raster_Map *
create_font_raster(int width, int height)
{
   TT_Raster_Map      *rmap;

   rmap = Emalloc(sizeof(TT_Raster_Map));
   rmap->width = (width + 3) & -4;
   rmap->rows = height;
   rmap->flow = TT_Flow_Down;
   rmap->cols = rmap->width;
   rmap->size = rmap->rows * rmap->width;
   if (rmap->size > 0)
     {
	rmap->bitmap = Emalloc(rmap->size);
	memset(rmap->bitmap, 0, rmap->size);
     }
   else
      rmap->bitmap = NULL;
   return rmap;
}

static TT_Raster_Map *
duplicate_raster(TT_Raster_Map * rmap)
{
   TT_Raster_Map      *new_rmap;

   new_rmap = Emalloc(sizeof(TT_Raster_Map));
   *new_rmap = *rmap;
   if (new_rmap->size > 0)
     {
	new_rmap->bitmap = Emalloc(new_rmap->size);
	memcpy(new_rmap->bitmap, rmap->bitmap, new_rmap->size);
     }
   else
      new_rmap->bitmap = NULL;
   return new_rmap;
}

static void
clear_raster(TT_Raster_Map * rmap)
{
   memset(rmap->bitmap, 0, rmap->size);
}

static void
destroy_font_raster(TT_Raster_Map * rmap)
{
   if (!rmap)
      return;
   if (rmap->bitmap)
      Efree(rmap->bitmap);
   Efree(rmap);
}

static TT_Raster_Map *
calc_size(Efont * f, int *width, int *height, char *text)
{
   int                 i, upm, ascent, descent, pw, ph;
   TT_Instance_Metrics imetrics;
   TT_Glyph_Metrics    gmetrics;
   TT_Raster_Map      *rtmp;

   TT_Get_Instance_Metrics(f->instance, &imetrics);
   upm = f->properties.header->Units_Per_EM;
   ascent = (f->properties.horizontal->Ascender * imetrics.y_ppem) / upm;
   descent = (f->properties.horizontal->Descender * imetrics.y_ppem) / upm;
   if (descent < 0)
      descent = -descent;
   pw = 0;
   ph = ((f->max_ascent) - f->max_descent) / 64;

   for (i = 0; text[i]; i++)
     {
	unsigned char       j = text[i];

	if (!TT_VALID(f->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(f->glyphs[j], &gmetrics);
	if (i == 0)
	  {
	     pw += ((-gmetrics.bearingX) / 64);
	  }
	if (text[i + 1] == 0)
	  {
	     pw += (gmetrics.bbox.xMax / 64);
	  }
	else
	   pw += gmetrics.advance / 64;
     }
   *width = pw;
   *height = ph;

   rtmp = create_font_raster(imetrics.x_ppem + 32, imetrics.y_ppem + 32);
   rtmp->flow = TT_Flow_Up;
   return rtmp;
}

static void
render_text(TT_Raster_Map * rmap, TT_Raster_Map * rchr, Efont * f, char *text,
	    int *xor, int *yor)
{
   TT_Glyph_Metrics    metrics;
   TT_Instance_Metrics imetrics;
   TT_F26Dot6          x, y, xmin, ymin, xmax, ymax;
   int                 i, ioff, iread;
   char               *off, *read, *_off, *_read;
   int                 x_offset, y_offset;
   unsigned char       j;
   TT_Raster_Map      *rtmp;

   TT_Get_Instance_Metrics(f->instance, &imetrics);

   j = text[0];
   TT_Get_Glyph_Metrics(f->glyphs[j], &metrics);
   x_offset = (-metrics.bearingX) / 64;

   y_offset = -(f->max_descent / 64);

   *xor = x_offset;
   *yor = rmap->rows - y_offset;

   rtmp = NULL;
   for (i = 0; text[i]; i++)
     {
	j = text[i];

	if (!TT_VALID(f->glyphs[j]))
	   continue;

	TT_Get_Glyph_Metrics(f->glyphs[j], &metrics);

	xmin = metrics.bbox.xMin & -64;
	ymin = metrics.bbox.yMin & -64;
	xmax = (metrics.bbox.xMax + 63) & -64;
	ymax = (metrics.bbox.yMax + 63) & -64;

	if (f->glyphs_cached[j])
	   rtmp = f->glyphs_cached[j];
	else
	  {
	     rtmp = rchr;
	     clear_raster(rtmp);
	     TT_Get_Glyph_Pixmap(f->glyphs[j], rtmp, -xmin, -ymin);
	     f->glyphs_cached[j] = duplicate_raster(rtmp);
	  }
	/* Blit-or the resulting small pixmap into the biggest one */
	/* We do that by hand, and provide also clipping.          */

	xmin = (xmin >> 6) + x_offset;
	ymin = (ymin >> 6) + y_offset;
	xmax = (xmax >> 6) + x_offset;
	ymax = (ymax >> 6) + y_offset;

	/* Take care of comparing xmin and ymin with signed values!  */
	/* This was the cause of strange misplacements when Bit.rows */
	/* was unsigned.                                             */

	if (xmin >= (int)rmap->width ||
	    ymin >= (int)rmap->rows || xmax < 0 || ymax < 0)
	   continue;

	/* Note that the clipping check is performed _after_ rendering */
	/* the glyph in the small bitmap to let this function return   */
	/* potential error codes for all glyphs, even hidden ones.     */

	/* In exotic glyphs, the bounding box may be larger than the   */
	/* size of the small pixmap.  Take care of that here.          */

	if (xmax - xmin + 1 > rtmp->width)
	   xmax = xmin + rtmp->width - 1;

	if (ymax - ymin + 1 > rtmp->rows)
	   ymax = ymin + rtmp->rows - 1;

	/* set up clipping and cursors */

	iread = 0;
	if (ymin < 0)
	  {
	     iread -= ymin * rtmp->cols;
	     ioff = 0;
	     ymin = 0;
	  }
	else
	   ioff = (rmap->rows - ymin - 1) * rmap->cols;

	if (ymax >= rmap->rows)
	   ymax = rmap->rows - 1;

	if (xmin < 0)
	  {
	     iread -= xmin;
	     xmin = 0;
	  }
	else
	   ioff += xmin;

	if (xmax >= rmap->width)
	   xmax = rmap->width - 1;

	_read = (char *)rtmp->bitmap + iread;
	_off = (char *)rmap->bitmap + ioff;

	for (y = ymin; y <= ymax; y++)
	  {
	     read = _read;
	     off = _off;

	     for (x = xmin; x <= xmax; x++)
	       {
		  *off = bounded_palette[*off | *read];
		  off++;
		  read++;
	       }
	     _read += rtmp->cols;
	     _off -= rmap->cols;
	  }
	x_offset += metrics.advance / 64;
     }
}

static void
merge_text_16(XImage * xim, TT_Raster_Map * rmap, int offset_x, int offset_y,
	      unsigned long col)
{
   int                 x, y, tmp;
   unsigned char      *ptr;
   unsigned char       cr, cg, cb, a, r, g, b, nr, ng, nb;
   unsigned long       pixel;

   cr = (col >> 8) & 0xf8;
   cg = (col >> 3) & 0xfc;
   cb = (col << 3) & 0xf8;
   for (y = 0; y < xim->height; y++)
     {
	ptr =
	   (unsigned char *)rmap->bitmap + offset_x +
	   ((y + offset_y) * rmap->cols);
	for (x = 0; x < xim->width; x++)
	  {
	     if ((a = alpha_lut[*ptr]) > 0)
	       {
		  if (a < 255)
		    {
		       pixel = XGetPixel(xim, x, y);
		       r = (pixel >> 8) & 0xf8;
		       g = (pixel >> 3) & 0xfc;
		       b = (pixel << 3) & 0xf8;

		       tmp = (cr - r) * a;
		       nr = r + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       tmp = (cg - g) * a;
		       ng = g + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       tmp = (cb - b) * a;
		       nb = b + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       pixel =
			  ((nr & 0xf8) << 8) | ((ng & 0xfc) << 3) | ((nb & 0xf8)
								     >> 3);
		       XPutPixel(xim, x, y, pixel);
		    }
		  else
		     XPutPixel(xim, x, y, col);
	       }
	     ptr++;
	  }
     }
}

static void
merge_text_15(XImage * xim, TT_Raster_Map * rmap, int offset_x, int offset_y,
	      unsigned long col)
{
   int                 x, y, tmp;
   unsigned char      *ptr;
   unsigned char       cr, cg, cb, a, r, g, b, nr, ng, nb;
   unsigned long       pixel;

   cr = (col >> 7) & 0xf8;
   cg = (col >> 2) & 0xf8;
   cb = (col << 3) & 0xf8;
   for (y = 0; y < xim->height; y++)
     {
	ptr =
	   (unsigned char *)rmap->bitmap + offset_x +
	   ((y + offset_y) * rmap->cols);
	for (x = 0; x < xim->width; x++)
	  {
	     if ((a = alpha_lut[*ptr]) > 0)
	       {
		  if (a < 255)
		    {
		       pixel = XGetPixel(xim, x, y);
		       r = (pixel >> 7) & 0xf8;
		       g = (pixel >> 2) & 0xf8;
		       b = (pixel << 3) & 0xf8;

		       tmp = (cr - r) * a;
		       nr = r + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       tmp = (cg - g) * a;
		       ng = g + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       tmp = (cb - b) * a;
		       nb = b + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       pixel =
			  ((nr & 0xf8) << 7) | ((ng & 0xf8) << 2) | ((nb & 0xf8)
								     >> 3);
		       XPutPixel(xim, x, y, pixel);
		    }
		  else
		     XPutPixel(xim, x, y, col);
	       }
	     ptr++;
	  }
     }
}

static void
merge_text_24(XImage * xim, TT_Raster_Map * rmap, int offset_x, int offset_y,
	      unsigned long col)
{
   int                 x, y, tmp;
   unsigned char      *ptr;
   unsigned char       cr, cg, cb, a, r, g, b, nr, ng, nb;
   unsigned long       pixel;

   cr = (col >> 16) & 0xff;
   cg = (col >> 8) & 0xff;
   cb = col & 0xff;
   for (y = 0; y < xim->height; y++)
     {
	ptr =
	   (unsigned char *)rmap->bitmap + offset_x +
	   ((y + offset_y) * rmap->cols);
	for (x = 0; x < xim->width; x++)
	  {
	     if ((a = alpha_lut[*ptr]) > 0)
	       {
		  if (a < 255)
		    {
		       pixel = XGetPixel(xim, x, y);
		       r = (pixel >> 16) & 0xff;
		       g = (pixel >> 8) & 0xff;
		       b = pixel & 0xff;

		       tmp = (cr - r) * a;
		       nr = r + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       tmp = (cg - g) * a;
		       ng = g + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       tmp = (cb - b) * a;
		       nb = b + ((tmp + (tmp >> 8) + 0x80) >> 8);
		       pixel = ((nr << 16) | (ng << 8) | (nb));
		       XPutPixel(xim, x, y, pixel);
		    }
		  else
		     XPutPixel(xim, x, y, col);
	       }
	     ptr++;
	  }
     }
}

/*
 * static void
 * merge_text_8(XImage * xim, TT_Raster_Map * rmap, int offset_x, int offset_y,
 * unsigned long col, Colormap cm)
 * {
 * int                 x, y, tmp;
 * unsigned char      *ptr;
 * unsigned char       a, r, g, b, nr, ng, nb;
 * unsigned long       pixel;
 * 
 * for (y = 0; y < xim->height; y++)
 * {
 * ptr = (unsigned char *)rmap->bitmap + offset_x + ((y + offset_y) * rmap->cols);
 * for (x = 0; x < xim->width; x++)
 * {
 * if ((a = alpha_lut[*ptr]) > 0)
 * {
 * pixel = XGetPixel(xim, x, y);
 * r = (pixel >> 8) & 0xf8;
 * g = (pixel >> 3) & 0xfc;
 * b = (pixel << 3) & 0xf8;
 * 
 * tmp = (255 - r) * a;
 * nr = r + ((tmp + (tmp >> 8) + 0x80) >> 8);
 * tmp = (255 - g) * a;
 * ng = g + ((tmp + (tmp >> 8) + 0x80) >> 8);
 * tmp = (255 - b) * a;
 * nb = b + ((tmp + (tmp >> 8) + 0x80) >> 8);
 * pixel = ((nr & 0xf8) << 8) | ((ng & 0xfc) << 3) | ((nb & 0xf8) >> 3);
 * XPutPixel(xim, x, y, pixel);
 * }
 * ptr++;
 * }
 * }
 * col = 0;
 * cm = 0;
 * }
 */

static void
merge_text_1(XImage * xim, TT_Raster_Map * rmap, int offset_x, int offset_y,
	     unsigned long col)
{
   int                 x, y;
   unsigned char      *ptr;

   for (y = 0; y < xim->height; y++)
     {
	ptr =
	   (unsigned char *)rmap->bitmap + offset_x +
	   ((y + offset_y) * rmap->cols);
	for (x = 0; x < xim->width; x++)
	  {
	     if (alpha_lut[*ptr] > 2)
		XPutPixel(xim, x, y, col);
	     ptr++;
	  }
     }
}

static char         x_error = 0;

static void
handle_x_error(Display * d, XErrorEvent * ev)
{
   d = NULL;
   ev = NULL;
   x_error = 1;
}

void
EFont_draw_string(Display * disp, Drawable win, GC gc, int x, int y, char *text,
		  Efont * font, Visual * vis, Colormap cm)
{
   XImage             *xim;
   XShmSegmentInfo     shminfo;
   int                 width, height, w, h, inx, iny, clipx, clipy, rx, ry;
   XWindowAttributes   xatt, ratt;
   TT_Raster_Map      *rmap, *rtmp;
   unsigned long       col;
   XGCValues           gcv;
   static char         shm_checked = 0, shm = 1;
   XErrorHandler       erh = NULL;
   Window              chld;
   char                is_pixmap = 0;

   inx = 0;
   iny = 0;
   rtmp = calc_size(font, &w, &h, text);
   rmap = create_font_raster(w, h);

   render_text(rmap, rtmp, font, text, &inx, &iny);

/*  XGrabServer(disp); */
   GrabX();
   erh = XSetErrorHandler((XErrorHandler) handle_x_error);
   x_error = 0;
   XGetWindowAttributes(disp, win, &xatt);
   XFlush(disp);
   if (x_error)
     {
	x_error = 0;
	is_pixmap = 1;
	EGetGeometry(disp, win, &chld, &rx, &rx,
		     (unsigned int *)&xatt.width, (unsigned int *)&xatt.height,
		     (unsigned int *)&rx, (unsigned int *)&xatt.depth);
	XFlush(disp);
	if (x_error)
	  {
	     destroy_font_raster(rmap);
	     destroy_font_raster(rtmp);
	     UngrabX();
/*        XUngrabServer(disp); */
/*        XFlush(disp); */
	     XSetErrorHandler((XErrorHandler) erh);
	     return;
	  }
     }
   XSetErrorHandler((XErrorHandler) erh);
   if (!is_pixmap)
     {
	XGetWindowAttributes(disp, xatt.root, &ratt);
	XTranslateCoordinates(disp, win, xatt.root, 0, 0, &rx, &ry, &chld);
	if ((xatt.map_state != IsViewable) && (xatt.backing_store == NotUseful))
	  {
	     destroy_font_raster(rmap);
	     destroy_font_raster(rtmp);
	     UngrabX();
/*        XUngrabServer(disp); */
/*        XFlush(disp); */
	     return;
	  }
     }
   XGetGCValues(disp, gc, GCForeground, &gcv);
   col = gcv.foreground;

   clipx = 0;
   clipy = 0;

   x = x - inx;
   y = y - iny;

   width = xatt.width - x;
   height = xatt.height - y;
   if (width > w)
      width = w;
   if (height > h)
      height = h;

   if (!is_pixmap)
     {
	if ((rx + x + width) > ratt.width)
	   width = ratt.width - (rx + x);
	if ((ry + y + height) > ratt.height)
	   height = ratt.height - (ry + y);
     }
   if (x < 0)
     {
	clipx = -x;
	width += x;
	x = 0;
     }
   if (y < 0)
     {
	clipy = -y;
	height += y;
	y = 0;
     }
   if (!is_pixmap)
     {
	if ((rx + x) < 0)
	  {
	     clipx -= (rx + x);
	     width += (rx + x);
	     x = -rx;
	  }
	if ((ry + y) < 0)
	  {
	     clipy -= (ry + y);
	     height += (ry + y);
	     y = -ry;
	  }
     }
   if ((width <= 0) || (height <= 0))
     {
	destroy_font_raster(rmap);
	destroy_font_raster(rtmp);
	UngrabX();
/*      XUngrabServer(disp); */
/*      XFlush(disp, False); */
	return;
     }
   if (shm)
     {
	if (!shm_checked)
	  {
	     erh = XSetErrorHandler((XErrorHandler) handle_x_error);
	  }
	xim = XShmCreateImage(disp, vis, xatt.depth, ZPixmap, NULL,
			      &shminfo, width, height);
	if (!shm_checked)
	  {
	     XSync(disp, False);
	     if (x_error)
	       {
		  shm = 0;
		  XDestroyImage(xim);
		  xim =
		     XGetImage(disp, win, x, y, width, height, 0xffffffff,
			       ZPixmap);
		  XSetErrorHandler((XErrorHandler) erh);
		  shm_checked = 1;
	       }
	     else
	       {
		  shminfo.shmid = shmget(IPC_PRIVATE, xim->bytes_per_line *
					 xim->height, IPC_CREAT | 0666);
		  if (shminfo.shmid < 0)
		    {
		       shm = 0;
		       XDestroyImage(xim);
		       xim =
			  XGetImage(disp, win, x, y, width, height, 0xffffffff,
				    ZPixmap);
		       XSetErrorHandler((XErrorHandler) erh);
		       shm_checked = 1;
		    }
		  else
		    {
		       shminfo.shmaddr = xim->data = shmat(shminfo.shmid, 0, 0);
		       shminfo.readOnly = False;
		       XShmAttach(disp, &shminfo);
		    }
	       }
	  }
	else
	  {
	     shminfo.shmid = shmget(IPC_PRIVATE, xim->bytes_per_line *
				    xim->height, IPC_CREAT | 0666);
	     if (shminfo.shmid < 0)
	       {
		  shm = 0;
		  XDestroyImage(xim);
		  xim =
		     XGetImage(disp, win, x, y, width, height, 0xffffffff,
			       ZPixmap);
		  XSetErrorHandler((XErrorHandler) erh);
		  shm_checked = 1;
	       }
	     else
	       {
		  shminfo.shmaddr = xim->data = shmat(shminfo.shmid, 0, 0);
		  shminfo.readOnly = False;
		  XShmAttach(disp, &shminfo);
	       }
	  }
	if (!shm_checked)
	  {
	     XSync(disp, False);
	     if (x_error)
	       {
		  shm = 0;
		  XDestroyImage(xim);
		  xim =
		     XGetImage(disp, win, x, y, width, height, 0xffffffff,
			       ZPixmap);
		  shm_checked = 1;
	       }
	     XSetErrorHandler((XErrorHandler) erh);
	     shm_checked = 1;
	  }
     }
   else
      xim = XGetImage(disp, win, x, y, width, height, 0xffffffff, ZPixmap);
   if (shm)
      XShmGetImage(disp, win, xim, x, y, 0xffffffff);
   UngrabX();
/*  XUngrabServer(disp); */
/*  XFlush(disp); */

/*  
 * if (xatt.depth == 16)
 * {
 * XVisualInfo         xvi, *xvir;
 * int                 num;
 * 
 * xvi.visualid = XVisualIDFromVisual(vis);;
 * xvir = XGetVisualInfo(disp, VisualIDMask, &xvi, &num);
 * if (xvir)
 * {
 * if (xvir->red_mask != 0xf800)
 * xatt.depth = 15;
 * XFree(xvir);
 * }
 * }
 */
   if (xatt.depth == 16)
     {
	if (id->x.render_depth == 15)
	   merge_text_15(xim, rmap, clipx, clipy, col);
	else
	   merge_text_16(xim, rmap, clipx, clipy, col);
     }
   else if ((xatt.depth == 24) || (xatt.depth == 32))
      merge_text_24(xim, rmap, clipx, clipy, col);
/*  else if (xatt.depth == 8)
 * merge_text_8(xim, rmap, clipx, clipy, cm, col); */
   else if (xatt.depth == 15)
      merge_text_15(xim, rmap, clipx, clipy, col);
   else if (xatt.depth <= 8)
      merge_text_1(xim, rmap, clipx, clipy, col);

   if (shm)
      XShmPutImage(disp, win, gc, xim, 0, 0, x, y, width, height, False);
   else
      XPutImage(disp, win, gc, xim, 0, 0, x, y, width, height);
   destroy_font_raster(rmap);
   destroy_font_raster(rtmp);
   if (shm)
     {
	XSync(disp, False);
	XShmDetach(disp, &shminfo);
	shmdt(shminfo.shmaddr);
	shmctl(shminfo.shmid, IPC_RMID, 0);
     }
   XDestroyImage(xim);
   cm = 0;
}

void
Efont_free(Efont * f)
{
   int                 i;

   if (!f)
      return;
   TT_Done_Instance(f->instance);
   TT_Close_Face(f->face);
   for (i = 0; i < 256; i++)
     {
	if (f->glyphs_cached[i])
	   destroy_font_raster(f->glyphs_cached[i]);
	if (!TT_VALID(f->glyphs[i]))
	   TT_Done_Glyph(f->glyphs[i]);
     }
   if (f->glyphs)
      Efree(f->glyphs);
   if (f->glyphs_cached)
      Efree(f->glyphs_cached);
   Efree(f);
}

Efont              *
Efont_load(char *file, int size)
{
   TT_Error            error;
   TT_CharMap          char_map;
   TT_Glyph_Metrics    metrics;
   static TT_Engine    engine;
   static char         have_engine = 0;
   int                 dpi = 96;
   Efont              *f;
   unsigned short      i, n, code, load_flags;
   unsigned short      num_glyphs = 0, no_cmap = 0;
   unsigned short      platform, encoding;

   if (!have_engine)
     {
	error = TT_Init_FreeType(&engine);
	if (error)
	   return NULL;
	have_engine = 1;
     }
   f = Emalloc(sizeof(Efont));
   f->engine = engine;
   error = TT_Open_Face(f->engine, file, &f->face);
   if (error)
     {
	Efree(f);
/*      fprintf(stderr, "Unable to open font\n"); */
	return NULL;
     }
   error = TT_Get_Face_Properties(f->face, &f->properties);
   if (error)
     {
	TT_Close_Face(f->face);
	Efree(f);
/*      fprintf(stderr, "Unable to get face properties\n"); */
	return NULL;
     }
   error = TT_New_Instance(f->face, &f->instance);
   if (error)
     {
	TT_Close_Face(f->face);
	Efree(f);
/*      fprintf(stderr, "Unable to create instance\n"); */
	return NULL;
     }
   TT_Set_Instance_Resolutions(f->instance, dpi, dpi);
   TT_Set_Instance_CharSize(f->instance, size * 64);

   n = f->properties.num_CharMaps;

   for (i = 0; i < n; i++)
     {
	TT_Get_CharMap_ID(f->face, i, &platform, &encoding);
	if ((platform == 3 && encoding == 1) ||
	    (platform == 0 && encoding == 0))
	  {
	     TT_Get_CharMap(f->face, i, &char_map);
	     break;
	  }
     }
   if (i == n)
     {
	no_cmap = 1;
	num_glyphs = f->properties.num_Glyphs;
	TT_Done_Instance(f->instance);
	TT_Close_Face(f->face);
	Efree(f);
/*      fprintf(stderr, "Sorry, but this font doesn't contain any Unicode mapping table\n"); */
	return NULL;
     }
   f->num_glyph = 256;
   f->glyphs = (TT_Glyph *) Emalloc(256 * sizeof(TT_Glyph));
   memset(f->glyphs, 0, 256 * sizeof(TT_Glyph));
   f->glyphs_cached = (TT_Raster_Map **) Emalloc(256 * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached, 0, 256 * sizeof(TT_Raster_Map *));

   load_flags = TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH;

   f->max_descent = 0;
   f->max_ascent = 0;

   for (i = 0; i < 256; ++i)
     {
	if (TT_VALID(f->glyphs[i]))
	   continue;

	if (no_cmap)
	  {
	     code = (i - ' ' + 1) < 0 ? 0 : (i - ' ' + 1);
	     if (code >= num_glyphs)
		code = 0;
	  }
	else
	   code = TT_Char_Index(char_map, i);

	TT_New_Glyph(f->face, &f->glyphs[i]);
	TT_Load_Glyph(f->instance, f->glyphs[i], code, load_flags);
	TT_Get_Glyph_Metrics(f->glyphs[i], &metrics);

	if ((metrics.bbox.yMin & -64) < f->max_descent)
	   f->max_descent = (metrics.bbox.yMin & -64);
	if (((metrics.bbox.yMax + 63) & -64) > f->max_ascent)
	   f->max_ascent = ((metrics.bbox.yMax + 63) & -64);
     }
   return f;
}

void
Efont_extents(Efont * f, char *text, int *font_ascent_return,
	      int *font_descent_return, int *width_return,
	      int *max_ascent_return, int *max_descent_return,
	      int *lbearing_return, int *rbearing_return)
{
   int                 i, upm, ascent, descent, pw;
   TT_Instance_Metrics imetrics;
   TT_Glyph_Metrics    gmetrics;

   if (!f)
      return;

   TT_Get_Instance_Metrics(f->instance, &imetrics);
   upm = f->properties.header->Units_Per_EM;
   ascent = (f->properties.horizontal->Ascender * imetrics.y_ppem) / upm;
   descent = (f->properties.horizontal->Descender * imetrics.y_ppem) / upm;
   if (ascent < 0)
      ascent = -ascent;
   if (descent < 0)
      descent = -descent;
   pw = 0;

   for (i = 0; text[i]; i++)
     {
	unsigned char       j = text[i];

	if (!TT_VALID(f->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(f->glyphs[j], &gmetrics);
	if (i == 0)
	  {
	     if (lbearing_return)
		*lbearing_return = ((-gmetrics.bearingX) / 64);
	  }
	if (text[i + 1] == 0)
	  {
	     if (rbearing_return)
		*rbearing_return =
		   ((gmetrics.bbox.xMax - gmetrics.advance) / 64);
	  }
	pw += gmetrics.advance / 64;
     }
   if (font_ascent_return)
      *font_ascent_return = ascent;
   if (font_descent_return)
      *font_descent_return = descent;
   if (width_return)
      *width_return = pw;
   if (max_ascent_return)
      *max_ascent_return = f->max_ascent;
   if (max_descent_return)
      *max_descent_return = f->max_descent;
}

/*
 * int
 * main( int argc, char **argv)
 * {
 * Display            *disp;
 * Efont              *f;
 * GC                  gc;
 * XGCValues           gcv;
 * Window              win;
 * int                 i;
 * 
 * disp=XOpenDisplay(NULL);
 * XSync(disp, False);
 * srand(time(NULL));
 * win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 640, 480, 0, 
 * 0, 0);
 * EMapWindow(disp, win);
 * XSync(disp, False);
 * 
 * gcv.subwindow_mode = IncludeInferiors;
 * gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
 * for (;;)
 * {
 * for (i = 3; i < argc; i++)
 * {
 * XSetForeground(disp, gc, rand()<<16 | rand());
 * f = Efont_load(argv[i], atoi(argv[1]));
 * if (f)
 * EFont_draw_string(disp, win, gc, 20, (atoi(argv[1])/10) * (i-2), argv[2], 
 * f,
 * DefaultVisual(disp, DefaultScreen(disp)),
 * DefaultColormap(disp, DefaultScreen(disp)));
 * Efont_free(f);
 * f = NULL;
 * }
 * }
 * return 0;
 * }
 */
