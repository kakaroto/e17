#include "common.h"
#include "updates.h"

enum _t_used
{
   T_UNUSED  = 0,
   T_USED = 1,
   T_SPAN_H = 2,
   T_SPAN_V = 4,
   T_DONE = 8
};

struct _tile
{
   enum _t_used used;
   int min_x, min_y, max_x, max_y;
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
   int tw, th, x, y, i, r1, r2;

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
   i = 0;
   /* fill in tiles to be all not used */
   for (y = 0; y < th; y++)
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
	       {
		  if (T(x, y).used == T_USED)
		    {
		       int xx, yy;
		       
		       xx = MAX(uu->x, x << TB);
		       yy = MAX(uu->y, y << TB);
		       T(x, y).min_x = (MIN(xx, T(x, y).min_x)) - (x << TB);
		       T(x, y).min_y = (MIN(yy, T(x, y).min_y)) - (y << TB);
		       xx = MIN(uu->x + uu->w - 1, ((x + 1) << TB) - 1);
		       yy = MIN(uu->y + uu->h - 1, ((y + 1) << TB) - 1);
		       T(x, y).max_x = (MAX(xx, T(x, y).max_x)) - (x << TB);
		       T(x, y).max_y = (MAX(yy, T(x, y).max_y)) - (y << TB);
		    }
		  else
		    {
		       T(x, y).used = T_USED;
		       T(x, y).min_x = (MAX(x << TB, uu->x)) - (x << TB);
		       T(x, y).min_y = (MAX(y << TB, uu->y)) - (y << TB);
		       T(x, y).max_x = (MIN(((x + 1) << TB) - 1, uu->x + uu->w - 1)) - (x << TB);
		       T(x, y).max_y = (MIN(((y + 1) << TB) - 1, uu->y + uu->h - 1)) - (y << TB);
		    }
	       }
	  }
     }
   /* concatinate tiles horizontally */
   for (y = 0; y < th; y++)
     {
	for (x = 0; x < tw - 1; x++)
	  {
	     if ((T(x, y).max_x == TM) && (T(x + 1, y).min_x == 0) &&
		 (T(x, y).min_y == T(x + 1, y).min_y) &&
		 (T(x, y).max_y == T(x + 1, y).max_y))
		T(x, y).used |= T_SPAN_H;
	  }
     }
   /* concatinate tiles vertically */
   for (y = 0; y < th - 1; y++)
     {
	for (x = 0; x < tw; x++)
	  {
	     if ((T(x, y).max_y == TM) && (T(x, y + 1).min_y == 0) &&
		 (T(x, y).min_x == T(x, y + 1).min_x) &&
		 (T(x, y).max_x == T(x, y + 1).max_x))
		T(x, y).used |= T_SPAN_V;
	  }
     }
   /* generate new rect list from tiles */
   for (y = 0; y < th; y++)
     {
	for (x = 0; x < tw; x++)
	  {
	     if (!(T(x, y).used & (T_SPAN_H | T_SPAN_V)))
		nu = __imlib_AddUpdate(nu, 
				       (x << TB) + T(x, y).min_x,
				       (y << TB) + T(x, y).min_y,
				       (T(x, y).max_x  - T(x, y).min_x) + 1,
				       (T(x, y).max_y  - T(x, y).min_y) + 1);
/* keep going here */
	  }
     }   
   free(t);
   /* count up our "update rects" */
   for (r1 = 0, uu = u; uu; uu = uu->next, r1++);
   for (r2 = 0, uu = nu; uu; uu = uu->next, r2++);
   /* if our original rect list is smaller r the same size as the new rect */
   /* list then just use the original rect list */
   if (r1 <= r2)
     {
	__imlib_FreeUpdates(nu);
	return u;
     }
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

