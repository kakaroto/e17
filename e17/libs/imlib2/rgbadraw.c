#include <X11/Xlib.h>
#include "common.h"
#include "scale.h"
#include "image.h"
#include "context.h"
#include "rgba.h"
#include "blend.h"
#include "rgbadraw.h"

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
   int x, y, per, tmp, na, nr, ng, nb, mix, a, r, g, b, aa, rr, gg, bb;
   
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


