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

static int          new_desk_focus_nesting = 0;

/* Mostly stolen from the temporary 'ToolTipTimeout' */
static void
AutoraiseTimeout(int val, void *data)
{
   EWin               *ewin;

   if (conf.focus.mode == MODE_FOCUS_CLICK)
      return;

   ewin = FindItem("", val, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
      RaiseEwin(ewin);
}

static void
ReverseTimeout(int val, void *data)
{
   EWin               *ewin;

   ewin = RemoveItem("EWIN", val, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
      AddItem(ewin, "EWIN", ewin->client.win, LIST_TYPE_EWIN);
}

static void
FocusCycle(int inc)
{
   EWin              **lst0, **lst, *ewin;
   int                 i, num0, num, ax, ay;

   EDBUG(5, "FocusCycle");

   if (conf.warplist.enable)
     {
	WarpFocus(inc);
	EDBUG_RETURN_;
     }

   /* On previous only ? */
   RemoveTimerEvent("REVERSE_FOCUS_TIMEOUT");
   DoIn("REVERSE_FOCUS_TIMEOUT", 1.0, ReverseTimeout, 0, NULL);

   GetCurrentArea(&ax, &ay);

   lst0 = (EWin **) ListItemType(&num0, LIST_TYPE_EWIN);
   if (lst0 == NULL)
      EDBUG_RETURN_;

   num = 0;
   lst = NULL;
   for (i = 0; i < num0; i++)
     {
	ewin = lst0[i];
	DetermineEwinArea(ewin);	// ???
	if (((ewin->sticky) || (ewin->desktop == desks.current)) &&
	    ((ewin->area_x == ax) && (ewin->area_y == ay)) &&
	    (!ewin->skipfocus) && (!ewin->neverfocus) &&
	    (!ewin->shaded) && (!ewin->iconified) &&
	    (!ewin->menu) && (!ewin->pager) && (!ewin->ibox))
	  {
	     num++;
	     lst = Erealloc(lst, sizeof(EWin *) * num);
	     lst[num - 1] = lst0[i];
	  }
     }
   Efree(lst0);

   if (lst == NULL)
      EDBUG_RETURN_;

   for (i = 0; i < num; i++)
     {
	if (mode.focuswin == lst[i])
	   break;
     }
   i += inc + num;
   i %= num;
   ewin = lst[i];
   Efree(lst);

   FocusToEWin(ewin, FOCUS_SET);

   EDBUG_RETURN_;
}

void
FocusGetNextEwin(void)
{
   FocusCycle(1);
}

void
FocusGetPrevEwin(void)
{
   FocusCycle(-1);
}

void
FocusFix(void)
{
   EWin              **lst, *ewin;
   int                 i, num;

   EDBUG(5, "FocusFix");
   num = 0;
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (conf.focus.mode == MODE_FOCUS_CLICK)
	       {
		  if (!(ewin->active))
		     XGrabButton(disp, AnyButton, AnyModifier,
				 ewin->win_container, False, ButtonPressMask,
				 GrabModeSync, GrabModeAsync, None, None);
	       }
	     else
	       {
		  XUngrabButton(disp, AnyButton, AnyModifier,
				ewin->win_container);
		  GrabButtonGrabs(ewin);
	       }
	  }
	Efree(lst);
     }
}

