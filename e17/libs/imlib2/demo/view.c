#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "Imlib2.h"

Display *disp;
Window   win;
Pixmap   pm = 0;
Visual  *vis;
Colormap cm;
int      depth;
int      image_width = 0, image_height = 0;
Imlib_Image bg_im = NULL;

static void 
progress(Imlib_Image im, char percent, int update_x, int update_y,
	 int update_w, int update_h);

static void
progress(Imlib_Image im, char percent, int update_x, int update_y,
	 int update_w, int update_h)
{
   /* first time it's called */
   if (image_width == 0)
     {
	int x, y, onoff;
	
	image_width  = imlib_image_get_width(im);
	image_height = imlib_image_get_height(im);
	if (pm)
	   XFreePixmap(disp, pm);
	pm = XCreatePixmap(disp, win, image_width, image_height, depth);
	if (bg_im)
	   imlib_free_image_and_decache(bg_im);
	bg_im = imlib_create_image(image_width, image_height);
	for (y = 0; y < image_height; y += 8)
	  {
	     onoff = (y / 8) & 0x1;
	     for (x = 0; x < image_width; x += 8)
	       {
		  Imlib_Color col;
		  
		  if (onoff)
		    {
		       col.red   = 144;
		       col.green = 144;
		       col.blue  = 144;
		       col.alpha = 255;
		    }
		  else
		    {
		       col.red   = 100;
		       col.green = 100;
		       col.blue  = 100;
		       col.alpha = 255;
		    }
		  imlib_image_fill_rectangle(bg_im, x, y, 8, 8, &col, 
					     IMLIB_OP_COPY);
		  onoff++;
		  if (onoff == 2)
		     onoff = 0;
	       }
	  }
	imlib_render_image_part_on_drawable_at_size(bg_im, 
					       disp, pm, vis, cm, depth, 
					       0, 0, 0,
					       0, 0,
					       image_width, image_height,
					       0, 0,
					       image_width, image_height,
					       NULL, IMLIB_OP_COPY);
	XSetWindowBackgroundPixmap(disp, win, pm);
	XResizeWindow(disp, win, image_width, image_height);
	XMapWindow(disp, win);
	XSync(disp, False);
     }
   imlib_blend_image_onto_image(im, bg_im, 0, 1, 0, 
				update_x, update_y,
				update_w, update_h,
				update_x, update_y,
				update_w, update_h,
				NULL, IMLIB_OP_COPY);   
   imlib_render_image_part_on_drawable_at_size(bg_im, 
					       disp, pm, vis, cm, depth, 
					       0, 1, 0,
					       update_x, update_y,
					       update_w, update_h,
					       update_x, update_y,
					       update_w, update_h,
					       NULL, IMLIB_OP_COPY);
   XSetWindowBackgroundPixmap(disp, win, pm);
   XClearArea(disp, win, update_x, update_y, update_w, update_h, False);
   XFlush(disp);
}

