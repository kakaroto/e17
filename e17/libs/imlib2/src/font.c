#include "config.h"
#include "common.h"
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
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* cached font list and font path */
static ImlibFont  *fonts = NULL;
static int         fpath_num = 0;
static char      **fpath = NULL;
static TT_Engine   engine;
static char        have_engine = 0;
static int         font_cache_size = 0;

#ifdef  XMB_FONT_CACHE
static ImlibXmbHash *hashes = NULL;
#endif

/* lookupt table of raster_map -> RGBA Alpha values */
static int rend_lut[9] = 
{ 0, 64, 128, 192, 256, 256, 256, 256, 256};

#define XMB_BLEND(r1, g1, b1, a1, dest) \
bb = ((dest)     ) & 0xff;\
gg = ((dest) >> 8) & 0xff;\
rr = ((dest) >> 16) & 0xff;\
aa = ((dest) >> 24) & 0xff;\
tmp = ((r1) - rr) * (a1);\
nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);\
tmp = ((g1) - gg) * (a1);\
ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);\
tmp = ((b1) - bb) * (a1);\
nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);\
tmp = (a1) + aa;\
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

#define XMB_BLEND_ADD(r1, g1, b1, a1, dest) \
bb = ((dest)     ) & 0xff;\
gg = ((dest) >> 8) & 0xff;\
rr = ((dest) >> 16) & 0xff;\
aa = ((dest) >> 24) & 0xff;\
tmp = rr + (((r1) * (a1)) >> 8);\
nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
tmp = gg + (((g1) * (a1)) >> 8);\
ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
tmp = bb + (((b1) * (a1)) >> 8);\
nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
tmp = (a1) + aa;\
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

#define XMB_BLEND_SUB(r1, g1, b1, a1, dest) \
bb = ((dest)     ) & 0xff;\
gg = ((dest) >> 8) & 0xff;\
rr = ((dest) >> 16) & 0xff;\
aa = ((dest) >> 24) & 0xff;\
tmp = rr - (((r1) * (a1)) >> 8);\
nr = tmp & (~(tmp >> 8));\
tmp = gg - (((g1) * (a1)) >> 8);\
ng = tmp & (~(tmp >> 8));\
tmp = bb - (((b1) * (a1)) >> 8);\
nb = tmp & (~(tmp >> 8));\
tmp = (a1) + aa;\
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

#define XMB_BLEND_RE(r1, g1, b1, a1, dest) \
bb = ((dest)     ) & 0xff;\
gg = ((dest) >> 8) & 0xff;\
rr = ((dest) >> 16) & 0xff;\
aa = ((dest) >> 24) & 0xff;\
tmp = rr + ((((r1) - 127) * (a1)) >> 7);\
nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));\
tmp = gg + ((((g1) - 127) * (a1)) >> 7);\
ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));\
tmp = bb + ((((b1) - 127) * (a1)) >> 7);\
nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));\
tmp = (a1) + aa;\
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

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
__imlib_find_cached_font(const char *ttffontname, const char *xfontname, int mode)
{
   ImlibFont *pf, *f;
   
   if ((mode & IMLIB_FONT_TYPE_TTF) && !ttffontname)
     return NULL;

   if ((mode & IMLIB_FONT_TYPE_X) && !xfontname)
     return NULL;

   pf = NULL;
   f = fonts;
   while(f)
     {
	int             m;
	ImlibFont      *tf;

	m = 0;
	switch(f->type)
	  {
	  case IMLIB_FONT_TYPE_TTF:
	     if (mode == IMLIB_FONT_TYPE_TTF &&
		  !strcmp(f->hdr.name, ttffontname))
		m++;
	     break;
	  case IMLIB_FONT_TYPE_X:
	     if (mode == IMLIB_FONT_TYPE_X &&
		  !strcmp(f->hdr.name, xfontname))
		m++;
	     break;
	  case IMLIB_FONT_TYPE_TTF_X:
	     if (mode == IMLIB_FONT_TYPE_TTF_X)
		{
		  tf = f->xf.ttffont;
		  if (!strcmp(tf->hdr.name, ttffontname) && 
		  	!strcmp(f->hdr.name, xfontname))
		    m++;
		}
	     break;
	  }

	if (m)
	  {
	     /* if it's not the top of the list - move it there */
	     if (pf)
	       {
		  pf->hdr.next = f->hdr.next;
		  f->hdr.next = fonts;
		  fonts = f;
	       }
	     return f;
	  }
	pf = f;
	f = f->hdr.next;
     }
   return NULL;
}

