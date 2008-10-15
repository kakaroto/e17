/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "cursors.h"
#include "desktops.h"		/* FIXME - Should not be here */
#include "dialog.h"
#include "emodule.h"
#include "ewins.h"
#include "focus.h"
#include "grabs.h"
#include "hints.h"
#include "settings.h"
#include "timers.h"
#include "xwin.h"

#define EwinListFocusRaise(ewin) EobjListFocusRaise(EoObj(ewin))

static char         focus_inhibit = 1;
static char         focus_is_set = 0;
static char         click_pending_update_grabs = 0;
static int          focus_pending_why = 0;
static EWin        *focus_pending_ewin = NULL;
static EWin        *focus_pending_new = NULL;
static EWin        *focus_pending_raise = NULL;
static Timer       *focus_timer_autoraise = NULL;

void
FocusEnable(int on)
{
   if (on)
     {
	if (focus_inhibit > 0)
	   focus_inhibit--;
     }
   else
     {
	focus_inhibit++;
     }

   if (EDebug(EDBUG_TYPE_FOCUS))
      Eprintf("FocusEnable inhibit=%d\n", focus_inhibit);
}

/*
 * Return !0 if it is OK to focus ewin.
 */
static int
FocusEwinValid(EWin * ewin, int want_on_screen, int click, int want_visible)
{
   if (!ewin)
      return 0;

#if 0
   Eprintf("FocusEwinValid %#lx %s: st=%d sh=%d inh=%d cl=%d(%d) vis=%d(%d)\n",
	   EwinGetClientXwin(ewin), EwinGetTitle(ewin),
	   ewin->state.state, EoIsShown(ewin), ewin->state.inhibit_focus,
	   click, ewin->props.focusclick, want_visible, ewin->state.visibility);
#endif

   if (ewin->state.inhibit_focus)
      return 0;

   if (!EwinIsMapped(ewin) || !EoIsShown(ewin))
      return 0;

   if (ewin->props.focusclick && !click)
      return 0;

   if (want_visible && ewin->state.visibility == VisibilityFullyObscured)
      return 0;

   return !want_on_screen || EwinIsOnScreen(ewin);
}

/*
 * Return the ewin to focus after entering area or losing focused window.
 */
static EWin        *
FocusEwinSelect(void)
{
   EWin               *const *lst, *ewin;
   int                 num, i;

   switch (Conf.focus.mode)
     {
     default:
     case MODE_FOCUS_POINTER:
	ewin = GetEwinPointerInClient();
	if (ewin && !FocusEwinValid(ewin, 1, 0, 0))
	   ewin = NULL;
	break;

     case MODE_FOCUS_SLOPPY:
	ewin = GetEwinPointerInClient();
	if (ewin && FocusEwinValid(ewin, 1, 0, 0))
	   break;
	goto do_select;

     case MODE_FOCUS_CLICK:
	goto do_select;

      do_select:
	ewin = NULL;
	lst = EwinListFocusGet(&num);
	for (i = 0; i < num; i++)
	  {
	     if (!FocusEwinValid(lst[i], 1, 0, 0) ||
		 lst[i]->props.skip_focuslist)
		continue;
	     ewin = lst[i];
	     break;
	  }
	break;
     }

   return ewin;
}

static int
AutoraiseTimeout(void *data)
{
   EWin               *ewin = (EWin *) data;

   if (Conf.focus.mode == MODE_FOCUS_CLICK)
      goto done;

   if (EwinFindByPtr(ewin))	/* May be gone */
      EwinRaise(ewin);

 done:
   focus_timer_autoraise = NULL;
   return 0;
}

static void
FocusRaisePending(void)
{
   EWin               *ewin = focus_pending_raise;
   unsigned int        mask;

   /* The focusing cycle ends when no more modifiers are depressed */
   mask = 0;
   EQueryPointer(NULL, NULL, NULL, NULL, &mask);
   if ((mask & Mode.masks.mod_key_mask) != 0)
      return;

   if (EwinFindByPtr(ewin))	/* May be gone */
      EwinListFocusRaise(ewin);

   GrabKeyboardRelease();

   focus_pending_raise = NULL;
}