void
FocusToEWin(EWin * ewin, int why)
{
   int                 ax, ay;

   EDBUG(4, "FocusToEWin");

   if (mode.slideout)
      EDBUG_RETURN_;

   if (clickmenu)
      EDBUG_RETURN_;

   ICCCM_Cmap(ewin);
   if ((!ewin) && (conf.focus.mode != MODE_FOCUS_POINTER))
     {
	ewin = FindItem("", 0, LIST_FINDBY_NONE, LIST_TYPE_EWIN);
	if (conf.focus.mode == MODE_FOCUS_CLICK)
	  {
	     if ((mode.focuswin) && (ewin))
	       {
		  if (!
		      (((ewin->desktop
			 == mode.focuswin->desktop) || (ewin->sticky))
		       && (ewin->area_x == mode.focuswin->area_x)
		       && (ewin->area_y == mode.focuswin->area_y)))
		     ewin = NULL;
	       }
	     else if (!mode.focuswin)
		ewin = NULL;
	  }
     }
   if (ewin == mode.focuswin)
     {
	if (!mode.cur_menu_mode)
	   mode.context_ewin = ewin;
	EDBUG_RETURN_;
     }
   /* Never focus a window that's not on the current desktop.  That's just dumb. -- mej */
   if ((ewin) && (ewin->desktop != desks.current && !(ewin->sticky)))
      ewin = NULL;
   if ((ewin) && (ewin->neverfocus))
     {
	EDBUG_RETURN_;
     }
   if ((ewin) && (!ewin->client.need_input))
     {
	if (mode.windowdestroy == 1)
	  {
	     mode.focuswin = NULL;
	     mode.realfocuswin = NULL;
	     mode.windowdestroy = 0;
	  }
	if (!mode.cur_menu_mode)
	   mode.context_ewin = ewin;
	EDBUG_RETURN_;
     }
   mode.windowdestroy = 0;
   if (mode.focuswin)
     {
	if (conf.autoraise)
	   RemoveTimerEvent("AUTORAISE_TIMEOUT");
	mode.focuswin->active = 0;
	/* losing the focus may cause the titlebar to be resized */
	CalcEwinSizes(mode.focuswin);
	DrawEwin(mode.focuswin);
	if ((conf.focus.clickraises) || (conf.focus.mode == MODE_FOCUS_CLICK))
	   XGrabButton(disp, AnyButton, AnyModifier,
		       mode.focuswin->win_container, False, ButtonPressMask,
		       GrabModeSync, GrabModeAsync, None, None);
     }
   if (!ewin)
     {
	mode.focuswin = NULL;
	mode.realfocuswin = NULL;
	mode.context_ewin = NULL;
	ICCCM_Focus(NULL);
	EDBUG_RETURN_;
     }
   else if (!ewin->menu)
      mode.realfocuswin = ewin;
   if (ewin)
     {
	if (!mode.cur_menu_mode)
	   mode.context_ewin = ewin;
	mode.focuswin = ewin;
	mode.focuswin->active = 1;
     }
   /* gaining the focus may cause the titlebar to be resized */
   if ((conf.focus.mode == MODE_FOCUS_CLICK) && (mode.focuswin))
     {
	XUngrabButton(disp, AnyButton, AnyModifier,
		      mode.focuswin->win_container);
	GrabButtonGrabs(mode.focuswin);
     }
   if (mode.focuswin)
     {
	CalcEwinSizes(mode.focuswin);
	DrawEwin(mode.focuswin);
	SoundPlay("SOUND_FOCUS_SET");
	ICCCM_Focus(mode.focuswin);
     }
/*   ReZoom(mode.focuswin); */
   if ((conf.autoraise) && (mode.focuswin) && (!mode.focuswin->menu)
       && (conf.focus.mode != MODE_FOCUS_CLICK))
      DoIn("AUTORAISE_TIMEOUT", conf.autoraisetime, AutoraiseTimeout,
	   mode.focuswin->client.win, NULL);
   if (conf.focus.mode == MODE_FOCUS_CLICK)
     {
	if (ewin)
	  {
	     if (!ewin->sticky)
	       {
		  if ((ewin->desktop != desks.current) && (!ewin->iconified))
		     GotoDesktop(ewin->desktop);
	       }
	     if ((!ewin->fixedpos) && (!ewin->sticky))
	       {
		  GetCurrentArea(&ax, &ay);
		  if ((ax != ewin->area_x) || (ay != ewin->area_y))
		    {
		       if ((ewin->x >= root.w) || (ewin->y >= root.h)
			   || ((ewin->x + ewin->w) < 0)
			   || ((ewin->y + ewin->h) < 0))
			  SetCurrentArea(ewin->area_x, ewin->area_y);
		    }
	       }
	  }
     }
   RemoveTimerEvent("REVERSE_FOCUS_TIMEOUT");
   if (mode.focuswin)
      DoIn("REVERSE_FOCUS_TIMEOUT", 0.5, ReverseTimeout,
	   mode.focuswin->client.win, NULL);
   EDBUG_RETURN_;
}