ImlibFont *
__imlib_load_font(const char *fontname)
{
   ImlibFont *fn;
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
   ImlibTtfFont       *f;

   fn = __imlib_find_cached_font(fontname, NULL, IMLIB_FONT_TYPE_TTF);
   if (fn)
     {
	/* reference it up by one and return it */
	fn->hdr.references++;
	return fn;
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
   f = (ImlibTtfFont *)malloc(sizeof(ImlibTtfFont));
   /* put in name and references */
   f->type = IMLIB_FONT_TYPE_TTF;
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
   fonts = (ImlibFont *)f;
   /* we dont need the file handle hanging around so flush it out */
   TT_Flush_Face(f->face);

   return (ImlibFont *)f;
}

ImlibFont     *
__imlib_load_xfontset(Display *display, const char *xfontsetname)
{
   ImlibXFontSet       *fn;
   int                  i, missing_count;
   char               **missing_list, *def_string;
#ifdef	XMB_FONT_CACHE
   int                  XMBflag = 0;
#endif

   fn = (ImlibXFontSet *)malloc(sizeof(ImlibXFontSet));
   fn->type = IMLIB_FONT_TYPE_X;
   fn->name = strdup(xfontsetname);
   fn->references = 1;
   fn->ttffont = NULL;

   fn->xfontset = XCreateFontSet(display, fn->name,
   				  &missing_list, &missing_count, &def_string);

   if (missing_count)
     XFreeStringList(missing_list);

   if (!fn->xfontset)
     {
	free(fn->name);
	free(fn);
	return NULL;
     }

   fn->font_count = XFontsOfFontSet(fn->xfontset, &fn->font_struct, &fn->font_name);
   fn->ascent = fn->descent = 0;
   fn->max_ascent = fn->max_descent = fn->max_width = 0;
   for (i = 0; i < fn->font_count; i++)
     {
	fn->ascent = MAX(fn->font_struct[i]->ascent, fn->ascent);
	fn->descent = MAX(fn->font_struct[i]->descent, fn->descent);
	fn->max_ascent = MAX(fn->font_struct[i]->max_bounds.ascent, fn->max_ascent);
	fn->max_descent = MAX(fn->font_struct[i]->max_bounds.descent, fn->max_descent);
	fn->max_width = MAX(fn->font_struct[i]->max_bounds.width, fn->max_width);

#ifdef	XMB_FONT_CACHE
	if (fn->font_struct[i]->min_byte1 && fn->font_struct[i]->min_byte1)
		XMBflag = 1;
#endif
     }

#ifdef	XMB_FONT_CACHE
   fn->hash = __imlib_create_font_hash_table(xfontsetname, XMBflag);
#endif

   fn->next = fonts;
   fonts = (ImlibFont *)fn;

   return (ImlibFont *)fn;
}

#ifdef	XMB_FONT_CACHE
ImlibXmbHash *
__imlib_create_font_hash_table(const char *xfontsetname, int type)
{
   int i, size;
   ImlibXmbHash	*h;

   h = hashes;
   while(h)
     if (!strcmp(xfontsetname, h->name))
	{
	  h->references++;
	  return h;
	}
     else
	h = h->next;

   h = malloc(sizeof(ImlibXmbHash));
   h->next = hashes;
   hashes = h;

   h->next = NULL;
   h->name = strdup(xfontsetname);
   h->references = 1;

   h->type = type;
   if (type)
     h->size = XMB_HASH_SIZE;
   else
     h->size = 256;
   h->hash = (ImlibXmbHashElm **)malloc( sizeof(ImlibXmbHashElm *) * h->size);
   for (i=0; i<h->size; i++)
     h->hash[i] = NULL;

   h->hash_count = 0;
   h->collision_count = 0;
   h->mem_use = sizeof(ImlibXmbHashElm *) * h->size;

   return h;
}
#endif

ImlibFont *
__imlib_clone_cached_font(ImlibFont *fn)
{
   ImlibFont   *f;

   switch (fn->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	f = (ImlibFont *)malloc(sizeof(ImlibTtfFont));
	memcpy(f, fn, sizeof(ImlibTtfFont));
	if (fn->ttf.num_glyph)
	  {
   	    f->ttf.glyphs = (TT_Glyph *)malloc(f->ttf.num_glyph * sizeof(TT_Glyph));
	    memcpy(f->ttf.glyphs, fn->ttf.glyphs,
		    f->ttf.num_glyph * sizeof(TT_Glyph));
	    f->ttf.glyphs_cached_right = (TT_Raster_Map **)malloc(f->ttf.num_glyph * sizeof(TT_Raster_Map *));
	    memcpy(f->ttf.glyphs_cached_right, fn->ttf.glyphs_cached_right,
		    f->ttf.num_glyph * sizeof(TT_Raster_Map *));
	  }
        break;
     case IMLIB_FONT_TYPE_X:
     case IMLIB_FONT_TYPE_TTF_X:
	f = (ImlibFont *)malloc(sizeof(ImlibXFontSet));
	memcpy(f, fn, sizeof(ImlibXFontSet));
/*
	if (f->xf.ttffont)
	  f->xf.ttffont->hdr.references++;
*/
#ifdef  XMB_FONT_CACHE
	if (fn->xf.hash != NULL)
	  fn->xf.hash->references++;
#endif
        break;
     default:
	return NULL;
     }

   f->hdr.references = 1;
   if (fn->hdr.name)
     f->hdr.name = strdup(fn->hdr.name);

   f->hdr.next = fonts;
   fonts = f;

   return f;
}

void
__imlib_calc_size(ImlibFont *fn, int *width, int *height, const char *text)
{
   int                 i, ascent, descent, pw, ph;
   TT_Glyph_Metrics    gmetrics;
   ImlibTtfFont       *f;
   
   switch (fn->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	f = (ImlibTtfFont *)fn;
        break;
     case IMLIB_FONT_TYPE_X:
	*width = *height = 0;
	return;
     case IMLIB_FONT_TYPE_TTF_X:
	f = (ImlibTtfFont *)fn->xf.ttffont;
        break;
     default:
	*width = *height = 0;
	return;
     }

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
	  {
	     /* special code just for tridge - maaaaate */
	     if (gmetrics.bbox.xMax == 0)
	       pw += gmetrics.advance / 64;
	     else
	       pw += (gmetrics.bbox.xMax / 64);
	  }
	else
	   pw += gmetrics.advance / 64;
     }
   *width = pw;
   *height = ph;
}

