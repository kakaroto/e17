#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "common.h"
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "image.h"

char load (ImlibImage *im,
	   void (*progress)(ImlibImage *im, char percent,
			    int update_x, int update_y,
			    int update_w, int update_h),
	   char progress_granularity, char immediate_load);
char save (ImlibImage *im,
	   void (*progress)(ImlibImage *im, char percent,
			    int update_x, int update_y,
			    int update_w, int update_h),
	   char progress_granularity);
void formats (ImlibLoader *l);

char 
load (ImlibImage *im,
      void (*progress)(ImlibImage *im, char percent, 
		       int update_x, int update_y, 
		       int update_w, int update_h),
      char progress_granularity, char immediate_load)
{
   char                 p = ' ';
   int                  w = 0, h = 0, v = 255;
   DATA8                *data = NULL;
   FILE                *f = NULL;
   
   if (im->data)
      return 0;
   f = fopen(im->file, "rb");
   if (!f)
      return 0;

     {
	char buf[256], buf2[256];
	/* read the header info */
	
	if (fgets(buf, 255, f))
	  {
	     if ((buf[0] == 'P') && (isspace(buf[2])))
	       {
		  int    i, j;
		  char   comment = 1;
		  
		  p = buf[1];
		  /* skip blanks */
		  while (comment)
		    {
		       comment = 0;
		       if (!fgets(buf, 244, f))
			 {
			    fclose(f);
			    return 0;
			 }
		       if (buf[0] == '#')
			  comment = 1;
		    }
		  
		  /* get the width */
		  i = 0;
		  j = 0;
		  while ((buf[i]) && (!isspace(buf[i])))
		     buf2[j++] = buf[i++];
		  buf2[j] = 0;
		  w = atoi(buf2);
		  
		  while ((buf[i]) && (isspace(buf[i])))
		     i++;
		  if (!buf[i])
		    {
		       i = 0;
		       if (!fgets(buf, 244, f))
			 {
			    fclose(f);
			    return 0;
			 }
		    }

		  /* get the height */
		  j = 0;
		  while ((buf[i]) && (!isspace(buf[i])))
		     buf2[j++] = buf[i++];
		  buf2[j] = 0;
		  h = atoi(buf2);
		  
		  /* if its a color or grayscale get the max value */
		  if (!((p == '4') || (p == '1')))
		    {
		       while ((buf[i]) && (isspace(buf[i])))
			  i++;
		       if (!buf[i])
			 {
			    i = 0;
			    if (!fgets(buf, 244, f))
			      {
				 fclose(f);
				 return 0;
			      }
			 }
		       i = 0;
		       j = 0;
		       while ((buf[i]) && (!isspace(buf[i])))
			  buf2[j++] = buf[i++];
		       buf2[j] = 0;
		       v = atoi(buf2);
		    }
	       }
	     else
	       {
		  fclose(f);
		  return 0;
	       }
	  }
	else
	  {
	     fclose(f);
	     return 0;
	  }
	im->w = w;
	im->h = h;
	if (!im->format)
	  {
	     if (p == '8')
		SET_FLAG(im->flags, F_HAS_ALPHA);
	     else
		UNSET_FLAG(im->flags, F_HAS_ALPHA);
	     im->format = strdup("pnm");
	  }
     }
   
   if (((!im->data) && (im->loader)) || (immediate_load) || (progress))
     {
	DATA8  *ptr;
	DATA32 *ptr2;
	int     i, j, x, y, pl = 0;
	char    pper = 0;
	
	/* must set the im->data member before callign progress function */
	ptr2 = im->data = malloc(w * h * sizeof(DATA32));
	if (!im->data)
	  {
	     fclose(f);
	     return 0;
	  }
	/* start reading the data */
	switch (p)
	  {
	  case '1': /* */
	     fclose(f);
	     return 0;
	     break;
	  case '2': /* */
	     fclose(f);
	     return 0;
	     break;
	  case '3': /* */
	     fclose(f);
	     return 0;
	     break;
	  case '4': /* binary 1bit monochrome */
	     data = malloc(1 * sizeof(DATA8));
	     if (!data)
	       {
		  fclose(f);
		  return 0;
	       }
	     ptr2 = im->data;
	     j = 0;
	     while ((fread(data, 1, 1, f)) && (j < (w * h)))
	       {
		  for (i = 7; i >= 0; i--)
		    {
		       if (j < (w * h))
			 {
			    if (data[0] & (1 << i))
			       *ptr2 = 0xff000000;
			    else
			       *ptr2 = 0xffffffff;
			    ptr2++;
			 }
		       j++;
		    }
	       }
	     break;
	  case '5': /* binary 8bit grayscale GGGGGGGG */
	     data = malloc(1 * sizeof(DATA8) * w);
	     if (!data)
	       {
		  fclose(f);
		  return 0;
	       }
	     ptr2 = im->data;
	     for (y = 0; y < h; y++)
	       {
		  if (!fread(data, w * 1, 1, f))
		    {
		       free(data);
		       fclose(f);
		       return 1;
		    }
		  ptr = data;
		  if (v == 255)
		    {
		       for (x = 0; x < w; x++)
			 {
			    *ptr2 = 0xff000000 | 
			       (ptr[0] << 16) | 
			       (ptr[0] << 8) |  
			       ptr[0];
			    ptr2++;
			    ptr ++;
			 }
		    }
		  else
		    {
		       for (x = 0; x < w; x++)
			 {
			    *ptr2 = 0xff000000 | 
			       (((ptr[0] * 255) / v) << 16) |
			       (((ptr[0] * 255) / v) << 8) |
			       ((ptr[0] * 255) / v);
			    ptr2++;
			    ptr++;
			 }
		    }
		  if (progress)
		    {
		       char per;
		       int l;
		       
		       per = (char)((100 * y) / im->h);
		       if ((per - pper) >= progress_granularity)
			 {
			    l = y - pl;
			    progress(im, per, 0, (y - l), im->w, l);
			    pper = per;
			    pl = y;
			 }
		    }
	       }	     
	     break;
	  case '6': /* 24bit binary RGBRGBRGB */
	     data = malloc(3 * sizeof(DATA8) * w);
	     if (!data)
	       {
		  fclose(f);
		  return 0;
	       }
	     ptr2 = im->data;
	     for (y = 0; y < h; y++)
	       {
		  if (!fread(data, w * 3, 1, f))
		    {
		       free(data);
		       fclose(f);
		       return 1;
		    }
		  ptr = data;
		  if (v == 255)
		    {
		       for (x = 0; x < w; x++)
			 {
			    *ptr2 = 0xff000000 | 
			       (ptr[0] << 16) | 
			       (ptr[1] << 8) |  
			       ptr[2];
			    ptr2++;
			    ptr += 3;
			 }
		    }
		  else
		    {
		       for (x = 0; x < w; x++)
			 {
			    *ptr2 = 0xff000000 | 
			       (((ptr[0] * 255) / v) << 16) |
			       (((ptr[1] * 255) / v) << 8) |
			       ((ptr[2] * 255) / v);
			    ptr2++;
			    ptr += 3;
			 }		       
		    }
		  if (progress)
		    {
		       char per;
		       int l;
		       
		       per = (char)((100 * y) / im->h);
		       if ((per - pper) >= progress_granularity)
			 {
			    l = y - pl;
			    progress(im, per, 0, (y - l), im->w, l);
			    pper = per;
			    pl = y;
			 }
		    }
	       }
	     break;
	  case '7': /* XV's 8bit 332 format */
	     data = malloc(1 * sizeof(DATA8) * w);
	     if (!data)
	       {
		  fclose(f);
		  return 0;
	       }
	     ptr2 = im->data;
	     for (y = 0; y < h; y++)
	       {
		  if (!fread(data, w * 1, 1, f))
		    {
		       free(data);
		       fclose(f);
		       return 1;
		    }
		  ptr = data;
		  for (x = 0; x < w; x++)
		    {
		       int r, g, b;
		       
		       r = (*ptr >> 5) & 0x7;
		       g = (*ptr >> 2) & 0x7;
		       b = (*ptr     ) & 0x3;
		       *ptr2 = 0xff000000 | 
			  (((r << 21) | (r << 18) | (r << 15)) & 0xff0000) |
			  (((g << 13) | (g << 10) | (g << 7)) & 0xff00) |
			  ((b << 6) | (b << 4) | (b << 2) | (b << 0));
		       ptr2++;
		       ptr++;
		    }
		  if (progress)
		    {
		       char per;
		       int l;
		       
		       per = (char)((100 * y) / im->h);
		       if ((per - pper) >= progress_granularity)
			 {
			    l = y - pl;
			    progress(im, per, 0, (y - l), im->w, l);
			    pper = per;
			    pl = y;
			 }
		    }
	       }	     
	     break;
	  case '8': /* 24bit binary RGBARGBARGBA */
	     data = malloc(4 * sizeof(DATA8) * w);
	     if (!data)
	       {
		  fclose(f);
		  return 0;
	       }
	     ptr2 = im->data;
	     for (y = 0; y < h; y++)
	       {
		  if (!fread(data, w * 4, 1, f))
		    {
		       free(data);
		       fclose(f);
		       return 1;
		    }
		  ptr = data;
		  if (v == 255)
		    {
		       for (x = 0; x < w; x++)
			 {
			    *ptr2 = (ptr[3] << 24) |  
			       (ptr[0] << 16) | 
			       (ptr[1] << 8) |  
			       ptr[2];
			    ptr2++;
			    ptr += 4;
			 }
		    }
		  else
		    {
		       for (x = 0; x < w; x++)
			 {
			    *ptr2 = (((ptr[3] * 255) / v) << 24) |
			       (((ptr[0] * 255) / v) << 16) |
			       (((ptr[1] * 255) / v) << 8) |
			       ((ptr[2] * 255) / v);
			    ptr2++;
			    ptr += 4;
			 }		       
		    }
		  if (progress)
		    {
		       char per;
		       int l;
		       
		       per = (char)((100 * y) / im->h);
		       if ((per - pper) >= progress_granularity)
			 {
			    l = y - pl;
			    progress(im, per, 0, (y - l), im->w, l);
			    pper = per;
			    pl = y;
			 }
		    }
	       }
	     break;
	  default:
	     fclose(f);
	     return 0;
	     break;
	  }
	if (data)
	   free(data);
     }
   fclose(f);
   return 1;
}

