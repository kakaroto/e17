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
#define DECLARE_STRUCT_BUTTON
#include "E.h"
#include <time.h>
#include <sys/time.h>

void
ChangeNumberOfDesktops(int quantity)
{
   int                 pnum, i, num;
   EWin               *const *lst;

   pnum = Conf.desks.num;
   for (i = quantity; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
      LowerDesktop(i);
   Conf.desks.num = quantity;

   if (Conf.desks.num <= 0)
      Conf.desks.num = 1;
   else if (Conf.desks.num > ENLIGHTENMENT_CONF_NUM_DESKTOPS)
      Conf.desks.num = ENLIGHTENMENT_CONF_NUM_DESKTOPS;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (lst[i]->desktop >= Conf.desks.num)
	   MoveEwinToDesktop(lst[i], Conf.desks.num - 1);
     }

   if (Conf.desks.num > pnum)
     {
	for (i = pnum; i < Conf.desks.num; i++)
	   NewPagerForDesktop(i);
     }
   else if (Conf.desks.num < pnum)
     {
	for (i = Conf.desks.num; i < pnum; i++)
	   DisablePagersForDesktop(i);
     }
   if (desks.current >= Conf.desks.num)
      GotoDesktop(Conf.desks.num - 1);

   HintsSetDesktopConfig();
}

void
ShowDesktopControls(void)
{
   Button            **blst;
   int                 num, i;

   blst = (Button **) ListItemTypeID(&num, LIST_TYPE_BUTTON, 1);
   if ((blst) && (num > 0))
     {
	for (i = 0; i < num; i++)
	   ButtonShow(blst[i]);
	Efree(blst);
	StackDesktops();
     }
}

void
ShowDesktopTabs(void)
{
   Button            **blst;
   int                 num, i;

   blst = (Button **) ListItemTypeID(&num, LIST_TYPE_BUTTON, 2);
   if ((blst) && (num > 0))
     {
	for (i = 0; i < num; i++)
	   ButtonShow(blst[i]);
	Efree(blst);
	StackDesktops();
     }
}

void
HideDesktopTabs(void)
{
   Button            **blst;
   int                 num, i;

   blst = (Button **) ListItemTypeID(&num, LIST_TYPE_BUTTON, 2);
   if ((blst) && (num > 0))
     {
	for (i = 0; i < num; i++)
	   ButtonHide(blst[i]);
	Efree(blst);
	StackDesktops();
     }
}

void
ShowDesktopButtons(void)
{
   Button            **blst;
   int                 i, num;

   blst = (Button **) ListItemTypeID(&num, LIST_TYPE_BUTTON, 0);
   if (blst)
     {
	for (i = 0; i < num; i++)
	  {
	     if ((!blst[i]->internal) && (blst[i]->default_show))
		ButtonShow(blst[i]);
	  }
	Efree(blst);
     }
}

void
MoveToDeskTop(int num)
{
   int                 i, j;

   EDBUG(6, "MoveToDeskTop");
   j = -1;
   i = 0;
   while ((j < 0) && (i < ENLIGHTENMENT_CONF_NUM_DESKTOPS))
     {
	if (desks.order[i] == num)
	   j = i;
	i++;
     }
   if (j < 0)
      EDBUG_RETURN_;
   if (j > 0)
     {
	for (i = j - 1; i >= 0; i--)
	   desks.order[i + 1] = desks.order[i];
	desks.order[0] = num;
     }
   EDBUG_RETURN_;
}

void
MoveToDeskBottom(int num)
{
   int                 i, j;

   EDBUG(6, "MoveToDeskBottom");
   j = -1;
   i = 0;
   while ((j < 0) && (i < ENLIGHTENMENT_CONF_NUM_DESKTOPS))
     {
	if (desks.order[i] == num)
	   j = i;
	i++;
     }
   if (j < 0)
      EDBUG_RETURN_;
   if (j < ENLIGHTENMENT_CONF_NUM_DESKTOPS - 1)
     {
	for (i = j; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS - 1; i++)
	   desks.order[i] = desks.order[i + 1];
	desks.order[ENLIGHTENMENT_CONF_NUM_DESKTOPS - 1] = num;
     }
   EDBUG_RETURN_;
}

void
SlideWindowTo(Window win, int fx, int fy, int tx, int ty, int speed)
{
   int                 k, spd, x, y, min;
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
	EMoveWindow(disp, win, x, y);
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
   EMoveWindow(disp, win, tx, ty);
   UngrabX();
   EDBUG_RETURN_;
}

void
RefreshCurrentDesktop(void)
{
   EDBUG(5, "RefreshCurrentDesktop");
   RefreshDesktop(desks.current);
   EDBUG_RETURN_;
}

