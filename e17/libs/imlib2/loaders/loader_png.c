#ifdef HAVE_CONFIG_H
#  include <config.h>
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
   png_uint_32         w32, h32;
   int                 w, h;
   char                hasa = 0;
   FILE               *f;
   png_structp         png_ptr = NULL;
   png_infop           info_ptr = NULL;
   int                 bit_depth, color_type, interlace_type;
   
   /* if immediate_load is 1, then dont delay image laoding as below, or */
   /* already data in this image - dont load it again */
   if (im->data)
      return 0;
   f = fopen(im->file, "rb");
   if (!f)
      return 0;
   /* read header */
   if (!im->data)
     {
	unsigned char       buf[PNG_BYTES_TO_CHECK];
	
	/* if we havent read the header before, set the header data */	
	fread(buf, 1, PNG_BYTES_TO_CHECK, f);
	if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
	  {
	     fclose(f);
	     return 0;
	  }
	rewind(f);
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, 
					 NULL);
	if (!png_ptr)
	  {
	     fclose(f);
	     return 0;
	  }
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	  {
	     png_destroy_read_struct(&png_ptr, NULL, NULL);
	     fclose(f);
	     return 0;
	  }
	if (setjmp(png_ptr->jmpbuf))
	  {
	     png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	     fclose(f);
	     return 0;
	  }
	png_init_io(png_ptr, f);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, 
		     (png_uint_32 *)(&w32), (png_uint_32 *)(&h32),
		     &bit_depth, &color_type, &interlace_type, NULL, NULL);
	im->w = (int)w32;
	im->h = (int)h32;
	if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
	   hasa = 1;
	else
	   hasa = 0;
	if (hasa)
	   SET_FLAG(im->flags, F_HAS_ALPHA);
	else
	   UNSET_FLAG(im->flags, F_HAS_ALPHA);
	/* set the format string member to the lower-case full extension */
	/* name for the format - so example names would be: */
	/* "png", "jpeg", "tiff", "ppm", "pgm", "pbm", "gif", "xpm" ... */
	if (!im->loader)
	   im->format = strdup("png");
     }
   /* if its the second phase load OR its immediate load or a progress */
   /* callback is set then load the data */
   if ((im->loader) || (immediate_load) || (progress))
     {
	unsigned char **lines;
	int             i;
	
	w = im->w;
	h = im->h;
	if (hasa)
	   png_set_expand(png_ptr);
        /* we want ARGB */
/* note form raster:                                                         */
/* thanks to mustapha for helping debug this on PPC Linux remotely by        */
/* sending across screenshots all the tiem and me figuring out form them     */
/* what the hell was up with the colors                                      */
/* now png loading shoudl work on big endian machines nicely                 */
#ifdef WORDS_BIGENDIAN
	png_set_swap_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_BEFORE);
#else	
	png_set_bgr(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
#endif	
	/* 16bit color -> 8bit color */
	png_set_strip_16(png_ptr);
	/* pack all pixels to byte boundaires */
	png_set_packing(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	   png_set_expand(png_ptr);
	im->data = malloc(w * h * sizeof(DATA32));
	if (!im->data)
	  {
	     png_read_end(png_ptr, info_ptr);
	     png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	     fclose(f);
	     return 0;
	  }
	lines = (unsigned char **)malloc(h * sizeof(unsigned char *));
	if (!lines)
	  {
	     free(im->data);
	     im->data = NULL;
	     png_read_end(png_ptr, info_ptr);
	     png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	     fclose(f);
	     return 0;
	  }
	for (i = 0; i < h; i++)
	   lines[i] = ((unsigned char *)(im->data)) + (i * w * sizeof(DATA32));
	if (progress)
	  {
	     int y, count, prevy, pass, number_passes, per, nrows = 1;

	     count = 0;
	     number_passes = png_set_interlace_handling(png_ptr);
	     for (pass = 0; pass < number_passes; pass++)
	       {
		  prevy = 0;
		  per = 0;
		  for (y = 0; y < h; y += nrows)
		    {
		       png_read_rows(png_ptr, &lines[y], NULL, nrows);

		       per = (((pass * h) + y) * 100) /  (h * number_passes);
		       if ((per - count) >=  progress_granularity)
			 {
			    count = per;
			    progress(im, per, 0, prevy, w, y - prevy + 1);
			    prevy = y + 1;
			 }		       
		    }
		  progress(im, per, 0, prevy, w, y - prevy + 1);
	       }
	  }
	else	   
	   png_read_image(png_ptr, lines);
	free(lines);	
	png_read_end(png_ptr, info_ptr);
     }
   png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
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
   png_structp         png_ptr;
   png_infop           info_ptr;
   DATA32             *ptr;
   int                 x, y, j;
   png_bytep           row_ptr, data = NULL;
   png_color_8         sig_bit;
   int                 pl = 0;
   char                pper = 0;
   
   f = fopen(im->file, "wb");
   if (!f)
      return 0;
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				     NULL, NULL, NULL);
   if (!png_ptr)
     {
	fclose(f);
	return 0;
     }
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
     {
	fclose(f);
	png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
	return 0;
     }
   if (setjmp(png_ptr->jmpbuf))
     {
	fclose(f);
	png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
	return 0;
     }
   png_init_io(png_ptr, f);
   if (im->flags & F_HAS_ALPHA)
     {
	png_set_IHDR(png_ptr, info_ptr, im->w, im->h, 8,
		     PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
#ifdef WORDS_BIGENDIAN
	png_set_swap_alpha(png_ptr);
#else
	png_set_bgr(png_ptr);
#endif
     }
   else
     {
	png_set_IHDR(png_ptr, info_ptr, im->w, im->h, 8,
		     PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	data = malloc(im->w * 3 * sizeof(char));
     }
   sig_bit.red = 8;
   sig_bit.green = 8;
   sig_bit.blue = 8;
   sig_bit.alpha = 8;
   png_set_sBIT(png_ptr, info_ptr, &sig_bit);
   png_write_info(png_ptr, info_ptr);
   png_set_shift(png_ptr, &sig_bit);
   png_set_packing(png_ptr);

   ptr = im->data;
   for (y = 0; y < im->h; y++)
     {
	if (im->flags & F_HAS_ALPHA)
	   row_ptr = (png_bytep)ptr;
	else
	  {
	     for (j = 0, x = 0; x < im->w; x++)
	       {
		  data[j++] = (ptr[x] >> 16) & 0xff;
		  data[j++] = (ptr[x] >> 8 ) & 0xff;
		  data[j++] = (ptr[x]      ) & 0xff;
	       }
	     row_ptr = (png_bytep)data;
	  }
	png_write_rows(png_ptr, &row_ptr, 1);
	if (progress)
	  {
	     char per;
	     int l;
	     
	     per = (char)((100 * y) / im->h);
	     if ((per - pper) >= progress_granularity)
	       {
		  l = y - pl;
		  progress(im, per, 0, (y - l), im->w, l);
		  pper = per;
		  pl = y;
	       }
	  }
	ptr += im->w;
     }
   if (data)
      free(data);
   png_write_end(png_ptr, info_ptr);
   png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
   
   fclose(f);
   return 1;
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

