#include "common.h"
#include "blend.h"

void
__imlib_BlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h)
{
  int x, y;
  DATA32 *p1, *p2;
  
  for (y = 0; y < h; y++)
    {
      DATA8 a, nr, ng, nb, r, g, b, rr, gg, bb;
      int tmp;
      
      p1 = src + (y * (w + src_jump));
      p2 = dst + (y * (w + dst_jump));
      for (x = 0; x < w; x++)
	{
	  a =  (*p1 >> 24) & 0xff;
	  if (a < 255)
	    {
	       r = 255 - a;
	       *p2 = 
		  ((((*p1 & 0x00ff00ff) * a) >> 8) & 0x00ff00ff) + 
		  ((((*p1 >> 8) & 0x00ff00ff) * a) & 0xff00ff00) +
		  ((((*p2 & 0x00ff00ff) * r) >> 8) & 0x00ff00ff) + 
		  ((((*p2 >> 8) & 0x00ff00ff) * r) & 0xff00ff00);
	    }
	  else
	    *p2 = *p1;	  
	  p1++;
	  p2++;
	}
    }
}
