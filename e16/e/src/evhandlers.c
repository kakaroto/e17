/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 * *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 * *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

static ToolTip     *ttip = NULL;

static void         ToolTipTimeout(int val, void *data);

static char         sentpress = 0;

static void
ToolTipTimeout(int val, void *data)
{
   int                 x, y, dum;

   unsigned int        mask;
   Window              win, rt, ch;
   ActionClass        *ac;

   EDBUG(5, "ToolTipTimeout");

   /* In the case of multiple screens, check to make sure
    * the root window is still where the mouse is... */
   if (False ==
       XQueryPointer(disp, root.win, &rt, &ch, &x, &y, &dum, &dum, &mask))
      EDBUG_RETURN_;
   /* dont pop up tooltip is mouse button down */
   if (mask &
       (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask))
      EDBUG_RETURN_;
   win = WindowAtXY(x, y);
   ac = FindActionClass(win);
   if (!ac)
      EDBUG_RETURN_;

   if (!ttip)
      ttip = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);

   if (ac->tooltipstring)
     {
	if (Conf.tooltips.showroottooltip)
	  {
	     ShowToolTip(ttip, ac->tooltipstring, ac, x, y);
	  }
	else
	  {
	     int                 i;
	     int                 show = 1;

	     for (i = 0; i < Conf.desks.num; i++)
	       {
		  if (win == desks.desk[i].win)
		     show = 0;
	       }
	     if (show)
		ShowToolTip(ttip, ac->tooltipstring, ac, x, y);
	  }
     }
   data = NULL;
   val = 0;

   EDBUG_RETURN_;
}

static void
TooltipsHandleEvent(void)
{
   if (ttip)
      HideToolTip(ttip);
   RemoveTimerEvent("TOOLTIP_TIMEOUT");
   if (Conf.tooltips.enable)
      DoIn("TOOLTIP_TIMEOUT", Conf.tooltips.delay, ToolTipTimeout, 0, NULL);
}

static void
ButtonProxySendEvent(XEvent * ev)
{
   if (Mode.button_proxy_win)
      XSendEvent(disp, Mode.button_proxy_win, False, SubstructureNotifyMask,
		 ev);
}

