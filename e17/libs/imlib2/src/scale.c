#include "common.h"
#include <X11/Xlib.h>
#include "colormod.h"
#include "image.h"
#include "blend.h"
#include "scale.h"

#define RGBA_COMPOSE(r, g, b, a)  ((a) << 24) | ((r) << 16) | ((g) << 8) | (b)
#define INV_XAP                   (255 - xapoints[x])
#define XAP                       (xapoints[x])
#define INV_YAP                   (255 - yapoints[dyy + y])
#define YAP                       (yapoints[dyy + y])

/* forcibly diable asm for scaling - comment out to enable */
/*
#ifdef DO_MMX_ASM
#undef DO_MMX_ASM
#endif
*/

DATA32 **
__imlib_CalcYPoints(DATA32 *src, int sw, int sh, int dh, int b1, int b2)
{
   DATA32 **p;
   int i, j = 0;
   int val, inc;

   p = malloc((dh + 1) * sizeof(DATA32 *));
   if (dh < (b1 + b2))
     {
	if (dh < b1)
	  {
	     b1 = dh;
	     b2 = 0;
	  }
	else
	   b2 = dh - b1;
     }
   val = 0;
   inc = 1 << 16;
   for (i = 0; i < b1; i++)
     {
	p[j++] = src + ((val >> 16) * sw);
	val += inc;
     }
   if (dh > (b1 + b2))
     {
	val = (b1 << 16);
	inc = ((sh - b1 - b2) << 16) / (dh - b1 - b2);
	for (i = 0; i < (dh - b1 - b2); i++)
	  {
	     p[j++] = src + ((val >> 16) * sw);
	     val += inc;
	  }
     }
   val = (sh - b2) << 16;
   inc = 1 << 16;
   for (i = 0; i <= b2; i++)
     {
	p[j++] = src + ((val >> 16) * sw);
	val += inc;
     }
   return p;
}

int *
__imlib_CalcXPoints(int sw, int dw, int b1, int b2)
{
   int *p, i, j = 0;
   int val, inc;

   p = malloc((dw + 1) * sizeof(int));
   if (dw < (b1 + b2))
     {
	if (dw < b1)
	  {
	     b1 = dw;
	     b2 = 0;
	  }
	else
	   b2 = dw - b1;
     }
   val = 0;
   inc = 1 << 16;
   for (i = 0; i < b1; i++)
     {
	p[j++] = (val >> 16);
	val += inc;
     }
   if (dw > (b1 + b2))
     {
	val = (b1 << 16);
	inc = ((sw - b1 - b2) << 16) / (dw - b1 - b2);
	for (i = 0; i < (dw - b1 - b2); i++)
	  {
	     p[j++] = (val >> 16);
	     val += inc;
	  }
     }
   val = (sw - b2) << 16;
   inc = 1 << 16;
   for (i = 0; i <= b2; i++)
     {
	p[j++] = (val >> 16);
	val += inc;
     }
   return p;
}

int *
__imlib_CalcApoints(int s, int d, int b1, int b2)
{
   int *p, i, v, j = 0;
   
   p = malloc(d * sizeof(int));
   if (d < (b1 + b2))
     {
	if (d < b1)
	  {
	     b1 = d;
	     b2 = 0;
	  }
	else
	   b2 = d - b1;
     }
   /* scaling up */
   if (d > s)
     {
	int val, inc;
	
	for (i = 0; i < b1; i++)
	   p[j++] = 0;
	if (d > (b1 + b2))
	  {
	     int ss, dd;
	     
	     ss = s - b1 - b2;
	     dd = d - b1 - b2;
	     val = 0;
	     inc = (ss << 16) / dd;
	     for (i = 0; i < dd; i++)
	       {
		  p[j++] = (val >> 8) - ((val >> 8) & 0xffffff00);
		  if (((val >> 16) + b1) >= (s - 1))
		     p[j - 1] = 0;
		  val += inc;
	       }
	  }
	for (i = 0; i < b2; i++)
	   p[j++] = 0;
     }
   /* scaling down */
   else
     {
	int val, inc;
	
	for (i = 0; i < b1; i++)
	   p[j++] = (1 << (16 + 14)) + (1 << 14);
	if (d > (b1 + b2))
	  {
	     int ss, dd, ap, Cp;
	     
	     ss = s - b1 - b2;
	     dd = d - b1 - b2;
	     val = 0;
	     inc = (ss << 16) / dd;
	     Cp = ((dd << 14) / ss) + 1;
	     for (i = 0; i < dd; i++)
	       {
		  ap = ((0x100 - ((val >> 8) & 0xff)) * Cp) >> 8;
		  p[j] = ap | (Cp << 16);
		  j++;
		  val += inc;
	       }
	  }
	for (i = 0; i < b2; i++)
	   p[j++] = (1 << (16 + 14)) + (1 << 14);
     }
   return p;
}

/* scale by pixel sampling only */
void
__imlib_ScaleSampleRGBA(DATA32 **ypoints, int *xpoints, DATA32 *dest,
			int dxx, int dyy, int dx, int dy, int dw, int dh, int dow)
{
   DATA32 *sptr, *dptr;
   int x, y, end;
   
   /* whats the last pixel ont he line so we stop there */
   end = dxx + dw;
   /* go through every scanline in the output buffer */
   for (y = 0; y < dh; y++)
     {
	/* get the pointer to the start of the destination scanline */
	dptr = dest + dx + ((y + dy) * dow);
	/* calculate the source line we'll scan from */
	sptr = ypoints[dyy + y];
	/* go thru the scanline and copy across */
	for (x = dxx; x < end; x++)
	   *dptr++ = sptr[xpoints[x]];
     }
}

/* FIXME: NEED to optimise ScaleAARGBA - currently its "ok" but needs work*/

/* scale by area sampling */
void
__imlib_ScaleAARGBA(DATA32 **ypoints, int *xpoints, DATA32 *dest,
		    int *xapoints, int *yapoints, char xup, char yup,
		    int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow)
{
   DATA32 *sptr, *dptr;
   int x, y, i, j, end;
   
   end = dxx + dw;
   /* scaling up both ways */
   if ((xup) && (yup))
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     if (YAP > 0)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r, g, b, a;
		       int rr, gg, bb, aa;
		       DATA32 *pix;
				   
		       if (XAP > 0)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_XAP;
			    g = G_VAL(pix) * INV_XAP;
			    b = B_VAL(pix) * INV_XAP;
			    a = A_VAL(pix) * INV_XAP;
			    pix++;
			    r += R_VAL(pix) * XAP;
			    g += G_VAL(pix) * XAP;
			    b += B_VAL(pix) * XAP;
			    a += A_VAL(pix) * XAP;
			    pix += sow;
			    rr = R_VAL(pix) * XAP;
			    gg = G_VAL(pix) * XAP;
			    bb = B_VAL(pix) * XAP;
			    aa = A_VAL(pix) * XAP;
			    pix--;
			    rr += R_VAL(pix) * INV_XAP;
			    gg += G_VAL(pix) * INV_XAP;
			    bb += B_VAL(pix) * INV_XAP;
			    aa += A_VAL(pix) * INV_XAP;
			    r = ((rr * YAP) + (r * INV_YAP)) >> 16;
			    g = ((gg * YAP) + (g * INV_YAP)) >> 16;
			    b = ((bb * YAP) + (b * INV_YAP)) >> 16;
			    a = ((aa * YAP) + (a * INV_YAP)) >> 16;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_YAP;
			    g = G_VAL(pix) * INV_YAP;
			    b = B_VAL(pix) * INV_YAP;
			    a = A_VAL(pix) * INV_YAP;
			    pix += sow;
			    r += R_VAL(pix) * YAP;
			    g += G_VAL(pix) * YAP;
			    b += B_VAL(pix) * YAP;
			    a += A_VAL(pix) * YAP;
			    r >>= 8;
			    g >>= 8;
			    b >>= 8;
			    a >>= 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		    }
	       }
	     else
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r, g, b, a;
		       int rr, gg, bb, aa;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_XAP;
			    g = G_VAL(pix) * INV_XAP;
			    b = B_VAL(pix) * INV_XAP;
			    a = A_VAL(pix) * INV_XAP;
			    pix++;
			    r += R_VAL(pix) * XAP;
			    g += G_VAL(pix) * XAP;
			    b += B_VAL(pix) * XAP;
			    a += A_VAL(pix) * XAP;
			    r >>= 8;
			    g >>= 8;
			    b >>= 8;
			    a >>= 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			  *dptr++ = sptr[xpoints[x] ];
		    }
	       }
	  }
     }
   /* if we're scaling down vertically */
   else if ((xup) && (!yup))
