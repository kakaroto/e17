#include <X11/Xlib.h>
#include "common.h"
#include "scale.h"
#include "image.h"
#include "context.h"
#include "rgba.h"
#include "blend.h"
#include "rgbadraw.h"
#include "updates.h"

void
__imlib_FlipImageHoriz(ImlibImage *im)
{
   DATA32 *p1, *p2, tmp;
   int x, y;
   
   for (y = 0; y < im->h; y++)
     {
	p1 = im->data + (y * im->w);
	p2 = im->data + ((y  + 1) * im->w) - 1;
	for (x = 0; x < (im->w >> 1); x++)
	  {
	     tmp = *p1;
	     *p1 = *p2;
	     *p2 = tmp;
	     p1++;
	     p2--;
	  }
     }
}

void
__imlib_FlipImageVert(ImlibImage *im)
{
   DATA32 *p1, *p2, tmp;
   int x, y;
   
   for (y = 0; y < (im->h >> 1); y++)
     {
	p1 = im->data + (y * im->w);
	p2 = im->data + ((im->h - 1 - y) * im->w);
	for (x = 0; x < im->w; x++)
	  {
	     tmp = *p1;
	     *p1 = *p2;
	     *p2 = tmp;
	     p1++;
	     p2++;
	  }
     }
}

void
__imlib_FlipImageDiagonal(ImlibImage *im)
{
   DATA32 *p1, *p2, *data;
   int x, y, tmp;

   data = malloc(im->w * im->h * sizeof(DATA32));
   p1 = im->data;
   for (y = im->h - 1; y >= 0; y--)
     {
	p2 = data + y;
	for (x = 0; x < im->w; x++)
	  {
	     *p2 = *p1;
	     p2 += im->h;
	     p1++;
	  }
     }
   free(im->data);
   im->data = data;
   tmp = im->w;
   im->w = im->h;
   im->h = tmp;
   tmp = im->border.top;
   im->border.top = im->border.left;
   im->border.left = tmp;
   tmp = im->border.bottom;
   im->border.bottom = im->border.right;
   im->border.right = tmp;
}

void
__imlib_BlurImage(ImlibImage *im, int rad)
{
   DATA32 *p1, *p2, *data;
   int x, y, mx, my, mw, mh, mt, xx, yy;
   int a, r, g, b;
   int *as, *rs, *gs, *bs;
   
   if (rad < 1)
      return;
   data = malloc(im->w * im->h * sizeof(DATA32));
   as = malloc(sizeof(int) * im->w);
   rs = malloc(sizeof(int) * im->w);
   gs = malloc(sizeof(int) * im->w);
   bs = malloc(sizeof(int) * im->w);
   for (y = 0; y < im->h; y++)
     {
	my = y - rad;
	mh = (rad << 1) + 1;
	if (my < 0)
	  {
	     mh += my;
	     my = 0;
	  }
	if ((my + mh) > im->h)
	   mh = im->h - my;
	
	p1 = data + (y * im->w);
	memset(as, 0, im->w * sizeof(int));
	memset(rs, 0, im->w * sizeof(int));
	memset(gs, 0, im->w * sizeof(int));
	memset(bs, 0, im->w * sizeof(int));
	for (yy = 0; yy < mh; yy++)
	  {
	     p2 = im->data + ((yy + my) * im->w);
	     for (x = 0; x < im->w; x++)
	       {
		  as[x] += (*p2 >> 24) & 0xff;
		  rs[x] += (*p2 >> 16) & 0xff;
		  gs[x] += (*p2 >> 8) & 0xff;
		  bs[x] += *p2 & 0xff;
		  p2 ++;
	       }
	  }
	if (im->w > ((rad << 1) + 1))
	  {
	     for (x = 0; x < im->w; x++)
	       {	     
		  a = 0; r = 0; g = 0; b = 0;
		  mx = x - rad;
		  mw = (rad << 1) + 1;
		  if (mx < 0)
		    {
		       mw += mx;
		       mx = 0;
		    }
		  if ((mx + mw) > im->w)
		     mw = im->w - mx;
		  mt = mw * mh;
		  for (xx = mx; xx < (mw + mx); xx++)
		    {
		       a += as[xx];
		       r += rs[xx];
		       g += gs[xx];
		       b += bs[xx];
		    }
		  a = a / mt; r = r / mt; g = g / mt; b = b / mt;
		  *p1 = (a << 24) | (r << 16) | (g << 8) | b;
		  p1++;
	       }
	  }
	else
	  {
	  }
     }
   free(as);
   free(rs);
   free(gs);
   free(bs);
   free(im->data);
   im->data = data;
}