void
RefreshDesktop(int desk)
{
   Background         *bg;

   EDBUG(4, "RefreshDesktop");

   desk = desk % ENLIGHTENMENT_CONF_NUM_DESKTOPS;
   if (!desks.desk[desk].viewable)
      EDBUG_RETURN_;

   bg = desks.desk[desk].bg;
   if (!bg)
      EDBUG_RETURN_;

   BackgroundApply(bg, desks.desk[desk].win, 1);
   EDBUG_RETURN_;
}

void
DesktopsRefresh(void)
{
   int                 i;

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
     {
	if (desks.desk[i].bg)
	   DesktopSetBg(i, desks.desk[i].bg, 1);
     }
}

void
InitDesktopBgs(void)
{
   int                 i;
   Desk               *d;
   Atom                at;

   EDBUG(6, "InitDesktopBgs");
   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
     {
	d = &desks.desk[i];
	d->bg = NULL;
	desks.order[i] = i;
	d->tag = NULL;
	d->x = 0;
	d->y = 0;
	d->current_area_x = 0;
	d->current_area_y = 0;
	d->viewable = 0;
	if (i == 0)
	  {
	     d->win = root.win;
	  }
	else
	  {
	     d->win =
		ECreateWindow(root.win, -root.w, -root.h, root.w, root.h, 0);
	     XSelectInput(disp, d->win,
			  SubstructureNotifyMask | ButtonPressMask |
			  ButtonReleaseMask | EnterWindowMask | LeaveWindowMask
			  | ButtonMotionMask | PropertyChangeMask |
			  SubstructureRedirectMask | KeyPressMask |
			  KeyReleaseMask | PointerMotionMask);
	  }
	at = XInternAtom(disp, "ENLIGHTENMENT_DESKTOP", False);
	XChangeProperty(disp, d->win, at, XA_CARDINAL, 32, PropModeReplace,
			(unsigned char *)&i, 1);
/* I don't believe it.. this property causes xv and Xscreensaver to barf
 * stupid bloody clients - I cant' believe peope write such shitty code
 */
/*      
 * at = XInternAtom(disp, "__SWM_VROOT", False);
 * XChangeProperty(disp, d->win, at, XA_CARDINAL, 32, PropModeReplace,
 * (unsigned char *)&i, 1);
 */
     }

   EDBUG_RETURN_;
}