void
HandleMouseDown(XEvent * ev)
{
   Window              win = ev->xbutton.window;
   EWin               *ewin;
   int                 i;
   int                 desk_click;
   char                double_click = 0;
   float               mode_double_click_time = 0.25;

   EDBUG(5, "HandleMouseDown");

   /* DON'T handle clicks whilst moving/resizing things unless doing manual placement */
   if (Mode.mode != MODE_NONE)
      EDBUG_RETURN_;

   if ((Mode.cur_menu_mode) && (!clickmenu))
     {
	unsigned int        bmask = 0, evmask;

	evmask =
	   ev->xbutton.state & (Button1Mask | Button2Mask | Button3Mask |
				Button4Mask | Button5Mask);
	if (ev->xbutton.button == 1)
	   bmask = Button1Mask;
	else if (ev->xbutton.button == 2)
	   bmask = Button2Mask;
	else if (ev->xbutton.button == 3)
	   bmask = Button3Mask;
	else if (ev->xbutton.button == 4)
	   bmask = Button4Mask;
	else if (ev->xbutton.button == 5)
	   bmask = Button5Mask;
	if (bmask != evmask)
	   clickmenu = 1;
	else
	  {
	     EDBUG_RETURN_;
	  }
     }

   TooltipsHandleEvent();

   if ((((float)(ev->xbutton.time - Mode.last_time) / 1000) <
	mode_double_click_time) &&
       ((int)(ev->xbutton.button) == (int)(Mode.last_button)))
      double_click = 1;

   Mode.last_time = ev->xbutton.time;
   Mode.last_button = ev->xbutton.button;
   Mode.last_bpress = win;

   Mode.x = ev->xbutton.x_root;
   Mode.y = ev->xbutton.y_root;

   desk_click = -1;
   for (i = 0; i < Conf.desks.num; i++)
     {
	if (win == desks.desk[i].win)
	  {
	     desk_click = i;
	     break;
	  }
     }
   if (desk_click >= 0)
     {
	ActionClass        *ac;

	XUngrabPointer(disp, CurrentTime);

	ac = FindItem("DESKBINDINGS", 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
	if (ac)
	  {
	     if (!EventAclass(ev, NULL, ac))
		ButtonProxySendEvent(ev);
	  }
	EDBUG_RETURN_;
     }

   if (MenusEventMouseDown(ev))
      goto done;

   FocusHandleClick(win);

   if (double_click)
      ev->xbutton.time = 0;

   if ( /*!clickmenu && */ BordersEventMouseDown(ev))
      goto done;

   if (ButtonsEventMouseDown(ev))
      goto done;

   if (DialogEventMouseDown(ev))
      goto done;

   ewin = FindEwinByBase(win);
   if (ewin)
     {
	ActionClass        *ac;

	ac = (ActionClass *) FindItem("BUTTONBINDINGS", 0, LIST_FINDBY_NAME,
				      LIST_TYPE_ACLASS);
	if (ac)
	  {
	     GrabThePointer(ewin->win);
	     if (EventAclass(ev, ewin, ac))
	       {
		  goto done;
	       }
	  }
     }

   if (PagersEventMouseDown(ev))
      goto done;

 done:
   EDBUG_RETURN_;
}

void
HandleMouseUp(XEvent * ev)
{
   Window              win = ev->xbutton.window;
   EWin               *ewin;
   Slideout           *pslideout;

   EDBUG(5, "HandleMouseUp");

   /* DON'T handle clicks whilst moving/resizing things */
   if ((Mode.mode != MODE_NONE) &&
       (!((Mode.place) &&
	  (Mode.mode == MODE_MOVE_PENDING || Mode.mode == MODE_MOVE))))
     {
	if ((int)Mode.last_button != (int)ev->xbutton.button)
	   EDBUG_RETURN_;
     }

   TooltipsHandleEvent();
   UnGrabTheButtons();

   Mode.x = ev->xbutton.x_root;
   Mode.y = ev->xbutton.y_root;

   pslideout = Mode.slideout;

   ActionsEnd(NULL);

   if ((Mode.last_bpress) && (Mode.last_bpress != win))
     {
	ev->xbutton.window = Mode.last_bpress;
	BordersEventMouseOut2(ev);
	ev->xbutton.window = win;
     }

   if (Mode.place)
     {
	Mode.place = 0;
	goto done;
     }

   if (sentpress)
     {
	/* We never get here? */
	sentpress = 0;
	ButtonProxySendEvent(ev);
     }

   Mode.context_win = Mode.last_bpress;

   if ((((float)(ev->xbutton.time - Mode.last_time) / 1000) < 0.5)
       && (Mode.cur_menu_depth > 0) && (!clickmenu))
     {
	clickmenu = 1;
	Mode.justclicked = 1;
     }

   if ( /*!clickmenu && */ BordersEventMouseUp(ev))
      goto done;

   if (Mode.action_inhibit)
      goto done;

   if (MenusEventMouseUp(ev))
      goto done;

   if (ButtonsEventMouseUp(ev))
      goto done;

   if (DialogEventMouseUp(ev))
      goto done;

   ewin = FindEwinByBase(win);
   if (ewin)
     {
	ActionClass        *ac;

	ac = (ActionClass *) FindItem("BUTTONBINDINGS", 0, LIST_FINDBY_NAME,
				      LIST_TYPE_ACLASS);
	if (ac)
	  {
	     if (EventAclass(ev, ewin, ac))
	       {
		  goto done;
	       }
	  }
     }

   if (PagersEventMouseUp(ev))
      goto done;

 done:
   if ((Mode.slideout) && (pslideout))
      SlideoutHide(Mode.slideout);

   Mode.action_inhibit = 0;
   Mode.justclicked = 0;
   Mode.last_bpress = 0;

   EDBUG_RETURN_;
}

void
HandleMotion(XEvent * ev)
{
   EDBUG(5, "HandleMotion");

   TooltipsHandleEvent();
   EdgeHandleMotion(ev);
   Mode.px = Mode.x;
   Mode.py = Mode.y;
   Mode.x = ev->xmotion.x_root;
   Mode.y = ev->xmotion.y_root;
   desks.current = DesktopAt(Mode.x, Mode.y);

   if ((!(ev->xmotion.state
	  & (Button1Mask | Button2Mask | Button3Mask | Button4Mask |
	     Button5Mask)) && (!Mode.place)))
     {
	if (ActionsEnd(NULL))
	   EDBUG_RETURN_;
     }

   ActionsHandleMotion();

#define SCROLL_RATIO 2/3
   if (((Mode.cur_menu_mode) || (clickmenu)) && (Mode.cur_menu_depth > 0))
     {
	int                 i, offx = 0, offy = 0, xdist = 0, ydist = 0;
	EWin               *ewin;
	EWin               *menus[256];
	int                 fx[256];
	int                 fy[256];
	int                 tx[256];
	int                 ty[256];
	static int          menu_scroll_dist = 4;
	int                 my_width, my_height, x_org, y_org, head_num = 0;

	head_num = ScreenGetGeometry(Mode.x, Mode.y, &x_org, &y_org,
				     &my_width, &my_height);

	if (Mode.x > ((x_org + my_width) - (menu_scroll_dist + 1)))
	  {
	     xdist = -(menu_scroll_dist + (Mode.x - (x_org + my_width)));
	  }
	else if (Mode.x < (menu_scroll_dist + x_org))
	  {
	     xdist = x_org + menu_scroll_dist - (Mode.x);
	  }

	if (Mode.y > (root.h - (menu_scroll_dist + 1)))
	  {
	     ydist = -(menu_scroll_dist + (Mode.y - (y_org + my_height)));
	  }
	else if (Mode.y < (menu_scroll_dist + y_org))
	  {
	     ydist = y_org + menu_scroll_dist - (Mode.y);
	  }

	/* That's a hack to avoid unwanted events:
	 * If the user entered the border area, he has to
	 * leave it first, before he can scroll menus again ...
	 */
	if ((xdist != 0) || (ydist != 0) || Mode.doingslide)
	  {
	     /* -10 has no meaning, only makes sure that the if's */
	     /* above can't be fulfilled ... */
	     menu_scroll_dist = -10;
	  }
	else
	  {
	     menu_scroll_dist = 13;
	  }

	if (Mode.cur_menu_depth > 0)
	  {
	     int                 x1, y1, x2, y2;

	     x1 = x_org + my_width;
	     x2 = x_org - 1;
	     y1 = y_org + my_height;
	     y2 = y_org - 1;
	     /* work out the minimum and maximum extents of our */
	     /* currently active menus */
	     for (i = 0; i < Mode.cur_menu_depth; i++)
	       {
		  if (Mode.cur_menu[i])
		    {
		       ewin = FindEwinByMenu(Mode.cur_menu[i]);
		       if (ewin)
			 {
			    if (ewin->x < x1)
			       x1 = ewin->x;
			    if (ewin->y < y1)
			       y1 = ewin->y;
			    if ((ewin->x + ewin->w - 1) > x2)
			       x2 = ewin->x + ewin->w - 1;
			    if ((ewin->y + ewin->h - 1) > y2)
			       y2 = ewin->y + ewin->h - 1;
			 }
		    }
	       }

	     if (xdist < 0)
	       {
		  offx = (x_org + my_width) - x2;
	       }
	     else if (xdist > 0)
	       {
		  offx = x_org - x1;
	       }
	     if (ydist < 0)
	       {
		  offy = (y_org + my_height) - y2;
	       }
	     else if (ydist > 0)
	       {
		  offy = y_org - y1;
	       }

	     if ((xdist < 0) && (offx <= 0))
		xdist = offx;
	     if ((xdist > 0) && (offx >= 0))
		xdist = offx;
	     if ((ydist < 0) && (offy <= 0))
		ydist = offy;
	     if ((ydist > 0) && (offy >= 0))
		ydist = offy;

	     /* only if any active menus are partially off screen then scroll */
	     if ((((xdist > 0) && (x1 < x_org))
		  || ((xdist < 0) && (x2 >= (x_org + my_width))))
		 || (((ydist > 0) && (y1 < y_org))
		     || ((ydist < 0) && (y2 >= (y_org + my_height)))))
	       {
		  /* If we would scroll too far, limit scrolling to 2/3s of screen */
		  if (ydist < -my_width)
		     ydist = -my_width * SCROLL_RATIO;
		  if (ydist > my_width)
		     ydist = my_width * SCROLL_RATIO;

		  if (xdist < -my_height)
		     xdist = -my_height * SCROLL_RATIO;
		  if (xdist > my_height)
		     xdist = my_height * SCROLL_RATIO;

		  if (Mode.cur_menu_depth)
		    {
#ifdef HAS_XINERAMA
		       ewin = FindEwinByMenu(Mode.cur_menu[0]);
		       if (ewin->head == head_num)
			 {
#endif
			    for (i = 0; i < Mode.cur_menu_depth; i++)
			      {
				 menus[i] = NULL;
				 if (Mode.cur_menu[i])
				   {
				      ewin = FindEwinByMenu(Mode.cur_menu[i]);
				      if (ewin)
					{
					   menus[i] = ewin;
					   fx[i] = ewin->x;
					   fy[i] = ewin->y;
					   tx[i] = ewin->x + xdist;
					   ty[i] = ewin->y + ydist;
					}
				   }
			      }
			    SlideEwinsTo(menus, fx, fy, tx, ty,
					 Mode.cur_menu_depth, Conf.shadespeed);
			    if (((xdist != 0) || (ydist != 0))
				&& (Conf.warpmenus))
			       XWarpPointer(disp, None, None, 0, 0, 0, 0, xdist,
					    ydist);
#ifdef HAS_XINERAMA
			 }
#endif
		    }
	       }
	  }
     }

   PagersEventMotion(ev);

   DialogEventMotion(ev);

   EDBUG_RETURN_;
}

void
HandleMouseIn(XEvent * ev)
{
   Window              win = ev->xcrossing.window;

   EDBUG(5, "HandleMouseIn");

   if (Mode.mode != MODE_NONE)
      EDBUG_RETURN_;

   if (win == root.win)
      goto done;

   Mode.context_win = win;

   TooltipsHandleEvent();
   EdgeHandleEnter(ev);

   if (PagersEventMouseIn(ev))
      goto done;

   if (MenusEventMouseIn(ev))
      goto done;

   if ( /*!clickmenu && */ BordersEventMouseIn(ev))
      goto done;

   if (ButtonsEventMouseIn(ev))
      goto done;

   if (DialogEventMouseIn(ev))
      goto done;

 done:
   FocusHandleEnter(ev);

   EDBUG_RETURN_;
}

void
HandleMouseOut(XEvent * ev)
{
   Window              win = ev->xcrossing.window;

   EDBUG(5, "HandleMouseOut");

   if (Mode.mode != MODE_NONE)
      EDBUG_RETURN_;

   TooltipsHandleEvent();
   EdgeHandleLeave(ev);

   Mode.context_win = win;

   if (PagersEventMouseOut(ev))
      goto done;

   if (MenusEventMouseOut(ev))
      goto done;

   if ( /*!clickmenu && */ BordersEventMouseOut(ev))
      goto done;

   if (ButtonsEventMouseOut(ev))
      goto done;

   if (DialogEventMouseOut(ev))
      goto done;

 done:
   FocusHandleLeave(ev);

   EDBUG_RETURN_;
}

void
HandleFocusIn(XEvent * ev __UNUSED__)
{
#if 0
   Window              win = ev->xfocus.window;
   EWin               *ewin;

   EDBUG(5, "HandleFocusIn");

   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin && !ewin->active)
     {
	ewin->active = 1;
	DrawEwin(ewin);

	FocusEwinSetGrabs(ewin);
     }

   EDBUG_RETURN_;
#endif
}

