#include "config.h"
#include "common.h"
#include "colormod.h"
#include "image.h"
#include "blend.h"
#ifdef HAVE_FREETYPE1_FREETYPE_FREETYPE_H
#include <freetype1/freetype/freetype.h>
#else
#ifdef HAVE_FREETYPE_FREETYPE_H
#include <freetype/freetype.h>
#else
#include <freetype.h>
#endif
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

/* List of TTF Hashes */
#ifdef  TTF_FONT_CACHE
static ImlibTTFHash *ttfhashes = NULL;
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

/* Encoding maps */
static const ImlibEncodingMap iso1 =
{
  /* ISO-8859-1 encoding (conversion to UTF-8) */
  0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
};

static const ImlibEncodingMap iso2 =
{
  /* ISO-8859-2 encoding (conversion to UTF-8) */
  0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7, 0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B, 0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7, 0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C, 0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7, 0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E, 0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7, 0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF, 0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7, 0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F, 0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7, 0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
};

static const ImlibEncodingMap iso3 =
{
  /* ISO-8859-3 encoding (conversion to UTF-8) */
  0x00A0, 0x0126, 0x02D8, 0x00A3, 0x00A4, 0x0124, 0x00A7, 0x00A8, 0x0130, 0x015E, 0x011E, 0x0134, 0x00AD, 0x017B, 0x00B0, 0x0127, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x0125, 0x00B7, 0x00B8, 0x0131, 0x015F, 0x011F, 0x0135, 0x00BD, 0x017C, 0x00C0, 0x00C1, 0x00C2, 0x00C4, 0x010A, 0x0108, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x0120, 0x00D6, 0x00D7, 0x011C, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x016C, 0x015C, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E4, 0x010B, 0x0109, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x0121, 0x00F6, 0x00F7, 0x011D, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x016D, 0x015D, 0x02D9
};

static const ImlibEncodingMap iso4 =
{
  /* ISO-8859-4 encoding (conversion to UTF-8) */
  0x00A0, 0x0104, 0x0138, 0x0156, 0x00A4, 0x0128, 0x013B, 0x00A7, 0x00A8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00AD, 0x017D, 0x00AF, 0x00B0, 0x0105, 0x02DB, 0x0157, 0x00B4, 0x0129, 0x013C, 0x02C7, 0x00B8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014A, 0x017E, 0x014B, 0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E, 0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x012A, 0x0110, 0x0145, 0x014C, 0x0136, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x0168, 0x016A, 0x00DF, 0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F, 0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x012B, 0x0111, 0x0146, 0x014D, 0x0137, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x0169, 0x016B, 0x02D9
};

static const ImlibEncodingMap iso5 =
{
  /* ISO-8859-5 encoding (conversion to UTF-8) */
  0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F, 0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F, 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F, 0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F
};

static const ImlibEncodingMap* _imlib_encodings[5] = { &iso1, &iso2, &iso3, &iso4, &iso5 };
static unsigned char imlib2_encoding = 0;
static unsigned char encoding_initialized = 0;

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
	     if (fpath[i]) free(fpath[i]);
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

int
__imlib_font_path_exists(const char *path)
{
   int i;
   
   for (i = 0; i < fpath_num; i++)
     {
	if (!strcmp(path, fpath[i])) return 1;
     }
   return 0;
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
#ifndef TTF_FONT_CACHE
   TT_CharMap          char_map;
#endif
   TT_Glyph_Metrics    metrics;
   TT_Instance_Metrics imetrics;
   int                 dpi = 96;
   unsigned short      i, n, index, load_flags;
   unsigned short      num_glyphs = 0, no_cmap = 0;
   unsigned short      platform, encoding;
   int                 size, j, upm, ascent, descent;
#ifdef TTF_FONT_CACHE
   int                 glyphs_loaded;
#endif
   char                *name, *file = NULL, *tmp;
   ImlibTtfFont       *f;

   __imlib_init_encoding();

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
#ifdef TTF_FONT_CACHE
   /* We have to store the Char Map for futher use */
	     TT_Get_CharMap(f->face, i, &(f->char_map));
#else
	     TT_Get_CharMap(f->face, i, &char_map);
#endif
	     break;
	  }
     }