void
InitDesktopControls(void)
{
   int                 i;
   ActionClass        *ac, *ac2, *ac3;
   ImageClass         *ic, *ic2, *ic3, *ic4;
   Button             *b;
   Action             *a;
   int                 x[3], y[3], w[3], h[3], m, n, o;
   char                s[512], *param;

   EDBUG(6, "InitDesktopControls");

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
     {
	Esnprintf(s, sizeof(s), "DRAGBAR_DESKTOP_%i", i);
	ac = FindItem(s, 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
	if (!ac)
	  {
	     ac = CreateAclass(s);
	     AddItem(ac, ac->name, 0, LIST_TYPE_ACLASS);
	     a = CreateAction(EVENT_MOUSE_DOWN, 0, 0, 0, 1, 0, NULL, NULL);
	     AddAction(ac, a);
	     param = Emalloc(3);
	     Esnprintf(param, 3, "%i", i);
	     AddToAction(a, ACTION_DESKTOP_DRAG, param);
	     a = CreateAction(EVENT_MOUSE_DOWN, 0, 0, 0, 3, 0, NULL, NULL);
	     AddAction(ac, a);
	     Esnprintf(s, sizeof(s), "deskmenu");
	     AddToAction(a, ACTION_SHOW_MENU, Estrdup(s));
	     a = CreateAction(EVENT_MOUSE_DOWN, 0, 0, 0, 2, 0, NULL, NULL);
	     AddAction(ac, a);
	     Esnprintf(s, sizeof(s), "taskmenu");
	     AddToAction(a, ACTION_SHOW_MENU, Estrdup(s));
	     if (i > 0)
	       {
		  ac->tooltipstring =
		     Estrdup(_
			     ("Hold down the mouse button and drag\n"
			      "the mouse to be able to drag the desktop\n"
			      "back and forth.\n"
			      "Click right mouse button for a list of all\n"
			      "Desktops and their applications.\n"
			      "Click middle mouse button for a list of all\n"
			      "applications currently running.\n"));
	       }
	     else
	       {
		  ac->tooltipstring =
		     Estrdup(_
			     ("This is the Root desktop.\n"
			      "You cannot drag the root desktop around.\n"
			      "Click right mouse button for a list of all\n"
			      "Desktops and their applications.\n"
			      "Click middle mouse button for a list of all\n"
			      "applications currently running.\n"));
	       }
	  }
	Esnprintf(s, sizeof(s), "RAISEBUTTON_DESKTOP_%i", i);
	ac2 = FindItem(s, 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
	if (!ac2)
	  {
	     ac2 = CreateAclass(s);
	     AddItem(ac2, ac2->name, 0, LIST_TYPE_ACLASS);
	     a = CreateAction(EVENT_MOUSE_UP, 1, 0, 1, 0, 0, NULL, NULL);
	     AddAction(ac2, a);
	     param = Emalloc(3);
	     Esnprintf(param, 3, "%i", i);
	     AddToAction(a, ACTION_DESKTOP_RAISE, param);
	     ac2->tooltipstring =
		Estrdup(_
			("Click here to raise this desktop\n" "to the top.\n"));
	  }
	Esnprintf(s, sizeof(s), "LOWERBUTTON_DESKTOP_%i", i);
	ac3 = FindItem(s, 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
	if (!ac3)
	  {
	     ac3 = CreateAclass(s);
	     AddItem(ac3, ac3->name, 0, LIST_TYPE_ACLASS);
	     a = CreateAction(EVENT_MOUSE_UP, 1, 0, 1, 0, 0, NULL, NULL);
	     AddAction(ac3, a);
	     param = Emalloc(3);
	     Esnprintf(param, 3, "%i", i);
	     AddToAction(a, ACTION_DESKTOP_LOWER, param);
	     ac3->tooltipstring =
		Estrdup(_
			("Click here to lower this desktop\n"
			 "to the bottom.\n"));
	  }
	b = NULL;

	if (Conf.desks.dragdir < 2)
	  {
	     ic = FindItem("DESKTOP_DRAGBUTTON_VERT", 0, LIST_FINDBY_NAME,
			   LIST_TYPE_ICLASS);
	     ic2 =
		FindItem("DESKTOP_RAISEBUTTON_VERT", 0, LIST_FINDBY_NAME,
			 LIST_TYPE_ICLASS);
	     ic3 =
		FindItem("DESKTOP_LOWERBUTTON_VERT", 0, LIST_FINDBY_NAME,
			 LIST_TYPE_ICLASS);
	     ic4 =
		FindItem("DESKTOP_DESKRAY_VERT", 0, LIST_FINDBY_NAME,
			 LIST_TYPE_ICLASS);
	  }
	else
	  {
	     ic = FindItem("DESKTOP_DRAGBUTTON_HORIZ", 0, LIST_FINDBY_NAME,
			   LIST_TYPE_ICLASS);
	     ic2 =
		FindItem("DESKTOP_RAISEBUTTON_HORIZ", 0, LIST_FINDBY_NAME,
			 LIST_TYPE_ICLASS);
	     ic3 =
		FindItem("DESKTOP_LOWERBUTTON_HORIZ", 0, LIST_FINDBY_NAME,
			 LIST_TYPE_ICLASS);
	     ic4 =
		FindItem("DESKTOP_DESKRAY_HORIZ", 0, LIST_FINDBY_NAME,
			 LIST_TYPE_ICLASS);
	  }

	switch (Conf.desks.dragbar_ordering)
	  {
	  case 0:
	     m = 0;
	     n = 1;
	     o = 2;
	     break;
	  case 1:
	     m = 0;
	     n = 2;
	     o = 1;
	     break;
	  case 2:
	     m = 2;
	     n = 0;
	     o = 1;
	     break;
	  case 3:
	     m = 1;
	     n = 0;
	     o = 2;
	     break;
	  case 4:
	     m = 1;
	     n = 2;
	     o = 0;
	     break;
	  case 5:
	     m = 2;
	     n = 1;
	     o = 0;
	     break;
	  default:
	     m = 0;
	     n = 1;
	     o = 2;
	     break;
	  }

	switch (Conf.desks.dragdir)
	  {
	  case 0:
	     w[0] = w[1] = w[2] = h[0] = h[1] = Conf.desks.dragbar_width;
	     if (Conf.desks.dragbar_length == 0)
		h[2] = root.h - (Conf.desks.dragbar_width * 2);
	     else
		h[2] = Conf.desks.dragbar_length;
	     x[0] = x[1] = x[2] = 0;
	     y[m] = 0;
	     y[n] = y[m] + h[m];
	     y[o] = y[n] + h[n];
	     break;
	  case 1:
	     w[0] = w[1] = w[2] = h[0] = h[1] = Conf.desks.dragbar_width;
	     if (Conf.desks.dragbar_length == 0)
		h[2] = root.h - (Conf.desks.dragbar_width * 2);
	     else
		h[2] = Conf.desks.dragbar_length;
	     x[0] = x[1] = x[2] = root.w - Conf.desks.dragbar_width;
	     y[m] = 0;
	     y[n] = y[m] + h[m];
	     y[o] = y[n] + h[n];
	     break;
	  case 2:
	     h[0] = h[1] = h[2] = w[0] = w[1] = Conf.desks.dragbar_width;
	     if (Conf.desks.dragbar_length == 0)
		w[2] = root.w - (Conf.desks.dragbar_width * 2);
	     else
		w[2] = Conf.desks.dragbar_length;
	     y[0] = y[1] = y[2] = 0;
	     x[m] = 0;
	     x[n] = x[m] + w[m];
	     x[o] = x[n] + w[n];
	     break;
	  case 3:
	     h[0] = h[1] = h[2] = w[0] = w[1] = Conf.desks.dragbar_width;
	     if (Conf.desks.dragbar_length == 0)
		w[2] = root.w - (Conf.desks.dragbar_width * 2);
	     else
		w[2] = Conf.desks.dragbar_length;
	     y[0] = y[1] = y[2] = root.h - Conf.desks.dragbar_width;
	     x[m] = 0;
	     x[n] = x[m] + w[m];
	     x[o] = x[n] + w[n];
	     break;
	  default:
	     break;
	  }

	if (Conf.desks.dragbar_width > 0)
	  {
	     b = ButtonCreate("_DESKTOP_DRAG_CONTROL", ic2, ac2, NULL, NULL, -1,
			      FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[0], 0,
			      y[0], 0, 0, w[0], 0, h[0], 0, i, 0);
	     AddItem(b, b->name, 1, LIST_TYPE_BUTTON);
	     b = ButtonCreate("_DESKTOP_DRAG_CONTROL", ic3, ac3, NULL, NULL, -1,
			      FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[1], 0,
			      y[1], 0, 0, w[1], 0, h[1], 0, i, 0);
	     AddItem(b, b->name, 1, LIST_TYPE_BUTTON);
	     b = ButtonCreate("_DESKTOP_DRAG_CONTROL", ic, ac, NULL, NULL, -1,
			      FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[2], 0,
			      y[2], 0, 0, w[2], 0, h[2], 0, i, 0);
	     AddItem(b, b->name, 1, LIST_TYPE_BUTTON);
	  }
	if (i > 0)
	  {
	     if (Conf.desks.dragdir == 0)
	       {
		  b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", ic4, ac,
				   NULL, NULL, 1, FLAG_FIXED_VERT, 1, 99999, 1,
				   99999, 0, 0, desks.desk[i].x, 0,
				   desks.desk[i].y, 0, 0, 0, 0, 0, 1, 0, 1);
	       }
	     else if (Conf.desks.dragdir == 1)
	       {
		  b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", ic4, ac,
				   NULL, NULL, 1, FLAG_FIXED_VERT, 1, 99999, 1,
				   99999, 0, 0,
				   desks.desk[i].x + root.w -
				   Conf.desks.dragbar_width, 0, desks.desk[i].y,
				   0, 0, 0, 0, 0, 1, 0, 1);
	       }
	     else if (Conf.desks.dragdir == 2)
	       {
		  b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", ic4, ac,
				   NULL, NULL, 1, FLAG_FIXED_HORIZ, 1, 99999, 1,
				   99999, 0, 0, desks.desk[i].x, 0,
				   desks.desk[i].y, 0, 0, 0, 0, 0, 1, 0, 1);
	       }
	     else
	       {
		  b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", ic4, ac,
				   NULL, NULL, 1, FLAG_FIXED_HORIZ, 1, 99999, 1,
				   99999, 0, 0, desks.desk[i].x, 0,
				   desks.desk[i].y + root.h -
				   Conf.desks.dragbar_width, 0, 0, 0, 0, 0, 1,
				   0, 1);
	       }
	     AddItem(b, b->name, 2, LIST_TYPE_BUTTON);
	     desks.desk[i].tag = b;
	  }
	else
	   desks.desk[i].tag = NULL;
     }
   EDBUG_RETURN_;
}

