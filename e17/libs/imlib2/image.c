#include "common.h"
#include <time.h>
#include <string.h>
#include <dlfcn.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "image.h"
#include "file.h"

static ImlibImage        *images = NULL;
static ImlibImagePixmap  *pixmaps = NULL;
static ImlibLoader       *loaders = NULL;
static int                cache_size = 4096 * 1024;

void
SetCacheSize(int size)
{
   cache_size = size;
   CleanupImageCache();
   CleanupImagePixmapCache();
}

int 
GetCacheSize(void)
{
   return cache_size;
}

ImlibImage *
ProduceImage(void)
{
   ImlibImage *im;
   
   im = malloc(sizeof(ImlibImage));
   im->data = NULL;
   im->file = NULL;
   im->moddate = 0;
   im->flags = F_NONE;
   im->border.left = 0;
   im->border.right = 0;
   im->border.top = 0;
   im->border.bottom = 0;
   im->references = 0;
   im->w = 0;
   im->h = 0;
   im->loader = NULL;
   im->next = NULL;
   return im;
}

void
ConsumeImage(ImlibImage *im)
{
   if (im->file)
      free(im->file);
   if (im->data)
      free(im->data);
   if (im->format)
      free(im->format);
   free(im);
}

ImlibImage *
FindCachedImage(char *file)
{
   ImlibImage *im, *previous_im;
   
   im = images;
   previous_im = NULL;
   /* go through the images list */
   while (im)
     {
	/* if the filenames match */
	if (!strcmp(file, im->file))
	  {
	     /* move the image to the head of the pixmap list */
	     if (previous_im)
	       {
		  previous_im->next = im->next;
		  im->next = images;
		  images = im;
	       }
	     /* return it */
	     return im;
	  }
	previous_im = im;
	im = im->next;	
     }
   return NULL;
}

void
AddImageToCache(ImlibImage *im)
{
   im->next = images;
   images = im;
}

void
RemoveImageFromCache(ImlibImage *im)
{
   ImlibImage *current_im, *previous_im;
   
   current_im = images;
   previous_im = NULL;
   while (current_im)
     {
	if (im == current_im)
	  {
             if (previous_im)
		previous_im->next = im->next;
	     else
		images = im->next;
	     return;
	  }
	current_im = current_im->next;
     }	
}

int 
CurrentCacheSize(void)
{
   ImlibImage *im;   
   ImlibImagePixmap *ip;   
   int current_cache = 0;

   im = images;
   while(im)
     {
	if (im->references == 0)
	  {
	     if (!(IMAGE_IS_VALID(im)))
	       {
		  RemoveImageFromCache(im);
		  ConsumeImage(im);
	       }
	     else
		current_cache += im->w * im->h * sizeof(DATA32);
	  }
	im = im->next;
     }
   ip = pixmaps;
   while(ip)
     {
	if (ip->references == 0)
	  {
	     if (!(IMAGE_IS_VALID(ip->image)))
	       {
		  RemoveImagePixmapFromCache(ip);
		  ConsumeImagePixmap(ip);
	       }
	     else
	       {
		  if (ip->pixmap)
		    {
		       if (ip->depth < 8)
			  current_cache += ip->w * ip->h * (ip->depth / 8);
		       else if (ip->depth == 8)
			  current_cache += ip->w * ip->h;
		       else if (ip->depth <= 16)
			  current_cache += ip->w * ip->h * 2;
		       else if (ip->depth <= 32)
			  current_cache += ip->w * ip->h * 4;		  
		    }
		  if (ip->mask)
		     current_cache += ip->w * ip->h / 8;
	       }
	  }
	ip = ip->next;
     }
}

void
CleanupImageCache(void)
{
   ImlibImage *im, *im_last;   
   int         current_cache;
   char        operation = 1;

   current_cache = CurrentCacheSize();
   while ((current_cache > cache_size) || (operation)); 
     {
	im_last = NULL;
	operation = 0;
	im = images;
	while(im)
	  {
	     if (im->references <= 0)
		im_last = im;
	     im = im->next;
	  }
	if (im_last)
	  {
	     RemoveImageFromCache(im_last);
	     ConsumeImage(im_last);
	     operation = 1;
	  }
	current_cache = CurrentCacheSize();
     }
}