void
HandleFocusOut(XEvent * ev __UNUSED__)
{
#if 0
   Window              win = ev->xfocus.window;
   EWin               *ewin;

   EDBUG(5, "HandleFocusOut");

   /* Do nothing if the focus is passed down to child */
   if (ev->xfocus.detail == NotifyInferior)
      goto done;

   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin && ewin->active)
     {
	ewin->active = 0;
	DrawEwin(ewin);

	FocusEwinSetGrabs(ewin);
     }

 done:
   EDBUG_RETURN_;
#endif
}

void
HandleExpose(XEvent * ev)
{
   EDBUG(5, "HandleExpose");

   if (BordersEventExpose(ev))
      goto done;

   if (ButtonsEventExpose(ev))
      goto done;

   if (DialogEventExpose(ev))
      goto done;

 done:
   EDBUG_RETURN_;
}

void
HandleDestroy(XEvent * ev)
{
   Window              win = ev->xdestroywindow.window;
   EWin               *ewin;
   Client             *c;

   EDBUG(5, "HandleDestroy");

   EForgetWindow(disp, win);

   ewin = RemoveItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	FreeEwin(ewin);
	EDBUG_RETURN_;
     }

   c = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_CLIENT);
   if (c)
      DeleteClient(c);

   EDBUG_RETURN_;
}