void
FocusNewDeskBegin(void)
{
   EWin               *ewin, **lst;
   int                 i, j, num;

   if (new_desk_focus_nesting++)
      return;

   /* we are about to flip desktops or areas - disable enter and leave events
    * temporarily */

   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     XSelectInput(disp, ewin->win,
			  FocusChangeMask | SubstructureNotifyMask |
			  SubstructureRedirectMask | PropertyChangeMask |
			  ResizeRedirectMask);
	     if (ewin->pager)
	       {
#if 0
		  XSelectInput(disp, ewin->client.win,
			       PropertyChangeMask | FocusChangeMask |
			       ResizeRedirectMask | StructureNotifyMask |
			       ColormapChangeMask | ButtonPressMask |
			       ButtonReleaseMask | PointerMotionMask);
#endif
	       }
	     else if (ewin->dialog)
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask | ExposureMask | KeyPressMask);
	     else
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask);
	     for (j = 0; j < ewin->border->num_winparts; j++)
	       {
		  if (ewin->border->part[j].flags & FLAG_TITLE)
		     XSelectInput(disp, ewin->bits[j].win,
				  ExposureMask | ButtonPressMask |
				  ButtonReleaseMask);
		  else
		     XSelectInput(disp, ewin->bits[j].win,
				  ButtonPressMask | ButtonReleaseMask);
	       }
	  }
	Efree(lst);
     }

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
      XSelectInput(disp, desks.desk[i].win,
		   PropertyChangeMask | SubstructureRedirectMask |
		   ButtonPressMask | ButtonReleaseMask);
}

void
FocusNewDesk(void)
{
   EWin               *ewin, **lst;
   int                 i, j, num;

   EDBUG(4, "FocusNewDesk");

   if (--new_desk_focus_nesting)
      return;

   /* we flipped - re-enable enter and leave events */
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     XSelectInput(disp, ewin->win,
			  FocusChangeMask | SubstructureNotifyMask |
			  SubstructureRedirectMask | EnterWindowMask |
			  LeaveWindowMask | PointerMotionMask |
			  PropertyChangeMask | ResizeRedirectMask |
			  ButtonPressMask | ButtonReleaseMask);
	     if (ewin->pager)
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | EnterWindowMask |
			     LeaveWindowMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask | ButtonPressMask |
			     ButtonReleaseMask | PointerMotionMask);
	     else if (ewin->dialog)
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | EnterWindowMask |
			     LeaveWindowMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask | ExposureMask | KeyPressMask);
	     else
		XSelectInput(disp, ewin->client.win,
			     PropertyChangeMask | EnterWindowMask |
			     LeaveWindowMask | FocusChangeMask |
			     ResizeRedirectMask | StructureNotifyMask |
			     ColormapChangeMask);
	     for (j = 0; j < ewin->border->num_winparts; j++)
	       {
		  if (ewin->border->part[j].flags & FLAG_TITLE)
		     XSelectInput(disp, ewin->bits[j].win,
				  ExposureMask | KeyPressMask | KeyReleaseMask |
				  ButtonPressMask | ButtonReleaseMask |
				  EnterWindowMask | LeaveWindowMask |
				  PointerMotionMask);
		  else
		     XSelectInput(disp, ewin->bits[j].win,
				  KeyPressMask | KeyReleaseMask |
				  ButtonPressMask | ButtonReleaseMask |
				  EnterWindowMask | LeaveWindowMask |
				  PointerMotionMask);
	       }
	  }
	Efree(lst);
     }

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
      XSelectInput(disp, desks.desk[i].win,
		   SubstructureNotifyMask | ButtonPressMask |
		   ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
		   ButtonMotionMask | PropertyChangeMask |
		   SubstructureRedirectMask | KeyPressMask | KeyReleaseMask |
		   PointerMotionMask);

   if ((conf.focus.mode == MODE_FOCUS_POINTER) ||
       (conf.focus.mode == MODE_FOCUS_SLOPPY))
     {
	ewin = GetEwinPointerInClient();
	ICCCM_Focus(ewin);
     }
   else
     {
	lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
	if (lst)
	  {
	     int                 ax, ay;

	     GetCurrentArea(&ax, &ay);
	     for (i = 0; i < num; i++)
	       {
		  ewin = lst[i];
		  if ((ewin->sticky) ||
		      ((((ewin->area_x == ax) && (ewin->area_y == ay)) ||
			(ewin->fixedpos)) && (ewin->desktop == desks.current)))
		    {
		       ICCCM_Focus(ewin);
		       break;
		    }
	       }
	     Efree(lst);
	  }
     }

   EDBUG_RETURN_;
}

