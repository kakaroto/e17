/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "dox.h"

#if USE_IMLIB2

struct _efont
{
   Imlib_Font         *face;
};

static void
ImlibSetFgColorFromGC(Display * dpy, GC gc, Colormap cm)
{
   XGCValues           xgcv;
   XColor              xclr;
   int                 r, g, b;

   XGetGCValues(dpy, gc, GCForeground, &xgcv);
   xclr.pixel = xgcv.foreground;
   XQueryColor(dpy, cm, &xclr);
   EGetColor(&xclr, &r, &g, &b);
   imlib_context_set_color(r, g, b, 255);
}

void
EFont_draw_string(Display * dpy, Drawable win, GC gc, int x, int y, char *text,
		  Efont * f, Visual * vis __UNUSED__, Colormap cm)
{
   Imlib_Image         im;
   int                 w, h, ascent, descent;

   Efont_extents(f, text, &ascent, &descent, &w, NULL, NULL, NULL, NULL);
   h = ascent + descent;

   imlib_context_set_drawable(win);
   im = imlib_create_image_from_drawable(0, x, y - ascent, w, h, 0);
   imlib_context_set_image(im);

   imlib_context_set_font(f->face);
   ImlibSetFgColorFromGC(dpy, gc, cm);
   imlib_text_draw(0, 0, text);
   imlib_render_image_on_drawable(x, y - ascent);
   imlib_free_image();
}

void
Efont_free(Efont * f)
{
   if (!f)
      return;

   imlib_context_set_font(f->face);
   imlib_free_font();

   Efree(f);
}

Efont              *
Efont_load(char *file, int size)
{
   char                s[4096];
   Efont              *f;
   Imlib_Font         *ff;

   Esnprintf(s, sizeof(s), "%s/%d", file, size);
   ff = imlib_load_font(s);
   if (ff == NULL)
      return NULL;

   f = Emalloc(sizeof(Efont));
   f->face = ff;

   return f;
}

void
Efont_extents(Efont * f, char *text, int *font_ascent_return,
	      int *font_descent_return, int *width_return,
	      int *max_ascent_return, int *max_descent_return,
	      int *lbearing_return __UNUSED__, int *rbearing_return __UNUSED__)
{
   int                 height;

   if (!f)
      return;

   imlib_context_set_font(f->face);
   imlib_get_text_size(text, width_return, &height);
   if (font_ascent_return)
      *font_ascent_return = imlib_get_font_ascent();
   if (font_descent_return)
      *font_descent_return = imlib_get_font_descent();
   if (max_ascent_return)
      *max_ascent_return = imlib_get_maximum_font_ascent();
   if (max_descent_return)
      *max_descent_return = imlib_get_maximum_font_descent();
}

#else /* USE_IMLIB1 */

#define TT_VALID( handle )  ( ( handle ).z != NULL )

#if TEST_TTFONT
#undef XSync
#undef IC_RenderDepth
#define IC_RenderDepth() DefaultDepth(disp, DefaultScreen(disp))
#define EGetGeometry XGetGeometry
#endif

#ifdef HAVE_FREETYPE1_FREETYPE_FREETYPE_H
#include <freetype1/freetype/freetype.h>
#elif defined(HAVE_FREETYPE_FREETYPE_H)
#include <freetype/freetype.h>
#else
#include <freetype.h>
#endif

struct _efont
{
   TT_Engine           engine;
   TT_Face             face;
   TT_Instance         instance;
   TT_Face_Properties  properties;
   int                 num_glyph;
   TT_Glyph           *glyphs;
   TT_Raster_Map     **glyphs_cached;
   TT_CharMap          char_map;
   int                 max_descent;
   int                 max_ascent;
};

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

struct _imlib_encoding_map
{
   unsigned short      char_map[98];
};

typedef struct _imlib_encoding_map ImlibEncodingMap;

/* Encoding maps */
static const ImlibEncodingMap iso1 = {
   {
    /* ISO-8859-1 encoding (conversion to UTF-8) */
    0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8,
    0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 0x00B0, 0x00B1,
    0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA,
    0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3,
    0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC,
    0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5,
    0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE,
    0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
    0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x00F0,
    0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9,
    0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF}
};

static const ImlibEncodingMap iso2 = {
   {
    /* ISO-8859-2 encoding (conversion to UTF-8) */
    0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7, 0x00A8,
    0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B, 0x00B0, 0x0105,
    0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7, 0x00B8, 0x0161, 0x015F,
    0x0165, 0x017A, 0x02DD, 0x017E, 0x017C, 0x0154, 0x00C1, 0x00C2, 0x0102,
    0x00C4, 0x0139, 0x0106, 0x00C7, 0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A,
    0x00CD, 0x00CE, 0x010E, 0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150,
    0x00D6, 0x00D7, 0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162,
    0x00DF, 0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
    0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F, 0x0111,
    0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7, 0x0159, 0x016F,
    0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9}
};