#ifndef OLD_SCALE_DOWN
     {
	/*\ 'Correct' version, with math units prepared for MMXification \*/
	int Cy, j;
	DATA32 *pix;
	int r, g, b, a, rr, gg, bb, aa;
	int yap;
		 
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     Cy = YAP >> 16;
	     yap = YAP & 0xffff;

	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  pix = ypoints[dyy + y] + xpoints[x];
		  r = (R_VAL(pix) * yap) >> 10;
		  g = (G_VAL(pix) * yap) >> 10;
		  b = (B_VAL(pix) * yap) >> 10;
		  a = (A_VAL(pix) * yap) >> 10;
		  pix += sow;
		  for (j = (1 << 14) - yap; j > Cy; j -= Cy)
		    {
		       r += (R_VAL(pix) * Cy) >> 10;
		       g += (G_VAL(pix) * Cy) >> 10;
		       b += (B_VAL(pix) * Cy) >> 10;
		       a += (A_VAL(pix) * Cy) >> 10;
		       pix += sow;
		    }
		  if (j > 0)
		    {
		       r += (R_VAL(pix) * j) >> 10;
		       g += (G_VAL(pix) * j) >> 10;
		       b += (B_VAL(pix) * j) >> 10;
		       a += (A_VAL(pix) * j) >> 10;
		    }
		  if (XAP > 0)
		    {
		       pix = ypoints[dyy + y] + xpoints[x] + 1;
		       rr = (R_VAL(pix) * yap) >> 10;
		       gg = (G_VAL(pix) * yap) >> 10;
		       bb = (B_VAL(pix) * yap) >> 10;
		       aa = (A_VAL(pix) * yap) >> 10;
		       pix += sow;
		       for (j = (1 << 14) - yap; j > Cy; j -= Cy)
			 {
			    rr += (R_VAL(pix) * Cy) >> 10;
			    gg += (G_VAL(pix) * Cy) >> 10;
			    bb += (B_VAL(pix) * Cy) >> 10;
			    aa += (A_VAL(pix) * Cy) >> 10;
			    pix += sow;
			 }
		       if (j > 0)
			 {
			    rr += (R_VAL(pix) * j) >> 10;
			    gg += (G_VAL(pix) * j) >> 10;
			    bb += (B_VAL(pix) * j) >> 10;
			    aa += (A_VAL(pix) * j) >> 10;
			 }
		       r = r * INV_XAP;
		       g = g * INV_XAP;
		       b = b * INV_XAP;
		       a = a * INV_XAP;
		       r = (r + ((rr * XAP))) >> 12;
		       g = (g + ((gg * XAP))) >> 12;
		       b = (b + ((bb * XAP))) >> 12;
		       a = (a + ((aa * XAP))) >> 12;
		    }
		  else
		    {
		       r >>= 4;
		       g >>= 4;
		       b >>= 4;
		       a >>= 4;
		    }
		  *dptr = RGBA_COMPOSE(r, g, b, a);
		  dptr++;
	       }		       
	  }
     }
#else
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     int yap;
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];

	     yap = (ypoints[dyy + y + 1] - ypoints[dyy + y]) / sow;
	     if (yap > 1)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0, a = 0;
		       int rr = 0, gg = 0, bb = 0, aa = 0;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    pix = sptr + xpoints[x];
			    for (j = 0; j < yap; j++)
			      {
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 a += A_VAL(pix);
				 rr += R_VAL(pix + 1);
				 gg += G_VAL(pix + 1);
				 bb += B_VAL(pix + 1);
				 aa += A_VAL(pix + 1);
				 pix += sow;
			      }
			    r = r * INV_XAP / yap;
			    g = g * INV_XAP / yap;
			    b = b * INV_XAP / yap;
			    a = a * INV_XAP / yap;
			    r = (r + ((rr * XAP) / yap)) >> 8;
			    g = (g + ((gg * XAP) / yap)) >> 8;
			    b = (b + ((bb * XAP) / yap)) >> 8;
			    a = (a + ((aa * XAP) / yap)) >> 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			 {
			    pix = sptr + xpoints[x];
			    for (j = 0; j < yap; j++)
			      {
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 a += A_VAL(pix);
				 pix += sow;
			      }
			    r /= yap;
			    g /= yap;
			    b /= yap;
			    a /= yap;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		    }		       
	       }
	     else
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0, a = 0;
		       int count;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_XAP;
			    g = G_VAL(pix) * INV_XAP;
			    b = B_VAL(pix) * INV_XAP;
			    a = A_VAL(pix) * INV_XAP;
			    pix++;
			    r += R_VAL(pix) * XAP;
			    g += G_VAL(pix) * XAP;
			    b += B_VAL(pix) * XAP;
			    a += A_VAL(pix) * XAP;
			    r >>= 8;
			    g >>= 8;
			    b >>= 8;
			    a >>= 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			  *dptr++ = sptr[xpoints[x]];		      
		    }
 	       }
	  }
     }
#endif
   /* if we're scaling down horizontally */
   else if ((!xup) && (yup))
#ifndef OLD_SCALE_DOWN
     {
	/*\ 'Correct' version, with math units prepared for MMXification \*/
	int Cx, j;
	DATA32 *pix;
	int r, g, b, a, rr, gg, bb, aa;
	int xap;

	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  Cx = XAP >> 16;
		  xap = XAP & 0xffff;

		  pix = ypoints[dyy + y] + xpoints[x];
		  r = (R_VAL(pix) * xap) >> 10;
		  g = (G_VAL(pix) * xap) >> 10;
		  b = (B_VAL(pix) * xap) >> 10;
		  a = (A_VAL(pix) * xap) >> 10;
		  pix++;
		  for (j = (1 << 14) - xap; j > Cx; j -= Cx)
		    {
		       r += (R_VAL(pix) * Cx) >> 10;
		       g += (G_VAL(pix) * Cx) >> 10;
		       b += (B_VAL(pix) * Cx) >> 10;
		       a += (A_VAL(pix) * Cx) >> 10;
		       pix++;
		    }
		  if (j > 0)
		    {
		       r += (R_VAL(pix) * j) >> 10;
		       g += (G_VAL(pix) * j) >> 10;
		       b += (B_VAL(pix) * j) >> 10;
		       a += (A_VAL(pix) * j) >> 10;
		    }
		  if (YAP > 0)
		    {
		       pix = ypoints[dyy + y] + xpoints[x] + sow;
		       rr = (R_VAL(pix) * xap) >> 10;
		       gg = (G_VAL(pix) * xap) >> 10;
		       bb = (B_VAL(pix) * xap) >> 10;
		       aa = (A_VAL(pix) * xap) >> 10;
		       pix++;
		       for (j = (1 << 14) - xap; j > Cx; j -= Cx)
			 {
			    rr += (R_VAL(pix) * Cx) >> 10;
			    gg += (G_VAL(pix) * Cx) >> 10;
			    bb += (B_VAL(pix) * Cx) >> 10;
			    aa += (A_VAL(pix) * Cx) >> 10;
			    pix++;
			 }
		       if (j > 0)
			 {
			    rr += (R_VAL(pix) * j) >> 10;
			    gg += (G_VAL(pix) * j) >> 10;
			    bb += (B_VAL(pix) * j) >> 10;
			    aa += (A_VAL(pix) * j) >> 10;
			 }
		       r = r * INV_YAP;
		       g = g * INV_YAP;
		       b = b * INV_YAP;
		       a = a * INV_YAP;
		       r = (r + ((rr * YAP))) >> 12;
		       g = (g + ((gg * YAP))) >> 12;
		       b = (b + ((bb * YAP))) >> 12;
		       a = (a + ((aa * YAP))) >> 12;
		    }
		  else
		    {
		       r >>= 4;
		       g >>= 4;
		       b >>= 4;
		       a >>= 4;
		    }
		  *dptr = RGBA_COMPOSE(r, g, b, a);
		  dptr++;
	       }		       
	  }
     }