void
DesktopSetBg(int desk, Background * bg, int refresh)
{
   EDBUG(5, "DesktopSetBg");

   if (desk < 0 || desk >= ENLIGHTENMENT_CONF_NUM_DESKTOPS)
      EDBUG_RETURN_;

   if (refresh)
      BackgroundPixmapFree(desks.desk[desk].bg);

   if (desks.desk[desk].bg != bg)
     {
	if (desks.desk[desk].bg)
	   BackgroundDecRefcount(desks.desk[desk].bg);
	if (bg)
	   BackgroundIncRefcount(bg);
     }

   desks.desk[desk].bg = bg;

   if (desks.desk[desk].viewable)
      RefreshDesktop(desk);

   if (desk == desks.current)
     {
	RedrawPagersForDesktop(desk, 2);
	ForceUpdatePagersForDesktop(desk);
     }
   else
      RedrawPagersForDesktop(desk, 1);

   EDBUG_RETURN_;
}

void
ConformEwinToDesktop(EWin * ewin)
{
   int                 xo, yo;

   EDBUG(3, "ConformEwinToDesktop");

   if ((ewin->iconified) && (ewin->parent != desks.desk[ewin->desktop].win))
     {
	ewin->parent = desks.desk[ewin->desktop].win;
	EwinListStackingRaise(ewin);
	EReparentWindow(disp, ewin->win, desks.desk[ewin->desktop].win, ewin->x,
			ewin->y);
	ICCCM_Configure(ewin);
	StackDesktops();
     }
   else if (ewin->floating)
     {
	xo = desks.desk[ewin->desktop].x;
	yo = desks.desk[ewin->desktop].y;
	if ((ewin->parent != root.win) && (ewin->floating == 2))
	  {
	     ewin->parent = root.win;
	     EReparentWindow(disp, ewin->win, root.win, ewin->x, ewin->y);
	     ewin->desktop = 0;
	  }
	XRaiseWindow(disp, ewin->win);
	ShowEdgeWindows();
	ICCCM_Configure(ewin);
     }
   else if (ewin->parent != desks.desk[ewin->desktop].win)
     {
	ewin->parent = desks.desk[ewin->desktop].win;
	EwinListStackingRaise(ewin);
	EReparentWindow(disp, ewin->win, desks.desk[ewin->desktop].win, ewin->x,
			ewin->y);
	StackDesktops();
	MoveEwin(ewin, ewin->x, ewin->y);
     }
   else
     {
	MoveEwin(ewin, ewin->x, ewin->y);
     }
   HintsSetWindowDesktop(ewin);
   EDBUG_RETURN_;
}

