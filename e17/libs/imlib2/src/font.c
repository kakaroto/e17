#include "config.h"
#include "common.h"
#include <X11/Xlib.h>
#include "colormod.h"
#include "image.h"
#include "blend.h"
#ifdef HAVE_FREETYPE_FREETYPE_H
#include <freetype/freetype.h>
#else
#include <freetype.h>
#endif
#include "font.h"
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include "file.h"
#include "updates.h"
#include "rgbadraw.h"
#include "rotate.h"

#define TT_VALID( handle )  ( ( handle ).z != NULL )

/* cached font list and font path */
static ImlibFont  *fonts = NULL;
static int         fpath_num = 0;
static char      **fpath = NULL;
static TT_Engine   engine;
static char        have_engine = 0;
static int         font_cache_size = 0;

/* lookupt table of raster_map -> RGBA Alpha values */
static int rend_lut[9] = 
{ 0, 64, 128, 192, 256, 256, 256, 256, 256};

/* create an rmap of width and height */
TT_Raster_Map *
__imlib_create_font_raster(int width, int height)
{
   TT_Raster_Map      *rmap;
   
   rmap = malloc(sizeof(TT_Raster_Map));
   if (!rmap)
      return NULL;
   rmap->width = (width + 3) & -4;
   rmap->rows = height;
   rmap->flow = TT_Flow_Up;
   rmap->cols = rmap->width;
   rmap->size = rmap->rows * rmap->width;
   if (rmap->size <= 0)
     {
	free(rmap);
	return NULL;
     }
   rmap->bitmap = malloc(rmap->size);
   if (!rmap->bitmap)
     {
	free(rmap);
	return NULL;
     }
   memset(rmap->bitmap, 0, rmap->size);
   return rmap;
}

/* free the rmap */
void
__imlib_destroy_font_raster(TT_Raster_Map * rmap)
{
   if (rmap->bitmap)
      free(rmap->bitmap);
   free(rmap);
}

void
__imlib_add_font_path(const char *path)
{
   fpath_num++;
   if (!fpath)
      fpath = malloc(sizeof(char *));
   else
      fpath = realloc(fpath, (fpath_num * sizeof(char *)));
   fpath[fpath_num - 1] = strdup(path);
}

void
__imlib_del_font_path(const char *path)
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
__imlib_list_font_path(int *num_ret)
{
   *num_ret = fpath_num;
   return fpath;
}

ImlibFont *
__imlib_find_cached_font(const char *fontname)
{
   ImlibFont *pf, *f;
   
   pf = NULL;
   f = fonts;
   while(f)
     {
	if (!strcmp(f->name, fontname))
	  {
	     /* if it's not the top of the list - move it there */
	     if (pf)
	       {
		  pf->next = f->next;
		  f->next = fonts;
		  fonts = f;
	       }
	     return f;
	  }
	pf = f;
	f = f->next;
     }
   return NULL;
}