void
__imlib_calc_advance(ImlibFont *fn, int *adv_w, int *adv_h, const char *text)
{
   int                 i, ascent, descent, pw, ph;
   TT_Glyph_Metrics    gmetrics;
   ImlibTtfFont       *f;
   
   switch (fn->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	f = (ImlibTtfFont *)fn;
        break;
     case IMLIB_FONT_TYPE_X:
	*adv_w = *adv_h = 0;
	return;
     case IMLIB_FONT_TYPE_TTF_X:
	f = (ImlibTtfFont *)fn->xf.ttffont;
        break;
     default:
	*adv_w = *adv_h = 0;
	return;
     }
   
   ascent = f->ascent;
   descent = f->descent;
   pw = 0;
   ph = ascent + descent;
   
   for (i = 0; text[i]; i++)
     {
	unsigned char       j;
	
	j = text[i];	
	if (!TT_VALID(f->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(f->glyphs[j], &gmetrics);
	if (i == 0)
	   pw += ((-gmetrics.bearingX) / 64);
	pw += gmetrics.advance / 64;
     }
   *adv_w = pw;
   *adv_h = ph;
}

int
__imlib_calc_inset(ImlibFont *fn, const char *text)
{
   int                 i;
   TT_Glyph_Metrics    gmetrics;
   ImlibTtfFont       *f;
   
   switch (fn->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	f = (ImlibTtfFont *)fn;
        break;
     case IMLIB_FONT_TYPE_X:
	return 0;
     case IMLIB_FONT_TYPE_TTF_X:
	f = (ImlibTtfFont *)fn->xf.ttffont;
        break;
     default:
	return 0;
     }
   
   for (i = 0; text[i]; i++)
     {
	unsigned char       j;
	
	j = text[i];
	if (!TT_VALID(f->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(f->glyphs[j], &gmetrics);
	return ((-gmetrics.bearingX) / 64);
     }
   return 0;
}

void
__imlib_render_str(ImlibImage *im, ImlibFont *f, int drx, int dry, const char *text,
		   DATA8 r, DATA8 g, DATA8 b, DATA8 a,
		   char dir, double angle, int *retw, int *reth, int blur, 
		   int *nextx, int *nexty, ImlibOp op,
		   int clx, int cly, int clw, int clh)
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
   ImlibTtfFont       *fn;

   switch (f->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	fn = (ImlibTtfFont *)f;
        break;
     case IMLIB_FONT_TYPE_X:
	*retw = *reth = *nextx = *nexty = 0;
	return;
     case IMLIB_FONT_TYPE_TTF_X:
	fn = (ImlibTtfFont *)f->xf.ttffont;
        break;
     default:
	*retw = *reth = *nextx = *nexty = 0;
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
   __imlib_calc_size(f, &w, &h, text);
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

   /* if we draw outside the image from here - give up */
   if ((drx > im->w) || (dry > im->h))
      return;

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
		  rtmp = __imlib_create_font_raster(((xmax - xmin) / 64) + 1,
						    ((ymax - ymin) / 64) + 1);
		  TT_Get_Glyph_Pixmap(fn->glyphs[j], rtmp, -xmin, -ymin);
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
	     SET_FLAG(im2.flags, F_HAS_ALPHA);
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
		__imlib_BlendImageToImage(&im2, im, 0, 1, IMAGE_HAS_ALPHA(im), 
					  0, 0, im2.w, im2.h,
					  drx, dry, im2.w, im2.h,
					  NULL, OP_COPY, clx, cly, clw, clh);
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
						NULL, op, 
						clx, cly, clw, clh);
	     }
	     free(tmp);
	  }
	__imlib_destroy_font_raster(rmap);   
     }
}

