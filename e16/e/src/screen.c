/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2003-2004 Kim Woelders
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

#ifdef HAS_XINERAMA
char                xinerama_active;
static XineramaScreenInfo *screens = NULL;
static int          num_screens = 0;
#endif

void
ScreenInit(void)
{
#ifdef HAS_XINERAMA
   xinerama_active = XineramaIsActive(disp);
   Conf.extra_head = 0;
   if (xinerama_active)
      screens = XineramaQueryScreens(disp, &num_screens);
#endif
}

int
ScreenGetGeometry(int xi, int yi, int *px, int *py, int *pw, int *ph)
{
   int                 x, y, w, h, head;

   head = 0;

   x = 0;
   y = 0;
   w = root.w;
   h = root.h;

#ifdef HAS_XINERAMA
   if (xinerama_active)
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

int
GetPointerScreenGeometry(int *px, int *py, int *pw, int *ph)
{
   Window              rt, ch;
   int                 pointer_x, pointer_y;
   int                 d;
   unsigned int        ud;

   XQueryPointer(disp, root.win, &rt, &ch, &pointer_x, &pointer_y, &d, &d, &ud);

   return ScreenGetGeometry(pointer_x, pointer_y, px, py, pw, ph);
}
