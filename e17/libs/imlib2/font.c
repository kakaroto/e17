#include "common.h"
#include <X11/Xlib.h>
#include "image.h"
#include "blend.h"
#include <freetype.h>
#include "font.h"
#include <sys/types.h>
#include "file.h"
#include "rgbadraw.h"

#define TT_VALID( handle )  ( ( handle ).z != NULL )

/* cached font list and font path */
static ImlibFont *fonts = NULL;
static int        fpath_num = 0;
static char     **fpath = NULL;

/* lookupt table of raster_map -> RGBA Alpha values */
static int rend_lut[9] = 
{ 0, 64, 128, 192, 256, 256, 256, 256, 256};

/* create an rmap of width and height */
TT_Raster_Map *
__imlib_create_font_raster(int width, int height)
{
   TT_Raster_Map      *rmap;
   
   rmap = malloc(sizeof(TT_Raster_Map));
   rmap->width = (width + 3) & -4;
   rmap->rows = height;
   rmap->flow = TT_Flow_Up;
   rmap->cols = rmap->width;
   rmap->size = rmap->rows * rmap->width;
   rmap->bitmap = malloc(rmap->size);
   memset(rmap->bitmap, 0, rmap->size);
   return rmap;
}

/* free the rmap */
void
__imlib_destroy_font_raster(TT_Raster_Map * rmap)
{
   free(rmap->bitmap);
   free(rmap);
}

void
__imlib_add_font_path(char *path)
{
   fpath_num++;
   if (fpath_num == 1)
      fpath = malloc(sizeof(char *));
   else
      fpath = realloc(fpath, (fpath_num * sizeof(char *)));
   fpath[fpath_num - 1] = strdup(path);
}

void
__imlib_del_font_path(char *path)
{
   int i, j;
   
   for (i = 0; i < fpath_num; i++)
     {
	if (!strcmp(path, fpath[i]))
	  {
	     fpath_num--;
	     for (j = i; j < fpath_num; j++)
		fpath[j] = fpath[j + 1];
	     if (fpath_num > 0)
		fpath = realloc(fpath, fpath_num * sizeof(char *));
	     else
	       {
		  free(fpath);
		  fpath = NULL;
	       }
	  }
     }
}

char **
__imlib_list_font_path(char *num_ret)
{
   *num_ret = fpath_num;
   return fpath;
}

ImlibFont *
__imlib_find_cached_font(char *fontname)
{
   ImlibFont *f;
   
   f = fonts;
   while(f)
     {
	if (!strcmp(f->name, fontname))
	   return f;
	f = f->next;
     }
   return NULL;
}

