/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#include "desktops.h"
#include "ewins.h"
#include "groups.h"
#include "screen.h"

#define DEBUG_ARRANGE 0

typedef struct
{
   void               *data;
   int                 x, y, w, h;
   int                 p;
} RectBox;

static int
ArrangeAddToList(int *array, int current_size, int value)
{
   int                 i, j;

   for (i = 0; i < current_size; i++)
     {
	if (value < array[i])
	  {
	     for (j = current_size; j > i; j--)
		array[j] = array[j - 1];
	     array[i] = value;
	     return current_size + 1;
	  }
	else if (value == array[i])
	   return current_size;
     }
   array[current_size] = value;
   return current_size + 1;
}

#define Filled(x,y) (filled[(y * (xsize - 1)) + x])

static void
ArrangeMakeFillLists(int startx, int width, int starty, int height,
		     const RectBox * sorted, int num_sorted,
		     int *xarray, int *nx, int *yarray, int *ny,
		     unsigned char *filled)
{
   int                 j, x1, x2, y1, y2, k, y, x;
   int                 xsize, ysize;

   xsize = 0;
   ysize = 0;

   /* put all the sorted rects into the xy arrays */
   xsize = ArrangeAddToList(xarray, xsize, startx);
   xsize = ArrangeAddToList(xarray, xsize, width);
   ysize = ArrangeAddToList(yarray, ysize, starty);
   ysize = ArrangeAddToList(yarray, ysize, height);

   for (j = 0; j < num_sorted; j++)
     {
	if (sorted[j].x < width)
	   xsize = ArrangeAddToList(xarray, xsize, sorted[j].x);
	if ((sorted[j].x + sorted[j].w) < width)
	   xsize = ArrangeAddToList(xarray, xsize, sorted[j].x + sorted[j].w);
	if (sorted[j].y < height)
	   ysize = ArrangeAddToList(yarray, ysize, sorted[j].y);
	if ((sorted[j].y + sorted[j].h) < height)
	   ysize = ArrangeAddToList(yarray, ysize, sorted[j].y + sorted[j].h);
     }
#if DEBUG_ARRANGE
   for (j = 0; j < xsize; j++)
      Eprintf("xarray[%d] = %d\n", j, xarray[j]);
   for (j = 0; j < ysize; j++)
      Eprintf("yarray[%d] = %d\n", j, yarray[j]);
#endif

   /* fill the allocation array */
   for (j = 0; j < (xsize - 1) * (ysize - 1); filled[j++] = 0)
      ;
   for (j = 0; j < num_sorted; j++)
     {
	x1 = -1;
	x2 = -1;
	y1 = -1;
	y2 = -1;
	for (k = 0; k < xsize - 1; k++)
	   if (sorted[j].x == xarray[k])
	     {
		x1 = x2 = k;
		break;
	     }
	for (k++; k < xsize; k++)
	   if (sorted[j].x + sorted[j].w > xarray[k])
	      x2 = k;
	for (k = 0; k < ysize - 1; k++)
	   if (sorted[j].y == yarray[k])
	     {
		y1 = y2 = k;
		break;
	     }
	for (k++; k < ysize; k++)
	   if (sorted[j].y + sorted[j].h > yarray[k])
	      y2 = k;
#if DEBUG_ARRANGE
	Eprintf("Fill %4d,%4d %4dx%4d: (%2d)%4d->(%2d)%4d,(%2d)%4d->(%2d)%4d\n",
		sorted[j].x, sorted[j].y, sorted[j].w, sorted[j].h,
		x1, xarray[x1], x2, xarray[x2], y1, yarray[y1], y2, yarray[y2]);
#endif
	if ((x1 >= 0) && (x2 >= 0) && (y1 >= 0) && (y2 >= 0))
	  {
	     for (y = y1; y <= y2; y++)
	       {
		  for (x = x1; x <= x2; x++)
		    {
		       if (Filled(x, y) <= sorted[j].p)
			  Filled(x, y) = sorted[j].p + 1;
		    }
	       }
	  }
     }

#if DEBUG_ARRANGE
   Eprintf("Filled[%2d,%2d] =\n", xsize, ysize);
   for (k = 0; k < ysize - 1; k++)
     {
	for (j = 0; j < xsize - 1; j++)
	   printf(" %2d", Filled(j, k));
	printf("\n");
     }
#endif

   *nx = xsize;
   *ny = ysize;
}

