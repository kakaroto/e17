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

static int          area_w = 3;
static int          area_h = 3;

void
AreaFix(int *ax, int *ay)
{
   if (*ax < 0)
     {
	if (Conf.desks.areas_wraparound)
	   *ax = area_w - 1;
	else
	   *ax = 0;
     }
   else if (*ax >= area_w)
     {
	if (Conf.desks.areas_wraparound)
	   *ax = 0;
	else
	   *ax = area_w - 1;
     }

   if (*ay < 0)
     {
	if (Conf.desks.areas_wraparound)
	   *ay = area_h - 1;
	else
	   *ay = 0;
     }
   else if (*ay >= area_h)
     {
	if (Conf.desks.areas_wraparound)
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

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (!EoIsSticky(lst[i]))
	  {
	     if (lst[i]->area_x >= ax)
		MoveEwinToArea(lst[i], ax - 1, lst[i]->area_x);
	     if (lst[i]->area_y >= ay)
		MoveEwinToArea(lst[i], lst[i]->area_x, ay - 1);
	  }
     }

   DeskGetCurrentArea(&a, &b);
   if (a >= ax)
     {
	SetCurrentArea(ax - 1, b);
	DeskGetCurrentArea(&a, &b);
     }
   if (b >= ay)
      SetCurrentArea(a, ay - 1);
}

void
SetAreaSize(int aw, int ah)
{
   EDBUG(4, "SetAreaSize");
   if (aw < 1)
      aw = 1;
   if (ah < 1)
      ah = 1;
   Conf.desks.areas_nx = area_w = aw;
   Conf.desks.areas_ny = area_h = ah;
   HintsSetViewportConfig();
   ModulesSignal(ESIGNAL_AREA_CONFIGURED, NULL);
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
SetCurrentLinearArea(int a)
{
   int                 ax, ay;

   AreaLinearToXY(a, &ax, &ay);
   SetCurrentArea(ax, ay);
}

int
GetCurrentLinearArea(void)
{
   int                 ax, ay;

   DeskGetCurrentArea(&ax, &ay);

   return AreaXYToLinear(ax, ay);
}

void
MoveCurrentLinearAreaBy(int a)
{
   SetCurrentLinearArea(GetCurrentLinearArea() + a);
}

void
SlideWindowsBy(Window * win, int num, int dx, int dy, int speed)
{
   int                 i, k, x, y;
   struct _xy
   {
      int                 x, y;
   }                  *xy;

   EDBUG(5, "SlideWindowsBy");
   if (num < 1)
      EDBUG_RETURN_;

   xy = Emalloc(sizeof(struct _xy) * num);

   for (i = 0; i < num; i++)
      GetWinXY(win[i], &(xy[i].x), &(xy[i].y));

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	for (i = 0; i < num; i++)
	  {
	     x = ((xy[i].x * (1024 - k)) + ((xy[i].x + dx) * k)) >> 10;
	     y = ((xy[i].y * (1024 - k)) + ((xy[i].y + dy) * k)) >> 10;
	     EMoveWindow(disp, win[i], x, y);
	  }
	ecore_x_sync();

	k = ETimedLoopNext();
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
   int                 i, num, dx, dy, pax, pay;

   EDBUG(4, "SetCurrentArea");

   if ((Mode.mode == MODE_RESIZE) || (Mode.mode == MODE_RESIZE_H)
       || (Mode.mode == MODE_RESIZE_V))
      EDBUG_RETURN_;

   AreaFix(&ax, &ay);
   DeskGetCurrentArea(&pax, &pay);

   if (ax == pax && ay == pay)
      EDBUG_RETURN_;

   if (EventDebug(EDBUG_TYPE_DESKS))
      Eprintf("SetCurrentArea %d,%d\n", ax, ay);

   ModulesSignal(ESIGNAL_AREA_SWITCH_START, NULL);

   dx = VRoot.w * (ax - pax);
   dy = VRoot.h * (ay - pay);

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
   DeskSetCurrentArea(ax, ay);

   /* move all the windows around */
   lst = EwinListGetAll(&num);
   if (Conf.desks.slidein)
     {
	int                 wnum = 0;
	Window             *wl = NULL;

	/* create the list of windwos to move */
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (EoIsSticky(ewin) || ewin->iconified)
		continue;
	     if (EoGetDesk(ewin) != DesksGetCurrent() && !EoIsFloating(ewin))
		continue;

	     if (!(EoIsFloating(ewin) && Conf.movres.mode_move == 0))
	       {
		  wnum++;
		  wl = Erealloc(wl, sizeof(Window) * wnum);
		  wl[wnum - 1] = EoGetWin(ewin);
	       }
	  }

	/* slide them */
	if (wl)
	  {
	     SlideWindowsBy(wl, wnum, -dx, -dy, Conf.desks.slidespeed);
	     Efree(wl);
	  }
     }

   /* move all windows to their final positions */
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	if (EoIsSticky(ewin))
	   continue;
	if (ewin->client.transient > 0)
	   continue;
	if (EoGetDesk(ewin) != DesksGetCurrent() && !EoIsFloating(ewin))
	   continue;

	if (!(EoIsFloating(ewin) && Conf.movres.mode_move == 0))
	   MoveEwin(ewin, EoGetX(ewin) - dx, EoGetY(ewin) - dy);
     }

   /* set hints up for it */
   HintsSetDesktopViewport();
   ecore_x_sync();

   ActionsResume();

   /* re-focus on a new ewin on that new desktop area */
   FocusNewDesk();

   ModulesSignal(ESIGNAL_AREA_SWITCH_DONE, NULL);

   /* update which "edge flip resistance" detector windows are visible */
   EdgeWindowsShow();

   EDBUG_RETURN_;
}

void
MoveCurrentAreaBy(int dx, int dy)
{
   int                 ax, ay;

   DeskGetCurrentArea(&ax, &ay);
   SetCurrentArea(ax + dx, ay + dy);
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
