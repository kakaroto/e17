#ifndef __FONT
#define __FONT 1

typedef struct	_imlib_font_header	ImlibFontHeader;
typedef struct	_imlib_ttffont		ImlibTtfFont;
typedef struct	_imlib_xfont		ImlibXFontSet;
typedef union	_imlib_font		ImlibFont;
typedef struct  _imlib_encoding_map     ImlibEncodingMap;
typedef unsigned short                  ImlibWideChar;

#define	TTF_FONT_CACHE	1
#define	TTF_HASH_SIZE	(256*1)

#ifdef	TTF_FONT_CACHE
typedef	struct _imlib_ttf_hash	   ImlibTTFHash;
typedef	struct _imlib_ttf_hash_elm ImlibTTFHashElm;

struct _imlib_ttf_hash_elm
{
   wchar_t              wc;
   TT_Glyph            *glyph;
   TT_Raster_Map       *glyph_raster;
};

struct _imlib_ttf_hash
{
   ImlibTTFHash	       *next;
   char                *name;
   int                  references;

   int                  type;
   int                  size;
   ImlibTTFHashElm    **hash;
   int                  mem_use;
};
#endif

struct _imlib_font_header
{
   int        type;

   ImlibFont *next;
   char      *name;
   int        references;
};

struct _imlib_ttffont
{
   int        type;

   ImlibFont *next;
   char      *name;
   int        references;
   
   TT_Engine           engine;
   TT_Face             face;
   TT_Instance         instance;
   TT_Face_Properties  properties;
   int                 num_glyph;
#ifdef TTF_FONT_CACHE
   ImlibTTFHash       *glyph_hash;
   TT_CharMap          char_map;
#else
   TT_Glyph           *glyphs;
   TT_Raster_Map     **glyphs_cached_right;
#endif
   int                 max_descent;
   int                 max_ascent;   
   int                 descent;
   int                 ascent;
   int                 mem_use;
};

struct _imlib_encoding_map {
  ImlibWideChar char_map[96];
};

#define	XMB_FONT_CACHE	level2
#define	XMB_HASH_SIZE	(256*1)
#define XMB_HASH_VAL1	8
#define XMB_HASH_VAL2	8
#define XMB_HASH_VAL3	3

#ifdef	XMB_FONT_CACHE
typedef	struct _imlib_xfd_hash	   ImlibXmbHash;
typedef	struct _imlib_xfd_hash_elm ImlibXmbHashElm;

struct _imlib_xfd_hash_elm
{
   ImlibXmbHashElm     *next;
   wchar_t              wc;
   int                  w, h;
   DATA32              *im;
};

struct _imlib_xfd_hash
{
   ImlibXmbHash	       *next;
   char                *name;
   int                  references;

   int                  type;
   int                  size;
   ImlibXmbHashElm    **hash;
   int			hash_count;
   int			collision_count;
   int                  mem_use;
};
#endif

struct _imlib_xfont
{
   int          type;

   ImlibFont   *next;
   char        *name;
   int          references;

   XFontSet     xfontset;
   int          font_count;
   XFontStruct **font_struct;
   char        **font_name;
   int          ascent;
   int          descent;
   int          max_ascent;
   int          max_descent;
   int          max_width;
   
   ImlibFont   *ttffont;
   int          total_ascent;
   int          total_descent;

#ifdef	XMB_FONT_CACHE
   ImlibXmbHash	*hash;
#endif
};

union _imlib_font 
{
   int                 type;
   ImlibFontHeader     hdr;
   ImlibTtfFont        ttf;
   ImlibXFontSet       xf;
};

/* Imlib font type */
#define IMLIB_FONT_TYPE_TTF	1	/* find ttf font only */
#define IMLIB_FONT_TYPE_X	(1<<1)	/* find x font only */
#define IMLIB_FONT_TYPE_TTF_X	(IMLIB_FONT_TYPE_TTF | IMLIB_FONT_TYPE_X)

/* Imlib encoding */
#define IMLIB_ENCOING_ISO_8859_1 0
#define IMLIB_ENCOING_ISO_8859_2 1
#define IMLIB_ENCOING_ISO_8859_3 2
#define IMLIB_ENCOING_ISO_8859_4 3