ImlibFont *
__imlib_load_font(const char *fontname)
{
   ImlibFont *f;
   TT_Error            error;
   TT_CharMap          char_map;
   TT_Glyph_Metrics    metrics;
   TT_Instance_Metrics imetrics;
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
   name = malloc((j + 1) * sizeof(char));
   memcpy(name, fontname, j);
   name[j] = 0;
   /* find file if it exists */
   tmp = malloc(strlen(name) + 4 + 1);
   if (!tmp)
     {
	free(name);
	return NULL;
     }
   sprintf(tmp, "%s.ttf", name);
   if (__imlib_FileIsFile(tmp))
      file = strdup(tmp);
   else
     {
	sprintf(tmp, "%s.TTF", name);
	if (__imlib_FileIsFile(tmp))
	   file = strdup(tmp);
	else
	  {
	     sprintf(tmp, "%s", name);
	     if (__imlib_FileIsFile(tmp))
		file = strdup(tmp);
	  }
     }
   free(tmp);
   if (!file)
     {
	for (j = 0; (j < fpath_num) && (!file); j++)
	  {
	     tmp = malloc(strlen(fpath[j]) + 1 + strlen(name) + 4 + 1);
	     if (!tmp)
	       {
		  free(name);
		  return NULL;
	       }
	     else
	       {
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
	       }
	     free(tmp);
	  }
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
   f->mem_use = 0;
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
      TT_Get_CharMap(f->face, 0, &char_map);
   f->num_glyph = 256;
   f->glyphs = (TT_Glyph *)malloc(f->num_glyph * sizeof(TT_Glyph));
   memset(f->glyphs, 0, f->num_glyph * sizeof(TT_Glyph));
   
   f->glyphs_cached_right = 
      (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_right, 0, f->num_glyph * sizeof(TT_Raster_Map *));
   
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
   /* work around broken fonts - some just have wrong ascent and */
   /* descent members */
   if (((f->ascent == 0) && (f->descent == 0)) || (f->ascent == 0))
     {
	f->ascent = f->max_ascent / 64;
	f->descent = -f->max_descent / 64;
     }
   /* all ent well in loading, so add to head of font list and return */
   f->next = fonts;
   fonts = f;
   /* we dont need the file handle hanging around so flush it out */
   TT_Flush_Face(f->face);
   return f;
}

void
__imlib_calc_size(ImlibFont *f, int *width, int *height, const char *text)
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
__imlib_render_str(ImlibImage *im, ImlibFont *fn, int drx, int dry, const char *text,
		   DATA8 r, DATA8 g, DATA8 b, DATA8 a,
		   char dir, double angle, int *retw, int *reth, int blur, 
		   int *nextx, int *nexty, ImlibOp op)
{
   DATA32              lut[9], *p, *tmp;
   TT_Glyph_Metrics    metrics;
   TT_F26Dot6          x, y, xmin, ymin, xmax, ymax;
   int                 w, h, i, ioff, iread, tw, th;
   char               *off, *read, *_off, *_read;
   int                 x_offset, y_offset;
   unsigned char       j;
   TT_Raster_Map      *rtmp = NULL, *rmap;
   ImlibImage          im2;

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

   /* figure out the size this text string is going to be */
   __imlib_calc_size(fn, &w, &h, text);
   tw = w; th = h;
   switch(dir)
     {
     case 0:
     case 1:
	if (retw)
	   *retw = tw;
	if (reth)
	   *reth = th;
	if (nexty)
	   *nexty = fn->ascent + fn->descent;
	if (nextx)
	  {
	     j = text[strlen(text) - 1];
	     TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
	     *nextx = w - x_offset + (metrics.advance / 64) - 
		(metrics.bbox.xMax / 64);
	  }
	break;
     case 2:
     case 3:
	tw = h; th = w;
	if (retw)
	   *retw = tw;
	if (reth)
	   *reth = th;
	if (nextx)
	   *nextx = fn->ascent + fn->descent;
	if (nexty)
	  {
	     j = text[strlen(text) - 1];
	     TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
	     *nexty = w - x_offset + (metrics.advance / 64) - 
		(metrics.bbox.xMax / 64);
	  }
	break;
     case 4:
	{
	   double sa, ca;
	   double x1, x2, xt;
	   double y1, y2, yt;
	   sa = sin(angle);
	   ca = cos(angle);

	   x1 = x2 = 0.0;
	   xt = ca * w;
	   if (xt < x1) x1 = xt;
	   if (xt > x2) x2 = xt;
	   xt = -(sa * h);
	   if (xt < x1) x1 = xt;
	   if (xt > x2) x2 = xt;
	   xt = ca * w - sa * h;
	   if (xt < x1) x1 = xt;
	   if (xt > x2) x2 = xt;
	   tw = (int)(x2 - x1);

	   y1 = y2 = 0.0;
	   yt = sa * w;
	   if (yt < y1) y1 = yt;
	   if (yt > y2) y2 = yt;
	   yt = ca * h;
	   if (yt < y1) y1 = yt;
	   if (yt > y2) y2 = yt;
	   yt = sa * w + ca * h;
	   if (yt < y1) y1 = yt;
	   if (yt > y2) y2 = yt;
	   th = (int)(y2 - y1);
	}
	if (retw)
	   *retw = tw;
	if (reth)
	   *reth = th;
	if (nexty)
	   *nexty = fn->ascent + fn->descent;
	if (nextx)
	  {
	     j = text[strlen(text) - 1];
	     TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
	     *nextx = w - x_offset + (metrics.advance / 64) - 
		(metrics.bbox.xMax / 64);
	  }
	break;
     default:
	break;
     }
   /* if the text is completely outside the image - give up */
   if (((drx + tw) <= 0) || ((dry + th) <= 0))
      return;
   /* create a scratch pad for it */
   rmap = __imlib_create_font_raster(w, h);
   if (rmap)
     {
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
	     
	     rtmp = fn->glyphs_cached_right[j];
	     if (!rtmp)
	       {
#if 1
		  rtmp = __imlib_create_font_raster(((xmax - xmin) / 64) + 1,
						    ((ymax - ymin) / 64) + 1);
		  TT_Get_Glyph_Pixmap(fn->glyphs[j], rtmp, -xmin, -ymin);
#else		  
		  TT_Raster_Map *rbuf;
		  
		  rbuf = __imlib_create_font_raster(((xmax - xmin) / 64) + 1, 
						    ((ymax - ymin) / 64) + 1);
		  rtmp = __imlib_create_font_raster(((xmax - xmin) / 64) + 1, 
						    ((ymax - ymin) / 64) + 1);
		  TT_Get_Glyph_Bitmap(fn->glyphs[j], rbuf, -xmin, -ymin);
		  for (y = 0; y < rtmp->rows; y++)
		    {
		       for (x = 0; x < rtmp->cols; x++)
			 {
			    int val;
			    
			    val = (((DATA8 *)rbuf->bitmap)[(y * rbuf->cols) + (x >> 3)] >> (7 - (x - ((x >> 3) << 3))) & 0x1);
			    ((DATA8 *)(rtmp->bitmap))[(y * rtmp->cols) + x] = val * 8;
			    printf("%i", val);
			 }
		       printf("\n");
		    }
		  
		  __imlib_destroy_font_raster(rbuf);
#endif
		  fn->glyphs_cached_right[j] = rtmp;
		  fn->mem_use += 
		     (((xmax - xmin) / 64) + 1) *
		     (((ymax - ymin) / 64) + 1);
	       }
	     if (rtmp)
	       {
		  
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
	       }
	     x_offset += metrics.advance / 64;
	  }
	/* temporary RGBA buffer to build */
	if ((rmap->rows > 0) && (rmap->cols > 0))
	  {
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
	     im2.data = tmp;
	     im2.w = rmap->cols;
	     im2.h = rmap->rows;
	     if (blur > 0)
		__imlib_BlurImage(&im2, blur);
	     switch(dir)
	       {
	       case 0: /* to right */
		  angle = 0.0;
		  break;
	       case 1: /* to left */
		  angle = 0.0;
		  __imlib_FlipImageBoth(&im2);
		  break;
	       case 2: /* to down */
		  angle = 0.0;
		  __imlib_FlipImageDiagonal(&im2, 1);
		  break;
	       case 3: /* to up */
		  angle = 0.0;
		  __imlib_FlipImageDiagonal(&im2, 2);
		  break;
	       default:
		  break;
	       }
	     tmp = im2.data;
	     if (angle == 0.0) {
		__imlib_BlendRGBAToData(tmp, im2.w, im2.h,
					im->data, im->w, im->h,
					0, 0, drx, dry, im2.w, im2.h,
					1, IMAGE_HAS_ALPHA(im), NULL, op, 0);
	     } else {
		int xx, yy;
		double sa, ca;
		sa = sin(angle);
		ca = cos(angle);
		xx = drx;
		yy = dry;
		if (sa > 0.0)
		   xx += sa * im2.h;
		else
		   yy -= sa * im2.w;
		if (ca < 0.0) {
		   xx -= ca * im2.w;
		   yy -= ca * im2.h;
		}
		__imlib_BlendImageToImageSkewed(&im2, im, 1, 1,
					IMAGE_HAS_ALPHA(im),
					0, 0, im2.w, im2.h,
					xx, yy, (w * ca), (w * sa), 0, 0,
					NULL, op);
	     }
	     free(tmp);
	  }
	__imlib_destroy_font_raster(rmap);   
     }
}

