/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2009 Kim Woelders
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
#include "cursors.h"
#include "desktops.h"
#include "emodule.h"
#include "events.h"
#include "ewins.h"
#include "focus.h"
#include "grabs.h"
#include "groups.h"
#include "hints.h"
#include "timers.h"
#include "xwin.h"
#include <X11/keysym.h>

static struct {
   EWin               *ewin;
   char                mode;
   char                using_kbd;
   char                nogroup;
   char                grab_server;
   int                 start_x, start_y;
   int                 cur_x, cur_y;
   int                 win_x, win_y, win_w, win_h;
   int                 swapcoord_x, swapcoord_y;
   int                 resize_detail;
} Mode_mr;

static int
_NeedServerGrab(int mode)
{
   if (mode == 0)
      return 0;
   if (mode <= 2)
      return !Conf.movres.avoid_server_grab;
   return 1;
}

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
ActionMoveStart(EWin * ewin, int kbd, int constrained, int nogroup)
{
   EWin              **gwins;
   int                 i, num, cx, cy;

   if (!ewin || ewin->state.inhibit_move)
      return 0;

   Mode_mr.ewin = ewin;
   Mode_mr.using_kbd = kbd;
   Mode_mr.nogroup = nogroup;

   EventsGetXY(&cx, &cy);

   SoundPlay(SOUND_MOVE_START);

   if (kbd)
      GrabKeyboardSet(EoGetWin(ewin));
   else
      GrabPointerSet(EoGetWin(ewin), ECSR_ACT_MOVE, 1);

   Mode.mode = MODE_MOVE_PENDING;
   Mode.constrained = constrained;

   Mode_mr.start_x = Mode_mr.cur_x = cx;
   Mode_mr.start_y = Mode_mr.cur_y = cy;

   Mode_mr.win_x = Mode_mr.start_x - (EoGetX(ewin) + EoGetX(EoGetDesk(ewin)));
   Mode_mr.win_y = Mode_mr.start_y - (EoGetY(ewin) + EoGetY(EoGetDesk(ewin)));

   EwinRaise(ewin);
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, nogroup
				      || Mode.move.swap, &num);

   if (Conf.movres.mode_move < 0 || Conf.movres.mode_move > 5)
      Conf.movres.mode_move = 0;
   Mode_mr.mode = Conf.movres.mode_move;
   if (num > 1 && Conf.movres.mode_move == 5)
      Mode_mr.mode = 0;
   Mode_mr.grab_server = _NeedServerGrab(Mode_mr.mode);

   for (i = 0; i < num; i++)
     {
	EwinShapeSet(gwins[i]);
	EwinOpFloatAt(gwins[i], OPSRC_USER, EoGetX(gwins[i]), EoGetY(gwins[i]));
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

static int
ActionMoveEnd(EWin * ewin)
{
   EWin              **gwins;
   int                 num, i;
   Desk               *d1, *d2;

   if (ewin && ewin != Mode_mr.ewin)
      return 0;

   GrabKeyboardRelease();
   GrabPointerRelease();

   SoundPlay(SOUND_MOVE_STOP);

   ewin = Mode_mr.ewin;
   if (!ewin)
      goto done;

   ewin->state.show_coords = 0;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, Mode_mr.nogroup
				      || Mode.move.swap, &num);

   if (Mode.mode == MODE_MOVE)
     {
	for (i = 0; i < num; i++)
	   DrawEwinShape(gwins[i], Mode_mr.mode,
			 gwins[i]->shape_x, gwins[i]->shape_y,
			 gwins[i]->client.w, gwins[i]->client.h, 2, i);
     }
   Mode.mode = MODE_NONE;

   d2 = DesktopAt(Mode.events.mx, Mode.events.my);

   for (i = 0; i < num; i++)
     {
	ewin = gwins[i];
	d1 = EoGetDesk(ewin);
	if (d2 == d1)
	   EwinOpUnfloatAt(ewin, OPSRC_USER, d2, ewin->shape_x, ewin->shape_y);
	else
	   EwinOpUnfloatAt(ewin, OPSRC_USER, d2,
			   ewin->shape_x - (EoGetX(d2) - EoGetX(d1)),
			   ewin->shape_y - (EoGetY(d2) - EoGetY(d1)));
	if (Mode_mr.mode == 0)
	  {
	     ewin->state.moving = 0;
	     EwinUpdateOpacity(ewin);
	  }
     }

   Efree(gwins);

   ESync(ESYNC_MOVRES);

 done:
   Mode.mode = MODE_NONE;
   Mode.move.swap = 0;
   Mode.place.doing_manual = 0;

   if (Mode_mr.grab_server)
     {
	FocusEnable(1);
	EUngrabServer();
	ModulesSignal(ESIGNAL_ANIMATION_RESUME, NULL);
     }

   return 0;
}