static void
ArrangeFindSpaces(const int *xarray, int xsize, const int *yarray, int ysize,
		  unsigned char *filled, RectBox * spaces, int *ns,
		  RectBox * fit, int must_fit)
{
   int                 i, j, x, y, x1, y1, xbest, ybest;
   int                 num_spaces;
   unsigned int        a, abest;

   /* create list of all "spaces" */
   num_spaces = 0;
   for (y = 0; y < ysize - 1; y++)
     {
	for (x = 0; x < xsize - 1; x++)
	  {
	     /* if the square is empty "grow" the space */
	     if (Filled(x, y) > fit->p)
		continue;

	     xbest = x;
	     ybest = y;
	     abest = 0;
	     x1 = xsize - 1;
#if DEBUG_ARRANGE
	     Eprintf("Check %d,%d: %d,%d\n", x, y, xarray[x], yarray[y]);
#endif
	     for (j = y; j < ysize - 1;)
	       {
		  for (i = x; i < x1; i++)
		    {
		       if (Filled(i, j) > fit->p)
			  break;
		    }
		  x1 = i;
		  j++;
		  if (x1 <= x)
		     break;
		  if (must_fit && (xarray[x1] - xarray[x] < fit->w))
		     continue;
		  for (; j < ysize - 1; j++)
		    {
		       for (i = x; i < x1; i++)
			 {
			    if (Filled(i, j) > fit->p)
			       goto got_one;
			 }
		    }
		got_one:
		  y1 = j;
		  if (must_fit && (yarray[y1] - yarray[y] < fit->h))
		     continue;
		  a = (xarray[x1] - xarray[x]) * (yarray[y1] - yarray[y]);
#if DEBUG_ARRANGE
		  Eprintf("Got %4d,%4d %4dx%4d: %d\n", xarray[x],
			  yarray[y], xarray[x1] - xarray[x],
			  yarray[y1] - yarray[y], a);
#endif
		  if (a > abest)
		    {
		       xbest = x1;
		       ybest = y1;
		       abest = a;
		    }
	       }
	     if (abest == 0)
		continue;

	     spaces[num_spaces].x = xarray[x];
	     spaces[num_spaces].y = yarray[y];
	     spaces[num_spaces].w = xarray[xbest] - xarray[x];
	     spaces[num_spaces].h = yarray[ybest] - yarray[y];
#if 0
	     spaces[num_spaces].p = spaces[num_spaces].w >= fit->w &&
		spaces[num_spaces].h >= fit->h;
#else
	     spaces[num_spaces].p = 1;
#endif
	     num_spaces++;
	  }
     }
#if DEBUG_ARRANGE
   for (j = 0; j < num_spaces; j++)
      Eprintf("Spaces: x,y=%4d,%4d wxh=%3dx%3d p=%2d\n",
	      spaces[j].x, spaces[j].y, spaces[j].w, spaces[j].h, spaces[j].p);
#endif

   *ns = num_spaces;
}

static void
ArrangeSwapList(RectBox * list, int a, int b)
{
   RectBox             bb;

   bb.data = list[a].data;
   bb.x = list[a].x;
   bb.y = list[a].y;
   bb.w = list[a].w;
   bb.h = list[a].h;
   list[a].data = list[b].data;
   list[a].x = list[b].x;
   list[a].y = list[b].y;
   list[a].w = list[b].w;
   list[a].h = list[b].h;
   list[b].data = bb.data;
   list[b].x = bb.x;
   list[b].y = bb.y;
   list[b].w = bb.w;
   list[b].h = bb.h;
}