ImlibFont *
__imlib_load_font(char *fontname)
{
   ImlibFont *f;
   TT_Error            error;
   TT_CharMap          char_map;
   TT_Glyph_Metrics    metrics;
   TT_Instance_Metrics imetrics;
   static TT_Engine    engine;
   static char         have_engine = 0;
   int                 dpi = 96;
   unsigned short      i, n, code, load_flags;
   unsigned short      num_glyphs = 0, no_cmap = 0;
   unsigned short      platform, encoding;
   int                 size, j, upm, ascent, descent;
   char                *name, *file = NULL, *tmp;
   
   /* find a cached font */
   f = __imlib_find_cached_font(fontname);
   if (f)
     {
	/* reference it up by one and return it */
	f->references++;
	return f;
     }
   /* split fontname into file and size */
   /* if we dont have a truetype font engine yet - make one */
   if (!have_engine)
     {
	error = TT_Init_FreeType(&engine);
	if (error)
	   return NULL;
	have_engine = 1;
     }
   /* split font name (in format name/size) */
   for (j = strlen(fontname) - 1;
	(j >= 0) && (fontname[j] != '/');
	j--);
   /* no "/" in font after the first char */
   if (j <= 0)
      return NULL;
   /* get size */
   size = atoi(&(fontname[j + 1]));
   /* split name in front off */
   name = malloc(j * sizeof(char));
   memcpy(name, fontname, j);
   name[j] = 0;
   /* find file if it exists */
   for (j = 0; (j < fpath_num) && (!file); j++)
     {
	tmp = malloc(strlen(fpath[j]) + 1 + strlen(name) + 4 + 1);
	if (!tmp)
	  {
	     free(name);
	     return NULL;
	  }
	sprintf(tmp, "%s/%s.ttf", fpath[j], name);
	if (__imlib_FileIsFile(tmp))
	   file = strdup(tmp);
	else
	  {
	     sprintf(tmp, "%s/%s.TTF", fpath[j], name);
	     if (__imlib_FileIsFile(tmp))
		file = strdup(tmp);
	     else
	       {
		  sprintf(tmp, "%s/%s", fpath[j], name);
		  if (__imlib_FileIsFile(tmp))
		     file = strdup(tmp);
	       }
	  }
	free(tmp);
     }
   free(name);
   /* didnt find a file? abort */
   if (!file)
      return NULL;
   /* allocate */
   f = malloc(sizeof(ImlibFont));
   /* put in name and references */
   f->name = strdup(fontname);
   f->references = 1;
   /* remember engine */
   f->engine = engine;
   error = TT_Open_Face(f->engine, file, &f->face);
   if (error)
     {
	free(f->name);
	free(f);
	/*      fprintf(stderr, "Unable to open font\n"); */
	return NULL;
     }
   free(file);
   error = TT_Get_Face_Properties(f->face, &f->properties);
   if (error)
     {
	TT_Close_Face(f->face);
	free(f->name);
	free(f);
	/*      fprintf(stderr, "Unable to get face properties\n"); */
	return NULL;
     }
     
   error = TT_New_Instance(f->face, &f->instance);
   if (error)
     {
	TT_Close_Face(f->face);
	free(f->name);
	free(f);
	/*      fprintf(stderr, "Unable to create instance\n"); */
	return NULL;
     }
   
   TT_Set_Instance_Resolutions(f->instance, dpi, dpi);
   TT_Set_Instance_CharSize(f->instance, size * 64);
   n = f->properties.num_CharMaps;
   
   /* get ascent & descent */
   TT_Get_Instance_Metrics(f->instance, &imetrics);
   upm = f->properties.header->Units_Per_EM;
   ascent = (f->properties.horizontal->Ascender * imetrics.y_ppem) / upm;
   descent = (f->properties.horizontal->Descender * imetrics.y_ppem) / upm;
   if (descent < 0)
      descent = -descent;
   f->ascent = ascent;
   f->descent = descent;
   
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
	free(f->name);
	free(f);
	return NULL;
     }
   f->num_glyph = 256;
   f->glyphs = (TT_Glyph *)malloc(f->num_glyph * sizeof(TT_Glyph));
   memset(f->glyphs, 0, f->num_glyph * sizeof(TT_Glyph));
   
   f->glyphs_cached_right = 
      (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_right, 0, f->num_glyph * sizeof(TT_Raster_Map *));
   f->glyphs_cached_left = 
      (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_left, 0, f->num_glyph * sizeof(TT_Raster_Map *));
   f->glyphs_cached_down = 
      (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_down, 0, f->num_glyph * sizeof(TT_Raster_Map *));
   f->glyphs_cached_up = 
      (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_up, 0, f->num_glyph * sizeof(TT_Raster_Map *));
   
   load_flags = TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH;   
   f->max_descent = 0;
   f->max_ascent = 0;   
   for (i = 0; i < f->num_glyph; ++i)
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
   /* work around broken fonts - some just have wrogn ascent and */
   /* descent members */
   if (((f->ascent == 0) && (f->descent == 0)) || (f->ascent == 0))
     {
	f->ascent = f->max_ascent / 64;
	f->descent = -f->max_descent / 64;
     }
   /* all ent well in loading, so add to head of font list and return */
   f->next = fonts;
   fonts = f;
   return f;
}