static int
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
   if (Mode_mr.grab_server)
     {
	lst = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE,
					 Mode_mr.nogroup, &num);
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     DrawEwinShape(ewin, Mode_mr.mode, ewin->shape_x,
			   ewin->shape_y, ewin->client.w, ewin->client.h, 3, i);
	  }
	Efree(lst);

	EUngrabServer();
     }

   return 0;
}

static int
ActionMoveResume(void)
{
   EWin               *ewin, **lst;
   int                 i, num;
   int                 x, y, fl, dx, dy;

   ewin = Mode_mr.ewin;
   if (!ewin)
      return 0;

   GrabPointerSet(EoGetWin(ewin), ECSR_ACT_MOVE, 1);

   fl = (Mode_mr.mode == 5) ? 4 : 0;
   if (Mode.mode == MODE_MOVE_PENDING)
     {
	Mode.mode = MODE_MOVE;
	fl = 0;			/* This is the first time we draw it */
     }

   if (Mode_mr.grab_server)
      EGrabServer();

   dx =
      Mode.events.mx - Mode_mr.win_x - EoGetX(EoGetDesk(ewin)) - ewin->shape_x;
   dy =
      Mode.events.my - Mode_mr.win_y - EoGetY(EoGetDesk(ewin)) - ewin->shape_y;

   /* Redraw any windows that were in "move mode" */
   lst = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE,
				    Mode_mr.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];

	if (!EoIsFloating(ewin))
	   continue;

	x = ewin->shape_x + dx;
	y = ewin->shape_y + dy;
	DrawEwinShape(ewin, Mode_mr.mode, x, y,
		      ewin->client.w, ewin->client.h, fl, i);
     }
   Efree(lst);

   return 0;
}

#define RD(h, v) (((h) << 8) + (v))
#define RD_H(hv) (((hv) >> 8) & 0xff)
#define RD_V(hv) (((hv)     ) & 0xff)

