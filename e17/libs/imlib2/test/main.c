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
#include <math.h>
/*
#include <sys/time.h>
#include "common.h"
#include "image.h"
#include "rend.h"
#include "rgba.h"
#include "ximage.h"
#include "color.h"
 */
#include "Imlib2.h"

Display *disp;
Window win;
Visual *vis;
Colormap cm;
int depth;

void progress(Imlib_Image *im, char percent, int update_x, int update_y,
	      int update_w, int update_h);

void
progress(Imlib_Image *im, char percent,
	 int update_x, int update_y,
	 int update_w, int update_h)
{
   imlib_context_set_display(disp);
   imlib_context_set_visual(vis);
   imlib_context_set_colormap(cm);
   imlib_context_set_drawable(win);
   imlib_context_set_dither(0);
   imlib_context_set_blend(0);
   imlib_context_set_color_modifier(NULL);
   imlib_context_set_operation(IMLIB_OP_COPY);
   imlib_context_set_image(im);
   imlib_render_image_part_on_drawable_at_size(update_x, update_y, 
					       update_w, update_h,
					       update_x, update_y,
					       update_w, update_h);
}

int main (int argc, char **argv)
{
   int i, j;
   Imlib_Image *im = NULL;
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
   int filter = 0;
   int rotate = 0;
   int rottest = 0;
   int scaleup = 0;
   int scaleboth = 0;
   int origone = 0;
   Imlib_Color_Modifier colormod = 0;
   
   /**
    * Parse all the command line arguments
    */
   if ((argc > 1) && (!strcmp(argv[1], "-help")))
     {
	printf ("Imlib2 program test. (Imlib v2.0.0.4)\n");
	printf ("usage: imlib2 [options] [file]\n");
	printf ("options are:\n");
	printf ("-help\t\tDisplays this help.\n");
	printf ("-root\t\tDraw in the root window.\n");
	printf ("-smooth\t\tWhen scaling images scale with anti-aliasing.\n");
	printf ("-up\t\tWhen doing scal test scale up, not down.\n");
	printf ("-both\t\tScale horizontally AND vertically in scale test.\n");
	printf ("-orig\t\tKeep original width and height in each pass of scale test.\n");
	printf ("-blend\t\tBlending test.\n");
	printf ("-dither\t\tTurn dithering on for depths < 24bpp\n");
	printf ("-colormod <r> <g> <b> <a>\t\tSet up color mod tables\n");
	printf ("-scale\t\tScale test.\n");
	printf ("-noloop\t\tDont loop - timing test.\n");
	printf ("-rotate\t\tAlso rotate background image with mouse in interative test.\n");
	printf ("-size <w> <h>\t\tScale from w x h down in scaling test.\n"); // require parameters w / h
	printf ("-maxcolors <n>\t\tLimit color allocation count to n colors.\n"); // require parameter nb colors
	printf ("-text\t\tDisplays the text following this option. Need a loaded font.\n");
	printf ("-font\t\tLoads a font. The parameter must follow the police_name/size format. Example: loading the grunge font at size 18 is : grunge/18.");
	printf ("The following options requires a file to work properly.\n");
	printf ("-blast\t\tDisplays the file.\n");
	printf ("-loop\t\tScales down the image.\n");
	printf ("-blendtest\tPerforms a blending test on the file.\n");
	printf ("-rotatetest\tPerforms a rotate test on the file.\n");
	printf ("-filter\t\tPerforms filtering. Possible filters are,\n\t\t\t1:Blur filter, 2:Sharpen filter, 3:Color blur filter, \n\t\t\t4:Emboss filter, 5:Grayscale filter, 6:Saturation filter,\n\t\t\t7:Edge detection filter.\n");
	return 0;
     }
   
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-root"))
	   root = 1;
	else if (!strcmp(argv[i], "-smooth"))
	   aa = 1;
	else if (!strcmp(argv[i], "-blast"))
	   interactive = 0;
	else if (!strcmp(argv[i], "-loop"))
	  {
	     interactive = 0;
	     loop = 1;
	  }
	else if (!strcmp(argv[i], "-up"))
	   scaleup = 1;
	else if (!strcmp(argv[i], "-both"))
	   scaleboth = 1;
	else if (!strcmp(argv[i], "-orig"))
	   origone = 1;
	else if (!strcmp(argv[i], "-blend"))
	   blend = 1;
	else if (!strcmp(argv[i], "-blendtest"))
	  {
	     blendtest = 1;
	     interactive = 0;
	  }
	else if (!strcmp(argv[i], "-colormod"))
	  {
	     DATA8 rt[256], gt[256], bt[256], at[256];
	     double rm, gm, bm, am;
	     int j;
	     /*\ Setup color mod tables \*/
	     if (!colormod) colormod = imlib_create_color_modifier();
	     imlib_context_set_color_modifier(colormod);
	     rm = strtod(argv[++i], 0);
	     gm = strtod(argv[++i], 0);
	     bm = strtod(argv[++i], 0);
	     am = strtod(argv[++i], 0);
	     imlib_get_color_modifier_tables(rt, gt, bt, at);
	     for (j = 0x100; --j >= 0; ) {
		rt[j] = ((double)rt[j]) * rm;
		gt[j] = ((double)gt[j]) * gm;
		bt[j] = ((double)bt[j]) * bm;
		at[j] = ((double)at[j]) * am;
	     }
	     imlib_set_color_modifier_tables(rt, gt, bt, at);
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
	else if (!strcmp(argv[i], "-rotate"))
	   rotate = 1;
	else if (!strcmp(argv[i], "-filter"))
	  {
	     filter = atoi(argv[++i]);
	     interactive = 0;
	  }
	else if (!strcmp(argv[i], "-rotatetest"))
	  {
	     rottest = 1;
	     interactive = 0;
	  }
	else
	   file = argv[i];
     }
   
   /**
    * Initialization according to options
    */
   printf("init\n");
   
   /**
    * First tests to determine which rendering task to perform
    */
   if (!blendtest)
     {
	disp = XOpenDisplay(NULL);
	vis = DefaultVisual(disp, DefaultScreen(disp));
	depth = DefaultDepth(disp, DefaultScreen(disp));    
	cm = DefaultColormap(disp, DefaultScreen(disp));
	/* nasty - using imlib internal function.. but it makes benchmarks fair */
	if (!interactive)
	   __imlib_SetMaxXImageCount(disp, 3);  
	if (root)
	   win = DefaultRootWindow(disp);
	else
	  {
	     win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 10, 10, 0, 0, 0);
	     XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask | 
			  ButtonMotionMask | PointerMotionMask | ExposureMask);
	  }
     }
   
   if (!interactive)
     {
	printf("load %s\n", file);
	im = imlib_load_image_immediately(file);
	if (!im)
	  {
	     printf("load failed\n");
	     exit(0);
	  }
	imlib_context_set_image(im);
	w = imlib_image_get_width();
	h = imlib_image_get_height();   
	printf("image %i x %i\n", w, h);
     }
   
   if (!blendtest)
     {
	if (!root)
	  {
	     if (scaleup)
		XResizeWindow(disp, win, w * 4, h * 4);
	     else if (scaleboth)
		XResizeWindow(disp, win, w * 2, h * 2);
	     else
		XResizeWindow(disp, win, w, h);
	     XMapWindow(disp, win);
	  }
	if (scale)
	  {
	     Window d;
	     int dd;
	     
	     XGetGeometry(disp, win, &d, &dd, &dd, &dd, &dd, &dd, &dd);
	  }
	XSync(disp, False);
     }
   
   
   /**
    * Start rendering
    */
   printf("rend\n");
   
   if (!blendtest)
     {
	imlib_context_set_display(disp);
	imlib_context_set_visual(vis);
	imlib_context_set_colormap(cm);
	imlib_context_set_drawable(win);
     }
   imlib_context_set_anti_alias(aa);
   imlib_context_set_dither(dith);
   imlib_context_set_blend(blend);
   imlib_context_set_color_modifier(NULL);
   imlib_context_set_operation(IMLIB_OP_COPY);
   imlib_context_set_image(im);
   
   gettimeofday(&timev,NULL);
   sec1=(int)timev.tv_sec; /* and stores it so we can time outselves */
   usec1=(int)timev.tv_usec; /* we will use this to vary speed of rot */
   
   if (loop)
     {
	printf("loop\n");
	
	// first test
	if (scaleup)
	  {
	     printf("scale up\n");
	     for (i = 0; i < w * 3; i+= 8)
	       {
		  if (!blendtest)
		    {
		       Imlib_Image im_tmp;
		       
		       im_tmp = imlib_create_cropped_scaled_image(0, 0, w, h, 
								  w + i, (((w + i) * h) / w));
		       if (im_tmp)
			 {
			    imlib_context_set_image(im_tmp);
			    imlib_render_image_on_drawable(0, 0);
			    imlib_free_image();
			 }
		       imlib_context_set_image(im);
		    }
		  else
		    {
		       Imlib_Image im_tmp;
		       im_tmp = imlib_create_cropped_scaled_image(0, 0, w, h, 
								  w + i, (((w + i) * h) / w));
		       if (im_tmp)
			 {
			    imlib_context_set_image(im_tmp);
			    imlib_free_image();
			 }
		       imlib_context_set_image(im);
		    }
		  pixels += (w + i) * (((w + i) * h) / w);
	       }
	  }
	
	// else if // second
	else if (scaleboth)
	  {
	     if (origone)
	       {
		  for (i = 0; i < w * 2; i+= 4)
		    {
		       if (!blendtest)
			 {
			    Imlib_Image im_tmp;
			    
			    im_tmp = imlib_create_cropped_scaled_image(0, 0, w, h, 
								       w, (((i) * h) / w));
			    if (im_tmp)
			      {
				 imlib_context_set_image(im_tmp);
				 imlib_render_image_on_drawable(0, 0);
				 imlib_free_image();
			      }
			    imlib_context_set_image(im);
			 }
		       else
			 {
			    Imlib_Image im_tmp;
			    im_tmp = imlib_create_cropped_scaled_image(0, 0, w, h, 
								       w, (((i) * h) / w));
			    if (im_tmp)
			      {
				 imlib_context_set_image(im_tmp);
				 imlib_free_image();
			      }
			    imlib_context_set_image(im);
			 }
		       XSync(disp, False);
		       pixels += (2 * w - i) * (((i) * h) / w);
		    }
		  for (i = 0; i < w * 2; i+= 4)
		    {
		       if (!blendtest)
			  imlib_render_image_on_drawable_at_size(0, 0,
								 2 * w - i, h);
		       else
			 {
			    Imlib_Image im_tmp;
			    im_tmp = imlib_create_cropped_scaled_image(0, 0, w, h, 
								       2 * w - i, h);
			    if (im_tmp)
			      {
				 imlib_context_set_image(im_tmp);
				 imlib_free_image();
			      }
			    imlib_context_set_image(im);
			 }
		       pixels += (2 * w - i) * h;
		    }
	       }
	     else
	       {
		  for (i = 0; i < w * 2; i+= 4)
		    {
		       if (!blendtest)
			  imlib_render_image_on_drawable_at_size(0, 0,
								 2 * w - i, (((i) * h) / w));
		       else
			 {
			    Imlib_Image im_tmp;
			    im_tmp = imlib_create_cropped_scaled_image(0, 0, w, h, 
								       2 * w - i, (((i) * h) / w));
			    if (im_tmp)
			      {
				 imlib_context_set_image(im_tmp);
				 imlib_free_image();
			      }
			    imlib_context_set_image(im);
			 }
		       pixels += w * (((i) * h) / w);
		    }
	       }
	  }
	else
	  {
	     printf("scale down 0 -> %i incriment by 1\n", w);
	     for (i = 0; i < w; i++)
	       {
		  if (!blendtest)
		     imlib_render_image_on_drawable_at_size(0, 0,
							    w - i, (((w - i) * h) / w));
		  else
		    {
		       Imlib_Image im_tmp;
		       im_tmp = imlib_create_cropped_scaled_image(0, 0, w, h, 
								  w - i, (((w - i) * h) / w));
		       if (im_tmp)
			 {
			    imlib_context_set_image(im_tmp);
			    imlib_free_image();
			 }
		       imlib_context_set_image(im);
		    }
		  pixels += (w + i) * (((w + i) * h) / w);
	       }
	  }
     }
   
   // last test
   /*	else if (scaleboth)
     {
	for (i = 0; i < w * 2; i+= 1)
	  {
	     if (!blendtest)
		imlib_render_image_on_drawable_at_size(0, 0,
						       2 * w - i, (((i) * h) / w));
	     else
	       {
		  Imlib_Image im_tmp;
		  im_tmp = imlib_create_cropped_scaled_image(0, 0, w, h, 
							     2 * w - i, (((i) * h) / w));
		  if (im_tmp)
		    {
		       imlib_context_set_image(im_tmp);
		       imlib_free_image();
		    }
		  imlib_context_set_image(im);
	       }
	     pixels += (2 * w - i) * (((i) * h) / w);
	  }
     }
    } */// end if loop
   else if (blendtest)
     {
	Imlib_Image im2;
	
	im2 = imlib_create_image(w, h);
	imlib_context_set_image(im2);
	w = imlib_image_get_width();
	h = imlib_image_get_height();   
	imlib_context_set_image(im2);
	imlib_context_set_color_modifier(colormod);
	for (i = 0; i < 256; i++)
	  {
             imlib_blend_image_onto_image(im, 0, 0, 0, w, h, 0, 0, w, h);
	     pixels += (w * h);	     
	  }
     }
   else if (rottest)
     {
	int w, h;
	double i;
	
	imlib_context_set_image(im);
	imlib_render_image_on_drawable(0, 0);
	
	w = imlib_image_get_width();
	h = imlib_image_get_height();
	printf("rotating inside %dx%d frame\n", w, h);
	
	imlib_context_set_blend(1);
	imlib_context_set_image(imlib_create_image(w, h));
	for (i = 0; i < 1; i += 0.01) {
	   imlib_blend_image_onto_image(im, 0, 0, 0, w, h, 0, 0, w, h);
	   imlib_context_set_color_modifier(colormod);
	   imlib_blend_image_onto_image_at_angle(im, 0, 0, 0, w, h,
						 0, h * i,
						 w * (1 - i), -(h * i));
	   imlib_context_set_color_modifier(NULL);
	   imlib_render_image_on_drawable(0, 0);
	   pixels += w * h;
	}
	for (i = 0; i < 1; i += 0.01) {
	   imlib_blend_image_onto_image(im, 0, 0, 0, w, h, 0, 0, w, h);
	   imlib_context_set_color_modifier(colormod);
	   imlib_context_set_operation(IMLIB_OP_ADD);
	   imlib_blend_image_onto_image_at_angle(im, 0, 0, 0, w, h,
						 w * i, h,
						 -(w * i), h * (i - 1));
	   imlib_context_set_operation(IMLIB_OP_COPY);
	   imlib_context_set_color_modifier(NULL);
	   imlib_render_image_on_drawable(0, 0);
	   pixels += w * h;
	}
	for (i = 0; i < 1; i += 0.01) {
	   imlib_blend_image_onto_image(im, 0, 0, 0, w, h, 0, 0, w, h);
	   imlib_context_set_color_modifier(colormod);
	   imlib_context_set_operation(IMLIB_OP_SUBTRACT);
	   imlib_blend_image_onto_image_at_angle(im, 0, 0, 0, w, h,
						 w, h * (1 - i),
						 w * (i - 1), h * i);
	   imlib_context_set_operation(IMLIB_OP_COPY);
	   imlib_context_set_color_modifier(NULL);
	   imlib_render_image_on_drawable(0, 0);
	   pixels += w * h;
	}
	for (i = 0; i < 1; i += 0.01) {
	   imlib_blend_image_onto_image(im, 0, 0, 0, w, h, 0, 0, w, h);
	   imlib_context_set_color_modifier(colormod);
	   imlib_context_set_operation(IMLIB_OP_RESHADE);
	   imlib_blend_image_onto_image_at_angle(im, 0, 0, 0, w, h,
						 w * (1 - i), 0,
						 w * i, h * (1 - i));
	   imlib_context_set_operation(IMLIB_OP_COPY);
	   imlib_context_set_color_modifier(NULL);
	   imlib_render_image_on_drawable(0, 0);
	   pixels += w * h;
	}
	imlib_free_image();
     }
   else if (filter)
     {
	imlib_context_set_filter(imlib_create_filter(0));
	switch (filter) {
	default:
	case 1:
	   /*\ Blur filter \*/
	   imlib_filter_set( 0,  0, 0, 8, 8, 8);
	   imlib_filter_set(-1,  0, 0, 4, 4, 4);
	   imlib_filter_set( 0, -1, 0, 4, 4, 4);
	   imlib_filter_set( 1,  0, 0, 4, 4, 4);
	   imlib_filter_set( 0,  1, 0, 4, 4, 4);
	   imlib_filter_set(-2,  0, 0, 1, 1, 1);
	   imlib_filter_set( 0, -2, 0, 1, 1, 1);
	   imlib_filter_set( 2,  0, 0, 1, 1, 1);
	   imlib_filter_set( 0,  2, 0, 1, 1, 1);
	   imlib_filter_set(-1, -1, 0, 1, 1, 1);
	   imlib_filter_set(-1,  1, 0, 1, 1, 1);
	   imlib_filter_set( 1, -1, 0, 1, 1, 1);
	   imlib_filter_set( 1,  1, 0, 1, 1, 1);
	   break;
	case 2:
	   /*\ Sharpen filter \*/
	   imlib_filter_set( 0,  0,  0,  5,  5,  5);
	   imlib_filter_set(-1,  0,  0, -1, -1, -1);
	   imlib_filter_set( 0, -1,  0, -1, -1, -1);
	   imlib_filter_set( 1,  0,  0, -1, -1, -1);
	   imlib_filter_set( 0,  1,  0, -1, -1, -1);
	   break;
	case 3:
	   /*\ Color blur filter \*/
	   imlib_filter_set( 0,  0,  0,  3,  3,  3);
	   imlib_filter_set(-1, -1,  0,  1,  0,  0);
	   imlib_filter_set( 1, -1,  0,  0,  1,  0);
	   imlib_filter_set( 0,  1,  0,  0,  0,  1);
	   break;
	case 4:
	   /*\ Emboss filter \*/
	   imlib_filter_set_red  (-1, -1, 0, -1, -1, -1);
	   imlib_filter_set_red  ( 0,  0, 0,  1,  1,  1);
	   imlib_filter_set_green(-1, -1, 0, -1, -1, -1);
	   imlib_filter_set_green( 0,  0, 0,  1,  1,  1);
	   imlib_filter_set_blue (-1, -1, 0, -1, -1, -1);
	   imlib_filter_set_blue ( 0,  0, 0,  1,  1,  1);
	   
	   imlib_filter_constants(0, 768, 768, 768);
	   imlib_filter_divisors (0,   6,   6,   6);
	   break;
	case 5:
	   /*\ Grayscale filter \*/
	   imlib_filter_set_red  (0, 0, 0, 80, 1, 1);
	   imlib_filter_set_green(0, 0, 0, 1, 80, 1);
	   imlib_filter_set_blue (0, 0, 0, 1, 1, 80);
	   break;
	case 6:
	   /*\ Saturation filter \*/
	   imlib_filter_set_red  (0, 0, 0, 80, -1, -1);
	   imlib_filter_set_green(0, 0, 0, -1, 80, -1);
	   imlib_filter_set_blue (0, 0, 0, -1, -1, 80);
	   break;
	case 7:
	   /*\ Edge detection filter \*/
	   imlib_filter_set(-1, -1, 0, -1, -1, -1);
	   imlib_filter_set(-1,  0, 0, -3, -3, -3);
	   imlib_filter_set(-1,  1, 0, -1, -1, -1);
	   imlib_filter_set( 0, -1, 0, -3, -3, -3);
	   imlib_filter_set( 0,  0, 0, 16, 16, 16);
	   imlib_filter_set( 0,  1, 0, -3, -3, -3);
	   imlib_filter_set( 1, -1, 0, -1, -1, -1);
	   imlib_filter_set( 1,  0, 0, -3, -3, -3);
	   imlib_filter_set( 1,  1, 0, -1, -1, -1);
	   imlib_filter_divisors (0,   3,   3,   3);
	}
	pixels = 0;
	imlib_render_image_on_drawable_at_size(0, 0, w, h);
	for (i = 0; i < w; i++)
	  {
	     imlib_image_filter();
	     imlib_render_image_on_drawable_at_size(0, 0, w, h);
	     pixels += w * h;
	  }
	imlib_free_filter();
     }
   else if (interactive)
     {
	int wo, ho, px, py, first = 1;
	Imlib_Image im_bg, im_sh1, im_sh2, im_sh3, im_ic[13], im_tmp;
	/* Imlib_Border border; */
	Imlib_Updates up = NULL;
	int x, y, i, j;
	XEvent ev;
	Imlib_Font fn=NULL;
	
	/* "ARIAL/30" "COMIC/30" "IMPACT/30" "Prole/30" "Proteron/30" */
	/* "TIMES/30" "badacid/30" "bajoran/30" "bigfish/30" */
	imlib_add_path_to_font_path("./ttfonts");
	if (fon)
	  {
	     fn = imlib_load_font(fon);
	     imlib_context_set_font(fn);
	     imlib_context_set_direction(IMLIB_TEXT_TO_RIGHT);
	     if (!fn) 
		fon = NULL;
	  }
	
	imlib_context_set_progress_function(NULL);
	imlib_context_set_progress_granularity(0);
	if (file)
	   im_bg = imlib_load_image(file);
	else
	   im_bg = imlib_load_image("test_images/bg.png");
	imlib_context_set_image(im_bg);
	im_tmp = imlib_clone_image();
	w = imlib_image_get_width();
	h = imlib_image_get_height();
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
	     
	     imlib_context_set_image(im);    
	     if (first)
	       {
		  imlib_blend_image_onto_image(im_bg, 0,
					       0, 0, w, h,
					       0, 0, w, h);
		  first = 0;
	       }
	     else if (rotate)
	       {
		  Imlib_Image rotim;
		  double s, c;
		  int x1, y1, x2, y2, w, h;
		  
		  w = imlib_image_get_width();
		  h = imlib_image_get_height();
		  s = sin(6.2831853 * (double)y / (double)h);
		  c = cos(6.2831853 * (double)y / (double)h);
		  
		  x1 = (w - w * c + h * s) / 2;
		  y1 = (h - h * c - w * s) / 2;
		  x2 = (w + w * c - h * s) / 2;
		  y2 = (h + h * c + w * s) / 2;
		  
		  imlib_context_set_blend(1);
		  imlib_blend_image_onto_image_at_angle(im_bg, 0,
							0, 0,
							imlib_image_get_width(),
							imlib_image_get_height(),
							x1, y1, x2, y2);
		  up = imlib_update_append_rect(up, 0, 0,
						imlib_image_get_width(),
						imlib_image_get_height());
		  
	       }
	     imlib_apply_filter("bump_map_point(x=[],y=[],map=test_images/bulb.png);", &x, &y, NULL);
	     up = imlib_update_append_rect(up, 0, 0,
					   imlib_image_get_width(),
					   imlib_image_get_height());
	     {
		  Imlib_Updates uu;
		  
		  imlib_context_set_color(255, 255, 255, 255);
		  uu = imlib_image_draw_line(200, 200, x, y, 1);
		  up = imlib_updates_append_updates(up, uu);
	       }
	       {
		  static Imlib_Color_Range rg = NULL;
		  
		  if (!rg)
		    {
		       rg = imlib_create_color_range();
		       imlib_context_set_color_range(rg);
		       imlib_context_set_color(255, 255, 255, 255);
		       imlib_add_color_to_color_range(0);
		       imlib_context_set_color(255, 255,160, 255);
		       imlib_add_color_to_color_range(1);
		       imlib_context_set_color(255, 160, 120, 255);
		       imlib_add_color_to_color_range(1);
		       imlib_context_set_color(255, 80, 100, 128);
		       imlib_add_color_to_color_range(1);
		       imlib_context_set_color(32, 48, 80, 0);
		       imlib_add_color_to_color_range(1);
		    }
		  imlib_context_set_operation(IMLIB_OP_RESHADE);
		  imlib_image_fill_color_range_rectangle(60, 60, 256, 256, (double)x);
		  up = imlib_update_append_rect(up, 60, 60, 256, 256);
		  imlib_context_set_operation(IMLIB_OP_COPY);
	       }
	     if (fon)
	       {
		  int retw, reth, ty, nx, ny, cx, cy, cw, ch, cp;
		  
		  if (!str)
		     str = "This is a test string";
		  ty = 50;
		  for (i = 0; i < 16; i++)
		    {
		       int al;
		       double an = (double)i / 10.0;
		       
		       imlib_context_set_direction(IMLIB_TEXT_TO_ANGLE);
		       imlib_context_set_angle(an);
		       
		       al = (15 - i) * 16;
		       if (al > 255)
			  al = 255;
		       imlib_context_set_color(255, 255, 255, al);
		       imlib_text_draw_with_return_metrics(50, ty, str,
							   &retw, &reth, 
							   &nx, &ny);
		       up = imlib_update_append_rect(up, 50, ty, retw, reth);
		       ty += ny;
		    }
		  imlib_context_set_direction(IMLIB_TEXT_TO_RIGHT);
		  cp = imlib_text_get_index_and_location(str, x - 50, y - 50,
							 &cx, &cy, &cw, &ch);
		  if (cp >= 0)
		     printf("over char %c\n", str[cp]);
	       }
	     imlib_context_set_blend(1);
	     if ((px != x) || (py != y))
	       {
		  for (j = 0; j < 32; j++)
		    {
		       for (i = 0; i < 32; i++)
			 {
			    int ic, iw, ih, ww, hh;
			    
			    ic = ((j * 32) + i) % 13;
			    imlib_context_set_image(im_ic[ic]);
			    iw = imlib_image_get_width();
			    ih = imlib_image_get_height();
			    ww = iw;
			    hh = ih;
			    up = imlib_update_append_rect(up, x + (i * iw * 2), 
							  y + (j * ih * 2), ww, hh);
			    up = imlib_update_append_rect(up, px + (i * iw * 2), 
							  py + (j * ih * 2), ww, hh);
			    imlib_context_set_image(im);
			    imlib_blend_image_onto_image(im_ic[ic], 0,
							 0, 0, iw, ih, 
							 x + (i * iw * 2), 
							 y + (j * ih * 2), 
							 ww, hh);
			 }
		    }
	       }
	     imlib_apply_filter( "tint(x=200,y=200,w=300,h=100,alpha=100,red=155,green=25,blue=25);"\
                                 "tint(green=20,red=20,blue=20,alpha=200,x=30,y=30);" \
                                 "tint(green=40,red=40,blue=240,alpha=60,x=50,y=150,h=200);",
				NULL );
	     
	     imlib_blend_image_onto_image(im_sh1, 0, 0, 0, 50, 50, 0, 0, 50, 50);
	     up = imlib_update_append_rect(up, 0, 0, 50, 50);
	     imlib_blend_image_onto_image(im_sh2, 0, 0, 0, 50, 50, 50, 0, w - 50, 50);
	     up = imlib_update_append_rect(up, 50, 0, w - 50, 50);
	     imlib_blend_image_onto_image(im_sh3, 0, 0, 0, 50, 50, 0, 50, 50, h - 50);
	     up = imlib_update_append_rect(up, 0, 50, 50, h - 50);
	     up = imlib_updates_merge_for_rendering(up, w, h);
	     imlib_context_set_blend(0);
	     imlib_render_image_updates_on_drawable(up, 0, 0);
	     if ((px != x) || (py != y))
               {
		  Imlib_Updates u;
		  
		  u = up;
		  while (u)
		    {
		       int ux, uy, uw, uh;
		       
		       imlib_updates_get_coordinates(u, &ux, &uy, &uw, &uh);
		       imlib_blend_image_onto_image(im_bg, 0,
						    ux, uy, uw, uh,
						    ux, uy, uw, uh);
		       u = imlib_updates_get_next(u);
		    }
	       }
	     imlib_updates_free(up);
	     up = NULL;
	     imlib_context_set_image(im_sh1);imlib_free_image();
	     imlib_context_set_image(im_sh1);imlib_free_image();
	     imlib_context_set_image(im_sh1);imlib_free_image();
	     imlib_context_set_image(im_ic[0]);imlib_free_image();
	     imlib_context_set_image(im_ic[1]);imlib_free_image();
	     imlib_context_set_image(im_ic[2]);imlib_free_image();
	     imlib_context_set_image(im_ic[3]);imlib_free_image();
	     
	  }
     }
   else
     {
	pixels = 0;
	imlib_context_set_color_modifier(colormod);
	for (i = 0; i < w; i++)
	  {
	     imlib_render_image_on_drawable_at_size(0, 0, w, h);
	     pixels += w * h;
	  }
     }
   
   /**
    * Determine horse power of your video card driver
    */
   gettimeofday(&timev,NULL);
   sec2=(int)timev.tv_sec; /* and stores it so we can time outselves */
   usec2=(int)timev.tv_usec; /* we will use this to vary speed of rot */
   printf("done\n");
     {
	double t1, t2;
	
	t1 = (double)sec1 + ((double)usec1 / 1000000);
	t2 = (double)sec2 + ((double)usec2 / 1000000);
	sec = t2 - t1;
     }
   printf("%3.3f sec, %3.3f M pixels (%i)\n", sec, (double)pixels / 1000000, pixels);
   printf("%3.3f Mpixels / sec\n", (double)(pixels) / (sec * 1000000));
   return 0;
}