static void
ArrangeRects(const RectBox * fixed, int fixed_count, RectBox * floating,
	     int floating_count, RectBox * sorted, int startx, int starty,
	     int width, int height, int policy, char initial_window)
{
   int                 num_sorted;
   int                 xsize = 0, ysize = 0;
   int                *xarray, *yarray;
   int                *leftover;
   int                 i, j, k;
   unsigned char      *filled;
   RectBox            *spaces;
   int                 num_spaces;
   int                 sort;
   int                 a1, a2;
   int                 num_leftover;

   if (initial_window)
     {
	int                 xx1, yy1, xx2, yy2;

	GetPointerScreenAvailableArea(&xx1, &yy1, &xx2, &yy2);
	xx2 += xx1;
	yy2 += yy1;
	if (startx < xx1)
	   startx = xx1;
	if (width > xx2)
	   width = xx2;
	if (starty < yy1)
	   starty = yy1;
	if (height > yy2)
	   height = yy2;
     }
#if DEBUG_ARRANGE
   Eprintf("Start %d,%d %dx%d\n", startx, starty, width, height);
#endif

   switch (policy)
     {
     case ARRANGE_VERBATIM:
	break;
     case ARRANGE_BY_SIZE:
	sort = 0;
	while (!sort)
	  {
	     sort = 1;
	     for (i = 0; i < floating_count - 1; i++)
	       {
		  a1 = floating[i].w * floating[i].h;
		  a2 = floating[i + 1].w * floating[i + 1].h;
		  if (a2 > a1)
		    {
		       sort = 0;
		       ArrangeSwapList(floating, i, i + 1);
		    }
	       }
	  }
	break;
     case ARRANGE_BY_POSITION:
	sort = 0;
	while (!sort)
	  {
	     sort = 1;
	     for (i = 0; i < floating_count - 1; i++)
	       {
		  a1 = floating[i].x + floating[i].y;
		  a2 = (floating[i + 1].x + (floating[i + 1].w >> 1)) +
		     (floating[i + 1].y + (floating[i + 1].h >> 1));
		  if (a2 < a1)
		    {
		       sort = 0;
		       ArrangeSwapList(floating, i, i + 1);
		    }
	       }
	  }
	break;
     default:
	break;
     }

   /* for every floating rect in order, "fit" it into the sorted list */
   i = ((fixed_count + floating_count) * 2) + 2;
   xarray = Emalloc(i * sizeof(int));
   yarray = Emalloc(i * sizeof(int));
   filled = Emalloc(i * i * sizeof(char));
   spaces = Emalloc(i * i * sizeof(RectBox));
   leftover = NULL;
   if (floating_count)
      leftover = Emalloc(floating_count * sizeof(int));

   if (!xarray || !yarray || !filled || !spaces)
      goto done;

   /* copy "fixed" rects into the sorted list */
   memcpy(sorted, fixed, fixed_count * sizeof(RectBox));
   num_sorted = fixed_count;

   /* go through each floating rect in order and "fit" it in */
   num_leftover = 0;
   for (i = 0; i < floating_count; i++)
     {
	ArrangeMakeFillLists(startx, width, starty, height, sorted, num_sorted,
			     xarray, &xsize, yarray, &ysize, filled);

	/* create list of all "spaces" */
	ArrangeFindSpaces(xarray, xsize, yarray, ysize, filled,
			  spaces, &num_spaces, floating + i, 1);

	/* find the first space that fits */
	k = -1;
	sort = 0x7fffffff;
	for (j = 0; j < num_spaces; j++)
	  {
	     if ((spaces[j].w < floating[i].w) ||
		 (spaces[j].h < floating[i].h) ||
		 (spaces[j].x < startx) ||
		 (spaces[j].x + spaces[j].w > width) ||
		 (spaces[j].y < starty) || (spaces[j].y + spaces[j].h > height))
		continue;

	     if (policy == ARRANGE_BY_POSITION)
	       {
		  a1 = (spaces[j].x + (spaces[j].w >> 1)) -
		     (floating[i].x + (floating[i].w >> 1));
		  a2 = (spaces[j].y + (spaces[j].h >> 1)) -
		     (floating[i].y + (floating[i].h >> 1));
		  if (a1 < 0)
		     a1 = -a1;
		  if (a2 < 0)
		     a2 = -a2;
		  if ((a1 + a2) < sort)
		    {
		       sort = a1 + a2;
		       k = j;
		    }
	       }
	     else
	       {
		  k = j;
		  break;
	       }
	  }

	if (k >= 0)
	  {
	     if (policy == ARRANGE_BY_POSITION)
	       {
		  a1 = (spaces[k].x + (spaces[k].w >> 1)) - (floating[i].x +
							     (floating[i].w >>
							      1));
		  a2 = (spaces[k].y + (spaces[k].h >> 1)) - (floating[i].y +
							     (floating[i].h >>
							      1));
		  if (a1 >= 0)
		    {
		       sorted[num_sorted].x = spaces[k].x;
		    }
		  else
		    {
		       sorted[num_sorted].x =
			  spaces[k].x + spaces[k].w - floating[i].w;
		    }
		  if (a2 >= 0)
		    {
		       sorted[num_sorted].y = spaces[k].y;
		    }
		  else
		    {
		       sorted[num_sorted].y =
			  spaces[k].y + spaces[k].h - floating[i].h;
		    }
	       }
	     else
	       {
		  sorted[num_sorted].x = spaces[k].x;
		  sorted[num_sorted].y = spaces[k].y;
	       }
	     sorted[num_sorted].data = floating[i].data;
	     sorted[num_sorted].w = floating[i].w;
	     sorted[num_sorted].h = floating[i].h;
	     sorted[num_sorted].p = floating[i].p;
	     num_sorted++;
	  }
	else
	   leftover[num_leftover++] = i;
     }

#if DEBUG_ARRANGE
   Eprintf("Leftovers: %d\n", num_leftover);
#endif
   /* ok we cant fit everything in this baby.... time to fit */
   /* the leftovers into the leftover space */
   for (i = 0; i < num_leftover; i++)
     {
	ArrangeMakeFillLists(startx, width, starty, height, sorted, num_sorted,
			     xarray, &xsize, yarray, &ysize, filled);

	/* create list of all "spaces" */
	ArrangeFindSpaces(xarray, xsize, yarray, ysize, filled,
			  spaces, &num_spaces, floating + leftover[i], 0);

	/* find the first space that fits */
	k = -1;
	sort = 0x7fffffff;
	a1 = floating[leftover[i]].w * floating[leftover[i]].h;
	k = -1;
	for (j = 0; j < num_spaces; j++)
	  {
	     a2 = spaces[j].w * spaces[j].h;
	     if ((a2 != 0) && ((a1 - a2) < sort) && (spaces[j].p))
	       {
		  k = j;
		  sort = a1 - a2;
	       }
	  }

	if (k >= 0)
	  {
	     /* if there's a small space ... */
	     sorted[num_sorted].x = spaces[k].x;
	     sorted[num_sorted].y = spaces[k].y;
	  }
	else
	  {
	     /* there is no room - put it centered */
	     /* (but dont put top left off screen) */
	     sorted[num_sorted].x = (width - floating[leftover[i]].w) / 2;
	     sorted[num_sorted].y = (height - floating[leftover[i]].h) / 2;
	  }
	sorted[num_sorted].data = floating[leftover[i]].data;
	sorted[num_sorted].w = floating[leftover[i]].w;
	sorted[num_sorted].h = floating[leftover[i]].h;
	sorted[num_sorted].p = floating[leftover[i]].p;
	if ((sorted[num_sorted].x + sorted[num_sorted].w) > width)
	   sorted[num_sorted].x = width - sorted[num_sorted].w;
	if ((sorted[num_sorted].y + sorted[num_sorted].h) > height)
	   sorted[num_sorted].y = height - sorted[num_sorted].h;
	if (sorted[num_sorted].x < startx)
	   sorted[num_sorted].x = startx;
	if (sorted[num_sorted].y < starty)
	   sorted[num_sorted].y = starty;
	num_sorted++;
     }

#if DEBUG_ARRANGE
   for (i = 0; i < num_sorted; i++)
      Eprintf("Sorted: x,y=%4d,%4d wxh=%3dx%3d p=%2d: %s\n",
	      sorted[i].x, sorted[i].y, sorted[i].w, sorted[i].h, sorted[i].p,
	      (sorted[i].data) ? ((EObj *) sorted[i].data)->name : "?");
#endif

 done:
   /* free up memory */
   if (xarray)
      Efree(xarray);
   if (yarray)
      Efree(yarray);
   if (filled)
      Efree(filled);
   if (spaces)
      Efree(spaces);
   if (leftover)
      Efree(leftover);
}

