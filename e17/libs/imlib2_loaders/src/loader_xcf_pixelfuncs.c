#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <netinet/in.h>
#include "Imlib2.h"
#include "common.h"
#include "image.h"
#include "color_values.h"

#define EPSILON            0.0001
#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)
#define INT_MULT(a,b,t)  ((t) = (a) * (b) + 0x80, ((((t) >> 8) + (t)) >> 8))
#define LINEAR(x,y,w) ((w*y + x)*4)

#define alphify(src_alpha,new_alpha) \
        b = 3;                                                                                  \
	if (new_alpha != 0)		       				                        \
	  {											\
	      ratio = (float) src_alpha / new_alpha;						\
	      compl_ratio = 1.0 - ratio;							\
	  											\
              do { b--; \
	        dest[d_idx + b] =        								\
	          (unsigned char) (src[s_idx + b] * ratio + dest[d_idx + b] * compl_ratio + EPSILON);   \
         	  } while (b); \
	  }


  /* translate negative destinations */
void clip(int * src_tl_x, int * src_tl_y,
	  int * src_br_x, int * src_br_y,
	  int * dest_x, int * dest_y,
	  int dest_w, int dest_h)
{
  if (*dest_x + *src_br_x >= dest_w) 
    { *src_br_x -= (*dest_x + *src_br_x) - dest_w; }  

  if (*dest_y + *src_br_y >= dest_h) 
    { *src_br_y -= (*dest_y + *src_br_y) - dest_h; }  

  if (*dest_x < 0) 
    { 
      *src_tl_x = -(*dest_x); 
      *dest_x = 0; 
    } 
  if (*dest_y < 0) 
    { 
      *src_tl_y = -(*dest_y); 
      *dest_y = 0; 
    }
}


void
combine_pixels_normal (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;

  int b;
  unsigned char src_alpha;
  unsigned char new_alpha;
  float ratio, compl_ratio;
  long tmp;
  
  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	src_alpha = A_VAL(src + s_idx);
	
	if (src_alpha != 0)
	  {
	    if (src_alpha == 255)
	      {
		new_alpha = src_alpha;
		alphify (src_alpha, new_alpha);
		A_VAL(dest + d_idx) = new_alpha;	      
	      }
	    else
	      {
		new_alpha = A_VAL(dest + d_idx) + INT_MULT((255 - A_VAL(dest + d_idx)), src_alpha, tmp);
		alphify (src_alpha, new_alpha);
		A_VAL(dest + d_idx) = new_alpha;
	      }
	  }
      }
}


void
combine_pixels_add (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;
  int tmp;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	tmp = R_VAL(dest + d_idx) + R_VAL(src + s_idx);
	R_VAL(dest + d_idx) = (tmp > 255 ? 255 : tmp);

	tmp = G_VAL(dest + d_idx) + G_VAL(src + s_idx);
	G_VAL(dest + d_idx) = (tmp > 255 ? 255 : tmp);

	tmp = B_VAL(dest + d_idx) + B_VAL(src + s_idx);
	B_VAL(dest + d_idx) = (tmp > 255 ? 255 : tmp);

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}


void
combine_pixels_sub (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;
  int tmp;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	tmp = R_VAL(dest + d_idx) - R_VAL(src + s_idx);
	R_VAL(dest + d_idx) = (tmp < 0 ? 0 : tmp);

	tmp = G_VAL(dest + d_idx) - G_VAL(src + s_idx);
	G_VAL(dest + d_idx) = (tmp < 0 ? 0 : tmp);

	tmp = B_VAL(dest + d_idx) - B_VAL(src + s_idx);
	B_VAL(dest + d_idx) = (tmp < 0 ? 0 : tmp);

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}


void
combine_pixels_diff (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;
  int tmp;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	tmp = R_VAL(dest + d_idx) - R_VAL(src + s_idx);
	R_VAL(dest + d_idx) = (tmp < 0 ? -tmp : tmp);

	tmp = G_VAL(dest + d_idx) - G_VAL(src + s_idx);
	G_VAL(dest + d_idx) = (tmp < 0 ? -tmp : tmp);

	tmp = B_VAL(dest + d_idx) - B_VAL(src + s_idx);
	B_VAL(dest + d_idx) = (tmp < 0 ? -tmp : tmp);

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}


