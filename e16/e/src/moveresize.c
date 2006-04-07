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
#include "emodule.h"
#include "ewins.h"
#include "groups.h"
#include "hints.h"
#include "xwin.h"

static struct
{
   EWin               *ewin;
   int                 mode;
   int                 start_x, start_y;
   int                 win_x, win_y, win_w, win_h;
   int                 swapcoord_x, swapcoord_y;
   int                 resize_detail;
} Mode_mr;

void
EwinShapeSet(EWin * ewin)
{
   int                 bl, br, bt, bb;

   ewin->shape_x = EoGetX(ewin);
   ewin->shape_y = EoGetY(ewin);

   if (ewin->state.shaded)
     {
	EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);
	ewin->shape_w = EoGetW(ewin) - (bl + br);
	ewin->shape_h = EoGetH(ewin) - (bt + bb);
     }
   else
     {
	ewin->shape_w = ewin->client.w;
	ewin->shape_h = ewin->client.h;
     }
}

int
ActionMoveStart(EWin * ewin, int grab, char constrained, int nogroup)
{
   EWin              **gwins;
   int                 i, num;

   if (!ewin || ewin->state.inhibit_move)
      return 0;

   Mode_mr.ewin = ewin;

   SoundPlay("SOUND_MOVE_START");

   if (grab)
     {
	GrabPointerRelease();
	GrabPointerSet(VRoot.win, ECSR_ACT_MOVE, 1);
     }

   Mode.mode = MODE_MOVE_PENDING;
   Mode.constrained = constrained;

   Mode_mr.win_x = Mode.events.x - (EoGetX(ewin) + EoGetX(EoGetDesk(ewin)));
   Mode_mr.win_y = Mode.events.y - (EoGetY(ewin) + EoGetY(EoGetDesk(ewin)));

   RaiseEwin(ewin);
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, nogroup
				      || Mode.move.swap, &num);

   if (Conf.movres.mode_move < 0 || Conf.movres.mode_move > 5)
      Conf.movres.mode_move = 0;
   Mode_mr.mode = Conf.movres.mode_move;
   if (num > 1 && Conf.movres.mode_move == 5)
      Mode_mr.mode = 0;

   for (i = 0; i < num; i++)
     {
	EwinShapeSet(gwins[i]);
	EwinFloatAt(gwins[i], EoGetX(gwins[i]), EoGetY(gwins[i]));
	if (Mode_mr.mode == 0)
	  {
	     ewin->state.moving = 1;
	     EwinUpdateOpacity(gwins[i]);
	  }
     }
   Efree(gwins);

   Mode_mr.swapcoord_x = EoGetX(ewin);
   Mode_mr.swapcoord_y = EoGetY(ewin);

   return 0;
}

int
ActionMoveEnd(EWin * ewin)
{
   EWin              **gwins;
   int                 num, i;
   Desk               *d1, *d2;

   if (ewin && ewin != Mode_mr.ewin)
      return 0;

   GrabPointerRelease();

   SoundPlay("SOUND_MOVE_STOP");

   ewin = Mode_mr.ewin;
   if (!ewin)
      goto done;

   ewin->state.show_coords = 0;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, Mode.nogroup
				      || Mode.move.swap, &num);

   if (Mode.mode == MODE_MOVE)
     {
	for (i = 0; i < num; i++)
	   DrawEwinShape(gwins[i], Mode_mr.mode,
			 gwins[i]->shape_x, gwins[i]->shape_y,
			 gwins[i]->client.w, gwins[i]->client.h, 2);
     }
   Mode.mode = MODE_NONE;

   d2 = DesktopAt(Mode.events.x, Mode.events.y);

   for (i = 0; i < num; i++)
     {
	ewin = gwins[i];
	d1 = EoGetDesk(ewin);
	if (d2 == d1)
	   EwinUnfloatAt(ewin, d2, ewin->shape_x, ewin->shape_y);
	else
	   EwinUnfloatAt(ewin, d2,
			 ewin->shape_x - (EoGetX(d2) - EoGetX(d1)),
			 ewin->shape_y - (EoGetY(d2) - EoGetY(d1)));
	if (Mode_mr.mode == 0)
	  {
	     ewin->state.moving = 0;
	     EwinUpdateOpacity(ewin);
	  }
     }

   Efree(gwins);

   ESync();

 done:
   Mode.mode = MODE_NONE;
   Mode.nogroup = 0;
   Mode.move.swap = 0;
   Mode.place.doing_manual = 0;

   if (Mode_mr.mode > 0)
     {
	FocusEnable(1);
	EUngrabServer();
	ModulesSignal(ESIGNAL_ANIMATION_RESUME, NULL);
     }

   return 0;
}