void
__imlib_xfd_draw_str(Display *display, Drawable drawable, Visual *v, int depth,
		     Colormap cm, ImlibImage *im, ImlibFont *fn, int x, int y,
		     const char *text, DATA8 r, DATA8 g, DATA8 b, DATA8 a,
		     char dir, double angle, char blend,
		     ImlibColorModifier *cmod, char hiq, char dmask,
		     ImlibOp op, int *retw, int *reth, int *nextx, int *nexty,
		     int clx, int cly, int clw, int clh)
{
   ImlibImage          *im2;
   ImlibImagePixmap    *ip;
   XRectangle           i_ret, l_ret;
   int                  x1, y1, lbearing;
#ifndef	XMB_FONT_CACHE
   Pixmap               p, m;
   XGCValues            gcv;
   GC                   gc;
#endif

   XmbTextExtents(fn->xf.xfontset, text, strlen(text), &i_ret, &l_ret);
   switch(dir)
     {
     case 0:
     case 1:
	if (retw)
	  *retw = l_ret.width;
	if (reth)
	  *reth = l_ret.height;
	if (nextx)
	  *nextx = i_ret.width;
	if (nexty)
	  *nexty = i_ret.height;
	break;
     case 2:
     case 3:
	if (retw)
	  *retw = l_ret.height;
	if (reth)
	  *reth = l_ret.width;
	if (nextx)
	  *nextx = i_ret.height;
	if (nexty)
	  *nexty = i_ret.width;
	break;
     case 4:
	{
	   int tw, th;
	   double sa, ca;
	   double x1, x2, xt;
	   double y1, y2, yt;
	   sa = sin(angle);
	   ca = cos(angle);

	   x1 = x2 = 0.0;
	   xt = ca * l_ret.width;
	   if (xt < x1) x1 = xt;
	   if (xt > x2) x2 = xt;
	   xt = -(sa * i_ret.height);
	   if (xt < x1) x1 = xt;
	   if (xt > x2) x2 = xt;
	   xt = ca * l_ret.width - sa * i_ret.height;
	   if (xt < x1) x1 = xt;
	   if (xt > x2) x2 = xt;
	   tw = (int)(x2 - x1);

	   y1 = y2 = 0.0;
	   yt = sa * l_ret.width;
	   if (yt < y1) y1 = yt;
	   if (yt > y2) y2 = yt;
	   yt = ca * i_ret.height;
	   if (yt < y1) y1 = yt;
	   if (yt > y2) y2 = yt;
	   yt = sa * l_ret.width + ca * i_ret.height;
	   if (yt < y1) y1 = yt;
	   if (yt > y2) y2 = yt;
	   th = (int)(y2 - y1);
	   if (retw)
	     *retw = tw;
	   if (reth)
	     *reth = th;
	   if (nextx)
	     *nextx = i_ret.width;
	   if (nexty)
	     *nexty = i_ret.height;
	}
     }

   /* if we draw outside the image from here - give up */
   if ((x > im->w) || (y > im->h))
      return;

   /* if the text is completely outside the image - give up */
   if (((x + *retw) <= 0) || ((y + *reth) <= 0))
      return;

   im2 = __imlib_CreateImage(MAX(i_ret.width, l_ret.width),
		   MAX(i_ret.height, l_ret.height), NULL);
   im2->data = malloc(im2->w * im2->h * sizeof(DATA32));

#ifdef	XMB_FONT_CACHE
   __imlib_xfd_build_str_image(display, drawable, v, fn, im2, text, r, g, b, a);
#else
   m = XCreatePixmap(display, drawable, im2->w, im2->h, 1);
   gcv.foreground = 0;
   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(display, m, 0, &gcv);
   XFillRectangle(display, m, gc, 0, 0, im2->w, im2->h);
   XSetForeground(display, gc, 1);
   XmbDrawString(display, m, fn->xf.xfontset, gc, 0, fn->xf.ascent,
		 text, strlen(text));

   __imlib_GrabDrawableToRGBA(im2->data, 0, 0, im2->w, im2->h, display, m, NULL,
			      v, NULL, 1, 0, 0, im2->w, im2->h, 0, 0);
#endif
   SET_FLAG(im2->flags, F_HAS_ALPHA);

/*
   printf( "i_ret.x=%d, i_ret.y=%d, i_ret.w=%d, i_ret.h=%d, ascent=%d\n",
		   i_ret.x, i_ret.y, i_ret.width, i_ret.height, fn->xf.ascent);
   printf( "l_ret.x=%d, l_ret.y=%d, l_ret.w=%d, l_ret.h=%d, dascent=%d\n",
		   l_ret.x, l_ret.y, l_ret.width, l_ret.height, fn->xf.descent);
*/
   switch(dir)
     {
      case 0: /* to right */
	angle = 0.0;
	break;
      case 1: /* to left */
	angle = 0.0;
	__imlib_FlipImageBoth(im2);
	break;
      case 2: /* to down */
	angle = 0.0;
	__imlib_FlipImageDiagonal(im2, 1);
	break;
      case 3: /* to up */
	angle = 0.0;
	__imlib_FlipImageDiagonal(im2, 2);
	break;
      default:
	break;
     }
   if (angle == 0.0) {
      __imlib_BlendImageToImage(im2, im, 0, 1, IMAGE_HAS_ALPHA(im), 
				0, 0, im2->w, im2->h,
				x, y, im2->w, im2->h,
				NULL, OP_COPY, clx, cly, clw, clh);
   } else {
      int xx, yy;
      double sa, ca;
      sa = sin(angle);
      ca = cos(angle);
      xx = x;
      yy = y;
      if (sa > 0.0)
	xx += sa * im2->h;
      else
	yy -= sa * im2->w;
      if (ca < 0.0) {
	 xx -= ca * im2->w;
	 yy -= ca * im2->h;
      }
      __imlib_BlendImageToImageSkewed(im2, im, 1, 1,
				      IMAGE_HAS_ALPHA(im),
				      0, 0, im2->w, im2->h,
				      xx, yy, (im2->w * ca), (im2->w * sa), 0, 0,
				      NULL, op, 
				      clx, cly, clw, clh);
   }

#ifndef	XMB_FONT_CACHE
   XFreeGC(display, gc);
   XFreePixmap(display, m);
#endif
   __imlib_FreeImage(im2);
}