ImlibImagePixmap *
ProduceImagePixmap(void)
{
   ImlibImagePixmap *ip;
   
   ip = malloc(sizeof(ImlibImagePixmap));
   ip->w = 0;
   ip->h = 0;
   ip->pixmap = 0;
   ip->mask = 0;
   ip->display = NULL;
   ip->visual = NULL;
   ip->depth = 0;
   ip->mode_count = 0;
   ip->image = NULL;
   ip->references = 0;
   ip->next = NULL;
   return ip;
}

void
ConsumeImagePixmap(ImlibImagePixmap *ip)
{
   if (ip->pixmap)
      XFreePixmap(ip->display, ip->pixmap);
   if (ip->mask)
      XFreePixmap(ip->display, ip->mask);
   free(ip);
}

ImlibImagePixmap *
FindCachedImagePixmap(ImlibImage *im, int w, int h, Display *d, Visual *v, 
		      int depth, int mode_count)
{
   ImlibImagePixmap *ip, *previous_ip;
   
   ip = pixmaps;
   previous_ip = NULL;
   /* go through the pixmap list */
   while (ip)
     {
	/* if all the pixmap attributes match */
	if ((ip->w == w) && (ip->h == h) && (ip->depth == depth) && 
	    (ip->visual == v) && (ip->display == d) && 
	    (ip->mode_count == mode_count))
	  {
	     /* move the pixmap to the head of the pixmap list */
	     if (previous_ip)
	       {
		  previous_ip->next = ip->next;
		  ip->next = pixmaps;
		  pixmaps = ip;
	       }
	     /* return it */
	     return ip;
	  }
	previous_ip = ip;
	ip = ip->next;	
     }
   return NULL;
}

void
AddImagePixmapToCache(ImlibImagePixmap *ip)
{
   ip->next = pixmaps;
   pixmaps = ip;
}

void
RemoveImagePixmapFromCache(ImlibImagePixmap *ip)
{
   ImlibImagePixmap *current_ip, *previous_ip;
   
   current_ip = pixmaps;
   previous_ip = NULL;
   while (current_ip)
     {
	if (ip == current_ip)
	  {
             if (previous_ip)
		previous_ip->next = ip->next;
	     else
		pixmaps = ip->next;
	     return;
	  }
	current_ip = current_ip->next;
     }	
}

void
CleanupImagePixmapCache()
{
   ImlibImagePixmap *ip, *ip_last;   
   int               current_cache;
   char              operation = 1;

   current_cache = CurrentCacheSize();
   while ((current_cache > cache_size) || (operation)); 
     {
	ip_last = NULL;
	operation = 0;
	ip = pixmaps;
	while(ip)
	  {
	     if (ip->references <= 0)
		ip_last = ip;
	     ip = ip->next;
	  }
	if (ip_last)
	  {
	     RemoveImagePixmapFromCache(ip_last);
	     ConsumeImagePixmap(ip_last);
	     operation = 1;
	  }
	current_cache = CurrentCacheSize();
     }
}

ImlibLoader *
ProduceLoader(char *file)
{
   ImlibLoader *l;
   void (*l_formats)(ImlibLoader *l) ;
   
   l = malloc(sizeof(ImlibLoader));
   l->num_formats = 0;
   l->formats = NULL;
   l->handle = dlopen(file, RTLD_NOW);
   if (!l->handle)
     {
	free(l);
	return NULL;
     }
   l->load = dlsym(l->handle, "load");
   l->save = dlsym(l->handle, "save");
   l_formats = dlsym(l->handle, "formats");
   if ((!(l->load)) || (!(l->save)) || (!(l_formats)))
     {
	dlclose(l->handle);
	free(l);
	return NULL;
     }
   l_formats(l);
   l->file = strdup(file);
   l->next = NULL;
   return l;
}