void
__imlib_SharpenImage(ImlibImage *im, int rad)
{
   DATA32 *data, *p1, *p2;
   int a, r, g, b, x, y;
   /* FIXME: impliment */
   
   data = malloc(im->w * im->h * sizeof(DATA32));
   if (rad == 0)
      return;
   else
     {
	int mul, mul2, tot;
	
	mul = (rad * 4) + 1;
	mul2 = rad;
	tot = mul - (mul2 * 4);
	for (y = 1; y < (im->h - 1); y++)
	  {
	     p1 = im->data + 1 + (y * im->w);
	     p2 = data + 1 + (y * im->w);
	     for (x = 1; x < (im->w - 1); x++)
	       {
		  b =  (int)((p1[0]      ) & 0xff) * 5;
		  g =  (int)((p1[0] >> 8 ) & 0xff) * 5;
		  r =  (int)((p1[0] >> 16) & 0xff) * 5;
		  a =  (int)((p1[0] >> 24) & 0xff) * 5;
		  b -= (int)((p1[-1]      ) & 0xff);
		  g -= (int)((p1[-1] >> 8 ) & 0xff);
		  r -= (int)((p1[-1] >> 16) & 0xff);
		  a -= (int)((p1[-1] >> 24) & 0xff);
		  b -= (int)((p1[1]      ) & 0xff);
		  g -= (int)((p1[1] >> 8 ) & 0xff);
		  r -= (int)((p1[1] >> 16) & 0xff);
		  a -= (int)((p1[1] >> 24) & 0xff);
		  b -= (int)((p1[-im->w]      ) & 0xff);
		  g -= (int)((p1[-im->w] >> 8 ) & 0xff);
		  r -= (int)((p1[-im->w] >> 16) & 0xff);
		  a -= (int)((p1[-im->w] >> 24) & 0xff);
		  b -= (int)((p1[im->w]      ) & 0xff);
		  g -= (int)((p1[im->w] >> 8 ) & 0xff);
		  r -= (int)((p1[im->w] >> 16) & 0xff);
		  a -= (int)((p1[im->w] >> 24) & 0xff);
		  
		  a = (a & ((~a) >> 16));
		  a = ((a | ((a & 256) - ((a & 256) >> 8))) );
		  r = (r & ((~r) >> 16));
		  r = ((r | ((r & 256) - ((r & 256) >> 8))) );
		  g = (g & ((~g) >> 16));
		  g = ((g | ((g & 256) - ((g & 256) >> 8))) );
		  b = (b & ((~b) >> 16));
		  b = ((b | ((b & 256) - ((b & 256) >> 8))) );
		  
		  *p2 = (a << 24) | (r << 16) | (g << 8) | b;
		  p2++;
		  p1++;
	       }
	  }
     }
   free(im->data);
   im->data = data;
}

void
__imlib_TileImageHoriz(ImlibImage *im)
{
   DATA32 *p1, *p2, *p3, *p, *data;
   int x, y, per, tmp, na, nr, ng, nb, mix, a, r, g, b, aa, rr, gg, bb;
   
   data = malloc(im->w * im->h * sizeof(DATA32));
   p1 = im->data;
   p = data;
   for (y = 0; y < im->h; y++)
     {
	p2 = p1 + (im->w >> 1);
	p3 = p1;
	per = (im->w >> 1);
	for (x = 0; x < (im->w >> 1); x++)
	  {
	     mix = (x * 255) / per;
	     b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     a =  (*p1 >> 24) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     aa = (*p2 >> 24) & 0xff;
	     
	     tmp = (r - rr) * mix;
	     nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (g - gg) * mix;
	     ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (b - bb) * mix;
	     nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (a - aa) * mix;
	     na = aa + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     *p = (na << 24) | (nr << 16) | (ng << 8) | nb;
	     p++;
	     p1++;
	     p2++;
	  }
	p2 = p3;
	per = (im->w - (im->w >> 1));
	for (; x < im->w; x++)
	  {
	     mix = ((im->w - 1 - x) * 255) / per;
	     b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     a =  (*p1 >> 24) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     aa = (*p2 >> 24) & 0xff;
	     
	     tmp = (r - rr) * mix;
	     nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (g - gg) * mix;
	     ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (b - bb) * mix;
	     nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (a - aa) * mix;
	     na = aa + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     *p = (na << 24) | (nr << 16) | (ng << 8) | nb;
	     p++;
	     p1++;
	     p2++;
	  }
     }
   free(im->data);
   im->data = data;
}

