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
ActionMoveStart(EWin * ewin, void *params, char constrained, int nogroup)
{
   EWin              **gwins;
   int                 i, num;

   EDBUG(6, "DoMove");

   if (!ewin || ewin->fixedpos)
      EDBUG_RETURN(0);

   mode_moveresize_ewin = ewin;
   real_move_mode = conf.movemode;
#if 0				/* Why do this? Let's see what happens if we don't :) */
   if (((ewin->groups) || (ewin->has_transients)) && (conf.movemode > 0))
      conf.movemode = 0;
#endif
   if (conf.movemode > 0)
     {
	FX_Pause();
	GrabX();
     }
   UnGrabTheButtons();
   GrabConfineThePointer(root.win);
   SoundPlay("SOUND_MOVE_START");
   mode.mode = MODE_MOVE_PENDING;
   mode.constrained = constrained;
   mode.start_x = mode.x;
   mode.start_y = mode.y;
   mode.win_x = ewin->x;
   mode.win_y = ewin->y;
   mode.win_w = ewin->client.w;
   mode.win_h = ewin->client.h;
   mode.firstlast = 0;
   start_move_desk = ewin->desktop;

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, nogroup
				      || mode.swapmovemode, &num);
   for (i = 0; i < num; i++)
     {
	FloatEwinAt(gwins[i], gwins[i]->x, gwins[i]->y);
#if 0				/* Will never get here */
	if (mode.mode == MODE_MOVE)
	   DrawEwinShape(gwins[i], conf.movemode, gwins[i]->x, gwins[i]->y,
			 gwins[i]->client.w, gwins[i]->client.h,
			 mode.firstlast);
#endif
     }
   Efree(gwins);
   mode.firstlast = 1;
   mode.swapcoord_x = start_move_x = ewin->x;
   mode.swapcoord_y = start_move_y = ewin->y;
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
	if (conf.movemode > 0)
	   UngrabX();
	if (mode.mode == MODE_MOVE)
	   ForceUpdatePagersForDesktop(desks.current);
	conf.movemode = real_move_mode;
	EDBUG_RETURN(0);
     }
   mode.firstlast = 2;
   d = DesktopAt(mode.x, mode.y);

   gwins = ListWinGroupMembersForEwin(ewin, ACTION_MOVE, mode.nogroup
				      || mode.swapmovemode, &num);

#if 1				/* Is this OK? */
   if (conf.movemode == 0)
      for (i = 0; i < num; i++)
	 DetermineEwinFloat(gwins[i], 0, 0);