void
HandleUnmap(XEvent * ev)
{
   Window              win = ev->xunmap.window;
   EWin               *ewin;

   EDBUG(5, "HandleUnmap");

   if (win == Mode.context_win)
      Mode.context_win = 0;

   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	ewin->mapped = 0;

	if (ewin->pager)
	   PagerEventUnmap(ewin->pager);

	if (Conf.dockapp_support && ewin->docked)
	   DockDestroy(ewin);

	ActionsEnd(ewin);
	if (ewin == GetContextEwin())
	   SlideoutsHide();

	if (ewin == Mode.focuswin)
	   FocusToEWin(ewin, FOCUS_EWIN_GONE);
	if (ewin == Mode.mouse_over_win)
	   Mode.mouse_over_win = NULL;

	if (!ewin->iconified)
	  {
	     XTranslateCoordinates(disp, ewin->client.win, root.win,
				   -ewin->border->border.left,
				   -ewin->border->border.top, &ewin->client.x,
				   &ewin->client.y, &win);
	     EReparentWindow(disp, ewin->client.win, root.win, ewin->client.x,
			     ewin->client.y);
	     ICCCM_Withdraw(ewin);
	     RemoveItem(NULL, ewin->client.win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	     FreeEwin(ewin);
	  }
	else
	  {
	     HideEwin(ewin);
	  }
     }
   EDBUG_RETURN_;
}

