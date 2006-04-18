/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "desktops.h"
#include "e16-ecore_hints.h"
#include "ewins.h"
#include "hints.h"
#include "xwin.h"
#if USE_XSYNC
#include <X11/extensions/sync.h>
#endif

#undef USE_XSYNC		/* No - Not sure this is safe */

static void         ICCCM_SetIconSizes(void);

void
ICCCM_Init(void)
{
#ifndef USE_ECORE_X
   ecore_x_icccm_init();
#endif

   ICCCM_SetIconSizes();

   if (Mode.wm.window)
     {
	Atom                wm_props[1];

	wm_props[0] = ECORE_X_ATOM_WM_DELETE_WINDOW;
	XSetWMProtocols(disp, VRoot.win, wm_props, 1);
     }
}

void
ICCCM_ProcessClientMessage(XClientMessageEvent * event)
{
   EWin               *ewin;
   Atom                a;

   if (event->message_type == ECORE_X_ATOM_WM_CHANGE_STATE)
     {
	ewin = EwinFindByClient(event->window);
	if (ewin == NULL)
	   return;

	if (event->data.l[0] == IconicState)
	  {
	     EwinIconify(ewin);
	  }
     }
   else if (event->message_type == ECORE_X_ATOM_WM_PROTOCOLS)
     {
	a = event->data.l[0];
	if (a == ECORE_X_ATOM_WM_DELETE_WINDOW && event->window == VRoot.win)
	   SessionExit(EEXIT_EXIT, NULL);
     }
}

void
ICCCM_GetTitle(EWin * ewin, Atom atom_change)
{
   if (atom_change && atom_change != ECORE_X_ATOM_WM_NAME)
      return;

   _EFREE(ewin->icccm.wm_name);

   ewin->icccm.wm_name = ecore_x_icccm_title_get(_EwinGetClientXwin(ewin));

   EwinChange(ewin, EWIN_CHANGE_NAME);
}

void
ICCCM_GetColormap(EWin * ewin)
{
   XWindowAttributes   xwa;

   ewin->client.cmap = None;
   if (XGetWindowAttributes(disp, _EwinGetClientXwin(ewin), &xwa)
       && xwa.colormap)
      ewin->client.cmap = xwa.colormap;
}

void
ICCCM_Delete(const EWin * ewin)
{
   if (EwinIsInternal(ewin))
     {
	HideEwin((EWin *) ewin);
	return;
     }

   if (ewin->icccm.delete_window)
      ecore_x_icccm_delete_window_send(_EwinGetClientXwin(ewin), CurrentTime);
   else
      XKillClient(disp, _EwinGetClientXwin(ewin));
}

#if 0				/* Deprecated */
void
ICCCM_Save(const EWin * ewin)
{
   if (EwinIsInternal(ewin))
      return;

   ecore_x_icccm_send_save_yourself(_EwinGetClientXwin(ewin));
}
#endif

void
ICCCM_Iconify(const EWin * ewin)
{
   EUnmapWindow(_EwinGetClientWin(ewin));
   ecore_x_icccm_state_set_iconic(_EwinGetClientXwin(ewin));
}

void
ICCCM_DeIconify(const EWin * ewin)
{
   EMapWindow(_EwinGetClientWin(ewin));
   ecore_x_icccm_state_set_normal(_EwinGetClientXwin(ewin));
}

void
ICCCM_Withdraw(const EWin * ewin)
{
   /* We have a choice of deleting the WM_STATE property
    * or changing the value to Withdrawn. Since twm/fvwm does
    * it that way, we change it to Withdrawn.
    */
   ecore_x_icccm_state_set_withdrawn(_EwinGetClientXwin(ewin));

   XRemoveFromSaveSet(disp, _EwinGetClientXwin(ewin));
}