void
__imlib_free_font(ImlibFont *font)
{
   int                 i;
   ImlibFont          *f, *pf;
   
   /* defererence */
   font->references--;
   /* if still referenced exit here */
   if (font->references > 0)
      return;

   /* remove form font cache list */
   pf = NULL;
   f = fonts;
   while (f)
     {
	if (f == font)
	  {
	     if (!pf)
		fonts = f->next;
	     else
		pf->next = f->next;
	     f = NULL;
	  }
	else
	  {
	     pf = f;
	     f = f->next;
	  }
     }
   /* free freetype instance stuff */
   TT_Done_Instance(font->instance);
   TT_Close_Face(font->face);
   /* free all cached glyphs */
   for (i = 0; i < font->num_glyph; i++)
     {	
	if ((font->glyphs_cached_right) && (font->glyphs_cached_right[i]))
	   __imlib_destroy_font_raster(font->glyphs_cached_right[i]);
	if ((font->glyphs_cached_left) && (font->glyphs_cached_left[i]))
	   __imlib_destroy_font_raster(font->glyphs_cached_left[i]);
	if ((font->glyphs_cached_down) && (font->glyphs_cached_down[i]))
	   __imlib_destroy_font_raster(font->glyphs_cached_down[i]);
	if ((font->glyphs_cached_up) && (font->glyphs_cached_up[i]))
	   __imlib_destroy_font_raster(font->glyphs_cached_up[i]);
	if (!TT_VALID(font->glyphs[i]))
	   TT_Done_Glyph(font->glyphs[i]);
     }
   /* free glyph info */
   free(font->glyphs);
   /* free glyph cache arrays */
   if (font->glyphs_cached_right)
      free(font->glyphs_cached_right);
   if (font->glyphs_cached_left)
      free(font->glyphs_cached_left);
   if (font->glyphs_cached_down)
      free(font->glyphs_cached_down);
   if (font->glyphs_cached_up)
      free(font->glyphs_cached_up);
   /* free font struct & name */
   free(font->name);
   free(font);
}

