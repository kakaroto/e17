#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
/*
#include <sys/time.h>
#include "common.h"
#include "image.h"
#include "rend.h"
#include "rgba.h"
#include "ximage.h"
#include "color.h"
 */
#include "api.h"

Display *disp;

#if 1
int main (int argc, char **argv)
{
   Window win;
   int i, j;
   Imlib_Image *im;
   Visual *vis;
   int depth;
   int sec1, usec1, sec2, usec2;
   int pixels = 0;
   struct timeval timev;
   double sec;
   char *file;
   
   int root = 0;
   int scale = 0;
   int w = -1;
   int h = -1;
   int aa = 0;
   int dith = 0;
   int loop = 1;
   int blend = 0;
   int interactive = 0;
   
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-root"))
	   root = 1;
	else if (!strcmp(argv[i], "-smooth"))
	   aa = 1;
	else if (!strcmp(argv[i], "-interactive"))
	  {
	     interactive = 1;
	     loop = 0;
	  }
	else if (!strcmp(argv[i], "-blend"))
	   blend = 1;
	else if (!strcmp(argv[i], "-dither"))
	   dith = 1;
	else if (!strcmp(argv[i], "-scale"))
	   scale = 1;
	else if (!strcmp(argv[i], "-noloop"))
	   loop = 0;
	else if (!strcmp(argv[i], "-size"))
	  {
	     i++;
	     w = atoi(argv[i++]);
	     h = atoi(argv[i]);
	  }
	else
	   file = argv[i];
     }
   printf("init\n");
   disp = XOpenDisplay(NULL);
   printf("load\n");
   im = imlib_load_image(file);
   if (!im)
     {
	printf("load fialed\n");
	exit(0);
     }
   if (w < 0)
     {
	w = imlib_image_get_width(im);
	h = imlib_image_get_height(im);   
     }
   if (root)
      win = DefaultRootWindow(disp);
   else
     {
	win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, w, h, 0, 0, 0);
	XMapWindow(disp, win);
     }
   if (scale)
     {
	Window d;
	int dd;
	
	XGetGeometry(disp, win, &d, &dd, &dd, &w, &h, &dd, &dd);
     }
   vis = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));    
   XSync(disp, False);
   printf("rend\n");
   gettimeofday(&timev,NULL);
   sec1=(int)timev.tv_sec; /* and stores it so we can time outselves */
   usec1=(int)timev.tv_usec; /* we will use this to vary speed of rot */
   __imlib_SetMaxXImageCount(disp, 0);
   if (loop)
     {
	for (i = 0; i < w; i++)
	  {
	     imlib_render_image_on_drawable_at_size(im, disp, win, vis, 
						    DefaultColormap(disp, DefaultScreen(disp)),
						    depth, 
						    aa, dith, blend, 
						    0, 0,
						    w - i, (((w - i) * h) / w),
						    NULL);
	     pixels += (w - i) * (((w - i) * h) / w);
	  }
     }
   else if (interactive)
     {
	Imlib_Image im2;
	
	im2 = imlib_create_image(640, 480);
	while (1)
	  {
	     int x, y, dum;
	     unsigned int dui;
	     Window rt;
	     
	     XQueryPointer(disp, win, &rt, &rt, &x, &y,
			   &dum, &dum, &dui);
	     imlib_copy_drawable_to_image(im2, disp, win, 0, vis, 
					  DefaultColormap(disp, DefaultScreen(disp)),
					  depth, x - (w / 2), y - (h / 2), w, h, 
					  x - w, y - h, 0);
	     imlib_render_image_on_drawable(im2, disp, win, vis, 
					    DefaultColormap(disp, DefaultScreen(disp)),
					    depth, dith, 0, 1700, 100, NULL);
	  }
     }
   else
     {
	for (i = 0; i < w; i++)
	  {
	     imlib_render_image_on_drawable_at_size(im, disp, win, vis, 
						    DefaultColormap(disp, DefaultScreen(disp)),
						    depth, 
						    aa, dith, blend, 
						    0, 0,
						    w, h,
						    NULL);
	     pixels += w * h;
	  }
     }
   gettimeofday(&timev,NULL);
   sec2=(int)timev.tv_sec; /* and stores it so we can time outselves */
   usec2=(int)timev.tv_usec; /* we will use this to vary speed of rot */
   printf("done\n");
   i = sec2 - sec1;
   j = usec2 - usec1;
   while (j < 0)
     {
	i++;
	j += 1000000;
     }
   sec = (double)i + ((double)j / 1000000);
   printf("%3.3f sec\n", sec);
   printf("%3.3f Mpixels / sec\n", (double)(pixels) / (sec * 1000000));
   return 0;
}
#else
int main (int argc, char **argv)
{
   Window win;
/*   
   Pixmap back, scratch;
   GC gc;
 */
   XGCValues gcv;
   Imlib_Image *im = NULL, tmp, grab;
   int x, y, start, i, w, h;
   Visual *vis;
   int depth;   
   int dith = 0;
   int blend = 0;
   DATA32 *data1, *data2;
   Colormap cm;
   
   start = 1;
   x = 0;
   y = 0;
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-blend"))
	   blend = 1;
	else if (!strcmp(argv[i], "-dither"))
	   dith = 1;
	else if (!strcmp(argv[i], "-pos"))
	  {
	     i++;
	     x = atoi(argv[i++]);
	     y = atoi(argv[i]);
	  }
	else
	  {
	     start = i;
	     i = argc;
	  }
     }
   disp = XOpenDisplay(NULL);
   printf("load\n");
   im = malloc(sizeof(Imlib_Image) * (argc - start));
   for (i = start; i < argc; i++)
      im[i - start] = imlib_load_image(argv[i]);
   win = DefaultRootWindow(disp);
   vis = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));    
   cm = DefaultColormap(disp, DefaultScreen(disp));
   __imlib_SetMaxXImageCount(disp, 0);  
   XSync(disp, False);
   printf("init\n");
   w = imlib_image_get_width(im[0]);
   h = imlib_image_get_height(im[0]);   
/*   
   gc = XCreateGC(disp, win, 0, &gcv);
   back = XCreatePixmap(disp, win, w, h, depth);
   scratch = XCreatePixmap(disp, win, w, h, depth);
   XCopyArea(disp, win, back, gc, x, y, w, h, 0, 0);
   XCopyArea(disp, back, scratch, gc, 0, 0, w, h, 0, 0);
 */
   grab = imlib_create_image_from_drawable(disp, win, 0, vis, cm, depth, 
					   x, y, w, h);
   tmp = imlib_clone_image(grab);
   
   data1 = imlib_image_get_data(grab);
   data2 = imlib_image_get_data(tmp);
   
   printf("animate\n");
   for(;;)
     {	
	for (i = 0; i < (argc - start); i++)
	  {
/*	     imlib_blend_image_onto_image(im[i], tmp, 0, 0, w, h, 0, 0, w, h);*/
	     imlib_render_image_on_drawable(im[i], disp, win, vis, cm, depth, 
					    dith, 0, 
					    x, y);
	     memcpy(data2, data1, w * h *sizeof(DATA32));
	  }
     }
   return 0;
}
#endif