void
ICCCM_SizeMatch(const EWin * ewin, int wi, int hi, int *pwo, int *pho)
{
   int                 w, h;
   int                 i, j;
   double              aspect;

   w = wi;
   h = hi;

   if (w < ewin->icccm.width.min)
      w = ewin->icccm.width.min;
   if (w > ewin->icccm.width.max)
      w = ewin->icccm.width.max;
   if (h < ewin->icccm.height.min)
      h = ewin->icccm.height.min;
   if (h > ewin->icccm.height.max)
      h = ewin->icccm.height.max;
   if ((w > 0) && (h > 0))
     {
	w -= ewin->icccm.base_w;
	h -= ewin->icccm.base_h;
	if ((w > 0) && (h > 0))
	  {
	     aspect = ((double)w) / ((double)h);
	     if (Mode.mode == MODE_RESIZE_H)
	       {
		  if (aspect < ewin->icccm.aspect_min)
		     h = (int)((double)w / ewin->icccm.aspect_min);
		  if (aspect > ewin->icccm.aspect_max)
		     h = (int)((double)w / ewin->icccm.aspect_max);
	       }
	     else if (Mode.mode == MODE_RESIZE_V)
	       {
		  if (aspect < ewin->icccm.aspect_min)
		     w = (int)((double)h * ewin->icccm.aspect_min);
		  if (aspect > ewin->icccm.aspect_max)
		     w = (int)((double)h * ewin->icccm.aspect_max);
	       }
	     else
	       {
		  if (aspect < ewin->icccm.aspect_min)
		     w = (int)((double)h * ewin->icccm.aspect_min);
		  if (aspect > ewin->icccm.aspect_max)
		     h = (int)((double)w / ewin->icccm.aspect_max);
	       }
	     i = (w + ewin->icccm.w_inc / 2) / ewin->icccm.w_inc;
	     j = (h + ewin->icccm.h_inc / 2) / ewin->icccm.h_inc;
	     w = i * ewin->icccm.w_inc;
	     h = j * ewin->icccm.h_inc;
	  }
	w += ewin->icccm.base_w;
	h += ewin->icccm.base_h;
     }

   *pwo = w;
   *pho = h;
}

void
ICCCM_MatchSize(EWin * ewin)
{
   ICCCM_SizeMatch(ewin, ewin->client.w, ewin->client.h, &ewin->client.w,
		   &ewin->client.h);
}

void
ICCCM_GetIncrementalSize(EWin * ewin, unsigned int w, unsigned int h,
			 unsigned int *wi, unsigned int *hi)
{
   *wi = (w - ewin->icccm.base_w) / ewin->icccm.w_inc;
   *hi = (h - ewin->icccm.base_h) / ewin->icccm.h_inc;
}

void
ICCCM_SetSizeConstraints(EWin * ewin, unsigned int wmin, unsigned int hmin,
			 unsigned int wmax, unsigned int hmax,
			 unsigned int wbase, unsigned int hbase,
			 unsigned int winc, unsigned int hinc,
			 double amin, double amax)
{
   ewin->icccm.width.min = wmin;
   ewin->icccm.height.min = hmin;
   ewin->icccm.width.max = wmax;
   ewin->icccm.height.max = hmax;

   ewin->icccm.base_w = wbase;
   ewin->icccm.base_h = hbase;
   ewin->icccm.w_inc = winc;
   ewin->icccm.h_inc = hinc;

   ewin->icccm.aspect_min = amin;
   ewin->icccm.aspect_max = amax;

   ewin->props.no_resize_h = (wmin == wmax);
   ewin->props.no_resize_v = (hmin == hmax);

   ewin->icccm.grav = NorthWestGravity;
}