void
SnapEwin(EWin * ewin, int dx, int dy, int *new_dx, int *new_dy)
{
   EWin               *const *lst1;
   EWin              **lst, **gwins;
   int                 gnum, num, i, j, screen_snap_dist, odx, ody;
   static char         last_res = 0;
   int                 top_bound, bottom_bound, left_bound, right_bound, w, h;

   if (!ewin)
      return;

   if (!Conf.snap.enable)
     {
	*new_dx = dx;
	*new_dy = dy;
	return;
     }

   ScreenGetAvailableArea(ewin->shape_x, ewin->shape_y,
			  &left_bound, &top_bound, &w, &h);
   right_bound = left_bound + w;
   bottom_bound = top_bound + h;
   screen_snap_dist = Mode.constrained ? (w + h) : Conf.snap.screen_snap_dist;

   lst = NULL;
   lst1 = EwinListOrderGet(&num);
   if (lst1)
     {
	lst = malloc(num * sizeof(EWin *));
	if (!lst)
	   return;
	memcpy(lst, lst1, num * sizeof(EWin *));
     }
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, Mode.nogroup
				      || Mode.move.swap, &gnum);
   if (gwins)
     {
	for (i = 0; i < gnum; i++)
	  {
	     for (j = 0; j < num; j++)
	       {
		  if ((lst[j] == gwins[i]) || (lst[j] == ewin))
		     lst[j] = NULL;
	       }
	  }
	Efree(gwins);
     }

   odx = dx;
   ody = dy;
   if (dx < 0)
     {
	if (IN_BELOW(ewin->shape_x + dx, left_bound, screen_snap_dist)
	    && (ewin->shape_x >= left_bound))
	  {
	     dx = left_bound - ewin->shape_x;
	  }
	else if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (lst[i] == NULL)
		     continue;

		  if ((EoGetDesk(ewin) == EoGetDesk(lst[i]) ||
		       EoIsSticky(lst[i])) && !(EoIsFloating(lst[i])) &&
		      !lst[i]->state.iconified && !lst[i]->props.ignorearrange)
		    {
		       if (IN_BELOW
			   (ewin->shape_x + dx,
			    EoGetX(lst[i]) + EoGetW(lst[i]) - 1,
			    Conf.snap.edge_snap_dist)
			   && SPANS_COMMON(ewin->shape_y, EoGetH(ewin),
					   EoGetY(lst[i]), EoGetH(lst[i]))
			   && (ewin->shape_x >=
			       (EoGetX(lst[i]) + EoGetW(lst[i]))))
			 {
			    dx =
			       (EoGetX(lst[i]) + EoGetW(lst[i])) -
			       ewin->shape_x;
			    break;
			 }
		    }
	       }
	  }
	if ((ewin->req_x - ewin->shape_x) > 0)
	   dx = 0;
     }
   else if (dx > 0)
     {
	if (IN_ABOVE
	    (ewin->shape_x + EoGetW(ewin) + dx, right_bound, screen_snap_dist)
	    && ((ewin->shape_x + EoGetW(ewin)) <= right_bound))
	  {
	     dx = right_bound - (ewin->shape_x + EoGetW(ewin));
	  }
	else if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (lst[i] == NULL)
		     continue;

		  if ((EoGetDesk(ewin) == EoGetDesk(lst[i]) ||
		       EoIsSticky(lst[i])) && !(EoIsFloating(lst[i])) &&
		      !lst[i]->state.iconified && !lst[i]->props.ignorearrange)
		    {
		       if (IN_ABOVE
			   (ewin->shape_x + EoGetW(ewin) + dx - 1,
			    EoGetX(lst[i]), Conf.snap.edge_snap_dist)
			   && SPANS_COMMON(ewin->shape_y, EoGetH(ewin),
					   EoGetY(lst[i]), EoGetH(lst[i]))
			   && ((ewin->shape_x + EoGetW(ewin)) <=
			       EoGetX(lst[i])))
			 {
			    dx =
			       EoGetX(lst[i]) - (ewin->shape_x + EoGetW(ewin));
			    break;
			 }
		    }
	       }
	  }
	if ((ewin->req_x - ewin->shape_x) < 0)
	   dx = 0;
     }

   if (dy < 0)
     {
	if (IN_BELOW(ewin->shape_y + dy, top_bound, screen_snap_dist)
	    && (ewin->shape_y >= top_bound))
	  {
	     dy = top_bound - ewin->shape_y;
	  }
	else if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (lst[i] == NULL)
		     continue;

		  if ((EoGetDesk(ewin) == EoGetDesk(lst[i]) ||
		       EoIsSticky(lst[i])) && !(EoIsFloating(lst[i])) &&
		      !lst[i]->state.iconified && !lst[i]->props.ignorearrange)
		    {
		       if (IN_BELOW
			   (ewin->shape_y + dy,
			    EoGetY(lst[i]) + EoGetH(lst[i]) - 1,
			    Conf.snap.edge_snap_dist)
			   && SPANS_COMMON(ewin->shape_x, EoGetW(ewin),
					   EoGetX(lst[i]), EoGetW(lst[i]))
			   && (ewin->shape_y >=
			       (EoGetY(lst[i]) + EoGetH(lst[i]))))
			 {
			    dy =
			       (EoGetY(lst[i]) + EoGetH(lst[i])) -
			       ewin->shape_y;
			    break;
			 }
		    }
	       }
	  }
	if ((ewin->req_y - ewin->shape_y) > 0)
	   dy = 0;
     }
   else if (dy > 0)
     {
	if (IN_ABOVE
	    (ewin->shape_y + EoGetH(ewin) + dy, bottom_bound,
	     screen_snap_dist)
	    && ((ewin->shape_y + EoGetH(ewin)) <= bottom_bound))
	  {
	     dy = bottom_bound - (ewin->shape_y + EoGetH(ewin));
	  }
	else if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (lst[i] == NULL)
		     continue;

		  if ((EoGetDesk(ewin) == EoGetDesk(lst[i]) ||
		       EoIsSticky(lst[i])) && !(EoIsFloating(lst[i])) &&
		      !lst[i]->state.iconified && !lst[i]->props.ignorearrange)
		    {
		       if (IN_ABOVE
			   (ewin->shape_y + EoGetH(ewin) + dy - 1,
			    EoGetY(lst[i]), Conf.snap.edge_snap_dist)
			   && SPANS_COMMON(ewin->shape_x, EoGetW(ewin),
					   EoGetX(lst[i]), EoGetW(lst[i]))
			   && ((ewin->shape_y + EoGetH(ewin)) <=
			       EoGetY(lst[i])))
			 {
			    dy =
			       EoGetY(lst[i]) - (ewin->shape_y + EoGetH(ewin));
			    break;
			 }
		    }
	       }
	  }
	if ((ewin->req_y - ewin->shape_y) < 0)
	   dy = 0;
     }

   if (lst)
      Efree(lst);

   if ((odx != dx) || (ody != dy))
     {
	if (!last_res)
	  {
	     /* SoundPlay("SOUND_MOVE_RESIST"); */
	     last_res = 1;
	  }
     }
   else
     {
	last_res = 0;
     }
   *new_dx = dx;
   *new_dy = dy;
}

