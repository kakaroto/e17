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
#include <time.h>

Window              init_win1 = 0;
Window              init_win2 = 0;

void
CreateStartupDisplay(char start)
{
   static Window       w1, w2, win1, win2, b1, b2;
   static Background  *bg = NULL;
   static Background  *bg_sideways = NULL;	/* currently used to determine if the startup screen should slide sideways */
   static ImageClass  *ic = NULL;
   char                pq;

   if (!Conf.startup.animate)
      return;

   if (start)
     {
	bg_sideways = FindItem("STARTUP_BACKGROUND_SIDEWAYS", 0,
			       LIST_FINDBY_NAME, LIST_TYPE_BACKGROUND);
	ic = ImageclassFind("STARTUP_BAR", 0);
	if (!ic)
	   ic = ImageclassFind("DESKTOP_DRAGBUTTON_HORIZ", 0);
	bg =
	   FindItem("STARTUP_BACKGROUND", 0, LIST_FINDBY_NAME,
		    LIST_TYPE_BACKGROUND);
	if ((!ic) || (!bg))
	   EDBUG_RETURN_;

	if (bg_sideways)
	  {
	     w1 =
		ECreateWindow(VRoot.win, (VRoot.w / 2), 0, VRoot.w, VRoot.h, 1);
	     w2 =
		ECreateWindow(VRoot.win, -(VRoot.w / 2), 0, VRoot.w, VRoot.h,
			      1);
	     win1 = ECreateWindow(w1, -(VRoot.w / 2), 0, VRoot.w, VRoot.h, 0);
	     win2 = ECreateWindow(w2, (VRoot.w / 2), 0, VRoot.w, VRoot.h, 0);
	  }
	else
	  {
	     w1 =
		ECreateWindow(VRoot.win, 0, -(VRoot.h / 2), VRoot.w, VRoot.h,
			      1);
	     w2 =
		ECreateWindow(VRoot.win, 0, (VRoot.h / 2), VRoot.w, VRoot.h, 1);
	     win1 = ECreateWindow(w1, 0, (VRoot.h / 2), VRoot.w, VRoot.h, 0);
	     win2 = ECreateWindow(w2, 0, -(VRoot.h / 2), VRoot.w, VRoot.h, 0);
	  }

	EMapWindow(disp, win1);
	EMapWindow(disp, win2);

	b1 = ECreateWindow(w1, 0, VRoot.h - Conf.desks.dragbar_width, VRoot.w,
			   Conf.desks.dragbar_width, 0);
	b2 = ECreateWindow(w2, 0, 0, VRoot.w, Conf.desks.dragbar_width, 0);
	EMapRaised(disp, b1);
	EMapRaised(disp, b2);

	pq = Mode.queue_up;
	Mode.queue_up = 0;
	ImageclassApply(ic, b1, VRoot.w, Conf.desks.dragbar_width, 0, 0, 0, 0,
			ST_UNKNWN);
	ImageclassApply(ic, b2, VRoot.w, Conf.desks.dragbar_width, 0, 0, 0, 0,
			ST_UNKNWN);
	Mode.queue_up = pq;
	BackgroundApply(bg, win1, 1);
	BackgroundApply(bg, win2, 1);
	BackgroundImagesFree(bg, 1);
	init_win1 = w1;
	init_win2 = w2;
	EMapRaised(disp, w1);
	EMapRaised(disp, w2);
     }
   else
     {
	int                 k, x, y, xOffset, yOffset, ty, fy;

	if ((!ic) || (!bg))
	   EDBUG_RETURN_;

	fy = 0;

	ETimedLoopInit(0, 1024, Conf.slidespeedcleanup / 2);
	for (k = 0; k <= 1024;)
	  {
	     if (bg_sideways)
	       {		/* so we can have two different slide methods */
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

	     EMoveWindow(disp, w1, x + xOffset, -y - yOffset);
	     EMoveWindow(disp, w2, -x - xOffset, y + yOffset);
	     ecore_x_sync();

	     k = ETimedLoopNext();
	  }

	EDestroyWindow(disp, w1);
	EDestroyWindow(disp, w2);
	init_win1 = 0;
	init_win2 = 0;

	BackgroundDestroyByName("STARTUP_BACKGROUND_SIDEWAYS");
	BackgroundDestroyByName("STARTUP_BACKGROUND");
     }
   EDBUG_RETURN_;
}
