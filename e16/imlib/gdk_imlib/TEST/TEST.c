#include "gdk_imlib.h"
#include <gdk/gdk.h>
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
   GdkWindowAttr         attr;
   GdkWindow             *win;
   GdkPixmap             *p;
   GdkPixmap             *m;
   GdkImlibImage         *im;
   GdkImlibColorModifier mod;
   int w,h,i,j,k,n,o;
   double t1,t2,total;
   struct timeval timev;
   
   gdk_init(&argc,&argv);
   gdk_imlib_init();
   if (argc<=1) 
     {
	printf("Usage:\n TEST image_file\n");
	exit(1);
     }
   im=gdk_imlib_load_image(argv[1]);

   w=im->rgb_width;h=im->rgb_height;
   attr.window_type=GDK_WINDOW_TOPLEVEL;attr.wclass=GDK_INPUT_OUTPUT;
   attr.event_mask=GDK_STRUCTURE_MASK;attr.width=w;attr.height=h;
   attr.visual=gdk_imlib_get_visual();attr.colormap=gdk_imlib_get_colormap();
   win=gdk_window_new(NULL,&attr,GDK_WA_VISUAL|GDK_WA_COLORMAP);
   
   gdk_imlib_render(im,w,h);
   p=gdk_imlib_move_image(im);
   m=gdk_imlib_move_mask(im);
   gdk_window_set_back_pixmap(win,p,0);
   if (m) gdk_window_shape_combine_mask(win,m,0,0);
   gdk_window_clear(win);
   gdk_window_show(win);
   gdk_flush();
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
	     gdk_imlib_render(im,i,j);
	     gdk_imlib_free_pixmap(p);
	     p=gdk_imlib_move_image(im);
	     m=gdk_imlib_move_mask(im);
	     gdk_window_set_back_pixmap(win,p,0);
	     if (m) gdk_window_shape_combine_mask(win,m,0,0);
	     gdk_window_clear(win);
	     gdk_flush();
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
	gdk_imlib_set_image_modifier(im,&mod);
	gdk_imlib_render(im,w,h);
	gdk_imlib_free_pixmap(p);
	p=gdk_imlib_move_image(im);
	m=gdk_imlib_move_mask(im);
	gdk_window_set_back_pixmap(win,p,0);
	if (m) gdk_window_shape_combine_mask(win,m,0,0);
	gdk_window_clear(win);
	gdk_flush();
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
	gdk_imlib_set_image_modifier(im,&mod);
	gdk_imlib_render(im,w,h);
	gdk_imlib_free_pixmap(p);
	p=gdk_imlib_move_image(im);
	m=gdk_imlib_move_mask(im);
	gdk_window_set_back_pixmap(win,p,0);
	if (m) gdk_window_shape_combine_mask(win,m,0,0);
	gdk_window_clear(win);
	gdk_flush();
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
	gdk_imlib_set_image_modifier(im,&mod);
	gdk_imlib_render(im,w,h);
	gdk_imlib_free_pixmap(p);
	p=gdk_imlib_move_image(im);
	m=gdk_imlib_move_mask(im);
	gdk_window_set_back_pixmap(win,p,0);
	if (m) gdk_window_shape_combine_mask(win,m,0,0);
	gdk_window_clear(win);
	gdk_flush();
     }
   gettimeofday(&timev, NULL);
   t2=(double)timev.tv_sec+(((double)timev.tv_usec)/1000000);
   total=t2-t1;
   printf("\tpixels rendered per second this run: %8i\n",(int)((double)k/total));
   return 0;
}