/*
 * dir > 0: Focus previously focused window
 * else   : Focus least recently focused window
 */
static void
FocusCycleEwin(int dir)
{
   EWin               *const *lst;
   EWin               *ewin;
   int                 i, j, num;

   lst = EwinListFocusGet(&num);
   if (num <= 1)
      return;

   dir = (dir > 0) ? 1 : -1;

   for (j = 0; j < num; j++)
     {
	if (lst[j] == Mode.focuswin)
	   break;
     }
   for (i = 1; i < num; i++)
     {
	ewin = lst[(j + i * dir + num) % num];
	if (!FocusEwinValid(ewin, 1, 0, 0) || ewin->props.skip_focuslist)
	   continue;
	FocusToEWin(ewin, FOCUS_PREV);
	break;
     }
}

static void
ClickGrabsSet(EWin * ewin)
{
   int                 set = 0;

   if ((Conf.focus.clickraises && !EwinListStackIsRaised(ewin)) ||
       (!ewin->state.active && !ewin->state.inhibit_focus))
      set = 1;

   if (set)
     {
	if (!ewin->state.click_grab_isset)
	  {
	     GrabButtonSet(AnyButton, AnyModifier, EwinGetContainerWin(ewin),
			   ButtonPressMask, ECSR_PGRAB, 1);
	     if (EDebug(EDBUG_TYPE_GRABS))
		Eprintf("ClickGrabsSet: %#lx set %s\n",
			EwinGetClientXwin(ewin), EwinGetTitle(ewin));
	     ewin->state.click_grab_isset = 1;
	  }
     }
   else
     {
	if (ewin->state.click_grab_isset)
	  {
	     GrabButtonRelease(AnyButton, AnyModifier,
			       EwinGetContainerWin(ewin));
	     if (EDebug(EDBUG_TYPE_GRABS))
		Eprintf("ClickGrabsSet: %#lx unset %s\n",
			EwinGetClientXwin(ewin), EwinGetTitle(ewin));
	     ewin->state.click_grab_isset = 0;
	  }
     }
}

static void
FocusEwinSetActive(EWin * ewin, int active)
{
   if (ewin->state.active == (unsigned)active)
      return;

   ewin->state.active = active;
   EwinBorderUpdateState(ewin);
   EwinUpdateOpacity(ewin);

   if (active && ewin->state.attention)
     {
	ewin->state.attention = 0;
	HintsSetWindowState(ewin);
     }
}

static void
doClickGrabsUpdate(void)
{
   EWin               *const *lst, *ewin;
   int                 i, num;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	ClickGrabsSet(ewin);
     }
   click_pending_update_grabs = 0;
}

void
ClickGrabsUpdate(void)
{
   click_pending_update_grabs = 1;
}