int
DesktopAt(int x, int y)
{
   int                 i;

   EDBUG(3, "DesktopAt");

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
     {
	if ((x >= desks.desk[desks.order[i]].x)
	    && (x < (desks.desk[desks.order[i]].x + root.w))
	    && (y >= desks.desk[desks.order[i]].y)
	    && (y < (desks.desk[desks.order[i]].y + root.h)))
	   EDBUG_RETURN(desks.order[i]);
     }

   EDBUG_RETURN(0);
}

static void
MoveStickyWindowsToCurrentDesk(void)
{
   EWin               *const *lst, *ewin, *last_ewin;
   int                 i, num;

   lst = EwinListGetStacking(&num);
   last_ewin = NULL;
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	if (!ewin->sticky)
	   continue;

	ewin->desktop = desks.current;
	ewin->parent = desks.desk[ewin->desktop].win;
	EReparentWindow(disp, ewin->win,
			desks.desk[ewin->desktop].win, root.w, root.h);
	EMoveWindow(disp, ewin->win, ewin->x, ewin->y);
	HintsSetWindowArea(ewin);
	HintsSetWindowDesktop(ewin);
	last_ewin = ewin;
     }
}

void
GotoDesktop(int desk)
{
   int                 x, y, pdesk;

   EDBUG(2, "GotoDesktop");

   if (Conf.desks.wraparound)
     {
	if (desk >= Conf.desks.num)
	   desk = 0;
	else if (desk < 0)
	   desk = Conf.desks.num - 1;
     }
   if (desk < 0 || desk >= Conf.desks.num || desk == desks.current)
      EDBUG_RETURN_;

   pdesk = desks.current;

   SlideoutsHide();

   {
      ToolTip           **lst;
      int                 i, j;

      lst = (ToolTip **) ListItemType(&j, LIST_TYPE_TOOLTIP);
      if (lst)
	{
	   for (i = 0; i < j; i++)
	     {
		HideToolTip(lst[i]);
	     }
	   Efree(lst);
	}
   }

   ActionsSuspend();

   FocusNewDeskBegin();

   if (Mode.mode == MODE_NONE)
      Mode.mode = MODE_DESKSWITCH;

   if (desk > 0)
     {
	if (Conf.desks.slidein)
	  {
	     if (!desks.desk[desk].viewable)
	       {
		  switch (Conf.desks.dragdir)
		    {
		    case 0:
		       MoveDesktop(desk, root.w, 0);
		       RaiseDesktop(desk);
		       SlideWindowTo(desks.desk[desk].win, root.w, 0, 0, 0,
				     Conf.desks.slidespeed);
		       break;
		    case 1:
		       MoveDesktop(desk, -root.w, 0);
		       RaiseDesktop(desk);
		       SlideWindowTo(desks.desk[desk].win, -root.w, 0, 0, 0,
				     Conf.desks.slidespeed);
		       break;
		    case 2:
		       MoveDesktop(desk, 0, root.h);
		       RaiseDesktop(desk);
		       SlideWindowTo(desks.desk[desk].win, 0, root.h, 0, 0,
				     Conf.desks.slidespeed);
		       break;
		    case 3:
		       MoveDesktop(desk, 0, -root.h);
		       RaiseDesktop(desk);
		       SlideWindowTo(desks.desk[desk].win, 0, -root.h, 0, 0,
				     Conf.desks.slidespeed);
		       break;
		    default:
		       break;
		    }
	       }
	     else
	       {
		  GetWinXY(desks.desk[desk].win, &x, &y);
		  SlideWindowTo(desks.desk[desk].win, desks.desk[desk].x,
				desks.desk[desk].y, 0, 0,
				Conf.desks.slidespeed);
		  RaiseDesktop(desk);
	       }
	     StackDesktops();
	  }
	else
	  {
	     RaiseDesktop(desk);
	     StackDesktops();
	  }
	MoveDesktop(desk, 0, 0);
     }
   else
     {
	RaiseDesktop(desk);
     }

   ActionsResume();
   FocusNewDesk();

   if (Mode.mode == MODE_DESKSWITCH)
      Mode.mode = MODE_NONE;

   RedrawPagersForDesktop(pdesk, 0);
   RedrawPagersForDesktop(desk, 3);
   ForceUpdatePagersForDesktop(desk);
   HandleDrawQueue();

   EDBUG_RETURN_;
}

