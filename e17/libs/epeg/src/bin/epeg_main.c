#include "epeg_main.h"

/* main */

int
main(int argc, char **argv)
{
   Epeg_Image *im;

   if (argc != 3)
     {
	printf("Usage: %s input.jpg thumb.jpg\n", argv[0]);
	exit(0);
     }
   im = epeg_file_open(argv[1]);
   if (!im)
     {
	printf("cannot open %s\n", argv[1]);
	exit(-1);
     }
     {
	const char *com;
	Epeg_Thumbnail_Info info;
	int w, h;
	
	com = epeg_comment_get(im);
	if (com) printf("Comment: %s\n", com);
	epeg_thumbnail_comments_get(im, &info);
	if (info.mimetype)
	  {
	     printf("Thumb Mimetype: %s\n", info.mimetype);
	     if (info.uri) printf("Thumb URI: %s\n", info.uri);
	     printf("Thumb Mtime: %llu\n", info.mtime);
	     printf("Thumb Width: %i\n", info.w);
	     printf("Thumb Height: %i\n", info.h);
	  }
	epeg_size_get(im, &w, &h);
	printf("Image size: %ix%i\n", w, h);
     }
   
   epeg_decode_size_set           (im, 128, 96);
   
     {
	unsigned int *pixels;

/*	epeg_decode_colorspace_set(im, EPEG_ARGB32); */
	pixels = epeg_pixels_get(im, 0, 0, 128, 96);
	if (pixels)
	  {
	     int x, y;
	     unsigned int *p;
	     
/*	     printf("Image pixels:\n"); */
	     p = pixels;
	     for (y = 0; y < 96; y++)
	       {
		  for (x = 0; x < 128; x++)
		    {
/*		       printf("%08x ", p[0]);*/
		       p ++;
		    }
/*		  printf("\n");*/
	       }
	     epeg_pixels_free(im, pixels);
	  }
     }
   
   epeg_quality_set               (im, 75);
   epeg_thumbnail_comments_enable (im, 1);
   epeg_comment_set               (im, "Smelly pants!");
   epeg_file_output_set           (im, argv[2]);
   epeg_encode                    (im);
   epeg_close                     (im);
   return 0;
}