#endif

   if (mode.mode == MODE_MOVE)
     {
	wasresize = 1;
	for (i = 0; i < num; i++)
	   DrawEwinShape(gwins[i], conf.movemode, gwins[i]->x, gwins[i]->y,
			 gwins[i]->client.w, gwins[i]->client.h,
			 mode.firstlast);
	for (i = 0; i < num; i++)
	   MoveEwin(gwins[i], gwins[i]->x, gwins[i]->y);
     }
   mode.mode = MODE_NONE;

   for (i = 0; i < num; i++)
     {
	if ((gwins[i]->floating) || (conf.movemode > 0))
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
	if ((conf.movemode > 0) && (gwins[i]->has_transients))
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
   mode.firstlast = 0;
   HideCoords();
   XSync(disp, False);
   if (conf.movemode > 0)
     {
	FX_Pause();
	UngrabX();
     }
   RememberImportantInfoForEwins(ewin);
   if (wasresize)
      ForceUpdatePagersForDesktop(desks.current);
   Efree(gwins);
   conf.movemode = real_move_mode;
   mode.nogroup = 0;
   mode.swapmovemode = 0;

   if (mode.have_place_grab)
     {
	mode.have_place_grab = 0;
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

   if ((mode.mode == MODE_MOVE) && (conf.movemode > 0))
     {
	x = ewin->x;
	y = ewin->y;
	ewin->x = -99999;
	ewin->y = -99999;
	ewin->reqx = -99999;
	ewin->reqy = -99999;
	DrawEwinShape(ewin, conf.movemode, x, y,
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

   if (mode.mode == MODE_MOVE_PENDING)
      mode.mode = MODE_MOVE;

   XLowerWindow(disp, ewin->win);
   x = ewin->x;
   y = ewin->y;
   ewin->x = -99999;
   ewin->y = -99999;
   ewin->reqx = -99999;
   ewin->reqy = -99999;
   DrawEwinShape(ewin, conf.movemode, x, y,
		 ewin->client.w, ewin->client.h, (conf.movemode == 5) ? 4 : 0);

   return 0;
}

int
ActionResizeStart(EWin * ewin, void *params, int hv)
{
   int                 x, y, w, h;

   EDBUG(6, "DoResize");

   if (!ewin || ewin->shaded)
      EDBUG_RETURN(0);

   mode_moveresize_ewin = ewin;
   if (conf.resizemode > 0)
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
	mode.mode = hv;
	x = mode.x - ewin->x;
	y = mode.y - ewin->y;
	w = ewin->w >> 1;
	h = ewin->h >> 1;
	if ((x < w) && (y < h))
	   mode.resize_detail = 0;
	if ((x >= w) && (y < h))
	   mode.resize_detail = 1;
	if ((x < w) && (y >= h))
	   mode.resize_detail = 2;
	if ((x >= w) && (y >= h))
	   mode.resize_detail = 3;
	break;
     case MODE_RESIZE_H:
	mode.mode = hv;
	x = mode.x - ewin->x;
	w = ewin->w >> 1;
	if (x < w)
	   mode.resize_detail = 0;
	else
	   mode.resize_detail = 1;
	break;
     case MODE_RESIZE_V:
	mode.mode = hv;
	y = mode.y - ewin->y;
	h = ewin->h >> 1;
	if (y < h)
	   mode.resize_detail = 0;
	else
	   mode.resize_detail = 1;
	break;
     }
   mode.start_x = mode.x;
   mode.start_y = mode.y;
   mode.win_x = ewin->x;
   mode.win_y = ewin->y;
   mode.win_w = ewin->client.w;
   mode.win_h = ewin->client.h;
   mode.firstlast = 0;
   DrawEwinShape(ewin, conf.resizemode, ewin->x, ewin->y, ewin->client.w,
		 ewin->client.h, mode.firstlast);
   mode.firstlast = 1;

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
	if (conf.resizemode > 0)
	   UngrabX();
	ForceUpdatePagersForDesktop(desks.current);
	EDBUG_RETURN(0);
     }
   queue_up = DRAW_QUEUE_ENABLE;
   mode.mode = MODE_NONE;
   mode.firstlast = 2;
   DrawEwinShape(ewin, conf.resizemode, ewin->x, ewin->y, ewin->client.w,
		 ewin->client.h, mode.firstlast);
   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].no_expose = 1;
   ICCCM_Configure(ewin);
   HideCoords();
   XSync(disp, False);
   if (conf.resizemode > 0)
     {
	FX_Pause();
	UngrabX();
     }
   mode.firstlast = 0;
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
				      mode.nogroup || mode.swapmovemode, &num);

   if (mode.mode == MODE_MOVE_PENDING)
     {
	for (i = 0; i < num; i++)
	   DrawEwinShape(gwins[i], conf.movemode, gwins[i]->x,
			 gwins[i]->y, gwins[i]->client.w,
			 gwins[i]->client.h, 0);
	mode.mode = MODE_MOVE;
     }

   dx = mode.x - mode.px;
   dy = mode.y - mode.py;
   if (mode.next_move_x_plus != 0)
     {
	dx += mode.next_move_x_plus;
	mode.next_move_x_plus = 0;
     }
   if (mode.next_move_y_plus != 0)
     {
	dy += mode.next_move_y_plus;
	mode.next_move_y_plus = 0;
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
	 mode.constrained ? (root.w + root.h) : conf.snap.screen_snap_dist;
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
			       conf.snap.edge_snap_dist)))))))
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
			       conf.snap.edge_snap_dist)))))))
	     {
		jumpy = 1;
		ndy = gwins[i]->reqy - gwins[i]->y + dy;
	     }
	}
      for (i = 0; i < num; i++)
	{
	   /* if its opaque move mode check to see if we have to float */
	   /* the window aboe all desktops (reparent to root) */
	   if (conf.movemode == 0)
	      DetermineEwinFloat(gwins[i], ndx, ndy);
	   /* draw the new position of the window */
	   prx = gwins[i]->reqx;
	   pry = gwins[i]->reqy;
	   DrawEwinShape(gwins[i], conf.movemode, gwins[i]->x + ndx,
			 gwins[i]->y + ndy, gwins[i]->client.w,
			 gwins[i]->client.h, mode.firstlast);
	   /* if we didnt jump the winow after a resist at the edge */
	   /* reset the requested x to be the prev. requested + delta */
	   if (!(jumpx))
	      gwins[i]->reqx = prx + dx;
	   if (!(jumpy))
	      gwins[i]->reqy = pry + dy;

	   /* swapping of group member locations: */
	   if (mode.swapmovemode && conf.group_swapmove)
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
			   && mode.x <= mode.px)
			  || (gwins[i]->x <= all_gwins[j]->x
			      && gwins[i]->x + gwins[i]->w / 2 >=
			      all_gwins[j]->x && mode.x >= mode.px))
			 &&
			 ((gwins
			   [i]->y >= all_gwins[j]->y
			   && gwins[i]->y <=
			   all_gwins[j]->y + all_gwins[j]->h / 2
			   && mode.y <= mode.py)
			  || (gwins[i]->y <= all_gwins[j]->y
			      && gwins[i]->y + gwins[i]->h / 2 >=
			      all_gwins[j]->y && mode.y >= mode.py)))
		       {
			  int                 tmp_swapcoord_x;
			  int                 tmp_swapcoord_y;

			  tmp_swapcoord_x = mode.swapcoord_x;
			  tmp_swapcoord_y = mode.swapcoord_y;
			  mode.swapcoord_x = all_gwins[j]->x;
			  mode.swapcoord_y = all_gwins[j]->y;
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

   switch (mode.mode)
     {
     case MODE_RESIZE:
	switch (mode.resize_detail)
	  {
	  case 0:
	     pw = ewin->client.w;
	     ph = ewin->client.h;
	     w = mode.win_w - (mode.x - mode.start_x);
	     h = mode.win_h - (mode.y - mode.start_y);
	     x = mode.win_x + (mode.x - mode.start_x);
	     y = mode.win_y + (mode.y - mode.start_y);
	     ewin->client.w = w;
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     h = ewin->client.h;
	     if (pw == ewin->client.w)
		x = ewin->x;
	     else
		x = mode.win_x + mode.win_w - w;
	     if (ph == ewin->client.h)
		y = ewin->y;
	     else
		y = mode.win_y + mode.win_h - h;
	     ewin->client.w = pw;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, conf.resizemode, x, y, w, h, mode.firstlast);
	     break;
	  case 1:
	     ph = ewin->client.h;
	     w = mode.win_w + (mode.x - mode.start_x);
	     h = mode.win_h - (mode.y - mode.start_y);
	     x = ewin->x;
	     y = mode.win_y + (mode.y - mode.start_y);
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     h = ewin->client.h;
	     if (ph == ewin->client.h)
		y = ewin->y;
	     else
		y = mode.win_y + mode.win_h - h;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, conf.resizemode, x, y, w, h, mode.firstlast);
	     break;
	  case 2:
	     pw = ewin->client.w;
	     w = mode.win_w - (mode.x - mode.start_x);
	     h = mode.win_h + (mode.y - mode.start_y);
	     x = mode.win_x + (mode.x - mode.start_x);
	     y = ewin->y;
	     ewin->client.w = w;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     if (pw == ewin->client.w)
		x = ewin->x;
	     else
		x = mode.win_x + mode.win_w - w;
	     ewin->client.w = pw;
	     DrawEwinShape(ewin, conf.resizemode, x, y, w, h, mode.firstlast);
	     break;
	  case 3:
	     w = mode.win_w + (mode.x - mode.start_x);
	     h = mode.win_h + (mode.y - mode.start_y);
	     x = ewin->x;
	     y = ewin->y;
	     DrawEwinShape(ewin, conf.resizemode, x, y, w, h, mode.firstlast);
	     break;
	  default:
	     break;
	  }
	break;

     case MODE_RESIZE_H:
	switch (mode.resize_detail)
	  {
	  case 0:
	     pw = ewin->client.w;
	     w = mode.win_w - (mode.x - mode.start_x);
	     h = ewin->client.h;
	     x = mode.win_x + (mode.x - mode.start_x);
	     y = ewin->y;
	     ewin->client.w = w;
	     ICCCM_MatchSize(ewin);
	     w = ewin->client.w;
	     if (pw == ewin->client.w)
		x = ewin->x;
	     else
		x = mode.win_x + mode.win_w - w;
	     ewin->client.w = pw;
	     DrawEwinShape(ewin, conf.resizemode, x, y, w, h, mode.firstlast);
	     break;
	  case 1:
	     w = mode.win_w + (mode.x - mode.start_x);
	     h = ewin->client.h;
	     x = ewin->x;
	     y = ewin->y;
	     DrawEwinShape(ewin, conf.resizemode, x, y, w, h, mode.firstlast);
	     break;
	  default:
	     break;
	  }
	break;

     case MODE_RESIZE_V:
	switch (mode.resize_detail)
	  {
	  case 0:
	     ph = ewin->client.h;
	     w = ewin->client.w;
	     h = mode.win_h - (mode.y - mode.start_y);
	     x = ewin->x;
	     y = mode.win_y + (mode.y - mode.start_y);
	     ewin->client.h = h;
	     ICCCM_MatchSize(ewin);
	     h = ewin->client.h;
	     if (ph == ewin->client.h)
		y = ewin->y;
	     else
		y = mode.win_y + mode.win_h - h;
	     ewin->client.h = ph;
	     DrawEwinShape(ewin, conf.resizemode, x, y, w, h, mode.firstlast);
	     break;
	  case 1:
	     w = ewin->client.w;
	     h = mode.win_h + (mode.y - mode.start_y);
	     x = ewin->x;
	     y = ewin->y;
	     DrawEwinShape(ewin, conf.resizemode, x, y, w, h, mode.firstlast);
	     break;
	  default:
	     break;
	  }
	break;

     default:
	break;
     }
}
