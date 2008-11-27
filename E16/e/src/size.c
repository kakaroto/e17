/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2003-2008 Kim Woelders
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
#include "screen.h"

#define MAX_ABSOLUTE     0	/* Fill screen */
#define MAX_AVAILABLE    1	/* Expand until don't cover */
#define MAX_CONSERVATIVE 2	/* Expand until something */
#define MAX_XINERAMA     3	/* Fill Xinerama screen */

void
MaxSizeHV(EWin * ewin, const char *resize_type, int hor, int ver)
{
   int                 x, y, w, h, x1, x2, y1, y2, type, bl, br, bt, bb;
   EWin               *const *lst, *pe;
   int                 i, num;

   if (!ewin)
      return;

   if (ewin->state.inhibit_max_hor && hor)
      return;
   if (ewin->state.inhibit_max_ver && ver)
      return;

   if (ewin->state.maximized_horz || ewin->state.maximized_vert)
     {
	EwinMoveResize(ewin, ewin->save_max.x, ewin->save_max.y,
		       ewin->save_max.w, ewin->save_max.h);
	ewin->save_max.x = EoGetX(ewin);
	ewin->save_max.y = EoGetY(ewin);
	ewin->save_max.w = ewin->client.w;
	ewin->save_max.h = ewin->client.h;
	ewin->state.maximized_horz = 0;
	ewin->state.maximized_vert = 0;
	goto done;
     }

   type = MAX_ABSOLUTE;		/* Select default */
   if (!resize_type || !resize_type[0])
      type = Conf.movres.mode_maximize_default;
   else if (!strcmp(resize_type, "absolute"))
      type = MAX_ABSOLUTE;
   else if (!strcmp(resize_type, "available"))
      type = MAX_AVAILABLE;
   else if (!strcmp(resize_type, "conservative"))
      type = MAX_CONSERVATIVE;
   else if (!strcmp(resize_type, "xinerama"))
      type = MAX_XINERAMA;

   /* Default is no change */
   x = EoGetX(ewin);
   y = EoGetY(ewin);
   h = EoGetH(ewin);
   w = EoGetW(ewin);

   switch (type)
     {
     case MAX_XINERAMA:
	if (hor)
	  {
	     x = 0;
	     w = WinGetW(VROOT);
	     ewin->state.maximized_horz = 1;
	  }
	if (ver)
	  {
	     y = 0;
	     h = WinGetH(VROOT);
	     ewin->state.maximized_vert = 1;
	  }
	break;

     default:
     case MAX_ABSOLUTE:
     case MAX_AVAILABLE:
     case MAX_CONSERVATIVE:
	ScreenGetAvailableArea(x + w / 2, y + h / 2, &x1, &y1, &x2, &y2);
	x2 += x1;
	y2 += y1;

	if (Conf.movres.dragbar_nocover && type != MAX_ABSOLUTE)
	  {
	     /* Leave room for the dragbar */
	     switch (Conf.desks.dragdir)
	       {
	       case 0:		/* left */
		  if (x1 < Conf.desks.dragbar_width)
		     x1 = Conf.desks.dragbar_width;
		  break;

	       case 1:		/* right */
		  if (x2 > WinGetW(VROOT) - Conf.desks.dragbar_width)
		     x2 = WinGetW(VROOT) - Conf.desks.dragbar_width;
		  break;

	       case 2:		/* top */
		  if (y1 < Conf.desks.dragbar_width)
		     y1 = Conf.desks.dragbar_width;
		  break;

	       case 3:		/* bottom */
		  if (y2 > WinGetH(VROOT) - Conf.desks.dragbar_width)
		     y2 = WinGetH(VROOT) - Conf.desks.dragbar_width;
		  break;

	       default:
		  break;
	       }
	  }

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

	if (ver)
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
	     h = y2 - y1;

	     ewin->state.maximized_vert = 1;
	  }

	if (hor)
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
	     w = x2 - x1;

	     ewin->state.maximized_horz = 1;
	  }

	break;
     }

   EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);
   w -= (bl + br);
   if (w < 10)
      w = 10;
   h -= (bt + bb);
   if (h < 10)
      h = 10;

   ewin->save_max.x = EoGetX(ewin);
   ewin->save_max.y = EoGetY(ewin);
   ewin->save_max.w = ewin->client.w;
   ewin->save_max.h = ewin->client.h;

   ewin->state.maximizing = 1;
   EwinMoveResize(ewin, x, y, w, h);
   ewin->state.maximizing = 0;
 done:
   HintsSetWindowState(ewin);
}