#else
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     if (YAP > 0)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0, a = 0;
		       int rr = 0, gg = 0, bb = 0, aa = 0;
		       int xap;
		       DATA32 *pix;
		  
		       xap = xpoints[x + 1] - xpoints[x];
		       if (xap > 1)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    for (i = 0; i < xap; i++)
			      {
				 r += R_VAL(pix + i);
				 g += G_VAL(pix + i);
				 b += B_VAL(pix + i);
				 a += A_VAL(pix + i);
			      }
			    r = r * INV_YAP / xap;
			    g = g * INV_YAP / xap;
			    b = b * INV_YAP / xap;
			    a = a * INV_YAP / xap;
			    pix = ypoints[dyy + y] + xpoints[x] + sow;
			    for (i = 0; i < xap; i++)
			      {
				 rr += R_VAL(pix + i);
				 gg += G_VAL(pix + i);
				 bb += B_VAL(pix + i);
				 aa += A_VAL(pix + i);
			      }
			    r = (r + ((rr * YAP) / xap)) >> 8;
			    g = (g + ((gg * YAP) / xap)) >> 8;
			    b = (b + ((bb * YAP) / xap)) >> 8;
			    a = (a + ((aa * YAP) / xap)) >> 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_YAP;
			    g = G_VAL(pix) * INV_YAP;
			    b = B_VAL(pix) * INV_YAP;
			    a = A_VAL(pix) * INV_YAP;
			    pix += sow;
			    r += R_VAL(pix) * YAP;
			    g += G_VAL(pix) * YAP;
			    b += B_VAL(pix) * YAP;
			    a += A_VAL(pix) * YAP;
			    r >>= 8;
			    g >>= 8;
			    b >>= 8;
			    a >>= 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		    }
	       }
	     else
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0, a = 0;
		       int xap;
		       DATA32 *pix;
		  
		       xap = xpoints[x + 1] - xpoints[x];
		       if (xap > 1)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    for (i = 0; i < xap; i++)
			      {
				 r += R_VAL(pix + i);
				 g += G_VAL(pix + i);
				 b += B_VAL(pix + i);
				 a += A_VAL(pix + i);
			      }
			    r /= xap;
			    g /= xap;
			    b /= xap;
			    a /= xap;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			  *dptr++ = sptr[xpoints[x]];		      
		    }
	       }
	  }
     }
#endif
   /* if we're scaling down horizontally & vertically */
   else
#ifndef OLD_SCALE_DOWN
     {
	/*\ 'Correct' version, with math units prepared for MMXification:
	|*|  The operation 'b = (b * c) >> 16' translates to pmulhw,
	|*|  so the operation 'b = (b * c) >> d' would translate to
	|*|  psllw (16 - d), %mmb; pmulh %mmc, %mmb
	\*/
	int Cx, Cy, i, j;
	DATA32 *pix;
	int a, r, g, b, ax, rx, gx, bx;
	int xap, yap;

	for (y = 0; y < dh; y++)
	  {
	     Cy = YAP >> 16;
	     yap = YAP & 0xffff;

	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  Cx = XAP >> 16;
		  xap = XAP & 0xffff;

		  sptr = ypoints[dyy + y] + xpoints[x];
		  pix = sptr;
		  sptr += sow;
		  rx = (R_VAL(pix) * xap) >> 9;
		  gx = (G_VAL(pix) * xap) >> 9;
		  bx = (B_VAL(pix) * xap) >> 9;
		  ax = (A_VAL(pix) * xap) >> 9;
		  pix++;
		  for (i = (1 << 14) - xap; i > Cx; i -= Cx)
		    {
		       rx += (R_VAL(pix) * Cx) >> 9;
		       gx += (G_VAL(pix) * Cx) >> 9;
		       bx += (B_VAL(pix) * Cx) >> 9;
		       ax += (A_VAL(pix) * Cx) >> 9;
		       pix++;
		    }
		  if (i > 0)
		    {
		       rx += (R_VAL(pix) * i) >> 9;
		       gx += (G_VAL(pix) * i) >> 9;
		       bx += (B_VAL(pix) * i) >> 9;
		       ax += (A_VAL(pix) * i) >> 9;
		    }

		  r = (rx * yap) >> 14;
		  g = (gx * yap) >> 14;
		  b = (bx * yap) >> 14;
		  a = (ax * yap) >> 14;

		  for (j = (1 << 14) - yap; j > Cy; j -= Cy)
		    {
		       pix = sptr;
		       sptr += sow;
		       rx = (R_VAL(pix) * xap) >> 9;
		       gx = (G_VAL(pix) * xap) >> 9;
		       bx = (B_VAL(pix) * xap) >> 9;
		       ax = (A_VAL(pix) * xap) >> 9;
		       pix++;
		       for (i = (1 << 14) - xap; i > Cx; i -= Cx)
			 {
			    rx += (R_VAL(pix) * Cx) >> 9;
			    gx += (G_VAL(pix) * Cx) >> 9;
			    bx += (B_VAL(pix) * Cx) >> 9;
			    ax += (A_VAL(pix) * Cx) >> 9;
			    pix++;
			 }
		       if (i > 0)
			 {
			    rx += (R_VAL(pix) * i) >> 9;
			    gx += (G_VAL(pix) * i) >> 9;
			    bx += (B_VAL(pix) * i) >> 9;
			    ax += (A_VAL(pix) * i) >> 9;
			 }

		       r += (rx * Cy) >> 14;
		       g += (gx * Cy) >> 14;
		       b += (bx * Cy) >> 14;
		       a += (ax * Cy) >> 14;
		    }
		  if (j > 0)
		    {
		       pix = sptr;
		       sptr += sow;
		       rx = (R_VAL(pix) * xap) >> 9;
		       gx = (G_VAL(pix) * xap) >> 9;
		       bx = (B_VAL(pix) * xap) >> 9;
		       ax = (A_VAL(pix) * xap) >> 9;
		       pix++;
		       for (i = (1 << 14) - xap; i > Cx; i -= Cx)
			 {
			    rx += (R_VAL(pix) * Cx) >> 9;
			    gx += (G_VAL(pix) * Cx) >> 9;
			    bx += (B_VAL(pix) * Cx) >> 9;
			    ax += (A_VAL(pix) * Cx) >> 9;
			    pix++;
			 }
		       if (i > 0)
			 {
			    rx += (R_VAL(pix) * i) >> 9;
			    gx += (G_VAL(pix) * i) >> 9;
			    bx += (B_VAL(pix) * i) >> 9;
			    ax += (A_VAL(pix) * i) >> 9;
			 }
	
		       r += (rx * j) >> 14;
		       g += (gx * j) >> 14;
		       b += (bx * j) >> 14;
		       a += (ax * j) >> 14;
		    }

		  R_VAL(dptr) = r >> 5;
		  G_VAL(dptr) = g >> 5;
		  B_VAL(dptr) = b >> 5;
		  A_VAL(dptr) = a >> 5;
		  dptr++;
	       }
	  }
     }
#else
     {
	int count;
	DATA32 *pix;
	int a, r, g, b;
	
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     int yap = (ypoints[dyy + y + 1] - ypoints[dyy + y]) / sow;
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     for (x = dxx; x < end; x++)
	       {
		  int xap = xpoints[x + 1] - xpoints[x];
		  if ((xap > 1) || (yap > 1))
		    {
		       r = 0; g = 0; b = 0;
		       pix = ypoints[dyy + y] + xpoints[x];
		       for (j = yap; --j >= 0; )
			 {
			    for (i = xap; --i >= 0; )
			      {
				 r += R_VAL(pix + i);
				 g += G_VAL(pix + i);
				 b += B_VAL(pix + i);
				 a += A_VAL(pix + i);
			      }
			    pix += sow;
			 }
		       count = xap * yap;
		       R_VAL(dptr) = r / count;
		       G_VAL(dptr) = g / count;
		       B_VAL(dptr) = b / count;
		       A_VAL(dptr) = a / count;
		       dptr++;
		    }
		  else
		     *dptr++ = sptr[xpoints[x]];
	       }
	  }
     }
