#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* Some architectures require non-text files be opened in binary mode. */
#ifdef USE_FOPEN_BINARY
#  define FOPEN_BINARY_FLAG "b"
#else
#  define FOPEN_BINARY_FLAG
#endif

#include "common.h"
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "image.h"
#include <png.h>

/* this is a quick sample png loader module... nice and small isnt it? */

/* PNG stuff */
#define PNG_BYTES_TO_CHECK 4

static void *
_load_PNG (int *ww, int *hh, FILE *f)
{
  png_structp         png_ptr;
  png_infop           info_ptr;
  unsigned char      *data, **lines;
  int                 i, bit_depth, color_type, interlace_type;
  unsigned char       buf[PNG_BYTES_TO_CHECK];
  int                 w, h;
  
  fread(buf, 1, PNG_BYTES_TO_CHECK, f);
  if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
    {
      rewind(f);
      return NULL;
    }
  rewind(f);
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    {
      rewind(f);
      return NULL;
    }
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      rewind(f);
      return NULL;
    }
  if (setjmp(png_ptr->jmpbuf))
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      rewind(f);
      return NULL;
    }
  png_init_io(png_ptr, f);
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *)(&w), (png_uint_32 *)(&h),
	       &bit_depth, &color_type, &interlace_type,
	       NULL, NULL);
  /* Palette -> RGB */
  if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    png_set_expand(png_ptr);
   /* we want ARGB */
   png_set_bgr(png_ptr);
   /* 16bit color -> 8bit color */
  png_set_strip_16(png_ptr);
  /* pack all pixels to byte boundaires */
  png_set_packing(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand(png_ptr);
  png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
  *ww = w;
  *hh = h;  
  data = malloc(w * h * 4);
  lines = (unsigned char **)malloc(h * sizeof(unsigned char *));
  for (i = 0; i < h; i++)
    lines[i] = data + (i * w * 4);  
  png_read_image(png_ptr, lines);  
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  free(lines);  
  return data;
}

DATA32 *
RGBA_Load(char *file, int *w, int *h)
{
  FILE *f;
  DATA32 *data;

  f = fopen(file, "r" FOPEN_BINARY_FLAG);
  data = (DATA32 *)_load_PNG(w, h, f);
  fclose(f);
  return data;
}

char 
load (ImlibImage *im,
      void (*progress)(ImlibImage *im, char percent, 
		       int update_x, int update_y, 
		       int update_w, int update_h),
      char progress_granularity, char immediate_load)
{
   int w, h;
   DATA32 *data;
   
   /* if immediate_load is 1, then dont delay image laoding as below, or */
   /* already data in this image - dont load it again */
   if (im->data)
      return 0;
   /* ok - this is not 100% right. */
   /* what I SHOULD be doing is checking the im->loader value - if it's */
   /* set and im->data is NULL - then I do the SECOND loading phase. */
   /* this Is optional - but an OPTIMISED loader (like all loaders should */
   /* if the format allows it) has 2 loading phases. the first time the */
   /* loader is called If it can withotu performance penalty, read the */
   /* image header - glean ingormation like width, height, if the image */
   /* has an alpha channel or not - etc. If this information can be gathered */
   /* now without haveing to also decode the image data, and if it is */
   /* possible for the image data to later in a second load / phase, to be */
   /* loaded separately withotu significant penalty, then the loader */
   /* shoudl delay imge data decoding until it is actually needed in the */
   /* second load phase. if it is unable to do this, or the author does */
   /* not have time to do it properly, then ALL data, including image data */
   /* should be decoded on the first phase. The loader, if it does this */
   /* shoudl ignore the image laod if the data memebr is not NULL */
   /* the below code for now just does a one phase load */
   data = RGBA_Load(im->file, &w, &h);
   if (data)
     {
	im->data = data;
	SET_FLAG(im->flags, F_HAS_ALPHA);
	/* setting the width to somthign > 0 means you managed to load */
	/* the image */	
	im->w = w;
	im->h = h;
	/* set the format string member to the lower-case full extension */
	/* name for the format - so example names would be: */
	/* "png", "jpeg", "tiff", "ppm", "pgm", "pbm", "gif", "xpm" ... */
	im->format = strdup("png");
	/* success - retrun 1 */
	return 1;
     }
   /* failure to load... return 0 */
   return 0;
}

char 
save (ImlibImage *im,
      void (*progress)(ImlibImage *im, char percent, 
		       int update_x, int update_y, 
		       int update_w, int update_h),
      char progress_granularity)
{
   /* if we cant do this - just return 0 */
   return 0;
}

/* fills the ImlibLoader struct with a strign array of format file */
/* extensions this loader can load. eg: */
/* loader->formats = { "jpeg", "jpg"}; */
/* giving permutations is a good idea. case sensitivity is irrelevant */
/* your laoder CAN load more than one format if it likes - like: */
/* loader->formats = { "gif", "png", "jpeg", "jpg"} */
/* if it can load those formats. */
void 
formats (ImlibLoader *l)
{  
   /* this is the only bit you have to change... */
   char *list_formats[] = 
     { "png" };

   /* don't bother changing any of this - it just reads this in and sets */
   /* the struct values and makes copies */
     {
	int i;
	
	l->num_formats = (sizeof(list_formats) / sizeof (char *));
	l->formats = malloc(sizeof(char *) * l->num_formats);
	for (i = 0; i < l->num_formats; i++)
	   l->formats[i] = strdup(list_formats[i]);
     }
}