void
ICCCM_Configure(EWin * ewin)
{
   XEvent              ev;
   Window              child;

   if (EwinIsInternal(ewin))
      return;

#if USE_XSYNC
   if (ewin->ewmh.sync_request_enable && !EServerIsGrabbed())
     {
	long long           count;

	count = ++ewin->ewmh.sync_request_count;

	if (count == 0)
	   ewin->ewmh.sync_request_count = ++count;
	ecore_x_client_message32_send(_EwinGetClientXwin(ewin),
				      ECORE_X_ATOM_WM_PROTOCOLS,
				      StructureNotifyMask,
				      ECORE_X_ATOM_NET_WM_SYNC_REQUEST,
				      Mode.events.time,
				      count & 0xffffffff, count >> 32, 0);
     }
#endif

   ev.type = ConfigureNotify;
   ev.xconfigure.display = disp;
   ev.xconfigure.event = _EwinGetClientXwin(ewin);
   ev.xconfigure.window = _EwinGetClientXwin(ewin);
#if 0				/* FIXME - Remove? */
   Desk               *dsk;

   dsk = EoGetDesk(ewin);
   ev.xconfigure.x = EoGetX(dsk) + ewin->client.x;
   ev.xconfigure.y = EoGetY(dsk) + ewin->client.y;
#else
   ev.xconfigure.x = ewin->client.x;
   ev.xconfigure.y = ewin->client.y;
#endif
   if (Mode.wm.window)
      XTranslateCoordinates(disp, VRoot.win, RRoot.win,
			    ev.xconfigure.x, ev.xconfigure.y,
			    &ev.xconfigure.x, &ev.xconfigure.y, &child);
   ev.xconfigure.width = ewin->client.w;
   ev.xconfigure.height = ewin->client.h;
   ev.xconfigure.border_width = 0;
   ev.xconfigure.above = EoGetXwin(ewin);
   ev.xconfigure.override_redirect = False;
   XSendEvent(disp, _EwinGetClientXwin(ewin), False, StructureNotifyMask, &ev);

#if USE_XSYNC
   if (ewin->ewmh.sync_request_enable && !EServerIsGrabbed())
     {
	XSyncWaitCondition  xswc[1];

	xswc[0].trigger.counter = ewin->ewmh.sync_request_counter;
	xswc[0].trigger.value_type = XSyncAbsolute;
	XSyncIntsToValue(&xswc[0].trigger.wait_value,
			 ewin->ewmh.sync_request_count & 0xffffffff,
			 ewin->ewmh.sync_request_count >> 32);
	xswc[0].trigger.test_type = XSyncPositiveComparison;
	XSyncIntsToValue(&xswc[0].event_threshold, 0, 0);
	Eprintf("Sync t=%#lx c=%llx\n", xswc[0].trigger.counter,
		ewin->ewmh.sync_request_count);
	XSyncAwait(disp, xswc, 1);
     }
#endif
}

void
ICCCM_AdoptStart(const EWin * ewin)
{
   Window              win = _EwinGetClientXwin(ewin);

   if (!EwinIsInternal(ewin))
      XAddToSaveSet(disp, win);
}

void
ICCCM_Adopt(const EWin * ewin)
{
   Window              win = _EwinGetClientXwin(ewin);

   if (ewin->icccm.start_iconified)
      ecore_x_icccm_state_set_iconic(win);
   else
      ecore_x_icccm_state_set_normal(win);
}

void
ICCCM_Cmap(EWin * ewin)
{
   if (!ewin)
     {
	if (Mode.current_cmap)
	  {
	     XUninstallColormap(disp, Mode.current_cmap);
	     Mode.current_cmap = 0;
	  }
	return;
     }

   if (EwinIsInternal(ewin))
      return;

   ICCCM_GetColormap(ewin);

   if ((ewin->client.cmap) && (Mode.current_cmap != ewin->client.cmap))
     {
	XWindowAttributes   xwa;
	int                 i, num;
	Ecore_X_Window     *wlist;

	num = ecore_x_window_prop_window_list_get(_EwinGetClientXwin(ewin),
						  ECORE_X_ATOM_WM_COLORMAP_WINDOWS,
						  &wlist);
	if (num > 0)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (XGetWindowAttributes(disp, wlist[i], &xwa))
		    {
		       if (xwa.colormap != DefaultColormap(disp, VRoot.scr))
			 {
			    XInstallColormap(disp, xwa.colormap);
			    Mode.current_cmap = xwa.colormap;
			 }
		    }
	       }
	     Efree(wlist);
	     return;
	  }
	XInstallColormap(disp, ewin->client.cmap);
	Mode.current_cmap = ewin->client.cmap;
     }
}

void
ICCCM_Focus(const EWin * ewin)
{
   if (EventDebug(EDBUG_TYPE_FOCUS))
     {
	if (ewin)
	   Eprintf("ICCCM_Focus T=%#lx %#lx %s\n", Mode.events.time,
		   _EwinGetClientXwin(ewin), EwinGetName(ewin));
	else
	   Eprintf("ICCCM_Focus None T=%#lx\n", Mode.events.time);
     }

   if (!ewin)
     {
	XSetInputFocus(disp, VRoot.win, RevertToPointerRoot, Mode.events.time);
	HintsSetActiveWindow(None);
	return;
     }

   if (ewin->icccm.take_focus)
     {
	ecore_x_icccm_take_focus_send(_EwinGetClientXwin(ewin),
				      Mode.events.time);
     }

   XSetInputFocus(disp, _EwinGetClientXwin(ewin), RevertToPointerRoot,
		  Mode.events.time);

   HintsSetActiveWindow(_EwinGetClientXwin(ewin));
}

