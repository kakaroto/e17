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

static char         coords_visible = 0;
static Window       c_win = 0;
static int          cx = 0, cy = 0, cw = 0, ch = 0;

void
CoordsShow(EWin * ewin)
{
   TextClass          *tc;
   ImageClass         *ic;
   char                s[256], pq;
   int                 md;
   int                 x, y, w, h;

   if (!Conf.movres.mode_info)
      return;
   if (ewin == NULL)
      return;

   tc = TextclassFind("COORDS", 1);
   ic = ImageclassFind("COORDS", 1);
   if ((!ic) || (!tc))
      return;

   x = ewin->shape_x;
   y = ewin->shape_y;
   w = (ewin->client.w - ewin->client.base_w) / ewin->client.w_inc;
   h = (ewin->client.h - ewin->client.base_h) / ewin->client.h_inc;

   Esnprintf(s, sizeof(s), "%i x %i (%i, %i)", w, h, x, y);
   TextSize(tc, 0, 0, 0, s, &cw, &ch, 17);
   cw += (ic->padding.left + ic->padding.right);
   ch += (ic->padding.top + ic->padding.bottom);

   cx = 0;
   cy = 0;
   if (ewin)
     {
	if (Mode.mode == MODE_MOVE)
	   md = Conf.movres.mode_move;
	else
	   md = Conf.movres.mode_resize;

	if ((md == 0) || ((cw < ewin->client.w) && (ch < ewin->client.h)))
	  {
	     if (Conf.movres.mode_info == 1)
	       {
		  switch (md)
		    {
		    case 0:
		    case 1:
		    case 2:
		       cx = x + ((EoGetW(ewin) - cw) / 2) +
			  DeskGetX(EoGetDesk(ewin));
		       cy = y + ((EoGetH(ewin) - ch) / 2) +
			  DeskGetY(EoGetDesk(ewin));
		       break;
		    }
	       }
	  }
     }

   if (!c_win)
      c_win = ECreateWindow(VRoot.win, 0, 0, 1, 1, 2);

   EMoveResizeWindow(c_win, cx, cy, cw, ch);
   ERaiseWindow(c_win);

   if (!coords_visible)
      EMapWindow(c_win);
   coords_visible = 1;

   pq = Mode.queue_up;
   Mode.queue_up = 0;
   ImageclassApply(ic, c_win, cw, ch, 1, 0, STATE_NORMAL, 0, ST_UNKNWN);
   TextclassApply(ic, c_win, cw, ch, 0, 0, STATE_NORMAL, 0, tc, s);
   Mode.queue_up = pq;
   XFlush(disp);
}

void
CoordsHide(void)
{
   if (c_win)
      EUnmapWindow(c_win);
   coords_visible = 0;
}
