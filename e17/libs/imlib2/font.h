#ifndef __FONT
#define __FONT 1

typedef struct _imlib_font ImlibFont;

struct _imlib_font
{
   char      *name;
   int        references;
   ImlibFont *next;
   
   TT_Engine           engine;
   TT_Face             face;
   TT_Instance         instance;
   TT_Face_Properties  properties;
   int                 num_glyph;
   TT_Glyph           *glyphs;
   TT_Raster_Map     **glyphs_cached;
   int                 max_descent;
   int                 max_ascent;   
};
#endif