void
__imlib_TileImageVert(ImlibImage *im)
{
   DATA32 *p1, *p2, *p, *data;
   int x, y, tmp, na, nr, ng, nb, mix, a, r, g, b, aa, rr, gg, bb;
   
   data = malloc(im->w * im->h * sizeof(DATA32));
   p = data;
   for (y = 0; y < im->h; y++)
     {
	p1 = im->data + (y * im->w);
	if (y < (im->h >> 1))
	  {
	     p2 = im->data + ((y + (im->h >> 1)) * im->w);
	     mix = (y * 255) / (im->h >> 1);
	  }
	else
	  {
	     p2 = im->data + ((y - (im->h - (im->h >> 1))) * im->w);
	     mix = ((im->h - y) * 255) / (im->h - (im->h >> 1));
	  }
	for (x = 0; x < im->w; x++)
	  {
	     b =  (*p1      ) & 0xff;
	     g =  (*p1 >> 8 ) & 0xff;
	     r =  (*p1 >> 16) & 0xff;
	     a =  (*p1 >> 24) & 0xff;
	     
	     bb = (*p2      ) & 0xff;
	     gg = (*p2 >> 8 ) & 0xff;
	     rr = (*p2 >> 16) & 0xff;
	     aa = (*p2 >> 24) & 0xff;
	     
	     tmp = (r - rr) * mix;
	     nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (g - gg) * mix;
	     ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (b - bb) * mix;
	     nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     tmp = (a - aa) * mix;
	     na = aa + ((tmp + (tmp >> 8) + 0x80) >> 8);
	     *p = (na << 24) | (nr << 16) | (ng << 8) | nb;
	     p++;
	     p1++;
	     p2++;
	  }
     }
   free(im->data);
   im->data = data;
}

#define BLEND(r1, g1, b1, a1, dest) \
bb = ((dest)      ) & 0xff;\
gg = ((dest) >> 8 ) & 0xff;\
rr = ((dest) >> 16) & 0xff;\
aa = ((dest) >> 24) & 0xff;\
tmp = ((r1) - rr) * (a1);\
nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);\
tmp = ((g1) - gg) * (a1);\
ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);\
tmp = ((b1) - bb) * (a1);\
nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);\
tmp = (a1) + aa;\
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

