/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2003-2006 Kim Woelders
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
#include "ewins.h"
#include "xwin.h"
#ifdef HAS_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif

#ifdef HAS_XINERAMA
static XineramaScreenInfo *screens = NULL;
static int          num_screens = 0;
#endif

void
ScreenInit(void)
{
   if (Mode.wm.window)
      return;
#ifdef HAS_XINERAMA
   Mode.display.xinerama_active = XineramaIsActive(disp);
   Conf.extra_head = 0;
   if (Mode.display.xinerama_active)
      screens = XineramaQueryScreens(disp, &num_screens);
#endif
}

void
ScreenShowInfo(void)
{
#ifdef HAS_XINERAMA
   if (Mode.display.xinerama_active)
     {
	XineramaScreenInfo *scrns;
	int                 num, i;

	scrns = XineramaQueryScreens(disp, &num);

	IpcPrintf("Xinerama active:\n");
	IpcPrintf("Head  Screen  X-Origin  Y-Origin     Width    Height\n");
	for (i = 0; i < num; i++)
	  {
	     IpcPrintf(" %2d     %2d       %5d     %5d     %5d     %5d\n",
		       i, scrns[i].screen_number,
		       scrns[i].x_org, scrns[i].y_org, scrns[i].width,
		       scrns[i].height);
	  }
	XFree(screens);
	return;
     }
   else
     {
	IpcPrintf("Xinerama is not active\n");
     }
#endif

   IpcPrintf("Head  Screen  X-Origin  Y-Origin     Width    Height\n");
   IpcPrintf(" %2d     %2d       %5d     %5d     %5d     %5d\n",
	     0, VRoot.scr, 0, 0, VRoot.w, VRoot.h);
}

int
ScreenGetGeometry(int xi, int yi, int *px, int *py, int *pw, int *ph)
{
   int                 x, y, w, h, head;

   head = 0;

   x = 0;
   y = 0;
   w = VRoot.w;
   h = VRoot.h;

#ifdef HAS_XINERAMA
   if (Mode.display.xinerama_active)
     {
	int                 i;

	for (i = 0; i < num_screens; i++)
	  {
	     XineramaScreenInfo *ps = &screens[i];

	     if (xi >= ps->x_org && xi < ps->x_org + ps->width &&
		 yi >= ps->y_org && yi < ps->y_org + ps->height)
	       {
		  x = ps->x_org;
		  y = ps->y_org;
		  w = ps->width;
		  h = ps->height;
		  head = i;
		  /* NB! *First* matching head is used */
		  break;
	       }
	  }
     }
#endif

   if (px)
      *px = x;
   if (py)
      *py = y;
   if (pw)
      *pw = w;
   if (ph)
      *ph = h;

   return head;
}

static void
VRootGetAvailableArea(int *px, int *py, int *pw, int *ph)
{
   EWin               *const *lst, *ewin;
   int                 i, num, l, r, t, b;

   l = r = t = b = 0;
   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];

	if (l < ewin->strut.left)
	   l = ewin->strut.left;
	if (r < ewin->strut.right)
	   r = ewin->strut.right;
	if (t < ewin->strut.top)
	   t = ewin->strut.top;
	if (b < ewin->strut.bottom)
	   b = ewin->strut.bottom;
     }

   *px = l;
   *py = t;
   *pw = VRoot.w - (l + r);
   *ph = VRoot.h - (t + b);
}

int
ScreenGetAvailableArea(int xi, int yi, int *px, int *py, int *pw, int *ph)
{
   int                 x1, y1, w1, h1, x2, y2, w2, h2, head;

   head = ScreenGetGeometry(xi, yi, &x1, &y1, &w1, &h1);

   if (!Conf.place.ignore_struts)
     {
	VRootGetAvailableArea(&x2, &y2, &w2, &h2);
	if (x1 < x2)
	   x1 = x2;
	if (y1 < y2)
	   y1 = y2;
	if (w1 > w2)
	   w1 = w2;
	if (h1 > h2)
	   h1 = h2;
     }

   *px = x1;
   *py = y1;
   *pw = w1;
   *ph = h1;

   return head;
}

int
GetPointerScreenGeometry(int *px, int *py, int *pw, int *ph)
{
   int                 pointer_x, pointer_y;

   EQueryPointer(VRoot.win, &pointer_x, &pointer_y, NULL, NULL);

   return ScreenGetGeometry(pointer_x, pointer_y, px, py, pw, ph);
}

int
GetPointerScreenAvailableArea(int *px, int *py, int *pw, int *ph)
{
   int                 pointer_x, pointer_y;

   EQueryPointer(VRoot.win, &pointer_x, &pointer_y, NULL, NULL);

   return ScreenGetAvailableArea(pointer_x, pointer_y, px, py, pw, ph);
}