int
ActionMoveSuspend(void)
{
   EWin               *ewin, **lst;
   int                 i, num;

   ewin = Mode_mr.ewin;
   if (!ewin)
      return 0;

   if (Mode.mode == MODE_MOVE_PENDING)
      return 0;

   /* If non opaque undraw our boxes */
   if (Mode_mr.mode > 0)
     {
	lst =
	   ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, Mode.nogroup,
				      &num);
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     DrawEwinShape(ewin, Mode_mr.mode, ewin->shape_x,
			   ewin->shape_y, ewin->client.w, ewin->client.h, 3);
	  }
	if (lst)
	   Efree(lst);

	EUngrabServer();
     }

   return 0;
}

int
ActionMoveResume(void)
{
   EWin               *ewin, **lst;
   int                 i, num;
   int                 x, y, fl, dx, dy;

   ewin = Mode_mr.ewin;
   if (!ewin)
      return 0;

   fl = (Mode_mr.mode == 5) ? 4 : 0;
   if (Mode.mode == MODE_MOVE_PENDING)
     {
	Mode.mode = MODE_MOVE;
	fl = 0;			/* This is the first time we draw it */
     }

   if (Mode_mr.mode > 0)
      EGrabServer();

   dx = Mode.events.x - Mode_mr.win_x - EoGetX(EoGetDesk(ewin)) - ewin->shape_x;
   dy = Mode.events.y - Mode_mr.win_y - EoGetY(EoGetDesk(ewin)) - ewin->shape_y;

   /* Redraw any windows that were in "move mode" */
   lst =
      ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];

	if (!EoIsFloating(ewin))
	   continue;

	x = ewin->shape_x + dx;
	y = ewin->shape_y + dy;
	DrawEwinShape(ewin, Mode_mr.mode, x, y,
		      ewin->client.w, ewin->client.h, fl);
     }
   if (lst)
      Efree(lst);

   return 0;
}

