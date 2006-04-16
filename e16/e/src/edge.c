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
#include "eobj.h"
#include "ewins.h"
#include "menus.h"		/* FIXME - Should not be here */
#include "timers.h"
#include "xwin.h"

/* FIXME: Screen resizing not handled. */

static EObj        *w1 = NULL, *w2 = NULL, *w3 = NULL, *w4 = NULL;

static void
EdgeTimeout(int val, void *data __UNUSED__)
{
   int                 ax, ay, aw, ah, dx, dy, dax, day;
   EWin               *ewin;

   if (MenusActive())
      return;
   if (!Conf.edge_flip_resistance)
      return;

   /* Quit if pointer has left screen */
   if (!EQueryPointer(None, NULL, NULL, NULL, NULL))
      return;

   /* Quit if in fullscreen window */
   ewin = GetEwinPointerInClient();
   if (ewin && ewin->state.fullscreen)
      return;

   DeskCurrentGetArea(&ax, &ay);
   DesksGetAreaSize(&aw, &ah);
   dx = 0;
   dy = 0;
   dax = 0;
   day = 0;
   switch (val)
     {
     case 0:
	if (ax == 0 && !Conf.desks.areas_wraparound)
	   return;
	dx = VRoot.w - 2;
	dax = -1;
	break;
     case 1:
	if (ax == (aw - 1) && !Conf.desks.areas_wraparound)
	   return;
	dx = -(VRoot.w - 2);
	dax = 1;
	break;
     case 2:
	if (ay == 0 && !Conf.desks.areas_wraparound)
	   return;
	dy = VRoot.h - 2;
	day = -1;
	break;
     case 3:
	if (ay == (ah - 1) && !Conf.desks.areas_wraparound)
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
   Mode.events.px = Mode.events.x;
   Mode.events.py = Mode.events.y;
   Mode.events.x += dx;
   Mode.events.y += dy;
   EWarpPointer(VRoot.win, Mode.events.x, Mode.events.y);
   DeskCurrentMoveAreaBy(dax, day);
   Mode.events.px = Mode.events.x;
   Mode.events.py = Mode.events.y;
}

static void
EdgeEvent(int dir)
{
   static int          lastdir = -1;

#if 0
   Eprintf("EdgeEvent %d -> %d\n", lastdir, dir);
#endif
   if (lastdir == dir || !Conf.edge_flip_resistance)
      return;

   RemoveTimerEvent("EDGE_TIMEOUT");
   if (dir >= 0)
     {
	DoIn("EDGE_TIMEOUT",
	     ((double)Conf.edge_flip_resistance) / 100.0, EdgeTimeout,
	     dir, NULL);
     }
   lastdir = dir;
}

static void
EdgeHandleEvents(XEvent * ev, void *prm)
{
   static Time         last_time = 0;
   int                 dir;
   unsigned long       dt;

   dir = (long)prm;

   switch (ev->type)
     {
     case EnterNotify:
	/* Avoid excessive flipping */
	dt = ev->xcrossing.time - last_time;
	if (dt < 500)
	   return;
	last_time = ev->xcrossing.time;
	EdgeEvent(dir);
	break;

     case LeaveNotify:
	EdgeEvent(-1);
	break;

#if 0
     case MotionNotify:
	if (Mode.mode != MODE_MOVE_PENDING && Mode.mode != MODE_MOVE)
	   break;

	EdgeEvent(dir);
	break;
#endif
     }
}

void
EdgeCheckMotion(int x, int y)
{
   int                 dir;

   if (x == 0)
      dir = 0;
   else if (x == VRoot.w - 1)
      dir = 1;
   else if (y == 0)
      dir = 2;
   else if (y == VRoot.h - 1)
      dir = 3;
   else
      dir = -1;
   EdgeEvent(dir);
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
	w1 = EobjWindowCreate(EOBJ_TYPE_EVENT, 0, 0, 1, VRoot.h, 0, "Edge-L");
	w2 = EobjWindowCreate(EOBJ_TYPE_EVENT, VRoot.w - 1, 0, 1, VRoot.h, 0,
			      "Edge-R");
	w3 = EobjWindowCreate(EOBJ_TYPE_EVENT, 0, 0, VRoot.w, 1, 0, "Edge-T");
	w4 = EobjWindowCreate(EOBJ_TYPE_EVENT, 0, VRoot.h - 1, VRoot.w, 1, 0,
			      "Edge-B");
	ESelectInput(w1->win, EnterWindowMask | LeaveWindowMask);
	ESelectInput(w2->win, EnterWindowMask | LeaveWindowMask);
	ESelectInput(w3->win, EnterWindowMask | LeaveWindowMask);
	ESelectInput(w4->win, EnterWindowMask | LeaveWindowMask);
	EventCallbackRegister(w1->win, 0, EdgeHandleEvents, (void *)0);
	EventCallbackRegister(w2->win, 0, EdgeHandleEvents, (void *)1);
	EventCallbackRegister(w3->win, 0, EdgeHandleEvents, (void *)2);
	EventCallbackRegister(w4->win, 0, EdgeHandleEvents, (void *)3);
     }
   DeskCurrentGetArea(&cx, &cy);
   DesksGetAreaSize(&ax, &ay);

   if (cx == 0 && !Conf.desks.areas_wraparound)
      EobjUnmap(w1);
   else
      EobjMap(w1, 0);
   if (cx == (ax - 1) && !Conf.desks.areas_wraparound)
      EobjUnmap(w2);
   else
      EobjMap(w2, 0);
   if (cy == 0 && !Conf.desks.areas_wraparound)
      EobjUnmap(w3);
   else
      EobjMap(w3, 0);
   if (cy == (ay - 1) && !Conf.desks.areas_wraparound)
      EobjUnmap(w4);
   else
      EobjMap(w4, 0);
}

void
EdgeWindowsHide(void)
{
   if (!w1)
      return;

   EobjUnmap(w1);
   EobjUnmap(w2);
   EobjUnmap(w3);
   EobjUnmap(w4);
}
