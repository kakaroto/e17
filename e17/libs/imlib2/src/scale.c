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

#if defined(DO_MMX_ASM) && defined(__GNUC__)
/*\ MMX asm versions. TODO: insn order for pairing on PMMX \*/
#define INTERP_ARGB_XY(dest, src, sow, x, y) __asm__ (\
	"pxor %%mm6, %%mm6\n\t"			\
	"movd %3, %%mm0\n\t"			\
	"movd %4, %%mm1\n\t"			\
	"punpcklwd %%mm0, %%mm0\n\t"		\
	"punpcklwd %%mm1, %%mm1\n\t"		\
	"punpckldq %%mm0, %%mm0\n\t"		\
	"punpckldq %%mm1, %%mm1\n\t"		\
	"movq (%1), %%mm2\n\t"			\
	"movq (%1, %2, 4), %%mm4\n\t"		\
	"movq %%mm2, %%mm3\n\t"			\
	"movq %%mm4, %%mm5\n\t"			\
	"punpcklbw %%mm6, %%mm2\n\t"		\
	"punpcklbw %%mm6, %%mm4\n\t"		\
	"punpckhbw %%mm6, %%mm3\n\t"		\
	"punpckhbw %%mm6, %%mm5\n\t"		\
	"psubw %%mm2, %%mm3\n\t"		\
	"psubw %%mm4, %%mm5\n\t"		\
	"psllw %5, %%mm3\n\t"			\
	"psllw %5, %%mm5\n\t"			\
	"pmulhw %%mm0, %%mm3\n\t"		\
	"pmulhw %%mm0, %%mm5\n\t"		\
	"paddw %%mm2, %%mm3\n\t"		\
	"paddw %%mm4, %%mm5\n\t"		\
	"psubw %%mm3, %%mm5\n\t"		\
	"psllw %5, %%mm5\n\t"			\
	"pmulhw %%mm1, %%mm5\n\t"		\
	"paddw %%mm3, %%mm5\n\t"		\
	"packuswb %%mm5, %%mm5\n\t"		\
	"movd %%mm5, (%0)"			\
	: /*\ No outputs \*/			\
	: "r" ((dest)), "r" ((src)), "r" ((sow)),	\
	  "g" ((x) << 4), "g" ((y) << 4), "I" (16 - 12))

#define INTERP_ARGB_Y(dest, src, sow, y) __asm__ (\
	"pxor %%mm6, %%mm6\n\t"			\
	"movd %3, %%mm0\n\t"			\
	"punpcklwd %%mm0, %%mm0\n\t"		\
	"punpckldq %%mm0, %%mm0\n\t"		\
	"movd (%1), %%mm2\n\t"			\
	"movd (%1, %2, 4), %%mm4\n\t"		\
	"punpcklbw %%mm6, %%mm2\n\t"		\
	"punpcklbw %%mm6, %%mm4\n\t"		\
	"psubw %%mm2, %%mm4\n\t"		\
	"psllw %4, %%mm4\n\t"			\
	"pmulhw %%mm0, %%mm4\n\t"		\
	"paddw %%mm2, %%mm4\n\t"		\
	"packuswb %%mm4, %%mm4\n\t"		\
	"movd %%mm4, (%0)"			\
	: /*\ No outputs \*/			\
	: "r" ((dest)), "r" ((src)), "r" ((sow)),	\
	  "g" ((y) << 4), "I" (16 - 12))

#define EMMS() __asm__ ("emms" : : )
#endif