static void
doFocusToEwin(EWin * ewin, int why)
{
   int                 do_focus = 0;
   int                 do_raise = 0, do_warp = 0;

   if (focus_inhibit)
      return;

   if (EDebug(EDBUG_TYPE_FOCUS))
      Eprintf("doFocusToEWin %#lx %s why=%d\n",
	      (ewin) ? EwinGetClientXwin(ewin) : 0,
	      (ewin) ? EwinGetTitle(ewin) : "None", why);

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
	if (ewin && ewin == Mode.focuswin)
	   return;
	if (ewin == NULL)	/* Unfocus */
	   break;
	if (!FocusEwinValid(ewin, 1, why == FOCUS_CLICK, 0))
	   return;
	break;

     case FOCUS_INIT:
     case FOCUS_DESK_ENTER:
	ewin = FocusEwinSelect();
	break;

     case FOCUS_DESK_LEAVE:
	focus_is_set = 0;
     case FOCUS_NONE:
	ewin = NULL;
	if (ewin == Mode.focuswin)
	   return;
	break;

     case FOCUS_EWIN_UNMAP:
	if (Mode.focuswin)
	   return;
	ewin = FocusEwinSelect();
	if (ewin == Mode.focuswin)
	   ewin = NULL;
	break;

     case FOCUS_EWIN_NEW:
	if (Conf.focus.all_new_windows_get_focus)
	   do_focus = 1;
	else if (Mode.place.doing_manual)
	   do_focus = 1;

	if (ewin->props.focus_when_mapped)
	   do_focus = 2;

	if (EwinIsTransient(ewin))
	  {
	     if (Conf.focus.new_transients_get_focus)
	       {
		  do_focus = 2;
	       }
	     else if (Conf.focus.new_transients_get_focus_if_group_focused)
	       {
		  EWin               *ewin2;

		  ewin2 = EwinFindByClient(EwinGetTransientFor(ewin));
		  if ((ewin2) && (Mode.focuswin == ewin2))
		     do_focus = 2;
	       }

	     if (do_focus == 2)
		DeskGotoByEwin(ewin);
	  }

	if (!do_focus)
	   return;
	if (!FocusEwinValid(ewin, 1, 0, 0))
	   return;
	break;
     }

   if (ewin == Mode.focuswin && focus_is_set)
      return;

   /* Check if ewin is a valid focus window target */

   if (!ewin)
      goto done;

   /* NB! ewin != NULL */

   if (why != FOCUS_CLICK && ewin->props.focusclick)
      return;

   if (Conf.autoraise.enable)
     {
	TIMER_DEL(focus_timer_autoraise);

	if (Conf.focus.mode != MODE_FOCUS_CLICK)
	   TIMER_ADD(focus_timer_autoraise, 0.001 * Conf.autoraise.delay,
		     AutoraiseTimeout, ewin);
     }

   if (do_raise)
      EwinRaise(ewin);

   if (Conf.focus.warp_always)
      do_warp = 1;
   if (do_warp)
      EwinWarpTo(ewin);

   switch (why)
     {
     case FOCUS_PREV:
     case FOCUS_NEXT:
	GrabKeyboardSet(VROOT);	/* Causes idler to be called on KeyRelease */
	focus_pending_raise = ewin;
	break;
     case FOCUS_DESK_ENTER:
	if (Conf.focus.mode == MODE_FOCUS_CLICK)
	   break;
     default:
     case FOCUS_INIT:
	EwinListFocusRaise(ewin);
	break;
     }

   SoundPlay("SOUND_FOCUS_SET");
 done:

   ClickGrabsUpdate();

   /* Unset old focus window (if any) highlighting */
   if (Mode.focuswin)
      FocusEwinSetActive(Mode.focuswin, 0);
   ICCCM_Cmap(ewin);

   /* Quit if pointer is not on our screen */
   if (!EQueryPointer(NULL, NULL, NULL, NULL, NULL))
     {
	Mode.focuswin = NULL;
	return;
     }

   /* Set new focus window (if any) highlighting */
   Mode.focuswin = ewin;
   if (Mode.focuswin)
      FocusEwinSetActive(Mode.focuswin, 1);

   if (why == FOCUS_DESK_LEAVE)
      return;

   ICCCM_Focus(ewin);
   focus_is_set = 1;
}