void
combine_pixels_darken (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	R_VAL(dest + d_idx) = MIN(R_VAL(dest + d_idx), R_VAL(src + s_idx));
	G_VAL(dest + d_idx) = MIN(G_VAL(dest + d_idx), G_VAL(src + s_idx));
	B_VAL(dest + d_idx) = MIN(B_VAL(dest + d_idx), B_VAL(src + s_idx));

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}


void
combine_pixels_lighten (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	R_VAL(dest + d_idx) = MAX(R_VAL(dest + d_idx), R_VAL(src + s_idx));
	G_VAL(dest + d_idx) = MAX(G_VAL(dest + d_idx), G_VAL(src + s_idx));
	B_VAL(dest + d_idx) = MAX(B_VAL(dest + d_idx), B_VAL(src + s_idx));

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}


void
combine_pixels_mult (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	R_VAL(dest + d_idx) = (R_VAL(dest + d_idx) * R_VAL(src + s_idx)) >> 8;
	G_VAL(dest + d_idx) = (G_VAL(dest + d_idx) * G_VAL(src + s_idx)) >> 8;
	B_VAL(dest + d_idx) = (B_VAL(dest + d_idx) * B_VAL(src + s_idx)) >> 8;

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}


void
combine_pixels_div (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	R_VAL(dest + d_idx) = MIN(255, ((float)R_VAL(dest + d_idx) / (R_VAL(src + s_idx) + 1)) * 256);
	G_VAL(dest + d_idx) = MIN(255, ((float)G_VAL(dest + d_idx) / (G_VAL(src + s_idx) + 1)) * 256);
	B_VAL(dest + d_idx) = MIN(255, ((float)B_VAL(dest + d_idx) / (B_VAL(src + s_idx) + 1)) * 256);

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}


void
combine_pixels_screen (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	R_VAL(dest + d_idx) = 255 - (((255 - R_VAL(dest + d_idx)) * (255 - R_VAL(src + s_idx))) >> 8);
	G_VAL(dest + d_idx) = 255 - (((255 - G_VAL(dest + d_idx)) * (255 - G_VAL(src + s_idx))) >> 8);
	B_VAL(dest + d_idx) = 255 - (((255 - B_VAL(dest + d_idx)) * (255 - B_VAL(src + s_idx))) >> 8);

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}


void
combine_pixels_overlay (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;
  int tmp_screen, tmp_mult;

  clip(&src_tl_x, &src_tl_y, &src_br_x, &src_br_y, &dest_x, &dest_y, dest_w, dest_h);

  for (y = src_tl_y; y < src_br_y; y++)
    for (x = src_tl_x; x < src_br_x; x++)
      {
	d_idx = LINEAR((dest_x + x - src_tl_x), (dest_y + y - src_tl_y), dest_w);
	s_idx = LINEAR(x, y, src_w);
	
	tmp_screen = 255 - (((255 - R_VAL(dest + d_idx)) * (255 - R_VAL(src + s_idx))) >> 8);
	tmp_mult   = (R_VAL(dest + d_idx) * R_VAL(src + s_idx)) >> 8;
	R_VAL(dest + d_idx) = (R_VAL(dest + d_idx) * tmp_screen + (255 - R_VAL(dest + d_idx)) * tmp_mult) >> 8;

	tmp_screen = 255 - (((255 - G_VAL(dest + d_idx)) * (255 - G_VAL(src + s_idx))) >> 8);
	tmp_mult   = (G_VAL(dest + d_idx) * G_VAL(src + s_idx)) >> 8;
	G_VAL(dest + d_idx) = (G_VAL(dest + d_idx) * tmp_screen + (255 - G_VAL(dest + d_idx)) * tmp_mult) >> 8;

	tmp_screen = 255 - (((255 - B_VAL(dest + d_idx)) * (255 - B_VAL(src + s_idx))) >> 8);
	tmp_mult   = (B_VAL(dest + d_idx) * B_VAL(src + s_idx)) >> 8;
	B_VAL(dest + d_idx) = (B_VAL(dest + d_idx) * tmp_screen + (255 - B_VAL(dest + d_idx)) * tmp_mult) >> 8;

	A_VAL(dest + d_idx) = A_VAL(src + s_idx);
      }
}

