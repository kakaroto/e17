#include "common.h"
#include "colormod.h"
#include "file.h"
#include "loaderpath.h"
#include <X11/Xlib.h>
#include <math.h>
#include "image.h"
#include "blend.h"
#include "grad.h"

ImlibRange *
__imlib_CreateRange(void)
{
   ImlibRange *rg = NULL;
   
   rg = malloc(sizeof(ImlibRange));
   rg->color = NULL;
   return rg;
}

void
__imlib_FreeRange(ImlibRange *rg)
{
   ImlibRangeColor *p, *pp;
   
   p = rg->color;
   while(p)
     {
        pp = p;
	p = p->next;
	free(pp);
     }
   free(rg);
}

void
__imlib_AddRangeColor(ImlibRange *rg, DATA8 r, DATA8 g, DATA8 b, DATA8 a,
		      int dist)
{
   ImlibRangeColor *p, *rc;

   if (dist < 1)
      dist = 1;
   if (!rg->color)
      dist = 0;
   
   rc = malloc(sizeof(ImlibRangeColor));
   rc->red = r;
   rc->green = g;
   rc->blue = b;
   rc->alpha = a;
   rc->distance = 0;
   rc->next = NULL;
   
   p = rg->color;
   if (p)
     {
	while (p)
	  {
	     if (!p->next)
	       {
		  p->distance = dist;
		  p->next = rc;
		  p = NULL;
	       }
	     else
		p = p->next;
	  }
     }
   else
      rg->color = rc;
}

DATA32 *
__imlib_MapRange(ImlibRange *rg, int len)
{
   ImlibRangeColor *p;
   DATA32 *map, *pmap, v, vv;
   int r, g, b, a, rr, gg, bb, aa, i, l, ll, v1, v2, inc, j;
   
   if (!rg->color)
      return NULL;
   if (!rg->color->next)
      return NULL;
   ll = 1;
   for (p = rg->color; p; p = p->next)
      ll += p->distance;
   map = malloc(len * sizeof(DATA32));   
   pmap = malloc(ll * sizeof(DATA32));   
   i = 0;
   for (p = rg->color; p; p = p->next)
     {
	if (p->next)
	  {
	     for (j = 0; j < p->distance; j++)
	       {
		  v1 = (j << 16) / p->distance;
		  v2 = 65536 - v1;
		  r = ((p->red * v2)   + (p->next->red * v1))  >> 16;
		  g = ((p->green * v2) + (p->next->green * v1)) >> 16;
		  b = ((p->blue * v2)  + (p->next->blue * v1))  >> 16;
		  a = ((p->alpha * v2) + (p->next->alpha * v1)) >> 16;
		  pmap[i++] = (a << 24) | (r << 16) | (g << 8) | b;
	       }
	  }
	else
	  {
	     r = p->red;
	     g = p->green;
	     b = p->blue;
	     a = p->alpha;
	     pmap[i++] = (a << 24) | (r << 16) | (g << 8) | b;
	  }
     }
   inc = ((ll - 1) << 16) / (len);
   l = 0;
   for (i = 0; i < len; i++)
     {
	v = pmap[l >> 16];
	if ((l >> 16) < ll)
	   vv = pmap[(l >> 16) + 1];
	else
	   vv = pmap[(l >> 16)];
	v1 = l - ((l >> 16) << 16);
	v2 = 65536 - v1;
	b = ((v)      ) & 0xff;
	g = ((v) >> 8 ) & 0xff;
	r = ((v) >> 16) & 0xff;
	a = ((v) >> 24) & 0xff;
	bb = ((vv)      ) & 0xff;
	gg = ((vv) >> 8 ) & 0xff;
	rr = ((vv) >> 16) & 0xff;
	aa = ((vv) >> 24) & 0xff;
	r = ((r * v2) + (rr * v1)) >> 16;
	g = ((g * v2) + (gg * v1)) >> 16;
	b = ((b * v2) + (bb * v1)) >> 16;
	a = ((a * v2) + (aa * v1)) >> 16;
	map[i] = (a << 24) | (r << 16) | (g << 8) | b;
	l += inc;
     }
   free(pmap);
   return map;
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
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

#define BLEND_ADD(r1, g1, b1, a1, dest) \
bb = ((dest)      ) & 0xff;\
gg = ((dest) >> 8 ) & 0xff;\
rr = ((dest) >> 16) & 0xff;\
aa = ((dest) >> 24) & 0xff;\
tmp = rr + (((r1) * (a1)) >> 8);\
nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 9))) & 0xff;\
tmp = gg + (((g1) * (a1)) >> 8);\
ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 9))) & 0xff;\
tmp = bb + (((b1) * (a1)) >> 8);\
nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 9))) & 0xff;\
tmp = (a1) + aa;\
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

#define BLEND_SUB(r1, g1, b1, a1, dest) \
bb = ((dest)      ) & 0xff;\
gg = ((dest) >> 8 ) & 0xff;\
rr = ((dest) >> 16) & 0xff;\
aa = ((dest) >> 24) & 0xff;\
tmp = rr - (((r1) * (a1)) >> 8);\
nr = tmp & (~(tmp >> 8));\
tmp = gg - (((g1) * (a1)) >> 8);\
ng = tmp & (~(tmp >> 8));\
tmp = bb - (((b1) * (a1)) >> 8);\
nb = tmp & (~(tmp >> 8));\
tmp = (a1) + aa;\
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