static const ImlibEncodingMap iso3 = {
   {
    /* ISO-8859-3 encoding (conversion to UTF-8) */
    0x00A0, 0x0126, 0x02D8, 0x00A3, 0x00A4, 0x0124, 0x00A7, 0x00A8, 0x0130,
    0x015E, 0x011E, 0x0134, 0x00AD, 0x017B, 0x00B0, 0x0127, 0x00B2, 0x00B3,
    0x00B4, 0x00B5, 0x0125, 0x00B7, 0x00B8, 0x0131, 0x015F, 0x011F, 0x0135,
    0x00BD, 0x017C, 0x00C0, 0x00C1, 0x00C2, 0x00C4, 0x010A, 0x0108, 0x00C7,
    0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D1,
    0x00D2, 0x00D3, 0x00D4, 0x0120, 0x00D6, 0x00D7, 0x011C, 0x00D9, 0x00DA,
    0x00DB, 0x00DC, 0x016C, 0x015C, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E4,
    0x010B, 0x0109, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED,
    0x00EE, 0x00EF, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x0121, 0x00F6, 0x00F7,
    0x011D, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x016D, 0x015D, 0x02D9}
};

static const ImlibEncodingMap iso4 = {
   {
    /* ISO-8859-4 encoding (conversion to UTF-8) */
    0x00A0, 0x0104, 0x0138, 0x0156, 0x00A4, 0x0128, 0x013B, 0x00A7, 0x00A8,
    0x0160, 0x0112, 0x0122, 0x0166, 0x00AD, 0x017D, 0x00AF, 0x00B0, 0x0105,
    0x02DB, 0x0157, 0x00B4, 0x0129, 0x013C, 0x02C7, 0x00B8, 0x0161, 0x0113,
    0x0123, 0x0167, 0x014A, 0x017E, 0x014B, 0x0100, 0x00C1, 0x00C2, 0x00C3,
    0x00C4, 0x00C5, 0x00C6, 0x012E, 0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116,
    0x00CD, 0x00CE, 0x012A, 0x0110, 0x0145, 0x014C, 0x0136, 0x00D4, 0x00D5,
    0x00D6, 0x00D7, 0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x0168, 0x016A,
    0x00DF, 0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
    0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x012B, 0x0111,
    0x0146, 0x014D, 0x0137, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x0173,
    0x00FA, 0x00FB, 0x00FC, 0x0169, 0x016B, 0x02D9}
};

static const ImlibEncodingMap iso5 = {
   {
    /* ISO-8859-5 encoding (conversion to UTF-8) */
    0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408,
    0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F, 0x0410, 0x0411,
    0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A,
    0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 0x0420, 0x0421, 0x0422, 0x0423,
    0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C,
    0x042D, 0x042E, 0x042F, 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435,
    0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E,
    0x043F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F, 0x2116,
    0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459,
    0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F}
};

static const ImlibEncodingMap *_imlib_encodings[5] =
   { &iso1, &iso2, &iso3, &iso4, &iso5 };
int                 IsoEncoding = ENCOING_ISO_8859_1;
char                encoding_initialized = 0;