#ifdef	XMB_FONT_CACHE
void
__imlib_xfd_build_str_image(Display *display, Drawable drawable, Visual *v,
			    ImlibFont *fn, ImlibImage *im, const char *text,
			    DATA8 r, DATA8 g, DATA8 b, DATA8 a)
{
   int                 i;
   int                 x;
   Pixmap              pix=(Pixmap)NULL;
   GC                gc;
   int                 strlen_text;

   strlen_text = strlen(text);
   x = 0;
   for (i=0; i<strlen_text; i++ )
     {
	int               len;
	int               j, k;
	wchar_t           wc;
	unsigned long     hash;
	ImlibXmbHash     *h;
	ImlibXmbHashElm  *hel, *hel2;
	ImlibImage       *cim;
	
	len = mblen(text+i, MB_CUR_MAX);
	if (len < 0)
	  continue;
	
	if (mbtowc(&wc, text+i, len) == -1)
	  continue;
	
	/* create hash id */
	h = fn->xf.hash;
	if (h->type)
	  {
	     hash = (wc ^ (wc >> XMB_HASH_VAL1) ^ (wc << XMB_HASH_VAL2)) * 
	       XMB_HASH_VAL3;
	     hash += XMB_HASH_SIZE;
	     hash %= XMB_HASH_SIZE;
	  }
	else
	  hash = (unsigned char)text[i];
/*
 printf(" hash=%0x wc=%0x", hash, wc);
 printf(" c=");
 for (j=0; j<len; j++) printf("%c",text[i+j]);
 */
	
	/* search hash element */
	for (hel=h->hash[hash]; hel!=NULL; hel=hel->next)
	  {
	     if (hel->wc == wc)
	       break;
	     if (hel->next == NULL)
	       break;
	  }
	
	/* create new hash element */
	if (hel==NULL || (hel!=NULL && hel->wc != wc))
	  {
	     XRectangle    i_ret, l_ret;
	     
	     hel2 = (ImlibXmbHashElm *)malloc(sizeof(ImlibXmbHashElm));
	     hel2->wc = wc;
	     hel2->next = NULL;
	     
	     if (pix == (Pixmap)NULL )
	    {
	       XGCValues         gcv;
	       int               pw, ph;
	       
	       pw = fn->xf.max_width;
	       ph = fn->xf.max_ascent + fn->xf.max_descent;
	       pix = XCreatePixmap(display, drawable, pw, ph, 1);
	       gcv.foreground = 0;
	       gcv.subwindow_mode = IncludeInferiors;
	       gc = XCreateGC(display, pix, 0, &gcv);
	    }
	     
	     XwcTextExtents(fn->xf.xfontset, &wc, 1, &i_ret, &l_ret);
	     hel2->w = MAX(i_ret.width, l_ret.width);
	     hel2->h = MAX(i_ret.height, l_ret.height);
	     hel2->im = (DATA32 *)malloc(hel2->w * hel2->h * sizeof(DATA32));
	     for (j=0; j< hel2->w * hel2->h; j++ ) *(hel2->im + j) = 0;
	     XSetForeground(display, gc, 0);
	     XFillRectangle(display, pix, gc, 0, 0, hel2->w, hel2->h);
	     XSetForeground(display, gc, 1);
	     XwcDrawString(display, pix, fn->xf.xfontset, gc, 0, fn->xf.ascent,
			   &wc, 1);
	     __imlib_GrabDrawableToRGBA(hel2->im, 0, 0, hel2->w, hel2->h,
					display, pix, NULL, v, NULL, 1,
					0, 0, hel2->w, hel2->h, 0, 0);
	     if (hel==NULL)
	       {
		  h->hash[hash] = hel2;
	      h->hash_count++;
		  /* printf(" created!"); */
	       }
	     else if (hel->next==NULL)
	       {
		  hel->next = hel2;
		  h->collision_count++;
		  /* printf(" Collision!"); */
	       }
	     h->mem_use += sizeof(ImlibXmbHashElm);
	     h->mem_use += hel2->w * hel2->h * sizeof(DATA32);
	     
	  hel = hel2;
	  }
	/*
	 else
	 printf(" Found!");
	 printf(" p=%0x\n", hel);
	 */
	
      /* concatenate string image */
	for (j=0; j<hel->h && j<im->h; j++)
	  {
	     int   s, d;
	     DATA32 p1, p2;
	     
	     s = hel->w * j;
	     d = im->w * j;
	     p1 = (a << 24) | (r << 16) | (g << 8) | b;
	     p2 = (r << 16) | (g << 8) | b;
	     for (k=0; k<hel->w && (x + k)<im->w; k++)
	       {
		  if (hel->im[s + k] == 0xffffffff) im->data[d + x + k] = p1;
		  else im->data[d + x + k] = p2;
	       }
/*	     im->data[d + x + k] = hel->im[s + k];*/
	  }
	x += hel->w;
	
	if (len>1)
	  i += len -1;
     }
   
   if (pix != (Pixmap)NULL )
     {
	XFreeGC(display, gc);
	XFreePixmap(display, pix);
     }
}
#endif