void
FocusToEWin(EWin * ewin, int why)
{
   if (EDebug(EDBUG_TYPE_FOCUS))
      Eprintf("FocusToEWin(%d) %#lx %s why=%d\n", focus_inhibit,
	      (ewin) ? EwinGetClientXwin(ewin) : 0,
	      (ewin) ? EwinGetTitle(ewin) : "None", why);

   switch (why)
     {
     case FOCUS_EWIN_NEW:
	if (!FocusEwinValid(ewin, 0, 0, 0))
	   break;
	focus_pending_new = ewin;
	focus_pending_why = why;
	focus_pending_ewin = ewin;
	break;

     default:
	if (ewin && !FocusEwinValid(ewin, 0, why == FOCUS_CLICK, 0))
	   break;
	focus_pending_why = why;
	focus_pending_ewin = ewin;
	break;

     case FOCUS_EWIN_UNMAP:
	focus_pending_why = why;
	focus_pending_ewin = NULL;
	if (ewin == Mode.focuswin)
	  {
	     Mode.focuswin = NULL;
	     focus_is_set = 0;
	     if (!EoIsGone(ewin))
		FocusEwinSetActive(ewin, 0);
	  }
	if (ewin == focus_pending_new)
	   focus_pending_new = NULL;
	break;
     }
}

static void
FocusSet(void)
{
   if (focus_pending_new && Conf.focus.all_new_windows_get_focus)
      doFocusToEwin(focus_pending_new, FOCUS_EWIN_NEW);
   else
      doFocusToEwin(focus_pending_ewin, focus_pending_why);
   focus_pending_why = 0;
   focus_pending_ewin = focus_pending_new = NULL;
}

void
FocusNewDeskBegin(void)
{
   /* Freeze keyboard */
   XGrabKeyboard(disp, WinGetXwin(VROOT), False, GrabModeAsync,
		 GrabModeSync, CurrentTime);

   focus_pending_new = NULL;
   doFocusToEwin(NULL, FOCUS_DESK_LEAVE);
}

void
FocusNewDesk(void)
{
   FocusToEWin(NULL, FOCUS_DESK_ENTER);

   /* Unfreeze keyboard */
   XUngrabKeyboard(disp, CurrentTime);
}

static void
FocusInit(void)
{
   /* Start focusing windows */
   FocusEnable(1);

   FocusToEWin(NULL, FOCUS_INIT);
   FocusSet();

   /* Enable window placement features */
   Mode.place.enable_features++;
}

static void
FocusExit(void)
{
}

/*
 * Focus event handlers
 */

void
FocusHandleEnter(EWin * ewin, XEvent * ev)
{
   Window              win = ev->xcrossing.window;

   Mode.mouse_over_ewin = ewin;

   if (!ewin)
     {
	/* Entering root may mean entering this screen */
	if (win == WinGetXwin(VROOT) &&
	    (ev->xcrossing.mode == NotifyNormal &&
	     ev->xcrossing.detail != NotifyInferior))
	  {
	     FocusToEWin(NULL, FOCUS_DESK_ENTER);
	     return;
	  }
     }

   if (ev->xcrossing.mode == NotifyUngrab &&
       ev->xcrossing.detail == NotifyNonlinearVirtual)
      return;

   switch (Conf.focus.mode)
     {
     default:
     case MODE_FOCUS_CLICK:
	break;
     case MODE_FOCUS_SLOPPY:
	if (FocusEwinValid(ewin, 1, 0, 0))
	   FocusToEWin(ewin, FOCUS_ENTER);
	break;
     case MODE_FOCUS_POINTER:
	if (!ewin || FocusEwinValid(ewin, 1, 0, 0))
	   FocusToEWin(ewin, FOCUS_ENTER);
	break;
     }
}

void
FocusHandleLeave(EWin * ewin __UNUSED__, XEvent * ev)
{
   Window              win = ev->xcrossing.window;

   /* Leaving root may mean entering other screen */
   if (win == WinGetXwin(VROOT) &&
       (ev->xcrossing.mode == NotifyNormal &&
	ev->xcrossing.detail != NotifyInferior))
      FocusToEWin(NULL, FOCUS_DESK_LEAVE);
}

