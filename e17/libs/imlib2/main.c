#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <sys/time.h>
#include "common.h"
#include "image.h"
#include "rend.h"
#include "rgba.h"


Display *disp;

int main (int argc, char **argv)
{
   Window win;
   int i, j;
   ImlibImage *im;
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
   
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-root"))
	   root = 1;
	else if (!strcmp(argv[i], "-smooth"))
	   aa = 1;
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
   __imlib_RGBA_init();
   disp = XOpenDisplay(NULL);
   printf("load\n");
   im = __imlib_LoadImage(file, NULL, 0, 0, 0);
   if (!im)
      printf("load fialed\n");
   if (w < 0)
     {
	w = im->w;
	h = im->h;
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
   if (depth == 8)
      __imlib_AllocColorTable(disp, DefaultColormap(disp, DefaultScreen(disp)));
   XSync(disp, False);
   printf("rend\n");
   gettimeofday(&timev,NULL);
   sec1=(int)timev.tv_sec; /* and stores it so we can time outselves */
   usec1=(int)timev.tv_usec; /* we will use this to vary speed of rot */
   __imlib_SetMaxXImageCount(disp, 5);  
   if (loop)
     {
	for (i = 0; i < w; i++)
	  {
	     static Pixmap m = 0;
	     
	     if (m)
		XFreePixmap(disp, m);
	     m = 0;
	     /*	  
	      if (((w - i) > 0) && ((((w - i) * h) / w) > 0))
	      m = XCreatePixmap(disp, win, (w - i), ((w - i) * h) / w, 1);
	      */	  __imlib_RenderImage(disp, im, 
				      win, m, 
				      vis, 
				      DefaultColormap(disp, DefaultScreen(disp)), 
				      depth, 
				      0, 0, im->w, im->h, 
				      0, 0, w - i, ((w - i) * h) / w,
				      (char)aa, (char)dith, (char)blend, 0
				      );
	     if (m)
	       {
		  XShapeCombineMask(disp, win, ShapeBounding, 0, 0, m, ShapeSet);
	       }
	     pixels += (w - i) * (((w - i) * h) / w);
	  }
     }
   else
     {
	__imlib_RenderImage(disp, im, 
		    win, 0, 
		    vis, 
		    DefaultColormap(disp, DefaultScreen(disp)), 
		    depth, 
		    0, 0, im->w, im->h, 
		    0, 0, w, h,
		    (char)aa, (char)dith, (char)blend, 0
		    );
	pixels += (w) * (h);
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