int
__imlib_char_pos(ImlibFont *f, const char *text, int x, int y,
		 int *cx, int *cy, int *cw, int *ch)
{
   int                 i, px, ppx;
   TT_Glyph_Metrics    gmetrics;
   ImlibTtfFont       *fn;
   
   switch (f->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	fn = (ImlibTtfFont *)f;
        break;
     case IMLIB_FONT_TYPE_X:
	return -1;
     case IMLIB_FONT_TYPE_TTF_X:
	fn = (ImlibTtfFont *)f->xf.ttffont;
        break;
     default:
	return -1;
     }

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

void
__imlib_char_geom(ImlibFont *f, const char *text, int num,
		  int *cx, int *cy, int *cw, int *ch)
{
   int                 i, px, ppx;
   TT_Glyph_Metrics    gmetrics;
   ImlibTtfFont       *fn;
   
   switch (f->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	fn = (ImlibTtfFont *)f;
        break;
     case IMLIB_FONT_TYPE_X:
	*cx = *cy = *cw = *ch = 0;
	return;
     case IMLIB_FONT_TYPE_TTF_X:
	fn = (ImlibTtfFont *)f->xf.ttffont;
        break;
     default:
	*cx = *cy = *cw = *ch = 0;
	return;
     }

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
	if (i == num)
	  {
	     if (cx)
		*cx = ppx;
	     if (cw)
		*cw = px - ppx;
	     return;
	  }
     }
}

