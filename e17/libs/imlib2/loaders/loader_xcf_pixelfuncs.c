
/* These are the tons of different functions for merging layers.
   All of them assume merging of src2 ONTO src1. Heavily adapted
   from gimp's paint_funcs.c.
                                                   --cK.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "common.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <netinet/in.h>
#include "image.h"
#include "Imlib2.h"

#include "colormod.h"
#include "blend.h"

#define STD_BUF_SIZE       1021
#define MAXDIFF            195076
#define HASH_TABLE_SIZE    1021
#define RANDOM_TABLE_SIZE  4096
#define RANDOM_SEED        314159265
#define EPSILON            0.0001

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


void
combine_pixels (DATA8* src, int src_w, int src_h, DATA8* dest, int dest_w, int dest_h, int dest_x, int dest_y)
{
  int x, y, s_idx, d_idx, dest_col;
  int src_tl_x = 0, src_tl_y = 0;
  int src_br_x = src_w, src_br_y = src_h;

  int a, red, green, blue;

  int alpha, b;
  unsigned char src_alpha;
  unsigned char new_alpha;
  float ratio, compl_ratio;
  long tmp;
  int length;

  /*printf ("Blending %ix%i onto %ix%i at %i, %i -->", src_w, src_h, dest_w, dest_h, dest_x, dest_y);*/

  /* translate negative destinations */
  if (dest_x + src_br_x >= dest_w)
    src_br_x -= (dest_x + src_br_x) - dest_w; 

  if (dest_y + src_br_y >= dest_h)
    src_br_y -= (dest_y + src_br_y) - dest_h; 

  if (dest_x < 0)
    {
      src_tl_x = -dest_x;
      dest_x = 0;
    }
  if (dest_y < 0)
    {
      src_tl_y = -dest_y;
      dest_y = 0;
    }

  for (y = src_tl_y; y < src_br_y; y++)
    {
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
}
