/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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

void
SlideWindowSizeTo(Window win, int fx, int fy, int tx, int ty, int fw, int fh,
		  int tw, int th, int speed)
{
   int                 k, spd, x, y, min, w, h;
   struct timeval      timev1, timev2;
   int                 dsec, dusec;
   double              tm;

   EDBUG(5, "SlideWindowTo");
   spd = 16;
   min = 2;
   GrabX();
   for (k = 0; k <= 1024; k += spd)
     {
	gettimeofday(&timev1, NULL);
	x = ((fx * (1024 - k)) + (tx * k)) >> 10;
	y = ((fy * (1024 - k)) + (ty * k)) >> 10;
	w = ((fw * (1024 - k)) + (tw * k)) >> 10;
	h = ((fh * (1024 - k)) + (th * k)) >> 10;
	EMoveResizeWindow(disp, win, x, y, w, h);
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
   EMoveResizeWindow(disp, win, tx, ty, tw, th);
   UngrabX();
   EDBUG_RETURN_;
}

Slideout           *
CreateSlideout(char *name, char dir)
{
   Slideout           *s;

   EDBUG(5, "CreateSlideout");

   s = Emalloc(sizeof(Slideout));
   if (!s)
      EDBUG_RETURN(NULL);

   s->name = duplicate(name);
   s->direction = dir;
   s->num_buttons = 0;
   s->button = NULL;
   s->w = 0;
   s->h = 0;
   s->win = ECreateWindow(root.win, -10, -10, 1, 1, 1);
   s->from_win = 0;
   s->ref_count = 0;

   EDBUG_RETURN(s);
}

void
ShowSlideout(Slideout * s, Window win)
{
   int                 x, y, i, xx, yy, di;
   Window              dw;
   char                pdir;
   XSetWindowAttributes att;
   unsigned int        w, h, d;

   EDBUG(5, "ShowSlideout");

   if (mode.slideout)
      EDBUG_RETURN_;

   CalcSlideoutSize(s);
   EGetGeometry(disp, win, &dw, &di, &di, &w, &h, &d, &d);
   XTranslateCoordinates(disp, win, root.win, 0, 0, &x, &y, &dw);

   xx = 0;
   yy = 0;
   switch (s->direction)
     {
     case 2:
	xx = x + ((w - s->w) >> 1);
	yy = y - s->h;
	if ((yy < 0) && (s->h < root.h))
	  {
	     pdir = s->direction;
	     s->direction = 1;
	     ShowSlideout(s, win);
	     s->direction = pdir;
	     EDBUG_RETURN_;
	  }
	break;
     case 3:
	xx = x + ((w - s->w) >> 1);
	yy = y + h;
	if (((yy + s->h) > root.h) && (s->h < root.h))
	  {
	     pdir = s->direction;
	     s->direction = 0;
	     ShowSlideout(s, win);
	     s->direction = pdir;
	     EDBUG_RETURN_;
	  }
	break;
     case 0:
	xx = x - s->w;
	yy = y + ((h - s->h) >> 1);
	if ((xx < 0) && (s->w < root.w))
	  {
	     pdir = s->direction;
	     s->direction = 1;
	     ShowSlideout(s, win);
	     s->direction = pdir;
	     EDBUG_RETURN_;
	  }
	break;
     case 1:
	xx = x + w;
	yy = y + ((h - s->h) >> 1);
	if (((xx + s->w) > root.w) && (s->w < root.w))
	  {
	     pdir = s->direction;
	     s->direction = 0;
	     ShowSlideout(s, win);
	     s->direction = pdir;
	     EDBUG_RETURN_;
	  }
	break;
     default:
	break;
     }

   if ((mode.ewin) && (!mode.ewin->sticky) && (!mode.ewin->floating))
     {
	xx -= desks.desk[DESKTOPS_WRAP_NUM(mode.ewin->desktop)].x;
	yy -= desks.desk[DESKTOPS_WRAP_NUM(mode.ewin->desktop)].y;
	EReparentWindow(disp, s->win,
			desks.desk[DESKTOPS_WRAP_NUM(mode.ewin->desktop)].win,
			xx, yy);
     }
   else
      EReparentWindow(disp, s->win, root.win, xx, yy);
   switch (s->direction)
     {
     case 0:
	att.win_gravity = SouthEastGravity;
	XChangeWindowAttributes(disp, s->win, CWWinGravity, &att);
	att.win_gravity = NorthWestGravity;
	for (i = 0; i < s->num_buttons; i++)
	   XChangeWindowAttributes(disp, s->button[i]->win, CWWinGravity, &att);
	EMoveResizeWindow(disp, s->win, xx, yy, 1, 1);
	XSync(disp, False);
	EMapRaised(disp, s->win);
	SlideWindowSizeTo(s->win, xx + s->w, yy, xx, yy, 0, s->h, s->w, s->h,
			  mode.slidespeedmap);
	break;
     case 1:
	att.win_gravity = NorthWestGravity;
	XChangeWindowAttributes(disp, s->win, CWWinGravity, &att);
	att.win_gravity = SouthEastGravity;
	for (i = 0; i < s->num_buttons; i++)
	   XChangeWindowAttributes(disp, s->button[i]->win, CWWinGravity, &att);
	EMoveResizeWindow(disp, s->win, xx, yy, 1, 1);
	XSync(disp, False);
	EMapRaised(disp, s->win);
	SlideWindowSizeTo(s->win, xx, yy, xx, yy, 0, s->h, s->w, s->h,
			  mode.slidespeedmap);
	break;
     case 2:
	att.win_gravity = SouthEastGravity;
	XChangeWindowAttributes(disp, s->win, CWWinGravity, &att);
	att.win_gravity = NorthWestGravity;
	for (i = 0; i < s->num_buttons; i++)
	   XChangeWindowAttributes(disp, s->button[i]->win, CWWinGravity, &att);
	EMoveResizeWindow(disp, s->win, xx, yy, 1, 1);
	XSync(disp, False);
	EMapRaised(disp, s->win);
	SlideWindowSizeTo(s->win, xx, yy + s->h, xx, yy, s->w, 0, s->w, s->h,
			  mode.slidespeedmap);
	break;
     case 3:
	att.win_gravity = NorthWestGravity;
	XChangeWindowAttributes(disp, s->win, CWWinGravity, &att);
	att.win_gravity = SouthEastGravity;
	for (i = 0; i < s->num_buttons; i++)
	   XChangeWindowAttributes(disp, s->button[i]->win, CWWinGravity, &att);
	EMoveResizeWindow(disp, s->win, xx, yy, 1, 1);
	XSync(disp, False);
	EMapRaised(disp, s->win);
	SlideWindowSizeTo(s->win, xx, yy, xx, yy, s->w, 0, s->w, s->h,
			  mode.slidespeedmap);
	break;
     default:
	break;
     }
   s->from_win = win;
   mode.slideout = s;

   EDBUG_RETURN_;
}

void
HideSlideout(Slideout * s, Window w)
{
   EDBUG(5, "HideSlideout");

   if (!s)
      EDBUG_RETURN_;

   EUnmapWindow(disp, s->win);
   s->from_win = 0;
   s->ref_count--;
   mode.slideout = NULL;
   w = 0;

   EDBUG_RETURN_;
}

void
CalcSlideoutSize(Slideout * s)
{
   int                 i;
   int                 mx, my, x, y;

   EDBUG(5, "CalcSlideoutSize");

   if (!s)
      EDBUG_RETURN_;

   mx = 0;
   my = 0;
   x = 0;
   y = 0;
   for (i = 0; i < s->num_buttons; i++)
     {
	switch (s->direction)
	  {
	  case 2:
	  case 3:
	     if (s->button[i]->w > mx)
		mx = s->button[i]->w;
	     my += s->button[i]->h;
	     break;
	  case 0:
	  case 1:
	     if (s->button[i]->h > my)
		my = s->button[i]->h;
	     mx += s->button[i]->w;
	     break;
	  default:
	     break;
	  }
     }
   EResizeWindow(disp, s->win, mx, my);
   s->w = mx;
   s->h = my;

   for (i = 0; i < s->num_buttons; i++)
     {
	switch (s->direction)
	  {
	  case 2:
	     y += s->button[i]->h;
	     EMoveWindow(disp, s->button[i]->win,
			 (s->w - s->button[i]->w) >> 1, s->h - y);
	     break;
	  case 3:
	     EMoveWindow(disp, s->button[i]->win,
			 (s->w - s->button[i]->w) >> 1, y);
	     y += s->button[i]->h;
	     break;
	  case 0:
	     x += s->button[i]->w;
	     EMoveWindow(disp, s->button[i]->win, s->w - x,
			 (s->h - s->button[i]->h) >> 1);
	     break;
	  case 1:
	     EMoveWindow(disp, s->button[i]->win, x,
			 (s->h - s->button[i]->h) >> 1);
	     x += s->button[i]->w;
	     break;
	  default:
	     break;
	  }
     }
   PropagateShapes(s->win);

   EDBUG_RETURN_;
}

void
AddButtonToSlideout(Slideout * s, Button * b)
{
   EDBUG(5, "AddButtonToSlideout");

   if (!b)
      EDBUG_RETURN_;
   if (!s)
      EDBUG_RETURN_;

   s->num_buttons++;
   s->button = Erealloc(s->button, sizeof(Button) * s->num_buttons);
   s->button[s->num_buttons - 1] = b;
   EReparentWindow(disp, b->win, s->win, 0, 0);
   b->internal = 1;
   b->default_show = 0;
   b->flags |= FLAG_FIXED;
   b->used = 1;
   SimpleShowButton(b);
   CalcSlideoutSize(s);

   EDBUG_RETURN_;
}

void
RemoveButtonFromSlideout(Slideout * s, Button * b)
{
   EDBUG(5, "RemoveButtonFromSlideout");

   s = NULL;
   b = NULL;

   EDBUG_RETURN_;
}