char **
ListLoaders(int *num_ret)
{
   char **list = NULL, **l, s[4096], *home;
   int num, i, pi = 0;
   
   *num_ret = 0;
   home = FileHomeDir(getuid());
   sprintf(s, "%s/.loaders/image/", home);
   l = FileDir(s, &num);
   if (num > 0)
     {
	*num_ret += num;
	list = malloc(sizeof(char *) * *num_ret);
	for (i = 0; i < num; i++)
	  {
	     sprintf(s, "%s/.loaders/image/%s", home, l[i]);
	     list[i] = strdup(s);
	  }
	pi = i;	
	FileFreeDirList(l, num);
     }
   sprintf(s, "/usr/lib/loaders/image/");
   l = FileDir(s, &num);
   if (num > 0)
     {
	*num_ret += num;
	list = realloc(list, sizeof(char *) * *num_ret);
	for (i = 0; i < num; i++)
	  {
	     sprintf(s, "/usr/lib/loaders/image/%s", l[i]);
	     list[pi + i] = strdup(s);
	  }
	FileFreeDirList(l, num);
	free(home);
     }
   return list;
}

void
ConsumeLoader(ImlibLoader *l)
{
   if (l->file)
      free(l->file);
   if (l->handle)
      dlclose(l->handle);
   if (l->formats)
     {
	int i;
	
	for (i = 0; i < l->num_formats; i++)
	   free(l->formats[i]);
	free(l->formats);
     }
   free(l);
}

void
RescanLoaders(void)
{
   static time_t last_scan_time = 0;
   static time_t last_modified_home_time = 0;   
   static time_t last_modified_system_time = 0;
   time_t current_time;
   char s[4096], *home;
   char do_reload = 0;
   
   /* dont stat the dir and rescan if we checked in the last 5 seconds */
   current_time = time(NULL);
   if ((current_time - last_scan_time) < 5)
      return;
   /* ok - was the system loaders dir contents modified ? */
   last_scan_time = current_time;
   if (FileIsDir("/usr/lib/loaders/image/"))
     {
	current_time = FileModDate("/usr/lib/loaders/image/");
	if (current_time > last_modified_system_time)
	  {
	     /* yup - set the "do_reload" flag */
	     do_reload = 1;
	     last_modified_system_time = current_time;
	  }
     }
   /* ok - was the users own loaders dir contents modified ? */
   home = FileHomeDir(getuid());
   sprintf(s, "%s/.loaders/image/", home);   
   free(home);   
   if (FileIsDir(s))
     {
	current_time = FileModDate(s);
	if (current_time > last_modified_home_time)
	  {
	     /* yup - set the "do_reload" flag */
	     do_reload = 1;
	     last_modified_home_time = current_time;
	  }
     }
   /* if we dont ned to reload the loaders - get out now */
   if (!do_reload)
      return;
   RemoveAllLoaders();
   LoadAllLoaders();
}

void
RemoveAllLoaders(void)
{
   ImlibLoader *l, *il;
   
   l = loaders;
   while(l)
     {
	il = l;
	l = l->next;
	ConsumeLoader(il);
     }
   loaders = NULL;
}

void
LoadAllLoaders(void)
{
   int    i, num;
   char **list;
   
   list = ListLoaders(&num);
   if (!list)
      return;
   
   for (i = num - 1; i >= 0; i--)
     {
	ImlibLoader *l;
	
	l = ProduceLoader(list[i]);
	if (l)
	  {
	     l->next = loaders;
	     loaders = l;
	  }
	if (list[i])
	   free(list[i]);
     }
   free(list);
}

ImlibLoader *
FindBestLoaderForFile(char *file)
{
   char *extension, *lower;
   ImlibLoader *l = NULL;

   /* use the file extension for a "best guess" as to what loader to try */
   /* first at any rate */
   extension = strdup(FileExtension(file));
   /* change the extensiont o all lwoer case as all "types" are listed as */
   /* lower case strings fromt he loader that represent all the possible */
   /* extensions that file format could have */
   lower = extension;
   while (*lower)
     {
	*lower = tolower(*lower); 
	lower++;
     }
   /* go through the loaders - first loader that claims to handle that */
   /* image type (extension wise) wins as a first guess to use - NOTE */
   /* this is an OPTIMISATION - it is possible the file has no extension */
   /* or has an unrecognised one but still is loadable by a loader. */
   /* if thkis initial loader failes to load the load mechanism will */
   /* systematically go from most recently used to least recently used */
   /* loader until one succeeds - or none are left and all have failed */
   /* and only if all fail does the laod fail. the lao9der that does */
   /* succeed gets it way tot he head of the list so it's going */
   /* to be used first next time in this search mechanims - this */
   /* assumes you tend to laod a few image types and ones generally */
   /* of the same format */
   l = loaders;
   while (l)
     {
	int i;
	
	for (i = 0; i < l->num_formats; i++)
	  {
	     if (!strcmp(l->formats[i], extension))
	       {
		  free(extension);
		  return l;
	       }
	  }
	l = l->next;
     }
   free(extension);
   return l;   
}