void
HandleMap(XEvent * ev)
{
   Window              win = ev->xunmap.window;
   EWin               *ewin;

   EDBUG(5, "HandleMap");
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	ewin->mapped = 1;
     }
   EDBUG_RETURN_;
}

void
HandleMapRequest(XEvent * ev)
{
   Window              win;
   EWin               *ewin;

   EDBUG(5, "HandleMapRequest");

   win = ev->xconfigurerequest.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin && ewin->iconified)
     {
	DeIconifyEwin(ewin);
     }
   else
     {
	AddToFamily(ev->xmap.window);
	HintsSetClientList();
     }

   EDBUG_RETURN_;
}

void
HandleReparent(XEvent * ev)
{
   EDBUG(5, "HandleReparent");
   EDBUG_RETURN_;
   ev = NULL;
}

void
HandleConfigureNotify(XEvent * ev)
{
   EDBUG(5, "HandleConfigureNotify");

#ifdef USE_XRANDR
   if (ev->xconfigure.window == root.win)
      DialogOK("Wheee! (ConfigureNotify)",
	       "Screen size changed to\n%dx%d pixels",
	       ev->xconfigure.width, ev->xconfigure.height);
#endif

   EDBUG_RETURN_;
   ev = NULL;
}

void
HandleConfigureRequest(XEvent * ev)
{
   Window              win, winrel;
   EWin               *ewin, *ewin2;
   int                 x = 0, y = 0, w = 0, h = 0;
   XWindowChanges      xwc;

   EDBUG(5, "HandleConfigureRequest");
   win = ev->xconfigurerequest.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	x = ewin->x + ewin->border->border.left;
	y = ewin->y + ewin->border->border.top;
	w = ewin->client.w;
	h = ewin->client.h;
	winrel = 0;
	if (ev->xconfigurerequest.value_mask & CWX)
	   x = ev->xconfigurerequest.x;
	if (ev->xconfigurerequest.value_mask & CWY)
	   y = ev->xconfigurerequest.y;
	if (ev->xconfigurerequest.value_mask & CWWidth)
	   w = ev->xconfigurerequest.width;
	if (ev->xconfigurerequest.value_mask & CWHeight)
	   h = ev->xconfigurerequest.height;
	if (ev->xconfigurerequest.value_mask & CWSibling)
	   winrel = ev->xconfigurerequest.above;
	if (ev->xconfigurerequest.value_mask & CWStackMode)
	  {
	     ewin2 = FindItem(NULL, winrel, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	     if (ewin2)
		winrel = ewin2->win;
	     xwc.sibling = winrel;
	     xwc.stack_mode = ev->xconfigurerequest.detail;
	     if (Mode.mode == MODE_NONE)
	       {
		  if (xwc.stack_mode == Above)
		     RaiseEwin(ewin);
		  else if (xwc.stack_mode == Below)
		     LowerEwin(ewin);
	       }
	     /*        else
	      * XConfigureWindow(disp, ewin->win,
	      * ev->xconfigurerequest.value_mask &
	      * (CWSibling | CWStackMode), &xwc); */
	  }
	/* this ugly workaround here is because x11amp is very brain-dead */
	/* and sets its minunum and maximm sizes the same - fair enough */
	/* to ensure it doesnt get resized - BUT hwne it shades itself */
	/* it resizes down to a smaller size - of course keeping the */
	/* minimum and maximim size same - E unconditionally disallows any */
	/* client window to be resized outside of its constraints */
	/* (any client could do this resize - not just x11amp thus E is */
	/* imposing the hints x11amp set up - this works around by */
	/* modifying the constraints to fit what the app requested */
	if (w < ewin->client.width.min)
	   ewin->client.width.min = w;
	if (w > ewin->client.width.max)
	   ewin->client.width.max = w;
	if (h < ewin->client.height.min)
	   ewin->client.height.min = h;
	if (h > ewin->client.height.max)
	   ewin->client.height.max = h;
	MoveResizeEwin(ewin, x - ewin->border->border.left,
		       y - ewin->border->border.top, w, h);
	if (Mode.mode == MODE_MOVE_PENDING || Mode.mode == MODE_MOVE)
	   ICCCM_Configure(ewin);
	{
	   char                pshaped;

	   pshaped = ewin->client.shaped;
	   ICCCM_GetShapeInfo(ewin);
	   if (pshaped != ewin->client.shaped)
	     {
		SyncBorderToEwin(ewin);
		PropagateShapes(ewin->win);
	     }
	}
	ReZoom(ewin);
     }
   else
     {
	xwc.x = ev->xconfigurerequest.x;
	xwc.y = ev->xconfigurerequest.y;
	xwc.width = ev->xconfigurerequest.width;
	xwc.height = ev->xconfigurerequest.height;
	xwc.border_width = ev->xconfigurerequest.border_width;
	xwc.sibling = ev->xconfigurerequest.above;
	xwc.stack_mode = ev->xconfigurerequest.detail;
	XConfigureWindow(disp, win, ev->xconfigurerequest.value_mask, &xwc);
     }
   EDBUG_RETURN_;
}