char 
save (ImlibImage *im,
      void (*progress)(ImlibImage *im, char percent, 
		       int update_x, int update_y, 
		       int update_w, int update_h),
      char progress_granularity)
{
   FILE               *f;
   DATA8              *buf, *bptr;
   DATA32             *ptr;
   int                 x, y, pl = 0;
   char                pper = 0;
   
   /* no image data? abort */
   if (!im->data)
      return 0;
   f = fopen(im->file, "wb");
   if (!f)
      return 0;
   /* if the image has a useful alpha channel */
   if (im->flags & F_HAS_ALPHA)
     {
	/* allocate a small buffer to convert image data */
	buf = malloc(im->w * 4 * sizeof(DATA8));
	if (!buf)
	  {
	     fclose(f);
	     return 0;
	  }
	ptr = im->data;
	fprintf(f, 
		"P8\n"
		"# PNM File written by Imlib2\n"
		"%i %i\n"
		"255\n", im->w, im->h);
	for (y = 0; y < im->h; y++)
	  {
	     bptr = buf;
	     for (x = 0; x < im->w; x++)
	       {
		  bptr[0] = ((*ptr) >> 16) & 0xff;
		  bptr[1] = ((*ptr) >> 8 ) & 0xff;
		  bptr[2] = ((*ptr)      ) & 0xff;
		  bptr[3] = ((*ptr) >> 24) & 0xff;
		  bptr += 4;
		  ptr++;
	       }
	     fwrite(buf, im->w * 4, 1, f);
	     if (progress)
	       {
		  char per;
		  int l;
		  
		  per = (char)((100 * y) / im->h);
		  if ((per - pper) >= progress_granularity)
		    {
		       l = y - pl;
		       progress(im, per, 0, (y - l), im->w, l);
		       pper = per;
		       pl = y;
		    }
	       }
	  }
     }
   else
     {
	/* allocate a small buffer to convert image data */
	buf = malloc(im->w * 3 * sizeof(DATA8));
	if (!buf)
	  {
	     fclose(f);
	     return 0;
	  }
	ptr = im->data;
	fprintf(f, 
		"P6\n"
		"# PNM File written by Imlib2\n"
		"%i %i\n"
		"255\n", im->w, im->h);
	for (y = 0; y < im->h; y++)
	  {
	     bptr = buf;
	     for (x = 0; x < im->w; x++)
	       {
		  bptr[0] = ((*ptr) >> 16) & 0xff;
		  bptr[1] = ((*ptr) >> 8 ) & 0xff;
		  bptr[2] = ((*ptr)      ) & 0xff;
		  bptr += 3;
		  ptr++;
	       }
	     fwrite(buf, im->w * 3, 1, f);
	     if (progress)
	       {
		  char per;
		  int l;
		  
		  per = (char)((100 * y) / im->h);
		  if ((per - pper) >= progress_granularity)
		    {
		       l = y - pl;
		       progress(im, per, 0, (y - l), im->w, l);
		       pper = per;
		       pl = y;
		    }
	       }
	  }
     }
   /* finish off */
   free(buf);
   fclose(f);
   return 1;
   progress = NULL;
}

void 
formats (ImlibLoader *l)
{  
   char *list_formats[] = 
     { "pnm", "ppm", "pgm", "pbm", "pam" };

     {
	int i;
	
	l->num_formats = (sizeof(list_formats) / sizeof (char *));
	l->formats = malloc(sizeof(char *) * l->num_formats);
	for (i = 0; i < l->num_formats; i++)
	   l->formats[i] = strdup(list_formats[i]);
     }
}

