#include "common.h"
#include "scale.h"

#define R_VAL(x)                  (((x) & 0x000000ff))
#define G_VAL(x)                  (((x) & 0x0000ff00) >> 8)
#define B_VAL(x)                  (((x) & 0x00ff0000) >> 16)
#define A_VAL(x)                  (((x) & 0xff000000) >> 24)
#define RGBA_COMPOSE(r, g, b, a)  ((a) << 24) | ((b) << 16) | ((g) << 8) | (r)
#define INV_XAP                   (255 - xapoints[x])
#define XAP                       (xapoints[x])
#define INV_YAP                   (255 - yapoints[dyy + y])
#define YAP                       (yapoints[dyy + y])

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
	  for (x = dxx; x < end; x++)
	    {
	      int r = 0, g = 0, b = 0, a = 0;
	      int rr = 0, gg = 0, bb = 0, aa = 0;
	      DATA32 pix;
	      
	      if (YAP > 0)
		{
		  if (XAP > 0)
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_XAP;
		      g = G_VAL(pix) * INV_XAP;
		      b = B_VAL(pix) * INV_XAP;
		      a = A_VAL(pix) * INV_XAP;
		      pix = ssptr[xpoints[x] + 1];
		      r += R_VAL(pix) * XAP;
		      g += G_VAL(pix) * XAP;
		      b += B_VAL(pix) * XAP;
		      a += A_VAL(pix) * XAP;
		      ssptr += sow;
		      pix = ssptr[xpoints[x]];
		      rr = R_VAL(pix) * INV_XAP;
		      gg = G_VAL(pix) * INV_XAP;
		      bb = B_VAL(pix) * INV_XAP;
		      aa = A_VAL(pix) * INV_XAP;
		      pix = ssptr[xpoints[x] + 1];
		      rr += R_VAL(pix) * XAP;
		      gg += G_VAL(pix) * XAP;
		      bb += B_VAL(pix) * XAP;
		      aa += A_VAL(pix) * XAP;
		      r = ((rr * YAP) + (r * INV_YAP)) >> 16;
		      g = ((gg * YAP) + (g * INV_YAP)) >> 16;
		      b = ((bb * YAP) + (b * INV_YAP)) >> 16;
		      a = ((aa * YAP) + (a * INV_YAP)) >> 16;
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
		    }
		  else
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_YAP;
		      g = G_VAL(pix) * INV_YAP;
		      b = B_VAL(pix) * INV_YAP;
		      a = A_VAL(pix) * INV_YAP;
		      ssptr += sow;
		      pix = ssptr[xpoints[x]];
		      r += R_VAL(pix) * YAP;
		      g += G_VAL(pix) * YAP;
		      b += B_VAL(pix) * YAP;
		      a += A_VAL(pix) * YAP;
		      r >>= 8;
		      g >>= 8;
		      b >>= 8;
		      a >>= 8;
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
		    }
		}
	      else
		{
		  if (XAP > 0)
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_XAP;
		      g = G_VAL(pix) * INV_XAP;
		      b = B_VAL(pix) * INV_XAP;
		      a = A_VAL(pix) * INV_XAP;
		      pix = ssptr[xpoints[x] + 1];
		      r += R_VAL(pix) * XAP;
		      g += G_VAL(pix) * XAP;
		      b += B_VAL(pix) * XAP;
		      a += A_VAL(pix) * XAP;
		      r >>= 8;
		      g >>= 8;
		      b >>= 8;
		      a >>= 8;
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
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
	  for (x = dxx; x < end; x++)
	    {
	      int r = 0, g = 0, b = 0, a = 0;
	      int rr = 0, gg = 0, bb = 0, aa = 0;
	      int count;
	      DATA32 pix;
	      
	      if (XAP > 0)
		{
		  if (YAP > 1)
		    {
		      for (j = 0; j < YAP; j++)
			{
			  ssptr = ypoints[dyy + y] + (j * sow);
			  pix = ssptr[xpoints[x]];
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
			  pix = ssptr[xpoints[x] + 1];
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
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
		    }
		  else
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_XAP;
		      g = G_VAL(pix) * INV_XAP;
		      b = B_VAL(pix) *  INV_XAP;
		      a = A_VAL(pix) *       INV_XAP;
		      pix = ssptr[xpoints[x] + 1];
		      r += R_VAL(pix) * XAP;
		      g += G_VAL(pix) * XAP;
		      b += B_VAL(pix) *  XAP;
		      a += A_VAL(pix) *       XAP;
		      r >>= 8;
		      g >>= 8;
		      b >>= 8;
		      a >>= 8;
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
		    }
		}
	      else
		{
		  if (YAP > 1)
		    {
		      for (j = 0; j < YAP; j++)
			{
			  ssptr = ypoints[dyy + y] + (j *sow);
			  pix = ssptr[xpoints[x]];
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
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
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
	  for (x = dxx; x < end; x++)
	    {
	      int r = 0, g = 0, b = 0, a = 0;
	      int rr = 0, gg = 0, bb = 0, aa = 0;
	      int count;
	      DATA32 pix;
	      
	      if (YAP > 0)
		{
		  if (XAP > 1)
		    {
		      ssptr = ypoints[dyy + y];
		      for (i = 0; i < XAP; i++)
			{
			  pix = ssptr[xpoints[x] + i];
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
			  pix = ssptr[xpoints[x] + i];
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
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
		    }
		  else
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_YAP;
		      g = G_VAL(pix) * INV_YAP;
		      b = B_VAL(pix) * INV_YAP;
		      a = A_VAL(pix) * INV_YAP;
		      ssptr += sow;
		      pix = ssptr[xpoints[x]];
		      r += R_VAL(pix) * YAP;
		      g += G_VAL(pix) * YAP;
		      b += B_VAL(pix) * YAP;
		      a += A_VAL(pix) * YAP;
		      r >>= 8;
		      g >>= 8;
		      b >>= 8;
		      a >>= 8;
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
		    }
		}
	      else
		{
		  if (XAP > 1)
		    {
		      ssptr = ypoints[dyy + y];
		      for (i = 0; i < XAP; i++)
			{
			  pix = ssptr[xpoints[x] + i];
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
		      pix = RGBA_COMPOSE(r, g, b, a);
		      *dptr++ = pix;
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
      /* go through every scanline in the output buffer */
      for (y = 0; y < dh; y++)
	{
	  /* calculate the source line we'll scan from */
	  dptr = dest + dx + ((y + dy) * dow);
	  sptr = ypoints[dyy + y];
	  for (x = dxx; x < end; x++)
	    {
	      int r = 0, g = 0, b = 0, a = 0;
	      int count;
	      DATA32 pix;
	      
	      if ((XAP > 1) || (YAP > 1))
		{
		  for (i = 0, j = 0; j < YAP; j++)
		    {
		      ssptr = ypoints[dyy + y] + (j * sow);
		      for (i = 0; i < XAP; i++)
			{
			  pix = ssptr[xpoints[x] + i];
			  r += R_VAL(pix);
			  g += G_VAL(pix);
			  b += B_VAL(pix);
			  a += A_VAL(pix);
			}
		    }
		  count = (i * j);
		  r /= count;
		  g /= count;
		  b /= count;
		  a /= count;
		  pix = RGBA_COMPOSE(r, g, b, a);
		  *dptr++ = pix;
		}
	      else
		*dptr++ = sptr[xpoints[x]];
	    }
	}
    }
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
	  for (x = dxx; x < end; x++)
	    {
	      int r = 0, g = 0, b = 0;
	      int rr = 0, gg = 0, bb = 0;
	      DATA32 pix;
	      
	      if (YAP > 0)
		{
		  if (XAP > 0)
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_XAP;
		      g = G_VAL(pix) * INV_XAP;
		      b = B_VAL(pix) * INV_XAP;
		      pix = ssptr[xpoints[x] + 1];
		      r += R_VAL(pix) * XAP;
		      g += G_VAL(pix) * XAP;
		      b += B_VAL(pix) * XAP;
		      ssptr += sow;
		      pix = ssptr[xpoints[x]];
		      rr = R_VAL(pix) * INV_XAP;
		      gg = G_VAL(pix) * INV_XAP;
		      bb = B_VAL(pix) * INV_XAP;
		      pix = ssptr[xpoints[x] + 1];
		      rr += R_VAL(pix) * XAP;
		      gg += G_VAL(pix) * XAP;
		      bb += B_VAL(pix) * XAP;
		      r = ((rr * YAP) + (r * INV_YAP)) >> 16;
		      g = ((gg * YAP) + (g * INV_YAP)) >> 16;
		      b = ((bb * YAP) + (b * INV_YAP)) >> 16;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
		    }
		  else
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_YAP;
		      g = G_VAL(pix) * INV_YAP;
		      b = B_VAL(pix) * INV_YAP;
		      ssptr += sow;
		      pix = ssptr[xpoints[x]];
		      r += R_VAL(pix) * YAP;
		      g += G_VAL(pix) * YAP;
		      b += B_VAL(pix) * YAP;
		      r >>= 8;
		      g >>= 8;
		      b >>= 8;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
		    }
		}
	      else
		{
		  if (XAP > 0)
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_XAP;
		      g = G_VAL(pix) * INV_XAP;
		      b = B_VAL(pix) * INV_XAP;
		      pix = ssptr[xpoints[x] + 1];
		      r += R_VAL(pix) * XAP;
		      g += G_VAL(pix) * XAP;
		      b += B_VAL(pix) * XAP;
		      r >>= 8;
		      g >>= 8;
		      b >>= 8;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
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
	  for (x = dxx; x < end; x++)
	    {
	      int r = 0, g = 0, b = 0;
	      int rr = 0, gg = 0, bb = 0;
	      int count;
	      DATA32 pix;
	      
	      if (XAP > 0)
		{
		  if (YAP > 1)
		    {
		      for (j = 0; j < YAP; j++)
			{
			  ssptr = ypoints[dyy + y] + (j * sow);
			  pix = ssptr[xpoints[x]];
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
			  pix = ssptr[xpoints[x] + 1];
			  rr += R_VAL(pix);
			  gg += G_VAL(pix);
			  bb += B_VAL(pix);
			}
		      count = j;
		      r = (r + ((rr * XAP) / count)) >> 8;
		      g = (g + ((gg * XAP) / count)) >> 8;
		      b = (b + ((bb * XAP) / count)) >> 8;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
		    }
		  else
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_XAP;
		      g = G_VAL(pix) * INV_XAP;
		      b = B_VAL(pix) * INV_XAP;
		      pix = ssptr[xpoints[x] + 1];
		      r += R_VAL(pix) * XAP;
		      g += G_VAL(pix) * XAP;
		      b += B_VAL(pix) * XAP;
		      r >>= 8;
		      g >>= 8;
		      b >>= 8;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
		    }
		}
	      else
		{
		  if (YAP > 1)
		    {
		      for (j = 0; j < YAP; j++)
			{
			  ssptr = ypoints[dyy + y] + (j *sow);
			  pix = ssptr[xpoints[x]];
			  r += R_VAL(pix);
			  g += G_VAL(pix);
			  b += B_VAL(pix);
			}
		      count = j;
		      r /= count;
		      g /= count;
		      b /= count;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
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
	  for (x = dxx; x < end; x++)
	    {
	      int r = 0, g = 0, b = 0;
	      int rr = 0, gg = 0, bb = 0;
	      int count;
	      DATA32 pix;
	      
	      if (YAP > 0)
		{
		  if (XAP > 1)
		    {
		      ssptr = ypoints[dyy + y];
		      for (i = 0; i < XAP; i++)
			{
			  pix = ssptr[xpoints[x] + i];
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
			  pix = ssptr[xpoints[x] + i];
			  rr += R_VAL(pix);
			  gg += G_VAL(pix);
			  bb += B_VAL(pix);
			}
		      count = i;
		      r = (r + ((rr * YAP) / count)) >> 8;
		      g = (g + ((gg * YAP) / count)) >> 8;
		      b = (b + ((bb * YAP) / count)) >> 8;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
		    }
		  else
		    {
		      ssptr = ypoints[dyy + y];
		      pix = ssptr[xpoints[x]];
		      r = R_VAL(pix) * INV_YAP;
		      g = G_VAL(pix) * INV_YAP;
		      b = B_VAL(pix) * INV_YAP;
		      ssptr += sow;
		      pix = ssptr[xpoints[x]];
		      r += R_VAL(pix) * YAP;
		      g += G_VAL(pix) * YAP;
		      b += B_VAL(pix) * YAP;
		      r >>= 8;
		      g >>= 8;
		      b >>= 8;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
		    }
		}
	      else
		{
		  if (XAP > 1)
		    {
		      ssptr = ypoints[dyy + y];
		      for (i = 0; i < XAP; i++)
			{
			  pix = ssptr[xpoints[x] + i];
			  r += R_VAL(pix);
			  g += G_VAL(pix);
			  b += B_VAL(pix);
			}
		      count = i;
		      r /= count;
		      g /= count;
		      b /= count;
		      pix = RGBA_COMPOSE(r, g, b, 0xff);
		      *dptr++ = pix;
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
      /* go through every scanline in the output buffer */
      for (y = 0; y < dh; y++)
	{
	  /* calculate the source line we'll scan from */
	  dptr = dest + dx + ((y + dy) * dow);
	  sptr = ypoints[dyy + y];
	  for (x = dxx; x < end; x++)
	    {
	      int r = 0, g = 0, b = 0;
	      int count;
	      DATA32 pix;
	      
	      if ((XAP > 1) || (YAP > 1))
		{
		  for (i = 0, j = 0; j < YAP; j++)
		    {
		      ssptr = ypoints[dyy + y] + (j * sow);
		      for (i = 0; i < XAP; i++)
			{
			  pix = ssptr[xpoints[x] + i];
			  r += R_VAL(pix);
			  g += G_VAL(pix);
			  b += B_VAL(pix);
			}
		    }
		  count = (i * j);
		  r /= count;
		  g /= count;
		  b /= count;
		  pix = RGBA_COMPOSE(r, g, b, 0xff);
		  *dptr++ = pix;
		}
	      else
		*dptr++ = sptr[xpoints[x]];
	    }
	}
    }
}

