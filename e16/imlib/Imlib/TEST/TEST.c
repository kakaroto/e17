#include "Imlib.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
   Display            *disp;
   ImlibData          *id;
   Window             win;
   Pixmap             p;
   Pixmap             m;
   ImlibImage         *im;
   ImlibColorModifier mod;
   int w,h,i,j,k,n,o;
   double t1,t2,total;
   struct timeval timev;
   
   disp=XOpenDisplay(NULL);
   id=Imlib_init(disp);
   if (argc<=1) 
     {
       im = Imlib_create_image_from_drawable(id, DefaultRootWindow(disp), 0, 
					     30, 30, 400, 400);
     }
   im=Imlib_load_image(id,argv[1]);
   
   w=im->rgb_width;h=im->rgb_height;
   win=XCreateSimpleWindow(disp,DefaultRootWindow(disp),0,0,w,h,0,0,0);
   
   Imlib_render(id,im,w,h);
   p=Imlib_move_image(id,im);
   m=Imlib_move_mask(id,im);
   XSetWindowBackgroundPixmap(disp,win,p);
   if (m) XShapeCombineMask(disp,win,ShapeBounding,0,0,m,ShapeSet);
   XClearWindow(disp,win);
  XMoveWindow(disp, win, 400, 300);
   XMapWindow(disp,win);
   XSync(disp,False);
   i=0;j=0;k=0;m=0;
   printf("----- Testing Scaling Code -----\n");
   for(o=0;o<4;o++)
     {
	k=0;
	gettimeofday(&timev, NULL);
	t1=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
	for(n=0;n<256;n+=4)
	  {
	     i=n;j=(h*n)/w;
	     if (i<=0) i=1;
	     if (j<=0) j=1;
	     k+=(i*j);
	     Imlib_render(id,im,i,j);
	     Imlib_free_pixmap(id,p);
	     p=Imlib_move_image(id,im);
	     m=Imlib_move_mask(id,im);
	     XSetWindowBackgroundPixmap(disp,win,p);
	     if (m) XShapeCombineMask(disp,win,ShapeBounding,0,0,m,ShapeSet);
	     XClearWindow(disp,win);
	     XMapWindow(disp,win);
	     XSync(disp,False);
	  }
	gettimeofday(&timev, NULL);
	t2=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
	total=t2-t1;
	printf("\tpixels scaled per second this run:   %8i\n",(int)((double)k/total));
     }
   printf("----- Testing Contrast Code -----\n");
   gettimeofday(&timev, NULL);
   t1=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
   k=0;
   for(n=0;n<512;n+=8)
     {
	mod.contrast=n;
	mod.brightness=256;
	mod.gamma=256;
	k+=(w*h);
	Imlib_set_image_modifier(id,im,&mod);
	Imlib_render(id,im,w,h);
	Imlib_free_pixmap(id,p);
	p=Imlib_move_image(id,im);
	m=Imlib_move_mask(id,im);
	XSetWindowBackgroundPixmap(disp,win,p);
	if (m) XShapeCombineMask(disp,win,ShapeBounding,0,0,m,ShapeSet);
	XClearWindow(disp,win);
	XMapWindow(disp,win);
	XSync(disp,False);
     }
   gettimeofday(&timev, NULL);
   t2=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
   total=t2-t1;
   printf("\tpixels rendered per second this run: %8i\n",(int)((double)k/total));
   printf("----- Testing Brightness Code -----\n");
   gettimeofday(&timev, NULL);
   t1=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
   k=0;
   for(n=0;n<512;n+=8)
     {
	mod.contrast=256;
	mod.brightness=n;
	mod.gamma=256;
	k+=(w*h);
	Imlib_set_image_modifier(id,im,&mod);
	Imlib_render(id,im,w,h);
	Imlib_free_pixmap(id,p);
	p=Imlib_move_image(id,im);
	m=Imlib_move_mask(id,im);
	XSetWindowBackgroundPixmap(disp,win,p);
	if (m) XShapeCombineMask(disp,win,ShapeBounding,0,0,m,ShapeSet);
	XClearWindow(disp,win);
	XMapWindow(disp,win);
	XSync(disp,False);
     }
   gettimeofday(&timev, NULL);
   t2=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
   total=t2-t1;
   printf("\tpixels rendered per second this run: %8i\n",(int)((double)k/total));
   printf("----- Testing Gamma Code -----\n");
   gettimeofday(&timev, NULL);
   t1=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
   k=0;
   for(n=0;n<512;n+=8)
     {
	mod.contrast=256;
	mod.brightness=256;
	mod.gamma=n;
	k+=(w*h);
	Imlib_set_image_modifier(id,im,&mod);
	Imlib_render(id,im,w,h);
	Imlib_free_pixmap(id,p);
	p=Imlib_move_image(id,im);
	m=Imlib_move_mask(id,im);
	XSetWindowBackgroundPixmap(disp,win,p);
	if (m) XShapeCombineMask(disp,win,ShapeBounding,0,0,m,ShapeSet);
	XClearWindow(disp,win);
	XMapWindow(disp,win);
	XSync(disp,False);
     }
   gettimeofday(&timev, NULL);
   t2=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
   total=t2-t1;
   printf("\tpixels rendered per second this run: %8i\n",(int)((double)k/total));
   return 0;
}