ImlibImage *
LoadImage(char *file)
{
   ImlibImage  *im;
   ImlibLoader *best_loader;
   
   /* see if we alreayd have the image cached */
   im = FindCachedImage(file);
   /* if we found a cached image and we shoudl always check that it is */
   /* accurate to the disk conents if they changed since we last loaded */
   /* and that it is still a valid image */
   if ((im) && (IMAGE_IS_VALID(im)) && (IMAGE_ALWAYS_CHECK_DISK(im)))
     {
	time_t current_modified_time;
	
	current_modified_time = FileModDate(file);
	/* if the file on disk is newer than the cached one */
	if (current_modified_time > im->moddate)
	  {
	     /* invalidate image */
	     im->flags &= F_INVALID;
	  }
	else
	  {
	     /* image is ok to re-use - program is just being stupid loading */
	     /* the same data twice */
	     im->references++;
	     return im;
	  }
     }
   /* either image in cache is invalid or we dont even have it in cache */
   /* so produce a new one and load an image into that */
   im = ProduceImage();
   im->file = strdup(file);
   im->moddate = FileModDate(file);
   /* ok - just check all our loaders are up to date */
   RescanLoaders();
   /* take a guess by extension on the best loader to use */
   best_loader = FindBestLoaderForFile(file);
   if (best_loader)
      best_loader->load(im);
   /* width is still 0 - the laoder didnt manage to do anything */
   if (im->w == 0)
     {
	ImlibLoader *l, *previous_l = NULL;
	l = loaders;
	/* run through all loaders and try load until one succeeds */
	while ((l) && (im->w == 0))
	  {
	     /* if its not the best loader that alreayd failed - try load */
	     if (l != best_loader)
		l->load(im);
	     /* if it failed - advance */
	     if (im->w == 0)
	       {
		  previous_l = l;
		  l = l->next;
	       }
	  }
	/* if we have a loader then its the loader that succeeded */
	/* move the successful loader to the head of the list */
	/* as long as it's not alreayd at the head of the list */
	if ((l) && (previous_l))
	  {
	     im->loader = l;
	     previous_l->next = l->next;
	     l->next = loaders;
	     loaders = l;
	  }
     }
   else
      im->loader = best_loader;
   /* all loaders have been tried and they all failed. free the skeleton */
   /* image struct we had and return NULL */
   if (im->w == 0)
     {
	ConsumeImage(im);
	return NULL;
     }
#if 0   
   /* FIXME: need to turn this png loading function into a loader andf then */
   /* remove the below stuff */
   im->data = RGBA_Load(file, &(im->w), &(im->h));
   im->flags = F_HAS_ALPHA;
#endif
   
   
   /* the laod succeeded - make sure the image is refernenced then add */
   /* it to our cache */
   im->references = 1;
   AddImageToCache(im);
   return im;
}

ImlibImagePixmap *
FindImlibImagePixmapByID(Display *d, Pixmap p)
{
   ImlibImagePixmap *ip;
   
   ip = pixmaps;
   /* go through the pixmap list */
   while (ip)
     {
	/* if all the pixmap ID & Display match */
	if ((ip->pixmap == p) && (ip->display == d))
	   return ip;
	ip = ip->next;	
     }
   return NULL;
}

void
FreeImage(ImlibImage *im)
{
   if (im->references > 0)
     {
	im->references--;
	CleanupImageCache();
     }
}

void
FreePixmap(Display *d, Pixmap p)
{
   ImlibImagePixmap *ip;
   
   ip = FindImlibImagePixmapByID(d, p);
   if (ip->references > 0)
     {
	ip->references--;
	CleanupImagePixmapCache();
     }
}