void
ICCCM_GetGeoms(EWin * ewin, Atom atom_change)
{
   XSizeHints          hint;
   long                mask;

   if (atom_change && atom_change != ECORE_X_ATOM_WM_NORMAL_HINTS)
      return;

   if (XGetWMNormalHints(disp, _EwinGetClientXwin(ewin), &hint, &mask))
     {
	if (!(ewin->state.placed))
	  {
	     if ((hint.flags & USPosition) || ((hint.flags & PPosition)))
	       {
		  if (hint.flags & PWinGravity)
		     ewin->icccm.grav = hint.win_gravity;
		  else
		     ewin->icccm.grav = NorthWestGravity;
		  ewin->client.grav = ewin->icccm.grav;

		  if ((hint.flags & PPosition) && (!EoIsSticky(ewin)))
		    {
		       Desk               *dsk;

		       dsk = EoGetDesk(ewin);
		       if (!dsk)
			  dsk = DesksGetCurrent();
		       ewin->client.x -= EoGetX(dsk);
		       ewin->client.y -= EoGetY(dsk);
		       if (ewin->client.x + ewin->client.w >= VRoot.w)
			 {
			    ewin->client.x += EoGetX(dsk);
			 }
		       else if (ewin->client.x < 0)
			 {
			    ewin->client.x += EoGetX(dsk);
			 }
		       if (ewin->client.y + ewin->client.h >= VRoot.h)
			 {
			    ewin->client.y += EoGetY(dsk);
			 }
		       else if (ewin->client.y < 0)
			 {
			    ewin->client.y += EoGetY(dsk);
			 }
		    }
		  ewin->state.placed = 1;
	       }
	  }

	if (hint.flags & PMinSize)
	  {
	     ewin->icccm.width.min = MAX(0, hint.min_width);
	     ewin->icccm.height.min = MAX(0, hint.min_height);
	  }
	else
	  {
	     ewin->icccm.width.min = 0;
	     ewin->icccm.height.min = 0;
	  }

	if (hint.flags & PMaxSize)
	  {
	     ewin->icccm.width.max = MAX(hint.max_width, ewin->client.w);
	     ewin->icccm.height.max = MAX(hint.max_height, ewin->client.h);
	  }
	else
	  {
	     ewin->icccm.width.max = 65535;
	     ewin->icccm.height.max = 65535;
	  }

	if (hint.flags & PResizeInc)
	  {
	     ewin->icccm.w_inc = MAX(1, hint.width_inc);
	     ewin->icccm.h_inc = MAX(1, hint.height_inc);
	  }
	else
	  {
	     ewin->icccm.w_inc = 1;
	     ewin->icccm.h_inc = 1;
	  }

	if (hint.flags & PAspect)
	  {
	     if ((hint.min_aspect.y > 0.0) && (hint.min_aspect.x > 0.0))
	       {
		  ewin->icccm.aspect_min =
		     ((double)hint.min_aspect.x) / ((double)hint.min_aspect.y);
	       }
	     else
	       {
		  ewin->icccm.aspect_min = 0.0;
	       }
	     if ((hint.max_aspect.y > 0.0) && (hint.max_aspect.x > 0.0))
	       {
		  ewin->icccm.aspect_max =
		     ((double)hint.max_aspect.x) / ((double)hint.max_aspect.y);
	       }
	     else
	       {
		  ewin->icccm.aspect_max = 65535.0;
	       }
	  }
	else
	  {
	     ewin->icccm.aspect_min = 0.0;
	     ewin->icccm.aspect_max = 65535.0;
	  }

	if (hint.flags & PBaseSize)
	  {
	     ewin->icccm.base_w = hint.base_width;
	     ewin->icccm.base_h = hint.base_height;
	  }
	else
	  {
	     ewin->icccm.base_w = ewin->icccm.width.min;
	     ewin->icccm.base_h = ewin->icccm.height.min;
	  }

	if (ewin->icccm.width.min < ewin->icccm.base_w)
	   ewin->icccm.width.min = ewin->icccm.base_w;
	if (ewin->icccm.height.min < ewin->icccm.base_h)
	   ewin->icccm.height.min = ewin->icccm.base_h;
     }

   ewin->props.no_resize_h = (ewin->icccm.width.min == ewin->icccm.width.max);
   ewin->props.no_resize_v = (ewin->icccm.height.min == ewin->icccm.height.max);

   if (EventDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap get icccm %#lx: %4d+%4d %4dx%4d: %s\n",
	      _EwinGetClientXwin(ewin), ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetName(ewin));
}

