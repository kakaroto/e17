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
#include <sys/time.h>

static int          area_w = 3;
static int          area_h = 3;

void
AreaFix(int *ax, int *ay)
{
   if (*ax < 0)
     {
	if (Conf.areas.wraparound)
	   *ax = area_w - 1;
	else
	   *ax = 0;
     }
   else if (*ax >= area_w)
     {
	if (Conf.areas.wraparound)
	   *ax = 0;
	else
	   *ax = area_w - 1;
     }

   if (*ay < 0)
     {
	if (Conf.areas.wraparound)
	   *ay = area_h - 1;
	else
	   *ay = 0;
     }
   else if (*ay >= area_h)
     {
	if (Conf.areas.wraparound)
	   *ay = 0;
	else
	   *ay = area_h - 1;
     }
}

static int
AreaXYToLinear(int ax, int ay)
{
   AreaFix(&ax, &ay);
   return (ay * area_w) + ax;
}

static void
AreaLinearToXY(int a, int *ax, int *ay)
{
   if (a < 0)
      a = 0;
   else if (a >= (area_w * area_h))
      a = (area_w * area_h) - 1;
   *ay = a / area_w;
   *ax = a - (*ay * area_w);
}

void
SetNewAreaSize(int ax, int ay)
{

   int                 a, b, i, num;
   EWin               *const *lst;

   if (ax <= 0)
      return;
   if (ay <= 0)
      return;

   GetAreaSize(&a, &b);
   if ((a == ax) && (b == ay))
      return;

   SetAreaSize(ax, ay);

   lst = EwinListGet(&num);
   for (i = 0; i < num; i++)
     {
	if ((!(lst[i]->sticky)) && (!(lst[i]->fixedpos)))
	  {
	     if (lst[i]->area_x >= ax)
		MoveEwinToArea(lst[i], ax - 1, lst[i]->area_x);
	     if (lst[i]->area_y >= ay)
		MoveEwinToArea(lst[i], lst[i]->area_x, ay - 1);
	  }
     }

   GetCurrentArea(&a, &b);
   if (a >= ax)
     {
	SetCurrentArea(ax - 1, b);
	GetCurrentArea(&a, &b);
     }
   if (b >= ay)
      SetCurrentArea(a, ay - 1);
}

void
GetCurrentArea(int *ax, int *ay)
{
   EDBUG(4, "GetCurrentArea");
   *ax = desks.desk[desks.current].current_area_x;
   *ay = desks.desk[desks.current].current_area_y;
   EDBUG_RETURN_;
}

void
SetAreaSize(int aw, int ah)
{
   EDBUG(4, "SetAreaSize");
   if (aw < 1)
      aw = 1;
   if (ah < 1)
      ah = 1;
   Conf.areas.nx = area_w = aw;
   Conf.areas.ny = area_h = ah;
   HintsSetViewportConfig();
   PagerReArea();
   EDBUG_RETURN_;
}

void
GetAreaSize(int *aw, int *ah)
{
   EDBUG(4, "GetAreaSize");
   *aw = area_w;
   *ah = area_h;
   EDBUG_RETURN_;
}

void
InitCurrentArea(int ax, int ay)
{
   EDBUG(4, "InitCurrentArea");
   AreaFix(&ax, &ay);
   desks.desk[desks.current].current_area_x = ax;
   desks.desk[desks.current].current_area_y = ay;
   EDBUG_RETURN_;
}

void
SetCurrentLinearArea(int a)
{
   int                 ax, ay;

   AreaLinearToXY(a, &ax, &ay);
   SetCurrentArea(ax, ay);
}

int
GetCurrentLinearArea(void)
{
   return AreaXYToLinear(desks.desk[desks.current].current_area_x,
			 desks.desk[desks.current].current_area_y);
}

void
MoveCurrentLinearAreaBy(int a)
{
   SetCurrentLinearArea(GetCurrentLinearArea() + a);
}

void
SlideWindowsBy(Window * win, int num, int dx, int dy, int speed)
{
   int                 i, k, spd, x, y, min;
   struct timeval      timev1, timev2;
   int                 dsec, dusec;
   double              tm;
   struct _xy
   {
      int                 x, y;
   }                  *xy;

   EDBUG(5, "SlideWindowsBy");
   if (num < 1)
      EDBUG_RETURN_;

   spd = 16;
   min = 2;
   xy = Emalloc(sizeof(struct _xy) * num);

   for (i = 0; i < num; i++)
      GetWinXY(win[i], &(xy[i].x), &(xy[i].y));

   for (k = 0; k <= 1024; k += spd)
     {
	gettimeofday(&timev1, NULL);
	for (i = 0; i < num; i++)
	  {
	     x = ((xy[i].x * (1024 - k)) + ((xy[i].x + dx) * k)) >> 10;
	     y = ((xy[i].y * (1024 - k)) + ((xy[i].y + dy) * k)) >> 10;
	     EMoveWindow(disp, win[i], x, y);
	  }
	XSync(disp, False);
	gettimeofday(&timev2, NULL);
	dsec = timev2.tv_sec - timev1.tv_sec;
	dusec = timev2.tv_usec - timev1.tv_usec;
	if (dusec < 0)
	  {
	     dsec--;
	     dusec += 1000000;
	  }
	tm = (double)dsec + (((double)dusec) / 1000000);
	spd = (int)((double)speed * tm);
	if (spd < min)
	   spd = min;
     }

   for (i = 0; i < num; i++)
      EMoveWindow(disp, win[i], xy[i].x + dx, xy[i].y + dy);

   if (xy)
      Efree(xy);

   EDBUG_RETURN_;
}

