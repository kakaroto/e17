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
#include "buttons.h"
#include "desktops.h"
#include "ewins.h"
#include "groups.h"

static int
ArrangeAddToList(int **array, int current_size, int value)
{
   int                 i, j;

   for (i = 0; i < current_size; i++)
     {
	if (value < (*array)[i])
	  {
	     for (j = current_size; j > i; j--)
		(*array)[j] = (*array)[j - 1];
	     (*array)[i] = value;
	     return current_size + 1;
	  }
	else if (value == (*array)[i])
	   return current_size;
     }
   (*array)[current_size] = value;
   return current_size + 1;
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

void
ArrangeRects(RectBox * fixed, int fixed_count, RectBox * floating,
	     int floating_count, RectBox * sorted, int startx, int starty,
	     int width, int height, int policy, char initial_window)
{
   int                 num_sorted = 0;
   int                 xsize = 0, ysize = 0;
   int                *xarray = NULL, *yarray = NULL;
   int                *leftover = NULL;
   int                 i, j, k, x, y, x1, x2, y1, y2;
   unsigned char      *filled = NULL;
   RectBox            *spaces = NULL;
   int                 num_spaces = 0;
   int                 sort;
   int                 a1, a2;
   int                 num_leftover = 0;

#define Filled(x,y) (filled[(y * (xsize - 1)) + x])

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
   if (floating_count)
      leftover = Emalloc(floating_count * sizeof(int));

   if (!xarray || !yarray || !filled || !spaces)
      goto done;

/* copy "fixed" rects into the sorted list */
   for (i = 0; i < fixed_count; i++)
     {
	sorted[num_sorted].data = fixed[i].data;
	sorted[num_sorted].x = fixed[i].x;
	sorted[num_sorted].y = fixed[i].y;
	sorted[num_sorted].w = fixed[i].w;
	sorted[num_sorted].h = fixed[i].h;
	sorted[num_sorted].p = fixed[i].p;
	num_sorted++;
     }
/* go through each floating rect in order and "fit" it in */
   for (i = 0; i < floating_count; i++)
     {
	xsize = 0;
	ysize = 0;
/* put all the sorted rects into the xy arrays */
	xsize = ArrangeAddToList(&xarray, xsize, startx);
	xsize = ArrangeAddToList(&xarray, xsize, width);
	ysize = ArrangeAddToList(&yarray, ysize, starty);
	ysize = ArrangeAddToList(&yarray, ysize, height);
	for (j = 0; j < num_sorted; j++)
	  {
	     if (sorted[j].x < width)
		xsize = ArrangeAddToList(&xarray, xsize, sorted[j].x);
	     if ((sorted[j].x + sorted[j].w) < width)
		xsize =
		   ArrangeAddToList(&xarray, xsize, sorted[j].x + sorted[j].w);
	     if (sorted[j].y < height)
		ysize = ArrangeAddToList(&yarray, ysize, sorted[j].y);
	     if ((sorted[j].y + sorted[j].h) < height)
		ysize =
		   ArrangeAddToList(&yarray, ysize, sorted[j].y + sorted[j].h);
	  }
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
	       {
		  if (sorted[j].x == xarray[k])
		    {
		       x1 = k;
		       x2 = k;
		    }
		  if (sorted[j].x + sorted[j].w == xarray[k + 1])
		     x2 = k;
	       }
	     for (k = 0; k < ysize - 1; k++)
	       {
		  if (sorted[j].y == yarray[k])
		    {
		       y1 = k;
		       y2 = k;
		    }
		  if (sorted[j].y + sorted[j].h == yarray[k + 1])
		     y2 = k;
	       }
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
	num_spaces = 0;
/* create list of all "spaces" */
	for (y = 0; y < ysize - 1; y++)
	  {
	     for (x = 0; x < xsize - 1; x++)
	       {
/* if the square is empty (lowe prioiryt suares filled) "grow" the space */
		  if (Filled(x, y) <= floating[i].p)
		    {
		       int                 can_expand_x = 1;
		       int                 can_expand_y = 1;

		       x1 = x + 1;
		       y1 = y + 1;
		       Filled(x, y) = 100;
		       if (x >= xsize - 2)
			  can_expand_x = 0;
		       if (y >= ysize - 2)
			  can_expand_y = 0;
		       while ((can_expand_x) || (can_expand_y))
			 {
			    if (x1 >= xsize - 1)
			       can_expand_x = 0;
			    if (y1 >= ysize - 1)
			       can_expand_y = 0;
			    if (can_expand_x)
			      {
				 for (j = y; j < y1; j++)
				   {
				      if (Filled(x1, j) >= floating[i].p + 1)
					 can_expand_x = 0;
				   }
			      }
			    if (can_expand_x)
			       x1++;
			    if (can_expand_y)
			      {
				 for (j = x; j < x1; j++)
				   {
				      if (Filled(j, y1) >= floating[i].p + 1)
					 can_expand_y = 0;
				   }
			      }
			    if (can_expand_y)
			       y1++;
			 }
		       spaces[num_spaces].x = xarray[x];
		       spaces[num_spaces].y = yarray[y];
		       spaces[num_spaces].w = xarray[x1] - xarray[x];
		       spaces[num_spaces].h = yarray[y1] - yarray[y];
		       spaces[num_spaces].p = 0;
		       num_spaces++;
		    }
	       }
	  }
/* find the first space that fits */
	k = -1;
	sort = 0x7fffffff;
	for (j = 0; j < num_spaces; j++)
	  {
	     if ((spaces[j].w >= floating[i].w)
		 && (spaces[j].h >= floating[i].h))
	       {
		  if (spaces[j].x >= startx)
		    {
		       if ((spaces[j].x + spaces[j].w) <= width)
			 {
			    if (spaces[j].y >= starty)
			      {
				 if ((spaces[j].y + spaces[j].h) <= height)
				   {
				      if (policy == ARRANGE_BY_POSITION)
					{
					   a1 = (spaces[j].x +
						 (spaces[j].w >> 1)) -
					      (floating[i].x +
					       (floating[i].w >> 1));
					   a2 = (spaces[j].y +
						 (spaces[j].h >> 1)) -
					      (floating[i].y +
					       (floating[i].h >> 1));
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
					   j = num_spaces;
					}
				   }
			      }
			 }
		    }
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

   /* ok we cant fit everything in this baby.... time to fit */
   /* the leftovers into the leftover space */
   for (i = 0; i < num_leftover; i++)
     {
	xsize = 0;
	ysize = 0;
	/* put all the sorted rects into the xy arrays */
	xsize = ArrangeAddToList(&xarray, xsize, 0);
	xsize = ArrangeAddToList(&xarray, xsize, width);
	ysize = ArrangeAddToList(&yarray, ysize, 0);
	ysize = ArrangeAddToList(&yarray, ysize, height);
	for (j = 0; j < num_sorted; j++)
	  {
	     if (sorted[j].x < width)
		xsize = ArrangeAddToList(&xarray, xsize, sorted[j].x);
	     if ((sorted[j].x + sorted[j].w) < width)
		xsize =
		   ArrangeAddToList(&xarray, xsize, sorted[j].x + sorted[j].w);
	     if (sorted[j].y < height)
		ysize = ArrangeAddToList(&yarray, ysize, sorted[j].y);
	     if ((sorted[j].y + sorted[j].h) < height)
		ysize =
		   ArrangeAddToList(&yarray, ysize, sorted[j].y + sorted[j].h);
	  }
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
	       {
		  if (sorted[j].x == xarray[k])
		    {
		       x1 = k;
		       x2 = k;
		    }
		  if (sorted[j].x + sorted[j].w == xarray[k + 1])
		     x2 = k;
	       }
	     for (k = 0; k < ysize - 1; k++)
	       {
		  if (sorted[j].y == yarray[k])
		    {
		       y1 = k;
		       y2 = k;
		    }
		  if (sorted[j].y + sorted[j].h == yarray[k + 1])
		     y2 = k;
	       }
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
	num_spaces = 0;
	/* create list of all "spaces" */
	for (y = 0; y < ysize - 1; y++)
	  {
	     for (x = 0; x < xsize - 1; x++)
	       {
		  /* if the square is empty "grow" the space */
		  if (Filled(x, y) <= floating[leftover[i]].p)
		    {
		       int                 can_expand_x = 1;
		       int                 can_expand_y = 1;
		       char                fitswin = 1;

		       x1 = x + 1;
		       y1 = y + 1;
		       if (x >= xsize - 2)
			  can_expand_x = 0;
		       if (y >= ysize - 2)
			  can_expand_y = 0;
		       while ((can_expand_x) || (can_expand_y))
			 {
			    if (x1 >= xsize - 1)
			       can_expand_x = 0;
			    if (y1 >= ysize - 1)
			       can_expand_y = 0;
			    if (can_expand_x)
			      {
				 for (j = y; j < y1; j++)
				   {
				      if (Filled(x1, j) >
					  floating[leftover[i]].p + 1)
					 can_expand_x = 0;
				   }
			      }
			    if (can_expand_x)
			       x1++;
			    if (can_expand_y)
			      {
				 for (j = x; j < x1; j++)
				   {
				      if (Filled(j, y1) >
					  floating[leftover[i]].p + 1)
					 can_expand_y = 0;
				   }
			      }
			    if (can_expand_y)
			       y1++;
			 }
		       spaces[num_spaces].x = xarray[x];
		       spaces[num_spaces].y = yarray[y];
		       spaces[num_spaces].w = xarray[x1] - xarray[x];
		       spaces[num_spaces].h = yarray[y1] - yarray[y];
		       spaces[num_spaces].p = fitswin;
		       num_spaces++;
		    }
	       }
	  }
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
	/* if there's a small space ... */
	if (k >= 0)
	  {
	     sorted[num_sorted].x = spaces[k].x;
	     sorted[num_sorted].y = spaces[k].y;
	     sorted[num_sorted].data = floating[leftover[i]].data;
	     sorted[num_sorted].w = floating[leftover[i]].w;
	     sorted[num_sorted].h = floating[leftover[i]].h;
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
	else
	  {
	     /* there is no room - put it centered */
	     /* (but dont put top left off screen) */
	     sorted[num_sorted].data = floating[leftover[i]].data;
	     sorted[num_sorted].x = (width - floating[leftover[i]].w) / 2;
	     sorted[num_sorted].y = (height - floating[leftover[i]].h) / 2;
	     sorted[num_sorted].w = floating[leftover[i]].w;
	     sorted[num_sorted].h = floating[leftover[i]].h;
	     if (sorted[num_sorted].x < startx)
		sorted[num_sorted].x = startx;
	     if (sorted[num_sorted].y < starty)
		sorted[num_sorted].y = starty;
	     num_sorted++;
	  }
     }
   for (i = 0; i < num_sorted; i++)
     {
	if ((sorted[i].x + sorted[i].w) > width)
	   sorted[i].x = VRoot.w - sorted[i].w;
	if ((sorted[i].y + sorted[i].h) > height)
	   sorted[i].y = VRoot.h - sorted[i].h;
	if (sorted[i].x < startx)
	   sorted[i].x = startx;
	if (sorted[i].y < starty)
	   sorted[i].y = starty;
     }

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

static int
EWinIsOnViewport(EWin * ewin, Desk * dsk)
{
   int                 ax, ay;

   if (EoIsSticky(ewin))
      return 1;

   DeskGetArea(dsk, &ax, &ay);
   if (EoGetDesk(ewin) == dsk && ewin->area_x == ax && ewin->area_y == ay)
      return 1;

   return 0;
}

void
ArrangeEwinXY(EWin * ewin, int *px, int *py)
{
   EWin               *const *lst;
   Button            **blst;
   int                 i, j, num;
   RectBox            *fixed, *ret, newrect;

   fixed = NULL;
   ret = NULL;

   lst = EwinListGetAll(&num);
   if ((lst) && (num > 0))
     {
	fixed = Emalloc(sizeof(RectBox) * num);
	j = 0;
	for (i = 0; i < num; i++)
	  {
	     EWin               *e = lst[i];

	     if (e == ewin ||
		 e->state.iconified || e->props.ignorearrange ||
		 EoGetLayer(e) == 0 || !EWinIsOnViewport(e, EoGetDesk(ewin)))
		continue;

	     fixed[j].data = e;
	     fixed[j].x = EoGetX(e);
	     fixed[j].y = EoGetY(e);
	     fixed[j].w = EoGetW(e);
	     fixed[j].h = EoGetH(e);
	     if (fixed[j].x < 0)
	       {
		  fixed[j].w += fixed[j].x;
		  fixed[j].x = 0;
	       }
	     if ((fixed[j].x + fixed[j].w) > VRoot.w)
		fixed[j].w = VRoot.w - fixed[j].x;
	     if (fixed[j].y < 0)
	       {
		  fixed[j].h += fixed[j].y;
		  fixed[j].y = 0;
	       }
	     if ((fixed[j].y + fixed[j].h) > VRoot.h)
		fixed[j].h = VRoot.h - fixed[j].y;
	     if ((fixed[j].w <= 0) || (fixed[j].h <= 0))
		continue;

	     if (e->props.never_use_area)
		fixed[j].p = 50;
	     else
		fixed[j].p = EoGetLayer(e);
	     j++;
	  }

	blst = ButtonsGetList(&num);
	if (blst)
	  {
	     fixed = Erealloc(fixed, sizeof(RectBox) * (num + j));
	     for (i = 0; i < num; i++)
	       {
		  if (ButtonGetInfo(blst[i], &fixed[j], EoGetDesk(ewin)))
		     continue;

		  if (fixed[j].x < 0)
		    {
		       fixed[j].w += fixed[j].x;
		       fixed[j].x = 0;
		    }
		  if ((fixed[j].x + fixed[j].w) > VRoot.w)
		     fixed[j].w = VRoot.w - fixed[j].x;
		  if (fixed[j].y < 0)
		    {
		       fixed[j].h += fixed[j].y;
		       fixed[j].y = 0;
		    }
		  if ((fixed[j].y + fixed[j].h) > VRoot.h)
		     fixed[j].h = VRoot.h - fixed[j].y;
		  if ((fixed[j].w <= 0) || (fixed[j].h <= 0))
		     continue;

		  if (fixed[j].p)	/* Sticky */
		     fixed[j].p = 1;
		  else
		     fixed[j].p = 0;
		  j++;
	       }
	     Efree(blst);
	  }
	ret = Emalloc(sizeof(RectBox) * (j + 1));
	newrect.data = ewin;
	newrect.x = 0;
	newrect.y = 0;
	newrect.w = EoGetW(ewin);
	newrect.h = EoGetH(ewin);
	newrect.p = EoGetLayer(ewin);
	ArrangeRects(fixed, j, &newrect, 1, ret,
		     0, 0, VRoot.w, VRoot.h, ARRANGE_BY_SIZE, 1);

	for (i = 0; i < j + 1; i++)
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
   else
     {
	ArrangeEwinCenteredXY(ewin, px, py);
     }
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
   int                 i, j, k, num, speed, ax, ay;
   RectBox            *fixed, *ret, *floating;
   char                doslide;
   EWin               *const *lst, *ewin;
   Button            **blst;

   type = params;
   method = ARRANGE_BY_SIZE;
   speed = Conf.place.slidespeedcleanup;
   doslide = Conf.place.cleanupslide;

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

   fixed = NULL;
   floating = Emalloc(sizeof(RectBox) * num);
   ret = Emalloc(sizeof(RectBox) * (num));
   j = 0;
   k = 0;
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];

	DeskGetArea(EoGetDesk(ewin), &ax, &ay);

	if ((EoGetDesk(ewin) == DesksGetCurrent()) &&
	    (!EoIsSticky(ewin)) && (!EoIsFloating(ewin)) &&
	    (!ewin->state.iconified) && (!ewin->props.ignorearrange) &&
	    (ewin->type != EWIN_TYPE_MENU) &&
	    (ewin->area_x == ax) && (ewin->area_y == ay))
	  {
	     floating[j].data = lst[i];
	     floating[j].x = EoGetX(ewin);
	     floating[j].y = EoGetY(ewin);
	     floating[j].w = EoGetW(ewin);
	     floating[j].p = EoGetLayer(ewin);
	     floating[j++].h = EoGetH(ewin);
	  }
	else if ((EoGetDesk(ewin) == DesksGetCurrent()) &&
		 (EoGetLayer(ewin) != 4) && (EoGetLayer(ewin) != 0) &&
		 (ewin->type != EWIN_TYPE_MENU))
	  {
	     fixed = Erealloc(fixed, sizeof(RectBox) * (k + 1));
	     fixed[k].data = lst[i];
	     fixed[k].x = EoGetX(ewin);
	     fixed[k].y = EoGetY(ewin);
	     fixed[k].w = EoGetW(ewin);
	     fixed[k].h = EoGetH(ewin);
	     if (fixed[k].x < 0)
	       {
		  fixed[k].x += fixed[k].w;
		  fixed[k].x = 0;
	       }
	     if ((fixed[k].x + fixed[k].w) > VRoot.w)
		fixed[k].w = VRoot.w - fixed[k].x;
	     if (fixed[k].y < 0)
	       {
		  fixed[k].y += fixed[k].h;
		  fixed[k].y = 0;
	       }
	     if ((fixed[k].y + fixed[k].h) > VRoot.h)
		fixed[k].h = VRoot.h - fixed[k].y;
	     if ((fixed[k].w > 0) && (fixed[k].h > 0))
	       {
		  if (!ewin->props.never_use_area)
		     fixed[k].p = EoGetLayer(ewin);
		  else
		     fixed[k].p = 99;
		  k++;
	       }
	  }
     }

   blst = ButtonsGetList(&num);
   if (blst)
     {
	fixed = Erealloc(fixed, sizeof(RectBox) * (num + k));
	ret = Erealloc(ret, sizeof(RectBox) * ((num + j) + 1 + k));
	for (i = 0; i < num; i++)
	  {
	     if (ButtonGetInfo(blst[i], &fixed[k], DesksGetCurrent()))
		continue;

	     if (fixed[k].x < 0)
	       {
		  fixed[k].x += fixed[k].w;
		  fixed[k].x = 0;
	       }
	     if ((fixed[k].x + fixed[k].w) > VRoot.w)
		fixed[k].w = VRoot.w - fixed[k].x;
	     if (fixed[k].y < 0)
	       {
		  fixed[k].y += fixed[k].h;
		  fixed[k].y = 0;
	       }
	     if ((fixed[k].y + fixed[k].h) > VRoot.h)
		fixed[k].h = VRoot.h - fixed[k].y;
	     if ((fixed[k].w <= 0) || (fixed[k].h <= 0))
		continue;

	     if (fixed[k].p)	/* Sticky */
		fixed[k].p = 50;
	     else
		fixed[k].p = 0;
	     k++;
	  }
	Efree(blst);
     }

   ArrangeRects(fixed, k, floating, j, ret, 0, 0, VRoot.w, VRoot.h, method, 0);

   for (i = 0; i < (j + k); i++)
     {
	if (ret[i].data)
	  {
	     if (doslide)
	       {
		  ewin = (EWin *) ret[i].data;
		  if (ewin)
		    {
		       if ((EoGetX(ewin) != ret[i].x)
			   || (EoGetY(ewin) != ret[i].y))
			 {
			    SlideEwinTo(ewin, EoGetX(ewin), EoGetY(ewin),
					ret[i].x, ret[i].y, speed);
			 }
		    }
	       }
	     else
	       {
		  ewin = (EWin *) ret[i].data;
		  if (ewin)
		    {
		       if ((EoGetX(ewin) != ret[i].x)
			   || (EoGetY(ewin) != ret[i].y))
			  EwinMove((EWin *) ret[i].data, ret[i].x, ret[i].y);
		    }
	       }
	  }
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