#endif
}

/* scale by area sampling - IGNORE the ALPHA byte*/
void
__imlib_ScaleAARGB(DATA32 **ypoints, int *xpoints, DATA32 *dest,
		   int *xapoints, int *yapoints, char xup, char yup,
		   int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow)
{
   DATA32 *sptr, *dptr;
   int x, y, i, j, end;
   
   end = dxx + dw;
   /* scaling up both ways */
   if ((xup) && (yup))
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     if (YAP > 0)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0;
		       int rr = 0, gg = 0, bb = 0;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_XAP;
			    g = G_VAL(pix) * INV_XAP;
			    b = B_VAL(pix) * INV_XAP;
			    pix++;
			    r += R_VAL(pix) * XAP;
			    g += G_VAL(pix) * XAP;
			    b += B_VAL(pix) * XAP;
			    pix += sow;
			    rr = R_VAL(pix) * XAP;
			    gg = G_VAL(pix) * XAP;
			    bb = B_VAL(pix) * XAP;
			    pix --;
			    rr += R_VAL(pix) * INV_XAP;
			    gg += G_VAL(pix) * INV_XAP;
			    bb += B_VAL(pix) * INV_XAP;
			    r = ((rr * YAP) + (r * INV_YAP)) >> 16;
			    g = ((gg * YAP) + (g * INV_YAP)) >> 16;
			    b = ((bb * YAP) + (b * INV_YAP)) >> 16;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_YAP;
			    g = G_VAL(pix) * INV_YAP;
			    b = B_VAL(pix) * INV_YAP;
			    pix += sow;
			    r += R_VAL(pix) * YAP;
			    g += G_VAL(pix) * YAP;
			    b += B_VAL(pix) * YAP;
			    r >>= 8;
			    g >>= 8;
			    b >>= 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		    }
	       }
	     else
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0;
		       int rr = 0, gg = 0, bb = 0;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_XAP;
			    g = G_VAL(pix) * INV_XAP;
			    b = B_VAL(pix) * INV_XAP;
			    pix++;
			    r += R_VAL(pix) * XAP;
			    g += G_VAL(pix) * XAP;
			    b += B_VAL(pix) * XAP;
			    r >>= 8;
			    g >>= 8;
			    b >>= 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			  *dptr++ = sptr[xpoints[x] ];
		    }
	       }
	  }
     }
   /* if we're scaling down vertically */
   else if ((xup) && (!yup))
#ifndef OLD_SCALE_DOWN
     {
	/*\ 'Correct' version, with math units prepared for MMXification \*/
	int Cy, j;
	DATA32 *pix;
	int r, g, b, rr, gg, bb;
	int yap;
		 
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     Cy = YAP >> 16;
	     yap = YAP & 0xffff;

	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  pix = ypoints[dyy + y] + xpoints[x];
		  r = (R_VAL(pix) * yap) >> 10;
		  g = (G_VAL(pix) * yap) >> 10;
		  b = (B_VAL(pix) * yap) >> 10;
		  pix += sow;
		  for (j = (1 << 14) - yap; j > Cy; j -= Cy)
		    {
		       r += (R_VAL(pix) * Cy) >> 10;
		       g += (G_VAL(pix) * Cy) >> 10;
		       b += (B_VAL(pix) * Cy) >> 10;
		       pix += sow;
		    }
		  if (j > 0)
		    {
		       r += (R_VAL(pix) * j) >> 10;
		       g += (G_VAL(pix) * j) >> 10;
		       b += (B_VAL(pix) * j) >> 10;
		    }
		  if (XAP > 0)
		    {
		       pix = ypoints[dyy + y] + xpoints[x] + 1;
		       rr = (R_VAL(pix) * yap) >> 10;
		       gg = (G_VAL(pix) * yap) >> 10;
		       bb = (B_VAL(pix) * yap) >> 10;
		       pix += sow;
		       for (j = (1 << 14) - yap; j > Cy; j -= Cy)
			 {
			    rr += (R_VAL(pix) * Cy) >> 10;
			    gg += (G_VAL(pix) * Cy) >> 10;
			    bb += (B_VAL(pix) * Cy) >> 10;
			    pix += sow;
			 }
		       if (j > 0)
			 {
			    rr += (R_VAL(pix) * j) >> 10;
			    gg += (G_VAL(pix) * j) >> 10;
			    bb += (B_VAL(pix) * j) >> 10;
			 }
		       r = r * INV_XAP;
		       g = g * INV_XAP;
		       b = b * INV_XAP;
		       r = (r + ((rr * XAP))) >> 12;
		       g = (g + ((gg * XAP))) >> 12;
		       b = (b + ((bb * XAP))) >> 12;
		    }
		  else
		    {
		       r >>= 4;
		       g >>= 4;
		       b >>= 4;
		    }
		  *dptr = RGBA_COMPOSE(r, g, b, 0xff);
		  dptr++;
	       }		       
	  }
     }
#else
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     int yap;
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];

	     yap = (ypoints[dyy + y + 1] - ypoints[dyy + y]) / sow;
	     if (yap > 1)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0;
		       int rr = 0, gg = 0, bb = 0;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    pix = sptr + xpoints[x];
			    for (j = 0; j < yap; j++)
			      {
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 rr += R_VAL(pix + 1);
				 gg += G_VAL(pix + 1);
				 bb += B_VAL(pix + 1);
				 pix += sow;
			      }
			    r = r * INV_XAP / yap;
			    g = g * INV_XAP / yap;
			    b = b * INV_XAP / yap;
			    r = (r + ((rr * XAP) / yap)) >> 8;
			    g = (g + ((gg * XAP) / yap)) >> 8;
			    b = (b + ((bb * XAP) / yap)) >> 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			 {
			    pix = sptr + xpoints[x];
			    for (j = 0; j < yap; j++)
			      {
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 pix += sow;
			      }
			    r /= yap;
			    g /= yap;
			    b /= yap;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		    }		       
	       }
	     else
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_XAP;
			    g = G_VAL(pix) * INV_XAP;
			    b = B_VAL(pix) * INV_XAP;
			    pix++;
			    r += R_VAL(pix) * XAP;
			    g += G_VAL(pix) * XAP;
			    b += B_VAL(pix) * XAP;
			    r >>= 8;
			    g >>= 8;
			    b >>= 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			  *dptr++ = sptr[xpoints[x]];		      
		    }
	       }
	  }
     }
#endif
   /* if we're scaling down horizontally */
   else if ((!xup) && (yup))