void
HandleResizeRequest(XEvent * ev)
{
   Window              win;
   EWin               *ewin;
   int                 w, h;

   EDBUG(5, "HandleResizeRequest");
   win = ev->xresizerequest.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	w = ev->xresizerequest.width;
	h = ev->xresizerequest.height;
	ResizeEwin(ewin, w, h);
	{
	   char                pshaped;

	   pshaped = ewin->client.shaped;
	   ICCCM_GetShapeInfo(ewin);
	   if (pshaped != ewin->client.shaped)
	     {
		SyncBorderToEwin(ewin);
		PropagateShapes(ewin->win);
	     }
	}
	ReZoom(ewin);
     }
   else
      EResizeWindow(disp, win, ev->xresizerequest.width,
		    ev->xresizerequest.height);
   EDBUG_RETURN_;
}

void
HandleCirculateRequest(XEvent * ev)
{
   Window              win;
   EWin               *ewin;

   EDBUG(5, "HandleCirculate");
   win = ev->xcirculaterequest.window;
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	if (ev->xcirculaterequest.place == PlaceOnTop)
	   RaiseEwin(ewin);
	else
	   LowerEwin(ewin);
     }
   else
     {
	if (ev->xcirculaterequest.place == PlaceOnTop)
	   XRaiseWindow(disp, win);
	else
	   XLowerWindow(disp, win);
     }
   EDBUG_RETURN_;
}