static void
__init_iso_encoding()
{
   /* Set the current ISO encoding based on (in that order):
    * the "IMLIB_ENCODING" environment variable,
    * the "LANG" variable,
    * the ISO-8859-1 default */

   char               *s, *enc_num, *lang;
   char                iso2lang[21] = "cs hu pl ro hr sk sl";
   char                iso3lang[6] = "eo mt";
   char                iso4lang[12] = "et lv lt kl";
   char                iso5lang[18] = "bg be mk ru sr uk";

   /* Check if we already initialized the encoding */
   if (!encoding_initialized)
     {
	IsoEncoding = 255;
	/* First check if we have IMLIB_ENCODING variable set */
	if ((s = getenv("IMLIB_ENCODING")) != NULL)
	  {
	     if ((enc_num = strstr(s, "8859-")) != NULL)
	       {
		  if (!strcmp((enc_num + 5), "1"))
		     IsoEncoding = 0;
		  else if (!strcmp((enc_num + 5), "2"))
		     IsoEncoding = 1;
		  else if (!strcmp((enc_num + 5), "3"))
		     IsoEncoding = 2;
		  else if (!strcmp((enc_num + 5), "4"))
		     IsoEncoding = 3;
		  else if (!strcmp((enc_num + 5), "5"))
		     IsoEncoding = 4;
	       }
	  }
	if (IsoEncoding == 255)
	  {
	     /* Still not initialized - try to guess from LANG variable */
	     if ((lang = getenv("LANG")) != NULL)
	       {
		  if (strstr(iso2lang, lang) != NULL)
		     IsoEncoding = 1;
		  else if (strstr(iso3lang, lang) != NULL)
		     IsoEncoding = 2;
		  else if (strstr(iso4lang, lang) != NULL)
		     IsoEncoding = 3;
		  else if (strstr(iso5lang, lang) != NULL)
		     IsoEncoding = 4;
	       }
	  }
	if (IsoEncoding == 255)
	  {
	     /* Still not initialized - default to ISO8859-1 */
	     IsoEncoding = 0;
	  }
	encoding_initialized = 1;
     }
}

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

	if (xmin >= (int)rmap->width || ymin >= (int)rmap->rows ||
	    xmax < 0 || ymax < 0)
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
	ptr = (unsigned char *)rmap->bitmap + offset_x +
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
		       pixel = ((nr & 0xf8) << 8) | ((ng & 0xfc) << 3) |
			  ((nb & 0xf8) >> 3);
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
	ptr = (unsigned char *)rmap->bitmap + offset_x +
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
		       pixel = ((nr & 0xf8) << 7) | ((ng & 0xf8) << 2) |
			  ((nb & 0xf8) >> 3);
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
	ptr = (unsigned char *)rmap->bitmap + offset_x +
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
	ptr = (unsigned char *)rmap->bitmap + offset_x +
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
		  Efont * f, Visual * vis, Colormap cm)
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
   rtmp = calc_size(f, &w, &h, text);
   rmap = create_font_raster(w, h);

   render_text(rmap, rtmp, f, text, &inx, &iny);

   XGrabServer(disp);
   erh = XSetErrorHandler((XErrorHandler) handle_x_error);
   x_error = 0;
   XGetWindowAttributes(disp, win, &xatt);
   XFlush(disp);
   if (x_error)
     {
	x_error = 0;
	is_pixmap = 1;
	XGetGeometry(disp, win, &chld, &rx, &rx,
		     (unsigned int *)&xatt.width, (unsigned int *)&xatt.height,
		     (unsigned int *)&rx, (unsigned int *)&xatt.depth);
	XFlush(disp);
	if (x_error)
	  {
	     destroy_font_raster(rmap);
	     destroy_font_raster(rtmp);
	     XUngrabServer(disp);
	     XFlush(disp);
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
	     XUngrabServer(disp);
	     XFlush(disp);
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
	XUngrabServer(disp);
	XFlush(disp);
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
		  xim = XGetImage(disp, win, x, y, width, height,
				  0xffffffff, ZPixmap);
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
		       xim = XGetImage(disp, win, x, y, width, height,
				       0xffffffff, ZPixmap);
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
		  xim = XGetImage(disp, win, x, y, width, height,
				  0xffffffff, ZPixmap);
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
		  xim = XGetImage(disp, win, x, y, width, height,
				  0xffffffff, ZPixmap);
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
   XUngrabServer(disp);
   XFlush(disp);

   if (xatt.depth == 16)
     {
	XVisualInfo         xvi, *xvir;
	int                 num;

	xvi.visualid = XVisualIDFromVisual(vis);;
	xvir = XGetVisualInfo(disp, VisualIDMask, &xvi, &num);
	if (xvir)
	  {
	     if (xvir->red_mask != 0xf800)
		xatt.depth = 15;
	     XFree(xvir);
	  }
     }
   if (xatt.depth == 16)
      merge_text_16(xim, rmap, clipx, clipy, col);
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
   TT_Glyph_Metrics    metrics;
   static TT_Engine    engine;
   static char         have_engine = 0;
   int                 dpi = 96;
   Efont              *f;
   unsigned short      i, n, index, load_flags;
   unsigned short      num_glyphs = 0, no_cmap = 0;
   unsigned short      platform, encoding;

   __init_iso_encoding();
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
	     /* TT_Get_CharMap(f->face, i, &char_map); */
	     TT_Get_CharMap(f->face, i, &(f->char_map));
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
	     index = (i - ' ' + 1) < 0 ? 0 : (i - ' ' + 1);
	     if (index >= num_glyphs)
		index = 0;
	  }
	else if (i < 0xA0)
	   index = TT_Char_Index(f->char_map, i);
	else
	   index =
	      TT_Char_Index(f->char_map,
			    _imlib_encodings[IsoEncoding]->
			    char_map[(i - 0xA0)]);

	TT_New_Glyph(f->face, &f->glyphs[i]);
	TT_Load_Glyph(f->instance, f->glyphs[i], index, load_flags);
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
#endif /* USE_IMLIB1 */

#if TEST_TTFONT

#undef XSync

Display            *disp;

int
main(int argc, char **argv)
{
   Efont              *f;
   GC                  gc;
   XGCValues           gcv;
   Window              win;
   int                 i, j;

   disp = XOpenDisplay(NULL);

#if USE_IMLIB2
   imlib_context_set_display(disp);
   imlib_context_set_visual(DefaultVisual(disp, DefaultScreen(disp)));
   imlib_context_set_colormap(DefaultColormap(disp, DefaultScreen(disp)));
#endif

   srand(time(NULL));
   win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 640, 480, 0,
			     0, 0);
   XMapWindow(disp, win);
   XSync(disp, False);

   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
   for (;; j++)
     {
	for (i = 3; i < argc; i++)
	  {
	     XSetForeground(disp, gc, rand() << 16 | rand());
	     f = Efont_load(argv[i], atoi(argv[1]));
	     if (f)
		EFont_draw_string(disp, win, gc, 20,
				  atoi(argv[1]) * (i - 2), argv[2], f,
				  DefaultVisual(disp, DefaultScreen(disp)),
				  DefaultColormap(disp, DefaultScreen(disp)));
	     Efont_free(f);
	     f = NULL;
	  }
     }
   return 0;
}
#endif
