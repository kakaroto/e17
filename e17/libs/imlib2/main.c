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
   char *fon = NULL, *str = NULL;
   
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
	else if (!strcmp(argv[i], "-loop"))
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
	else if (!strcmp(argv[i], "-font"))
	  {
	     i++;
	     fon = argv[i];
	  }
	else if (!strcmp(argv[i], "-text"))
	  {
	     i++;
	     str = argv[i];
	  }
	else
	   file = argv[i];
     }
   printf("init\n");
   disp = XOpenDisplay(NULL);
   vis = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));    
   printf("%i\n", depth);
   cm = DefaultColormap(disp, DefaultScreen(disp));
   if (root)
      win = DefaultRootWindow(disp);
   else
     {
	win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 10, 10, 0, 0, 0);
	XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask | 
		     ButtonMotionMask | PointerMotionMask | ExposureMask);
     }
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
	int wo, ho, px, py, first = 1;
	Imlib_Image im_bg, im_sh1, im_sh2, im_sh3, im_ic[13], im_tmp;
	Imlib_Border border;
	Imlib_Updates up = NULL;
	int x, y, dum, i, j;
	unsigned int dui;
	Window rt;
	XEvent ev;
	Imlib_Font fn;
	
	/* "ARIAL/30" "COMIC/30" "IMPACT/30" "Prole/30" "Proteron/30" */
	/* "TIMES/30" "badacid/30" "bajoran/30" "bigfish/30" */
	__imlib_add_font_path("./ttfonts");
	if (fon)
	  {
	     fn = __imlib_load_font(fon);
	     if (!fn) 
		fon = NULL;
	  }
	
	if (file)
	   im_bg = imlib_load_image(file);
	else
	   im_bg = imlib_load_image("test_images/bg.png");
	im_tmp = imlib_clone_image(im_bg);
	w = imlib_image_get_width(im_bg);
	h = imlib_image_get_height(im_bg);
	wo = w;
	ho = h;
	w *= 1;
	h *= 1;
	XResizeWindow(disp, win, w, h);
	XSync(disp, False);
	im = imlib_create_image(w, h);
	imlib_set_cache_size(4 * 1024 * 1024);
	i = 0;
	up = imlib_update_append_rect(up, 0, 0, w, h);
	x = -9999;
	y = -9999;
	while (1)
	  {
	     px = x;
	     py = y;
	     do 
	       {
		  XNextEvent(disp, &ev);
		  switch (ev.type)
		    {
		    case Expose:
		       up = imlib_update_append_rect(up, 
						     ev.xexpose.x, ev.xexpose.y, 
						     ev.xexpose.width, ev.xexpose.height);
		       break;
		    case ButtonRelease:
		       exit(0);
		       break;
		    case MotionNotify:
		       x = ev.xmotion.x;
		       y = ev.xmotion.y;
		    default:
		       break;
		       
		    }
	       }
	     while (XPending(disp));
	     
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

	     if (first)
	       {
		  imlib_blend_image_onto_image(im_bg, im,
					       0, 0, 0,
					       0, 0, w, h,
					       0, 0, w, h,
					       NULL, IMLIB_OP_COPY);
		  first = 0;
	       }
	     if (fon)
	       {
		  int retw, reth, ty, nx, ny;
		
		  if (!str)
		     str = "This is a test string";
		  ty = y;
		  for (i = 0; i < 16; i++)
		    {
		       int al;
		       al = (15 - i) * 16;
		       if (al > 255)
			  al = 255;
		       __imlib_render_str(im, fn, x, ty, str,
					  255, 255, 255, al, 
					  0, &retw, &reth, 0, &nx, &ny);
		       up = imlib_update_append_rect(up, px, ty + (py - y), retw, reth);
		       up = imlib_update_append_rect(up, x, ty, retw, reth);
		       ty += ny;
		    }
	       }
	     if ((px != x) || (py != y))
	       {
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
			    up = imlib_update_append_rect(up, x + (i * iw * 2), 
							  y + (j * ih * 2), ww, hh);
			    up = imlib_update_append_rect(up, px + (i * iw * 2), 
							  py + (j * ih * 2), ww, hh);
			    imlib_blend_image_onto_image(im_ic[ic], im, 
							 aa, blend, 0,
							 0, 0, iw, ih, 
							 x + (i * iw * 2), 
							 y + (j * ih * 2), 
							 ww, hh,
							 NULL, IMLIB_OP_COPY);
			 }
		    }
	       }
	     imlib_blend_image_onto_image(im_sh1, im, 
					  aa, blend, 0,
					  0, 0, 50, 50, 
					  0, 0, 50, 50, NULL, IMLIB_OP_COPY);
	     up = imlib_update_append_rect(up, 0, 0, 50, 50);
	     imlib_blend_image_onto_image(im_sh2, im, 
					  aa, blend, 0,
					  0, 0, 50, 50, 
					  50, 0, w - 50, 50, NULL, IMLIB_OP_COPY);
	     up = imlib_update_append_rect(up, 50, 0, w - 50, 50);
	     imlib_blend_image_onto_image(im_sh3, im, 
					  aa, blend, 0,
					  0, 0, 50, 50, 
					  0, 50, 50, h - 50, NULL, IMLIB_OP_COPY);
	     up = imlib_update_append_rect(up, 0, 50, 50, h - 50);
	     up = imlib_updates_merge(up, w, h);
	     imlib_render_image_updates_on_drawable(im, up, disp, win, vis, cm,
						    depth, dith, 0, 0, NULL);
	     if ((px != x) || (py != y))
               {
		  Imlib_Updates u;
		  
		  u = up;
		  while (u)
		    {
		       int ux, uy, uw, uh;
		       
		       imlib_updates_get_coordinates(u, &ux, &uy, &uw, &uh);
		       imlib_blend_image_onto_image(im_bg, im,
						    0, 0, 0,
						    ux, uy, uw, uh,
						    ux, uy, uw, uh,
						    NULL, IMLIB_OP_COPY);
		       u = imlib_updates_get_next(u);
		    }
	       }
#if 0	     
	       {
		  Imlib_Updates up2;
		  GC gc;
		  XGCValues gcv;
		  
		  gc = XCreateGC(disp, win, 0, &gcv);
		  XSetForeground(disp, gc, 0xffff);
		  up2 = up;
		  while(up2)
		    {
		       int ux, uy, uw, uh;
		       
		       imlib_updates_get_coordinates(up2, &ux, &uy, &uw, &uh);
		       XDrawRectangle(disp, win, gc, ux, uy, uw - 1, uh - 1);
		       up2 = imlib_updates_get_next(up2);
		    }
		  XFreeGC(disp, gc);
	       }
#endif	     
	     imlib_updates_free(up);
	     up = NULL;
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
