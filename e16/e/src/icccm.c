/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
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
#include "ecore-e16.h"
#include "ewins.h"
#include "xwin.h"

void
ICCCM_Init(void)
{
#ifndef USE_ECORE_X
   ecore_x_icccm_init();
#endif

   ICCCM_SetIconSizes();

   if (Mode.wm.window)
     {
	Atom                wm_props[1] = { ECORE_X_ATOM_WM_DELETE_WINDOW };
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
   Ecore_X_Window      win;
   int                 num;

   if (EwinIsInternal(ewin))
      return;

   /* Hmmm.. Why? */
   win = _EwinGetClientXwin(ewin);
   num = ecore_x_window_prop_window_get(_EwinGetClientXwin(ewin),
					ECORE_X_ATOM_WM_COLORMAP_WINDOWS,
					&win, 1);

   ewin->client.cmap = 0;
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

   if (w < ewin->client.width.min)
      w = ewin->client.width.min;
   if (w > ewin->client.width.max)
      w = ewin->client.width.max;
   if (h < ewin->client.height.min)
      h = ewin->client.height.min;
   if (h > ewin->client.height.max)
      h = ewin->client.height.max;
   if ((w > 0) && (h > 0))
     {
	w -= ewin->client.base_w;
	h -= ewin->client.base_h;
	if ((w > 0) && (h > 0))
	  {
	     aspect = ((double)w) / ((double)h);
	     if (Mode.mode == MODE_RESIZE_H)
	       {
		  if (aspect < ewin->client.aspect_min)
		     h = (int)((double)w / ewin->client.aspect_min);
		  if (aspect > ewin->client.aspect_max)
		     h = (int)((double)w / ewin->client.aspect_max);
	       }
	     else if (Mode.mode == MODE_RESIZE_V)
	       {
		  if (aspect < ewin->client.aspect_min)
		     w = (int)((double)h * ewin->client.aspect_min);
		  if (aspect > ewin->client.aspect_max)
		     w = (int)((double)h * ewin->client.aspect_max);
	       }
	     else
	       {
		  if (aspect < ewin->client.aspect_min)
		     w = (int)((double)h * ewin->client.aspect_min);
		  if (aspect > ewin->client.aspect_max)
		     h = (int)((double)w / ewin->client.aspect_max);
	       }
	     i = w / ewin->client.w_inc;
	     j = h / ewin->client.h_inc;
	     w = i * ewin->client.w_inc;
	     h = j * ewin->client.h_inc;
	  }
	w += ewin->client.base_w;
	h += ewin->client.base_h;
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
ICCCM_Configure(const EWin * ewin)
{
   XEvent              ev;
   int                 d;
   Window              child;

   if (EwinIsInternal(ewin))
      return;

   ev.type = ConfigureNotify;
   ev.xconfigure.display = disp;
   ev.xconfigure.event = _EwinGetClientXwin(ewin);
   ev.xconfigure.window = _EwinGetClientXwin(ewin);
   d = EoGetDesk(ewin);
   ev.xconfigure.x = DeskGetX(d) + ewin->client.x;
   ev.xconfigure.y = DeskGetY(d) + ewin->client.y;
   if (Mode.wm.window)
      XTranslateCoordinates(disp, VRoot.win, RRoot.win,
			    ev.xconfigure.x, ev.xconfigure.y,
			    &ev.xconfigure.x, &ev.xconfigure.y, &child);
   ev.xconfigure.width = ewin->client.w;
   ev.xconfigure.height = ewin->client.h;
   ev.xconfigure.border_width = 0;
   ev.xconfigure.above = EoGetWin(ewin);
   ev.xconfigure.override_redirect = False;
   XSendEvent(disp, _EwinGetClientXwin(ewin), False, StructureNotifyMask, &ev);
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

   if (MenusActive())
      return;

   ICCCM_GetColormap(ewin);

   if (EwinIsInternal(ewin))
      return;

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
   Window              ww;
   long                mask;
   int                 x, y, w, h, bw, dummy;

   if (atom_change && atom_change != ECORE_X_ATOM_WM_NORMAL_HINTS)
      return;

   x = ewin->client.x;
   y = ewin->client.y;
   w = ewin->client.w;
   h = ewin->client.h;
   bw = ewin->client.bw;
   EGetGeometry(_EwinGetClientWin(ewin), &ww, &x, &y, &w, &h, &bw, &dummy);
   ewin->client.x = x;
   ewin->client.y = y;
   ewin->client.w = w;
   ewin->client.h = h;
   ewin->client.bw = bw;

   if (XGetWMNormalHints(disp, _EwinGetClientXwin(ewin), &hint, &mask))
     {
	if (!(ewin->state.placed))
	  {
	     if ((hint.flags & USPosition) || ((hint.flags & PPosition)))
	       {
		  if (hint.flags & PWinGravity)
		    {
		       ewin->client.grav = hint.win_gravity;
		    }
		  else
		    {
		       ewin->client.grav = NorthWestGravity;
		    }
		  ewin->client.x = x;
		  ewin->client.y = y;
		  if ((hint.flags & PPosition) && (!EoIsSticky(ewin)))
		    {
		       int                 dsk;

		       dsk = EoGetDesk(ewin);
		       if ((dsk < 0) || (dsk >= DesksGetNumber()))
			  dsk = DesksGetCurrent();
		       ewin->client.x -= DeskGetX(dsk);
		       ewin->client.y -= DeskGetY(dsk);
		       if (ewin->client.x + ewin->client.w >= VRoot.w)
			 {
			    ewin->client.x += DeskGetX(dsk);
			 }
		       else if (ewin->client.x < 0)
			 {
			    ewin->client.x += DeskGetX(dsk);
			 }
		       if (ewin->client.y + ewin->client.h >= VRoot.h)
			 {
			    ewin->client.y += DeskGetY(dsk);
			 }
		       else if (ewin->client.y < 0)
			 {
			    ewin->client.y += DeskGetY(dsk);
			 }
		    }
		  ewin->state.placed = 1;
	       }
	  }
	else
	  {
	     ewin->client.x = 0;
	     ewin->client.y = 0;
	     ewin->state.placed = 0;
	  }

	if (hint.flags & PMinSize)
	  {
	     ewin->client.width.min = hint.min_width;
	     ewin->client.height.min = hint.min_height;
	  }
	else
	  {
	     ewin->client.width.min = 0;
	     ewin->client.height.min = 0;
	  }

	if (hint.flags & PMaxSize)
	  {
	     ewin->client.width.max = hint.max_width;
	     ewin->client.height.max = hint.max_height;
	     if (hint.max_width < ewin->client.w)
		ewin->client.width.max = ewin->client.w;
	     if (hint.max_height < ewin->client.h)
		ewin->client.height.max = ewin->client.h;
	  }
	else
	  {
	     ewin->client.width.max = 65535;
	     ewin->client.height.max = 65535;
	  }

	if (hint.flags & PResizeInc)
	  {
	     ewin->client.w_inc = hint.width_inc;
	     ewin->client.h_inc = hint.height_inc;
	     if (ewin->client.w_inc < 1)
		ewin->client.w_inc = 1;
	     if (ewin->client.h_inc < 1)
		ewin->client.h_inc = 1;
	  }
	else
	  {
	     ewin->client.w_inc = 1;
	     ewin->client.h_inc = 1;
	  }

	if (hint.flags & PAspect)
	  {
	     if ((hint.min_aspect.y > 0.0) && (hint.min_aspect.x > 0.0))
	       {
		  ewin->client.aspect_min =
		     ((double)hint.min_aspect.x) / ((double)hint.min_aspect.y);
	       }
	     else
	       {
		  ewin->client.aspect_min = 0.0;
	       }
	     if ((hint.max_aspect.y > 0.0) && (hint.max_aspect.x > 0.0))
	       {
		  ewin->client.aspect_max =
		     ((double)hint.max_aspect.x) / ((double)hint.max_aspect.y);
	       }
	     else
	       {
		  ewin->client.aspect_max = 65535.0;
	       }
	  }
	else
	  {
	     ewin->client.aspect_min = 0.0;
	     ewin->client.aspect_max = 65535.0;
	  }

	if (hint.flags & PBaseSize)
	  {
	     ewin->client.base_w = hint.base_width;
	     ewin->client.base_h = hint.base_height;
	  }
	else
	  {
	     ewin->client.base_w = ewin->client.width.min;
	     ewin->client.base_h = ewin->client.height.min;
	  }

	if (ewin->client.width.min < ewin->client.base_w)
	   ewin->client.width.min = ewin->client.base_w;
	if (ewin->client.height.min < ewin->client.base_h)
	   ewin->client.height.min = ewin->client.base_h;
     }

   ewin->client.no_resize_h = 0;
   ewin->client.no_resize_v = 0;
   if (ewin->client.width.min == ewin->client.width.max)
      ewin->client.no_resize_h = 1;
   if (ewin->client.height.min == ewin->client.height.max)
      ewin->client.no_resize_v = 1;

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
	char              **argv, s[4096];

	_EFREE(ewin->icccm.wm_command);

	argc = ecore_x_window_prop_string_list_get(_EwinGetClientXwin(ewin),
						   ECORE_X_ATOM_WM_COMMAND,
						   &argv);
	if ((argc < 0) && TryGroup(ewin))
	   argc = ecore_x_window_prop_string_list_get(ewin->icccm.group,
						      ECORE_X_ATOM_WM_COMMAND,
						      &argv);

	ewin->icccm.wm_command =
	   Estrdup(StrlistEncodeEscaped(s, sizeof(s), argv, argc));
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

   if (EwinIsInternal(ewin))
      return;

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