#ifndef OLD_SCALE_DOWN
     {
	/*\ 'Correct' version, with math units prepared for MMXification \*/
	int Cx, j;
	DATA32 *pix;
	int r, g, b, rr, gg, bb;
	int xap;

	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  Cx = XAP >> 16;
		  xap = XAP & 0xffff;

		  pix = ypoints[dyy + y] + xpoints[x];
		  r = (R_VAL(pix) * xap) >> 10;
		  g = (G_VAL(pix) * xap) >> 10;
		  b = (B_VAL(pix) * xap) >> 10;
		  pix++;
		  for (j = (1 << 14) - xap; j > Cx; j -= Cx)
		    {
		       r += (R_VAL(pix) * Cx) >> 10;
		       g += (G_VAL(pix) * Cx) >> 10;
		       b += (B_VAL(pix) * Cx) >> 10;
		       pix++;
		    }
		  if (j > 0)
		    {
		       r += (R_VAL(pix) * j) >> 10;
		       g += (G_VAL(pix) * j) >> 10;
		       b += (B_VAL(pix) * j) >> 10;
		    }
		  if (YAP > 0)
		    {
		       pix = ypoints[dyy + y] + xpoints[x] + sow;
		       rr = (R_VAL(pix) * xap) >> 10;
		       gg = (G_VAL(pix) * xap) >> 10;
		       bb = (B_VAL(pix) * xap) >> 10;
		       pix++;
		       for (j = (1 << 14) - xap; j > Cx; j -= Cx)
			 {
			    rr += (R_VAL(pix) * Cx) >> 10;
			    gg += (G_VAL(pix) * Cx) >> 10;
			    bb += (B_VAL(pix) * Cx) >> 10;
			    pix++;
			 }
		       if (j > 0)
			 {
			    rr += (R_VAL(pix) * j) >> 10;
			    gg += (G_VAL(pix) * j) >> 10;
			    bb += (B_VAL(pix) * j) >> 10;
			 }
		       r = r * INV_YAP;
		       g = g * INV_YAP;
		       b = b * INV_YAP;
		       r = (r + ((rr * YAP))) >> 12;
		       g = (g + ((gg * YAP))) >> 12;
		       b = (b + ((bb * YAP))) >> 12;
		    }
		  else
		    {
		       r >>= 4;
		       g >>= 4;
		       b >>= 4;
		    }
		  *dptr = RGBA_COMPOSE(r, g, b, 0xff);
		  dptr++;
	       }		       
	  }
     }
#else
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     if (YAP > 0)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0;
		       int rr = 0, gg = 0, bb = 0;
		       int xap;
		       DATA32 *pix;

		       xap = xpoints[x + 1] - xpoints[x];
		       if (xap > 1)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    for (i = 0; i < xap; i++)
			      {
				 r += R_VAL(pix + i);
				 g += G_VAL(pix + i);
				 b += B_VAL(pix + i);
			      }
			    r = r * INV_YAP / xap;
			    g = g * INV_YAP / xap;
			    b = b * INV_YAP / xap;
			    pix = ypoints[dyy + y] + xpoints[x] + sow;
			    for (i = 0; i < xap; i++)
			      {
				 rr += R_VAL(pix + i);
				 gg += G_VAL(pix + i);
				 bb += B_VAL(pix + i);
			      }
			    r = (r + ((rr * YAP) / xap)) >> 8;
			    g = (g + ((gg * YAP) / xap)) >> 8;
			    b = (b + ((bb * YAP) / xap)) >> 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    r = R_VAL(pix) * INV_YAP;
			    g = G_VAL(pix) * INV_YAP;
			    b = B_VAL(pix) * INV_YAP;
			    pix += sow;
			    r += R_VAL(pix) * YAP;
			    g += G_VAL(pix) * YAP;
			    b += B_VAL(pix) * YAP;
			    r >>= 8;
			    g >>= 8;
			    b >>= 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		    }
	       }
	     else
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0;
		       int xap;
		       DATA32 *pix;

		       xap = xpoints[x + 1] - xpoints[x];
		       if (xap > 1)
			 {
			    pix = ypoints[dyy + y] + xpoints[x];
			    for (i = 0; i < xap; i++)
			      {
				 r += R_VAL(pix + i);
				 g += G_VAL(pix + i);
				 b += B_VAL(pix + i);
			      }
			    r /= xap;
			    g /= xap;
			    b /= xap;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			  *dptr++ = sptr[xpoints[x]];		      
		    }
	       }
	  }
     }
#endif
   /* fully optimized (i think) - onyl change of algorithm can help */
   /* if we're scaling down horizontally & vertically */
   else
#ifndef OLD_SCALE_DOWN
     {
	/*\ 'Correct' version, with math units prepared for MMXification \*/
	int Cx, Cy, i, j;
	DATA32 *pix;
	int r, g, b, rx, gx, bx;
	int xap, yap;

	for (y = 0; y < dh; y++)
	  {
	     Cy = YAP >> 16;
	     yap = YAP & 0xffff;

	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  Cx = XAP >> 16;
		  xap = XAP & 0xffff;

		  sptr = ypoints[dyy + y] + xpoints[x];
		  pix = sptr;
		  sptr += sow;
		  rx = (R_VAL(pix) * xap) >> 9;
		  gx = (G_VAL(pix) * xap) >> 9;
		  bx = (B_VAL(pix) * xap) >> 9;
		  pix++;
		  for (i = (1 << 14) - xap; i > Cx; i -= Cx)
		    {
		       rx += (R_VAL(pix) * Cx) >> 9;
		       gx += (G_VAL(pix) * Cx) >> 9;
		       bx += (B_VAL(pix) * Cx) >> 9;
		       pix++;
		    }
		  if (i > 0)
		    {
		       rx += (R_VAL(pix) * i) >> 9;
		       gx += (G_VAL(pix) * i) >> 9;
		       bx += (B_VAL(pix) * i) >> 9;
		    }

		  r = (rx * yap) >> 14;
		  g = (gx * yap) >> 14;
		  b = (bx * yap) >> 14;

		  for (j = (1 << 14) - yap; j > Cy; j -= Cy)
		    {
		       pix = sptr;
		       sptr += sow;
		       rx = (R_VAL(pix) * xap) >> 9;
		       gx = (G_VAL(pix) * xap) >> 9;
		       bx = (B_VAL(pix) * xap) >> 9;
		       pix++;
		       for (i = (1 << 14) - xap; i > Cx; i -= Cx)
			 {
			    rx += (R_VAL(pix) * Cx) >> 9;
			    gx += (G_VAL(pix) * Cx) >> 9;
			    bx += (B_VAL(pix) * Cx) >> 9;
			    pix++;
			 }
		       if (i > 0)
			 {
			    rx += (R_VAL(pix) * i) >> 9;
			    gx += (G_VAL(pix) * i) >> 9;
			    bx += (B_VAL(pix) * i) >> 9;
			 }

		       r += (rx * Cy) >> 14;
		       g += (gx * Cy) >> 14;
		       b += (bx * Cy) >> 14;
		    }
		  if (j > 0)
		    {
		       pix = sptr;
		       sptr += sow;
		       rx = (R_VAL(pix) * xap) >> 9;
		       gx = (G_VAL(pix) * xap) >> 9;
		       bx = (B_VAL(pix) * xap) >> 9;
		       pix++;
		       for (i = (1 << 14) - xap; i > Cx; i -= Cx)
			 {
			    rx += (R_VAL(pix) * Cx) >> 9;
			    gx += (G_VAL(pix) * Cx) >> 9;
			    bx += (B_VAL(pix) * Cx) >> 9;
			    pix++;
			 }
		       if (i > 0)
			 {
			    rx += (R_VAL(pix) * i) >> 9;
			    gx += (G_VAL(pix) * i) >> 9;
			    bx += (B_VAL(pix) * i) >> 9;
			 }
     
		       r += (rx * j) >> 14;
		       g += (gx * j) >> 14;
		       b += (bx * j) >> 14;
		    }

		  R_VAL(dptr) = r >> 5;
		  G_VAL(dptr) = g >> 5;
		  B_VAL(dptr) = b >> 5;
		  dptr++;
	       }
	  }
     }
#else
     {
	int count;
	DATA32 *pix;
	int r, g, b;
	
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     int yap = (ypoints[dyy + y + 1] - ypoints[dyy + y]) / sow;
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     for (x = dxx; x < end; x++)
	       {
		  int xap = xpoints[x + 1] - xpoints[x];
		  if ((xap > 1) || (yap > 1))
		    {
		       r = 0; g = 0; b = 0;
		       pix = sptr + xpoints[x];
		       for (j = yap; --j >= 0; )
			 {
			    for (i = xap; --i >= 0; )
			      {
				 r += R_VAL(pix + i);
				 g += G_VAL(pix + i);
				 b += B_VAL(pix + i);
			      }
			    pix += sow;
			 }
		       count = xap * yap;
		       R_VAL(dptr) = r / count;
		       G_VAL(dptr) = g / count;
		       B_VAL(dptr) = b / count;
		       dptr++;
		    }
		  else
		     *dptr++ = sptr[xpoints[x]];
	       }
	  }
     }
#endif
}

