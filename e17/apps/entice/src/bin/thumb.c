#include "entice.h"
#include <limits.h>

int
e_do_thumb(char *file, char *thumb)
{
   Imlib_Image         im;

   im = imlib_load_image(file);
   if (im)
     {
	Imlib_Image         im_thumb;
	int                 w, h, iw, ih;

	imlib_context_set_image(im);
	w = imlib_image_get_width();
	h = imlib_image_get_height();
	imlib_context_set_anti_alias(1);
	iw = 1;
	ih = 1;
	if (w > h)
	  {
	     if (w > 96)
	       {
		  iw = 96;
		  ih = (96 * h) / w;
	       }
	     else
	       {
		  iw = w;
		  ih = h;
	       }
	  }
	else
	  {
	     if (h > 96)
	       {
		  ih = 96;
		  iw = (96 * w) / h;
	       }
	     else
	       {
		  ih = h;
		  iw = w;
	       }
	  }
	im_thumb = imlib_create_cropped_scaled_image(0, 0, w, h, iw, ih);
	imlib_context_set_image(im_thumb);
	imlib_image_set_format("png");
	e_mkdirs(thumb);
	imlib_save_image(thumb);
	return 1;
     }
   return 0;
}

void
e_generate_thumb(Image * im)
{
#ifdef PATH_MAX
   char		       buf[PATH_MAX];
#else
   char                buf[4096];
#endif

   if (generating_image)
      return;
   if (im->generator > 0)
      return;

   sprintf(buf, "%s/.entice/thumbs/%s._.png", e_file_home(), im->file);
   if (im->thumb)
      free(im->thumb);

   im->thumb = strdup(buf);
   generating_image = im;

   /*
   if ((im->thumb) && (im->file))
     {
	if (e_file_modified_time(im->thumb) > e_file_modified_time(im->file))
	   return;
	if (!e_do_thumb(im->file, im->thumb)) {
	  printf("unable to load: %s\n", im->file);
	  image_delete(im);
	}
     }
   */
   
   im->generator = fork();
   if (im->generator != 0)
     {
	if (im->generator > 0)
	  {
	  }
	else
	  {
	     printf("Eeeeek - help. Can't fork???\n");
	  }
	return;
     }
   else	// child
     {
	if ((im->thumb) && (im->file))
	  {
	     if (e_file_modified_time(im->thumb) >
		 e_file_modified_time(im->file))
		exit(0);
	if (!e_do_thumb(im->file, im->thumb)) {
	  printf("unable to load: %s\n", im->file);
	  exit(1);
	}
	  }
	exit(0);
     }
    
}