int 
main (int argc, char **argv)
{
   Imlib_Image *im   = NULL;
   char        *file = NULL;
   int          no   = 1;

   if (argc < 2)
      return 1;
   
   file  = argv[no];
   disp  = XOpenDisplay(NULL);
   vis   = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));    
   cm    = DefaultColormap(disp, DefaultScreen(disp));
   win   = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 
			       10, 10, 0, 0, 0);
   XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask | 
		ButtonMotionMask | PointerMotionMask);
   im = imlib_load_image_with_progress_callback(file, progress, 10);
   while (!im)
     {
	no++;
	if (no == argc)
	   exit(0);
	file = argv[no];
	image_width = 0;
	im = imlib_load_image_with_progress_callback(file, progress, 10);
     }
   if (!im)
     {
	fprintf(stderr, "Image format not available\n");
	exit(0);
     }
   for(;;)
     {
	int x, y, b, count, fdsize, xfd, timeout = 0;
	XEvent ev;
	static int zoom_mode = 0, zx, zy;
	static double zoom = 1.0;
	char   aa = 0, dith = 0;
	struct timeval tval;
	fd_set         fdset;
	double t1;
	
	XFlush(disp);
        while (XPending(disp))
	  {
	     XNextEvent(disp, &ev);
	     switch (ev.type)
	       {
	       case ButtonPress:
		  b = ev.xbutton.button;
		  x = ev.xbutton.x;
		  y = ev.xbutton.y;
		  if (b == 3)
		    {
		       zoom_mode = 1;
		       zx = x;
		       zy = y;
		       imlib_render_image_part_on_drawable_at_size
			  (bg_im, 
			   disp, pm, vis, cm, depth, 
			   0, 1, 0,
			   0, 0, image_width, image_height, 
			   0, 0, image_width, image_height,
			   NULL, IMLIB_OP_COPY);
		       XSetWindowBackgroundPixmap(disp, win, pm);
		       XClearWindow(disp, win);
		    }
		  break;
	       case ButtonRelease:
		  b = ev.xbutton.button;
		  x = ev.xbutton.x;
		  y = ev.xbutton.y;
		  if (b == 3)
		     zoom_mode = 0;
		  if (b == 1)
		    {
		       no++;
		       if (no == argc)
			  no = argc - 1;
		       file = argv[no];
		       image_width = 0;
		       zoom = 1.0;
		       zoom_mode = 0;
		       imlib_free_image_and_decache(im);
		       im = imlib_load_image_with_progress_callback(file, progress, 10);
		       while (!im)
			 {
			    no++;
			    if (no == argc)
			       exit(0);
			    file = argv[no];
			    image_width = 0;
			    im = imlib_load_image_with_progress_callback(file, progress, 10);
			 }
		    }
		  if (b == 2)
		    {
		       no--;
		       if (no == 0)
			  no = 1;
		       file = argv[no];
		       image_width = 0;
		       zoom = 1.0;
		       zoom_mode = 0;
		       imlib_free_image_and_decache(im);
		       im = imlib_load_image_with_progress_callback(file, progress, 10);
		       while (!im)
			 {
			    no--;
			    if (no == 0)
			       no = 1;
			    file = argv[no];
			    image_width = 0;
			    im = imlib_load_image_with_progress_callback(file, progress, 10);
			 }
		    }
		  break;
	       case MotionNotify:
		  while (XCheckTypedWindowEvent(disp, win, MotionNotify, &ev));
		  x = ev.xmotion.x;
		  y = ev.xmotion.y;
		  if (zoom_mode)
		    {
		       int sx, sy, sw, sh, dx, dy, dw, dh;
		       
		       zoom = ((double)x - (double)zx) / 32.0;
		       if (zoom < 0)
			  zoom = 1.0 + ((zoom * 32.0) / ((double)(zx + 1)));
		       else
			  zoom += 1.0;
		       if (zoom <= 0.0001)
			  zoom = 0.0001;
		       if (zoom > 1.0)
			 {
			    dx = 0;
			    dy = 0;
			    dw = image_width;
			    dh = image_height;
			    
			    sx = zx - (zx / zoom);
			    sy = zy - (zy / zoom);
			    sw = image_width / zoom;
			    sh = image_height / zoom;
			 }
		       else
			 {
			    dx = zx - (zx * zoom);
			    dy = zy - (zy * zoom);
			    dw = image_width * zoom;
			    dh = image_height * zoom;
			    
			    sx = 0;
			    sy = 0;
			    sw = image_width;
			    sh = image_height;
			 }
		       imlib_render_image_part_on_drawable_at_size
			  (bg_im, 
			   disp, pm, vis, cm, depth, 
			   aa, dith, 0,
			   sx, sy, sw, sh, dx, dy, dw, dh,
			   NULL, IMLIB_OP_COPY);
		       XSetWindowBackgroundPixmap(disp, win, pm);
		       XClearWindow(disp, win);
		       XFlush(disp);
		       timeout = 1;
		    }
	       default:
		  break;		  
	       }
	     t1 = 0.2;
	     tval.tv_sec = (long)t1;
	     tval.tv_usec = (long)((t1 - ((double)tval.tv_sec)) * 1000000);
	     xfd = ConnectionNumber(disp);
	     fdsize = xfd + 1;
	     FD_ZERO(&fdset);
	     FD_SET(xfd, &fdset);
	     if (timeout)
		count = select(fdsize, &fdset, NULL, NULL, &tval);
	     else
		count = select(fdsize, &fdset, NULL, NULL, NULL);
	     if (count < 0)
	       {
		  if ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF))
		     exit(1);
	       }
	     else
	       {
		  if ((count == 0) && (timeout))
		    {
		       int sx, sy, sw, sh, dx, dy, dw, dh;
		       
		       if (zoom > 1.0)
			 {
			    dx = 0;
			    dy = 0;
			    dw = image_width;
			    dh = image_height;
			    
			    sx = zx - (zx / zoom);
			    sy = zy - (zy / zoom);
			    sw = image_width / zoom;
			    sh = image_height / zoom;
			 }
		       else
			 {
			    dx = zx - (zx * zoom);
			    dy = zy - (zy * zoom);
			    dw = image_width * zoom;
			    dh = image_height * zoom;
			    
			    sx = 0;
			    sy = 0;
			    sw = image_width;
			    sh = image_height;
			 }
		       imlib_render_image_part_on_drawable_at_size
			  (bg_im, 
			   disp, pm, vis, cm, depth, 
			   1, 1, 0,
			   sx, sy, sw, sh, dx, dy, dw, dh,
			   NULL, IMLIB_OP_COPY);
		       XSetWindowBackgroundPixmap(disp, win, pm);
		       XClearWindow(disp, win);
		       XFlush(disp);
		       timeout = 0;
		    }
	       }
	     
	  }
     }
   return 0;
}
