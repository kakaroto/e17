#include "color_helpers.h"
/*
 * Color space conversion helper routines
 * Convert between rgb and hsv adn between rgb and hls
 */
 
void
__imlib_rgb_to_hsv(int r, int g, int b, float *hue, float *saturation, float *value)
{
   int f;
   float i,j,k,max,min,d;

   i = ((float)r)/255.0;
   j = ((float)g)/255.0;
   k = ((float)b)/255.0;

   f = 0;
   max = min = i;
   if (j>max) { max = j; f = 1; } else min = j; 
   if (k>max) { max = k; f = 2; } else if (k<min) min = k;
   d = max - min;

   *value = max;
   if (max!=0) *saturation = d/max; else *saturation = 0;
   if (*saturation==0) 
      *hue = 0;
   else
   {
      switch (f)
      {
         case 0:
           *hue = (j - k)/d;
           break;
         case 1:
           *hue = 2 + (k - i)/d;
           break;
         case 2:
           *hue = 4 + (i - j)/d;
           break;
      }
      *hue *= 60.0;
      if (*hue<0) *hue += 360.0;
   }
}

void
__imlib_hsv_to_rgb(float hue, float saturation, float value, int *r, int *g, int *b)
{
   int i,p,q,t,h;
   float vs,vsf;

   i = (int)(value*255.0);
   if (saturation==0)
      *r = *g = *b = i;
   else
   {
      if (hue==360) hue = 0;
      hue = hue/60.0;
		h = (int)hue;
      vs = value * saturation;
		vsf = vs * (hue - h);
      p = (int)(255.0 * (value - vs));
      q = (int)(255.0 * (value - vsf));
      t = (int)(255.0 * (value - vs + vsf));
      switch (h)
      {
         case 0:
           *r = i;
			  *g = t;
			  *b = p;
			  break;
         case 1:
           *r = q;
			  *g = i;
			  *b = p;
           break;
         case 2:
           *r = p;
			  *g = i;
			  *b = t;
           break;
         case 3:
           *r = p;
			  *g = q;
			  *b = i;
           break;
         case 4:
           *r = t;
			  *g = p;
			  *b = i;
           break;
         case 5:
           *r = i;
			  *g = p;
			  *b = q;
           break;
      }
   }
}

void
__imlib_rgb_to_hls(int r, int g, int b, float *hue, float *lightness, float *saturation)
{
   int f;
   float i,j,k,max,min,d;

   i = ((float)r)/255.0;
   j = ((float)g)/255.0;
   k = ((float)b)/255.0;

   f = 0;
   max = min = i;
   if (j>max) { max = j; f = 1; } else min = j; 
   if (k>max) { max = k; f = 2; } else if (k<min) min = k;
   d = max - min;

   *lightness = (max + min)/2.0;
   if (d==0) 
   {
      *saturation = 0;
      *hue = 0;
   } 
   else
   {
      if (*lightness < 0.5) *saturation = d/(max + min); else *saturation = d/(2 - max - min);
      switch (f)
      {
         case 0:
           *hue = (j - k)/d;
           break;
         case 1:
           *hue = 2 + (k - i)/d;
           break;
         case 2:
           *hue = 4 + (i - j)/d;
           break;
      }
      *hue *= 60.0;
      if (*hue<0) *hue += 360.0;
   }
}

void
__imlib_hls_to_rgb(float hue, float lightness, float saturation, int *r, int *g, int *b)
{
   float m1,m2,m21,h;

   if (saturation==0)
      *r = *g = *b = (int)(lightness * 255.0);
   else
   {
      if (lightness<=0.5)
         m2 = lightness * (1 + saturation);
      else
         m2 = lightness + saturation + lightness * saturation;
      m1 = 2 * lightness - m2;
      m21 = m2 - m1;
      h = hue + 120;
      if (h>360) h -= 360; else if (h<0) h += 360;
      if (h<60)
         *r = (int)(255.0 * (m1 + m21 * h/60.0));
      else if (h<180)
         *r = (int)(255.0 * m2);
      else if (h<240)
         *r = (int)(255.0 * (m1 + m21 * (240.0 - h)/60.0));
      else
         *r = (int)(255.0 * m1);
      h = hue;
      if (h>360) h -= 360; else if (h<0) h += 360;
      if (h<60)
         *g = (int)(255.0 * (m1 + m21 * h/60.0));
      else if (h<180)
         *g = (int)(255.0 * m2);
      else if (h<240)
         *g = (int)(255.0 * (m1 + m21 * (240.0 - h)/60.0));
      else
         *g = (int)(255.0 * m1);
      h = hue - 120;
      if (h>360) h -= 360; else if (h<0) h += 360;
      if (h<60)
         *b = (int)(255.0 * (m1 + m21 * h/60.0));
      else if (h<180)
         *b = (int)(255.0 * m2);
      else if (h<240)
         *b = (int)(255.0 * (m1 + m21 * (240.0 - h)/60.0));
      else
         *b = (int)(255.0 * m1);
   }
}