#if defined(DO_MMX_ASM) && defined(__GNUC__)
void
__imlib_Scale_mmx_AARGBA(DATA32 **ypoints, int *xpoints, DATA32 *dest,
	   int *xapoints, int *yapoints, char xup, char yup,
	   int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow)
{
   DATA32 *dptr;
   int x, y, end;
   
   end = dxx + dw;
   /* scaling up both ways */
   if ((xup) && (yup))
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     if (YAP > 0)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       if (XAP > 0)
			 {
			    __asm__ (
			    "pxor %%mm6, %%mm6\n\t"
			    "movd %3, %%mm0\n\t"
			    "movd %4, %%mm1\n\t"
			    "punpcklwd %%mm0, %%mm0\n\t"
			    "punpcklwd %%mm1, %%mm1\n\t"
			    "punpckldq %%mm0, %%mm0\n\t"
			    "punpckldq %%mm1, %%mm1\n\t"
			    "movq (%1), %%mm2\n\t"
			    "movq (%1, %2, 4), %%mm4\n\t"
			    "movq %%mm2, %%mm3\n\t"
			    "movq %%mm4, %%mm5\n\t"
			    "punpcklbw %%mm6, %%mm2\n\t"
			    "punpcklbw %%mm6, %%mm4\n\t"
			    "punpckhbw %%mm6, %%mm3\n\t"
			    "punpckhbw %%mm6, %%mm5\n\t"
			    "psubw %%mm2, %%mm3\n\t"
			    "psubw %%mm4, %%mm5\n\t"
			    "psllw $4, %%mm3\n\t"
			    "psllw $4, %%mm5\n\t"
			    "pmulhw %%mm0, %%mm3\n\t"
			    "pmulhw %%mm0, %%mm5\n\t"
			    "paddw %%mm2, %%mm3\n\t"
			    "paddw %%mm4, %%mm5\n\t"
			    "psubw %%mm3, %%mm5\n\t"
			    "psllw $4, %%mm5\n\t"
			    "pmulhw %%mm1, %%mm5\n\t"
			    "paddw %%mm3, %%mm5\n\t"
			    "packuswb %%mm5, %%mm5\n\t"
			    "movd %%mm5, (%0)"
			    : /*\ No outputs \*/
			    : "r" (dptr), "r" (ypoints[dyy + y] + xpoints[x]),
			      "r" (sow), "g" ((XAP) << 4),
			      "g" ((YAP) << 4));
			 }
		       else
			 {
			    __asm__ (
			    "pxor %%mm6, %%mm6\n\t"
			    "movd %3, %%mm0\n\t"
			    "punpcklwd %%mm0, %%mm0\n\t"
			    "punpckldq %%mm0, %%mm0\n\t"
			    "movd (%1), %%mm2\n\t"
			    "movd (%1, %2, 4), %%mm4\n\t"
			    "punpcklbw %%mm6, %%mm2\n\t"
			    "punpcklbw %%mm6, %%mm4\n\t"
			    "psubw %%mm2, %%mm4\n\t"
			    "psllw $4, %%mm4\n\t"
			    "pmulhw %%mm0, %%mm4\n\t"
			    "paddw %%mm2, %%mm4\n\t"
			    "packuswb %%mm4, %%mm4\n\t"
			    "movd %%mm4, (%0)"
			    : /*\ No outputs \*/
			    : "r" (dptr), "r" (ypoints[dyy + y] + xpoints[x]),
			      "r" (sow), "g" ((YAP) << 4));
			 }
		       dptr++;
		    }
	       }
	     else
	       {
		  for (x = dxx; x < end; x++)
		    {
		       if (XAP > 0)
			 {
			    __asm__ (
			    "pxor %%mm6, %%mm6\n\t"
			    "movd %2, %%mm0\n\t"
			    "punpcklwd %%mm0, %%mm0\n\t"
			    "punpckldq %%mm0, %%mm0\n\t"
			    "movq (%1), %%mm2\n\t"
			    "movq %%mm2, %%mm4\n\t"
			    "punpcklbw %%mm6, %%mm2\n\t"
			    "punpckhbw %%mm6, %%mm4\n\t"
			    "psubw %%mm2, %%mm4\n\t"
			    "psllw $4, %%mm4\n\t"
			    "pmulhw %%mm0, %%mm4\n\t"
			    "paddw %%mm2, %%mm4\n\t"
			    "packuswb %%mm4, %%mm4\n\t"
			    "movd %%mm4, (%0)"
			    : /*\ No outputs \*/
			    : "r" (dptr), "r" (ypoints[dyy + y] + xpoints[x]),
			      "g" ((XAP) << 4));
			 }
		       else
			  *dptr = *(ypoints[dyy + y] + xpoints[x]);
		       dptr++;
		    }
	       }
	  }
     }
   /* if we're scaling down vertically */
   else if ((xup) && (!yup))
     {
	int Cy, j;
	DATA32 *pix;
	int r, g, b, a, rr, gg, bb, aa;
	int yap;
		 
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     Cy = YAP >> 16;
	     yap = YAP & 0xffff;

	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  __asm__ (
		  "movd %4, %%mm4\n\t"		/*\ Cy \*/
		  "punpcklwd %%mm4, %%mm4\n\t"
		  "punpckldq %%mm4, %%mm4\n\t"
		  "movd %5, %%mm5\n\t"		/*\ yap \*/
		  "punpcklwd %%mm5, %%mm5\n\t"
		  "punpckldq %%mm5, %%mm5\n\t"
		  "pxor %%mm7, %%mm7\n\t"
		  "\n\t"
		  "movl %0, %%eax\n\t"		/*\ p \*/
		  "movd (%%eax), %%mm0\n\t"	/*\ v = (*p * yap) >> 10 \*/
		  "addl %1, %%eax\n\t"		/*\ p += sow \*/
		  "punpcklbw %%mm7, %%mm0\n\t"
		  "psllw $6, %%mm0\n\t"
		  "pmulhw %%mm5, %%mm0\n\t"
		  "\n\t"
		  "movl $0x4000, %%ecx\n\t"	/*\ i = 0x4000 - yap \*/
		  "subl %5, %%ecx\n\t"
		  "jbe 5f\n\t"			/*\ i <= 0: Skip it \*/
		  "jmp 2f\n"
		  "1:\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ v += (*p * Cy) >> 10 \*/
		  "addl %1, %%eax\n\t"		/*\ p += sow \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $6, %%mm1\n\t"
		  "pmulhw %%mm4, %%mm1\n\t"
		  "paddw %%mm1, %%mm0\n\t"
		  "\n\t"
		  "subl %4, %%ecx\n"		/*\ i -= Cy; while i > Cy \*/
		  "2:\n\t"
		  "cmpl %4, %%ecx\n\t"
		  "jg 1b\n\t"
		  "\n\t"
		  "movd %%ecx, %%mm6\n\t"	/*\ i \*/
		  "punpcklwd %%mm6, %%mm6\n\t"
		  "punpckldq %%mm6, %%mm6\n\t"
		  "\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ v += (*p * i) >> 10 \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $6, %%mm1\n\t"
		  "pmulhw %%mm6, %%mm1\n\t"
		  "paddw %%mm1, %%mm0\n"
		  "5:\n\t"
		  "movl %3, %%eax\n\t"
		  "sall $5, %%eax\n\t"
		  "jz 6f\n\t"
		  "movd %%eax, %%mm3\n\t"	/*\ XAP << 5 \*/
		  "punpcklwd %%mm3, %%mm3\n\t"
		  "punpckldq %%mm3, %%mm3\n\t"
		  "\n\t"
		  "movl %0, %%eax\n\t"		/*\ p + 1 \*/
		  "addl $4, %%eax\n\t"
		  "movd (%%eax), %%mm2\n\t"	/*\ vv = (*p * yap) >> 10 \*/
		  "addl %1, %%eax\n\t"		/*\ p += sow \*/
		  "punpcklbw %%mm7, %%mm2\n\t"
		  "psllw $6, %%mm2\n\t"
		  "pmulhw %%mm5, %%mm2\n\t"
		  "\n\t"
		  "movl $0x4000, %%ecx\n\t"	/*\ i = 0x4000 - yap \*/
		  "subl %5, %%ecx\n\t"
		  "jbe 5f\n\t"			/*\ i <= 0: Skip it \*/
		  "jmp 2f\n"
		  "1:\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ vv += (*p * Cy) >> 10 \*/
		  "addl %1, %%eax\n\t"		/*\ p += sow \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $6, %%mm1\n\t"
		  "pmulhw %%mm4, %%mm1\n\t"
		  "paddw %%mm1, %%mm2\n\t"
		  "\n\t"
		  "subl %4, %%ecx\n"		/*\ i -= Cy; while i > Cy \*/
		  "2:\n\t"
		  "cmpl %4, %%ecx\n\t"
		  "jg 1b\n\t"
		  "\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ vv += (*p * i) >> 10 \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $6, %%mm1\n\t"
		  "pmulhw %%mm6, %%mm1\n\t"
		  "paddw %%mm1, %%mm2\n"
		  "5:\n\t"
		  "psubw %%mm0, %%mm2\n\t"	/*\ v += (vv - v) * XAP \*/
		  "psllw $3, %%mm2\n\t"
		  "pmulhw %%mm3, %%mm2\n\t"
		  "paddw %%mm2, %%mm0\n"
		  "6:\n\t"
		  "psrlw $4, %%mm0\n\t"
		  "packuswb %%mm0, %%mm0\n\t"
		  "movl %2, %%eax\n\t"
		  "movd %%mm0, (%%eax)"
		  : /*\ No outputs \*/
		  : "r" (ypoints[dyy + y] + xpoints[x]), "g" (sow * 4),
		    "g" (dptr), "g" (XAP), "g" (Cy), "g" (yap)
		  : "ax", "cx");
		  dptr++;
	       }
	  }
     }
   /* if we're scaling down horizontally */
   else if ((!xup) && (yup))
     {
	int Cx, j;
	DATA32 *pix;
	int r, g, b, a, rr, gg, bb, aa;
	int xap;

	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  Cx = XAP >> 16;
		  xap = XAP & 0xffff;

		  __asm__ (
		  "movd %4, %%mm4\n\t"		/*\ Cx \*/
		  "punpcklwd %%mm4, %%mm4\n\t"
		  "punpckldq %%mm4, %%mm4\n\t"
		  "movd %5, %%mm5\n\t"		/*\ xap \*/
		  "punpcklwd %%mm5, %%mm5\n\t"
		  "punpckldq %%mm5, %%mm5\n\t"
		  "pxor %%mm7, %%mm7\n\t"
		  "\n\t"
		  "movl %0, %%eax\n\t"		/*\ p \*/
		  "movd (%%eax), %%mm0\n\t"	/*\ v = (*p * xap) >> 10 \*/
		  "addl $4, %%eax\n\t"		/*\ p += sow \*/
		  "punpcklbw %%mm7, %%mm0\n\t"
		  "psllw $6, %%mm0\n\t"
		  "pmulhw %%mm5, %%mm0\n\t"
		  "\n\t"
		  "movl $0x4000, %%ecx\n\t"	/*\ i = 0x4000 - xap \*/
		  "subl %5, %%ecx\n\t"
		  "jbe 5f\n\t"			/*\ i <= 0: Skip it \*/
		  "jmp 2f\n"
		  "1:\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ v += (*p * Cx) >> 10 \*/
		  "addl $4, %%eax\n\t"		/*\ p += sow \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $6, %%mm1\n\t"
		  "pmulhw %%mm4, %%mm1\n\t"
		  "paddw %%mm1, %%mm0\n\t"
		  "\n\t"
		  "subl %4, %%ecx\n"		/*\ i -= Cx; while i > Cx \*/
		  "2:\n\t"
		  "cmpl %4, %%ecx\n\t"
		  "jg 1b\n\t"
		  "\n\t"
		  "movd %%ecx, %%mm6\n\t"	/*\ i \*/
		  "punpcklwd %%mm6, %%mm6\n\t"
		  "punpckldq %%mm6, %%mm6\n\t"
		  "\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ v += (*p * i) >> 10 \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $6, %%mm1\n\t"
		  "pmulhw %%mm6, %%mm1\n\t"
		  "paddw %%mm1, %%mm0\n"
		  "5:\n\t"
		  "movl %3, %%eax\n\t"
		  "sall $5, %%eax\n\t"
		  "jz 6f\n\t"
		  "movd %%eax, %%mm3\n\t"	/*\ YAP << 5 \*/
		  "punpcklwd %%mm3, %%mm3\n\t"
		  "punpckldq %%mm3, %%mm3\n\t"
		  "\n\t"
		  "movl %0, %%eax\n\t"		/*\ p + 1 \*/
		  "addl %1, %%eax\n\t"
		  "movd (%%eax), %%mm2\n\t"	/*\ vv = (*p * xap) >> 10 \*/
		  "addl $4, %%eax\n\t"		/*\ p += sow \*/
		  "punpcklbw %%mm7, %%mm2\n\t"
		  "psllw $6, %%mm2\n\t"
		  "pmulhw %%mm5, %%mm2\n\t"
		  "\n\t"
		  "movl $0x4000, %%ecx\n\t"	/*\ i = 0x4000 - xap \*/
		  "subl %5, %%ecx\n\t"
		  "jbe 5f\n\t"			/*\ i <= 0: Skip it \*/
		  "jmp 2f\n"
		  "1:\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ vv += (*p * Cx) >> 10 \*/
		  "addl $4, %%eax\n\t"		/*\ p += sow \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $6, %%mm1\n\t"
		  "pmulhw %%mm4, %%mm1\n\t"
		  "paddw %%mm1, %%mm2\n\t"
		  "\n\t"
		  "subl %4, %%ecx\n"		/*\ i -= Cx; while i > Cx \*/
		  "2:\n\t"
		  "cmpl %4, %%ecx\n\t"
		  "jg 1b\n\t"
		  "\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ vv += (*p * i) >> 10 \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $6, %%mm1\n\t"
		  "pmulhw %%mm6, %%mm1\n\t"
		  "paddw %%mm1, %%mm2\n"
		  "5:\n\t"
		  "psubw %%mm0, %%mm2\n\t"	/*\ v += (vv - v) * XAP \*/
		  "psllw $3, %%mm2\n\t"
		  "pmulhw %%mm3, %%mm2\n\t"
		  "paddw %%mm2, %%mm0\n"
		  "6:\n\t"
		  "psrlw $4, %%mm0\n\t"
		  "packuswb %%mm0, %%mm0\n\t"
		  "movl %2, %%eax\n\t"
		  "movd %%mm0, (%%eax)"
		  : /*\ No outputs \*/
		  : "r" (ypoints[dyy + y] + xpoints[x]), "g" (sow * 4),
		    "g" (dptr), "g" (YAP), "g" (Cx), "g" (xap)
		  : "ax", "cx");
		  dptr++;
	       }
	  }
     }
   /* if we're scaling down horizontally & vertically */
   else
     {
	int Cx, Cy, xap, yap;

	for (y = 0; y < dh; y++)
	  {
	     Cy = YAP >> 16;
	     yap = YAP & 0xffff;

	     dptr = dest + dx + ((y + dy) * dow);
	     for (x = dxx; x < end; x++)
	       {
		  Cx = XAP >> 16;
		  xap = XAP & 0xffff;

		  __asm__ (
		  "movd %3, %%mm3\n\t"		/*\ Cx \*/
		  "punpcklwd %%mm3, %%mm3\n\t"
		  "punpckldq %%mm3, %%mm3\n\t"
		  "movd %4, %%mm4\n\t"		/*\ Cy \*/
		  "punpcklwd %%mm4, %%mm4\n\t"
		  "punpckldq %%mm4, %%mm4\n\t"
		  "movd %5, %%mm5\n\t"		/*\ xap \*/
		  "punpcklwd %%mm5, %%mm5\n\t"
		  "punpckldq %%mm5, %%mm5\n\t"
		  "pxor %%mm7, %%mm7\n\t"
		  "\n\t"
		  "movl %0, %%esi\n\t"		/*\ sptr \*/
		  "movl %%esi, %%eax\n\t"	/*\ p = sptr \*/
		  "addl %1, %%esi\n\t"		/*\ sptr += sow \*/
		  "movd (%%eax), %%mm0\n\t"	/*\ vx = (*p++ * xap) >> 9 \*/
		  "addl $4, %%eax\n\t"
		  "punpcklbw %%mm7, %%mm0\n\t"
		  "psllw $7, %%mm0\n\t"
		  "pmulhw %%mm5, %%mm0\n\t"
		  "\n\t"
		  "movl $0x4000, %%ecx\n\t"	/*\ i = 0x4000 - xap \*/
		  "subl %5, %%ecx\n\t"
		  "jbe 5f\n\t"			/*\ i <= 0: Skip it \*/
		  "jmp 2f\n"
		  "1:\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ vx += (*p++ * Cx) >> 9 \*/
		  "addl $4, %%eax\n\t"
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $7, %%mm1\n\t"
		  "pmulhw %%mm3, %%mm1\n\t"
		  "paddw %%mm1, %%mm0\n\t"
		  "\n\t"
		  "subl %3, %%ecx\n"		/*\ i -= Cx; while i > Cx \*/
		  "2:\n\t"
		  "cmpl %3, %%ecx\n\t"
		  "jg 1b\n\t"
		  "\n\t"
		  "movd %%ecx, %%mm6\n\t"	/*\ i \*/
		  "punpcklwd %%mm6, %%mm6\n\t"
		  "punpckldq %%mm6, %%mm6\n\t"
		  "\n\t"
		  "movd (%%eax), %%mm1\n\t"	/*\ vx += (*p * i) >> 9 \*/
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $7, %%mm1\n\t"
		  "pmulhw %%mm6, %%mm1\n\t"
		  "paddw %%mm1, %%mm0\n"
		  "5:\n\t"
		  "movd %6, %%mm2\n\t"		/*\ yap \*/
		  "punpcklwd %%mm2, %%mm2\n\t"
		  "punpckldq %%mm2, %%mm2\n\t"
		  "psllw $2, %%mm0\n\t"		/*\ v = (vx * yap) >> 14 \*/
		  "pmulhw %%mm2, %%mm0\n\t"
		  "\n\t"
		  "movl $0x4000, %%edx\n\t"	/*\ j = 0x4000 - yap \*/
		  "subl %6, %%edx\n\t"
		  "jbe 6f\n\t"			/*\ j <= 0: Skip it \*/
		  "jmp 4f\n"
		  "3:\n\t"
		  "movl %%esi, %%eax\n\t"	/*\ p = sptr \*/
		  "addl %1, %%esi\n\t"		/*\ sptr += sow \*/
		  "movd (%%eax), %%mm1\n\t"	/*\ vx = (*p++ * xap) >> 9 \*/
		  "addl $4, %%eax\n\t"
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $7, %%mm1\n\t"
		  "pmulhw %%mm5, %%mm1\n\t"
		  "\n\t"
		  "movl $0x4000, %%ecx\n\t"	/*\ i = 0x4000 - xap \*/
		  "subl %5, %%ecx\n\t"
		  "jbe 5f\n\t"			/*\ i <= 0: Skip it \*/
		  "jmp 2f\n"
		  "1:\n\t"
		  "movd (%%eax), %%mm2\n\t"	/*\ vx += (*p++ * Cx) >> 9 \*/
		  "addl $4, %%eax\n\t"
		  "punpcklbw %%mm7, %%mm2\n\t"
		  "psllw $7, %%mm2\n\t"
		  "pmulhw %%mm3, %%mm2\n\t"
		  "paddw %%mm2, %%mm1\n\t"
		  "\n\t"
		  "subl %3, %%ecx\n"		/*\ i -= Cx; while i > Cx \*/
		  "2:\n\t"
		  "cmpl %3, %%ecx\n\t"
		  "jg 1b\n\t"
		  "\n\t"
		  "movd (%%eax), %%mm2\n\t"	/*\ vx += (*p * i) >> 9 \*/
		  "punpcklbw %%mm7, %%mm2\n\t"
		  "psllw $7, %%mm2\n\t"
		  "pmulhw %%mm6, %%mm2\n\t"
		  "paddw %%mm2, %%mm1\n"
		  "5:\n\t"
		  "psllw $2, %%mm1\n\t"		/*\ v += (vx * Cy) >> 14 \*/
		  "pmulhw %%mm4, %%mm1\n\t"
		  "paddw %%mm1, %%mm0\n\t"
		  "\n\t"
		  "subl %4, %%edx\n"		/*\ j -= Cy; while j > Cy \*/
		  "4:\n\t"
		  "cmpl %4, %%edx\n\t"
		  "jg 3b\n\t"
		  "\n\t"
		  "movl %%esi, %%eax\n\t"	/*\ p = sptr \*/
		  "movd (%%eax), %%mm1\n\t"	/*\ vx = (*p++ * xap) >> 9 \*/
		  "addl $4, %%eax\n\t"
		  "punpcklbw %%mm7, %%mm1\n\t"
		  "psllw $7, %%mm1\n\t"
		  "pmulhw %%mm5, %%mm1\n\t"
		  "\n\t"
		  "movl $0x4000, %%ecx\n\t"	/*\ i = 0x4000 - xap \*/
		  "subl %5, %%ecx\n\t"
		  "jbe 5f\n\t"			/*\ i <= 0: Skip it \*/
		  "jmp 2f\n"
		  "1:\n\t"
		  "movd (%%eax), %%mm2\n\t"	/*\ vx += (*p++ * Cx) >> 9 \*/
		  "addl $4, %%eax\n\t"
		  "punpcklbw %%mm7, %%mm2\n\t"
		  "psllw $7, %%mm2\n\t"
		  "pmulhw %%mm3, %%mm2\n\t"
		  "paddw %%mm2, %%mm1\n\t"
		  "\n\t"
		  "subl %3, %%ecx\n"		/*\ i -= Cx; while i > Cx \*/
		  "2:\n\t"
		  "cmpl %3, %%ecx\n\t"
		  "jg 1b\n\t"
		  "\n\t"
		  "movd (%%eax), %%mm2\n\t"	/*\ vx += (*p * i) >> 9 \*/
		  "punpcklbw %%mm7, %%mm2\n\t"
		  "psllw $7, %%mm2\n\t"
		  "pmulhw %%mm6, %%mm2\n\t"
		  "paddw %%mm2, %%mm1\n"
		  "5:\n\t"
		  "movd %%edx, %%mm6\n\t"	/*\ j \*/
		  "punpcklwd %%mm6, %%mm6\n\t"
		  "punpckldq %%mm6, %%mm6\n\t"
		  "\n\t"
		  "psllw $2, %%mm1\n\t"		/*\ v += (vx * j) >> 14 \*/
		  "pmulhw %%mm6, %%mm1\n\t"
		  "paddw %%mm1, %%mm0\n"
		  "6:\n\t"
		  "psrlw $5, %%mm0\n\t"		/*\ *dest = v >> 5 \*/
		  "packuswb %%mm0, %%mm0\n\t"
		  "movl %2, %%eax\n\t"
		  "movd %%mm0, (%%eax)\n\t"
		  : /*\ No outputs \*/
		  : "g" (ypoints[dyy + y] + xpoints[x]), "g" (sow * 4),
		    "g" (dptr), "g" (Cx), "g" (Cy), "g" (xap), "g" (yap)
		  : "si", "ax", "cx", "dx");
		  dptr++;
	       }
	  }
     }
   __asm__ ("emms" : : );
}
#else
void
__imlib_Scale_mmx_AARGBA(DATA32 **ypoints, int *xpoints, DATA32 *dest,
	   int *xapoints, int *yapoints, char xup, char yup,
	   int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow)
{
   __imlib_ScaleAARGBA(ypoints, xpoints, dest, xapoints, yapoints, xup, yup,
		       dxx, dyy, dx, dy, dw, dh, dow, sow);
}
#endif
