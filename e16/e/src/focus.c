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

/*
 * Return !0 if it is OK to focus ewin.
 */
static int
FocusEwinValid(EWin * ewin)
{
   if (ewin->skipfocus || ewin->neverfocus || ewin->shaded || ewin->iconified)
      return 0;

   if (!ewin->mapped)
      return 0;

   return EwinIsOnScreen(ewin);
}

/*
 * Return the ewin to focus after entering area or losing focused window.
 */
static EWin        *
FocusEwinSelect(void)
{
   EWin               *const *lst, *ewin = NULL;
   int                 num, i;

   switch (Conf.focus.mode)
     {
     default:
     case MODE_FOCUS_POINTER:
     case MODE_FOCUS_SLOPPY:
	ewin = GetEwinPointerInClient();
	break;
     case MODE_FOCUS_CLICK:
	lst = EwinListGetFocus(&num);
	for (i = 0; i < num; i++)
	  {
	     if (!FocusEwinValid(lst[i]))
		continue;
	     ewin = lst[i];
	     break;
	  }
	break;
     }

   return ewin;
}

static void
AutoraiseTimeout(int val, void *data __UNUSED__)
{
   EWin               *ewin;

   if (Conf.focus.mode == MODE_FOCUS_CLICK)
      return;

   ewin = FindItem("", val, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
      RaiseEwin(ewin);
}

static void
ReverseTimeout(int val, void *data __UNUSED__)
{
   EWin               *ewin;

   ewin = FindItem("", val, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
      EwinListRaise(&EwinListFocus, ewin, 0);
}

static void
FocusCycle(int inc)
{
   EWin               *const *lst0;
   EWin              **lst, *ewin;
   int                 i, num0, num, ax, ay;

   EDBUG(5, "FocusCycle");

   /* On previous only ? */
   RemoveTimerEvent("REVERSE_FOCUS_TIMEOUT");
   DoIn("REVERSE_FOCUS_TIMEOUT", 1.0, ReverseTimeout, 0, NULL);

   GetCurrentArea(&ax, &ay);

   lst0 = EwinListGetFocus(&num0);
   if (lst0 == NULL)
      EDBUG_RETURN_;

   num = 0;
   lst = NULL;
   for (i = 0; i < num0; i++)
     {
	ewin = lst0[i];
	if (FocusEwinValid(ewin))
	  {
	     num++;
	     lst = Erealloc(lst, sizeof(EWin *) * num);
	     lst[num - 1] = ewin;
	  }
     }

   if (lst == NULL)
      EDBUG_RETURN_;

   for (i = 0; i < num; i++)
     {
	if (Mode.focuswin == lst[i])
	   break;
     }
   i += inc + num;
   i %= num;
   ewin = lst[i];
   Efree(lst);

   FocusToEWin(ewin, FOCUS_NEXT);

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
FocusEwinSetGrabs(EWin * ewin)
{
   if ((Conf.focus.mode != MODE_FOCUS_CLICK &&
	ewin->active && Conf.focus.clickraises) ||
       (Conf.focus.mode == MODE_FOCUS_CLICK && !ewin->active))
     {
	XGrabButton(disp, AnyButton, AnyModifier, ewin->win_container,
		    False, ButtonPressMask, GrabModeSync, GrabModeAsync,
		    None, None);
/*	Eprintf("FocusEwinSetGrabs: %#lx grab\n", ewin->client.win); */
     }
   else
     {
	XUngrabButton(disp, AnyButton, AnyModifier, ewin->win_container);
/*	Eprintf("FocusEwinSetGrabs: %#lx ungrab\n", ewin->client.win); */
     }
}

static void
FocusEwinSetActive(EWin * ewin, int active)
{
   ewin->active = active;
   EwinBorderDraw(ewin, 0, 1);

   FocusEwinSetGrabs(ewin);
}

void
FocusFix(void)
{
   EWin               *const *lst, *ewin;
   int                 i, num;

   EDBUG(5, "FocusFix");

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	XUngrabButton(disp, AnyButton, AnyModifier, ewin->win_container);
	FocusEwinSetGrabs(ewin);
     }

   EDBUG_RETURN_;
}

void
FocusToEWin(EWin * ewin, int why)
{
   int                 do_follow = 0;

   EDBUG(4, "FocusToEWin");

   if (EventDebug(EDBUG_TYPE_FOCUS))
     {
	if (ewin)
	   Eprintf("FocusToEWin %#lx %s why=%d\n", ewin->client.win,
		   ewin->icccm.wm_res_name, why);
	else
	   Eprintf("FocusToEWin None why=%d\n", why);
     }

   switch (why)
     {
     default:
	if (ewin == Mode.focuswin)
	   EDBUG_RETURN_;
	break;

     case FOCUS_EWIN_GONE:
	if (ewin != Mode.focuswin)
	   EDBUG_RETURN_;
	Mode.focuswin = NULL;
	if (Conf.focus.mode == MODE_FOCUS_CLICK)
	   ewin = FocusEwinSelect();
	else
	   ewin = NULL;
	break;

     case FOCUS_EWIN_NEW:
	if (Conf.focus.all_new_windows_get_focus)
	  {
	     do_follow = 2;
	  }
	else if (Conf.focus.new_transients_get_focus)
	  {
	     if (ewin->client.transient)
		do_follow = 2;
	  }
	else if (Conf.focus.new_transients_get_focus_if_group_focused)
	  {
	     EWin               *ewin2;

	     ewin2 = FindItem(NULL, ewin->client.transient_for,
			      LIST_FINDBY_ID, LIST_TYPE_EWIN);
	     if ((ewin2) && (Mode.focuswin == ewin2))
		do_follow = 2;
	  }
	else if (Mode.place)
	  {
	     do_follow = 1;
	  }

	if (!do_follow)
	   EDBUG_RETURN_;
	break;

     case FOCUS_WARP_NEXT:
	why = FOCUS_NEXT;
	break;

     case FOCUS_WARP_DONE:
	break;
     }

   if (!ewin)
      goto done;

   /* NB! ewin != NULL */

   if (ewin->iconified || ewin->neverfocus)
      EDBUG_RETURN_;

   if (!ewin->client.need_input)
      EDBUG_RETURN_;

   /* Don't focus menus (use client.need_input?) */
   if (ewin->menu)
      EDBUG_RETURN_;

   if (do_follow && !Mode.startup)
      GotoDesktopByEwin(ewin);

   if (!EwinIsOnScreen(ewin))
      EDBUG_RETURN_;

   if (Conf.autoraise.enable)
     {
	RemoveTimerEvent("AUTORAISE_TIMEOUT");

	if (Conf.focus.mode != MODE_FOCUS_CLICK)
	   DoIn("AUTORAISE_TIMEOUT", Conf.autoraise.delay, AutoraiseTimeout,
		ewin->client.win, NULL);
     }

   if ((Conf.focus.raise_on_next_focus && (why == FOCUS_NEXT)) ||
       (Conf.focus.raise_after_next_focus && (why == FOCUS_WARP_DONE)))
      RaiseEwin(ewin);

   if ((Conf.focus.warp_on_next_focus && (why == FOCUS_NEXT)) ||
       (Conf.focus.warp_after_next_focus && (why == FOCUS_WARP_DONE)))
     {
	if (ewin != Mode.mouse_over_win)
	   XWarpPointer(disp, None, ewin->win, 0, 0, 0, 0, ewin->w / 2,
			ewin->h / 2);
     }

   RemoveTimerEvent("REVERSE_FOCUS_TIMEOUT");
   if (why != FOCUS_DESK_ENTER)
      DoIn("REVERSE_FOCUS_TIMEOUT", 0.5, ReverseTimeout, ewin->client.win,
	   NULL);

   SoundPlay("SOUND_FOCUS_SET");
 done:
   /* Unset old focus window (if any) highlighting */
   if (Mode.focuswin)
      FocusEwinSetActive(Mode.focuswin, 0);
   ICCCM_Cmap(ewin);
   ICCCM_Focus(ewin);
   Mode.focuswin = ewin;
   /* Set new focus window (if any) highlighting */
   if (Mode.focuswin)
      FocusEwinSetActive(Mode.focuswin, 1);

   EDBUG_RETURN_;
}

void
FocusNewDeskBegin(void)
{
   if (new_desk_focus_nesting++)
      return;

   FocusToEWin(NULL, FOCUS_DESK_LEAVE);

   /* we are about to flip desktops or areas - disable enter and leave events
    * temporarily */
   EwinsEventsConfigure(1);
   DesktopsEventsConfigure(1);
}

void
FocusNewDesk(void)
{
   EWin               *ewin;

   EDBUG(4, "FocusNewDesk");

   if (--new_desk_focus_nesting)
      return;

   /* we flipped - re-enable enter and leave events */
   EwinsEventsConfigure(0);
   DesktopsEventsConfigure(0);

   /* Set the mouse-over window */
   ewin = GetEwinByCurrentPointer();
   Mode.mouse_over_win = ewin;

   ewin = FocusEwinSelect();
   if (ewin)
      FocusToEWin(ewin, FOCUS_DESK_ENTER);

   EDBUG_RETURN_;
}

/*
 * Focus event handlers
 */

void
FocusHandleEnter(XEvent * ev)
{
   Window              win = ev->xcrossing.window;
   EWin               *ewin;

   EDBUG(5, "FocusHandleEnter");

   /* Entering root may mean entering this screen */
   if (win == root.win &&
       (ev->xcrossing.mode == NotifyNormal &&
	ev->xcrossing.detail != NotifyInferior))
     {
	ewin = FocusEwinSelect();
	if (ewin)
	   FocusToEWin(ewin, FOCUS_DESK_ENTER);
	goto done;
     }

   ewin = GetEwinByCurrentPointer();
   Mode.mouse_over_win = ewin;

   switch (Conf.focus.mode)
     {
     default:
     case MODE_FOCUS_CLICK:
	break;
     case MODE_FOCUS_SLOPPY:
	if (!ewin || ewin->focusclick)
	   break;
	FocusToEWin(ewin, FOCUS_ENTER);
	break;
     case MODE_FOCUS_POINTER:
	if (ewin && ewin->focusclick)
	   break;
	FocusToEWin(ewin, FOCUS_ENTER);
	break;
     }

 done:
   EDBUG_RETURN_;
}

void
FocusHandleLeave(XEvent * ev)
{
   Window              win = ev->xcrossing.window;

   /* Leaving root may mean entering other screen */
   if (win == root.win &&
       (ev->xcrossing.mode == NotifyNormal &&
	ev->xcrossing.detail != NotifyInferior))
      FocusToEWin(NULL, FOCUS_SET);
}

#if 0				/* Remove old code */
void
FocusHandleEnter(XEvent * ev)
{
   /*
    * multi screen handling -- root windows receive
    * enter / leave notify
    */
   if (ev->xany.window == root.win)
     {
	if (!Mode.focuswin || Conf.focus.mode == MODE_FOCUS_POINTER)
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
	    && ev->xcrossing.detail != NotifyInferior && Mode.focuswin)
	   HandleFocusWindow(0);
	else
	   HandleFocusWindow(ev->xcrossing.window);
     }
/* THIS caused the "emacs focus bug" ? */
/*      else */
/*      HandleFocusWindow(ev->xcrossing.window); */
}
#endif

void
FocusHandleClick(Window win)
{
   EWin               *ewin;

   ewin = GetEwinByCurrentPointer();
   if (!ewin)
      return;

   if ((Conf.focus.clickraises) || (Conf.focus.mode == MODE_FOCUS_CLICK))
     {
	RaiseEwin(ewin);
	FocusToEWin(ewin, FOCUS_CLICK);

	/* allow click to pass thorugh */
	if (win == ewin->win_container)
	  {
	     XSync(disp, False);
	     XAllowEvents(disp, ReplayPointer, CurrentTime);
	     XSync(disp, False);
	  }
     }
   else if (ewin->focusclick)
     {
	FocusToEWin(ewin, FOCUS_CLICK);
     }
}
