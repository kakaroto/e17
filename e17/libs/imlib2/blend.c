#include "common.h"
#include "blend.h"

void
__imlib_BlendRGBAToData(DATA32 *src, int src_w, int src_h, DATA32 *dst, 
			int dst_w, int dst_h, int sx, int sy, int dx, int dy,
			int w, int h, char dalpha)
{
   if (sx < 0)
     {
	w += sx;
	dx -= sx;
	sx = 0;
     }
   if (sy < 0)
     {
	h += sy;
	dy -= sy;
	sy = 0;
     }
   if (dx < 0)
     {
	w += dx;
	sx -= dx;
	dx = 0;
     }
   if (dy < 0)
     {
	h += dy;
	sy -= dy;
	dy = 0;
     }
   if ((w <= 0) || (h <= 0))
      return;
   if ((sx + w) > src_w)
      w = src_w - sx;
   if ((sy + h) > src_h)
      h = src_h - sy;
   if ((dx + w) > dst_w)
      w = dst_w - dx;
   if ((dy + h) > dst_h)
      h = dst_h - dy;   
   if ((w <= 0) || (h <= 0))
      return;
   if (dalpha == 1)
      __imlib_BlendRGBAToRGBA(src + (sy * src_w) + sx, src_w - w, 
			      dst + (dy * dst_w) + dx, dst_w - w, w, h);
   else if (dalpha == 0)
      __imlib_BlendRGBAToRGB(src + (sy * src_w) + sx, src_w - w, 
			     dst + (dy * dst_w) + dx, dst_w - w, w, h);
   else if (dalpha == 2)
      __imlib_CopyRGBAToRGBA(src + (sy * src_w) + sx, src_w - w, 
			     dst + (dy * dst_w) + dx, dst_w - w, w, h);      
}			

void
__imlib_BlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
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
/* funny - i cant tell much of a speed diff between these 2 */
#if 0
/* this one over time leads to rounding errors :( */	  
		  r = 255 - a;
		  *p2 = 
		     ((((*p1 & 0x00ff00ff) * a) >> 8) & 0x00ff00ff) + 
		     ((((*p1 >> 8) & 0x00ff00ff) * a) & 0xff00ff00) +
		     ((((*p2 & 0x00ff00ff) * r) >> 8) & 0x00ff00ff) + 
		     ((((*p2 >> 8) & 0x00ff00ff) * r) & 0xff00ff00);
#else
/* this is more accurate - but slower ? doesnt seem to be :) */
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  
		  tmp = (r - rr) * a;
		  nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  tmp = (g - gg) * a;
		  ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  tmp = (b - bb) * a;
		  nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  *p2 = ((nr & 0xff) << 16) | ((ng & 0xff) << 8) | (nb & 0xff);
#endif
	       }
	     else
		*p2 = *p1;	  
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_BlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
	DATA8 a, nr, ng, nb, r, g, b, rr, gg, bb, aa, na;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
	     a =  (*p1 >> 24) & 0xff;
	     if (a < 255)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  aa = (*p2 >> 24) & 0xff;
		  
		  tmp = (r - rr) * a;
		  nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  tmp = (g - gg) * a;
		  ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  tmp = (b - bb) * a;
		  nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  tmp = a + aa;
		  na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = ((na & 0xff) << 24) | ((nr & 0xff) << 16) | 
		     ((ng & 0xff) << 8) | (nb & 0xff);
	       }
	     else
		*p2 = *p1;	  
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_CopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
	DATA8 a, nr, ng, nb, r, g, b, rr, gg, bb, aa, na;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	   *p2++ = *p1++;
     }
}
