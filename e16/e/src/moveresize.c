/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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

static EWin        *mode_moveresize_ewin = NULL;

static int          start_move_desk = 0;
static int          start_move_x = 0;
static int          start_move_y = 0;
static int          real_move_mode = 0;

int
ActionMoveStart(EWin * ewin, const void *params, char constrained, int nogroup)
{
   EWin              **gwins;
   int                 i, num;

   EDBUG(6, "DoMove");

   if (!ewin || ewin->fixedpos)
      EDBUG_RETURN(0);

   mode_moveresize_ewin = ewin;
   real_move_mode = Conf.movemode;
#if 0				/* Why do this? Let's see what happens if we don't :) */
   if (((ewin->groups) || (ewin->has_transients)) && (Conf.movemode > 0))
      Conf.movemode = 0;
#endif
   if (Conf.movemode > 0)
     {
	FX_Pause();
	GrabX();
     }
   UnGrabTheButtons();
   GrabConfineThePointer(root.win);
   SoundPlay("SOUND_MOVE_START");
   Mode.mode = MODE_MOVE_PENDING;
   Mode.constrained = constrained;
   Mode.start_x = Mode.x;
   Mode.start_y = Mode.y;
   Mode.win_x = ewin->x;
   Mode.win_y = ewin->y;
   Mode.win_w = ewin->client.w;
   Mode.win_h = ewin->client.h;
   Mode.firstlast = 0;
   start_move_desk = ewin->desktop;

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, nogroup
				      || Mode.swapmovemode, &num);
   for (i = 0; i < num; i++)
     {
	FloatEwinAt(gwins[i], gwins[i]->x, gwins[i]->y);
#if 0				/* Will never get here */
	if (Mode.mode == MODE_MOVE)
	   DrawEwinShape(gwins[i], Conf.movemode, gwins[i]->x, gwins[i]->y,
			 gwins[i]->client.w, gwins[i]->client.h,
			 Mode.firstlast);
#endif
     }
   Efree(gwins);
   Mode.firstlast = 1;
   Mode.swapcoord_x = start_move_x = ewin->x;
   Mode.swapcoord_y = start_move_y = ewin->y;
   EDBUG_RETURN(0);
   params = NULL;
}

int
ActionMoveEnd(EWin * ewin)
{
   EWin              **gwins;
   int                 d, wasresize = 0, num, i;

   EDBUG(6, "doMoveEnd");
   UnGrabTheButtons();

   if (ewin && ewin != mode_moveresize_ewin)
      return 0;

   SoundPlay("SOUND_MOVE_STOP");

   ewin = mode_moveresize_ewin;
   if (!ewin)
     {
	if (Conf.movemode > 0)
	   UngrabX();
	if (Mode.mode == MODE_MOVE)
	   ForceUpdatePagersForDesktop(desks.current);
	Conf.movemode = real_move_mode;
	EDBUG_RETURN(0);
     }
   Mode.firstlast = 2;
   d = DesktopAt(Mode.x, Mode.y);

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, Mode.nogroup
				      || Mode.swapmovemode, &num);

#if 1				/* Is this OK? */
   if (Conf.movemode == 0)
      for (i = 0; i < num; i++)
	 DetermineEwinFloat(gwins[i], 0, 0);
