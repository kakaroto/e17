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
   TT_Raster_Map     **glyphs_cached_right;
   int                 max_descent;
   int                 max_ascent;   
   int                 descent;
   int                 ascent;
};

TT_Raster_Map *__imlib_create_font_raster(int width, int height);
void           __imlib_destroy_font_raster(TT_Raster_Map * rmap);
void           __imlib_add_font_path(char *path);
void           __imlib_del_font_path(char *path);
char         **__imlib_list_font_path(int *num_ret);
ImlibFont     *__imlib_find_cached_font(char *fontname);
ImlibFont     *__imlib_load_font(char *fontname);
void           __imlib_free_font(ImlibFont *font);
void           __imlib_calc_size(ImlibFont *f, int *width, int *height, 
				 char *text);
void           __imlib_render_str(ImlibImage *im, ImlibFont *fn, int drx, 
				  int dry, char *text,
				  DATA8 r, DATA8 g, DATA8 b, DATA8 a,
				  char dir, int *retw, int *reth, int blur,
				  int *nextx, int *nexty, ImlibOp op);

#endif
