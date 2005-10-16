/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
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
#include "eobj.h"
#include "iclass.h"
#include "xwin.h"
#include <time.h>

static EObj        *init_win1 = NULL;
static EObj        *init_win2 = NULL;
static char         bg_sideways = 0;

void
StartupWindowsCreate(void)
{
   Window              w1, w2, win1, win2, b1, b2;
   Background         *bg;
   ImageClass         *ic;
   int                 x, y, bx, by, bw, bh;
   EObj               *eo;

   if (!Conf.startup.animate)
      return;

   /* Acting only as boolean? */
   if (FindItem
       ("STARTUP_BACKGROUND_SIDEWAYS", 0, LIST_FINDBY_NAME,
	LIST_TYPE_BACKGROUND))
      bg_sideways = 1;

   ic = ImageclassFind("STARTUP_BAR", 0);
   if (!ic)
      ic = ImageclassFind("DESKTOP_DRAGBUTTON_HORIZ", 0);
   bg = FindItem("STARTUP_BACKGROUND", 0, LIST_FINDBY_NAME,
		 LIST_TYPE_BACKGROUND);
   if (!ic || !bg)
      return;

   if (bg_sideways)
     {
	x = VRoot.w / 2;
	y = 0;
	bx = VRoot.w - Conf.desks.dragbar_width;
	by = 0;
	bw = Conf.desks.dragbar_width;
	bh = VRoot.h;
     }
   else
     {
	x = 0;
	y = VRoot.h / 2;
	bx = 0;
	by = VRoot.h - Conf.desks.dragbar_width;
	bw = VRoot.w;
	bh = Conf.desks.dragbar_width;
     }

   eo = EobjWindowCreate(EOBJ_TYPE_MISC, -x, -y, VRoot.w, VRoot.h, 1, "Init-1");
   if (!eo)
      return;
   init_win1 = eo;
   w1 = eo->win;
   eo->fade = 0;
   win1 = ECreateWindow(w1, x, y, VRoot.w, VRoot.h, 0);

   eo = EobjWindowCreate(EOBJ_TYPE_MISC, x, y, VRoot.w, VRoot.h, 1, "Init-2");
   if (!eo)
      return;
   init_win2 = eo;
   w2 = eo->win;
   eo->fade = 0;
   win2 = ECreateWindow(w2, -x, -y, VRoot.w, VRoot.h, 0);

   EMapWindow(win1);
   EMapWindow(win2);

   if (ic && bw > 0 && bh > 0)
     {
	b1 = ECreateWindow(w1, bx, by, bw, bh, 0);
	b2 = ECreateWindow(w2, 0, 0, bw, bh, 0);
	EMapRaised(b1);
	EMapRaised(b2);

	ImageclassApply(ic, b1, bw, bh, 0, 0, 0, 0, ST_SOLID);
	ImageclassApply(ic, b2, bw, bh, 0, 0, 0, 0, ST_SOLID);
     }

   BackgroundSet(bg, win1, VRoot.w, VRoot.h);
   BackgroundSet(bg, win2, VRoot.w, VRoot.h);
   BackgroundImagesFree(bg, 1);

   EobjMap(init_win1, 0);
   EobjMap(init_win2, 0);

   EobjsRepaint();
}

void
StartupWindowsOpen(void)
{
   int                 k, x, y, xOffset, yOffset, ty, fy;

   if (init_win1 == None || init_win2 == None)
      return;

   fy = 0;

   ETimedLoopInit(0, 1024, Conf.desks.slidespeed / 2);
   for (k = 0; k <= 1024;)
     {
	if (bg_sideways)
	  {			/* so we can have two different slide methods */
	     ty = (VRoot.w / 2);
	     xOffset = ((fy * (1024 - k)) + (ty * k)) >> 10;
	     x = ty;
	     yOffset = 0;
	     y = 0;
	  }
	else
	  {
	     ty = (VRoot.h / 2);
	     xOffset = 0;
	     x = 0;
	     yOffset = ((fy * (1024 - k)) + (ty * k)) >> 10;
	     y = ty;
	  }

	EobjMove(init_win1, -x - xOffset, -y - yOffset);
	EobjMove(init_win2, x + xOffset, y + yOffset);

	k = ETimedLoopNext();
     }

   EobjWindowDestroy(init_win1);
   EobjWindowDestroy(init_win2);
   init_win1 = NULL;
   init_win2 = NULL;

   BackgroundDestroyByName("STARTUP_BACKGROUND_SIDEWAYS");
   BackgroundDestroyByName("STARTUP_BACKGROUND");
}