ImlibUpdate *
__imlib_draw_line(ImlibImage *im, int x1, int y1, int x2, int y2,
		  DATA8 r, DATA8 g, DATA8 b, DATA8 a, char make_updates)
{
   int x, y, dx, dy, yy, xx, am, tmp;
   DATA32 *p;
   DATA8 aaa, nr, ng, nb, rr, gg, bb, aa, na;

   /* clip to top edge */
   if ((y1 < 0) && (y2 < 0))
      return NULL;
   if (y1 < 0)
     {
	x1 += (y1 * (x1 - x2)) / (y2 - y1);
	y1 = 0;
     }
   if (y2 < 0)
     {
	x2 += (y2 * (x1 - x2)) / (y2 - y1);
	y2 = 0;
     }
   /* clip to bottom edge */   
   if ((y1 >= im->h) && (y2 >= im->h))
      return NULL;
   if (y1 >= im->h)
     {
	x1 -= ((im->h - y1) * (x1 - x2)) / (y2 - y1);
	y1 = im->h - 1;
     }
   if (y2 >= im->h)
     {
	x2 -= ((im->h - y2) * (x1 - x2)) / (y2 - y1);
	y2 = im->h - 1;
     }
   /* clip to top edge */
   if ((x1 < 0) && (x2 < 0))
      return NULL;
   if (x1 < 0)
     {
	y1 += (x1 * (y1 - y2)) / (x2 - x1);
	x1 = 0;
     }
   if (x2 < 0)
     {
	y2 += (x2 * (y1 - y2)) / (x2 - x1);
	x2 = 0;
     }
   /* clip to bottom edge */   
   if ((x1 >= im->w) && (x2 >= im->w))
      return NULL;
   if (x1 >= im->w)
     {
	y1 -= ((im->w - x1) * (y1 - y2)) / (x2 - x1);
	x1 = im->w - 1;
     }
   if (y2 >= im->w)
     {
	y2 -= ((im->w - x2) * (y1 - y2)) / (x2 - x1);
	x2 = im->w - 1;
     }
   dx = x2 - x1;
   dy = y2 - y1;
   if (x1 > x2)
     {
	int tmp;
	
	tmp = x1; x1 = x2; x2 = tmp;
	tmp = y1; y1 = y2; y2 = tmp;
	dx = x2 - x1;
	dy = y2 - y1;
     }
   /* vertical line */
   if (dx == 0)
     {
	if (y1 < y2)
	  {
	     p = &(im->data[(im->w * y1) + x1]);
	     for (y = y1; y <= y2; y++)
	       {
		  BLEND(r, g, b, a, *p);
		  p += im->w;
	       }
             return __imlib_AddUpdate(NULL, x1, y1, 1, (y2 - y1 + 1));
	  }
	else
	  {
	     p = &(im->data[(im->w * y2) + x1]);
	     for (y = y2; y <= y1; y++)
	       {
		  BLEND(r, g, b, a, *p);
		  p += im->w;
	       }
             return __imlib_AddUpdate(NULL, x1, y2, 1, (y1 - y2 + 1));
	  }
     }
   /* horizontal line */
   if (dy == 0)
     {
	if (x1 < x2)
	  {
	     p = &(im->data[(im->w * y1) + x1]);
	     for (x = x1; x <= x2; x++)
	       {
		  BLEND(r, g, b, a, *p);
		  p++;
	       }
             return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1), 1);
	  }
	else
	  {
	     p = &(im->data[(im->w * y1) + x2]);
	     for (x = x2; x <= x1; x++)
	       {
		  BLEND(r, g, b, a, *p);
		  p++;
	       }
             return __imlib_AddUpdate(NULL, x2, y1, (x1 - x2 + 1), 1);
	  }
     }
   /* 1    */
   /*  \   */
   /*   \  */
   /*    2 */
   if (y2 > y1)
     {
	/* steep */
	if (dy > dx)
	  {
	     dx = ((dx << 16) / dy);
	     x = x1 << 16;
	     for (y = y1; y <= y2; y++)
	       {
		  xx = x >> 16;
		  am = 256 - (((x - (xx << 16)) + 1) >> 8);
		  aaa = (a * am) >> 8;
		  p = &(im->data[(im->w * y) + xx]);
		  BLEND(r, g, b, aaa, *p);
		  if (xx < (im->w - 1))
		    {
		       am = 256 - am;
		       aaa = (a * am) >> 8;
		       p ++;
		       BLEND(r, g, b, aaa, *p);
		    }
		  x += dx;
	       }
	     return __imlib_AddUpdate(NULL, x1, y1, 
				      (x2 - x1 + 1), (y2 - y1 + 1));
	  }
	/* shallow */
	else
	  {
	     dy = ((dy << 16) / dx);
	     y = y1 << 16;
	     for (x = x1; x <= x2; x++)
	       {
		  yy = y >> 16;
		  am = 256 - (((y - (yy << 16)) + 1) >> 8);
		  aaa = (a * am) >> 8;
		  p = &(im->data[(im->w * yy) + x]);
		  BLEND(r, g, b, aaa, *p);
		  if (yy < (im->h - 1))
		    {
		       am = 256 - am;
		       aaa = (a * am) >> 8;
		       p += im->w;
		       BLEND(r, g, b, aaa, *p);
		    }
		  y += dy;
	       }
	     return __imlib_AddUpdate(NULL, x1, y1, 
				      (x2 - x1 + 1), (y2 - y1 + 1));
	  }
     }
   /*    2 */
   /*   /  */
   /*  /   */
   /* 1    */
   else
     {
	/* steep */
	if (-dy > dx)
	  {
	     dx = ((dx << 16) / -dy);
	     x = (x1 + 1) << 16;
	     for (y = y1; y >= y2; y--)
	       {
		  xx = x >> 16;
		  am = (((x - (xx << 16)) + 1) >> 8);
		  aaa = (a * am) >> 8;
		  p = &(im->data[(im->w * y) + xx]);
		  BLEND(r, g, b, aaa, *p);
		  if (xx < (im->w - 1))
		    {
		       am = 256 - am;
		       aaa = (a * am) >> 8;
		       p--;
		       BLEND(r, g, b, aaa, *p);
		    }
		  x += dx;
	       }
	     return __imlib_AddUpdate(NULL, x1, y2, 
				      (x2 - x1 + 1), (y1 - y2 + 1));
	  }
	/* shallow */
	else
	  {
	     dy = ((dy << 16) / dx);
	     y = y1 << 16;
	     for (x = x1; x <= x2; x++)
	       {
		  yy = y >> 16;
		  am = 256 - (((y - (yy << 16)) + 1) >> 8);
		  aaa = (a * am) >> 8;
		  p = &(im->data[(im->w * yy) + x]);
		  BLEND(r, g, b, aaa, *p);
		  if (yy < (im->h - 1))
		    {
		       am = 256 - am;
		       aaa = (a * am) >> 8;
		       p += im->w;
		       BLEND(r, g, b, aaa, *p);
		    }
		  y += dy;
	       }
	     return __imlib_AddUpdate(NULL, x1, y2, 
				      (x2 - x1 + 1), (y1 - y2 + 1));
	  }
     }
   return NULL;
}
