#include "common.h"
#include <X11/Xlib.h>
#include "image.h"
#include <freetype.h>
#include "font.h"

#define TT_VALID( handle )  ( ( handle ).z != NULL )

/* cached font list */
static ImlibFont *fonts = NULL;

/* lookupt table of raster_map -> RGBA Alpha values */
static DATA8 rend_lut[9] = 
{ 0, 64, 128, 192, 255, 255, 255, 255, 255};

/* create an rmap of width and height */
static TT_Raster_Map *
__imlib_create_font_raster(int width, int height)
{
   TT_Raster_Map      *rmap;
   
   rmap = malloc(sizeof(TT_Raster_Map));
   rmap->width = (width + 3) & -4;
   rmap->rows = height;
   rmap->flow = TT_Flow_Down;
   rmap->cols = rmap->width;
   rmap->size = rmap->rows * rmap->width;
   rmap->bitmap = malloc(rmap->size);
   memset(rmap->bitmap, 0, rmap->size);
   return rmap;
}

/* duplicate the rmap */
static TT_Raster_Map *
__imlib_duplicate_raster(TT_Raster_Map * rmap)
{
   TT_Raster_Map      *new_rmap;
   
   new_rmap = malloc(sizeof(TT_Raster_Map));
   *new_rmap = *rmap;
   new_rmap->bitmap = malloc(new_rmap->size);
   memcpy(new_rmap->bitmap, rmap->bitmap, new_rmap->size);
   return new_rmap;
}

/* clear out the rmap and set to 0 */
static void
__imlib_clear_raster(TT_Raster_Map * rmap)
{
   memset(rmap->bitmap, 0, rmap->size);
}

/* free the rmap */
static void
__imlib_destroy_font_raster(TT_Raster_Map * rmap)
{
   free(rmap->bitmap);
   free(rmap);
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
   static TT_Engine    engine;
   static char         have_engine = 0;
   int                 dpi = 96;
   unsigned short      i, n, code, load_flags;
   unsigned short      num_glyphs = 0, no_cmap = 0;
   unsigned short      platform, encoding;
   int                 size;
   char               *file;
   
   /* find a cached font */
   f = __imlib_find_cached_font(fontname);
   if (f)
     {
	/* reference it up by one and return it */
	f->references++;
	return f;
     }
   /* if we dpont have a truetype font engine yet - make one */
   if (!have_engine)
     {
	error = TT_Init_FreeType(&engine);
	if (error)
	   return NULL;
	have_engine = 1;
     }
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
   
   f->glyphs_cached_right = (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_right, 0, f->num_glyph * sizeof(TT_Raster_Map *));
   f->glyphs_cached_left = (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_left, 0, f->num_glyph * sizeof(TT_Raster_Map *));
   f->glyphs_cached_down = (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_down, 0, f->num_glyph * sizeof(TT_Raster_Map *));
   f->glyphs_cached_up = (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
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
