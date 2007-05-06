#include <math.h>
#include "color_helpers.h"
/*
 * Color space conversion helper routines
 * Convert between rgb and hsv adn between rgb and hls
 */

void
__imlib_rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v)
{
   int min, max;
   int delta;
   
   max = (r+g+abs(r-g))/2;
   max = (max+b+abs(max-b))/2;
   min = (r+g-abs(r-g))/2;
   min = (min+b-abs(min-b))/2;
   
   delta = max - min;
   *v = (float)(100 * max) / 255.0;
   
   if (max!=0)
     *s = (float)(100 * delta) / (float)max;
   else
     {
	*s = 0.0;
	*h = 0.0;
	*v = 0.0;
     }
   if (r == max)
     {
	*h = (float)(100*(g-b)) / (float)(6.0*delta);
     }
   else
     {
	if (g == max)
	  {
	     *h = (float)(100*(2*delta + b-r)) / (float)(6.0*delta);
	  }
	else
	  {
	     *h = (float)(100*(4*delta+r-g)) / (float)(6.0*delta);
	  }
     }
   if (*h < 0.0) *h += 100.0;
   if (*h > 100.0) *h -= 100.0;
}

void
__imlib_hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b)
{
   float hh, f;
   float p, q, t;
   int i;
   
   if (s == 0.0)
     {
	*r = round ((v*255.0)/100.0);
	*g = round ((v*255.0)/100.0);
	*b = round ((v*255.0)/100.0);
	
	return;
     }
   
   hh = (h * 6.0) / 100.0;
   i = floor (hh);
   f = hh - (float)i;
   
   p = v*(1.0 - s / 100.0) / 100.0;
   q = v*(1.0 - (s*f) / 100.0) / 100.0;
   t = v*(1.0 - s*(1.0 - f) / 100.0) / 100.0;
   
   switch (i)
     {
      case 0:
	  {
	     *r = round (v*255.0 / 100.0);
	     *g = round (t*255.0);
	     *b = round (p*255.0);
	     break;
	  }
      case 1:
	  {
	     *r = round (q*255.0);
	     *g = round (v*255.0 / 100.0);
	     *b = round (p*255.0);
	     break;
	  }
      case 2:
	  {
	     *r = round (p*255.0);
	     *g = round (v*255.0 / 100.0);
	     *b = round (t*255.0);
	     break;
	  }
      case 3:
	  {
	     *r = round (p*255.0);
	     *g = round (q*255.0);
	     *b = round (v*255.0 / 100.0);
	     break;
	  }
      case 4:
	  {
	     *r = round (t*255.0);
	     *g = round (p*255.0);
	     *b = round (v*255.0 / 100.0);
	     break;
	  }
      case 5:
	  {
	     *r = round (v*255.0 / 100.0);
	     *g = round (p*255.0);
	     *b = round (q*255.0);
	     break;
	  }
     }
}

void
__imlib_rgb_to_hls(int r, int g, int b, float *hue, float *lightness,
                   float *saturation)
{
   int                 f;
   float               i, j, k, max, min, d;

   i = ((float)r) / 255.0;
   j = ((float)g) / 255.0;
   k = ((float)b) / 255.0;

   f = 0;
   max = min = i;
   if (j > max)
     {
        max = j;
        f = 1;
     }
   else
      min = j;
   if (k > max)
     {
        max = k;
        f = 2;
     }
   else if (k < min)
      min = k;
   d = max - min;

   *lightness = (max + min) / 2.0;
   if (d == 0)
     {
        *saturation = 0;
        *hue = 0;
     }
   else
     {
        if (*lightness < 0.5)
           *saturation = d / (max + min);
        else
           *saturation = d / (2 - max - min);
        switch (f)
          {
            case 0:
               *hue = (j - k) / d;
               break;
            case 1:
               *hue = 2 + (k - i) / d;
               break;
            case 2:
               *hue = 4 + (i - j) / d;
               break;
          }
        *hue *= 60.0;
        if (*hue < 0)
           *hue += 360.0;
     }
}

void
__imlib_hls_to_rgb(float hue, float lightness, float saturation, int *r, int *g,
                   int *b)
{
   float               m1, m2, m21, h;

   if (saturation == 0)
      *r = *g = *b = (int)(lightness * 255.0);
   else
     {
        if (lightness <= 0.5)
           m2 = lightness * (1 + saturation);
        else
           m2 = lightness + saturation + lightness * saturation;
        m1 = 2 * lightness - m2;
        m21 = m2 - m1;
        h = hue + 120;
        if (h > 360)
           h -= 360;
        else if (h < 0)
           h += 360;
        if (h < 60)
           *r = (int)(255.0 * (m1 + m21 * h / 60.0));
        else if (h < 180)
           *r = (int)(255.0 * m2);
        else if (h < 240)
           *r = (int)(255.0 * (m1 + m21 * (240.0 - h) / 60.0));
        else
           *r = (int)(255.0 * m1);
        h = hue;
        if (h > 360)
           h -= 360;
        else if (h < 0)
           h += 360;
        if (h < 60)
           *g = (int)(255.0 * (m1 + m21 * h / 60.0));
        else if (h < 180)
           *g = (int)(255.0 * m2);
        else if (h < 240)
           *g = (int)(255.0 * (m1 + m21 * (240.0 - h) / 60.0));
        else
           *g = (int)(255.0 * m1);
        h = hue - 120;
        if (h > 360)
           h -= 360;
        else if (h < 0)
           h += 360;
        if (h < 60)
           *b = (int)(255.0 * (m1 + m21 * h / 60.0));
        else if (h < 180)
           *b = (int)(255.0 * m2);
        else if (h < 240)
           *b = (int)(255.0 * (m1 + m21 * (240.0 - h) / 60.0));
        else
           *b = (int)(255.0 * m1);
     }
}
