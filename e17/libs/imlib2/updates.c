#include "common.h"
#include "updates.h"

enum _t_used
{
   T_UNUSED  = 0,
   T_USED = 1
};

struct _tile
{
   enum _t_used used;
};

#define TB 5
#define TM 31
#define TS 32
#define T(x, y) t[((y) * tw) + (x)]
#define CLIP(x, y, w, h, xx, yy, ww, hh) \
if (x < xx) {w += x; x = xx;} \
if (y < yy) {h += y; y = yy;} \
if ((x + w) > ww) {w = ww - x;} \
if ((y + h) > hh) {h = hh - y;}
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

ImlibUpdate *
__imlib_MergeUpdate(ImlibUpdate *u, int w, int h)
{
   ImlibUpdate *nu = NULL, *uu;
   struct _tile *t;
   int tw, th, x, y, i;

   /* if theres no rects to process.. return NULL */
   if (!u)
      return NULL;
   /* if theres only one rect - return it - no point cleaning up 1 rect */
   if (!u->next)
      return u;
   tw = w >> TB;
   if (w & TM)
      tw++;
   th = h >> TB;
   if (h & TM)
      th++;
   t = malloc(tw * th * sizeof(struct _tile));
   /* fill in tiles to be all not used */
   for (i = 0, y = 0; y < th; y++)
     {
	for (x = 0; x < tw; x++)
	   t[i++].used = T_UNUSED; 
     }
   /* fill in all tiles with minimum and maximum x & y values */
   for (uu = u; uu; uu = uu->next)
     {
	CLIP(uu->x, uu->y, uu->w, uu->h, 0, 0, w, h);
	for (y = uu->y >> TB; y < ((uu->y + uu->h - 1) >> TB) + 1; y++)
	  {
	     for (x = uu->x >> TB; x < ((uu->x + uu->w - 1) >> TB) + 1; x++)
		T(x, y).used = T_USED;
	  }
     }
   for (y = 0; y < th; y++)
     {
	for (x = 0; x < tw - 1; x++)
	  {
	     if (T(x + 1, y).used & T_USED)
	       {
		  int xx, yy, ww, hh, ok;
		  
		  for (xx = x, ww = 0; (T(xx, y).used & T_USED) && (xx < tw);
		       xx++, ww++); 
		  for (yy = y, hh = 1, ok = 1; (yy < th) && (ok); yy++, hh++)
		    {
		       for (xx = x; xx < ww; xx++)
			 {
			    if (!(T(xx, yy).used & T_USED))
			      {
				 ok = 0;
				 xx = ww;
			      }
			 }
		       if (ok)
			 {
			    for (xx = x; xx < ww; xx++)
			       T(xx, yy).used = T_UNUSED;
			 }
		    }
		  nu = __imlib_AddUpdate(nu, (x << TB), (y << TB),
					 (ww << TB), (hh << TB));
	       }
	  }
     }
   free(t);
   __imlib_FreeUpdates(u);
   return nu;
}

ImlibUpdate *
__imlib_AddUpdate(ImlibUpdate *u, int x, int y, int w, int h)
{
   ImlibUpdate *nu;

   if ((w < 1) || (h < 1) || ((x + w) < 1) || ((y + h) < 1)) 
      return u;
   nu = malloc(sizeof(ImlibUpdate));
   nu->x = x;
   nu->y = y;
   nu->w = w;
   nu->h = h;
   nu->next = u;
   return nu;
}

void
__imlib_FreeUpdates(ImlibUpdate *u)
{
   ImlibUpdate *uu;
   
   uu = u;
   while (uu)
     {
	u = uu;
	uu = uu->next;
	free(u);
     }
}

