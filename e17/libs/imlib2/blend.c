#include "common.h"
#include "blend.h"

void
BlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
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
#if 1		     
	       r = 255 - a;
	       *p2 = 
		  ((((*p1 & 0x00ff00ff) * a) >> 8) & 0x00ff00ff) + 
		  ((((*p1 >> 8) & 0x00ff00ff) * a) & 0xff00ff00) +
		  ((((*p2 & 0x00ff00ff) * r) >> 8) & 0x00ff00ff) + 
		  ((((*p2 >> 8) & 0x00ff00ff) * r) & 0xff00ff00);
#else		     
	      r =  (*p1      ) & 0xff;
	      g =  (*p1 >> 8 ) & 0xff;
	      b =  (*p1 >> 16) & 0xff;
	      
	      rr = (*p2      ) & 0xff;
	      gg = (*p2 >> 8 ) & 0xff;
	      bb = (*p2 >> 16) & 0xff;
	      
	      tmp = (r - rr) * a;
	      nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
	      tmp = (g - gg) * a;
	      ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
	      tmp = (b - bb) * a;
	      nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
	      *p2 = ((nb & 0xff) << 16) | ((ng & 0xff) << 8) | (nr & 0xff);
#endif	       
	    }
	  else
	    *p2 = *p1;	  
	  p1++;
	  p2++;
	}
    }
}
