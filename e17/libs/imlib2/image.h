#ifndef __IMAGE
# define __IMAGE 1

#include "ltdl.h"		/* for lt_dlhandle definition */

typedef enum   _iflags                  ImlibImageFlags;
typedef struct _imlibimage              ImlibImage;
typedef struct _imlibimagepixmap        ImlibImagePixmap;
typedef struct _imlibborder             ImlibBorder;
typedef struct _imlibloader             ImlibLoader;
typedef struct _imlib_color_modifier    ImlibColorModifier;
typedef enum   _load_error              ImlibLoadError;

enum _load_error
{
   LOAD_ERROR_NONE,
   LOAD_ERROR_FILE_DOES_NOT_EXIST,
   LOAD_ERROR_FILE_IS_DIRECTORY,
   LOAD_ERROR_PERMISSION_DENIED_TO_READ,
   LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT,
   LOAD_ERROR_PATH_TOO_LONG,
   LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT,
   LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY,
   LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE,
   LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS,
   LOAD_ERROR_OUT_OF_MEMORY,
   LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS,
   LOAD_ERROR_UNKNOWN
};

enum _iflags
{
   F_NONE              = 0,
   F_HAS_ALPHA         = (1 << 0),
   F_UNLOADED          = (1 << 1),
   F_UNCACHEABLE       = (1 << 2),
   F_ALWAYS_CHECK_DISK = (1 << 3),
   F_INVALID           = (1 << 4),
   F_DONT_FREE_DATA    = (1 << 5)
};

struct _imlib_color_modifier
{
   DATA8     red_mapping[256];
   DATA8     green_mapping[256];
   DATA8     blue_mapping[256];
   long long modification_count;
};

struct _imlibborder
{
   int left, right, top, bottom;
};

struct _imlibimage
{
   char             *file;
   int               w, h;
   DATA32           *data;
   ImlibImageFlags   flags;
   time_t            moddate;
   ImlibBorder       border;
   int               references;
   ImlibLoader      *loader;
   char             *format;
   ImlibImage       *next;
};

struct _imlibimagepixmap
{
   int               w, h;
   Pixmap            pixmap, mask;
   Display          *display;
   Visual           *visual;
   int               depth;
   int               source_x, source_y, source_w, source_h;
   Colormap          colormap;
   char              antialias, hi_quality, dither_mask;
   ImlibBorder       border;
   ImlibImage       *image;
   char              dirty;
   int               references;
   long long         modification_count;
   ImlibImagePixmap *next;
};

struct _imlibloader
{
   char         *file;
   int           num_formats;
   char        **formats;
   lt_dlhandle   handle;
   char        (*load)(ImlibImage *im,
		       void (*progress)(ImlibImage *im, char percent,
					int update_x, int update_y,
					int update_w, int update_h),
		       char progress_granularity, char immediate_load);
   char        (*save)(ImlibImage *im,
		       void (*progress)(ImlibImage *im, char percent,
					int update_x, int update_y,
					int update_w, int update_h),
		       char progress_granularity);
   ImlibLoader  *next;
};

void              __imlib_SetCacheSize(int size);
int               __imlib_GetCacheSize(void);
ImlibImage       *__imlib_ProduceImage(void);
void              __imlib_ConsumeImage(ImlibImage *im);
ImlibImage       *__imlib_FindCachedImage(char *file);
void              __imlib_AddImageToCache(ImlibImage *im);
void              __imlib_RemoveImageFromCache(ImlibImage *im);
int               __imlib_CurrentCacheSize(void);
void              __imlib_CleanupImageCache(void);
ImlibImagePixmap *__imlib_ProduceImagePixmap(void);
void              __imlib_ConsumeImagePixmap(ImlibImagePixmap *ip);
ImlibImagePixmap *__imlib_FindCachedImagePixmap(ImlibImage *im, int w, int h, 
						Display *d, Visual *v,
						int depth, int sx, int sy, 
						int sw, int sh, Colormap cm,
						char aa, char hiq, char dmask,
						long long modification_count);
void              __imlib_AddImagePixmapToCache(ImlibImagePixmap *ip);
void              __imlib_RemoveImagePixmapFromCache(ImlibImagePixmap *ip);
void              __imlib_CleanupImagePixmapCache(void);
ImlibLoader      *__imlib_ProduceLoader(char *file);
char            **__imlib_ListLoaders(int *num_ret);
void              __imlib_ConsumeLoader(ImlibLoader *l);
void              __imlib_RescanLoaders(void);
void              __imlib_RemoveAllLoaders(void);
void              __imlib_LoadAllLoaders(void);
ImlibLoader      *__imlib_FindBestLoaderForFile(char *file);
void              __imlib_SetImageAlphaFlag(ImlibImage *im, char alpha);
ImlibImage       *__imlib_CreateImage(int w, int h, DATA32 *data);
ImlibImage       *__imlib_LoadImage(char *file,
				    void (*progress)(ImlibImage *im, char percent,
						     int update_x, int update_y,
						     int update_w, int update_h),
				    char progress_granularity, char immediate_load,
				    char dont_cache, ImlibLoadError *er);
ImlibImagePixmap *__imlib_FindImlibImagePixmapByID(Display *d, Pixmap p);
void              __imlib_FreeImage(ImlibImage *im);
void              __imlib_FreePixmap(Display *d, Pixmap p);
void              __imlib_FlushCache(void);
void              __imlib_DirtyPixmapsForImage(ImlibImage *im);
void              __imlib_DirtyImage(ImlibImage *im);

# define IMAGE_HAS_ALPHA(im) ((im)->flags & F_HAS_ALPHA)
# define IMAGE_IS_UNLOADED(im) ((im)->flags & F_UNLOADED)
# define IMAGE_IS_UNCACHEABLE(im) ((im)->flags & F_UNCACHEABLE)
# define IMAGE_ALWAYS_CHECK_DISK(im) ((im)->flags & F_ALWAYS_CHECK_DISK)
# define IMAGE_IS_VALID(im) (!((im)->flags & F_INVALID))
# define IMAGE_FREE_DATA(im) (!((im)->flags & F_DONT_FREE_DATA))

# define SET_FLAG(flags, f) ((flags) |= (f))
# define UNSET_FLAG(flags, f) ((flags) &= (~f))

#endif