int
ActionResizeStart(EWin * ewin, int grab, int hv)
{
   int                 x, y, w, h, ww, hh;

   if (!ewin || ewin->state.inhibit_resize)
      return 0;

   Mode_mr.ewin = ewin;

   SoundPlay("SOUND_RESIZE_START");

   if (Conf.movres.mode_resize < 0 || Conf.movres.mode_resize > 4)
      Conf.movres.mode_resize = 0;
   if (Conf.movres.mode_resize > 0)
     {
	EGrabServer();
	ModulesSignal(ESIGNAL_ANIMATION_SUSPEND, NULL);
	/* Run idlers (stacking, border updates, ...) before drawing lines */
	ModulesSignal(ESIGNAL_IDLE, NULL);
     }
   else
     {
	ewin->state.resizing = 1;
	EwinUpdateOpacity(ewin);
     }

   if (grab)
     {
	GrabPointerRelease();
	GrabPointerSet(VRoot.win, ECSR_ACT_RESIZE, 1);
     }

   switch (hv)
     {
     case MODE_RESIZE:
	Mode.mode = hv;
	x = Mode.events.x - EoGetX(ewin);
	y = Mode.events.y - EoGetY(ewin);
	w = EoGetW(ewin) >> 1;
	h = EoGetH(ewin) >> 1;
	ww = EoGetW(ewin) / 6;
	hh = EoGetH(ewin) / 6;

	if ((x < w) && (y < h))
	   Mode_mr.resize_detail = 0;
	if ((x >= w) && (y < h))
	   Mode_mr.resize_detail = 1;
	if ((x < w) && (y >= h))
	   Mode_mr.resize_detail = 2;
	if ((x >= w) && (y >= h))
	   Mode_mr.resize_detail = 3;

	/* The following four if statements added on 07/22/04 by Josh Holtrop.
	 * They allow strictly horizontal or vertical resizing when the
	 * cursor is towards the middle of an edge of a window. */
	if ((abs(x - w) < (w >> 1)) && (y < hh))
	  {
	     Mode.mode = MODE_RESIZE_V;
	     Mode_mr.resize_detail = 0;
	  }
	else if ((abs(x - w) < (w >> 1)) && (y > (EoGetH(ewin) - hh)))
	  {
	     Mode.mode = MODE_RESIZE_V;
	     Mode_mr.resize_detail = 1;
	  }
	else if ((abs(y - h) < (h >> 1)) && (x < ww))
	  {
	     Mode.mode = MODE_RESIZE_H;
	     Mode_mr.resize_detail = 0;
	  }
	else if ((abs(y - h) < (h >> 1)) && (x > (EoGetW(ewin) - ww)))
	  {
	     Mode.mode = MODE_RESIZE_H;
	     Mode_mr.resize_detail = 1;
	  }
	break;

     case MODE_RESIZE_H:
	Mode.mode = hv;
	x = Mode.events.x - EoGetX(ewin);
	w = EoGetW(ewin) >> 1;
	if (x < w)
	   Mode_mr.resize_detail = 0;
	else
	   Mode_mr.resize_detail = 1;
	break;

     case MODE_RESIZE_V:
	Mode.mode = hv;
	y = Mode.events.y - EoGetY(ewin);
	h = EoGetH(ewin) >> 1;
	if (y < h)
	   Mode_mr.resize_detail = 0;
	else
	   Mode_mr.resize_detail = 1;
	break;
     }

   Mode_mr.start_x = Mode.events.x;
   Mode_mr.start_y = Mode.events.y;
   Mode_mr.win_x = EoGetX(ewin);
   Mode_mr.win_y = EoGetY(ewin);
   Mode_mr.win_w = ewin->client.w;
   Mode_mr.win_h = ewin->client.h;
   EwinShapeSet(ewin);
   ewin->state.show_coords = 1;
   DrawEwinShape(ewin, Conf.movres.mode_resize, EoGetX(ewin), EoGetY(ewin),
		 ewin->client.w, ewin->client.h, 0);

   return 0;
}

int
ActionResizeEnd(EWin * ewin)
{
   if (ewin && ewin != Mode_mr.ewin)
      return 0;

   Mode.mode = MODE_NONE;

   GrabPointerRelease();

   SoundPlay("SOUND_RESIZE_STOP");

   ewin = Mode_mr.ewin;
   if (!ewin)
      goto done;

   ewin->state.show_coords = 0;
   DrawEwinShape(ewin, Conf.movres.mode_resize, ewin->shape_x, ewin->shape_y,
		 ewin->client.w, ewin->client.h, 2);

   if (Conf.movres.mode_resize == 0)
     {
	ewin->state.resizing = 0;
	EwinUpdateOpacity(ewin);
     }
   else
     {
	if (ewin->state.shaded)
	   EwinOpMove(ewin, OPSRC_USER, ewin->shape_x, ewin->shape_y);
	else
	   EwinOpMoveResize(ewin, OPSRC_USER, ewin->shape_x, ewin->shape_y,
			    ewin->shape_w, ewin->shape_h);
     }

   ESync();

 done:
   if (Conf.movres.mode_resize > 0)
     {
	EUngrabServer();
	ModulesSignal(ESIGNAL_ANIMATION_RESUME, NULL);
     }

   return 0;
}

