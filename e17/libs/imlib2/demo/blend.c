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
Imlib_Image im      = NULL;
Imlib_Image bg_im   = NULL;
Imlib_Image fog1_im = NULL;
Imlib_Image fog2_im = NULL;
Imlib_Image logo_im = NULL;
Imlib_Image fog3_im = NULL;
Imlib_Image fog4_im = NULL;

int 
main (int argc, char **argv)
{
   int x1, x2, x3, x4;
   if (argc < 2)
      return 1;
   
   disp  = XOpenDisplay(NULL);
   vis   = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));    
   cm    = DefaultColormap(disp, DefaultScreen(disp));
   win   = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 
			       10, 10, 0, 0, 0);
   XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask | 
		ButtonMotionMask | PointerMotionMask);
   bg_im = imlib_load_image(argv[1]);
   image_width  = imlib_image_get_width(bg_im);
   image_height = imlib_image_get_height(bg_im);
   XResizeWindow(disp, win, image_width, image_height);
   XMapWindow(disp, win);
   XSync(disp, False);
   im = imlib_create_image(image_width, image_height);
   fog1_im = imlib_load_image(argv[2]);
   fog2_im = imlib_load_image(argv[3]);
   logo_im = imlib_load_image(argv[4]);
   fog3_im = imlib_load_image(argv[5]);
   fog4_im = imlib_load_image(argv[6]);
   
   imlib_image_tile_horizontal(fog1_im);
   imlib_image_tile_horizontal(fog2_im);
   imlib_image_tile_horizontal(fog3_im);
   imlib_image_tile_horizontal(fog4_im);
   
   x1 = 0;
   x2 = 0;
   x3 = 0;
   x4 = 0;
   for(;;)
     {
	imlib_blend_image_onto_image(bg_im, im, 0, 0, 0,
				     0, 0, image_width, image_height,
				     0, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(fog1_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     -x1, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(fog1_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     image_width - x1, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(fog2_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     - x2, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(fog2_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     image_width - x2, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(logo_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     0, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(fog3_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     -x3, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(fog3_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     image_width - x3, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(fog4_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     - x4, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_blend_image_onto_image(fog4_im, im, 0, 1, 0,
				     0, 0, image_width, image_height,
				     image_width - x4, 0, image_width, image_height,
				     NULL, IMLIB_OP_COPY);
	imlib_render_image_part_on_drawable_at_size(im, 
						    disp, win, vis, cm, depth, 
						    0, 1, 0,
						    0, 0, image_width, image_height,
						    0, 0, image_width, image_height,
						    NULL, IMLIB_OP_COPY);
	x1 -=1;
	x2 -=2;
	x3 -=4;
	x4 -=8;
	if (x1 < 0)
	   x1 += image_width;
	if (x2 < 0)
	   x2 += image_width;
	if (x3 < 0)
	   x3 += image_width;
	if (x4 < 0)
	   x4 += image_width;
     }
   return 0;
}