#endif

   if (Mode.mode == MODE_MOVE)
     {
	wasresize = 1;
	for (i = 0; i < num; i++)
	   DrawEwinShape(gwins[i], Conf.movemode, gwins[i]->x, gwins[i]->y,
			 gwins[i]->client.w, gwins[i]->client.h,
			 Mode.firstlast);
	for (i = 0; i < num; i++)
	   MoveEwin(gwins[i], gwins[i]->x, gwins[i]->y);
     }
   Mode.mode = MODE_NONE;

   for (i = 0; i < num; i++)
     {
	if ((gwins[i]->floating) || (Conf.movemode > 0))
	  {
	     if (gwins[i]->floating)
		MoveEwinToDesktopAt(gwins[i], d,
				    gwins[i]->x - (desks.desk[d].x -
						   desks.
						   desk[gwins[i]->desktop].x),
				    gwins[i]->y - (desks.desk[d].y -
						   desks.
						   desk[gwins[i]->desktop].y));
	     else
		MoveEwinToDesktopAt(gwins[i], d, gwins[i]->x, gwins[i]->y);
	     gwins[i]->floating = 0;
	  }
	if ((Conf.movemode > 0) && (gwins[i]->has_transients))
	  {
	     EWin              **lst;
	     int                 j, num2;
	     int                 dx, dy;

	     dx = ewin->x - start_move_x;
	     dy = ewin->y - start_move_y;

	     lst = ListTransientsFor(gwins[i]->client.win, &num2);
	     if (lst)
	       {
		  for (j = 0; j < num2; j++)
		     MoveEwin(lst[j], lst[j]->x + dx, lst[j]->y + dy);
		  Efree(lst);
	       }
	  }
	RaiseEwin(gwins[i]);
	ICCCM_Configure(gwins[i]);
     }
   Mode.firstlast = 0;
   HideCoords();
   XSync(disp, False);
   if (Conf.movemode > 0)
     {
	FX_Pause();
	UngrabX();
     }
   RememberImportantInfoForEwins(ewin);
   if (wasresize)
      ForceUpdatePagersForDesktop(desks.current);
   Efree(gwins);
   Conf.movemode = real_move_mode;
   Mode.nogroup = 0;
   Mode.swapmovemode = 0;

   if (Mode.have_place_grab)
     {
	Mode.have_place_grab = 0;
	XUngrabPointer(disp, CurrentTime);
     }

   EwinUpdateAfterMoveResize(ewin, 0);

   EDBUG_RETURN(0);
}

int
ActionMoveSuspend(void)
{
   EWin               *ewin;
   int                 x, y;

   ewin = mode_moveresize_ewin;
   if (!ewin)
      return 0;

   if ((Mode.mode == MODE_MOVE) && (Conf.movemode > 0))
     {
	x = ewin->x;
	y = ewin->y;
	ewin->x = -99999;
	ewin->y = -99999;
	ewin->reqx = -99999;
	ewin->reqy = -99999;
	DrawEwinShape(ewin, Conf.movemode, x, y,
		      ewin->client.w, ewin->client.h, /*3? */ 2);
     }
   else
     {
	FloatEwinAt(ewin,
		    ewin->x + desks.desk[ewin->desktop].x,
		    ewin->y + desks.desk[ewin->desktop].y);
     }

   return 0;
}

int
ActionMoveResume(void)
{
   EWin               *ewin;
   int                 x, y;

   ewin = mode_moveresize_ewin;
   if (!ewin)
      return 0;

   if (Mode.mode == MODE_MOVE_PENDING)
      Mode.mode = MODE_MOVE;

   XLowerWindow(disp, ewin->win);
   x = ewin->x;
   y = ewin->y;
   ewin->x = -99999;
   ewin->y = -99999;
   ewin->reqx = -99999;
   ewin->reqy = -99999;
   DrawEwinShape(ewin, Conf.movemode, x, y,
		 ewin->client.w, ewin->client.h, (Conf.movemode == 5) ? 4 : 0);

   return 0;
}

int
ActionResizeStart(EWin * ewin, const void *params, int hv)
{
   int                 x, y, w, h;

   EDBUG(6, "DoResize");

   if (!ewin || ewin->shaded)
      EDBUG_RETURN(0);

   mode_moveresize_ewin = ewin;
   if (Conf.resizemode > 0)
     {
	FX_Pause();
	GrabX();
     }
   queue_up = 0;
   SoundPlay("SOUND_RESIZE_START");
   UnGrabTheButtons();
   GrabConfineThePointer(root.win);
   switch (hv)
     {
     case MODE_RESIZE:
	Mode.mode = hv;
	x = Mode.x - ewin->x;
	y = Mode.y - ewin->y;
	w = ewin->w >> 1;
	h = ewin->h >> 1;
	if ((x < w) && (y < h))
	   Mode.resize_detail = 0;
	if ((x >= w) && (y < h))
	   Mode.resize_detail = 1;
	if ((x < w) && (y >= h))
	   Mode.resize_detail = 2;
	if ((x >= w) && (y >= h))
	   Mode.resize_detail = 3;
	break;
     case MODE_RESIZE_H:
	Mode.mode = hv;
	x = Mode.x - ewin->x;
	w = ewin->w >> 1;
	if (x < w)
	   Mode.resize_detail = 0;
	else
	   Mode.resize_detail = 1;
	break;
     case MODE_RESIZE_V:
	Mode.mode = hv;
	y = Mode.y - ewin->y;
	h = ewin->h >> 1;
	if (y < h)
	   Mode.resize_detail = 0;
	else
	   Mode.resize_detail = 1;
	break;
     }
   Mode.start_x = Mode.x;
   Mode.start_y = Mode.y;
   Mode.win_x = ewin->x;
   Mode.win_y = ewin->y;
   Mode.win_w = ewin->client.w;
   Mode.win_h = ewin->client.h;
   Mode.firstlast = 0;
   DrawEwinShape(ewin, Conf.resizemode, ewin->x, ewin->y, ewin->client.w,
		 ewin->client.h, Mode.firstlast);
   Mode.firstlast = 1;

   EDBUG_RETURN(0);
   params = NULL;
}