#if 0				/* Clean up if OK -- Remove FocusToNone */
void
FocusToNone(void)
{

   EDBUG(4, "FocusToNone");

   if ((mode.focuswin) && (mode.focuswin->floating))
      EDBUG_RETURN_;
   ICCCM_Cmap(NULL);
   if (mode.focuswin)
     {
	if (conf.autoraise)
	   RemoveTimerEvent("AUTORAISE_TIMEOUT");
	mode.focuswin->active = 0;
	/* losing the focus may cause the titlebar to be resized */
	CalcEwinSizes(mode.focuswin);
	DrawEwin(mode.focuswin);
	if (conf.focus.mode == MODE_FOCUS_CLICK)
	   XGrabButton(disp, AnyButton, AnyModifier,
		       mode.focuswin->win_container, False, ButtonPressMask,
		       GrabModeSync, GrabModeAsync, None, None);
     }
   XSetInputFocus(disp, root.win, RevertToPointerRoot, CurrentTime);
   mode.focuswin = NULL;
   mode.realfocuswin = NULL;
   EDBUG_RETURN_;
}
#endif

void
HandleFocusWindowIn(Window win)
{
   EWin               *ewin;

   EDBUG(5, "HandleFocusWindowIn");
   ewin = FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (!ewin)
      ewin = FindEwinByBase(win);
   if (ewin != mode.focuswin)
     {
	if (mode.focuswin)
	  {
	     mode.focuswin->active = 0;
	     DrawEwin(mode.focuswin);
	     if (conf.focus.mode == MODE_FOCUS_CLICK)
		XGrabButton(disp, AnyButton, AnyModifier,
			    mode.focuswin->win_container, False,
			    ButtonPressMask, GrabModeSync, GrabModeAsync, None,
			    None);
	  }
	mode.focuswin = ewin;
	if ((ewin) && (!ewin->menu))
	  {
	     mode.realfocuswin = ewin;
	     if (!mode.cur_menu_mode)
		mode.context_ewin = ewin;
	  }
	if (mode.focuswin)
	  {
	     mode.focuswin->active = 1;
	     DrawEwin(mode.focuswin);
	     if (conf.focus.mode == MODE_FOCUS_CLICK)
	       {
		  XUngrabButton(disp, AnyButton, AnyModifier,
				mode.focuswin->win_container);
		  GrabButtonGrabs(mode.focuswin);
	       }
	  }
     }
   EDBUG_RETURN_;
}

void
HandleFocusWindow(Window win)
{
   EWin               *found_ewin;

   EDBUG(5, "HandleFocusWindow");
   if (win == 0)
      FocusToEWin(NULL, FOCUS_SET);
   else
     {
	found_ewin = FindEwinByChildren(win);
	if (!found_ewin)
	   found_ewin = FindEwinByBase(win);
	if (conf.focus.mode == MODE_FOCUS_CLICK)
	   mode.mouse_over_win = found_ewin;
	else if (conf.focus.mode == MODE_FOCUS_SLOPPY)
	  {
	     if (!found_ewin)
		ICCCM_Cmap(NULL);
	     else if (!(found_ewin->focusclick))
		FocusToEWin(found_ewin, FOCUS_SET);
	     mode.mouse_over_win = found_ewin;
	  }
	else if (conf.focus.mode == MODE_FOCUS_POINTER)
	  {
	     if (!found_ewin)
		found_ewin = GetEwinPointerInClient();
	     FocusToEWin(found_ewin, FOCUS_SET);
	     mode.mouse_over_win = found_ewin;
	  }
     }
   EDBUG_RETURN_;
}

/*
 * Focus event handlers
 */

void
FocusHandleEnter(XEvent * ev)
{
   /*
    * multi screen handling -- root windows receive
    * enter / leave notify
    */
   if (ev->xany.window == root.win)
     {
	if (!mode.focuswin || conf.focus.mode == MODE_FOCUS_POINTER)
	   HandleFocusWindow(0);
     }
   else
     {
	HandleFocusWindow(ev->xcrossing.window);
     }
}

void
FocusHandleLeave(XEvent * ev)
{
   /*
    * If we are leaving the root window, we are switching
    * screens on a multi screen system - need to unfocus
    * to allow other desk to grab focus...
    */
   if (ev->xcrossing.window == root.win)
     {
	if (ev->xcrossing.mode == NotifyNormal
	    && ev->xcrossing.detail != NotifyInferior && mode.focuswin)
	   HandleFocusWindow(0);
	else
	   HandleFocusWindow(ev->xcrossing.window);
     }
/* THIS caused the "emacs focus bug" ? */
/*      else */
/*      HandleFocusWindow(ev->xcrossing.window); */
}
