#include "common.h"
#include <X11/Xlib.h>
#include "colormod.h"
#include "image.h"
#include "blend.h"
#include "scale.h"

void
__imlib_BlendRGBAToData(DATA32 *src, int src_w, int src_h, DATA32 *dst, 
			int dst_w, int dst_h, int sx, int sy, int dx, int dy,
			int w, int h, char dalpha, ImlibColorModifier *cm, 
			ImlibOp op)
{
   void (*blender)(DATA32*, int, DATA32*, int, int, int) = NULL;
   void (*blender_cm)(DATA32*, int, DATA32*, int, int, int, ImlibColorModifier *) = NULL;


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
   if (cm)
     {
	switch(op)
	  {
	  case OP_COPY:
	     if (dalpha == 0)
		blender_cm = __imlib_BlendRGBAToRGBCmod;
	     else if (dalpha == 1)
		blender_cm = __imlib_BlendRGBAToRGBACmod;
	     else if (dalpha == 2)
		blender_cm = __imlib_CopyRGBAToRGBCmod;
	     else if (dalpha == 3)
		blender_cm = __imlib_CopyRGBAToRGBACmod;
	     break;
	  case OP_ADD:
	     if (dalpha == 0)
		blender_cm = __imlib_AddBlendRGBAToRGBCmod;
	     else if (dalpha == 1)
		blender_cm = __imlib_AddBlendRGBAToRGBACmod;
	     else if (dalpha == 2)
		blender_cm = __imlib_AddCopyRGBAToRGBCmod;
	     else if (dalpha == 3)
		blender_cm = __imlib_AddCopyRGBAToRGBACmod;
	     break;
	  case OP_SUBTRACT:
	     if (dalpha == 0)
		blender_cm = __imlib_SubBlendRGBAToRGBCmod;
	     else if (dalpha == 1)
		blender_cm = __imlib_SubBlendRGBAToRGBACmod;
	     else if (dalpha == 2)
		blender_cm = __imlib_SubCopyRGBAToRGBCmod;
	     else if (dalpha == 3)
		blender_cm = __imlib_SubCopyRGBAToRGBACmod;
	     break;
	  case OP_RESHADE:
	     if (dalpha == 0)
		blender_cm = __imlib_ReBlendRGBAToRGBCmod;
	     else if (dalpha == 1)
		blender_cm = __imlib_ReBlendRGBAToRGBACmod;
	     else if (dalpha == 2)
		blender_cm = __imlib_ReCopyRGBAToRGBCmod;
	     else if (dalpha == 3)
		blender_cm = __imlib_ReCopyRGBAToRGBACmod;
	     break;
	  default:
	     break;
	  }
	  if (blender_cm)
	     blender_cm(src + (sy * src_w) + sx, src_w - w, 
			dst + (dy * dst_w) + dx, dst_w - w, w, h, cm);
     }
   else
     {
	switch(op)
	  {
	  case OP_COPY:
	     if (dalpha == 0)
		blender = __imlib_BlendRGBAToRGB;
	     else if (dalpha == 1)
		blender = __imlib_BlendRGBAToRGBA;
	     else if (dalpha == 2)
		blender = __imlib_CopyRGBAToRGB;
	     else if (dalpha == 3)
		blender = __imlib_CopyRGBAToRGBA;
	     break;
	  case OP_ADD:
	     if (dalpha == 0)
		blender = __imlib_AddBlendRGBAToRGB;
	     else if (dalpha == 1)
		blender = __imlib_AddBlendRGBAToRGBA;
	     else if (dalpha == 2)
		blender = __imlib_AddCopyRGBAToRGB;
	     else if (dalpha == 3)
		blender = __imlib_AddCopyRGBAToRGBA;
	     break;
	  case OP_SUBTRACT:
	     if (dalpha == 0)
		blender = __imlib_SubBlendRGBAToRGB;
	     else if (dalpha == 1)
		blender = __imlib_SubBlendRGBAToRGBA;
	     else if (dalpha == 2)
		blender = __imlib_SubCopyRGBAToRGB;
	     else if (dalpha == 3)
		blender = __imlib_SubCopyRGBAToRGBA;
	     break;
	  case OP_RESHADE:
	     if (dalpha == 0)
		blender = __imlib_ReBlendRGBAToRGB;
	     else if (dalpha == 1)
		blender = __imlib_ReBlendRGBAToRGBA;
	     else if (dalpha == 2)
		blender = __imlib_ReCopyRGBAToRGB;
	     else if (dalpha == 3)
		blender = __imlib_ReCopyRGBAToRGBA;
	     break;
	  default:
	     break;
	  }
	  if (blender)
	     blender(src + (sy * src_w) + sx, src_w - w, 
		     dst + (dy * dst_w) + dx, dst_w - w, w, h);
     }
}			