int
ActionResizeEnd(EWin * ewin)
{
   int                 i;

   EDBUG(0, "doResizeEnd");
   UnGrabTheButtons();

   if (ewin && ewin != mode_moveresize_ewin)
      return 0;

   SoundPlay("SOUND_RESIZE_STOP");

   ewin = mode_moveresize_ewin;
   if (!ewin)
     {
	if (Conf.resizemode > 0)
	   UngrabX();
	ForceUpdatePagersForDesktop(desks.current);
	EDBUG_RETURN(0);
     }
   queue_up = DRAW_QUEUE_ENABLE;
   Mode.mode = MODE_NONE;
   Mode.firstlast = 2;
   DrawEwinShape(ewin, Conf.resizemode, ewin->x, ewin->y, ewin->client.w,
		 ewin->client.h, Mode.firstlast);
   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].no_expose = 1;
   ICCCM_Configure(ewin);
   HideCoords();
   XSync(disp, False);
   if (Conf.resizemode > 0)
     {
	FX_Pause();
	UngrabX();
     }
   Mode.firstlast = 0;
   ForceUpdatePagersForDesktop(desks.current);
   RememberImportantInfoForEwin(ewin);

   EwinUpdateAfterMoveResize(ewin, 1);

   EDBUG_RETURN(0);
}

