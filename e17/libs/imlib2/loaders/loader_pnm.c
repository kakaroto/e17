#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "common.h"
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "image.h"

char load (ImlibImage *im,
	   void (*progress)(ImlibImage *im, char percent,
			    int update_x, int update_y,
			    int update_w, int update_h),
	   char progress_granularity, char immediate_load);
char save (ImlibImage *im,
	   void (*progress)(ImlibImage *im, char percent,
			    int update_x, int update_y,
			    int update_w, int update_h),
	   char progress_granularity);
void formats (ImlibLoader *l);

char 
load (ImlibImage *im,
      void (*progress)(ImlibImage *im, char percent, 
		       int update_x, int update_y, 
		       int update_w, int update_h),
      char progress_granularity, char immediate_load)
{
   char                 ascii = 0, alpha = 0, color = 0, mono = 0;
   char                 
   int                  w, h;
   FILE               *f;
   
   if (im->data)
      return 0;
   f = fopen(im->file, "rb");
   if (!f)
      return 0;

   if ((!im->loader) && (!im->data))
     {
	/* red the header info */
	im->w = w;
	im->h = h;
	if (!alpha)
	   UNSET_FLAG(im->flags, F_HAS_ALPHA);
	im->format = strdup("pnm");
     }
   if (((!im->data) && (im->loader)) || (immediate_load) || (progress))
     {
	DATA8  *ptr;
	DATA32 *ptr2;
	int     x, y, i;
	
	im->w = w;
	im->h = h;
	
	data = malloc(w * 3);
	if (!data)
	  {
	     fclose(f);
	     return 0;
	  }
	/* must set the im->data member before callign progress function */
	ptr2 = im->data = malloc(w * h * sizeof(DATA32));
	if (!im->data)
	  {
	     free(data);
	     fclose(f);
	     return 0;
	  }
	for (y = 0; y < h; y++)
	  {
	     if (progress)
	       {
		  int per;
		  
		  per = (y * 100) / h;
		  progress(im, per, 0, y, w, 1);
	       }
	  }
	free(data);	
     }
   fclose(f);
   return 1;
}

char 
save (ImlibImage *im,
      void (*progress)(ImlibImage *im, char percent, 
		       int update_x, int update_y, 
		       int update_w, int update_h),
      char progress_granularity)
{
   FILE               *f;
   DATA8              *buf;
   DATA32             *ptr;
   int                 y = 0, quality = 75;
   ImlibImageTag      *tag;

   /* no image data? abort */
   if (!im->data)
      return 0;
   /* allocate a small buffer to convert image data */
   buf = malloc(im->w * 3 * sizeof(DATA8));
   if (!buf)
      return 0;
   f = fopen(im->file, "wb");
   if (!f)
     {
	free(buf);
	return 0;
     }
   /* look for tags attached to image to get extra parameters liek quality */
   /* settigns etc. - thsi si the "api" to hint for extra information for */
   /* saver modules */
   tag = __imlib_GetTag(im, "quality");
   if (tag)
      quality = tag->val;
   if (quality < 1)
      quality = 1;
   if (quality > 100)
      quality = 100;

     {
	if (progress)
	  {
	     char per;
	     
	     per = (char)((100 * y) / im->h);
	     progress(im, per, 0, y, im->w, 1);
	  }
     }
   /* finish off */
   free(buf);
   fclose(f);
   return 1;
   progress = NULL;
}

void 
formats (ImlibLoader *l)
{  
   char *list_formats[] = 
     { "pnm", "ppm", "pgm", "pbm", "pam" };

     {
	int i;
	
	l->num_formats = (sizeof(list_formats) / sizeof (char *));
	l->formats = malloc(sizeof(char *) * l->num_formats);
	for (i = 0; i < l->num_formats; i++)
	   l->formats[i] = strdup(list_formats[i]);
     }
}