int
ActionResizeStart(EWin * ewin, int kbd, int hv)
{
   int                 x, y, w, h, ww, hh, cx, cy;
   unsigned int        csr;

   if (!ewin || ewin->state.inhibit_resize)
      return 0;

   Mode_mr.ewin = ewin;

   EventsGetXY(&cx, &cy);

   SoundPlay(SOUND_RESIZE_START);

   if (Conf.movres.mode_resize < 0 || Conf.movres.mode_resize > 4)
      Conf.movres.mode_resize = 0;
   Mode_mr.mode = Conf.movres.mode_resize;
   Mode_mr.using_kbd = kbd;
   Mode_mr.grab_server = _NeedServerGrab(Mode_mr.mode);
   if (Mode_mr.grab_server)
     {
	EGrabServer();
	ModulesSignal(ESIGNAL_ANIMATION_SUSPEND, NULL);
	/* Run idlers (stacking, border updates, ...) before drawing lines */
	IdlersRun();
     }
   if (Mode_mr.mode == 0)
     {
	ewin->state.resizing = 1;
	EwinUpdateOpacity(ewin);
     }

   switch (hv)
     {
     default:
     case MODE_RESIZE:
	Mode.mode = hv;
	if (kbd)
	  {
	     Mode_mr.resize_detail = 0;
	     csr = ECSR_ACT_RESIZE_BR;
	     break;
	  }
	x = cx - EoGetX(ewin);
	y = cy - EoGetY(ewin);
	w = EoGetW(ewin) >> 1;
	h = EoGetH(ewin) >> 1;
	ww = EoGetW(ewin) / 6;
	hh = EoGetH(ewin) / 6;

	csr = ECSR_ACT_RESIZE;
	if ((x < w) && (y < h))
	  {
	     Mode_mr.resize_detail = RD(1, 1);
	     csr = ECSR_ACT_RESIZE_TL;
	  }
	else if ((x >= w) && (y < h))
	  {
	     Mode_mr.resize_detail = RD(2, 1);
	     csr = ECSR_ACT_RESIZE_TR;
	  }
	else if ((x < w) && (y >= h))
	  {
	     Mode_mr.resize_detail = RD(1, 2);
	     csr = ECSR_ACT_RESIZE_BL;
	  }
	else if ((x >= w) && (y >= h))
	  {
	     Mode_mr.resize_detail = RD(2, 2);
	     csr = ECSR_ACT_RESIZE_BR;
	  }

	/* The following four if statements added on 07/22/04 by Josh Holtrop.
	 * They allow strictly horizontal or vertical resizing when the
	 * cursor is towards the middle of an edge of a window. */
	if ((abs(x - w) < (w >> 1)) && (y < hh))
	  {
	     Mode.mode = MODE_RESIZE_V;
	     Mode_mr.resize_detail = RD(0, 1);
	     csr = ECSR_ACT_RESIZE_V;
	  }
	else if ((abs(x - w) < (w >> 1)) && (y > (EoGetH(ewin) - hh)))
	  {
	     Mode.mode = MODE_RESIZE_V;
	     Mode_mr.resize_detail = RD(0, 2);
	     csr = ECSR_ACT_RESIZE_V;
	  }
	else if ((abs(y - h) < (h >> 1)) && (x < ww))
	  {
	     Mode.mode = MODE_RESIZE_H;
	     Mode_mr.resize_detail = RD(1, 0);
	     csr = ECSR_ACT_RESIZE_H;
	  }
	else if ((abs(y - h) < (h >> 1)) && (x > (EoGetW(ewin) - ww)))
	  {
	     Mode.mode = MODE_RESIZE_H;
	     Mode_mr.resize_detail = RD(2, 0);
	     csr = ECSR_ACT_RESIZE_H;
	  }
	break;

     case MODE_RESIZE_H:
	Mode.mode = hv;
	x = cx - EoGetX(ewin);
	w = EoGetW(ewin) >> 1;
	if (x < w)
	   Mode_mr.resize_detail = RD(1, 0);
	else
	   Mode_mr.resize_detail = RD(2, 0);
	csr = ECSR_ACT_RESIZE_H;
	break;

     case MODE_RESIZE_V:
	Mode.mode = hv;
	y = cy - EoGetY(ewin);
	h = EoGetH(ewin) >> 1;
	if (y < h)
	   Mode_mr.resize_detail = RD(0, 1);
	else
	   Mode_mr.resize_detail = RD(0, 2);
	csr = ECSR_ACT_RESIZE_V;
	break;
     }

   Mode_mr.start_x = Mode_mr.cur_x = cx;
   Mode_mr.start_y = Mode_mr.cur_y = cy;
   Mode_mr.win_x = EoGetX(ewin);
   Mode_mr.win_y = EoGetY(ewin);
   Mode_mr.win_w = ewin->client.w;
   Mode_mr.win_h = ewin->client.h;

   if (kbd)
      GrabKeyboardSet(EoGetWin(ewin));
   else
      GrabPointerSet(EoGetWin(ewin), csr, 1);

   EwinShapeSet(ewin);
   ewin->state.show_coords = 1;
   DrawEwinShape(ewin, Conf.movres.mode_resize, EoGetX(ewin), EoGetY(ewin),
		 ewin->client.w, ewin->client.h, 0, 0);

   return 0;
}

static int
ActionResizeEnd(EWin * ewin)
{
   if (ewin && ewin != Mode_mr.ewin)
      return 0;

   Mode.mode = MODE_NONE;

   GrabKeyboardRelease();
   GrabPointerRelease();

   SoundPlay(SOUND_RESIZE_STOP);

   ewin = Mode_mr.ewin;
   if (!ewin)
      goto done;

   ewin->state.show_coords = 0;
   DrawEwinShape(ewin, Conf.movres.mode_resize, ewin->shape_x, ewin->shape_y,
		 ewin->shape_w, ewin->shape_h, 2, 0);

   if (Mode_mr.mode == 0)
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

   ESync(ESYNC_MOVRES);

 done:
   if (Mode_mr.grab_server)
     {
	EUngrabServer();
	ModulesSignal(ESIGNAL_ANIMATION_RESUME, NULL);
     }

   return 0;
}