void
ActionMoveHandleMotion(void)
{
   int                 dx, dy;
   EWin               *ewin;

   EWin              **gwins;
   int                 i, j, num;
   int                 ndx, ndy;
   int                 prx, pry;
   int                 screen_snap_dist;

   ewin = mode_moveresize_ewin;
   if (!ewin)
      return;

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE,
				      Mode.nogroup || Mode.swapmovemode, &num);

   if (Mode.mode == MODE_MOVE_PENDING)
     {
	for (i = 0; i < num; i++)
	   DrawEwinShape(gwins[i], Conf.movemode, gwins[i]->x,
			 gwins[i]->y, gwins[i]->client.w,
			 gwins[i]->client.h, 0);
	Mode.mode = MODE_MOVE;
     }

   dx = Mode.x - Mode.px;
   dy = Mode.y - Mode.py;
   if (Mode.next_move_x_plus != 0)
     {
	dx += Mode.next_move_x_plus;
	Mode.next_move_x_plus = 0;
     }
   if (Mode.next_move_y_plus != 0)
     {
	dy += Mode.next_move_y_plus;
	Mode.next_move_y_plus = 0;
     }
   {
      char                jumpx, jumpy;
      int                 min_dx, max_dx, min_dy, max_dy;

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
	 Mode.constrained ? (root.w + root.h) : Conf.snap.screen_snap_dist;
      for (i = 0; i < num; i++)
	{
	   /* jump out of snap horizontally */
	   if ((ndx != dx)
	       && (((gwins[i]->x == 0)
		    &&
		    (!(IN_RANGE
		       (gwins[i]->reqx, gwins[i]->x,
			screen_snap_dist))))
		   || ((gwins[i]->x == (root.w - gwins[i]->w))
		       &&
		       (!(IN_RANGE
			  (gwins[i]->reqx, gwins[i]->x,
			   screen_snap_dist))))
		   || ((gwins[i]->x != 0)
		       && (gwins[i]->x != (root.w - gwins[i]->w)
			   &&
			   (!(IN_RANGE
			      (gwins[i]->reqx, gwins[i]->x,
			       Conf.snap.edge_snap_dist)))))))
	     {
		jumpx = 1;
		ndx = gwins[i]->reqx - gwins[i]->x + dx;
	     }
	   /* jump out of snap vertically */
	   if ((ndy != dy)
	       && (((gwins[i]->y == 0)
		    &&
		    (!(IN_RANGE
		       (gwins[i]->reqy, gwins[i]->y,
			screen_snap_dist))))
		   || ((gwins[i]->y == (root.h - gwins[i]->h))
		       &&
		       (!(IN_RANGE
			  (gwins[i]->reqy, gwins[i]->y,
			   screen_snap_dist))))
		   || ((gwins[i]->y != 0)
		       && (gwins[i]->y != (root.h - gwins[i]->h)
			   &&
			   (!(IN_RANGE
			      (gwins[i]->reqy, gwins[i]->y,
			       Conf.snap.edge_snap_dist)))))))
	     {
		jumpy = 1;
		ndy = gwins[i]->reqy - gwins[i]->y + dy;
	     }
	}
      for (i = 0; i < num; i++)
	{
	   /* if its opaque move mode check to see if we have to float */
	   /* the window aboe all desktops (reparent to root) */
	   if (Conf.movemode == 0)
	      DetermineEwinFloat(gwins[i], ndx, ndy);
	   /* draw the new position of the window */
	   prx = gwins[i]->reqx;
	   pry = gwins[i]->reqy;
	   DrawEwinShape(gwins[i], Conf.movemode, gwins[i]->x + ndx,
			 gwins[i]->y + ndy, gwins[i]->client.w,
			 gwins[i]->client.h, Mode.firstlast);
	   /* if we didnt jump the winow after a resist at the edge */
	   /* reset the requested x to be the prev. requested + delta */
	   if (!(jumpx))
	      gwins[i]->reqx = prx + dx;
	   if (!(jumpy))
	      gwins[i]->reqy = pry + dy;

	   /* swapping of group member locations: */
	   if (Mode.swapmovemode && Conf.group_swapmove)
	     {
		EWin              **all_gwins;
		int                 all_gwins_num;

		all_gwins =
		   ListWinGroupMembersForEwin(ewin, ACTION_NONE, 0,
					      &all_gwins_num);

		for (j = 0; j < all_gwins_num; j++)
		  {
		     if (gwins[i] == all_gwins[j])
			continue;

		     /* check for sufficient overlap and avoid flickering */
		     if (((gwins
			   [i]->x >= all_gwins[j]->x
			   && gwins[i]->x <=
			   all_gwins[j]->x + all_gwins[j]->w / 2
			   && Mode.x <= Mode.px)
			  || (gwins[i]->x <= all_gwins[j]->x
			      && gwins[i]->x + gwins[i]->w / 2 >=
			      all_gwins[j]->x && Mode.x >= Mode.px))
			 &&
			 ((gwins
			   [i]->y >= all_gwins[j]->y
			   && gwins[i]->y <=
			   all_gwins[j]->y + all_gwins[j]->h / 2
			   && Mode.y <= Mode.py)
			  || (gwins[i]->y <= all_gwins[j]->y
			      && gwins[i]->y + gwins[i]->h / 2 >=
			      all_gwins[j]->y && Mode.y >= Mode.py)))
		       {
			  int                 tmp_swapcoord_x;
			  int                 tmp_swapcoord_y;

			  tmp_swapcoord_x = Mode.swapcoord_x;
			  tmp_swapcoord_y = Mode.swapcoord_y;
			  Mode.swapcoord_x = all_gwins[j]->x;
			  Mode.swapcoord_y = all_gwins[j]->y;
			  MoveEwin(all_gwins[j], tmp_swapcoord_x,
				   tmp_swapcoord_y);
			  break;
		       }
		  }

		Efree(all_gwins);
	     }
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

   ewin = mode_moveresize_ewin;
   if (!ewin)
      return;

   switch (Mode.mode)
     {
     case MODE_RESIZE:
	switch (Mode.resize_detail)
	  {
	  case 0:
	     pw = ewin->client.w;
	     ph = ewin->client.h;
	     w = Mode.win_w - (Mode.x - Mode.start_x);
	     h = Mode.win_h - (Mode.y - Mode.start_y);
	     x = Mode.win_x + (Mode.x - Mode.start_x);
	     y = Mode.win_y + (Mode.y - Mode.start_y);
	     ewin->client.w = w;
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     h = ewin->client.h;
	     if (pw == ewin->client.w)
		x = ewin->x;
	     else
		x = Mode.win_x + Mode.win_w - w;
	     if (ph == ewin->client.h)
		y = ewin->y;
	     else
		y = Mode.win_y + Mode.win_h - h;
	     ewin->client.w = pw;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, Conf.resizemode, x, y, w, h, Mode.firstlast);
	     break;
	  case 1:
	     ph = ewin->client.h;
	     w = Mode.win_w + (Mode.x - Mode.start_x);
	     h = Mode.win_h - (Mode.y - Mode.start_y);
	     x = ewin->x;
	     y = Mode.win_y + (Mode.y - Mode.start_y);
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     h = ewin->client.h;
	     if (ph == ewin->client.h)
		y = ewin->y;
	     else
		y = Mode.win_y + Mode.win_h - h;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, Conf.resizemode, x, y, w, h, Mode.firstlast);
	     break;
	  case 2:
	     pw = ewin->client.w;
	     w = Mode.win_w - (Mode.x - Mode.start_x);
	     h = Mode.win_h + (Mode.y - Mode.start_y);
	     x = Mode.win_x + (Mode.x - Mode.start_x);
	     y = ewin->y;
	     ewin->client.w = w;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     if (pw == ewin->client.w)
		x = ewin->x;
	     else
		x = Mode.win_x + Mode.win_w - w;
	     ewin->client.w = pw;
	     DrawEwinShape(ewin, Conf.resizemode, x, y, w, h, Mode.firstlast);
	     break;
	  case 3:
	     w = Mode.win_w + (Mode.x - Mode.start_x);
	     h = Mode.win_h + (Mode.y - Mode.start_y);
	     x = ewin->x;
	     y = ewin->y;
	     DrawEwinShape(ewin, Conf.resizemode, x, y, w, h, Mode.firstlast);
	     break;
	  default:
	     break;
	  }
	break;

     case MODE_RESIZE_H:
	switch (Mode.resize_detail)
	  {
	  case 0:
	     pw = ewin->client.w;
	     w = Mode.win_w - (Mode.x - Mode.start_x);
	     h = ewin->client.h;
	     x = Mode.win_x + (Mode.x - Mode.start_x);
	     y = ewin->y;
	     ewin->client.w = w;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     if (pw == ewin->client.w)
		x = ewin->x;
	     else
		x = Mode.win_x + Mode.win_w - w;
	     ewin->client.w = pw;
	     DrawEwinShape(ewin, Conf.resizemode, x, y, w, h, Mode.firstlast);
	     break;
	  case 1:
	     w = Mode.win_w + (Mode.x - Mode.start_x);
	     h = ewin->client.h;
	     x = ewin->x;
	     y = ewin->y;
	     DrawEwinShape(ewin, Conf.resizemode, x, y, w, h, Mode.firstlast);
	     break;
	  default:
	     break;
	  }
	break;

     case MODE_RESIZE_V:
	switch (Mode.resize_detail)
	  {
	  case 0:
	     ph = ewin->client.h;
	     w = ewin->client.w;
	     h = Mode.win_h - (Mode.y - Mode.start_y);
	     x = ewin->x;
	     y = Mode.win_y + (Mode.y - Mode.start_y);
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     h = ewin->client.h;
	     if (ph == ewin->client.h)
		y = ewin->y;
	     else
		y = Mode.win_y + Mode.win_h - h;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, Conf.resizemode, x, y, w, h, Mode.firstlast);
	     break;
	  case 1:
	     w = ewin->client.w;
	     h = Mode.win_h + (Mode.y - Mode.start_y);
	     x = ewin->x;
	     y = ewin->y;
	     DrawEwinShape(ewin, Conf.resizemode, x, y, w, h, Mode.firstlast);
	     break;
	  default:
	     break;
	  }
	break;

     default:
	break;
     }
}