int
__imlib_char_pos(ImlibFont *fn, const char *text, int x, int y,
		 int *cx, int *cy, int *cw, int *ch)
{
   int                 i, px, ppx;
   TT_Glyph_Metrics    gmetrics;

   if ((y < 0) || (y > (fn->ascent + fn->descent)))
      return -1;
   if (cy)
      *cy = 0;
   if (ch)
      *ch = fn->ascent + fn->descent;
   ppx = 0;
   px = 0;
   for (i = 0; text[i]; i++)
     {
	unsigned char       j;
	
	j = text[i];
	if (!TT_VALID(fn->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(fn->glyphs[j], &gmetrics);
	ppx = px;
	if (i == 0)
	   px += ((-gmetrics.bearingX) / 64);
	if (text[i + 1] == 0)
	   px += (gmetrics.bbox.xMax / 64);
	else
	   px += gmetrics.advance / 64;
	if ((x >= ppx) && (x < px))
	  {
	     if (cx)
		*cx = ppx;
	     if (cw)
		*cw = px - ppx;
	     return i;
	  }
     }
   return -1;
}

char **
__imlib_list_fonts(int *num_ret)
{
   int i, j, d, l = 0;
   char **list = NULL, **dir, *path;
   TT_Error error;
   
   /* if we dont have a truetype font engine yet - make one */
   if (!have_engine)
     {
	error = TT_Init_FreeType(&engine);
	if (error)
	   return NULL;
	have_engine = 1;
     }
   for (i = 0; i < fpath_num; i++)
     {
	dir = __imlib_FileDir(fpath[i], &d);
	if (dir)
	  {
	     for (j = 0; j < d; j++)
	       {
		  if (__imlib_FileIsFile(dir[j]))
		    {
		       TT_Face f;
		       
		       path = malloc(strlen(fpath[i]) + 1 + strlen(dir[j] + 1));
		       strcpy(path, fpath[i]);
		       strcat(path, "/");
		       strcat(path, dir[j]);
		       error = TT_Open_Face(engine, path, &f);
		       free(path);
		       if (!error)
			 {
			    TT_Close_Face(f);
			    l++;
			    if (list)
			       list = realloc(list, sizeof(char *) * l);
			    else
			       list = malloc(sizeof(char *));
			    list[l - 1] = strdup(dir[j]);
			 }
		    }
	       }
	     free(dir);
	  }
     }
   *num_ret = l;
   return list;
}

void
__imlib_free_font_list(char **list, int num)
{
   __imlib_FileFreeDirList(list, num);
}

int
__imlib_get_cached_font_size(void)
{
   ImlibFont *f;
   int num = 0;
   
   f = fonts;
   while(f)
     {
	if (f->references == 0)
	   num += f->mem_use;
	f = f->next;
     }
   return num;
}

void
__imlib_flush_font_cache(void)
{
   int size;
   ImlibFont *flast, *f;
   
   size = __imlib_get_cached_font_size();
   while (size > font_cache_size)
     {
	flast = NULL;
	f = fonts;
	while (f)
	  {
	     if (f->references == 0)
		flast = f;
	     f = f->next;
	  }
	if (flast)
	  {
	     size -= flast->mem_use;
	     __imlib_nuke_font(flast);
	  }
     }
}

void
__imlib_purge_font_cache(void)
{
   ImlibFont *pf, *f;
   
   f = fonts;
   while(f)
     {
	pf = f;
	f = f->next;
	if (pf->references == 0)
	   __imlib_nuke_font(pf);
     }
   if (!fonts)
     {
	if (have_engine)
	  {
	     TT_Done_FreeType(engine);
	     have_engine = 0;
	  }
     }
}

int
__imlib_get_font_cache_size(void)
{
   return font_cache_size;
}

void
__imlib_set_font_cache_size(int size)
{
   if (size < 0)
      size = 0;
   font_cache_size = size;
   __imlib_flush_font_cache();
}

void
__imlib_free_font(ImlibFont *font)
{
   /* defererence */
   font->references--;
   /* if still referenced exit here */
   if (font->references > 0)
      return;
   __imlib_flush_font_cache();
}

void
__imlib_nuke_font(ImlibFont *font)
{
   int                 i;
   ImlibFont          *f, *pf;
   
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
	if (!TT_VALID(font->glyphs[i]))
	   TT_Done_Glyph(font->glyphs[i]);
     }
   /* free glyph info */
   free(font->glyphs);
   /* free glyph cache arrays */
   if (font->glyphs_cached_right)
      free(font->glyphs_cached_right);
   /* free font struct & name */
   free(font->name);
   free(font);
}