void
__imlib_calc_size(ImlibFont *f, int *width, int *height, char *text)
{
   int                 i, ascent, descent, pw, ph;
   TT_Glyph_Metrics    gmetrics;
   
   ascent = f->ascent;
   descent = f->descent;
   pw = 0;
   ph = ((f->max_ascent) - f->max_descent) / 64;
   
   for (i = 0; text[i]; i++)
     {
	unsigned char       j;
	
	j = text[i];	
	if (!TT_VALID(f->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(f->glyphs[j], &gmetrics);
	if (i == 0)
	   pw += ((-gmetrics.bearingX) / 64);
	if (text[i + 1] == 0)
	   pw += (gmetrics.bbox.xMax / 64);
	else
	   pw += gmetrics.advance / 64;
     }
   *width = pw;
   *height = ph;
}

void
__imlib_render_str(ImlibImage *im, ImlibFont *fn, int drx, int dry, char *text,
		   DATA8 r, DATA8 g, DATA8 b, DATA8 a,
		   char dir, int *retw, int *reth, int blur, 
		   int *nextx, int *nexty)
{
   DATA32              lut[9], *p, *tmp;
   TT_Glyph_Metrics    metrics;
   TT_F26Dot6          x, y, xmin, ymin, xmax, ymax;
   int                 w, h, i, ioff, iread, xor, yor;
   char               *off, *read, *_off, *_read;
   int                 x_offset, y_offset;
   unsigned char       j;
   TT_Raster_Map      *rtmp = NULL, *rmap;

   /* if we draw outside the image from here - give up */
   if ((drx > im->w) || (dry > im->h))
     {
	if ((retw) || (reth))
	  {
	     __imlib_calc_size(fn, &w, &h, text);
	     if (retw)
		*retw = w;
	     if (reth)
		*reth = h;
	  }
	return;
     }
   /* build LUT table */
   for (i = 0; i < 9; i++)
      lut[i] = (DATA32)(
			((((rend_lut[i] * (int)a) >> 8) & 0xff) << 24) |
			((int)r << 16) |
			((int)g << 8) |
			((int)b));

   /* get offset of first char */
   j = text[0];
   TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
   x_offset = (-metrics.bearingX) / 64;
   y_offset = -(fn->max_descent / 64);
   xor = x_offset;
   yor = rmap->rows - y_offset;

   /* figure out the size this text string is going to be */
   __imlib_calc_size(fn, &w, &h, text);
   if (retw)
      *retw = w;
   if (reth)
      *reth = h;
   if (*nexty)
      *nexty = fn->ascent + fn->descent;
   if (*nextx)
     {
	j = text[strlen(text) - 1];
	TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
	*nextx = w - (x_offset / 64) + metrics.advance - metrics.bbox.xMax;
     }
   /* if the text is completely outside the image - give up */
   if (((drx + w) <= 0) || ((dry + h) <= 0))
      return;
   /* create a scratch pad for it */
   rmap = __imlib_create_font_raster(w, h);
   rmap->flow = TT_Flow_Up;
   /* render the text into the scratch pad */
   for (i = 0; text[i]; i++)
     {
	j = text[i];	
	if (!TT_VALID(fn->glyphs[j]))
	   continue;	
	TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
	
	xmin = metrics.bbox.xMin & -64;
	ymin = metrics.bbox.yMin & -64;
	xmax = (metrics.bbox.xMax + 63) & -64;
	ymax = (metrics.bbox.yMax + 63) & -64;
	
	switch(dir)
	  {
	  case 0: /* to right */
	     if (fn->glyphs_cached_right[j])
		rtmp = fn->glyphs_cached_right[j];
	     else
	       {
		  rtmp = __imlib_create_font_raster(((xmax - xmin) / 64) + 1, 
						    ((ymax - ymin) / 64) + 1);
		  TT_Get_Glyph_Pixmap(fn->glyphs[j], rtmp, -xmin, -ymin);
		  fn->glyphs_cached_right[j] = rtmp;
	       }
	     break;
	  case 1: /* to left */
	     break;
	  case 2: /* to down */
	     break;
	  case 3: /* to up */
	     break;
	  default:
	     break;
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
	
	if ((xmin >= (int)rmap->width) || (ymin >= (int)rmap->rows) ||
	    (xmax < 0) || (ymax < 0))
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
		  *off |= *read;
		  off++;
		  read++;
	       }
	     _read += rtmp->cols;
	     _off -= rmap->cols;
	  }
	x_offset += metrics.advance / 64;
     }
   /* temporary RGBA buffer to build */
   tmp = malloc(rmap->rows * rmap->cols * sizeof(DATA32));
   p = tmp;
   read = rmap->bitmap;
   /* build the buffer */
   for (x = 0; x < rmap->size; x++)
     {
	*p = lut[(int)(*read)];
	p++;
	read++;
     }
   /* blend buffer onto image */
   if (blur > 0)
     {
	ImlibImage im2;
	
	im2.data = tmp;
	im2.w = rmap->cols;
	im2.h = rmap->rows;
	__imlib_BlurImage(&im2, blur);
	tmp = im2.data;
     }
   if (IMAGE_HAS_ALPHA(im))
      __imlib_BlendRGBAToData(tmp, rmap->cols, rmap->rows,
			      im->data, im->w, im->h,
			      0, 0, drx, dry, rmap->cols, rmap->rows,
			      1, NULL, OP_COPY);
   else
      __imlib_BlendRGBAToData(tmp, rmap->cols, rmap->rows,
			      im->data, im->w, im->h,
			      0, 0, drx, dry, rmap->cols, rmap->rows,
			      0, NULL, OP_COPY);
   free(tmp);
   __imlib_destroy_font_raster(rmap);   
}

