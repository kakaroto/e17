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
#include "Imlib2.h"

Display *disp;
Window   win;
Pixmap   pm;
Visual  *vis;
Colormap cm;
int      depth;
int      image_width = 0, image_height = 0;

static void 
progress(Imlib_Image im, char percent, int update_x, int update_y,
	 int update_w, int update_h);

static void
progress(Imlib_Image im, char percent, int update_x, int update_y,
	 int update_w, int update_h)
{
   static Imlib_Image bg_im = NULL;
   
   /* first time it's called */
   if (image_width == 0)
     {
	int x, y, onoff;
	
	image_width = imlib_image_get_width(im);
	image_height = imlib_image_get_height(im);
	pm = XCreatePixmap(disp, win, image_width, image_height, depth);
	bg_im = imlib_create_image(image_width, image_height);
	for (y = 0; y < image_height; y += 8)
	  {
	     onoff = y & 0x1;
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
					       0, 1, 0,
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
/*   
   imlib_render_image_part_on_drawable_at_size(bg_im, 
					       disp, pm, vis, cm, depth, 
					       0, 1, 0,
					       update_x, update_y,
					       update_w, update_h,
					       update_x, update_y,
					       update_w, update_h,
					       NULL, IMLIB_OP_COPY);*/
   XSetWindowBackgroundPixmap(disp, win, pm);
   XClearArea(disp, win, update_x, update_y, update_w, update_h, False);
}

int 
main (int argc, char **argv)
{
   Imlib_Image *im   = NULL;
   char        *file = NULL;
      
   file  = argv[1];
   disp  = XOpenDisplay(NULL);
   vis   = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));    
   cm    = DefaultColormap(disp, DefaultScreen(disp));
   win   = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 
			       10, 10, 0, 0, 0);
   XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask | 
		ButtonMotionMask | PointerMotionMask);
   im = imlib_load_image_with_progress_callback(file, progress, 0);
   if (!im)
     {
	fprintf(stderr, "Image format not available\n");
	exit(0);
     }
   for(;;)
     {
	int x, y, b;
	XEvent ev;
	
	XNextEvent(disp, &ev);
	switch (ev.type)
	  {
	  case Expose:
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
   return 0;
}