void
ArrangeEwin(EWin * ewin)
{
   int                 x, y;

   ArrangeEwinXY(ewin, &x, &y);
   EwinMove(ewin, x, y);
}

void
ArrangeEwinCentered(EWin * ewin)
{
   int                 x, y;

   ArrangeEwinCenteredXY(ewin, &x, &y);
   EwinMove(ewin, x, y);
}

static void
ArrangeGetRectList(RectBox ** pfixed, int *nfixed, RectBox ** pfloating,
		   int *nfloating, EWin * ewin)
{
   RectBox            *rb, *fixed, *floating;
   int                 x, y, w, h, i, nfix, nflt, num;
   EObj               *const *lst, *eo;
   Desk               *dsk;

   fixed = floating = NULL;
   nfix = nflt = 0;

   lst = EobjListStackGet(&num);
   if (!lst)
      goto done;

   fixed = Emalloc(sizeof(RectBox) * num);
   if (!fixed)
      goto done;
   rb = fixed;

   dsk = (ewin) ? EoGetDesk(ewin) : DesksGetCurrent();

   for (i = 0; i < num; i++)
     {
	rb = fixed + nfix;
	eo = lst[i];

	if (!eo->shown)
	   continue;

	if (eo->type == EOBJ_TYPE_EWIN)
	  {
	     EWin               *ew = (EWin *) eo;

	     if (ew == ewin)
		continue;
	     if (eo->desk != dsk)
		continue;

	     if (ew->props.ignorearrange || EoGetLayer(ew) == 0)
		continue;

	     if (pfloating)
	       {
		  int                 ax, ay;

		  DeskGetArea(EoGetDesk(ew), &ax, &ay);

		  if (!EoIsSticky(ew) && !EoIsFloating(ew) &&
		      ew->area_x == ax && ew->area_y == ay)
		    {
		       floating =
			  Erealloc(floating, (nflt + 1) * sizeof(RectBox));
		       rb = floating + nflt++;
		       rb->data = ew;
		       rb->x = EoGetX(ew);
		       rb->y = EoGetY(ew);
		       rb->w = EoGetW(ew);
		       rb->h = EoGetH(ew);
		       rb->p = EoGetLayer(ew);
#if DEBUG_ARRANGE
		       Eprintf("Add float: x,y=%4d,%4d wxh=%3dx%3d p=%2d: %s\n",
			       rb->x, rb->y, rb->w, rb->h, rb->p, eo->name);
#endif
		       continue;
		    }
	       }

	     rb->data = ew;

	     if (ew->props.never_use_area)
		rb->p = 50;
	     else
		rb->p = EoGetLayer(ew);
	  }
	else if (eo->type == EOBJ_TYPE_BUTTON)
	  {
	     if (!eo->sticky && eo->desk != dsk)
		continue;

	     rb->data = NULL;
	     rb->p = (eo->sticky) ? 1 : 0;
	  }
	else
	  {
	     continue;
	  }

	x = EobjGetX(eo);
	y = EobjGetY(eo);
	w = EobjGetW(eo);
	h = EobjGetH(eo);

	if (x < 0)
	  {
	     w += x;
	     x = 0;
	  }
	if ((x + w) > VRoot.w)
	   w = VRoot.w - x;

	if (y < 0)
	  {
	     h += y;
	     y = 0;
	  }
	if ((y + h) > VRoot.h)
	   h = VRoot.h - y;

	if ((w <= 0) || (h <= 0))
	   continue;

	rb->x = x;
	rb->y = y;
	rb->w = w;
	rb->h = h;
#if DEBUG_ARRANGE
	Eprintf("Add fixed: x,y=%4d,%4d wxh=%3dx%3d p=%2d: %s\n", rb->x, rb->y,
		rb->w, rb->h, rb->p, eo->name);
#endif

	nfix++;
     }

 done:
#if DEBUG_ARRANGE
   Eprintf("Fixed: %p/%d  Floating: %p/%d\n", fixed, nfix, floating, nflt);
#endif
   *pfixed = fixed;
   *nfixed = nfix;
   if (pfloating)
      *pfloating = floating;
   if (nfloating)
      *nfloating = nflt;
}