#ifdef TTF_FONT_CACHE
   if (i == n)
     /* We have to store the Char Map for futher use */
     TT_Get_CharMap(f->face, 0, &(f->char_map));
   /* Now we can see how many glyphs we have to load */
   f->num_glyph = f->properties.num_Glyphs;
   /* and reserve space for ALL of them */
   f->glyph_hash = __imlib_create_ttf_font_hash_table(fontname,0,f->num_glyph);

#else
   if (i == n)
     TT_Get_CharMap(f->face, 0, &char_map);
   f->num_glyph = 256;

   f->glyphs = (TT_Glyph *)malloc(f->num_glyph * sizeof(TT_Glyph));
   memset(f->glyphs, 0, f->num_glyph * sizeof(TT_Glyph));
   
   f->glyphs_cached_right = 
      (TT_Raster_Map **)malloc(f->num_glyph * sizeof(TT_Raster_Map *));
   memset(f->glyphs_cached_right, 0, f->num_glyph * sizeof(TT_Raster_Map *));
#endif   
   load_flags = TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH;   
   f->max_descent = 0;
   f->max_ascent = 0;   
#ifdef TTF_FONT_CACHE
   /* Now load all the glyphs, storing index number with each glyph */
   i = 0; glyphs_loaded = 0;
   while ((i < 65535) && (glyphs_loaded < f->num_glyph))
     {
        if (TT_VALID(*(f->glyph_hash->hash[glyphs_loaded])->glyph)) {
	  i++;
	  continue;
        }
	  
#else
   for (i = 0; i < f->num_glyph; ++i)
     {
       if (TT_VALID(f->glyphs[i]))
	 continue;
#endif
	    
       if (no_cmap)
	 {
	   index = (i - ' ' + 1) < 0 ? 0 : (i - ' ' + 1);
	   if (index >= num_glyphs)
	     index = 0;
	 }
       else 
#ifdef TTF_FONT_CACHE
	 index = i;
       
       TT_New_Glyph(f->face, f->glyph_hash->hash[glyphs_loaded]->glyph);
       TT_Load_Glyph(f->instance, *(f->glyph_hash->hash[glyphs_loaded]->glyph), index, load_flags);
       TT_Get_Glyph_Metrics(*(f->glyph_hash->hash[glyphs_loaded]->glyph), &metrics);
       f->glyph_hash->hash[glyphs_loaded]->wc = index;
       i++; glyphs_loaded++;	
#else
       if (i < 0xA0 )
	 index = TT_Char_Index(char_map, i);
       else
	 index = TT_Char_Index(char_map, _imlib_encodings[imlib2_encoding]->char_map[(i - 0xA0)]);
       
       TT_New_Glyph(f->face, &f->glyphs[i]);
       TT_Load_Glyph(f->instance, f->glyphs[i], index, load_flags);
       TT_Get_Glyph_Metrics(f->glyphs[i], &metrics);
#endif
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

#ifdef	TTF_FONT_CACHE
ImlibTTFHash *
__imlib_create_ttf_font_hash_table(const char *ttfontname, int type, int argsize)
{
/* Create an empty TTF cache */

   int i, size;
   ImlibTTFHash	*h;

   h = ttfhashes;
   while(h)
     if (!strcmp(ttfontname, h->name))
	{
	  h->references++;
	  return h;
	}
     else
	h = h->next;

   h = malloc(sizeof(ImlibTTFHash));
   h->next = ttfhashes;
   ttfhashes = h;

   /* Append this hash at the beginning of the list of hashes */
   h->name = strdup(ttfontname);
   h->references = 1;

   h->type = type;
   if (type)
     h->size = TTF_HASH_SIZE;
   else
     h->size = argsize;
   h->hash = (ImlibTTFHashElm **)malloc( sizeof(ImlibTTFHashElm *) * h->size);
   h->mem_use += sizeof(ImlibTTFHashElm *) * h->size;
   
   for (i=0; i<h->size; i++)
     h->hash[i] = NULL;

   for (i=0; i<h->size; i++) {
     h->hash[i] = (ImlibTTFHashElm *)malloc(sizeof(ImlibTTFHashElm));
     h->hash[i]->glyph = (TT_Glyph *)malloc(sizeof(TT_Glyph));
     memset(h->hash[i]->glyph, 0, sizeof(TT_Glyph));
      
     h->hash[i]->glyph_raster = NULL;
   }
   h->mem_use += sizeof(ImlibTTFHashElm) * h->size; 
   h->mem_use += sizeof(TT_Glyph) * h->size; 
   return h;
}

unsigned short
__imlib_find_hash_index(ImlibTtfFont *f, unsigned short argchar) 
{
  /* Find TTF index corresponding to the UTF code argchar 
      Returns 0 if there is no glyph with that code in the font */

  int i, span;
  unsigned short ttf_index;

  span = 0;
  if (argchar < 0xA0 )
    ttf_index = TT_Char_Index(f->char_map, argchar);
  else
    ttf_index = TT_Char_Index(f->char_map, _imlib_encodings[imlib2_encoding]->char_map[(argchar - 0xA0)]);

  /* the fast search algorithm - we can use it, because the glyphs
     are index-sorted (ascending) by the load procedure */

  i = span = f->num_glyph / 2;
  while (span > 1) {
    if (ttf_index == f->glyph_hash->hash[i]->wc)
      return i;
    else if (ttf_index < f->glyph_hash->hash[i]->wc) {
      span /= 2;
      i -= span;
    }
    else {
      span /= 2;
      i += span;
    }
  }
  /* This is needed if the number of glyphs in the font is not equal 2^n */
  while (!((ttf_index > f->glyph_hash->hash[i]->wc) && (ttf_index < f->glyph_hash->hash[i+1]->wc))) {
    
    if (ttf_index > f->glyph_hash->hash[i]->wc) {
      i++;
      if (i > f->num_glyph)
	return 0;
    }
    else {
      i--;
      if (i < 0)
	return 0;
    }
    if (ttf_index == f->glyph_hash->hash[i]->wc) 
      return i;
  }
  return 0;
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
#ifndef TTF_FONT_CACHE
	if (fn->ttf.num_glyph)
	  {
   	    f->ttf.glyphs = (TT_Glyph *)malloc(f->ttf.num_glyph * sizeof(TT_Glyph));
	    memcpy(f->ttf.glyphs, fn->ttf.glyphs,
		    f->ttf.num_glyph * sizeof(TT_Glyph));
	    f->ttf.glyphs_cached_right = (TT_Raster_Map **)malloc(f->ttf.num_glyph * sizeof(TT_Raster_Map *));
	    memcpy(f->ttf.glyphs_cached_right, fn->ttf.glyphs_cached_right,
		    f->ttf.num_glyph * sizeof(TT_Raster_Map *));
	  }
#else
	/* If cloning font, just increase the reference count on a hash */
	if (fn->ttf.glyph_hash) 
	  {
	    f->ttf.glyph_hash = fn->ttf.glyph_hash;
	    fn->ttf.glyph_hash->references++;
	  } 
#endif
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
#ifdef TTF_FONT_CACHE
	/* The index is a 2-byte number now! */
	unsigned short      j;
	
	/* Find the index for the code */
	j = __imlib_find_hash_index(f,(unsigned char) text[i]);	
	/* Use the cached glyph */
	if (!TT_VALID(*(f->glyph_hash->hash[j]->glyph)))
	   continue;
	TT_Get_Glyph_Metrics(*(f->glyph_hash->hash[j]->glyph), &gmetrics);
#else
	unsigned char       j;	

	j = text[i];	
	if (!TT_VALID(f->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(f->glyphs[j], &gmetrics);
#endif
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
#ifdef TTF_FONT_CACHE
	unsigned short      j;
	
	j = __imlib_find_hash_index(f,(unsigned char) text[i]);
	if (!TT_VALID(*(f->glyph_hash->hash[j]->glyph)))
	   continue;
	TT_Get_Glyph_Metrics(*(f->glyph_hash->hash[j]->glyph), &gmetrics);
#else
	unsigned char       j;

	j = text[i];	
	if (!TT_VALID(f->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(f->glyphs[j], &gmetrics);
#endif
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
#ifdef TTF_FONT_CACHE
	unsigned short      j;
	
	j = __imlib_find_hash_index(f,(unsigned char) text[i]);
	if (!TT_VALID(*(f->glyph_hash->hash[j]->glyph)))
	   continue;
	TT_Get_Glyph_Metrics(*(f->glyph_hash->hash[j]->glyph), &gmetrics);
#else
	unsigned char       j;

	j = text[i];
	if (!TT_VALID(f->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(f->glyphs[j], &gmetrics);
#endif
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
#ifdef TTF_FONT_CACHE
   unsigned short      j;
#else
   unsigned char       j;
#endif
   TT_Raster_Map      *rtmp = NULL, *rmap;
   ImlibImage          im2;
   ImlibTtfFont       *fn;

   switch (f->type)
     {
     case IMLIB_FONT_TYPE_TTF:
	fn = (ImlibTtfFont *)f;
        break;
     case IMLIB_FONT_TYPE_X:
	if (retw) *retw = 0;
	if (reth) *reth = 0;
	if (nextx) *nextx = 0;
	if (nexty) *nexty = 0;
	return;
     case IMLIB_FONT_TYPE_TTF_X:
	fn = (ImlibTtfFont *)f->xf.ttffont;
        break;
     default:
	if (retw) *retw = 0;
	if (reth) *reth = 0;
	if (nextx) *nextx = 0;
	if (nexty) *nexty = 0;
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
#ifdef TTF_FONT_CACHE
   j = __imlib_find_hash_index(fn,(unsigned char) text[0]);
   TT_Get_Glyph_Metrics(*(fn->glyph_hash->hash[j]->glyph), &metrics);
#else
   j = text[0];
   TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
#endif
   x_offset = (-metrics.bearingX) / 64;
   y_offset = -(fn->max_descent / 64);

   /* figure out the size this text string is going to be */
   __imlib_calc_size(f, &w, &h, text);
   tw = w; th = h;
   switch(dir)
     {
     case 0:
     case 1:
	if (retw) *retw = tw;
	if (reth) *reth = th;
	if (nexty) *nexty = fn->ascent + fn->descent;
	if (nextx)
	  {
#ifdef TTF_FONT_CACHE
	     j = __imlib_find_hash_index(fn,(unsigned char) text[strlen(text) - 1]);
	     TT_Get_Glyph_Metrics(*(fn->glyph_hash->hash[j]->glyph), &metrics);
#else
	     j = text[strlen(text) - 1];
	     TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
#endif
	     *nextx = w - x_offset + (metrics.advance / 64) - 
		(metrics.bbox.xMax / 64);
	  }
	break;
     case 2:
     case 3:
	tw = h; th = w;
	if (retw) *retw = tw;
	if (reth) *reth = th;
	if (nextx) *nextx = fn->ascent + fn->descent;
	if (nexty)
	  {
#ifdef TTF_FONT_CACHE
	     j = __imlib_find_hash_index(fn,(unsigned char) text[strlen(text) - 1]);
	     TT_Get_Glyph_Metrics(*(fn->glyph_hash->hash[j]->glyph), &metrics);
#else
	     j = text[strlen(text) - 1];
	     TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
#endif
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
	if (retw) *retw = tw;
	if (reth) *reth = th;
	if (nexty) *nexty = fn->ascent + fn->descent;
	if (nextx)
	  {
#ifdef TTF_FONT_CACHE
	    j = __imlib_find_hash_index(fn,(unsigned char) text[strlen(text) - 1]);
	     TT_Get_Glyph_Metrics(*(fn->glyph_hash->hash[j]->glyph), &metrics);
#else
	     j = text[strlen(text) - 1];
	     TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
#endif
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
#ifdef TTF_FONT_CACHE
	    j = __imlib_find_hash_index(fn,(unsigned char) text[i]);
	     if (!TT_VALID(*(fn->glyph_hash->hash[j]->glyph)))
		continue;	
	     TT_Get_Glyph_Metrics(*(fn->glyph_hash->hash[j]->glyph), &metrics);
#else
	     j = text[i];	
	     if (!TT_VALID(fn->glyphs[j]))
		continue;	
	     TT_Get_Glyph_Metrics(fn->glyphs[j], &metrics);
#endif
	     
	     xmin = metrics.bbox.xMin & -64;
	     ymin = metrics.bbox.yMin & -64;
	     xmax = (metrics.bbox.xMax + 63) & -64;
	     ymax = (metrics.bbox.yMax + 63) & -64;
	     
#ifdef TTF_FONT_CACHE
	     rtmp = fn->glyph_hash->hash[j]->glyph_raster;
#else
	     rtmp = fn->glyphs_cached_right[j];
#endif
	     if (!rtmp)
	       {
		  rtmp = __imlib_create_font_raster(((xmax - xmin) / 64) + 1,
						    ((ymax - ymin) / 64) + 1);
#ifdef TTF_FONT_CACHE
		  TT_Get_Glyph_Pixmap(*(fn->glyph_hash->hash[j]->glyph), rtmp, -xmin, -ymin);
		  fn->glyph_hash->hash[j]->glyph_raster = rtmp;
		  fn->glyph_hash->mem_use += 
		     (((xmax - xmin) / 64) + 1) *
		     (((ymax - ymin) / 64) + 1);
#else
		  TT_Get_Glyph_Pixmap(fn->glyphs[j], rtmp, -xmin, -ymin);
		  fn->glyphs_cached_right[j] = rtmp;
		  fn->mem_use += 
		     (((xmax - xmin) / 64) + 1) *
		     (((ymax - ymin) / 64) + 1);
#endif
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
					  NULL, op, clx, cly, clw, clh);
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
   int                 _retw, _reth, _nextx, _nexty;
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
	_retw = l_ret.width;
	_reth = l_ret.height;
	_nextx = i_ret.width;
	_nexty = i_ret.height;
	break;
     case 2:
     case 3:
	_retw = l_ret.height;
	_reth = l_ret.width;
	_nextx = i_ret.height;
	_nexty = i_ret.width;
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
	   _retw = tw;
	   _reth = th;
	   _nextx = i_ret.width;
	   _nexty = i_ret.height;
	}
     }
   if (retw) *retw = _retw;
   if (reth) *reth = _reth;
   if (nextx) *nextx = _nextx;
   if (nexty) *nexty = _nexty;

   /* if we draw outside the image from here - give up */
   if ((x > im->w) || (y > im->h))
      return;

   /* if the text is completely outside the image - give up */
   if (((x + _retw) <= 0) || ((y + _reth) <= 0))
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
				NULL, op, clx, cly, clw, clh);
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
#ifdef TTF_FONT_CACHE
	unsigned short      j;
	
	j = __imlib_find_hash_index(&(f->ttf),(unsigned char) text[i]);
	if (!TT_VALID(*(fn->glyph_hash->hash[j]->glyph)))
	   continue;
	TT_Get_Glyph_Metrics(*(fn->glyph_hash->hash[j]->glyph), &gmetrics);
#else
	unsigned char       j;
	
	j = text[i];
	if (!TT_VALID(fn->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(fn->glyphs[j], &gmetrics);
#endif
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
#ifdef TTF_FONT_CACHE
	unsigned short      j;
	
	j = __imlib_find_hash_index(&(f->ttf),(unsigned char) text[i]);
	if (!TT_VALID(*(fn->glyph_hash->hash[j]->glyph)))
	   continue;
	TT_Get_Glyph_Metrics(*(fn->glyph_hash->hash[j]->glyph), &gmetrics);
#else
	unsigned char       j;
	
	j = text[i];
	if (!TT_VALID(fn->glyphs[j]))
	   continue;
	TT_Get_Glyph_Metrics(fn->glyphs[j], &gmetrics);
#endif
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
#ifdef  TTF_FONT_CACHE
   ImlibTTFHash *ch;
#endif
   
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
   
#ifdef  TTF_FONT_CACHE

   ch = ttfhashes;
   while(h)
     {
        if (ch->references == 0)
	   num += ch->mem_use;
	ch = ch->next;
     }
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
#ifdef TTF_FONT_CACHE
   __imlib_free_ttf_font_hash(font->glyph_hash);
#else
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
#endif
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

#ifdef TTF_FONT_CACHE
void
__imlib_free_ttf_font_hash(ImlibTTFHash *h)
{
   /* Free all the memory allocated for TTF cache */
   int i;

   if (!(--(h->references)))
      {
	ImlibTTFHash *h, *ph;

   	h = ttfhashes;
	ph = NULL;
   	while(h)
	  if (!h->references)
	    {
	      if (ph)
		ph->next = h->next;
	      else
		ttfhashes = h->next;
	      break;
	    }
	  else
	    {
	      ph = h;
	      h = h->next;
	    }

	free(h->name);
	for (i = 0; i < h->size; i++)
	  {	
	    if ((h->hash) && (h->hash[i]) && (h->hash[i]->glyph_raster))
	      __imlib_destroy_font_raster(h->hash[i]->glyph_raster);
	    if ((h->hash) && (h->hash[i]) && TT_VALID(*(h->hash[i]->glyph)))
	      TT_Done_Glyph(*(h->hash[i]->glyph));
	    free(h->hash[i]);
	  }
	/* free the hash table*/
	free(h->hash);
	/* free glyph info */
	free(h);
      }
}

#endif

void          
__imlib_set_TTF_encoding(unsigned char enc) 
{
  /* Enforce the use of a specified encoding */
  imlib2_encoding = enc;
  encoding_initialized = 1;
}

void          
__imlib_init_encoding()
{
  /* Set the current ISO encoding based on (in that order):
   * the "IMLIB_ENCODING" environment variable,
   * the "LANG" variable,
   * the ISO-8859-1 default */
   
  char *s, *enc_num, *lang;
  char iso2lang[21] = "cs hu pl ro hr sk sl";
  char iso3lang[6] = "eo mt";
  char iso4lang[12] = "et lv lt kl";
  char iso5lang[18] = "bg be mk ru sr uk";

  /* Check if we already initialized the encoding */
  if (!encoding_initialized) 
    {
      imlib2_encoding = 255;
      /* First check if we have IMLIB_ENCODING variable set */
      if ((s = getenv("IMLIB_ENCODING")) != NULL) 
	{
	  if ((enc_num = strstr(s, "8859-")) != NULL) 
	    {
	      if (!strcmp((enc_num+5), "1")) imlib2_encoding = 0;
	      else if (!strcmp((enc_num+5), "2")) imlib2_encoding = 1;
	      else if (!strcmp((enc_num+5), "3")) imlib2_encoding = 2;
	      else if (!strcmp((enc_num+5), "4")) imlib2_encoding = 3;		
	      else if (!strcmp((enc_num+5), "5")) imlib2_encoding = 4;		
	    }
	}
      if (imlib2_encoding == 255)
	{
	  /* Still not initialized - try to guess from LANG variable */
	  if ((lang = getenv("LANG")) != NULL)
	    {
	      if (strstr(iso2lang, lang) != NULL) imlib2_encoding = 1;
	      else if (strstr(iso3lang, lang) != NULL) imlib2_encoding = 2;
	      else if (strstr(iso4lang, lang) != NULL) imlib2_encoding = 3;
	      else if (strstr(iso5lang, lang) != NULL) imlib2_encoding = 4;
	    }
	}
      if (imlib2_encoding == 255) { 
	/* Still not initialized - default to ISO8859-1 */
	imlib2_encoding = 0;
      }
     encoding_initialized = 1;
    }  
}