void
ActionMoveHandleMotion(void)
{
   int                 dx, dy, dd;
   EWin               *ewin, **gwins, *ewin1;
   int                 i, num;
   int                 ndx, ndy;
   int                 screen_snap_dist;
   char                jumpx, jumpy;
   int                 min_dx, max_dx, min_dy, max_dy;

   ewin = Mode_mr.ewin;
   if (!ewin)
      return;

   EdgeCheckMotion(Mode.events.x, Mode.events.y);

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE,
				      Mode.nogroup || Mode.move.swap, &num);

   if (Mode.mode == MODE_MOVE_PENDING)
     {
	if (Mode_mr.mode > 0)
	  {
	     EGrabServer();
	     FocusEnable(0);
	     ModulesSignal(ESIGNAL_ANIMATION_SUSPEND, NULL);
	  }

	if (Mode_mr.mode == 0 || num == 1)
	   ewin->state.show_coords = 1;

	for (i = 0; i < num; i++)
	  {
	     ewin1 = gwins[i];
	     DrawEwinShape(ewin1, Mode_mr.mode, EoGetX(ewin1),
			   EoGetY(ewin1), ewin1->client.w, ewin1->client.h, 0);
	  }
	Mode.mode = MODE_MOVE;
     }

   dx = Mode.events.x - Mode.events.px;
   dy = Mode.events.y - Mode.events.py;

   jumpx = 0;
   jumpy = 0;
   min_dx = dx;
   min_dy = dy;
   max_dx = dx;
   max_dy = dy;

   for (i = 0; i < num; i++)
     {
	ndx = dx;
	ndy = dy;
	/* make our ewin resist other ewins around the place */
	SnapEwin(gwins[i], dx, dy, &ndx, &ndy);
	if ((dx < 0) && (ndx <= 0))
	  {
	     if (ndx > min_dx)
		min_dx = ndx;
	     if (ndx < max_dx)
		max_dx = ndx;
	  }
	else if (ndx >= 0)
	  {
	     if (ndx < min_dx)
		min_dx = ndx;
	     if (ndx > max_dx)
		max_dx = ndx;
	  }
	if ((dy < 0) && (ndy <= 0))
	  {
	     if (ndy > min_dy)
		min_dy = ndy;
	     if (ndy < max_dy)
		max_dy = ndy;
	  }
	else if (ndy >= 0)
	  {
	     if (ndy < min_dy)
		min_dy = ndy;
	     if (ndy > max_dy)
		max_dy = ndy;
	  }
     }
   if (min_dx == dx)
      ndx = max_dx;
   else
      ndx = min_dx;
   if (min_dy == dy)
      ndy = max_dy;
   else
      ndy = min_dy;

   screen_snap_dist =
      Mode.constrained ? (VRoot.w + VRoot.h) : Conf.snap.screen_snap_dist;

   for (i = 0; i < num; i++)
     {
	ewin1 = gwins[i];

	/* jump out of snap horizontally */
	dd = ewin1->req_x - ewin1->shape_x;
	if (dd < 0)
	   dd = -dd;
	if ((ndx != dx) &&
	    (((ewin1->shape_x == 0) &&
	      (dd > screen_snap_dist)) ||
	     ((ewin1->shape_x == (VRoot.w - EoGetW(ewin1))) &&
	      (dd > screen_snap_dist)) ||
	     ((ewin1->shape_x != 0) &&
	      (ewin1->shape_x != (VRoot.w - EoGetW(ewin1)) &&
	       (dd > Conf.snap.edge_snap_dist)))))
	  {
	     jumpx = 1;
	     ndx = ewin1->req_x - ewin1->shape_x + dx;
	  }

	/* jump out of snap vertically */
	dd = ewin1->req_y - ewin1->shape_y;
	if (dd < 0)
	   dd = -dd;
	if ((ndy != dy) &&
	    (((ewin1->shape_y == 0) &&
	      (dd > screen_snap_dist)) ||
	     ((ewin1->shape_y == (VRoot.h - EoGetH(ewin1))) &&
	      (dd > screen_snap_dist)) ||
	     ((ewin1->shape_y != 0) &&
	      (ewin1->shape_y != (VRoot.h - EoGetH(ewin1)) &&
	       (dd > Conf.snap.edge_snap_dist)))))
	  {
	     jumpy = 1;
	     ndy = ewin1->req_y - ewin1->shape_y + dy;
	  }
     }

   for (i = 0; i < num; i++)
     {
	ewin1 = gwins[i];

	/* if its opaque move mode check to see if we have to float */
	/* the window above all desktops (reparent to root) */
	if (Mode_mr.mode == 0)
	  {
	     Desk               *dsk;

	     dsk = EoGetDesk(ewin1);
	     DetermineEwinFloat(ewin1, ndx, ndy);
	     if (dsk != EoGetDesk(ewin1))
	       {
		  ewin1->shape_x += EoGetX(dsk);
		  ewin1->shape_y += EoGetY(dsk);
		  ewin1->req_x += EoGetX(dsk);
		  ewin1->req_y += EoGetY(dsk);
	       }
	  }

	/* draw the new position of the window */
	DrawEwinShape(ewin1, Mode_mr.mode,
		      ewin1->shape_x + ndx, ewin1->shape_y + ndy,
		      ewin1->client.w, ewin1->client.h, 1);

	/* if we didnt jump the window after a resist at the edge */
	/* reset the requested x to be the prev. requested + delta */
	/* if we did jump set requested to current shape position */
	ewin1->req_x = (jumpx) ? ewin1->shape_x : ewin1->req_x + dx;
	ewin1->req_y = (jumpy) ? ewin1->shape_y : ewin1->req_y + dy;

	/* swapping of group member locations: */
	if (Mode.move.swap && GroupsGetSwapmove())
	  {
	     EWin              **all_gwins, *ewin2;
	     int                 j, all_gwins_num;

	     all_gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_ANY, 0,
						    &all_gwins_num);

	     for (j = 0; j < all_gwins_num; j++)
	       {
		  ewin2 = all_gwins[j];

		  if (ewin1 == ewin2)
		     continue;

		  /* check for sufficient overlap and avoid flickering */
		  if (((ewin1->shape_x >= ewin2->shape_x &&
			ewin1->shape_x <= ewin2->shape_x +
			EoGetW(ewin2) / 2 && Mode.events.x <= Mode.events.px) ||
		       (ewin1->shape_x <= ewin2->shape_x &&
			ewin1->shape_x + EoGetW(ewin1) / 2 >=
			ewin2->shape_x &&
			Mode.events.x >= Mode.events.px)) &&
		      ((ewin1->shape_y >= ewin2->shape_y
			&& ewin1->shape_y <=
			ewin2->shape_y + EoGetH(ewin2) / 2
			&& Mode.events.y <= Mode.events.py)
		       || (EoGetY(ewin1) <= EoGetY(ewin2)
			   && ewin1->shape_y + EoGetH(ewin1) / 2 >=
			   ewin2->shape_y && Mode.events.y >= Mode.events.py)))
		    {
		       int                 tmp_swapcoord_x;
		       int                 tmp_swapcoord_y;

		       tmp_swapcoord_x = Mode_mr.swapcoord_x;
		       tmp_swapcoord_y = Mode_mr.swapcoord_y;
		       Mode_mr.swapcoord_x = ewin2->shape_x;
		       Mode_mr.swapcoord_y = ewin2->shape_y;
		       EwinOpMove(ewin2, OPSRC_USER,
				  tmp_swapcoord_x, tmp_swapcoord_y);
		       break;
		    }
	       }

	     Efree(all_gwins);
	  }
     }
   Efree(gwins);
}

