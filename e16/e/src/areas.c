/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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

#define AREA_FIX(ax, ay) \
if (ax < 0) \
{ \
if (conf.areas.wraparound) \
ax = area_w - 1; \
else \
ax = 0; \
} \
else if (ax >= area_w) \
{ \
if (conf.areas.wraparound) \
ax = 0; \
else \
ax = area_w - 1; \
} \
if (ay < 0) \
{ \
if (conf.areas.wraparound) \
ay = area_h - 1; \
else \
ay = 0; \
} \
else if (ay >= area_h) \
{ \
if (conf.areas.wraparound) \
ay = 0; \
else \
ay = area_h - 1; \
}

void
SetNewAreaSize(int ax, int ay)
{

   int                 a, b, i, num;
   EWin              **lst;

   if (ax <= 0)
      return;
   if (ay <= 0)
      return;

   GetAreaSize(&a, &b);
   if ((a == ax) && (b == ay))
      return;
   SetAreaSize(ax, ay);
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
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
	Efree(lst);
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
   area_w = aw;
   area_h = ah;
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
   AREA_FIX(ax, ay);
   desks.desk[desks.current].current_area_x = ax;
   desks.desk[desks.current].current_area_y = ay;
   EDBUG_RETURN_;
}

void
SetCurrentLinearArea(int a)
{
   if (a < 0)
      a = 0;
   else if (a >= (area_w * area_h))
      a = (area_w * area_h) - 1;
   SetCurrentArea(a - ((a / area_w) * area_w), (a / area_w));
}

int
GetCurrentLinearArea(void)
{
   return ((desks.desk[desks.current].current_area_y * area_w) +
	   desks.desk[desks.current].current_area_x);
}

void
MoveCurrentLinearAreaBy(int a)
{
   SetCurrentLinearArea(GetCurrentLinearArea() + a);
}

void
MoveEwinToLinearArea(EWin * ewin, int a)
{
   if (a < 0)
      a = 0;
   else if (a >= (area_w * area_h))
      a = (area_w * area_h) - 1;
   MoveEwinToArea(ewin, a - ((a / area_w) * area_w), (a / area_w));
}

void
MoveEwinLinearAreaBy(EWin * ewin, int a)
{
   a += (ewin->area_y * area_w) + (ewin->area_x);
   if (a < 0)
      a = 0;
   else if (a >= (area_w * area_h))
      a = (area_w * area_h) - 1;
   MoveEwinToArea(ewin, a - ((a / area_w) * area_w), (a / area_w));
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
   }
                      *xy;

   EDBUG(5, "SlideWindowsBy");
   spd = 16;
   min = 2;
   if (num < 1)
      EDBUG_RETURN_;
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
   EWin              **lst;
   int                 i, num, a1, a2, x, y, dx, dy;
   ToolTip            *tt;

   EDBUG(4, "SetCurrentArea");

   if ((mode.mode == MODE_RESIZE) || (mode.mode == MODE_RESIZE_H)
       || (mode.mode == MODE_RESIZE_V))
      EDBUG_RETURN_;

   AREA_FIX(ax, ay);
   if ((ax == desks.desk[desks.current].current_area_x)
       && (ay == desks.desk[desks.current].current_area_y))
      EDBUG_RETURN_;

   tt = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);
   HideToolTip(tt);

   dx = ax - desks.desk[desks.current].current_area_x;
   dy = ay - desks.desk[desks.current].current_area_y;
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
#if 0				/* Clean up if not causing trouble */
   FocusToEWin(NULL);