void
ArrangeEwinXY(EWin * ewin, int *px, int *py)
{
   EWin               *const *lst;
   int                 i, num;
   RectBox            *fixed, *ret, newrect;

   fixed = NULL;
   ret = NULL;

   lst = EwinListGetAll(&num);
   if (num <= 1)
     {
	ArrangeEwinCenteredXY(ewin, px, py);
	return;
     }

   ArrangeGetRectList(&fixed, &num, NULL, NULL, ewin);

   newrect.data = ewin;
   newrect.x = 0;
   newrect.y = 0;
   newrect.w = EoGetW(ewin);
   newrect.h = EoGetH(ewin);
   newrect.p = EoGetLayer(ewin);

   ret = Emalloc(sizeof(RectBox) * (num + 1));
   ArrangeRects(fixed, num, &newrect, 1, ret, 0, 0, VRoot.w, VRoot.h,
		ARRANGE_BY_SIZE, 1);

   for (i = 0; i < num + 1; i++)
     {
	if (ret[i].data == ewin)
	  {
	     *px = ret[i].x;
	     *py = ret[i].y;
	     break;
	  }
     }
   if (ret)
      Efree(ret);
   if (fixed)
      Efree(fixed);
}

void
ArrangeEwinCenteredXY(EWin * ewin, int *px, int *py)
{
   int                 x, y, w, h;

   GetPointerScreenAvailableArea(&x, &y, &w, &h);
   *px = (w - EoGetW(ewin)) / 2 + x;
   *py = (h - EoGetH(ewin)) / 2 + y;
}