void
FocusHandleChange(EWin * ewin __UNUSED__, XEvent * ev __UNUSED__)
{
#if 0				/* Debug */
   if (ewin == Mode.focuswin && ev->type == FocusOut)
      Eprintf("??? Lost focus: %s\n", EwinGetTitle(ewin));
#endif
}

void
FocusHandleClick(EWin * ewin, Win win)
{
   if (Conf.focus.clickraises)
      EwinRaise(ewin);

   FocusToEWin(ewin, FOCUS_CLICK);

   /* Allow click to pass thorugh */
   if (EDebug(EDBUG_TYPE_GRABS))
      Eprintf("FocusHandleClick %#lx %#lx\n", WinGetXwin(win),
	      EwinGetContainerXwin(ewin));
   if (win == EwinGetContainerWin(ewin))
     {
	ESync(ESYNC_FOCUS);
	XAllowEvents(disp, ReplayPointer, CurrentTime);
	ESync(ESYNC_FOCUS);
     }
}

/*      
 * Configuration dialog
 */
static int          tmp_focus;
static char         tmp_clickalways;
static char         tmp_new_focus;
static char         tmp_popup_focus;
static char         tmp_owner_popup_focus;
static char         tmp_raise_focus;
static char         tmp_warp_focus;
static char         tmp_warp_always;

static char         tmp_display_warp;
static char         tmp_warp_after_focus;
static char         tmp_raise_after_focus;
static char         tmp_showsticky;
static char         tmp_showshaded;
static char         tmp_showiconified;
static char         tmp_showalldesks;
static char         tmp_warpfocused;
static int          tmp_warp_icon_mode;

static void
CB_ConfigureFocus(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.focus.mode = tmp_focus;
	Conf.focus.all_new_windows_get_focus = tmp_new_focus;
	Conf.focus.new_transients_get_focus = tmp_popup_focus;
	Conf.focus.new_transients_get_focus_if_group_focused =
	   tmp_owner_popup_focus;
	Conf.focus.raise_on_next = tmp_raise_focus;
	Conf.focus.warp_on_next = tmp_warp_focus;
	Conf.focus.warp_always = tmp_warp_always;

	Conf.warplist.enable = tmp_display_warp;
	Conf.warplist.warp_on_select = tmp_warp_after_focus;
	Conf.warplist.raise_on_select = tmp_raise_after_focus;
	Conf.warplist.showsticky = tmp_showsticky;
	Conf.warplist.showshaded = tmp_showshaded;
	Conf.warplist.showiconified = tmp_showiconified;
	Conf.warplist.showalldesks = tmp_showalldesks;
	Conf.warplist.warpfocused = tmp_warpfocused;
	Conf.warplist.icon_mode = tmp_warp_icon_mode;

	Conf.focus.clickraises = tmp_clickalways;
	ClickGrabsUpdate();
     }
   autosave();
}