void
ActionResizeHandleMotion(void)
{
   int                 pw, ph;
   int                 x, y, w, h;
   EWin               *ewin;

   ewin = Mode_mr.ewin;
   if (!ewin)
      return;

   switch (Mode.mode)
     {
     case MODE_RESIZE:
	switch (Mode_mr.resize_detail)
	  {
	  case 0:
	     pw = ewin->client.w;
	     ph = ewin->client.h;
	     w = Mode_mr.win_w - (Mode.events.x - Mode_mr.start_x);
	     h = Mode_mr.win_h - (Mode.events.y - Mode_mr.start_y);
	     x = Mode_mr.win_x + (Mode.events.x - Mode_mr.start_x);
	     y = Mode_mr.win_y + (Mode.events.y - Mode_mr.start_y);
	     ewin->client.w = w;
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     h = ewin->client.h;
	     if (pw == ewin->client.w)
		x = ewin->shape_x;
	     else
		x = Mode_mr.win_x + Mode_mr.win_w - w;
	     if (ph == ewin->client.h)
		y = ewin->shape_y;
	     else
		y = Mode_mr.win_y + Mode_mr.win_h - h;
	     ewin->client.w = pw;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1);
	     break;
	  case 1:
	     ph = ewin->client.h;
	     w = Mode_mr.win_w + (Mode.events.x - Mode_mr.start_x);
	     h = Mode_mr.win_h - (Mode.events.y - Mode_mr.start_y);
	     x = ewin->shape_x;
	     y = Mode_mr.win_y + (Mode.events.y - Mode_mr.start_y);
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     h = ewin->client.h;
	     if (ph == ewin->client.h)
		y = ewin->shape_y;
	     else
		y = Mode_mr.win_y + Mode_mr.win_h - h;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1);
	     break;
	  case 2:
	     pw = ewin->client.w;
	     w = Mode_mr.win_w - (Mode.events.x - Mode_mr.start_x);
	     h = Mode_mr.win_h + (Mode.events.y - Mode_mr.start_y);
	     x = Mode_mr.win_x + (Mode.events.x - Mode_mr.start_x);
	     y = ewin->shape_y;
	     ewin->client.w = w;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     if (pw == ewin->client.w)
		x = ewin->shape_x;
	     else
		x = Mode_mr.win_x + Mode_mr.win_w - w;
	     ewin->client.w = pw;
	     DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1);
	     break;
	  case 3:
	     w = Mode_mr.win_w + (Mode.events.x - Mode_mr.start_x);
	     h = Mode_mr.win_h + (Mode.events.y - Mode_mr.start_y);
	     x = ewin->shape_x;
	     y = ewin->shape_y;
	     DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1);
	     break;
	  default:
	     break;
	  }
	break;

     case MODE_RESIZE_H:
	switch (Mode_mr.resize_detail)
	  {
	  case 0:
	     pw = ewin->client.w;
	     w = Mode_mr.win_w - (Mode.events.x - Mode_mr.start_x);
	     h = ewin->client.h;
	     x = Mode_mr.win_x + (Mode.events.x - Mode_mr.start_x);
	     y = ewin->shape_y;
	     ewin->client.w = w;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     if (pw == ewin->client.w)
		x = ewin->shape_x;
	     else
		x = Mode_mr.win_x + Mode_mr.win_w - w;
	     ewin->client.w = pw;
	     DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1);
	     break;
	  case 1:
	     w = Mode_mr.win_w + (Mode.events.x - Mode_mr.start_x);
	     h = ewin->client.h;
	     x = ewin->shape_x;
	     y = ewin->shape_y;
	     DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1);
	     break;
	  default:
	     break;
	  }
	break;

     case MODE_RESIZE_V:
	switch (Mode_mr.resize_detail)
	  {
	  case 0:
	     ph = ewin->client.h;
	     w = ewin->client.w;
	     h = Mode_mr.win_h - (Mode.events.y - Mode_mr.start_y);
	     x = ewin->shape_x;
	     y = Mode_mr.win_y + (Mode.events.y - Mode_mr.start_y);
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     h = ewin->client.h;
	     if (ph == ewin->client.h)
		y = ewin->shape_y;
	     else
		y = Mode_mr.win_y + Mode_mr.win_h - h;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1);
	     break;
	  case 1:
	     w = ewin->client.w;
	     h = Mode_mr.win_h + (Mode.events.y - Mode_mr.start_y);
	     x = ewin->shape_x;
	     y = ewin->shape_y;
	     DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1);
	     break;
	  default:
	     break;
	  }
	break;

     default:
	break;
     }
}