void
ArrangeEwins(const char *params)
{
   const char         *type;
   int                 method;
   int                 i, nfix, nflt, num;
   RectBox            *fixed, *ret, *floating;
   EWin               *const *lst, *ewin;

   type = params;
   method = ARRANGE_BY_SIZE;

   if (params)
     {
	if (!strcmp("order", type))
	  {
	     method = ARRANGE_VERBATIM;
	  }
	else if (!strcmp("place", type))
	  {
	     method = ARRANGE_BY_POSITION;
	  }
     }

   lst = EwinListGetAll(&num);
   if (!lst)
      goto done;

   ArrangeGetRectList(&fixed, &nfix, &floating, &nflt, NULL);

   ret = Emalloc(sizeof(RectBox) * (nflt + nfix));
   ArrangeRects(fixed, nfix, floating, nflt, ret, 0, 0, VRoot.w, VRoot.h,
		method, 1);

   for (i = nfix; i < nflt + nfix; i++)
     {
	if (!ret[i].data)
	   continue;

	ewin = (EWin *) ret[i].data;
	if ((EoGetX(ewin) == ret[i].x) && (EoGetY(ewin) == ret[i].y))
	   continue;

	if (Conf.place.cleanupslide)
	   SlideEwinTo(ewin, EoGetX(ewin), EoGetY(ewin),
		       ret[i].x, ret[i].y, Conf.place.slidespeedcleanup);
	else
	   EwinMove(ewin, ret[i].x, ret[i].y);
     }

   if (fixed)
      Efree(fixed);
   if (ret)
      Efree(ret);
   if (floating)
      Efree(floating);

 done:
   return;
}
