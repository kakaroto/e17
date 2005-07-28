#include "config.h"
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

Display            *disp;
Visual             *vis;
Colormap            cm;
int                 depth;
int                 image_width = 0, image_height = 0;

int
main(int argc, char **argv)
{
   Imlib_Image        *im = NULL;
   char               *file = NULL;
   int                 no = 1;
   const char         *display_name = getenv("DISPLAY");

   if (argc < 2)
      return 1;

   file = argv[no];
   if (display_name == NULL)
       display_name = ":0";
   disp = XOpenDisplay(display_name);
   if (disp == NULL)
     {
       fprintf(stderr, "Can't open display %s\n", display_name);
       return 1;
     }
   vis = DefaultVisual(disp, DefaultScreen(disp));
   depth = DefaultDepth(disp, DefaultScreen(disp));
   cm = DefaultColormap(disp, DefaultScreen(disp));
   imlib_context_set_display(disp);
   imlib_context_set_visual(vis);
   imlib_context_set_colormap(cm);
   imlib_context_set_drawable(DefaultRootWindow(disp));
   im = imlib_create_image_from_drawable(0, 0, 0, 
					 DisplayWidth(disp, DefaultScreen(disp)),
					 DisplayHeight(disp, DefaultScreen(disp)),
					 1);
   if (!im)
     {
        fprintf(stderr, "Cannot grab image!\n");
        exit(0);
     }
   if (argc > 1)
     {
	imlib_context_set_image(im);
	imlib_save_image(argv[1]);
     }
   return 0;
}
