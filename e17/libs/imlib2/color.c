#include <X11/Xlib.h>
#include "common.h"
#include "color.h"

DATA8  _pal_type = 0;
DATA16 _max_colors = 256;

DATA8 *
__imlib_AllocColorTable(Display *d, Colormap cmap, DATA8 *type_return)
{
   DATA8 *color_lut = NULL;
   
   if ((_max_colors >= 256) && (color_lut = __imlib_AllocColors332(d, cmap)))
     {
	*type_return = _pal_type;
	return color_lut;
     }
   if ((_max_colors >= 128) && (color_lut = __imlib_AllocColors232(d, cmap)))
     {
	*type_return = _pal_type;
	return color_lut;
     }
   if ((_max_colors >= 64) && (color_lut = __imlib_AllocColors222(d, cmap)))
     {
	*type_return = _pal_type;
	return color_lut;
     }
   if ((_max_colors >= 32) && (color_lut = __imlib_AllocColors221(d, cmap)))
     {
	*type_return = _pal_type;
	return color_lut;
     }
   if ((_max_colors >= 16) && (color_lut = __imlib_AllocColors121(d, cmap)))
     {
	*type_return = _pal_type;
	return color_lut;
     }
   if ((_max_colors >= 8) && (color_lut = __imlib_AllocColors111(d, cmap)))
     {
	*type_return = _pal_type;
	return color_lut;
     }
   __imlib_AllocColors1(d, cmap);
   return color_lut;
}

DATA8 *
__imlib_AllocColors332(Display *d, Colormap cmap)
{
   int r, g, b, i = 0;
   DATA8 *color_lut;

   color_lut = malloc(256 * sizeof(DATA8));
   for (r = 0; r < 8; r++)
     {
	for (g = 0; g < 8; g++)
	  {
	     for (b = 0; b < 4; b++)
	       {
		  XColor xcl;
		  int val;
		  
		  val = (r << 5) | (r << 2) | (r >> 1);
		  xcl.red = (unsigned short)((val << 8) | (val));
		  val = (g << 5) | (g << 2) | (g >> 1);
		  xcl.green = (unsigned short)((val << 8) | (val));
		  val = (b << 6) | (b << 4) | (b << 2) | (b);
		  xcl.blue = (unsigned short)((val << 8) | (val));
		  if (!XAllocColor(d, cmap, &xcl))
		    {
		       unsigned long pixels[256];
		       int j;
		       
		       if (i > 0)
			 {
			    for(j = 0; j < i; j++)
			       pixels[j] = (unsigned long) color_lut[j];
			    XFreeColors(d, cmap, pixels, i, 0);
			 }
		       free(color_lut);
		       return NULL;
		    }
		  color_lut[i] = xcl.pixel;
		  i++;
	       }
	  }
     }
   _pal_type = 0;
   return color_lut;
}

DATA8 *
__imlib_AllocColors232(Display *d, Colormap cmap)
{
   int r, g, b, i = 0;
   DATA8 *color_lut;

   color_lut = malloc(128 * sizeof(DATA8));   
   for (r = 0; r < 4; r++)
     {
	for (g = 0; g < 8; g++)
	  {
	     for (b = 0; b < 4; b++)
	       {
		  XColor xcl;
		  int val;
		  
		  val = (r << 6) | (r << 4) | (r << 2) | (r);
		  xcl.red = (unsigned short)((val << 8) | (val));
		  val = (g << 5) | (g << 2) | (g >> 1);
		  xcl.green = (unsigned short)((val << 8) | (val));
		  val = (b << 6) | (b << 4) | (b << 2) | (b);
		  xcl.blue = (unsigned short)((val << 8) | (val));
		  if (!XAllocColor(d, cmap, &xcl))
		    {
		       unsigned long pixels[256];
		       int j;
		       
		       if (i > 0)
			 {
			    for(j = 0; j < i; j++)
			       pixels[j] = (unsigned long) color_lut[j];
			    XFreeColors(d, cmap, pixels, i, 0);
			 }
		       free(color_lut);
		       return NULL;
		    }
		  color_lut[i] = xcl.pixel;
		  i++;
	       }
	  }
     }
   _pal_type = 1;
   return color_lut;
}

DATA8 *
__imlib_AllocColors222(Display *d, Colormap cmap)
{
   int r, g, b, i = 0;
   DATA8 *color_lut;

   color_lut = malloc(64 * sizeof(DATA8));   
   for (r = 0; r < 4; r++)
     {
	for (g = 0; g < 4; g++)
	  {
	     for (b = 0; b < 4; b++)
	       {
		  XColor xcl;
		  int val;
		  
		  val = (r << 6) | (r << 4) | (r << 2) | (r);
		  xcl.red = (unsigned short)((val << 8) | (val));
		  val = (g << 6) | (g << 4) | (g << 2) | (g);
		  xcl.green = (unsigned short)((val << 8) | (val));
		  val = (b << 6) | (b << 4) | (b << 2) | (b);
		  xcl.blue = (unsigned short)((val << 8) | (val));
		  if (!XAllocColor(d, cmap, &xcl))
		    {
		       unsigned long pixels[256];
		       int j;
		       
		       if (i > 0)
			 {
			    for(j = 0; j < i; j++)
			       pixels[j] = (unsigned long) color_lut[j];
			    XFreeColors(d, cmap, pixels, i, 0);
			 }
		       free(color_lut);
		       return NULL;
		    }
		  color_lut[i] = xcl.pixel;
		  i++;
	       }
	  }
     }
   _pal_type = 2;
   return color_lut;
}