DATA32 **
__imlib_CalcYPoints(DATA32 *src, int sw, int sh, int dh, int b1, int b2)
{
   DATA32 **p;
   int i, j = 0;
   int val, inc;
   
   p = malloc(dh * sizeof(DATA32 *));
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
   for (i = 0; i < b2; i++)
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
   
   p = malloc(dw * sizeof(int));
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
   for (i = 0; i < b2; i++)
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
	for (i = 0; i < b1; i++)
	   p[j++] = 1;
	if (d > (b1 + b2))
	  {
	     int ss, dd;
	     
	     ss = s - b1 - b2;
	     dd = d - b1 - b2;
	     for (i = 0; i < dd; i++)
	       {
		  v = (((i + 1) * ss) / dd) - ((i * ss) / dd);
		  if (v != 1)
		    {
		       if (((((i + 1) * ss) / dd) + b1) >= s)
			  v = s - (((i * ss) / dd) + b1) - 1;
		       p[j++] = v;
		    }
		  else
		     p[j++] = v;
		  if (p[j - 1] < 1)
		     p[j - 1] = 1;
	       }
	  }
	for (i = 0; i < b2; i++)
	   p[j++] = 1;
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
   DATA32 *sptr, *ssptr, *dptr;
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
#ifdef INTERP_ARGB_XY
			    INTERP_ARGB_XY(dptr, ypoints[dyy + y] + xpoints[x],
					   sow, XAP, YAP);
			    dptr++;
#else
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
#endif
			 }
		       else
			 {
#ifdef INTERP_ARGB_Y
			    INTERP_ARGB_Y(dptr, ypoints[dyy + y] + xpoints[x],
					  sow, YAP);
			    dptr++;
#else
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
#endif
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
#ifdef INTERP_ARGB_Y
			    INTERP_ARGB_Y(dptr, ypoints[dyy + y] + xpoints[x],
					  1, XAP);
			    dptr++;
#else
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
#endif
			 }
		       else
			  *dptr++ = sptr[xpoints[x] ];
		    }
	       }
	  }
     }
   /* if we're scaling down vertically */
   else if ((xup) && (!yup))
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     if (YAP > 1)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0, a = 0;
		       int rr = 0, gg = 0, bb = 0, aa = 0;
		       int count;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    for (j = 0; j < YAP; j++)
			      {
				 ssptr = ypoints[dyy + y] + (j * sow);
				 pix = &ssptr[xpoints[x]];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 a += A_VAL(pix);
			      }
			    count = j;
			    r = r * INV_XAP / count;
			    g = g * INV_XAP / count;
			    b = b * INV_XAP / count;
			    a = a * INV_XAP / count;
			    for (j = 0; j < YAP; j++)
			      {
				 ssptr = ypoints[dyy + y] + (j * sow);
				 pix = &ssptr[xpoints[x] + 1];
				 rr += R_VAL(pix);
				 gg += G_VAL(pix);
				 bb += B_VAL(pix);
				 aa += A_VAL(pix);
			      }
			    count = j;
			    r = (r + ((rr * XAP) / count)) >> 8;
			    g = (g + ((gg * XAP) / count)) >> 8;
			    b = (b + ((bb * XAP) / count)) >> 8;
			    a = (a + ((aa * XAP) / count)) >> 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			 {
			    for (j = 0; j < YAP; j++)
			      {
				 ssptr = ypoints[dyy + y] + (j *sow);
				 pix = &ssptr[xpoints[x]];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 a += A_VAL(pix);
			      }
			    count = j;
			    r /= count;
			    g /= count;
			    b /= count;
			    a /= count;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		    }		       
	       }
	     else
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0, a = 0;
		       int rr = 0, gg = 0, bb = 0, aa = 0;
		       int count;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
   /* if we're scaling down horizontally */
   else if ((!xup) && (yup))
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
		       int count;
		       DATA32 *pix;
		  
		       if (XAP > 1)
			 {
			    ssptr = ypoints[dyy + y];
			    for (i = 0; i < XAP; i++)
			      {
				 pix = &ssptr[xpoints[x] + i];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 a += A_VAL(pix);
			      }
			    count = i;
			    r = r * INV_YAP / count;
			    g = g * INV_YAP / count;
			    b = b * INV_YAP / count;
			    a = a * INV_YAP / count;
			    ssptr = ypoints[dyy + y] + sow;
			    for (i = 0; i < XAP; i++)
			      {
				 pix = &ssptr[xpoints[x] + i];
				 rr += R_VAL(pix);
				 gg += G_VAL(pix);
				 bb += B_VAL(pix);
				 aa += A_VAL(pix);
			      }
			    count = i;
			    r = (r + ((rr * YAP) / count)) >> 8;
			    g = (g + ((gg * YAP) / count)) >> 8;
			    b = (b + ((bb * YAP) / count)) >> 8;
			    a = (a + ((aa * YAP) / count)) >> 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			 {
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
		       int rr = 0, gg = 0, bb = 0, aa = 0;
		       int count;
		       DATA32 *pix;
		  
		       if (XAP > 1)
			 {
			    ssptr = ypoints[dyy + y];
			    for (i = 0; i < XAP; i++)
			      {
				 pix = &ssptr[xpoints[x] + i];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 a += A_VAL(pix);
			      }
			    count = i;
			    r /= count;
			    g /= count;
			    b /= count;
			    a /= count;
			    *dptr++ = RGBA_COMPOSE(r, g, b, a);
			 }
		       else
			  *dptr++ = sptr[xpoints[x]];		      
		    }
	       }
	  }
     }
   /* if we're scaling down horizontally & vertically */
   else
     {
	int count;
	DATA32 *pix;
	int r, g, b, a;
	int xp, xap, yap;
	
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     for (x = dxx; x < end; x++)
	       {
		  if ((XAP > 1) || (YAP > 1))
		    {
		       r = 0; g = 0; b = 0; a = 0; count = 0;
		       xp = xpoints[x];
		       ssptr = ypoints[dyy + y];
		       for (j = 0; j < YAP; j++)
			 {
			    for (i = 0; i < XAP; i++)
			      {
				 pix = &ssptr[xp + i];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
				 a += A_VAL(pix);
			      }
			    count += i;
			    ssptr += sow;
			 }
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
#ifdef EMMS
     EMMS();
#endif
}

/* scale by area sampling - IGNORE the ALPHA byte*/
void
__imlib_ScaleAARGB(DATA32 **ypoints, int *xpoints, DATA32 *dest,
		   int *xapoints, int *yapoints, char xup, char yup,
		   int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow)
{
   DATA32 *sptr, *ssptr, *dptr;
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
#ifdef INTERP_ARGB_XY
			    INTERP_ARGB_XY(dptr, ypoints[dyy + y] + xpoints[x],
					   sow, XAP, YAP);
			    dptr++;
#else
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
#endif
			 }
		       else
			 {
#ifdef INTERP_ARGB_Y
			    INTERP_ARGB_Y(dptr, ypoints[dyy + y] + xpoints[x],
					  sow, YAP);
			    dptr++;
#else
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
#endif
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
#ifdef INTERP_ARGB_Y
			    INTERP_ARGB_Y(dptr, ypoints[dyy + y] + xpoints[x],
					  1, XAP);
			    dptr++;
#else
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
#endif
			 }
		       else
			  *dptr++ = sptr[xpoints[x] ];
		    }
	       }
	  }
     }
   /* if we're scaling down vertically */
   else if ((xup) && (!yup))
     {
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     if (YAP > 1)
	       {
		  for (x = dxx; x < end; x++)
		    {
		       int r = 0, g = 0, b = 0;
		       int rr = 0, gg = 0, bb = 0;
		       int count;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    for (j = 0; j < YAP; j++)
			      {
				 ssptr = ypoints[dyy + y] + (j * sow);
				 pix = &ssptr[xpoints[x]];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
			      }
			    count = j;
			    r = r * INV_XAP / count;
			    g = g * INV_XAP / count;
			    b = b * INV_XAP / count;
			    for (j = 0; j < YAP; j++)
			      {
				 ssptr = ypoints[dyy + y] + (j * sow);
				 pix = &ssptr[xpoints[x] + 1];
				 rr += R_VAL(pix);
				 gg += G_VAL(pix);
				 bb += B_VAL(pix);
			      }
			    count = j;
			    r = (r + ((rr * XAP) / count)) >> 8;
			    g = (g + ((gg * XAP) / count)) >> 8;
			    b = (b + ((bb * XAP) / count)) >> 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			 {
			    for (j = 0; j < YAP; j++)
			      {
				 ssptr = ypoints[dyy + y] + (j *sow);
				 pix = &ssptr[xpoints[x]];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
			      }
			    count = j;
			    r /= count;
			    g /= count;
			    b /= count;
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
		       int count;
		       DATA32 *pix;
		       
		       if (XAP > 0)
			 {
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
   /* if we're scaling down horizontally */
   else if ((!xup) && (yup))
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
		       int count;
		       DATA32 *pix;

		       if (XAP > 1)
			 {
			    ssptr = ypoints[dyy + y];
			    for (i = 0; i < XAP; i++)
			      {
				 pix = &ssptr[xpoints[x] + i];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
			      }
			    count = i;
			    r = r * INV_YAP / count;
			    g = g * INV_YAP / count;
			    b = b * INV_YAP / count;
			    ssptr = ypoints[dyy + y] + sow;
			    for (i = 0; i < XAP; i++)
			      {
				 pix = &ssptr[xpoints[x] + i];
				 rr += R_VAL(pix);
				 gg += G_VAL(pix);
				 bb += B_VAL(pix);
			      }
			    count = i;
			    r = (r + ((rr * YAP) / count)) >> 8;
			    g = (g + ((gg * YAP) / count)) >> 8;
			    b = (b + ((bb * YAP) / count)) >> 8;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			 {
			    ssptr = ypoints[dyy + y];
			    pix = &ssptr[xpoints[x]];
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
		       int count;
		       DATA32 *pix;

		       if (XAP > 1)
			 {
			    ssptr = ypoints[dyy + y];
			    for (i = 0; i < XAP; i++)
			      {
				 pix = &ssptr[xpoints[x] + i];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
			      }
			    count = i;
			    r /= count;
			    g /= count;
			    b /= count;
			    *dptr++ = RGBA_COMPOSE(r, g, b, 0xff);
			 }
		       else
			  *dptr++ = sptr[xpoints[x]];		      
		    }
	       }
	  }
     }
   /* fully optimized (i think) - onyl change of algorithm can help */
   /* if we're scaling down horizontally & vertically */
   else
     {
	int count;
	DATA32 *pix;
	int r, g, b;
	int xp, xap, yap;
	
	/* go through every scanline in the output buffer */
	for (y = 0; y < dh; y++)
	  {
	     /* calculate the source line we'll scan from */
	     dptr = dest + dx + ((y + dy) * dow);
	     sptr = ypoints[dyy + y];
	     for (x = dxx; x < end; x++)
	       {
		  if ((XAP > 1) || (YAP > 1))
		    {
		       r = 0; g = 0; b = 0; count = 0;
		       xp = xpoints[x];
		       ssptr = ypoints[dyy + y];
		       for (j = 0; j < YAP; j++)
			 {
			    for (i = 0; i < XAP; i++)
			      {
				 pix = &ssptr[xp + i];
				 r += R_VAL(pix);
				 g += G_VAL(pix);
				 b += B_VAL(pix);
			      }
			    count += i;
			    ssptr += sow;
			 }
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
#ifdef EMMS
     EMMS();
#endif
}

