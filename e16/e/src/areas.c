/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
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
		EwinMoveToArea(lst[i], ax - 1, lst[i]->area_x);
	     if (lst[i]->area_y >= ay)
		EwinMoveToArea(lst[i], lst[i]->area_x, ay - 1);
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
   if (aw < 1)
      aw = 1;
   if (ah < 1)
      ah = 1;
   Conf.desks.areas_nx = area_w = aw;
   Conf.desks.areas_ny = area_h = ah;
   HintsSetViewportConfig();
   EdgeWindowsShow();
   ModulesSignal(ESIGNAL_AREA_CONFIGURED, NULL);
}

void
GetAreaSize(int *aw, int *ah)
{
   *aw = area_w;
   *ah = area_h;
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
SetCurrentArea(int ax, int ay)
{
   EWin               *const *lst, *ewin;
   int                 i, num, dx, dy, pax, pay;

   if ((Mode.mode == MODE_RESIZE) || (Mode.mode == MODE_RESIZE_H)
       || (Mode.mode == MODE_RESIZE_V))
      return;

   AreaFix(&ax, &ay);
   DeskGetCurrentArea(&pax, &pay);

   if (ax == pax && ay == pay)
      return;

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
	EObj              **wl = NULL;

	/* create the list of windwos to move */
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (EoIsSticky(ewin) || ewin->state.iconified)
		continue;
	     if (EoGetDesk(ewin) != DesksGetCurrent() && !EoIsFloating(ewin))
		continue;

	     if (EoIsFloating(ewin) && Conf.movres.mode_move == 0)
		continue;

	     wnum++;
	     wl = Erealloc(wl, sizeof(EObj *) * wnum);
	     wl[wnum - 1] = &ewin->o;
	  }

	/* slide them */
	if (wl)
	  {
	     EobjsSlideBy(wl, wnum, -dx, -dy, Conf.desks.slidespeed);
	     Efree(wl);
	     EobjsRepaint();
	  }
     }

   /* move all windows to their final positions */
   Mode.move.check = 0;
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	if (ewin->client.transient > 0)
	   continue;
	if (EoGetDesk(ewin) != DesksGetCurrent() && !EoIsFloating(ewin))
	   continue;

	if (EoIsSticky(ewin) ||
	    (EoIsFloating(ewin) && Conf.movres.mode_move == 0) ||
	    (!ewin->state.iconified && Conf.desks.slidein))
	   EwinMove(ewin, EoGetX(ewin), EoGetY(ewin));
	else
	   EwinMove(ewin, EoGetX(ewin) - dx, EoGetY(ewin) - dy);
     }
   Mode.move.check = 1;

   if (!Conf.desks.slidein)
      EobjsRepaint();

   /* set hints up for it */
   HintsSetDesktopViewport();

   ActionsResume();

   /* re-focus on a new ewin on that new desktop area */
   FocusNewDesk();

   ModulesSignal(ESIGNAL_AREA_SWITCH_DONE, ((void *)(long)DesksGetCurrent()));

   /* update which "edge flip resistance" detector windows are visible */
   EdgeWindowsShow();
}

void
MoveCurrentAreaBy(int dx, int dy)
{
   int                 ax, ay;

   DeskGetCurrentArea(&ax, &ay);
   SetCurrentArea(ax + dx, ay + dy);
}

void
EwinMoveToLinearArea(EWin * ewin, int a)
{
   int                 ax, ay;

   AreaLinearToXY(a, &ax, &ay);
   EwinMoveToArea(ewin, ax, ay);
}

void
EwinMoveLinearAreaBy(EWin * ewin, int a)
{
   EwinMoveToLinearArea(ewin, AreaXYToLinear(ewin->area_x, ewin->area_y) + a);
}