static void
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

   EdgeCheckMotion(Mode.events.mx, Mode.events.my);

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE,
				      Mode_mr.nogroup || Mode.move.swap, &num);

   if (Mode.mode == MODE_MOVE_PENDING)
     {
	if (Mode_mr.grab_server)
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
	     DrawEwinShape(ewin1, Mode_mr.mode, EoGetX(ewin1), EoGetY(ewin1),
			   ewin1->client.w, ewin1->client.h, 0, i);
	     if (Conf.movres.mode_move == 0)
		Mode_mr.mode = 0;
	  }
	Mode.mode = MODE_MOVE;
	dx = Mode.events.mx - Mode_mr.start_x;
	dy = Mode.events.my - Mode_mr.start_y;
     }
   else if (Mode.mode == MODE_MOVE)
     {
	dx = Mode.events.mx - Mode.events.px;
	dy = Mode.events.my - Mode.events.py;
     }
   else
     {
	/* It should not be possible to get here. */
	goto done;
     }

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
      Mode.constrained ? (WinGetW(VROOT) +
			  WinGetH(VROOT)) : Conf.snap.screen_snap_dist;

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
	     ((ewin1->shape_x == (WinGetW(VROOT) - EoGetW(ewin1))) &&
	      (dd > screen_snap_dist)) ||
	     ((ewin1->shape_x != 0) &&
	      (ewin1->shape_x != (WinGetW(VROOT) - EoGetW(ewin1)) &&
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
	     ((ewin1->shape_y == (WinGetH(VROOT) - EoGetH(ewin1))) &&
	      (dd > screen_snap_dist)) ||
	     ((ewin1->shape_y != 0) &&
	      (ewin1->shape_y != (WinGetH(VROOT) - EoGetH(ewin1)) &&
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
		      ewin1->client.w, ewin1->client.h, 1, i);

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
			ewin1->shape_x <= ewin2->shape_x + EoGetW(ewin2) / 2 &&
			dx <= 0) ||
		       (ewin1->shape_x <= ewin2->shape_x &&
			ewin1->shape_x + EoGetW(ewin1) / 2 >= ewin2->shape_x &&
			dx >= 0)) &&
		      ((ewin1->shape_y >= ewin2->shape_y &&
			ewin1->shape_y <= ewin2->shape_y + EoGetH(ewin2) / 2 &&
			dy <= 0) ||
		       (EoGetY(ewin1) <= EoGetY(ewin2) &&
			ewin1->shape_y + EoGetH(ewin1) / 2 >= ewin2->shape_y &&
			dy >= 0)))
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

 done:
   Efree(gwins);
}

static void
ActionResizeHandleMotion(void)
{
   int                 x, y, w, h;
   EWin               *ewin;

   ewin = Mode_mr.ewin;
   if (!ewin)
      return;

   w = ewin->client.w;
   h = ewin->client.h;
   x = ewin->shape_x;
   y = ewin->shape_y;

   switch (RD_H(Mode_mr.resize_detail))
     {
     default:
	break;
     case 1:			/* Left */
	w = Mode_mr.win_w - (Mode.events.mx - Mode_mr.start_x);
	ICCCM_SizeMatch(ewin, w, h, &w, &h);
	x = Mode_mr.win_x + (Mode_mr.win_w - w);
	break;
     case 2:			/* Right */
	w = Mode_mr.win_w + (Mode.events.mx - Mode_mr.start_x);
	ICCCM_SizeMatch(ewin, w, h, &w, &h);
	break;
     }

   switch (RD_V(Mode_mr.resize_detail))
     {
     default:
	break;
     case 1:			/* Top */
	h = Mode_mr.win_h - (Mode.events.my - Mode_mr.start_y);
	ICCCM_SizeMatch(ewin, w, h, &w, &h);
	y = Mode_mr.win_y + (Mode_mr.win_h - h);
	break;
     case 2:			/* Bottom */
	h = Mode_mr.win_h + (Mode.events.my - Mode_mr.start_y);
	ICCCM_SizeMatch(ewin, w, h, &w, &h);
	break;
     }

   DrawEwinShape(ewin, Conf.movres.mode_resize, x, y, w, h, 1, 0);
}