#endif
   BeginNewDeskFocus();

   /* move all the windows around */
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	if (desks.slidein)
	  {
	     int                 wnum = 0;
	     Window             *wl = NULL;

	     /* create the list of windwos to move */
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->desktop == desks.current) && (!lst[i]->sticky)
		      && (!lst[i]->fixedpos) && (!lst[i]->iconified))
		    {
		       if ((lst[i]->floating) && (conf.movemode > 0))
			 {
			    wnum++;
			    wl = Erealloc(wl, sizeof(Window) * wnum);
			    wl[wnum - 1] = lst[i]->win;
			 }
		       else if (!lst[i]->floating)
			 {
			    wnum++;
			    wl = Erealloc(wl, sizeof(Window) * wnum);
			    wl[wnum - 1] = lst[i]->win;
			 }
		    }
	       }
	     /* slide them */
	     if (wl)
	       {
		  SlideWindowsBy(wl, wnum,
				 -(root.w *
				   (ax -
				    desks.desk[desks.current].current_area_x)),
				 -(root.h *
				   (ay -
				    desks.desk[desks.current].current_area_y)),
				 desks.slidespeed);
		  Efree(wl);
	       }
	     /* move the windows to their final positions */
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->desktop == desks.current) && (!lst[i]->sticky)
		      && (!lst[i]->fixedpos))
		    {
		       if (!lst[i]->floating)
			 {
			    char                setflip = 0;

			    a1 = lst[i]->area_x;
			    a2 = lst[i]->area_y;
			    if (!mode.flipp)
			      {
				 setflip = 1;
				 mode.flipp = 1;
			      }
			    MoveEwin(lst[i],
				     lst[i]->x -
				     (root.w *
				      (ax -
				       desks.desk[desks.
						  current].current_area_x)),
				     lst[i]->y -
				     (root.h *
				      (ay -
				       desks.desk[desks.
						  current].current_area_y)));
			    if (setflip)
			       mode.flipp = 0;
			    lst[i]->area_x = a1;
			    lst[i]->area_y = a2;
			    HintsSetWindowArea(lst[i]);
			 }
		    }
	       }
	  }
	else
	  {
	     /* move all widnwos across.... */
	     for (i = 0; i < num; i++)
	       {
		  if ((lst[i]->desktop == desks.current) && (!lst[i]->sticky)
		      && (!lst[i]->fixedpos))
		    {
		       /* if we're moving this window and its not opaque move */
		       /* warp it across withotu remebering the xy stuff */
		       /* well work out the xy stuff later when the move finishes */
		       if (lst[i]->floating)
			 {
			    if (conf.movemode > 0)
			      {
				 GetWinXY(lst[i]->win, &x, &y);
				 EMoveWindow(disp, lst[i]->win,
					     x -
					     (root.w *
					      (ax -
					       desks.desk[desks.
							  current].
					       current_area_x)),
					     y -
					     (root.h *
					      (ay -
					       desks.desk[desks.current].
					       current_area_y)));
			      }
			 }
		       /* if we're not moving it... move it across */
		       else
			 {
			    char                setflip = 0;

			    a1 = lst[i]->area_x;
			    a2 = lst[i]->area_y;
			    if (!mode.flipp)
			      {
				 setflip = 1;
				 mode.flipp = 1;
			      }
			    MoveEwin(lst[i],
				     lst[i]->x -
				     (root.w *
				      (ax -
				       desks.desk[desks.
						  current].current_area_x)),
				     lst[i]->y -
				     (root.h *
				      (ay -
				       desks.desk[desks.
						  current].current_area_y)));
			    if (setflip)
			       mode.flipp = 0;
			    lst[i]->area_x = a1;
			    lst[i]->area_y = a2;
			    HintsSetWindowArea(lst[i]);
			 }
		    }
	       }
	  }
	Efree(lst);
     }

   /* set the current area up in out data structs */
   desks.desk[desks.current].current_area_x = ax;
   desks.desk[desks.current].current_area_y = ay;

   /* set hints up for it */
   HintsSetDesktopViewport();
   XSync(disp, False);

   ActionsResume();

   /* re-focus on a new ewin on that new desktop area */
   NewDeskFocus();

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
MoveEwinToArea(EWin * ewin, int ax, int ay)
{
   EDBUG(4, "MoveEwinToArea");
   AREA_FIX(ax, ay);
   MoveEwin(ewin, ewin->x + (root.w * (ax - ewin->area_x)),
	    ewin->y + (root.h * (ay - ewin->area_y)));
   ewin->area_x = ax;
   ewin->area_y = ay;
   HintsSetWindowArea(ewin);
   EDBUG_RETURN_;
}

void
SetEwinToCurrentArea(EWin * ewin)
{
   EDBUG(4, "SetEwinToCurrentArea");
   ewin->area_x = desks.desk[ewin->desktop].current_area_x;
   ewin->area_y = desks.desk[ewin->desktop].current_area_y;
   HintsSetWindowArea(ewin);
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
