/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2003 Kim Woelders
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

#define MAX_HOR 0x1
#define MAX_VER 0x2

#define MAX_ABSOLUTE     0
#define MAX_AVAILABLE    1
#define MAX_CONSERVATIVE 2

static void
MaxSizeHV(EWin * ewin, char *resize_type, int direction)
{
   int                 x, y, w, h, x1, x2, y1, y2, type;
   EWin              **lst, *pe;
   int                 i, num;

   if (!ewin)
      return;

   if (ewin->toggle)
     {
	MoveResizeEwin(ewin, ewin->lx, ewin->ly, ewin->lw, ewin->lh);
	ewin->lx = ewin->x;
	ewin->ly = ewin->y;
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	ewin->toggle = 0;
	goto exit;
     }

   if ((resize_type) && (!strcmp(resize_type, "available")))
      type = MAX_AVAILABLE;
   else if ((resize_type) && (!strcmp(resize_type, "conservative")))
      type = MAX_CONSERVATIVE;
   else
      type = MAX_ABSOLUTE;

   /* Default is no change */
   y = ewin->y;
   h = ewin->h;
   x = ewin->x;
   w = ewin->w;

   switch (type)
     {
     case MAX_ABSOLUTE:
	if (direction & MAX_HOR)
	  {
	     x = 0;
	     w = root.w - ewin->border->border.left -
		ewin->border->border.right;
	  }
	if (direction & MAX_VER)
	  {
	     y = 0;
	     h = root.h - ewin->border->border.top -
		ewin->border->border.bottom;
	  }
	break;

     case MAX_CONSERVATIVE:
     case MAX_AVAILABLE:
	lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	if (!lst)
	   break;

	ScreenGetGeometry(ewin->x, ewin->y, &x1, &y1, &x2, &y2);
	x2 += x1;
	y2 += y1;

#if ENABLE_KDE
	if (mode.kde_support)
	  {
	     if (x1 < mode.kde_x1)
		x1 = mode.kde_x1;
	     if (x2 > mode.kde_x2)
		x2 = mode.kde_x2;
	     if (y1 < mode.kde_y1)
		y1 = mode.kde_y1;
	     if (y2 > mode.kde_y2)
		y2 = mode.kde_y2;
	  }
#endif

	if (direction & MAX_VER)
	  {
	     for (i = 0; i < num; i++)
	       {
		  pe = lst[i];
		  if (pe == ewin ||
		      pe->iconified ||
		      pe->floating ||
		      pe->ignorearrange ||
		      (ewin->desktop != pe->desktop && !pe->sticky) ||
		      (pe->type & (EWIN_TYPE_DIALOG | EWIN_TYPE_MENU)) ||
		      (type == MAX_AVAILABLE && pe->never_use_area) ||
		      !SPANS_COMMON(x, w, pe->x, pe->w))
		     continue;

		  if (((pe->y + pe->h) <= y) && ((pe->y + pe->h) >= y1))
		     y1 = pe->y + pe->h;
		  else if (((y + h) <= pe->y) && (y2 >= pe->y))
		     y2 = pe->y;
	       }
	     y = y1;
	     h = y2 - y1 - (ewin->border->border.top +
			    ewin->border->border.bottom);
	  }

	if (direction & MAX_HOR)
	  {
	     for (i = 0; i < num; i++)
	       {
		  pe = lst[i];
		  if (pe == ewin ||
		      pe->iconified ||
		      pe->floating ||
		      pe->ignorearrange ||
		      (ewin->desktop != pe->desktop && !pe->sticky) ||
		      (pe->type & (EWIN_TYPE_DIALOG | EWIN_TYPE_MENU)) ||
		      (type == MAX_AVAILABLE && pe->never_use_area) ||
		      !SPANS_COMMON(y, h, pe->y, pe->h))
		     continue;

		  if (((pe->x + pe->w) <= x) && ((pe->x + pe->w) >= x1))
		     x1 = pe->x + pe->w;
		  else if (((x + w) <= pe->x) && (x2 >= pe->x))
		     x2 = pe->x;
	       }
	     x = x1;
	     w = x2 - x1 - (ewin->border->border.left +
			    ewin->border->border.right);
	  }

	Efree(lst);
	break;
     }

   ewin->lx = ewin->x;
   ewin->ly = ewin->y;
   ewin->lw = ewin->client.w;
   ewin->lh = ewin->client.h;
   MoveResizeEwin(ewin, x, y, w, h);
   ewin->toggle = 1;

 exit:;
#if ENABLE_KDE
   if (mode.kde_support)
      KDE_UpdateClient(ewin);
#endif
}

void
MaxWidth(EWin * ewin, char *resize_type)
{
   MaxSizeHV(ewin, resize_type, MAX_HOR);
}

void
MaxHeight(EWin * ewin, char *resize_type)
{
   MaxSizeHV(ewin, resize_type, MAX_VER);
}

void
MaxSize(EWin * ewin, char *resize_type)
{
   MaxSizeHV(ewin, resize_type, MAX_HOR | MAX_VER);
}