#define TryGroup(e) (((e)->icccm.group != None) && ((e)->icccm.group != _EwinGetClientXwin(e)))

void
ICCCM_GetInfo(EWin * ewin, Atom atom_change)
{
   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_CLASS)
     {
	XClassHint          hint;

	_EFREE(ewin->icccm.wm_res_name);
	_EFREE(ewin->icccm.wm_res_class);

	if (XGetClassHint(disp, _EwinGetClientXwin(ewin), &hint) ||
	    (TryGroup(ewin) && XGetClassHint(disp, ewin->icccm.group, &hint)))
	  {
	     ewin->icccm.wm_res_name = Estrdup(hint.res_name);
	     ewin->icccm.wm_res_class = Estrdup(hint.res_class);
	     XFree(hint.res_name);
	     XFree(hint.res_class);
	  }
     }

   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_COMMAND)
     {
	int                 argc;
	char              **argv, s[4096], *ss;

	_EFREE(ewin->icccm.wm_command);

	argc = ecore_x_window_prop_string_list_get(_EwinGetClientXwin(ewin),
						   ECORE_X_ATOM_WM_COMMAND,
						   &argv);
	if ((argc < 0) && TryGroup(ewin))
	   argc = ecore_x_window_prop_string_list_get(ewin->icccm.group,
						      ECORE_X_ATOM_WM_COMMAND,
						      &argv);

	ss = StrlistEncodeEscaped(s, sizeof(s), argv, argc);
	ewin->icccm.wm_command = Estrdup(ss);
	StrlistFree(argv, argc);
     }

   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_CLIENT_MACHINE)
     {
	_EFREE(ewin->icccm.wm_machine);

	ewin->icccm.wm_machine =
	   ecore_x_window_prop_string_get(_EwinGetClientXwin(ewin),
					  ECORE_X_ATOM_WM_CLIENT_MACHINE);
	if (!ewin->icccm.wm_machine && TryGroup(ewin))
	   ewin->icccm.wm_machine =
	      ecore_x_window_prop_string_get(ewin->icccm.group,
					     ECORE_X_ATOM_WM_CLIENT_MACHINE);
     }

   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_ICON_NAME)
     {
	_EFREE(ewin->icccm.wm_icon_name);

	ewin->icccm.wm_icon_name =
	   ecore_x_window_prop_string_get(_EwinGetClientXwin(ewin),
					  ECORE_X_ATOM_WM_ICON_NAME);
	if (!ewin->icccm.wm_icon_name && TryGroup(ewin))
	   ewin->icccm.wm_icon_name =
	      ecore_x_window_prop_string_get(ewin->icccm.group,
					     ECORE_X_ATOM_WM_ICON_NAME);
     }

   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_WINDOW_ROLE)
     {
	_EFREE(ewin->icccm.wm_role);
	ewin->icccm.wm_role =
	   ecore_x_window_prop_string_get(_EwinGetClientXwin(ewin),
					  ECORE_X_ATOM_WM_WINDOW_ROLE);
     }
}

