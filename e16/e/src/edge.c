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
   EWin               *ewin;

   if (MenusActive())
      return;
   if (!Conf.edge_flip_resistance)
      return;

   ewin = GetEwinPointerInClient();
   if (ewin && ewin->st.fullscreen)
      return;

   throw_move_events_away = 1;
   DeskGetCurrentArea(&ax, &ay);
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
	dx = VRoot.w - 2;
	dax = -1;
	break;
     case 1:
	if (ax == (aw - 1) && !Conf.areas.wraparound)
	   return;
	dx = -(VRoot.w - 2);
	dax = 1;
	break;
     case 2:
	if (ay == 0 && !Conf.areas.wraparound)
	   return;
	dy = VRoot.h - 2;
	day = -1;
	break;
     case 3:
	if (ay == (ah - 1) && !Conf.areas.wraparound)
	   return;
	dy = -(VRoot.h - 2);
	day = 1;
	break;
     default:
	break;
     }
   if (aw == 1)
      dx = 0;
   if (ah == 1)
      dy = 0;
   Mode.px = Mode.x;
   Mode.py = Mode.y;
   Mode.x += dx;
   Mode.y += dy;
   XWarpPointer(disp, None, None, 0, 0, 0, 0, dx, dy);
   Mode.flipp = 1;
   MoveCurrentAreaBy(dax, day);
   Mode.flipp = 0;
   Mode.px = Mode.x;
   Mode.py = Mode.y;
   data = NULL;
}

static void
EdgeHandleEvents(XEvent * ev, void *prm)
{
   static int          lastdir = -1;
   int                 dir;

   dir = (int)prm;
   if (dir < 0 || dir > 3)	/* Should not be possible */
      return;

   switch (ev->type)
     {
     case EnterNotify:
	DoIn("EDGE_TIMEOUT", ((double)Conf.edge_flip_resistance) / 100.0,
	     EdgeTimeout, dir, NULL);
	break;

     case LeaveNotify:
	RemoveTimerEvent("EDGE_TIMEOUT");
	break;

     case MotionNotify:
	if (Mode.mode != MODE_MOVE_PENDING && Mode.mode != MODE_MOVE)
	   break;

	if ((lastdir != dir) && (Conf.edge_flip_resistance))
	  {
	     if (dir < 0)
		RemoveTimerEvent("EDGE_TIMEOUT");
	     else
		DoIn("EDGE_TIMEOUT",
		     ((double)Conf.edge_flip_resistance) / 100.0, EdgeTimeout,
		     dir, NULL);
	     lastdir = dir;
	  }
	break;
     }
}

void
EdgeWindowsShow(void)
{
   int                 ax, ay, cx, cy;

   if (Conf.edge_flip_resistance <= 0)
     {
	EdgeWindowsHide();
	return;
     }

   if (!w1)
     {
	w1 = ECreateEventWindow(VRoot.win, 0, 0, 1, VRoot.h);
	w2 = ECreateEventWindow(VRoot.win, VRoot.w - 1, 0, 1, VRoot.h);
	w3 = ECreateEventWindow(VRoot.win, 0, 0, VRoot.w, 1);
	w4 = ECreateEventWindow(VRoot.win, 0, VRoot.h - 1, VRoot.w, 1);
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
	EventCallbackRegister(w1, 0, EdgeHandleEvents, (void *)0);
	EventCallbackRegister(w2, 0, EdgeHandleEvents, (void *)1);
	EventCallbackRegister(w3, 0, EdgeHandleEvents, (void *)2);
	EventCallbackRegister(w4, 0, EdgeHandleEvents, (void *)3);
     }
   DeskGetCurrentArea(&cx, &cy);
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
EdgeWindowsHide(void)
{
   if (w1)
     {
	EUnmapWindow(disp, w1);
	EUnmapWindow(disp, w2);
	EUnmapWindow(disp, w3);
	EUnmapWindow(disp, w4);
     }
}