TT_Raster_Map *__imlib_create_font_raster(int width, int height);
void           __imlib_destroy_font_raster(TT_Raster_Map * rmap);
void           __imlib_add_font_path(const char *path);
void           __imlib_del_font_path(const char *path);
int            __imlib_font_path_exists(const char *path);
char         **__imlib_list_font_path(int *num_ret);
ImlibFont     *__imlib_find_cached_font(const char *ttffontname, const char *xfontname, int mode);
ImlibFont     *__imlib_load_font(const char *fontname);
ImlibFont     *__imlib_load_xfontset(Display *display, const char *fontname);
#ifdef        XMB_FONT_CACHE
ImlibXmbHash  *__imlib_create_font_hash_table(const char *xfontsetname, int type);
#endif
#ifdef TTF_FONT_CACHE
ImlibTTFHash  *__imlib_create_ttf_font_hash_table(const char *ttfontname, int type, int argsize);
#endif
ImlibFont     *__imlib_clone_cached_font(ImlibFont *fn);
void           __imlib_free_font(ImlibFont *font);
void           __imlib_calc_size(ImlibFont *f, int *width, int *height, 
				 const char *text);
void           __imlib_calc_advance(ImlibFont *f, int *adv_w, int *adv_h, 
				    const char *text);
int            __imlib_calc_inset(ImlibFont *f, const char *text);
void           __imlib_render_str(ImlibImage *im, ImlibFont *fn, int drx, 
				  int dry, const char *text,
				  DATA8 r, DATA8 g, DATA8 b, DATA8 a,
				  char dir, double angle, int *retw, int *reth, int blur,
				  int *nextx, int *nexty, ImlibOp op,
				  int cx, int cy, int cw, int ch);  
void           __imlib_xfd_draw_str(Display *display, Drawable drawable,
				    Visual *v, int depth, Colormap cm,
				    ImlibImage *im, ImlibFont *fn, int x,
				    int y, const char *text, DATA8 r, DATA8 g,
				    DATA8 b, DATA8 a, char dir, double angle,
				    char blend, ImlibColorModifier *cmod,
				    char hiq, char dmask, ImlibOp op,
				    int *retw, int *reth,
				    int *nextx, int *nexty,
				    int cx, int cy, int cw, int ch);
#ifdef	XMB_FONT_CACHE
void          __imlib_xfd_build_str_image(Display *display, Drawable drawable,
				     Visual *v, ImlibFont *fn, ImlibImage *im,
				     const char *text, DATA8 r, DATA8 g, DATA8 b, DATA8 a);
#endif
#ifdef	TTF_FONT_CACHE
unsigned short __imlib_find_hash_index(ImlibTtfFont *f, unsigned short argchar);	
#endif
int           __imlib_char_pos(ImlibFont *fn, const char *text, int x, int y,
			       int *cx, int *cy, int *cw, int *ch);
void          __imlib_char_geom(ImlibFont *fn, const char *text, int num,
				int *cx, int *cy, int *cw, int *ch);
int           __imlib_xfd_char_pos(ImlibFont *f, const char *text, int x, int y,
			           int *cx, int *cy, int *cw, int *ch);
void          __imlib_xfd_char_geom(ImlibFont *fn, const char *text, int num,
				int *cx, int *cy, int *cw, int *ch);
char        **__imlib_list_fonts(int *num_ret);
void          __imlib_free_font_list(char **list, int num);
int           __imlib_get_cached_font_size(void);
void          __imlib_flush_font_cache(void);
void          __imlib_purge_font_cache(void);
int           __imlib_get_font_cache_size(void);
void          __imlib_set_font_cache_size(int size);
void          __imlib_nuke_font(ImlibFont *font);
void          __imlib_set_TTF_encoding(unsigned char enc);
void          __imlib_init_encoding();
#ifdef	XMB_FONT_CACHE
void          __imlib_free_font_hash(ImlibXmbHash *h);
#endif

#ifdef	TTF_FONT_CACHE
void          __imlib_free_ttf_font_hash(ImlibTTFHash *h);
#endif

#endif