int
__imlib_xfd_char_pos(ImlibFont *f, const char *text, int x, int y,
		 int *cx, int *cy, int *cw, int *ch)
{
   int                 i, oldx;
   
   if (f->type != IMLIB_FONT_TYPE_X && f->type != IMLIB_FONT_TYPE_TTF_X)
     return -1;
   if ((y < 0) || (y > (f->xf.ascent + f->xf.descent)))
      return -1;

   if (cy)
      *cy = 0;
   if (ch)
      *ch = f->xf.ascent + f->xf.descent;

   oldx = 0;
   for (i = 0; i < strlen(text); i++)
     {
	int             len;
	XRectangle      i_ret, l_ret;

	len = mblen(text+i, MB_CUR_MAX);
	if (len < 0)
	  len = 1;

	XmbTextExtents(f->xf.xfontset, text, i+len, &i_ret, &l_ret);
	if (x >= oldx && x < i_ret.width)
	  {
	     if (cx)
		*cx = oldx;
	     if (cw)
		*cw = i_ret.width - oldx;
	     return i;
	  }
	oldx = i_ret.width;
	if (len > 1)
	  i += len - 1;
     }
   return -1;
}

void
__imlib_xfd_char_geom(ImlibFont *f, const char *text, int num,
		  int *cx, int *cy, int *cw, int *ch)
{
   int                 i, oldx;
   
   if (f->type != IMLIB_FONT_TYPE_X && f->type != IMLIB_FONT_TYPE_TTF_X)
     return;

   if (cy)
      *cy = 0;
   if (ch)
      *ch = f->xf.ascent + f->xf.descent;

   oldx = 0;
   for (i = 0; i < strlen(text); i++)
     {
	int             len;
	XRectangle      i_ret, l_ret;

	len = mblen(text+i, MB_CUR_MAX);
	if (len < 0)
	  len = 1;

	XmbTextExtents(f->xf.xfontset, text, i+len, &i_ret, &l_ret);
	if (num >= i && num < i+len)
	  {
	     if (cx)
		*cx = oldx;
	     if (cw)
		*cw = i_ret.width - oldx;
	     return;
	  }
	oldx = i_ret.width;
	if (len > 1)
	  i += len - 1;
     }
}

