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
Window win;
Visual *vis;
Colormap cm;
int depth;

void
progress(Imlib_Image *im, char percent,
	 int update_x, int update_y,
	 int update_w, int update_h)
{
   imlib_render_image_part_on_drawable_at_size(im, disp, win, vis, cm, depth, 
					       0, 0, 0,
					       update_x, update_y,
					       update_w, update_h,
					       update_x, update_y,
					       update_w, update_h,
					       NULL, IMLIB_OP_COPY);
}

#if 1
int main (int argc, char **argv)
{
   int i, j;
   Imlib_Image *im;
   int sec1, usec1, sec2, usec2;
   int pixels = 0;
   struct timeval timev;
   double sec;
   char *file = NULL;
   
   int root = 0;
   int scale = 0;
   int w = 20;
   int h = 20;
   int aa = 0;
   int dith = 0;
   int loop = 0;
   int blend = 1;
   int interactive = 1;
   int blendtest = 0;
   
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-root"))
	   root = 1;
	else if (!strcmp(argv[i], "-smooth"))
	   aa = 1;
	else if (!strcmp(argv[i], "-interactive"))
	  {
	     interactive = 0;
	     loop = 1;
	  }
	else if (!strcmp(argv[i], "-blend"))
	   blend = 1;
	else if (!strcmp(argv[i], "-blendtest"))
	  {
	     blendtest = 1;
	     interactive = 0;
	  }
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
	else if (!strcmp(argv[i], "-maxcolors"))
	  {
	     i++;
	     imlib_set_color_usage(atoi(argv[i]));
	  }
	else
	   file = argv[i];
     }
   printf("init\n");
   disp = XOpenDisplay(NULL);
   vis = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));    
   cm = DefaultColormap(disp, DefaultScreen(disp));
   if (root)
      win = DefaultRootWindow(disp);
   else
      win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 10, 10, 0, 0, 0);
   if (!interactive)
     {
	im = imlib_load_image_with_progress_callback(file, progress, 0);
	if (!im)
	  {
	     printf("load fialed\n");
	     exit(0);
	  }
	w = imlib_image_get_width(im);
	h = imlib_image_get_height(im);   
     }
   if (!root)
     {
	XResizeWindow(disp, win, w, h);
	XMapWindow(disp, win);
     }
   if (scale)
     {
	Window d;
	int dd;
	
	XGetGeometry(disp, win, &d, &dd, &dd, &w, &h, &dd, &dd);
     }
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
						    cm,
						    depth, 
						    aa, dith, blend, 
						    0, 0,
						    w - i, (((w - i) * h) / w),
						    NULL, IMLIB_OP_COPY);
	     pixels += (w - i) * (((w - i) * h) / w);
	  }
     }
   else if (blendtest)
     {
	Imlib_Image im2;

	im2 = imlib_create_image(w, h);
	w = imlib_image_get_width(im);
	h = imlib_image_get_height(im);   
	for (i = 0; i < 1024; i++)
	  {
             imlib_blend_image_onto_image(im, im2,
					  0, 0, 0,
					  0, 0, w, h,
					  0, 0, w, h, NULL, IMLIB_OP_COPY);
	     pixels += (w * h);	     
	  }
     }
   else if (interactive)
     {
	int wo, ho;
	Imlib_Image im_bg, im_sh1, im_sh2, im_sh3, im_ic[13];
	Imlib_Border border;

	if (file)
	   im_bg = imlib_load_image(file);
	else
	   im_bg = imlib_load_image("test_images/bg.png");
	w = imlib_image_get_width(im_bg);
	h = imlib_image_get_height(im_bg);
	wo = w;
	ho = h;
	w *= 1;
	h *= 1;
	XResizeWindow(disp, win, w, h);
	im = imlib_create_image(w, h);
	imlib_set_cache_size(4 * 1024 * 1024);
	while (1)
	  {
	     int x, y, dum, i, j;
	     unsigned int dui;
	     Window rt;
	     
	     im_sh1 = imlib_load_image("test_images/sh1.png");
	     im_sh2 = imlib_load_image("test_images/sh2.png");
	     im_sh3 = imlib_load_image("test_images/sh3.png");
	     im_ic[0] = imlib_load_image("test_images/audio.png");
	     im_ic[1] = imlib_load_image("test_images/folder.png");
	     im_ic[2] = imlib_load_image("test_images/mush.png");
	     im_ic[3] = imlib_load_image("test_images/paper.png");
	     im_ic[4] = imlib_load_image("test_images/mail.png");
	     im_ic[5] = imlib_load_image("test_images/calc.png");
	     im_ic[6] = imlib_load_image("test_images/cal.png");
	     im_ic[7] = imlib_load_image("test_images/stop.png");
	     im_ic[8] = imlib_load_image("test_images/globe.png");
	     im_ic[9] = imlib_load_image("test_images/menu.png");
	     im_ic[10] = imlib_load_image("test_images/tnt.png");
	     im_ic[11] = imlib_load_image("test_images/bulb.png");
	     im_ic[12] = imlib_load_image("test_images/lock.png");
	     XQueryPointer(disp, win, &rt, &rt, &dum, &dum,
			   &x, &y, &dui);
	     if ((dui) && (x > 0) && (y > 0) && (x < w) && (y < h))
		exit(0);
	     imlib_blend_image_onto_image(im_bg, im, 
					  0, 0, 0,
					  0, 0, wo, ho, 
					  0, 0, w, h,
					  NULL, IMLIB_OP_COPY);
	     for (j = 0; j < 32; j++)
	       {
		  for (i = 0; i < 32; i++)
		    {
		       int ic, iw, ih, ww, hh;
		       
		       ic = ((j * 32) + i) % 13;
		       iw = imlib_image_get_width(im_ic[ic]);
		       ih = imlib_image_get_height(im_ic[ic]);
		       ww = iw;
		       hh = ih;
		       imlib_blend_image_onto_image(im_ic[ic], im, 
						    aa, blend, 0,
						    0, 0, iw, ih, 
						    x + (i * iw), y + (j * ih), 
						    ww, hh,
						    NULL, IMLIB_OP_COPY);
		    }
	       }
	     imlib_blend_image_onto_image(im_sh1, im, 
					  aa, blend, 0,
					  0, 0, 50, 50, 
					  0, 0, 50, 50, NULL, IMLIB_OP_COPY);
	     imlib_blend_image_onto_image(im_sh2, im, 
					  aa, blend, 0,
					  0, 0, 50, 50, 
					  50, 0, w - 50, 50, NULL, IMLIB_OP_COPY);
	     imlib_blend_image_onto_image(im_sh3, im, 
					  aa, blend, 0,
					  0, 0, 50, 50, 
					  0, 50, 50, h - 50, NULL, IMLIB_OP_COPY);
	     imlib_render_image_on_drawable(im, disp, win, vis, 
					    cm, depth, dith, 1, 0, 0, NULL,
					    IMLIB_OP_COPY);
	     imlib_free_image(im_sh1);
	     imlib_free_image(im_sh2);
	     imlib_free_image(im_sh3);
	     imlib_free_image(im_ic[0]);
	     imlib_free_image(im_ic[1]);
	     imlib_free_image(im_ic[2]);
	     imlib_free_image(im_ic[3]);
	  }
     }
   else
     {
	for (i = 0; i < w; i++)
	  {
	     imlib_render_image_on_drawable_at_size(im, disp, win, vis, 
						    cm,
						    depth, 
						    aa, dith, blend, 
						    0, 0,
						    w, h,
						    NULL, IMLIB_OP_COPY);
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