void
HandleProperty(XEvent * ev)
{
   Window              win = ev->xproperty.window;
   EWin               *ewin;

   EDBUG(5, "HandleProperty");

   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (!ewin)
      goto done;

   GrabX();
   EwinChangesStart(ewin);

   HintsProcessPropertyChange(ewin, ev->xproperty.atom);
   ICCCM_GetTitle(ewin, ev->xproperty.atom);
   ICCCM_GetHints(ewin, ev->xproperty.atom);
   ICCCM_GetInfo(ewin, ev->xproperty.atom);
   ICCCM_Cmap(ewin);
   ICCCM_GetGeoms(ewin, ev->xproperty.atom);
   SessionGetInfo(ewin, ev->xproperty.atom);
   SyncBorderToEwin(ewin);

   EwinChangesProcess(ewin);
   UngrabX();

 done:
   EDBUG_RETURN_;
}

void
HandleClientMessage(XEvent * ev)
{
   EDBUG(5, "HandleClientMessage");

   HintsProcessClientMessage(&(ev->xclient));

   EDBUG_RETURN_;
}

void
HandleChildShapeChange(XEvent * ev)
{
   Window              win = ((XShapeEvent *) ev)->window;
   EWin               *ewin;
   Border             *b;

   EDBUG(5, "HandleChildShapeChange");

   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	b = ewin->border;
	SyncBorderToEwin(ewin);
	if (ewin->border == b)
	   PropagateShapes(ewin->win);
     }

   EDBUG_RETURN_;
}

#ifdef USE_XRANDR
#include <X11/extensions/Xrandr.h>

void
HandleScreenChange(XEvent * evx)
{
   XRRScreenChangeNotifyEvent *ev = (XRRScreenChangeNotifyEvent *) evx;

   EDBUG(5, "HandleScreenChange");

   DialogOK("Wheee! (RRScreenChangeNotify)",
	    "Screen size changed to\n%dx%d pixels (%dx%d millimeters)",
	    ev->width, ev->height, ev->mwidth, ev->mheight);

   EDBUG_RETURN_;
}
#endif