char **
__imlib_list_fonts(int *num_ret)
{
   int i, j, d, l = 0;
   char **list = NULL, **dir, *path;
   TT_Error error;
   char *p;
   
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
            path = malloc(strlen(fpath[i]) + strlen(dir[j]) + 2);
            sprintf(path, "%s/%s", fpath[i], dir[j]);
                /* trim .ttf if it is there */
                if((p = strrchr(dir[j], '.')))
                   *p = '\0';
            if(!__imlib_ItemInList(list, l, dir[j]))
            {
		  if (__imlib_FileIsFile(path))
		    {
		       TT_Face f;
		       
		       error = TT_Open_Face(engine, path, &f);
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
		       free(dir[j]);
		    }
            }
		     free(path);
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
   
#ifdef  XMB_FONT_CACHE
   ImlibXmbHash *h;

   h = hashes;
   while(h)
     {
        if (h->references == 0)
	   num += h->mem_use;
	h = h->next;
     }
   /* printf("xfd font cache size=%d\n", num); */
#endif

   f = fonts;
   while(f)
     {
	if (f->type == IMLIB_FONT_TYPE_TTF && f->hdr.references == 0)
	   num += f->ttf.mem_use;
	f = f->hdr.next;
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
	     if (f->hdr.references == 0)
		flast = f;
	     f = f->hdr.next;
	  }
	if (flast)
	  {
	     if (flast->type == IMLIB_FONT_TYPE_TTF)
	       size -= flast->ttf.mem_use;
#ifdef  XMB_FONT_CACHE
	     if (flast->type & IMLIB_FONT_TYPE_X && flast->xf.hash)
	       size -= flast->xf.hash->mem_use;
#endif
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
	f = f->hdr.next;
	if (pf->hdr.references == 0)
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
   font->hdr.references--;
   if (font->type == IMLIB_FONT_TYPE_TTF_X)
     font->xf.ttffont->hdr.references--;
#ifdef  XMB_FONT_CACHE
   if (font->type & IMLIB_FONT_TYPE_X && font->xf.hash )
	font->xf.hash->references--;
#endif

   /* if still referenced exit here */
   if (font->hdr.references > 0)
      return;

   __imlib_flush_font_cache();
}

void
__imlib_nuke_font(ImlibFont *fn)
{
   int                 i;
   ImlibFont          *f, *pf;
   ImlibTtfFont       *font;

   /* remove form font cache list */
   pf = NULL;
   f = fonts;
   while (f)
     {
	if (f == fn)
	  {
	     if (!pf)
		fonts = f->hdr.next;
	     else
		pf->hdr.next = f->hdr.next;
	     f = NULL;
	  }
	else
	  {
	     pf = f;
	     f = f->hdr.next;
	  }
     }

   switch (fn->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	font = (ImlibTtfFont *)fn;
        break;
     case IMLIB_FONT_TYPE_TTF_X:
/*
	if (fn->xf.ttffont->hdr.references == 0)
	  __imlib_nuke_font(fn->xf.ttffont);
*/
     case IMLIB_FONT_TYPE_X:
	free(fn->xf.name);

#ifdef  XMB_FONT_CACHE
	if (fn->xf.hash != NULL)
	  __imlib_free_font_hash(fn->xf.hash);
#endif
	free(fn);
        return;
     default:
	return;
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

#ifdef  XMB_FONT_CACHE
void
__imlib_free_font_hash(ImlibXmbHash *h)
{
    if (!h->references)
      {
	ImlibXmbHash *h, *ph;

   	h = hashes;
	ph = NULL;
   	while(h)
	  if (!h->references)
	    {
	      if (ph)
		ph->next = h->next;
	      else
		hashes = h->next;
	      break;
	    }
	  else
	    {
	      ph = h;
	      h = h->next;
	    }

	free(h->name);
	if (h->hash_count)
	  {
	    int i;

	    for (i=0; i<h->size; i++)
	       {
		 ImlibXmbHashElm *e;
	
		 e = h->hash[i];
		 while (e)
		    {	
		      ImlibXmbHashElm *n;

		      n = e->next;
		      free(e->im);
		      free(e);
		      e = n;
		    }
	       }
	  }
	free(h);
      }
}
#endif
