#ifndef __IMAGE
# define __IMAGE 1

typedef enum   _iflags                  Iflags;
typedef struct _imlibimage              ImlibImage;
typedef struct _imlibimagepixmap        ImlibImagePixmap;
typedef struct _imlibborder             ImlibBorder;
typedef struct _imlibloader             ImlibLoader;

enum _iflags
{
   F_NONE = 0,
   F_HAS_ALPHA = (1 << 0),
   F_UNLOADED = (1 << 1),
   F_UNCACHEABLE = (1 << 2),
   F_ALWAYS_CHECK_DISK = (1 << 3),
   F_INVALID = (1 << 4)
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
   Iflags            flags;
   time_t            moddate;
   ImlibBorder       border;
   int               references;
   ImlibLoader      *loader;
   ImlibImage       *next;
   char             *format;
};

struct _imlibimagepixmap
{
   int               w, h;
   Pixmap            pixmap, mask;
   Display          *display;
   Visual           *visual;
   int               depth;
   int               mode_count;
   ImlibImage       *image;
   int               references;
   ImlibImagePixmap *next;
};

struct _imlibloader
{
   char         *file;
   int           num_formats;
   char        **formats;
   void         *handle;
   char        (*load)(ImlibImage *im);
   char        (*save)(ImlibImage *im);
   ImlibLoader  *next;
};

void              SetCacheSize(int size);
int               GetCacheSize(void);
ImlibImage       *ProduceImage(void);
void              ConsumeImage(ImlibImage *im);
ImlibImage       *FindCachedImage(char *file);
void              AddImageToCache(ImlibImage *im);
void              RemoveImageFromCache(ImlibImage *im);
void              CleanupImageCache(void);
ImlibImagePixmap *ProduceImagePixmap(void);
void              ConsumeImagePixmap(ImlibImagePixmap *ip);
ImlibImagePixmap *FindCachedImagePixmap(ImlibImage *im, int w, int h, 
					Display *d, Visual *v,
					int depth, int mode_count);
void              AddImagePixmapToCache(ImlibImagePixmap *ip);
void              RemoveImagePixmapFromCache(ImlibImagePixmap *ip);
void              CleanupImagePixmapCache();
ImlibLoader      *ProduceLoader(char *file);
char            **ListLoaders(int *num_ret);
void              ConsumeLoader(ImlibLoader *l);
void              RescanLoaders(void);
void              RemoveAllLoaders(void);
void              LoadAllLoaders(void);
ImlibLoader      *FindBestLoaderForFile(char *file);
ImlibImage       *LoadImage(char *file);
ImlibImagePixmap *FindImlibImagePixmapByID(Display *d, Pixmap p);
void              FreeImage(ImlibImage *im);
void              FreePixmap(Display *d, Pixmap p);

# define IMAGE_HAS_ALPHA(im) (im->flags & F_HAS_ALPHA)
# define IMAGE_IS_UNLOADED(im) (im->flags & F_UNLOADED)
# define IMAGE_IS_UNCACHEABLE(im) (im->flags & F_UNCACHEABLE)
# define IMAGE_ALWAYS_CHECK_DISK(im) (im->flags & F_ALWAYS_CHECK_DISK)
# define IMAGE_IS_VALID(im) (!(im->flags & F_INVALID))

#endif