DATA8 *
__imlib_AllocColors221(Display *d, Colormap cmap)
{
   int r, g, b, i = 0;
   DATA8 *color_lut;

   color_lut = malloc(32 * sizeof(DATA8));   
   for (r = 0; r < 4; r++)
     {
	for (g = 0; g < 4; g++)
	  {
	     for (b = 0; b < 2; b++)
	       {
		  XColor xcl;
		  int val;
		  
		  val = (r << 6) | (r << 4) | (r << 2) | (r);
		  xcl.red = (unsigned short)((val << 8) | (val));
		  val = (g << 6) | (g << 4) | (g << 2) | (g);
		  xcl.green = (unsigned short)((val << 8) | (val));
		  val = (b << 7) | (b << 6) | (b << 5) | (b << 4) | (b << 3) | (b << 2) | (b << 1) | (b);
		  xcl.blue = (unsigned short)((val << 8) | (val));
		  if (!XAllocColor(d, cmap, &xcl))
		    {
		       unsigned long pixels[256];
		       int j;
		       
		       if (i > 0)
			 {
			    for(j = 0; j < i; j++)
			       pixels[j] = (unsigned long) color_lut[j];
			    XFreeColors(d, cmap, pixels, i, 0);
			 }
		       free(color_lut);
		       return NULL;
		    }
		  color_lut[i] = xcl.pixel;
		  i++;
	       }
	  }
     }
   _pal_type = 3;
   return color_lut;
}

DATA8 *
__imlib_AllocColors121(Display *d, Colormap cmap)
{
   int r, g, b, i = 0;
   DATA8 *color_lut;

   color_lut = malloc(16 * sizeof(DATA8));   
   for (r = 0; r < 2; r++)
     {
	for (g = 0; g < 4; g++)
	  {
	     for (b = 0; b < 2; b++)
	       {
		  XColor xcl;
		  int val;
		  
		  val = (r << 7) | (r << 6) | (r << 5) | (r << 4) | (r << 3) | (r << 2) | (r << 1) | (r);
		  xcl.red = (unsigned short)((val << 8) | (val));
		  val = (g << 6) | (g << 4) | (g << 2) | (g);
		  xcl.green = (unsigned short)((val << 8) | (val));
		  val = (b << 7) | (b << 6) | (b << 5) | (b << 4) | (b << 3) | (b << 2) | (b << 1) | (b);
		  xcl.blue = (unsigned short)((val << 8) | (val));
		  if (!XAllocColor(d, cmap, &xcl))
		    {
		       unsigned long pixels[256];
		       int j;
		       
		       if (i > 0)
			 {
			    for(j = 0; j < i; j++)
			       pixels[j] = (unsigned long) color_lut[j];
			    XFreeColors(d, cmap, pixels, i, 0);
			 }
		       free(color_lut);
		       return NULL;
		    }
		  color_lut[i] = xcl.pixel;
		  i++;
	       }
	  }
     }
   _pal_type = 4;
   return color_lut;
}

DATA8 *
__imlib_AllocColors111(Display *d, Colormap cmap)
{
   int r, g, b, i = 0;
   DATA8 *color_lut;

   color_lut = malloc(8 * sizeof(DATA8));   
   for (r = 0; r < 2; r++)
     {
	for (g = 0; g < 2; g++)
	  {
	     for (b = 0; b < 2; b++)
	       {
		  XColor xcl;
		  int val;
		  
		  val = (r << 7) | (r << 6) | (r << 5) | (r << 4) | (r << 3) | (r << 2) | (r << 1) | (r);
		  xcl.red = (unsigned short)((val << 8) | (val));
		  val = (g << 7) | (g << 6) | (g << 5) | (g << 4) | (g << 3) | (g << 2) | (g << 1) | (g);
		  xcl.green = (unsigned short)((val << 8) | (val));
		  val = (b << 7) | (b << 6) | (b << 5) | (b << 4) | (b << 3) | (b << 2) | (b << 1) | (b);
		  xcl.blue = (unsigned short)((val << 8) | (val));
		  if (!XAllocColor(d, cmap, &xcl))
		    {
		       unsigned long pixels[256];
		       int j;
		       
		       if (i > 0)
			 {
			    for(j = 0; j < i; j++)
			       pixels[j] = (unsigned long) color_lut[j];
			    XFreeColors(d, cmap, pixels, i, 0);
			 }
		       free(color_lut);
		       return NULL;
		    }
		  color_lut[i] = xcl.pixel;
		  i++;
	       }
	  }
     }
   _pal_type = 5;
   return color_lut;
}

DATA8 *
__imlib_AllocColors1(Display *d, Colormap cmap)
{
   XColor xcl;
   DATA8 *color_lut;

   color_lut = malloc(2 * sizeof(DATA8));   
   xcl.red = (unsigned short)(0x0000);
   xcl.green = (unsigned short)(0x0000);
   xcl.blue = (unsigned short)(0x0000);
   XAllocColor(d, cmap, &xcl);
   color_lut[0] = xcl.pixel;
   xcl.red = (unsigned short)(0xffff);
   xcl.green = (unsigned short)(0xffff);
   xcl.blue = (unsigned short)(0xffff);
   XAllocColor(d, cmap, &xcl);
   color_lut[1] = xcl.pixel;
   _pal_type = 6;
   return color_lut;
}