#define BLEND_RE(r1, g1, b1, a1, dest) \
bb = ((dest)      ) & 0xff;\
gg = ((dest) >> 8 ) & 0xff;\
rr = ((dest) >> 16) & 0xff;\
aa = ((dest) >> 24) & 0xff;\
tmp = rr + ((((r1) - 127) * (a1)) >> 7);\
nr = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9)) & 0xff;\
tmp = gg + ((((g1) - 127) * (a1)) >> 7);\
ng = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9)) & 0xff;\
tmp = bb + ((((b1) - 127) * (a1)) >> 7);\
nb = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9)) & 0xff;\
tmp = (a1) + aa;\
na =  (tmp | ((tmp & 256) - ((tmp & 256) >> 9)));\
(dest) = (na << 24) | (nr << 16) | (ng << 8) | nb;

void
__imlib_DrawGradient(ImlibImage *im, int x, int y, int w, int h,
		     ImlibRange *rg, double angle, ImlibOp op)
{
   DATA32 *map, *p, v;
   int    *hlut, *vlut, len = 0, xx, yy, xoff = 0, yoff  = 0, ww, hh, jump;
   int     tmp, i;
   DATA8   rr, gg, bb, aa, r, g, b, a, nr, ng, nb, na;
   
   ww = w;
   hh = h;
   if (x < 0)
     {
	w += x;
	xoff = -x;
	x = 0;
     }
   if (w <= 0)
      return;
   if ((x + w) > im->w)
      w = (im->w - x);
   if (w <= 0)
      return;
   if (y < 0)
     {
	h += y;
	yoff = -y;
	y = 0;
     }
   if (h <= 0)
      return;
   if ((y + h) > im->h)
      h = (im->h - y);
   if (h <= 0)
      return;
   
   hlut = malloc(sizeof(int) * ww);
   vlut = malloc(sizeof(int) * hh);
   if (ww > hh)
      len = ww * 16;
   else
      len = hh * 16;
   map = __imlib_MapRange(rg, len);
   
   xx = (int)(256 * sin(((angle + 180) * 2 * 3.141592654) / 360));
   yy = -(int)(256 * cos(((angle + 180) * 2 * 3.141592654) / 360));
   if (xx < 0)
     {
	for (i = 0; i < ww; i++)
	   hlut[i] = ((-xx * (ww - 1 - i) * len) / (ww - 1)) >> 8;
     }
   else
     {
	for (i = 0; i < ww; i++)
	   hlut[i] = ((xx * i * len) / (ww - 1)) >> 8;
     }
   if (yy < 0)
     {
	for (i = 0; i < hh; i++)
	   vlut[i] = ((-yy * (hh - 1 - i) * len) / (hh - 1)) >> 8;
     }
   else
     {
	for (i = 0; i < hh; i++)
	   vlut[i] = ((yy * i * len) / (hh - 1)) >> 8;
     }
   jump = im->w - w;
   p = im->data + (y * im->w) + x;
   switch (op)
     {
     case OP_COPY:
	for (yy = 0; yy < h; yy++)
	  {
	     for (xx = 0; xx < w; xx++)
	       {
		  i = vlut[yoff + yy] + hlut[xoff + xx];
		  if (i < 0)
		     i = 0;
		  else if (i >= len)
		     i = len - 1;
		  v = map[i];
		  b = ((v)      ) & 0xff;
		  g = ((v) >> 8 ) & 0xff;
		  r = ((v) >> 16) & 0xff;
		  a = ((v) >> 24) & 0xff;
		  BLEND(r, g, b, a, *p);
		  p++;
	       }
	     p += jump;
	  }
	break;
     case OP_ADD:
	for (yy = 0; yy < h; yy++)
	  {
	     for (xx = 0; xx < w; xx++)
	       {
		  i = vlut[yoff + yy] + hlut[xoff + xx];
		  if (i < 0)
		     i = 0;
		  else if (i >= len)
		     i = len - 1;
		  v = map[i];
		  b = ((v)      ) & 0xff;
		  g = ((v) >> 8 ) & 0xff;
		  r = ((v) >> 16) & 0xff;
		  a = ((v) >> 24) & 0xff;
		  BLEND_SUB(r, g, b, a, *p);
		  p++;
	       }
	     p += jump;
	  }
	break;
     case OP_SUBTRACT:
	for (yy = 0; yy < h; yy++)
	  {
	     for (xx = 0; xx < w; xx++)
	       {
		  i = vlut[yoff + yy] + hlut[xoff + xx];
		  if (i < 0)
		     i = 0;
		  else if (i >= len)
		     i = len - 1;
		  v = map[i];
		  b = ((v)      ) & 0xff;
		  g = ((v) >> 8 ) & 0xff;
		  r = ((v) >> 16) & 0xff;
		  a = ((v) >> 24) & 0xff;
		  BLEND_SUB(r, g, b, a, *p);
		  p++;
	       }
	     p += jump;
	  }
	break;
     case OP_RESHADE:
	for (yy = 0; yy < h; yy++)
	  {
	     for (xx = 0; xx < w; xx++)
	       {
		  i = vlut[yoff + yy] + hlut[xoff + xx];
		  if (i < 0)
		     i = 0;
		  else if (i >= len)
		     i = len - 1;
		  v = map[i];
		  b = ((v)      ) & 0xff;
		  g = ((v) >> 8 ) & 0xff;
		  r = ((v) >> 16) & 0xff;
		  a = ((v) >> 24) & 0xff;
		  BLEND_RE(r, g, b, a, *p);
		  p++;
	       }
	     p += jump;
	  }
	break;
     default:
	break;
     }
   
   free(vlut);
   free(hlut);
   free(map);
}
