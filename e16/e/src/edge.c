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

extern char         throw_move_events_away;

static Window       w1 = 0, w2 = 0, w3 = 0, w4 = 0;

static void
EdgeTimeout(int val, void *data)
{
   int                 ax, ay, aw, ah, dx, dy, dax, day;

   if (Mode.cur_menu_mode > 0)
      return;
   if (!Conf.edge_flip_resistance)
      return;
   throw_move_events_away = 1;
   GetCurrentArea(&ax, &ay);
   GetAreaSize(&aw, &ah);
   dx = 0;
   dy = 0;
   dax = 0;
   day = 0;
   switch (val)
     {
     case 0:
	if (ax == 0 && !Conf.areas.wraparound)
	   return;
	dx = root.w - 2;
	dax = -1;
	break;
     case 1:
	if (ax == (aw - 1) && !Conf.areas.wraparound)
	   return;
	dx = -(root.w - 2);
	dax = 1;
	break;
     case 2:
	if (ay == 0 && !Conf.areas.wraparound)
	   return;
	dy = root.h - 2;
	day = -1;
	break;
     case 3:
	if (ay == (ah - 1) && !Conf.areas.wraparound)
	   return;
	dy = -(root.h - 2);
	day = 1;
	break;
     default:
	break;
     }
   Mode.flipp = 1;
   MoveCurrentAreaBy(dax, day);
   Mode.flipp = 0;
   if (aw == 1)
      dx = 0;
   if (ah == 1)
      dy = 0;
   XWarpPointer(disp, None, None, 0, 0, 0, 0, dx, dy);
   data = NULL;
}

void
ShowEdgeWindows(void)
{
   int                 ax, ay, cx, cy;

   if (Conf.edge_flip_resistance <= 0)
     {
	HideEdgeWindows();
	return;
     }
   if (!w1)
     {
	w1 = ECreateEventWindow(root.win, 0, 0, 1, root.h);
	w2 = ECreateEventWindow(root.win, root.w - 1, 0, 1, root.h);
	w3 = ECreateEventWindow(root.win, 0, 0, root.w, 1);
	w4 = ECreateEventWindow(root.win, 0, root.h - 1, root.w, 1);
	XSelectInput(disp, w1,
		     EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w2,
		     EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w3,
		     EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
	XSelectInput(disp, w4,
		     EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		     ButtonPressMask | ButtonReleaseMask);
     }
   GetCurrentArea(&cx, &cy);
   GetAreaSize(&ax, &ay);

   if (cx == 0 && !Conf.areas.wraparound)
      EUnmapWindow(disp, w1);
   else
      EMapRaised(disp, w1);
   if (cx == (ax - 1) && !Conf.areas.wraparound)
      EUnmapWindow(disp, w2);
   else
      EMapRaised(disp, w2);
   if (cy == 0 && !Conf.areas.wraparound)
      EUnmapWindow(disp, w3);
   else
      EMapRaised(disp, w3);
   if (cy == (ay - 1) && !Conf.areas.wraparound)
      EUnmapWindow(disp, w4);
   else
      EMapRaised(disp, w4);
}

void
HideEdgeWindows(void)
{
   if (w1)
     {
	EUnmapWindow(disp, w1);
	EUnmapWindow(disp, w2);
	EUnmapWindow(disp, w3);
	EUnmapWindow(disp, w4);
     }
}

static int
IsEdgeWin(Window win)
{
   if (!w1)
      return -1;
   if (win == w1)
      return 0;
   else if (win == w2)
      return 1;
   else if (win == w3)
      return 2;
   else if (win == w4)
      return 3;
   return -1;
}

void
EdgeHandleEnter(XEvent * ev)
{
   int                 dir;

   dir = IsEdgeWin(ev->xcrossing.window);
   if (dir < 0)
      return;
   DoIn("EDGE_TIMEOUT", ((double)Conf.edge_flip_resistance) / 100.0,
	EdgeTimeout, dir, NULL);
}

void
EdgeHandleLeave(XEvent * ev)
{
   int                 dir;

   dir = IsEdgeWin(ev->xcrossing.window);
   if (dir < 0)
      return;
   RemoveTimerEvent("EDGE_TIMEOUT");
}

void
EdgeHandleMotion(XEvent * ev)
{
   static int          lastdir = -1;
   int                 dir;

   if (Mode.mode != MODE_MOVE_PENDING && Mode.mode != MODE_MOVE)
      return;

   dir = -1;
   if (ev->xmotion.x_root == 0)
      dir = 0;
   else if (ev->xmotion.x_root == (root.w - 1))
      dir = 1;
   else if (ev->xmotion.y_root == 0)
      dir = 2;
   else if (ev->xmotion.y_root == (root.h - 1))
      dir = 3;

   if ((lastdir != dir) && (Conf.edge_flip_resistance))
     {
	if (dir < 0)
	   RemoveTimerEvent("EDGE_TIMEOUT");
	else
	   DoIn("EDGE_TIMEOUT", ((double)Conf.edge_flip_resistance) / 100.0,
		EdgeTimeout, dir, NULL);
	lastdir = dir;
     }
}