/* COPY OPS */

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
	     if (a == 255)
		*p2 = *p1;	  
	     else if (a > 0)
	       {
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
		  *p2 = (nr << 16) | (ng << 8) | nb;
	       }
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
	     if (a == 255)
		*p2 = *p1;	  
	     else if (a > 0)
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
		  *p2 = (na << 24) | (nr << 16) | 
		     (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_CopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
	     *p2 = (*p2 & 0xff000000) | (*p1 & 0x00ffffff);	     
	     p2++;
	     p1++;
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
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	   *p2++ = *p1++;
     }
}

/* ADD OPS */

void
__imlib_AddBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  
		  tmp = rr + ((r * a) >> 8);
		  nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = gg + ((g * a) >> 8);
		  ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = bb + ((b * a) >> 8);
		  nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = (nr << 16) | (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_AddBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  aa = (*p2 >> 24) & 0xff;
		  
		  tmp = rr + ((r * a) >> 8);
		  nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = gg + ((g * a) >> 8);
		  ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = bb + ((b * a) >> 8);
		  nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = a + aa;
		  na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = (na << 24) | (nr << 16) | 
		     (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_AddCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr + r;
	     nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     tmp = gg + g;
	     ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     tmp = bb + b;
	     nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     *p2 = (*p2 & 0xff000000) | (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_AddCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr + r;
	     nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     tmp = gg + g;
	     ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     tmp = bb + b;
	     nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     *p2 = (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}

/* SUBTRACT OPS */

void
__imlib_SubBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  
		  tmp = rr - ((r * a) >> 8);
		  nr = tmp & (~(tmp >> 8));
		  tmp = gg - ((g * a) >> 8);
		  ng = tmp & (~(tmp >> 8));
		  tmp = bb - ((b * a) >> 8);
		  nb = tmp & (~(tmp >> 8));
		  *p2 = (nr << 16) | (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_SubBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  aa = (*p2 >> 24) & 0xff;
		  
		  tmp = rr - ((r * a) >> 8);
		  nr = tmp & (~(tmp >> 8));
		  tmp = gg - ((g * a) >> 8);
		  ng = tmp & (~(tmp >> 8));
		  tmp = bb - ((b * a) >> 8);
		  nb = tmp & (~(tmp >> 8));
		  tmp = a + aa;
		  na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = (na << 24) | (nr << 16) | 
		     (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_SubCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr - r;
	     nr = tmp & (~(tmp >> 8));
	     tmp = gg - g;
	     ng = tmp & (~(tmp >> 8));
	     tmp = bb - b;
	     nb = tmp & (~(tmp >> 8));
	     *p2 = (*p2 & 0xff000000) | (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_SubCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr - r;
	     nr = tmp & (~(tmp >> 8));
	     tmp = gg - g;
	     ng = tmp & (~(tmp >> 8));
	     tmp = bb - b;
	     nb = tmp & (~(tmp >> 8));
	     *p2 = (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}


/* RESHADE OPS */

void
__imlib_ReBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  
		  tmp = rr + (((r - 127) * a) >> 7);
		  nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = gg + (((g - 127) * a) >> 7);
		  ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = bb + (((b - 127) * a) >> 7);
		  nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  *p2 = (nr << 16) | (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_ReBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  aa = (*p2 >> 24) & 0xff;
		  
		  tmp = rr + (((r - 127) * a) >> 7);
		  nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = gg + (((g - 127) * a) >> 7);
		  ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = bb + (((b - 127) * a) >> 7);
		  nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = a + aa;
		  na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = (na << 24) | (nr << 16) | 
		     (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_ReCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr + ((r - 127) << 1);
	     nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     tmp = gg + ((g - 127) << 1);
	     ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     tmp = bb + ((b - 127) << 1);
	     nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     *p2 = (*p2 & 0xff000000) | (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_ReCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr + ((r - 127) << 1);
	     nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     tmp = gg + ((g - 127) << 1);
	     ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     tmp = bb + ((b - 127) << 1);
	     nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     *p2 = (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}


















/* COLORMOD COPY OPS */

void
__imlib_BlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
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
	     if (a == 255)
		*p2 = *p1;	  
	     else if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  CMOD_APPLY_RGB(cm, r, g, b);
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  
		  tmp = (r - rr) * a;
		  nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  tmp = (g - gg) * a;
		  ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  tmp = (b - bb) * a;
		  nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
		  *p2 = (nr << 16) | (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_BlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
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
	     if (a == 255)
		*p2 = *p1;	  
	     else if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  CMOD_APPLY_RGB(cm, r, g, b);
		  
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
		  *p2 = (na << 24) | (nr << 16) | 
		     (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_CopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   int x, y;
   DATA32 *p1, *p2;
   DATA8   r, g, b, a;
   
   for (y = 0; y < h; y++)
     {
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
	     b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     a =  (*p2 >> 24) & 0xff;
	     
	     CMOD_APPLY_RGB(cm, r, g, b);
	     
	     *p2 =  (a << 24) | (r << 16) | (g << 8) | b;
	     p2++;
	     p1++;
	  }
     }
}

void
__imlib_CopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   int x, y;
   DATA32 *p1, *p2;
   DATA8   r, g, b, a;
   
   for (y = 0; y < h; y++)
     {
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
	     b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     a =  (*p1 >> 24) & 0xff;
	     
	     CMOD_APPLY_RGBA(cm, r, g, b, a);
	     
	     *p2 =  (a << 24) | (r << 16) | (g << 8) | b;
	     p2++;
	     p1++;
	  }
     }
}

/* COLORMOD ADD OPS */

void
__imlib_AddBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  CMOD_APPLY_RGBA(cm, r, g, b, a);
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  
		  tmp = rr + ((r * a) >> 8);
		  nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = gg + ((g * a) >> 8);
		  ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = bb + ((b * a) >> 8);
		  nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = (nr << 16) | (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_AddBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  CMOD_APPLY_RGBA(cm, r, g, b, a);
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  aa = (*p2 >> 24) & 0xff;
		  
		  tmp = rr + ((r * a) >> 8);
		  nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = gg + ((g * a) >> 8);
		  ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = bb + ((b * a) >> 8);
		  nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  tmp = a + aa;
		  na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = (na << 24) | (nr << 16) | 
		     (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_AddCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     CMOD_APPLY_RGB(cm, r, g, b);
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr + r;
	     nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     tmp = gg + g;
	     ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     tmp = bb + b;
	     nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     *p2 = (*p2 & 0xff000000) | (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_AddCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     CMOD_APPLY_RGB(cm, r, g, b);
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr + r;
	     nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     tmp = gg + g;
	     ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     tmp = bb + b;
	     nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
	     *p2 = (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}

/* COLORMOD SUBTRACT OPS */

void
__imlib_SubBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  CMOD_APPLY_RGBA(cm, r, g, b, a);
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  
		  tmp = rr - ((r * a) >> 8);
		  nr = tmp & (~(tmp >> 8));
		  tmp = gg - ((g * a) >> 8);
		  ng = tmp & (~(tmp >> 8));
		  tmp = bb - ((b * a) >> 8);
		  nb = tmp & (~(tmp >> 8));
		  *p2 = (nr << 16) | (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_SubBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  CMOD_APPLY_RGBA(cm, r, g, b, a);
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  aa = (*p2 >> 24) & 0xff;
		  
		  tmp = rr - ((r * a) >> 8);
		  nr = tmp & (~(tmp >> 8));
		  tmp = gg - ((g * a) >> 8);
		  ng = tmp & (~(tmp >> 8));
		  tmp = bb - ((b * a) >> 8);
		  nb = tmp & (~(tmp >> 8));
		  tmp = a + aa;
		  na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = (na << 24) | (nr << 16) | 
		     (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_SubCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     	
	     CMOD_APPLY_RGB(cm, r, g, b);
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr - r;
	     nr = tmp & (~(tmp >> 8));
	     tmp = gg - g;
	     ng = tmp & (~(tmp >> 8));
	     tmp = bb - b;
	     nb = tmp & (~(tmp >> 8));
	     *p2 = (*p2 & 0xff000000) | (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_SubCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     CMOD_APPLY_RGB(cm, r, g, b);
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr - r;
	     nr = tmp & (~(tmp >> 8));
	     tmp = gg - g;
	     ng = tmp & (~(tmp >> 8));
	     tmp = bb - b;
	     nb = tmp & (~(tmp >> 8));
	     *p2 = (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}


/* COLORMOD RESHADE OPS */

void
__imlib_ReBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  CMOD_APPLY_RGBA(cm, r, g, b, a);
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  
		  tmp = rr + (((r - 127) * a) >> 7);
		  nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = gg + (((g - 127) * a) >> 7);
		  ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = bb + (((b - 127) * a) >> 7);
		  nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  *p2 = (nr << 16) | (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_ReBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
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
             if (a > 0)
	       {
		  b =  (*p1      ) & 0xff;
		  g =  (*p1 >> 8 ) & 0xff;
		  r =  (*p1 >> 16) & 0xff;
		  
		  CMOD_APPLY_RGBA(cm, r, g, b, a);
		  
		  bb = (*p2      ) & 0xff;
		  gg = (*p2 >> 8 ) & 0xff;
		  rr = (*p2 >> 16) & 0xff;
		  aa = (*p2 >> 24) & 0xff;
		  
		  tmp = rr + (((r - 127) * a) >> 7);
		  nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = gg + (((g - 127) * a) >> 7);
		  ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = bb + (((b - 127) * a) >> 7);
		  nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
		  tmp = a + aa;
		  na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));
		  *p2 = (na << 24) | (nr << 16) | 
		     (ng << 8) | nb;
	       }
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_ReCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     
	     CMOD_APPLY_RGB(cm, r, g, b);

	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr + ((r - 127) << 1);
	     nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     tmp = gg + ((g - 127) << 1);
	     ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     tmp = bb + ((b - 127) << 1);
	     nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     *p2 = (*p2 & 0xff000000) | (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_ReCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   int x, y;
   DATA32 *p1, *p2;
   
   for (y = 0; y < h; y++)
     {
        DATA8 nr, ng, nb, r, g, b, rr, gg, bb;
	int tmp;
	
	p1 = src + (y * (w + src_jump));
	p2 = dst + (y * (w + dst_jump));
	for (x = 0; x < w; x++)
	  {
             b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;

	     CMOD_APPLY_RGB(cm, r, g, b);
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     
	     tmp = rr + ((r - 127) << 1);
	     nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     tmp = gg + ((g - 127) << 1);
	     ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     tmp = bb + ((b - 127) << 1);
	     nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));
	     *p2 = (nr << 16) | (ng << 8) | nb;
	     p1++;
	     p2++;
	  }
     }
}
























#define LINESIZE 16
#define CLIP(x, y, w, h, xx, yy, ww, hh) \
if (x < xx) {w += x; x = xx;} \
if (y < yy) {h += y; y = yy;} \
if ((x + w) > ww) {w = ww - x;} \
if ((y + h) > hh) {h = hh - y;}

void
__imlib_BlendImageToImage(ImlibImage *im_src, ImlibImage *im_dst,
			  char aa, char blend, char merge_alpha, 
			  int ssx, int ssy, int ssw, int ssh,
			  int ddx, int ddy, int ddw, int ddh, 
			   ImlibColorModifier *cm, ImlibOp op)
{
   if ((!(im_src->data)) && (im_src->loader) && (im_src->loader->load))
      im_src->loader->load(im_src, NULL, 0, 1);
   if ((!(im_dst->data)) && (im_dst->loader) && (im_src->loader->load))
      im_dst->loader->load(im_dst, NULL, 0, 1);
   if (!im_src->data)
      return;
   if (!im_dst->data)
      return;
   
   if ((ssw == ddw) &&
       (ssh == ddh))
     {
	if (!IMAGE_HAS_ALPHA(im_src))
	   blend = 0;
	if (!IMAGE_HAS_ALPHA(im_dst))
	   merge_alpha = 0;
	/* 0 - blend RGB - leave dst A as-is */
	/* 1 - blend RGB - add src A to dst A */
	/* 2 - copy RGB - leave dst A as-is */
	/* 3 - copy RGB - set dst A to src A */
	if (merge_alpha)
	  {
	     if (blend)
		__imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
					im_dst->data, im_dst->w, im_dst->h,
					ssx, ssy,
					ddx, ddy,
					ssw, ssh, 1, cm, op);
	     else
		__imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
					im_dst->data, im_dst->w, im_dst->h,
					ssx, ssy,
					ddx, ddy,
					ssw, ssh, 3, cm, op);
	  }
	else
	  {
	     if (blend)
		__imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
					im_dst->data, im_dst->w, im_dst->h,
					ssx, ssy,
					ddx, ddy,
					ssw, ssh, 0, cm, op);
	     else
		__imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
					im_dst->data, im_dst->w, im_dst->h,
					ssx, ssy,
					ddx, ddy,
					ssw, ssh, 2, cm, op);
	  }
     }
   else
     {
	DATA32  **ypoints = NULL;
	int      *xpoints = NULL;
	int      *yapoints = NULL;
	int      *xapoints = NULL;
	DATA32   *buf = NULL;
	int       sx, sy, sw, sh, dx, dy, dw, dh, dxx, dyy, scw, sch, y2, x2;
	int       psx, psy, psw, psh;
	char      xup = 0, yup = 0;
	int       y, h, hh;
	sx = ssx;
	sy = ssy;
	sw = ssw;
	sh = ssh;
	dx = ddx;
	dy = ddy;
	dw = ddw;
	dh = ddh;
	/* dont do anything if we have a 0 widht or height image to render */
	/* if the input rect size < 0 dont render either */
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	   return;
	/* clip the source rect to be within the actual image */
	psx = sx;
	psy = sy;
	psw = sw;
	psh = sh;
	CLIP(sx, sy, sw, sh, 0, 0, im_src->w, im_src->h);
	/* clip output coords to clipped input coords */
	if (psx != sx)
	   dx += ((sx - psx) * ddw) / ssw;
	if (psy != sy)
	   dy += ((sy - psy) * ddh) / ssh;
	if (psw != sw)
	   dw = (dw * sw) / psw;
	if (psh != sh)
	   dh = (dh * sh) / psh;
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	   return;
	psx = dx;
	psy = dy;
	psw = dw;
	psh = dh;
	x2 = sx;
	y2 = sy;
	CLIP(dx, dy, dw, dh, 0, 0, im_dst->w, im_dst->h);
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	   return;
	if (psx != dx)
	   sx += ((dx - psx) * ssw) / ddw;
	if (psy != dy)
	   sy += ((dy - psy) * ssh) / ddh;
	if (psw != dw)
	   sw = (sw * dw) / psw;
	if (psh != dh)
	   sh = (sh * dh) / psh;
	dxx = dx - psx;
	dyy = dy - psy;
	dxx += (x2 * ddw) / ssw;
	dyy += (y2 * ddh) / ssh;
	
	/* do a second check to see if we now have invalid coords */
	/* dont do anything if we have a 0 widht or height image to render */
	/* if the input rect size < 0 dont render either */
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	   return;
	/* calculate the scaling factors of width and height for a whole image */
	scw = (ddw * im_src->w) / ssw;
	sch = (ddh * im_src->h) / ssh;
	/* if we are scaling the image at all make a scaling buffer */
	if (!((sw == dw) && (sh == dh)))
	  {
	     /* need to calculate ypoitns and xpoints array */
	     ypoints = __imlib_CalcYPoints(im_src->data, im_src->w, im_src->h,
					   sch, im_src->border.top,
					   im_src->border.bottom);
	     if (!ypoints)
		return;
	     xpoints = __imlib_CalcXPoints(im_src->w, scw,
					   im_src->border.left,
					   im_src->border.right);
	     if (!xpoints)
	       {
		  free(ypoints);
		  return;
	       }
	     /* calculate aliasing counts */
	     if (aa)
	       {
		  yapoints = __imlib_CalcApoints(im_src->h, sch,
						 im_src->border.top,
						 im_src->border.bottom);
		  if (!yapoints)
		    {
		       free(ypoints);
		       free(xpoints);
		       return;
		    }
		  xapoints = __imlib_CalcApoints(im_src->w, scw,
						 im_src->border.left,
						 im_src->border.right);
		  if (!xapoints)
		    {
		       free(yapoints);
		       free(ypoints);
		       free(xpoints);
		       return;
		    }
	       }
	  }
	/* if we are scaling the image at all make a scaling buffer */
	/* allocate a buffer to render scaled RGBA data into */
	buf = malloc(dw * LINESIZE * sizeof(DATA32));
	if (!buf)
	  {
	     if (aa)
	       {
		  free(xapoints);
		  free(yapoints);
	       }
	     free(ypoints);
	     free(xpoints);
	     return;
	  }
	/* setup h */
	h = dh;
	/* set our scaling up in x / y dir flags */
	if (dw > sw)
	   xup = 1;
	if (dh > sh)
	   yup = 1;
	if (!IMAGE_HAS_ALPHA(im_src))
	   blend = 0;
	if (!IMAGE_HAS_ALPHA(im_dst))
	   merge_alpha = 0;
	/* scale in LINESIZE Y chunks and convert to depth*/
	for (y = 0; y < dh; y += LINESIZE)
	  {
	     hh = LINESIZE;
	     if (h < LINESIZE)
		hh = h;
	     /* scale the imagedata for this LINESIZE lines chunk of image */
	     if (aa)
	       {
		  if (IMAGE_HAS_ALPHA(im_src))
		     __imlib_ScaleAARGBA(ypoints, xpoints, buf, xapoints,
					 yapoints, xup, yup, dxx, dyy + y,
					 0, 0, dw, hh, dw, im_src->w);
		  else
		     __imlib_ScaleAARGB(ypoints, xpoints, buf, xapoints,
					yapoints, xup, yup, dxx, dyy + y,
					0, 0, dw, hh, dw, im_src->w);
	       }
	     else
		__imlib_ScaleSampleRGBA(ypoints, xpoints, buf, dxx, dyy + y,
					0, 0, dw, hh, dw);
	     /* 0 - blend RGB - leave dst A as-is */
	     /* 1 - blend RGB - add src A to dst A */
	     /* 2 - copy RGB - leave dst A as-is */
	     /* 3 - copy RGB - set dst A to src A */
	     if (merge_alpha)
	       {
		  if (blend)
		     __imlib_BlendRGBAToData(buf, dw, hh,
					     im_dst->data, im_dst->w,
					     im_dst->h,
					     0, 0, dx, dy + y, dw, dh, 1, 
					     cm, op);
		  else
		     __imlib_BlendRGBAToData(buf, dw, hh,
					     im_dst->data, im_dst->w,
					     im_dst->h,
					     0, 0, dx, dy + y, dw, dh, 3,
					     cm, op);
	       }
	     else
	       {
		  if (blend)
		     __imlib_BlendRGBAToData(buf, dw, hh,
					     im_dst->data, im_dst->w,
					     im_dst->h,
					     0, 0, dx, dy + y, dw, dh, 0,
					     cm, op);
		  else
		     __imlib_BlendRGBAToData(buf, dw, hh,
					     im_dst->data, im_dst->w,
					     im_dst->h,
					     0, 0, dx, dy + y, dw, dh, 2,
					     cm, op);
	       }
	     h -= LINESIZE;
	  }
	/* free up our buffers and poit tables */
	if (buf)
	  {
	     free(buf);
	     free(ypoints);
	     free(xpoints);
	  }
	if (aa)
	  {
	     free(yapoints);
	     free(xapoints);
	  }
     }
}