static void
_DlgFillFocus(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *radio, *radio2;

   tmp_focus = Conf.focus.mode;
   tmp_new_focus = Conf.focus.all_new_windows_get_focus;
   tmp_popup_focus = Conf.focus.new_transients_get_focus;
   tmp_owner_popup_focus = Conf.focus.new_transients_get_focus_if_group_focused;
   tmp_raise_focus = Conf.focus.raise_on_next;
   tmp_warp_focus = Conf.focus.warp_on_next;
   tmp_warp_always = Conf.focus.warp_always;

   tmp_raise_after_focus = Conf.warplist.raise_on_select;
   tmp_warp_after_focus = Conf.warplist.warp_on_select;
   tmp_display_warp = Conf.warplist.enable;
   tmp_showsticky = Conf.warplist.showsticky;
   tmp_showshaded = Conf.warplist.showshaded;
   tmp_showiconified = Conf.warplist.showiconified;
   tmp_showalldesks = Conf.warplist.showalldesks;
   tmp_warpfocused = Conf.warplist.warpfocused;
   tmp_warp_icon_mode = Conf.warplist.icon_mode;

   tmp_clickalways = Conf.focus.clickraises;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Focus follows pointer"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Focus follows pointer sloppily"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Focus follows mouse clicks"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_focus);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Clicking in a window always raises it"));
   DialogItemCheckButtonSetPtr(di, &tmp_clickalways);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("All new windows first get the focus"));
   DialogItemCheckButtonSetPtr(di, &tmp_new_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Only new dialog windows get the focus"));
   DialogItemCheckButtonSetPtr(di, &tmp_popup_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _
		     ("Only new dialogs whose owner is focused get the focus"));
   DialogItemCheckButtonSetPtr(di, &tmp_owner_popup_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raise windows while switching focus"));
   DialogItemCheckButtonSetPtr(di, &tmp_raise_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _("Send mouse pointer to window while switching focus"));
   DialogItemCheckButtonSetPtr(di, &tmp_warp_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _("Always send mouse pointer to window on focus switch"));
   DialogItemCheckButtonSetPtr(di, &tmp_warp_always);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Display and use focus list"));
   DialogItemCheckButtonSetPtr(di, &tmp_display_warp);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Include sticky windows in focus list"));
   DialogItemCheckButtonSetPtr(di, &tmp_showsticky);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Include shaded windows in focus list"));
   DialogItemCheckButtonSetPtr(di, &tmp_showshaded);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Include iconified windows in focus list"));
   DialogItemCheckButtonSetPtr(di, &tmp_showiconified);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Include windows on other desks in focus list"));
   DialogItemCheckButtonSetPtr(di, &tmp_showalldesks);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Focus windows while switching"));
   DialogItemCheckButtonSetPtr(di, &tmp_warpfocused);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raise windows after focus switch"));
   DialogItemCheckButtonSetPtr(di, &tmp_raise_after_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Send mouse pointer to window after focus switch"));
   DialogItemCheckButtonSetPtr(di, &tmp_warp_after_focus);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _
		     ("Focuslist image display policy (if one operation fails, try the next):"));

   radio2 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("First E Icon, then App Icon"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("First App Icon, then E Icon"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 4);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("None"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 0);
   DialogItemRadioButtonGroupSetValPtr(radio2, &tmp_warp_icon_mode);
}

const DialogDef     DlgFocus = {
   "CONFIGURE_FOCUS",
   N_("Focus"),
   N_("Focus Settings"),
   "SOUND_SETTINGS_FOCUS",
   "pix/focus.png",
   N_("Enlightenment Focus\n" "Settings Dialog\n"),
   _DlgFillFocus,
   DLG_OAC, CB_ConfigureFocus,
};

/*
 * Focus Module
 */

static int
FocusInitTimeout(void *data __UNUSED__)
{
   FocusInit();
   return 0;
}

static void
_FocusIdler(void *data __UNUSED__)
{
   if (!focus_inhibit && focus_pending_why)
      FocusSet();
   if (click_pending_update_grabs)
      doClickGrabsUpdate();
   if (focus_pending_raise)
      FocusRaisePending();
}

static void
FocusSighan(int sig, void *prm __UNUSED__)
{
   Timer              *focus_init_timer;

   switch (sig)
     {
     case ESIGNAL_START:
	/* Delay focusing a bit to allow things to settle down */
	IdlerAdd(_FocusIdler, NULL);
	TIMER_ADD(focus_init_timer, 0.5, FocusInitTimeout, NULL);
	break;

     case ESIGNAL_EXIT:
	FocusExit();
	break;
     }
}

static void
FocusIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	EWin               *ewin;

	ewin = GetFocusEwin();
	if (ewin)
	   IpcPrintf("Focused: %#lx\n", EwinGetClientXwin(ewin));
	else
	   IpcPrintf("Focused: none\n");
     }
   else if (!strncmp(cmd, "mode", 2))
     {
	int                 mode = Conf.focus.mode;

	if (!strcmp(prm, "click"))
	  {
	     mode = MODE_FOCUS_CLICK;
	     Mode.grabs.pointer_grab_active = 1;
	  }
	else if (!strcmp(prm, "clicknograb"))
	  {
	     mode = MODE_FOCUS_CLICK;
	     Mode.grabs.pointer_grab_active = 0;
	  }
	else if (!strcmp(prm, "pointer"))
	  {
	     mode = MODE_FOCUS_POINTER;
	  }
	else if (!strcmp(prm, "sloppy"))
	  {
	     mode = MODE_FOCUS_SLOPPY;
	  }
	else if (!strcmp(prm, "?"))
	  {
	     if (Conf.focus.mode == MODE_FOCUS_CLICK)
	       {
		  if (Mode.grabs.pointer_grab_active)
		     p = "click";
		  else
		     p = "clicknograb";
	       }
	     else if (Conf.focus.mode == MODE_FOCUS_SLOPPY)
		p = "sloppy";
	     else if (Conf.focus.mode == MODE_FOCUS_POINTER)
		p = "pointer";
	     else
		p = "unknown";
	     IpcPrintf("Focus Mode: %s\n", p);
	  }
	else
	  {
	     IpcPrintf("Error: unknown focus type\n");
	  }
	if (Conf.focus.mode != mode)
	  {
	     Conf.focus.mode = mode;
	     ClickGrabsUpdate();
	     autosave();
	  }
     }
   else if (!strncmp(cmd, "next", 2))
     {
	/* Focus previously focused window */
	if (Conf.warplist.enable)
	   WarpFocus(1);
	else
	   FocusCycleEwin(1);
     }
   else if (!strncmp(cmd, "prev", 2))
     {
	/* Focus least recently focused window */
	if (Conf.warplist.enable)
	   WarpFocus(-1);
	else
	   FocusCycleEwin(-1);
     }
}

