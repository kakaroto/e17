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
 * on_screen: Require window to be on-screen now
 */
static int
FocusEwinValid(EWin * ewin, int on_screen)
{
   if (!ewin)
      return 0;

   if (ewin->neverfocus || ewin->iconified)
      return 0;

   if (!ewin->client.need_input)
      return 0;

   if (!EwinIsMapped(ewin))
      return 0;

   return !on_screen || EwinIsOnScreen(ewin);
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
	ewin = GetEwinPointerInClient();
	break;
     case MODE_FOCUS_SLOPPY:
	ewin = GetEwinPointerInClient();
	if (ewin)
	   break;
	/* If pointer not in window -  fall thru and select other */
     case MODE_FOCUS_CLICK:
	lst = EwinListGetFocus(&num);
	for (i = 0; i < num; i++)
	  {
	     if (!FocusEwinValid(lst[i], 1) || lst[i]->skipfocus)
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

void
FocusGetNextEwin(void)
{
   EWin               *const *lst;
   EWin               *ewin;
   int                 i, num;

   lst = EwinListGetFocus(&num);
   if (num <= 1)
      return;

   ewin = NULL;
   for (i = num - 1; i >= 0; i--)
     {
	if (lst[i]->skipfocus || !FocusEwinValid(lst[i], 1))
	   continue;
	ewin = lst[i];
	break;
     }

   if (ewin)
      FocusToEWin(ewin, FOCUS_NEXT);
}

void
FocusGetPrevEwin(void)
{
   EWin               *const *lst;
   EWin               *ewin;
   int                 i, num;

   lst = EwinListGetFocus(&num);
   if (num <= 1)
      return;

   ewin = NULL;
   for (i = 0; i < num; i++)
     {
	if (lst[i]->skipfocus || !FocusEwinValid(lst[i], 1))
	   continue;
	ewin = lst[i];
	break;
     }

   if (ewin)
      FocusToEWin(ewin, FOCUS_PREV);
}

void
FocusEwinSetGrabs(EWin * ewin)
{
   if ((Conf.focus.mode != MODE_FOCUS_CLICK &&
	ewin->active && Conf.focus.clickraises) ||
       (Conf.focus.mode == MODE_FOCUS_CLICK && !ewin->active))
     {
	GrabButtonSet(AnyButton, AnyModifier, ewin->win_container,
		      ButtonPressMask, ECSR_PGRAB, 1);
#if 0
	Eprintf("FocusEwinSetGrabs: %#lx grab %s\n", ewin->client.win,
		EwinGetTitle(ewin));
#endif
     }
   else
     {
	GrabButtonRelease(AnyButton, AnyModifier, ewin->win_container);
#if 0
	Eprintf("FocusEwinSetGrabs: %#lx ungrab %s\n", ewin->client.win,
		EwinGetTitle(ewin));
#endif
     }
}

static void
FocusEwinSetActive(EWin * ewin, int active)
{
   ewin->active = active;
   EwinBorderUpdateState(ewin);

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
	GrabButtonRelease(AnyButton, AnyModifier, ewin->win_container);
	FocusEwinSetGrabs(ewin);
     }

   EDBUG_RETURN_;
}

void
FocusToEWin(EWin * ewin, int why)
{
   int                 do_follow = 0;
   int                 do_raise = 0, do_warp = 0;

   EDBUG(4, "FocusToEWin");

   if (EventDebug(EDBUG_TYPE_FOCUS))
     {
	if (ewin)
	   Eprintf("FocusToEWin %#lx %s why=%d\n", ewin->client.win,
		   EwinGetTitle(ewin), why);
	else
	   Eprintf("FocusToEWin None why=%d\n", why);
     }

   switch (why)
     {
     case FOCUS_NEXT:
     case FOCUS_PREV:
	if (Conf.focus.raise_on_next)
	   do_raise = 1;
	if (Conf.focus.warp_on_next)
	   do_warp = 1;
	/* Fall thru */
     default:
     case FOCUS_SET:
     case FOCUS_ENTER:
     case FOCUS_LEAVE:		/* Unused */
     case FOCUS_CLICK:
	if (ewin == Mode.focuswin)
	   EDBUG_RETURN_;
	if (ewin == NULL)	/* Unfocus */
	   break;
	if (!FocusEwinValid(ewin, 1))
	   EDBUG_RETURN_;
	break;

     case FOCUS_DESK_ENTER:
	ewin = FocusEwinSelect();
	if (!ewin)
	   EDBUG_RETURN_;
	break;

     case FOCUS_NONE:
     case FOCUS_DESK_LEAVE:
	ewin = NULL;
	if (ewin == Mode.focuswin)
	   EDBUG_RETURN_;
	break;

     case FOCUS_EWIN_GONE:
	if (ewin != Mode.focuswin)
	   EDBUG_RETURN_;
	ewin = FocusEwinSelect();
	if (ewin == Mode.focuswin)
	   ewin = NULL;
	break;

     case FOCUS_EWIN_NEW:
	/* Don't chase around after the windows at startup */
	if (Mode.wm.startup)
	   EDBUG_RETURN_;

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
	if (ewin == Mode.focuswin)
	   EDBUG_RETURN_;
	if (!FocusEwinValid(ewin, 0))
	   EDBUG_RETURN_;
	break;
     }

   if (ewin == Mode.focuswin)
      EDBUG_RETURN_;

   /* Check if ewin is a valid focus window target */

   if (!ewin)
      goto done;

   /* NB! ewin != NULL */

   if (do_follow)
      GotoDesktopByEwin(ewin);

   if (Conf.autoraise.enable)
     {
	RemoveTimerEvent("AUTORAISE_TIMEOUT");

	if (Conf.focus.mode != MODE_FOCUS_CLICK)
	   DoIn("AUTORAISE_TIMEOUT", Conf.autoraise.delay, AutoraiseTimeout,
		ewin->client.win, NULL);
     }

   if (do_raise)
      RaiseEwin(ewin);

   if (do_warp && ewin != Mode.mouse_over_ewin)
      XWarpPointer(disp, None, ewin->win, 0, 0, 0, 0, ewin->w / 2, ewin->h / 2);

   RemoveTimerEvent("REVERSE_FOCUS_TIMEOUT");
   switch (why)
     {
     default:
     case FOCUS_PREV:
	DoIn("REVERSE_FOCUS_TIMEOUT", 0.5, ReverseTimeout, ewin->client.win,
	     NULL);
	break;
     case FOCUS_DESK_ENTER:
	break;
     case FOCUS_NEXT:
	EwinListRaise(&EwinListFocus, ewin, 0);
	break;
     }

   SoundPlay("SOUND_FOCUS_SET");
 done:
   /* Unset old focus window (if any) highlighting */
   if (Mode.focuswin)
      FocusEwinSetActive(Mode.focuswin, 0);
   ICCCM_Cmap(ewin);
   Mode.focuswin = ewin;
   /* Set new focus window (if any) highlighting */
   if (Mode.focuswin)
      FocusEwinSetActive(Mode.focuswin, 1);
   ICCCM_Focus(ewin);

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
   Mode.mouse_over_ewin = ewin;

   FocusToEWin(NULL, FOCUS_DESK_ENTER);

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
   if (win == VRoot.win &&
       (ev->xcrossing.mode == NotifyNormal &&
	ev->xcrossing.detail != NotifyInferior))
     {
	FocusToEWin(NULL, FOCUS_DESK_ENTER);
	goto done;
     }

   ewin = GetEwinByCurrentPointer();
   Mode.mouse_over_ewin = ewin;

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
   if (win == VRoot.win &&
       (ev->xcrossing.mode == NotifyNormal &&
	ev->xcrossing.detail != NotifyInferior))
      FocusToEWin(NULL, FOCUS_SET);
}

void
FocusHandleClick(Window win)
{
   EWin               *ewin;

   ewin = FindEwinByChildren(win);
   if (!ewin)
      ewin = FindEwinByBase(win);
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