void
ActionsHandleKey(unsigned int key)
{
   EWin               *ewin;
   int                 resize, delta, end = 0;

   ewin = Mode_mr.ewin;
   if (!ewin)
      return;

   resize = Mode.mode == MODE_RESIZE ||
      Mode.mode == MODE_RESIZE_H || Mode.mode == MODE_RESIZE_V;

   Mode.events.px = Mode_mr.cur_x;
   Mode.events.py = Mode_mr.cur_y;
   delta = 5;

   switch (key)
     {
     default:
	return;
     case XK_Escape:
	Mode_mr.cur_x = Mode_mr.start_x;
	Mode_mr.cur_y = Mode_mr.start_y;
     case XK_Return:
	end = 1;
	break;

     case XK_Left:
	if (!RD_H(Mode_mr.resize_detail))
	   Mode_mr.resize_detail |= RD(1, 0);
	if (resize && ewin->icccm.w_inc > delta)
	   delta = ewin->icccm.w_inc;
	Mode_mr.cur_x -= delta;
	break;
     case XK_Right:
	if (!RD_H(Mode_mr.resize_detail))
	   Mode_mr.resize_detail |= RD(2, 0);
	if (resize && ewin->icccm.w_inc > delta)
	   delta = ewin->icccm.w_inc;
	Mode_mr.cur_x += delta;
	break;
     case XK_Up:
	if (!RD_V(Mode_mr.resize_detail))
	   Mode_mr.resize_detail |= RD(0, 1);
	if (resize && ewin->icccm.h_inc > delta)
	   delta = ewin->icccm.h_inc;
	Mode_mr.cur_y -= delta;
	break;
     case XK_Down:
	if (!RD_V(Mode_mr.resize_detail))
	   Mode_mr.resize_detail |= RD(0, 2);
	if (resize && ewin->icccm.h_inc > delta)
	   delta = ewin->icccm.h_inc;
	Mode_mr.cur_y += delta;
	break;
     }

   Mode_mr.using_kbd = 2;
   Mode.events.mx = Mode_mr.cur_x;
   Mode.events.my = Mode_mr.cur_y;

   switch (Mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveHandleMotion();
	if (end)
	   ActionMoveEnd(NULL);
	break;

     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeHandleMotion();
	if (end)
	   ActionResizeEnd(NULL);
	break;

     default:
	break;
     }
}

void
ActionsHandleMotion(void)
{
   switch (Mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveHandleMotion();
	break;

     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeHandleMotion();
	break;

     default:
	break;
     }
}

int
ActionsSuspend(void)
{
   switch (Mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveSuspend();
	break;
     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeEnd(NULL);
	break;
     }

   return 0;
}

int
ActionsResume(void)
{
   switch (Mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveResume();
	break;
     }

   return 0;
}

int
ActionsEnd(EWin * ewin)
{
   int                 did_end = 1;

   switch (Mode.mode)
     {
     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeEnd(ewin);
	Mode.action_inhibit = 1;
	break;

     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveEnd(ewin);
	Mode.action_inhibit = 1;
	break;

     default:
	did_end = 0;
	break;
     }

   return did_end;
}

void
SlideEwinTo(EWin * ewin, int fx, int fy, int tx, int ty, int speed, int mode)
{
   SlideEwinsTo(&ewin, &fx, &fy, &tx, &ty, 1, speed, mode);
}

void
SlideEwinsTo(EWin ** ewin, int *fx, int *fy, int *tx, int *ty, int num_wins,
	     int speed, int mode)
{
   int                 k, x, y, w, h, i;
   char                firstlast;

   if (num_wins <= 0)
      return;

   firstlast = 0;
   FocusEnable(0);
   SoundPlay(SOUND_WINDOW_SLIDE);

   Mode_mr.grab_server = _NeedServerGrab(mode);
   if (Mode_mr.grab_server)
      EGrabServer();

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	for (i = 0; i < num_wins; i++)
	  {
	     if (!ewin[i])
		continue;

	     x = ((fx[i] * (1024 - k)) + (tx[i] * k)) >> 10;
	     y = ((fy[i] * (1024 - k)) + (ty[i] * k)) >> 10;
	     w = ewin[i]->client.w;
	     h = ewin[i]->client.h;
	     if (mode == 0)
		EoMove(ewin[i], x, y);
	     else
		DrawEwinShape(ewin[i], mode, x, y, w, h, firstlast, i);
	     firstlast = 1;
	  }
	/* We may loop faster here than originally intended */
	k = ETimedLoopNext();
     }

   for (i = 0; i < num_wins; i++)
     {
	if (!ewin[i])
	   continue;

	ewin[i]->state.animated = 0;

	if (mode > 0)
	   DrawEwinShape(ewin[i], mode, tx[i], ty[i], ewin[i]->client.w,
			 ewin[i]->client.h, 2, i);
	EwinMove(ewin[i], tx[i], ty[i]);
     }

   FocusEnable(1);

   if (Mode_mr.grab_server)
      EUngrabServer();

   SoundPlay(SOUND_WINDOW_SLIDE_END);
}
