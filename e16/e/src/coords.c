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

static char         coords_visible = 0;
static Window       c_win = 0;
static int          cx = 0, cy = 0, cw = 0, ch = 0;

void
SetCoords(int x, int y, int w, int h)
{
   TextClass          *tc = NULL;
   ImageClass         *ic = NULL;
   char                s[256], pq;
   EWin               *ewin;
   int                 md;

   if (!conf.geominfomode)
      return;

   if (!tc)
      tc = FindItem("COORDS", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
   if (!ic)
      ic = FindItem("COORDS", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!c_win)
      c_win = ECreateWindow(root.win, 0, 0, 1, 1, 2);
   if ((!ic) || (!tc))
      return;

   Esnprintf(s, sizeof(s), "%i x %i (%i, %i)", w, h, x, y);
   TextSize(tc, 0, 0, 0, s, &cw, &ch, 17);
   ewin = mode.ewin;
   cw += (ic->padding.left + ic->padding.right);
   ch += (ic->padding.top + ic->padding.bottom);
   if (ewin)
     {
	md = 0;
	if (mode.mode == MODE_MOVE)
	   md = conf.movemode;
	else
	   md = conf.resizemode;
	if ((md > 0) && ((cw >= (ewin->w)) || (ch >= (ewin->h))))
	  {
	     cx = 0;
	     cy = 0;
	  }
	else
	  {
	     cx = 0;
	     cy = 0;
	     if (conf.geominfomode == 1)
	       {
		  switch (md)
		    {
		    case 0:
		    case 1:
		    case 2:
		       cx = ewin->x + ((ewin->w - cw) / 2) +
			  desks.desk[ewin->desktop].x;
		       cy = ewin->y + ((ewin->h - ch) / 2) +
			  desks.desk[ewin->desktop].y;
		       break;
		    }
	       }
	  }
     }
   else
     {
	cx = 0;
	cy = 0;
     }
   if (!coords_visible)
      EMapWindow(disp, c_win);
   XRaiseWindow(disp, c_win);
   EMoveResizeWindow(disp, c_win, cx, cy, cw, ch);
   pq = queue_up;
   queue_up = 0;
   IclassApply(ic, c_win, cw, ch, 1, 0, STATE_NORMAL, 0);
   TclassApply(ic, c_win, cw, ch, 0, 0, STATE_NORMAL, 0, tc, s);
   queue_up = pq;
   XFlush(disp);
   coords_visible = 1;
}

void
HideCoords(void)
{
   if (c_win)
      EUnmapWindow(disp, c_win);
   coords_visible = 0;
}
