/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2003-2006 Kim Woelders
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
#include "ewins.h"
#include "hints.h"

#define MAX_HOR 0x1
#define MAX_VER 0x2

#define MAX_ABSOLUTE     0	/* Fill screen */
#define MAX_AVAILABLE    1	/* Expand until don't cover */
#define MAX_CONSERVATIVE 2	/* Expand until something */
#define MAX_XINERAMA     3	/* Fill Xinerama screen */

static void
MaxSizeHV(EWin * ewin, const char *resize_type, int direction)
{
   int                 x, y, w, h, x1, x2, y1, y2, type, bl, br, bt, bb;
   EWin               *const *lst, *pe;
   int                 i, num;

   if (!ewin)
      return;

   if (ewin->state.maximized_horz || ewin->state.maximized_vert)
     {
	EwinMoveResize(ewin, ewin->lx, ewin->ly, ewin->lw, ewin->lh);
	ewin->lx = EoGetX(ewin);
	ewin->ly = EoGetY(ewin);
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	ewin->state.maximized_horz = 0;
	ewin->state.maximized_vert = 0;
	goto done;
     }

   if (ewin->state.inhibit_max_hor && (direction & MAX_HOR))
      return;
   if (ewin->state.inhibit_max_ver && (direction & MAX_VER))
      return;

   type = MAX_ABSOLUTE;		/* Select default */
   if (!resize_type)
      ;
   else if (!strcmp(resize_type, "absolute"))
      type = MAX_ABSOLUTE;
   else if (!strcmp(resize_type, "available"))
      type = MAX_AVAILABLE;
   else if (!strcmp(resize_type, "conservative"))
      type = MAX_CONSERVATIVE;
   else if (!strcmp(resize_type, "xinerama"))
      type = MAX_XINERAMA;

   /* Default is no change */
   y = EoGetY(ewin);
   x = EoGetX(ewin);
   h = ewin->client.h;
   w = ewin->client.w;
   EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);

   switch (type)
     {
     case MAX_XINERAMA:
	if (direction & MAX_HOR)
	  {
	     x = 0;
	     w = VRoot.w - bl - br;
	  }
	if (direction & MAX_VER)
	  {
	     y = 0;
	     h = VRoot.h - bt - bb;
	  }
	break;

     case MAX_ABSOLUTE:
     case MAX_AVAILABLE:
     case MAX_CONSERVATIVE:
	ScreenGetAvailableArea(EoGetX(ewin), EoGetY(ewin), &x1, &y1, &x2, &y2);
	x2 += x1;
	y2 += y1;

	if (type == MAX_ABSOLUTE)
	  {
	     /* Simply ignore all windows */
	     lst = NULL;
	     num = 0;
	  }
	else
	  {
	     lst = EwinListGetAll(&num);
	  }

	if (direction & MAX_VER)
	  {
	     for (i = 0; i < num; i++)
	       {
		  pe = lst[i];
		  if (pe == ewin ||
		      pe->state.iconified || EoIsFloating(pe) ||
		      pe->props.ignorearrange ||
		      (EoGetDesk(ewin) != EoGetDesk(pe) && !EoIsSticky(pe)) ||
		      (pe->type & (EWIN_TYPE_DIALOG | EWIN_TYPE_MENU)) ||
		      (type == MAX_AVAILABLE && !pe->props.never_use_area) ||
		      !SPANS_COMMON(x, w, EoGetX(pe), EoGetW(pe)))
		     continue;

		  if (((EoGetY(pe) + EoGetH(pe)) <= y)
		      && ((EoGetY(pe) + EoGetH(pe)) >= y1))
		     y1 = EoGetY(pe) + EoGetH(pe);
		  else if (((y + h) <= EoGetY(pe)) && (y2 >= EoGetY(pe)))
		     y2 = EoGetY(pe);
	       }
	     y = y1;
	     h = y2 - y1 - (bt + bb);

	     ewin->state.maximized_vert = 1;
	  }

	if (direction & MAX_HOR)
	  {
	     for (i = 0; i < num; i++)
	       {
		  pe = lst[i];
		  if (pe == ewin ||
		      pe->state.iconified || EoIsFloating(pe) ||
		      pe->props.ignorearrange ||
		      (EoGetDesk(ewin) != EoGetDesk(pe) && !EoIsSticky(pe)) ||
		      (pe->type & (EWIN_TYPE_DIALOG | EWIN_TYPE_MENU)) ||
		      (type == MAX_AVAILABLE && !pe->props.never_use_area) ||
		      !SPANS_COMMON(y, h, EoGetY(pe), EoGetH(pe)))
		     continue;

		  if (((EoGetX(pe) + EoGetW(pe)) <= x)
		      && ((EoGetX(pe) + EoGetW(pe)) >= x1))
		     x1 = EoGetX(pe) + EoGetW(pe);
		  else if (((x + w) <= EoGetX(pe)) && (x2 >= EoGetX(pe)))
		     x2 = EoGetX(pe);
	       }
	     x = x1;
	     w = x2 - x1 - (bl + br);

	     ewin->state.maximized_horz = 1;
	  }

	break;
     }

   ewin->lx = EoGetX(ewin);
   ewin->ly = EoGetY(ewin);
   ewin->lw = ewin->client.w;
   ewin->lh = ewin->client.h;
   ewin->state.maximizing = 1;
   EwinMoveResize(ewin, x, y, w, h);
   ewin->state.maximizing = 0;
 done:
   HintsSetWindowState(ewin);
}

void
MaxWidth(EWin * ewin, const char *resize_type)
{
   MaxSizeHV(ewin, resize_type, MAX_HOR);
}

void
MaxHeight(EWin * ewin, const char *resize_type)
{
   MaxSizeHV(ewin, resize_type, MAX_VER);
}

void
MaxSize(EWin * ewin, const char *resize_type)
{
   MaxSizeHV(ewin, resize_type, MAX_HOR | MAX_VER);
}