void
SetCurrentArea(int ax, int ay)
{
   EWin               *const *lst, *ewin;
   int                 i, num, a1, a2, x, y, dx, dy;
   ToolTip            *tt;

   EDBUG(4, "SetCurrentArea");

   if ((Mode.mode == MODE_RESIZE) || (Mode.mode == MODE_RESIZE_H)
       || (Mode.mode == MODE_RESIZE_V))
      EDBUG_RETURN_;

   AreaFix(&ax, &ay);
   if ((ax == desks.desk[desks.current].current_area_x)
       && (ay == desks.desk[desks.current].current_area_y))
      EDBUG_RETURN_;

   tt = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);
   HideToolTip(tt);

   dx = root.w * (ax - desks.desk[desks.current].current_area_x);
   dy = root.h * (ay - desks.desk[desks.current].current_area_y);

   if (dx < 0)
      SoundPlay("SOUND_MOVE_AREA_LEFT");
   else if (dx > 0)
      SoundPlay("SOUND_MOVE_AREA_RIGHT");
   else if (dy < 0)
      SoundPlay("SOUND_MOVE_AREA_UP");
   else if (dy > 0)
      SoundPlay("SOUND_MOVE_AREA_DOWN");

   ActionsSuspend();

   /* remove lots of event masks from windows.. we dont want to bother */
   /* handling events as a result of our playing wiht windows */
   FocusNewDeskBegin();

   /* set the current area up in out data structs */
   desks.desk[desks.current].current_area_x = ax;
   desks.desk[desks.current].current_area_y = ay;

   /* move all the windows around */
   lst = EwinListGet(&num);
   if (Conf.desks.slidein)
     {
	int                 wnum = 0;
	Window             *wl = NULL;

	/* create the list of windwos to move */
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (ewin->desktop != desks.current || ewin->sticky ||
		 ewin->fixedpos || ewin->iconified)
		continue;

	     if ((ewin->floating) && (Conf.movemode > 0))
	       {
		  wnum++;
		  wl = Erealloc(wl, sizeof(Window) * wnum);
		  wl[wnum - 1] = ewin->win;
	       }
	     else if (!ewin->floating)
	       {
		  wnum++;
		  wl = Erealloc(wl, sizeof(Window) * wnum);
		  wl[wnum - 1] = ewin->win;
	       }
	  }

	/* slide them */
	if (wl)
	  {
	     SlideWindowsBy(wl, wnum, -dx, -dy, Conf.desks.slidespeed);
	     Efree(wl);
	  }

	/* move the windows to their final positions */
	for (i = 0; i < num; i++)
	  {
	     char                setflip = 0;

	     ewin = lst[i];
	     if (ewin->desktop != desks.current || ewin->sticky ||
		 ewin->fixedpos || ewin->floating || ewin->client.transient_for)
		continue;

	     a1 = ewin->area_x;
	     a2 = ewin->area_y;
	     if (!Mode.flipp)
	       {
		  setflip = 1;
		  Mode.flipp = 1;
	       }
	     MoveEwin(ewin, ewin->x - dx, ewin->y - dy);
	     if (setflip)
		Mode.flipp = 0;
	     ewin->area_x = a1;
	     ewin->area_y = a2;
	     HintsSetWindowArea(ewin);
	  }
     }
   else
     {
	/* move all windows across.... */
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (ewin->desktop != desks.current || ewin->sticky ||
		 ewin->fixedpos)
		continue;

	     /* if we're moving this window and its not opaque move */
	     /* warp it across without remebering the xy stuff */
	     /* well work out the xy stuff later when the move finishes */
	     if (ewin->floating)
	       {
		  if (Conf.movemode > 0)
		    {
		       GetWinXY(ewin->win, &x, &y);
		       EMoveWindow(disp, ewin->win, x - dx, y - dy);
		    }
	       }
	     /* if we're not moving it... move it across */
	     else if (!ewin->client.transient_for)
	       {
		  char                setflip = 0;

		  a1 = ewin->area_x;
		  a2 = ewin->area_y;
		  if (!Mode.flipp)
		    {
		       setflip = 1;
		       Mode.flipp = 1;
		    }
		  MoveEwin(ewin, ewin->x - dx, ewin->y - dy);
		  if (setflip)
		     Mode.flipp = 0;
		  ewin->area_x = a1;
		  ewin->area_y = a2;
		  HintsSetWindowArea(ewin);
	       }
	  }
     }

   /* set hints up for it */
   HintsSetDesktopViewport();
   XSync(disp, False);

   ActionsResume();

   /* re-focus on a new ewin on that new desktop area */
   FocusNewDesk();

   /* tell the FX api abotu the change */
   FX_DeskChange();

   /* update which "edge flip resistance" detector windows are visible */
   ShowEdgeWindows();

   /* update our pager */
   UpdatePagerSel();
   RedrawPagersForDesktop(desks.current, 3);
   ForceUpdatePagersForDesktop(desks.current);

   EDBUG_RETURN_;
}

void
MoveCurrentAreaBy(int ax, int ay)
{
   EDBUG(4, "MoveCurrentAreaBy");

   SetCurrentArea(desks.desk[desks.current].current_area_x + ax,
		  desks.desk[desks.current].current_area_y + ay);
   EDBUG_RETURN_;
}

void
MoveEwinToLinearArea(EWin * ewin, int a)
{
   int                 ax, ay;

   AreaLinearToXY(a, &ax, &ay);
   MoveEwinToArea(ewin, ax, ay);
}

void
MoveEwinLinearAreaBy(EWin * ewin, int a)
{
   MoveEwinToLinearArea(ewin, AreaXYToLinear(ewin->area_x, ewin->area_y) + a);
}