static const IpcItem FocusIpcArray[] = {
   {
    FocusIpc,
    "focus", "sf",
    "Focus functions",
    "  focus ?                     Show focus info\n"
    "  focus mode                  Set focus mode. Modes:\n"
    "    click:       The traditional click-to-focus mode.\n"
    "    clicknograb: A similar focus mode, but without the grabbing of the click\n"
    "      (you cannot click anywhere in a window to focus it)\n"
    "    pointer:     The focus will follow the mouse pointer\n"
    "    sloppy:      The focus follows the mouse, but when over the desktop background\n"
    "                 the last window does not lose the focus\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(FocusIpcArray)/sizeof(IpcItem))

static const CfgItem FocusCfgItems[] = {
   CFG_ITEM_INT(Conf.focus, mode, MODE_FOCUS_SLOPPY),
   CFG_ITEM_BOOL(Conf.focus, clickraises, 1),
   CFG_ITEM_BOOL(Conf.focus, transientsfollowleader, 1),
   CFG_ITEM_BOOL(Conf.focus, switchfortransientmap, 1),
   CFG_ITEM_BOOL(Conf.focus, all_new_windows_get_focus, 0),
   CFG_ITEM_BOOL(Conf.focus, new_transients_get_focus, 0),
   CFG_ITEM_BOOL(Conf.focus, new_transients_get_focus_if_group_focused, 1),
   CFG_ITEM_BOOL(Conf.focus, raise_on_next, 1),
   CFG_ITEM_BOOL(Conf.focus, warp_on_next, 0),
   CFG_ITEM_BOOL(Conf.focus, warp_always, 0),

   CFG_ITEM_BOOL(Conf, autoraise.enable, 0),
   CFG_ITEM_INT(Conf, autoraise.delay, 500),
};
#define N_CFG_ITEMS (sizeof(FocusCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModFocus;
const EModule       ModFocus = {
   "focus", NULL,
   FocusSighan,
   {N_IPC_FUNCS, FocusIpcArray},
   {N_CFG_ITEMS, FocusCfgItems}
};