void
MoveDesktop(int desk, int x, int y)
{
   int                 i;
   EWin               *const *lst;
   int                 n, v, dx, dy;

   EDBUG(3, "MoveDesktop");
   if (desk < 0)
      EDBUG_RETURN_;
   if (desk >= Conf.desks.num)
      EDBUG_RETURN_;
   if (desk == 0)
      EDBUG_RETURN_;
   dx = x - desks.desk[desk].x;
   dy = y - desks.desk[desk].y;
   if ((x == 0) && (y == 0))
     {
	n = -1;
	i = 0;
	while ((n < 0) && (i < ENLIGHTENMENT_CONF_NUM_DESKTOPS))
	  {
	     if (desks.order[i] == desk)
		n = i;
	     i++;
	  }
	if (n >= 0)
	  {
	     for (i = n + 1; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
	       {
		  if (desks.desk[desks.order[i]].viewable)
		     BackgroundTouch(desks.desk[desks.order[i]].bg);
		  desks.desk[desks.order[i]].viewable = 0;
	       }
	  }
     }
   else
     {
	n = -1;
	i = 0;

	while ((n < 0) && (i < ENLIGHTENMENT_CONF_NUM_DESKTOPS))
	  {
	     if (desks.order[i] == desk)
		n = i;
	     i++;
	  }

	if (n >= 0)
	  {
	     if (desks.desk[desks.order[n]].viewable)
	       {
		  v = 1;
	       }
	     else
	       {
		  v = 0;
	       }

	     for (i = n + 1; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
	       {

		  if ((!desks.desk[desks.order[i]].viewable) && (v))
		    {
		       desks.desk[desks.order[i]].viewable = v;
		       RefreshDesktop(desks.order[i]);
		    }
		  else
		    {
		       if ((!v) && (desks.desk[desks.order[i]].viewable))
			  BackgroundTouch(desks.desk[desks.order[i]].bg);
		       desks.desk[desks.order[i]].viewable = v;
		    }

		  if ((desks.desk[desks.order[i]].x == 0)
		      && (desks.desk[desks.order[i]].y == 0))
		    {
		       v = 0;
		    }
	       }
	  }
     }

   EMoveWindow(disp, desks.desk[desk].win, x, y);

   if (desks.desk[desk].tag)
      ButtonMoveRelative(desks.desk[desk].tag, dx, dy);

   desks.desk[desk].x = x;
   desks.desk[desk].y = y;

   lst = EwinListGetAll(&n);
   for (i = 0; i < n; i++)
      if (lst[i]->desktop == desk)
	 ICCCM_Configure(lst[i]);

   EDBUG_RETURN_;
}

void
RaiseDesktop(int desk)
{
   int                 i;

   EDBUG(3, "RaiseDesktop");

   if ((desk < 0) || (desk >= Conf.desks.num))
      EDBUG_RETURN_;

   FocusNewDeskBegin();
   CloneDesktop(desks.order[0]);
   desks.desk[desk].viewable = 1;
   RefreshDesktop(desk);
   MoveToDeskTop(desk);

   if (desk == 0)
     {
	for (i = ENLIGHTENMENT_CONF_NUM_DESKTOPS - 1; i > 0; i--)
	  {
	     HideDesktop(desks.order[i]);
	  }
     }
   StackDesktops();
   desks.current = desk;
   MoveStickyWindowsToCurrentDesk();
   StackDesktop(desks.current);
   FocusNewDesk();
   FX_DeskChange();
   RemoveClones();
   RedrawPagersForDesktop(desk, 3);
   ForceUpdatePagersForDesktop(desk);
   UpdatePagerSel();
   HandleDrawQueue();
   HintsSetCurrentDesktop();
   EMapWindow(disp, desks.desk[desk].win);
   XSync(disp, False);

   EDBUG_RETURN_;
}

void
LowerDesktop(int desk)
{
   EDBUG(3, "LowerDesktop");

   if ((desk <= 0) || (desk >= Conf.desks.num))
      EDBUG_RETURN_;

   FocusNewDeskBegin();
   CloneDesktop(desk);
   MoveToDeskBottom(desk);
   UncoverDesktop(desks.order[0]);
   HideDesktop(desk);
   StackDesktops();
   desks.current = desks.order[0];
   MoveStickyWindowsToCurrentDesk();
   StackDesktop(desks.current);
   FocusNewDesk();
   FX_DeskChange();
   RemoveClones();
   RedrawPagersForDesktop(desks.order[0], 3);
   ForceUpdatePagersForDesktop(desks.order[0]);
   UpdatePagerSel();
   HandleDrawQueue();
   HintsSetCurrentDesktop();
   XSync(disp, False);

   EDBUG_RETURN_;
}

void
HideDesktop(int desk)
{
   EDBUG(3, "HideDesktop");

   if ((desk < 0) || (desk >= Conf.desks.num))
      EDBUG_RETURN_;
   if (desk == 0)
      EDBUG_RETURN_;

   if (desks.desk[desk].viewable)
      BackgroundTouch(desks.desk[desk].bg);
   desks.desk[desk].viewable = 0;
   EMoveWindow(disp, desks.desk[desk].win, root.w, 0);

   EDBUG_RETURN_;
}

void
ShowDesktop(int desk)
{
   int                 i;

   EDBUG(3, "ShowDesktop");

   if (desk < 0)
      EDBUG_RETURN_;
   if (desk >= Conf.desks.num)
      EDBUG_RETURN_;

   desks.desk[desk].viewable = 1;
   RefreshDesktop(desk);
   MoveToDeskTop(desk);

   if (desk == 0)
     {
	for (i = ENLIGHTENMENT_CONF_NUM_DESKTOPS - 1; i > 0; i--)
	   HideDesktop(desks.order[i]);
     }
   else
     {
	StackDesktops();
	EMapWindow(disp, desks.desk[desk].win);
     }

   EDBUG_RETURN_;
}

void
StackDesktops(void)
{
   EDBUG(2, "StackDesktops");

   StackDesktop(0);

   EDBUG_RETURN_;
}

#define _APPEND_TO_WIN_LIST(win) \
  { \
     wl = Erealloc(wl, ++tot * sizeof(Window)); \
     wl[tot - 1] = win; \
  }
void
StackDesktop(int desk)
{
   Window             *wl, *wl2;
   int                 i, wnum, tot, bnum;
   EWin               *const *lst, *ewin;
   Button            **blst;

   EDBUG(2, "StackDesktop");
   tot = 0;
   wl = NULL;

   /*
    * Build the window stack, top to bottom
    */

   wl2 = ListProgressWindows(&wnum);
   if (wl2)
     {
	for (i = 0; i < wnum; i++)
	   _APPEND_TO_WIN_LIST(wl2[i]);
	Efree(wl2);
     }
   if (init_win_ext)
     {
	_APPEND_TO_WIN_LIST(init_win_ext);
     }
   if (init_win1)
     {
	_APPEND_TO_WIN_LIST(init_win1);
	_APPEND_TO_WIN_LIST(init_win2);
     }

   lst = EwinListGetStacking(&wnum);
   blst = (Button **) ListItemType(&bnum, LIST_TYPE_BUTTON);

   /* Sticky buttons */
   if (blst)
     {
	for (i = 0; i < bnum; i++)
	  {
	     if (!blst[i]->sticky || blst[i]->internal)
		continue;

	     _APPEND_TO_WIN_LIST(blst[i]->win);
	  }
     }

   /* Floating EWins */
   if (lst)
     {
	for (i = 0; i < wnum; i++)
	  {
	     if (!lst[i]->floating)
		continue;

	     _APPEND_TO_WIN_LIST(lst[i]->win);
	  }
     }

   /* The virtual desktop windows */
   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
     {
	if (desks.order[i] == 0)
	   break;

	_APPEND_TO_WIN_LIST(desks.desk[desks.order[i]].win);
     }

   /* Non-sticky, "above" buttons */
   if (blst)
     {
	for (i = 0; i < bnum; i++)
	  {
	     if (blst[i]->desktop != desk || blst[i]->ontop != 1 ||
		 blst[i]->sticky || blst[i]->internal)
		continue;

	     _APPEND_TO_WIN_LIST(blst[i]->win);
	  }
     }

   /* Normal EWins on this desk */
   for (i = 0; i < wnum; i++)
     {
	ewin = lst[i];
	if (EwinGetDesk(ewin) != desk || ewin->floating)
	   continue;

	_APPEND_TO_WIN_LIST(ewin->win);
	if (ewin->win == Mode.menu_win_covered)
	   _APPEND_TO_WIN_LIST(Mode.menu_cover_win);
     }

   /* Non-sticky, "below" buttons */
   if (blst)
     {
	for (i = 0; i < bnum; i++)
	  {
	     if (blst[i]->desktop != desk || blst[i]->ontop != -1 ||
		 blst[i]->sticky || blst[i]->internal)
		continue;

	     _APPEND_TO_WIN_LIST(blst[i]->win);
	  }
     }

   /* The current (virtual) root window */
   _APPEND_TO_WIN_LIST(desks.desk[desk].win);

   if (EventDebug(EDBUG_TYPE_STACKING))
     {
	Eprintf("StackDesktop %d:\n", desk);
	for (i = 0; i < tot; i++)
	   Eprintf(" win=%#10lx parent=%#10lx\n", wl[i], GetWinParent(wl[i]));
     }
   XRestackWindows(disp, wl, tot);
   ShowEdgeWindows();
   RaiseProgressbars();
   HintsSetClientList();

   if (wl)
      Efree(wl);
   if (blst)
      Efree(blst);

   EDBUG_RETURN_;
}

void
UncoverDesktop(int desk)
{
   EDBUG(3, "UncoverDesktop");
   if (desk < 0)
      EDBUG_RETURN_;
   if (desk >= Conf.desks.num)
      EDBUG_RETURN_;
   desks.desk[desk].viewable = 1;
   RefreshDesktop(desk);
   if (desk != 0)
      EMapWindow(disp, desks.desk[desk].win);
   EDBUG_RETURN_;
}

void
MoveEwinToDesktop(EWin * ewin, int desk)
{
   MoveEwinToDesktopAt(ewin, desk, ewin->x, ewin->y);
}

void
MoveEwinToDesktopAt(EWin * ewin, int desk, int x, int y)
{
   int                 pdesk;
   int                 dx, dy;

   EDBUG(3, "MoveEwinToDesktopAt");

   ewin->floating = 0;

   pdesk = ewin->desktop;
   desk = DESKTOPS_WRAP_NUM(desk);
   if (desk != pdesk && !ewin->sticky)
     {
	ForceUpdatePagersForDesktop(pdesk);
	ewin->desktop = DESKTOPS_WRAP_NUM(desk);
     }

   dx = x - ewin->x;
   dy = y - ewin->y;
   ewin->x = x;
   ewin->y = y;
   ConformEwinToDesktop(ewin);

   if (ewin->has_transients)
     {
	EWin              **lst;
	int                 i, nn;

	lst = ListTransientsFor(ewin->client.win, &nn);
	if (lst)
	  {
	     for (i = 0; i < nn; i++)
	       {
		  MoveEwinToDesktopAt(lst[i], desk, lst[i]->x + dx,
				      lst[i]->y + dy);
	       }
	     Efree(lst);
	  }
     }

   ForceUpdatePagersForDesktop(desk);

   EDBUG_RETURN_;
}

void
GotoDesktopByEwin(EWin * ewin)
{
   if (!ewin->sticky)
     {
	GotoDesktop(ewin->desktop);
	SetCurrentArea(ewin->area_x, ewin->area_y);
     }
}

#if 0				/* Unused */
void
FloatEwinAboveDesktops(EWin * ewin)
{
   int                 xo, yo;

   EDBUG(2, "FloatEwinAboveDesktops");
   xo = desks.desk[ewin->desktop].x;
   yo = desks.desk[ewin->desktop].y;
   ewin->desktop = 0;
   ewin->floating = 1;
   ConformEwinToDesktop(ewin);
   if (ewin->has_transients)
     {
	EWin              **lst;
	int                 i, num;

	lst = ListTransientsFor(ewin->client.win, &num);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
		FloatEwinAboveDesktops(lst[i]);
	     Efree(lst);
	  }
     }
   EDBUG_RETURN_;
}
#endif

void
DesktopsEventsConfigure(int mode)
{
   int                 i;
   long                event_mask;

   if (mode)
      event_mask =
	 PropertyChangeMask | SubstructureRedirectMask |
	 ButtonPressMask | ButtonReleaseMask;
   else
      event_mask =
	 SubstructureNotifyMask | ButtonPressMask |
	 ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
	 ButtonMotionMask | PropertyChangeMask |
	 SubstructureRedirectMask | KeyPressMask | KeyReleaseMask
	 | PointerMotionMask;

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
      XSelectInput(disp, desks.desk[i].win, event_mask);
}