void
ICCCM_GetHints(EWin * ewin, Atom atom_change)
{
   XWMHints           *hint;
   Window              win;
   Atom               *prop;
   int                 i, num;

   hint = NULL;
   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_HINTS)
      hint = XGetWMHints(disp, _EwinGetClientXwin(ewin));
   if (hint)
     {
	/* I have to make sure the thing i'm docking is a dock app */
	if ((hint->flags & StateHint)
	    && (hint->initial_state == WithdrawnState))
	  {
	     if (hint->flags & (StateHint | IconWindowHint | IconPositionHint |
				WindowGroupHint))
	       {
		  if ((hint->icon_x == 0) && (hint->icon_y == 0)
		      && hint->window_group == _EwinGetClientXwin(ewin))
		     ewin->state.docked = 1;
	       }
	  }

	ewin->icccm.need_input =
	   ((hint->flags & InputHint) && (!hint->input)) ? 0 : 1;

	ewin->icccm.start_iconified =
	   ((hint->flags & StateHint) &&
	    (hint->initial_state == IconicState)) ? 1 : 0;

	if (hint->flags & IconPixmapHint)
	  {
	     if (ewin->icccm.icon_pmap != hint->icon_pixmap)
	       {
		  ewin->icccm.icon_pmap = hint->icon_pixmap;
		  EwinChange(ewin, EWIN_CHANGE_ICON_PMAP);
	       }
	  }
	else
	  {
	     ewin->icccm.icon_pmap = None;
	  }

	ewin->icccm.icon_mask =
	   (hint->flags & IconMaskHint) ? hint->icon_mask : None;

	ewin->icccm.icon_win =
	   (hint->flags & IconWindowHint) ? hint->icon_window : None;

	ewin->icccm.group =
	   (hint->flags & WindowGroupHint) ? hint->window_group : None;

	if (hint->flags & XUrgencyHint)
	  {
	     if (!ewin->state.attention)
		EwinChange(ewin, EWIN_CHANGE_ATTENTION);
	     ewin->icccm.urgency = 1;
	     ewin->state.attention = 1;
	  }
	else
	  {
	     ewin->icccm.urgency = 0;
	  }

	XFree(hint);
     }

   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_PROTOCOLS)
     {
	if (XGetWMProtocols(disp, _EwinGetClientXwin(ewin), &prop, &num))
	  {
	     ewin->icccm.take_focus = 0;
	     ewin->icccm.delete_window = 0;
	     for (i = 0; i < num; i++)
	       {
		  if (prop[i] == ECORE_X_ATOM_WM_TAKE_FOCUS)
		     ewin->icccm.take_focus = ewin->icccm.need_input = 1;
		  else if (prop[i] == ECORE_X_ATOM_WM_DELETE_WINDOW)
		     ewin->icccm.delete_window = 1;
#if USE_XSYNC
		  else if (prop[i] == ECORE_X_ATOM_NET_WM_SYNC_REQUEST)
		    {
		       unsigned int        c;

		       ewin->ewmh.sync_request_enable = 1;
		       ecore_x_window_prop_card32_get(_EwinGetClientXwin(ewin),
						      ECORE_X_ATOM_NET_WM_SYNC_REQUEST_COUNTER,
						      &c, 1);
		       ewin->ewmh.sync_request_counter = c;
		    }
#endif
	       }
	     XFree(prop);
	  }
     }

   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_TRANSIENT_FOR)
     {
	ewin->icccm.transient = 0;
	ewin->icccm.transient_for = None;
	if (XGetTransientForHint(disp, _EwinGetClientXwin(ewin), &win))
	  {
	     ewin->icccm.transient = 1;
	     ewin->icccm.transient_for = win;
	  }
     }

   if (ewin->icccm.group == _EwinGetClientXwin(ewin))
     {
	ewin->icccm.is_group_leader = 1;
     }
   else
     {
	ewin->icccm.is_group_leader = 0;
     }

   if (atom_change == 0 || atom_change == ECORE_X_ATOM_WM_CLIENT_LEADER)
     {
	Ecore_X_Window      cleader;

	num = ecore_x_window_prop_window_get(_EwinGetClientXwin(ewin),
					     ECORE_X_ATOM_WM_CLIENT_LEADER,
					     &cleader, 1);
	if (num > 0)
	  {
	     ewin->icccm.client_leader = cleader;
	     if (!ewin->icccm.group)
		ewin->icccm.group = cleader;
	  }
     }
}

void
ICCCM_SetIconSizes(void)
{
   XIconSize          *is;

   is = XAllocIconSize();
   is->min_width = 8;
   is->min_height = 8;
   is->max_width = 48;
   is->max_height = 48;
   is->width_inc = 1;
   is->height_inc = 1;
   XSetIconSizes(disp, VRoot.win, is, 1);
   XFree(is);
}

/*
 * Process received window property change
 */
void
ICCCM_ProcessPropertyChange(EWin * ewin, Atom atom_change)
{
   ICCCM_GetTitle(ewin, atom_change);
   ICCCM_GetHints(ewin, atom_change);
   ICCCM_GetInfo(ewin, atom_change);
   ICCCM_Cmap(ewin);
   ICCCM_GetGeoms(ewin, atom_change);
}
