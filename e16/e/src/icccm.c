/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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

static Atom         E_XA_WM_STATE = 0;
static Atom         E_XA_WM_CHANGE_STATE = 0;

void
ICCCM_Init(void)
{
   E_XA_WM_STATE = XInternAtom(disp, "WM_STATE", False);
   E_XA_WM_CHANGE_STATE = XInternAtom(disp, "WM_CHANGE_STATE", False);
}

void
ICCCM_ProcessClientMessage(XClientMessageEvent * event)
{
   EWin               *ewin;

   if (event->message_type == E_XA_WM_CHANGE_STATE)
     {
	ewin = FindItem(NULL, event->window, LIST_FINDBY_ID, LIST_TYPE_EWIN);
	if (ewin == NULL)
	   goto exit;

	if (event->data.l[0] == IconicState)
	  {
	     if (!(ewin->iconified))
		IconifyEwin(ewin);
	  }
#if 0
	else if (event->data.l[0] == NormalState)
	  {
	     if (ewin->iconified)
		DeIconifyEwin(ewin);
	  }
	HintsSetWindowState(ewin);
#endif
     }
 exit:
   EDBUG_RETURN_;
}

void
ICCCM_GetTitle(EWin * ewin, Atom atom_change)
{
   XTextProperty       xtp;

   EDBUG(6, "ICCCM_GetTitle");
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_NAME", False);
	if (atom_change != atom)
	   EDBUG_RETURN_;
     }
   if (ewin->client.title)
      Efree(ewin->client.title);
   if (XGetWMName(disp, ewin->client.win, &xtp))
     {
	int                 items;
	char              **list;
	Status              s;

	if (xtp.format == 8)
	  {
	     s = XmbTextPropertyToTextList(disp, &xtp, &list, &items);
	     if ((s == Success) && (items > 0))
	       {
		  ewin->client.title = duplicate(*list);
		  XFreeStringList(list);
	       }
	     else
	       {
		  ewin->client.title = duplicate((char *)xtp.value);
	       }
	  }
	else
	  {
	     ewin->client.title = duplicate((char *)xtp.value);
	  }
	XFree(xtp.value);
     }
   else if (!ewin->internal)
     {
	ewin->client.title = duplicate("No Title");
     }
   EDBUG_RETURN_;
}

void
ICCCM_GetColormap(EWin * ewin)
{
   static Atom         atom = 0;
   XWindowAttributes   xwa;
   Window              win, *wlist;
   int                 num;

   EDBUG(6, "ICCCM_GetColormap");
   if (ewin->internal)
      EDBUG_RETURN_;
   if (!atom)
      atom = XInternAtom(disp, "WM_COLORMAP_WINDOWS", False);
   win = ewin->client.win;
   wlist = AtomGet(win, atom, XA_WINDOW, &num);
   if (wlist)
     {
	win = wlist[0];
	Efree(wlist);
     }
   if (XGetWindowAttributes(disp, ewin->client.win, &xwa))
     {
	if (xwa.colormap)
	  {
	     ewin->client.cmap = xwa.colormap;
	  }
	else
	  {
	     ewin->client.cmap = 0;
	  }
     }
   else
     {
	ewin->client.cmap = 0;
     }
   EDBUG_RETURN_;
}

void
ICCCM_Delete(EWin * ewin)
{
   XClientMessageEvent ev;
   static Atom         a1 = 0, a2 = 0;
   Atom                a3, *prop;
   unsigned long       lnum, ldummy;
   int                 num, i, del, dummy;

   EDBUG(6, "ICCCM_Delete");
   if (ewin->internal)
     {
	XEvent              ev;

	if (ewin->menu)
	   HideMenu(ewin->menu);
	if (ewin->pager)
	  {
	     HideEwin(ewin);
	     ev.xunmap.window = ewin->pager->win;
	     HandleUnmap(&ev);
	  }
	if (ewin->ibox)
	  {
	     HideEwin(ewin);
	     ev.xunmap.window = ewin->ibox->win;
	     HandleUnmap(&ev);
	  }
	if (ewin->dialog)
	   DialogClose(ewin->dialog);
	EDBUG_RETURN_;
     }
   if (!a1)
     {
	a1 = XInternAtom(disp, "WM_DELETE_WINDOW", False);
	a2 = XInternAtom(disp, "WM_PROTOCOLS", False);
     }
   num = 0;
   prop = NULL;
   del = 0;
   if (!XGetWMProtocols(disp, ewin->client.win, &prop, &num))
     {
	XGetWindowProperty(disp, ewin->client.win, a2, 0, 10, False, a2, &a3,
			   &dummy, &lnum, &ldummy, (unsigned char **)&prop);
	num = (int)lnum;
     }
   if (prop)
     {
	for (i = 0; i < num; i++)
	   if (prop[i] == a1)
	      del = 1;
	XFree(prop);
     }
   if (del)
     {
	ev.type = ClientMessage;
	ev.window = ewin->client.win;
	ev.message_type = a2;
	ev.format = 32;
	ev.data.l[0] = a1;
	ev.data.l[1] = CurrentTime;
	XSendEvent(disp, ewin->client.win, False, 0, (XEvent *) & ev);
     }
   else
     {
	XKillClient(disp, (XID) ewin->client.win);
     }
   EDBUG_RETURN_;
}

void
ICCCM_Save(EWin * ewin)
{
   XClientMessageEvent ev;
   static Atom         a1 = 0, a2 = 0;

   EDBUG(6, "ICCCM_Save");
   if (ewin->internal)
      EDBUG_RETURN_;
   if (!a1)
     {
	a1 = XInternAtom(disp, "WM_SAVE_YOURSELF", False);
	a2 = XInternAtom(disp, "WM_PROTOCOLS", False);
     }
   ev.type = ClientMessage;
   ev.window = ewin->client.win;
   ev.message_type = a2;
   ev.format = 32;
   ev.data.l[0] = a1;
   ev.data.l[1] = CurrentTime;
   XSendEvent(disp, ewin->client.win, False, 0, (XEvent *) & ev);
   EDBUG_RETURN_;
}

void
ICCCM_Iconify(EWin * ewin)
{
   unsigned long       c[2] = { IconicState, 0 };

   EDBUG(6, "ICCCM_Iconify");
   if (!ewin)
      EDBUG_RETURN_;
   XChangeProperty(disp, ewin->client.win, E_XA_WM_STATE, E_XA_WM_STATE,
		   32, PropModeReplace, (unsigned char *)c, 2);
   ewin->iconified = 3;
   AddItem(ewin, "ICON", ewin->client.win, LIST_TYPE_ICONIFIEDS);
   EUnmapWindow(disp, ewin->client.win);
   EDBUG_RETURN_;
}

void
ICCCM_DeIconify(EWin * ewin)
{
   unsigned long       c[2] = { NormalState, 0 };

   EDBUG(6, "ICCCM_DeIconify");
   if (!ewin)
      EDBUG_RETURN_;
   ewin->iconified = 0;
   XChangeProperty(disp, ewin->client.win, E_XA_WM_STATE, E_XA_WM_STATE,
		   32, PropModeReplace, (unsigned char *)c, 2);
   RemoveItem("ICON", ewin->client.win, LIST_FINDBY_BOTH, LIST_TYPE_ICONIFIEDS);
   EMapWindow(disp, ewin->client.win);
   EDBUG_RETURN_;
}

void
ICCCM_MatchSize(EWin * ewin)
{
   int                 w, h;
   int                 i, j;
   double              aspect;

   EDBUG(6, "ICCCM_MatchSize");
   w = ewin->client.w;
   h = ewin->client.h;

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
	     if (mode.mode == MODE_RESIZE_H)
	       {
		  if (aspect < ewin->client.aspect_min)
		     h = (int)((double)w / ewin->client.aspect_min);
		  if (aspect > ewin->client.aspect_max)
		     h = (int)((double)w / ewin->client.aspect_max);
	       }
	     else if (mode.mode == MODE_RESIZE_V)
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
   ewin->client.w = w;
   ewin->client.h = h;
   EDBUG_RETURN_;
}

void
ICCCM_Configure(EWin * ewin)
{
   XEvent              ev;
   XWindowChanges      xwc;
   int                 d;

   EDBUG(6, "ICCCM_Configure");
   d = ewin->desktop;
   if (d < 0)
      d = desks.current;
   if (ewin->shaded == 0)
     {
	xwc.x = ewin->border->border.left;
	xwc.y = ewin->border->border.top;
	xwc.width = ewin->client.w;
	xwc.height = ewin->client.h;
	XConfigureWindow(disp, ewin->win_container,
			 CWX | CWY | CWWidth | CWHeight, &xwc);
     }
   else
     {
	xwc.x = -30;
	xwc.y = -30;
	xwc.width = 1;
	xwc.height = 1;
	XConfigureWindow(disp, ewin->win_container,
			 CWX | CWY | CWWidth | CWHeight, &xwc);
	xwc.width = ewin->client.w;
	xwc.height = ewin->client.h;
     }
   xwc.x = 0;
   xwc.y = 0;
   XConfigureWindow(disp, ewin->client.win, CWX | CWY | CWWidth | CWHeight,
		    &xwc);
   if ((ewin->menu) || (ewin->dialog))
      EDBUG_RETURN_;
   ev.type = ConfigureNotify;
   ev.xconfigure.display = disp;
   ev.xconfigure.event = ewin->client.win;
   ev.xconfigure.window = ewin->client.win;
   ev.xconfigure.x = desks.desk[d].x + ewin->x + ewin->border->border.left;
   ev.xconfigure.y = desks.desk[d].y + ewin->y + ewin->border->border.top;
   ev.xconfigure.width = ewin->client.w;
   ev.xconfigure.height = ewin->client.h;
   ev.xconfigure.border_width = 0;
   ev.xconfigure.above = ewin->win;
   ev.xconfigure.override_redirect = False;
   XSendEvent(disp, ewin->client.win, False, StructureNotifyMask, &ev);
   EDBUG_RETURN_;
}

void
ICCCM_AdoptStart(EWin * ewin)
{
   Window              win = ewin->client.win;

   EDBUG(6, "ICCCM_AdoptStart");
   if (!ewin->internal)
      XAddToSaveSet(disp, win);
   EDBUG_RETURN_;
}

void
ICCCM_Adopt(EWin * ewin)
{
   Window              win = ewin->client.win;
   unsigned long       c[2] = { 0, 0 };
   XWindowAttributes   att;

   EDBUG(6, "ICCCM_Adopt");
   if (!ewin->internal)
      XSetWindowBorderWidth(disp, win, 0);
   EReparentWindow(disp, win, ewin->win_container, 0, 0);
   XGetWindowAttributes(disp, win, &att);
   XSelectInput(disp, win,
		att.your_event_mask | PropertyChangeMask | EnterWindowMask |
		LeaveWindowMask | FocusChangeMask | ResizeRedirectMask |
		StructureNotifyMask | ColormapChangeMask);
   XShapeSelectInput(disp, win, ShapeNotifyMask);
   c[0] = (ewin->client.start_iconified) ? IconicState : NormalState;
   XChangeProperty(disp, win, E_XA_WM_STATE, E_XA_WM_STATE, 32, PropModeReplace,
		   (unsigned char *)c, 2);
   ewin->x = ewin->client.x;
   ewin->y = ewin->client.y;
   ewin->reqx = ewin->client.x;
   ewin->reqy = ewin->client.y;
   ewin->w =
      ewin->client.w + ewin->border->border.left + ewin->border->border.right;
   ewin->h =
      ewin->client.h + ewin->border->border.top + ewin->border->border.bottom;
   EDBUG_RETURN_;
}

void
ICCCM_Withdraw(EWin * ewin)
{
   unsigned long       c[2] = { WithdrawnState, 0 };

   EDBUG(6, "ICCCM_Withdraw");
   if (!ewin)
      EDBUG_RETURN_;
   /* We have a choice of deleting the WM_STATE property
    * or changing the value to Withdrawn. Since twm/fvwm does
    * it that way, we change it to Withdrawn.
    */
   XChangeProperty(disp, ewin->client.win, E_XA_WM_STATE, E_XA_WM_STATE,
		   32, PropModeReplace, (unsigned char *)c, 2);
   XRemoveFromSaveSet(disp, ewin->client.win);
   EDBUG_RETURN_;
}

void
ICCCM_Cmap(EWin * ewin)
{
   static Atom         atom = 0;
   XWindowAttributes   xwa;
   Window             *wlist = NULL;
   int                 i, num;

   EDBUG(6, "ICCCM_Cmap");
   if (!ewin)
     {
	if (mode.current_cmap)
	  {
	     XUninstallColormap(disp, mode.current_cmap);
	     mode.current_cmap = 0;
	  }
	EDBUG_RETURN_;
     }
   if (mode.cur_menu_mode)
      EDBUG_RETURN_;
   ICCCM_GetColormap(ewin);

   EDBUG(6, "ICCCM_GetColormap");
   if (ewin->internal)
      EDBUG_RETURN_;

   if ((ewin->client.cmap) && (mode.current_cmap != ewin->client.cmap))
     {
	if (!atom)
	   atom = XInternAtom(disp, "WM_COLORMAP_WINDOWS", False);
	wlist = AtomGet(ewin->client.win, atom, XA_WINDOW, &num);
	if (wlist)
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (XGetWindowAttributes(disp, wlist[i], &xwa))
		    {
		       if (xwa.colormap != DefaultColormap(disp, root.scr))
			 {
			    XInstallColormap(disp, xwa.colormap);
			    mode.current_cmap = xwa.colormap;
			 }
		    }
	       }
	     Efree(wlist);
	     EDBUG_RETURN_;
	  }
	XInstallColormap(disp, ewin->client.cmap);
	mode.current_cmap = ewin->client.cmap;
     }
   EDBUG_RETURN_;
}

void
ICCCM_Focus(EWin * ewin)
{
   XClientMessageEvent ev;
   static Atom         a1 = 0, a2 = 0;
   Atom                a3, *prop;
   unsigned long       lnum, ldummy;
   int                 num, i, foc, dummy;

   EDBUG(6, "ICCCM_Focus");
   if (!a1)
     {
	a1 = XInternAtom(disp, "WM_TAKE_FOCUS", False);
	a2 = XInternAtom(disp, "WM_PROTOCOLS", False);
     }
   num = 0;
   prop = NULL;
   foc = 0;
   if (!ewin)
     {
	XSetInputFocus(disp, root.win, RevertToPointerRoot, CurrentTime);
	HintsSetActiveWindow(ewin);
	EDBUG_RETURN_;
     }
   if (ewin->menu)
      EDBUG_RETURN_;
   if (!ewin->client.need_input)
     {
	if (!mode.cur_menu_mode)
	   mode.context_ewin = ewin;
	EDBUG_RETURN_;
     }
   if (!XGetWMProtocols(disp, ewin->client.win, &prop, &num))
     {
	XGetWindowProperty(disp, ewin->client.win, a2, 0, 10, False, a2, &a3,
			   &dummy, &lnum, &ldummy, (unsigned char **)&prop);
	num = (int)lnum;
     }
   if (prop)
     {
	for (i = 0; i < num; i++)
	   if (prop[i] == a1)
	      foc = 1;
	XFree(prop);
     }
   if (foc)
     {
	ev.type = ClientMessage;
	ev.window = ewin->client.win;
	ev.message_type = a2;
	ev.format = 32;
	ev.data.l[0] = a1;
	ev.data.l[1] = CurrentTime;
	XSendEvent(disp, ewin->client.win, False, 0, (XEvent *) & ev);
     }
/*   else */
   XSetInputFocus(disp, ewin->client.win, RevertToPointerRoot, CurrentTime);
   HintsSetActiveWindow(ewin);
   EDBUG_RETURN_;
}

void
ICCCM_GetGeoms(EWin * ewin, Atom atom_change)
{
   XSizeHints          hint;
   Window              ww;
   long                mask;
   unsigned int        dummy, w, h, bw;
   int                 x, y;

   EDBUG(6, "ICCCM_GetGeoms");
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_NORMAL_HINTS", False);
	if (atom_change != atom)
	   EDBUG_RETURN_;
     }
   EGetGeometry(disp, ewin->client.win, &ww, &x, &y, &w, &h, &bw, &dummy);
   ewin->client.x = x;
   ewin->client.y = y;
   ewin->client.w = w;
   ewin->client.h = h;
   ewin->client.bw = bw;
   if (XGetWMNormalHints(disp, ewin->client.win, &hint, &mask))
     {
	if (!(ewin->client.already_placed))
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
		  if ((hint.flags & PPosition) && (!ewin->sticky))
		    {
		       int                 dsk;

		       dsk = ewin->desktop;
		       if ((dsk < 0) || (dsk >= mode.numdesktops))
			  dsk = desks.current;
		       ewin->client.x -= desks.desk[dsk].x;
		       ewin->client.y -= desks.desk[dsk].y;
		       if (ewin->client.x + ewin->client.w >= root.w)
			 {
			    ewin->client.x += desks.desk[dsk].x;
			 }
		       else if (ewin->client.x < 0)
			 {
			    ewin->client.x += desks.desk[dsk].x;
			 }
		       if (ewin->client.y + ewin->client.h >= root.h)
			 {
			    ewin->client.y += desks.desk[dsk].y;
			 }
		       else if (ewin->client.y < 0)
			 {
			    ewin->client.y += desks.desk[dsk].y;
			 }
		    }
		  ewin->client.already_placed = 1;
	       }
	  }
	else
	  {
	     ewin->client.x = 0;
	     ewin->client.y = 0;
	     ewin->client.already_placed = 0;
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
   if (ewin->client.width.min == 0)
     {
	if (ewin->internal)
	  {
	     ewin->client.width.min = w;
	     ewin->client.height.min = h;
	     ewin->client.width.max = w;
	     ewin->client.height.max = h;
	  }
     }
   ewin->client.no_resize_h = 0;
   ewin->client.no_resize_v = 0;
   if (ewin->client.width.min == ewin->client.width.max)
      ewin->client.no_resize_h = 1;
   if (ewin->client.height.min == ewin->client.height.max)
      ewin->client.no_resize_v = 1;
   EDBUG_RETURN_;
}

void
ICCCM_GetInfo(EWin * ewin, Atom atom_change)
{
   XClassHint          hint;
   XTextProperty       xtp;
   int                 cargc, i, size;
   char              **cargv, *s;
   Atom                a3;
   static Atom         a2 = 0;
   unsigned long       lnum, ldummy;
   int                 num, dummy;
   char                ok = 1;

   EDBUG(6, "ICCCM_GetInfo");
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_CLASS", False);
	if (atom_change != atom)
	   ok = 0;
     }
   if (ok)
     {
	if (XGetClassHint(disp, ewin->client.win, &hint))
	  {
	     if (ewin->client.name)
		Efree(ewin->client.name);
	     if (ewin->client.class)
		Efree(ewin->client.class);
	     ewin->client.name = duplicate(hint.res_name);
	     ewin->client.class = duplicate(hint.res_class);
	     XFree(hint.res_name);
	     XFree(hint.res_class);
	  }
	else if (XGetClassHint(disp, ewin->client.group, &hint))
	  {
	     if (ewin->client.name)
		Efree(ewin->client.name);
	     if (ewin->client.class)
		Efree(ewin->client.class);
	     ewin->client.name = duplicate(hint.res_name);
	     ewin->client.class = duplicate(hint.res_class);
	     XFree(hint.res_name);
	     XFree(hint.res_class);
	  }
	else
	  {
	     ewin->client.name = NULL;
	     ewin->client.class = NULL;
	  }
     }
   ok = 1;
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_COMMAND", False);
	if (atom_change != atom)
	   ok = 0;
     }
   if (ok)
     {
	if (XGetCommand(disp, ewin->client.win, &cargv, &cargc))
	  {
	     if (cargc > 0)
	       {
		  size = strlen(cargv[0]) + 1;
		  s = Emalloc(size);
		  strcpy(s, cargv[0]);
		  for (i = 1; i < cargc; i++)
		    {
		       size += strlen(cargv[i]) + 1;
		       s = Erealloc(s, size);
		       strcat(s, " ");
		       strcat(s, cargv[i]);
		    }
		  XFreeStringList(cargv);
		  if (ewin->client.command)
		     Efree(ewin->client.command);
		  ewin->client.command = s;
	       }
	     else
	       {
		  ewin->client.command = NULL;
	       }
	  }
	else if (XGetCommand(disp, ewin->client.group, &cargv, &cargc))
	  {
	     EWin              **lst;
	     int                 i, lnum, ok = 1;

	     lst = (EWin **) ListItemType(&lnum, LIST_TYPE_EWIN);
	     if (lst)
	       {
		  for (i = 0; i < lnum; i++)
		    {
		       if ((lst[i] != ewin)
			   && (lst[i]->client.group == ewin->client.group))
			 {
			    ok = 0;
			    i = lnum;
			 }
		    }
		  Efree(lst);
	       }
	     if (cargc > 0)
	       {
		  if (ok)
		    {
		       size = strlen(cargv[0]) + 1;
		       s = Emalloc(size);
		       strcpy(s, cargv[0]);
		       for (i = 1; i < cargc; i++)
			 {
			    size += strlen(cargv[i]) + 1;
			    s = Erealloc(s, size);
			    strcat(s, " ");
			    strcat(s, cargv[i]);
			 }
		       if (ewin->client.command)
			  Efree(ewin->client.command);
		       ewin->client.command = s;
		    }
		  else
		    {
		       ewin->client.command = NULL;
		    }
		  XFreeStringList(cargv);
	       }
	     else
	       {
		  ewin->client.command = NULL;
	       }
	  }
	else
	  {
	     ewin->client.command = NULL;
	  }
     }
   ok = 1;
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_CLIENT_MACHINE", False);
	if (atom_change != atom)
	   ok = 0;
     }
   if (ok)
     {
	if (XGetWMClientMachine(disp, ewin->client.win, &xtp))
	  {
	     if (ewin->client.machine)
		Efree(ewin->client.machine);
	     ewin->client.machine = duplicate((char *)xtp.value);
	     XFree(xtp.value);
	  }
	else if (XGetWMClientMachine(disp, ewin->client.group, &xtp))
	  {
	     if (ewin->client.machine)
		Efree(ewin->client.machine);
	     ewin->client.machine = duplicate((char *)xtp.value);
	     XFree(xtp.value);
	  }
	else
	  {
	     ewin->client.machine = NULL;
	  }
     }
   ok = 1;
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_ICON_NAME", False);
	if (atom_change != atom)
	   ok = 0;
     }
   if (ok)
     {
	if (XGetWMIconName(disp, ewin->client.win, &xtp))
	  {
	     if (ewin->client.icon_name)
		Efree(ewin->client.icon_name);
	     if (xtp.encoding == XA_STRING)
	       {
		  ewin->client.icon_name = duplicate((char *)xtp.value);
	       }
	     else
	       {
		  char              **cl;
		  Status              status;
		  int                 n;

		  status = XmbTextPropertyToTextList(disp, &xtp, &cl, &n);
		  if (status >= Success && n > 0 && cl[0])
		    {
		       ewin->client.icon_name = duplicate(cl[0]);
		       XFreeStringList(cl);
		    }
		  else
		     ewin->client.icon_name = duplicate((char *)xtp.value);
	       }
	     XFree(xtp.value);
	  }
	else if (XGetWMIconName(disp, ewin->client.group, &xtp))
	  {
	     if (ewin->client.icon_name)
		Efree(ewin->client.icon_name);
	     if (xtp.encoding == XA_STRING)
	       {
		  ewin->client.icon_name = duplicate((char *)xtp.value);
	       }
	     else
	       {
		  char              **cl;
		  Status              status;
		  int                 n;

		  status = XmbTextPropertyToTextList(disp, &xtp, &cl, &n);
		  if (status >= Success && n > 0 && cl[0])
		    {
		       ewin->client.icon_name = duplicate(cl[0]);
		       XFreeStringList(cl);
		    }
		  else
		     ewin->client.icon_name = duplicate((char *)xtp.value);
	       }
	     XFree(xtp.value);
	  }
	else
	  {
	     ewin->client.icon_name = NULL;
	  }
     }
   ok = 1;
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_WINDOW_ROLE", False);
	if (atom_change != atom)
	   ok = 0;
     }
   if (ok)
     {
	num = 0;
	s = NULL;
	if (!a2)
	   a2 = XInternAtom(disp, "WM_WINDOW_ROLE", False);
	XGetWindowProperty(disp, ewin->client.win, a2, 0, 10, False, XA_STRING,
			   &a3, &dummy, &lnum, &ldummy, (unsigned char **)&s);
	num = (int)lnum;
	if (s)
	  {
	     if (ewin->client.role)
		Efree(ewin->client.role);
	     ewin->client.role = Emalloc(num + 1);
	     memcpy(ewin->client.role, s, num);
	     ewin->client.role[num] = 0;
	     XFree(s);
	  }
     }
   EDBUG_RETURN_;
}

void
ICCCM_GetHints(EWin * ewin, Atom atom_change)
{
   XWMHints           *hint = NULL;
   Window              w;
   static Atom         a = 0;
   Atom                a2, *prop;
   Window             *cleader = NULL;
   unsigned long       lnum, ldummy;
   int                 i, num, dummy;
   static Atom         a3 = 0;
   char                ok = 1;

   EDBUG(6, "ICCCM_GetHints");
   if (ewin->internal)
      EDBUG_RETURN_;
   MWM_GetHints(ewin, atom_change);
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_HINTS", False);
	if (atom_change != atom)
	   ok = 0;
     }
   if (ok)
      hint = XGetWMHints(disp, ewin->client.win);
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
		      && hint->window_group == ewin->client.win)
		     ewin->docked = 1;
	       }
	  }
	if (hint->flags & InputHint)
	  {
	     if (hint->input)
	       {
		  ewin->client.need_input = 1;
	       }
	     else
	       {
		  ewin->client.need_input = 0;
	       }
	  }
	else
	  {
	     ewin->client.need_input = 1;
	  }
	if (hint->flags & StateHint)
	  {
	     if (hint->initial_state == IconicState)
	       {
		  ewin->client.start_iconified = 1;
	       }
	     else
	       {
		  ewin->client.start_iconified = 0;
	       }
	  }
	else
	  {
	     ewin->client.start_iconified = 0;
	  }
	if (hint->flags & IconPixmapHint)
	  {
	     ewin->client.icon_pmap = hint->icon_pixmap;
	  }
	else
	  {
	     ewin->client.icon_pmap = 0;
	  }
	if (hint->flags & IconMaskHint)
	  {
	     ewin->client.icon_mask = hint->icon_mask;
	  }
	else
	  {
	     ewin->client.icon_mask = 0;
	  }
	if (hint->flags & IconWindowHint)
	  {
	     ewin->client.icon_win = hint->icon_window;
	  }
	else
	  {
	     ewin->client.icon_win = 0;
	  }
	if (hint->flags & WindowGroupHint)
	  {
	     ewin->client.group = hint->window_group;
	  }
	else
	  {
	     ewin->client.group = 0;
	  }
	XFree(hint);
     }
   ok = 1;
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_PROTOCOLS", False);
	if (atom_change != atom)
	   ok = 0;
     }
   if (ok)
     {
	if (XGetWMProtocols(disp, ewin->client.win, &prop, &num))
	  {
	     if (!a3)
		a3 = XInternAtom(disp, "WM_TAKE_FOCUS", False);
	     if (prop)
	       {
		  for (i = 0; i < num; i++)
		     if (prop[i] == a3)
			ewin->client.need_input = 1;
		  XFree(prop);
	       }
	  }
     }
   if (!ewin->client.need_input)
     {
	ewin->skipfocus = 1;
     }
   if (XGetTransientForHint(disp, ewin->client.win, &w))
     {
	ewin->client.transient = 1;
	ewin->client.transient_for = w;
     }
   else
     {
	ewin->client.transient = 0;
     }
   if (ewin->client.group == ewin->client.win)
     {
	ewin->client.is_group_leader = 1;
     }
   else
     {
	ewin->client.is_group_leader = 0;
     }
   ok = 1;
   if (atom_change)
     {
	static Atom         atom = 0;

	if (!atom)
	   atom = XInternAtom(disp, "WM_CLIENT_LEADER", False);
	if (atom_change != atom)
	   ok = 0;
     }
   if (ok)
     {
	if (!a)
	   a = XInternAtom(disp, "WM_CLIENT_LEADER", False);
	XGetWindowProperty(disp, ewin->client.win, a, 0, 0x7fffffff, False,
			   XA_WINDOW, &a2, &dummy, &lnum, &ldummy,
			   (unsigned char **)&cleader);
	if (cleader)
	  {
	     ewin->client.client_leader = *cleader;
	     if (!ewin->client.group)
		ewin->client.group = *cleader;
	     XFree(cleader);
	  }
     }
   EDBUG_RETURN_;
}

void
ICCCM_GetShapeInfo(EWin * ewin)
{
   XRectangle         *rl = NULL;
   int                 rn = 0, ord;
   int                 x, y;
   unsigned int        w, h, d;
   Window              rt;

   EDBUG(6, "ICCCM_GetShapeInfo");
   GrabX();
   EGetGeometry(disp, ewin->client.win, &rt, &x, &y, &w, &h, &d, &d);
   rl = EShapeGetRectangles(disp, ewin->client.win, ShapeBounding, &rn, &ord);
   UngrabX();
   if (rn < 1)
     {
	ewin->client.shaped = 0;
	EShapeCombineMask(disp, ewin->win_container, ShapeBounding, 0, 0, None,
			  ShapeSet);
     }
   else if (rn == 1)
     {
	if ((rl[0].x <= 0) && (rl[0].y <= 0) && (rl[0].width >= w)
	    && (rl[0].height >= h))
	  {
	     ewin->client.shaped = 0;
	     EShapeCombineMask(disp, ewin->win_container, ShapeBounding, 0, 0,
			       None, ShapeSet);
	  }
	else
	  {
	     ewin->client.shaped = 1;
	     EShapeCombineShape(disp, ewin->win_container, ShapeBounding, 0, 0,
				ewin->client.win, ShapeBounding, ShapeSet);
	  }
     }
   else
     {
	ewin->client.shaped = 1;
	EShapeCombineShape(disp, ewin->win_container, ShapeBounding, 0, 0,
			   ewin->client.win, ShapeBounding, ShapeSet);
     }
   if (rl)
      XFree(rl);
   EDBUG_RETURN_;
}

void
ICCCM_SetIconSizes()
{
   XIconSize          *is;

   EDBUG(6, "ICCCM_SetIconSizes");
   is = XAllocIconSize();
   is->min_width = 8;
   is->min_height = 8;
   is->max_width = 48;
   is->max_height = 48;
   is->width_inc = 1;
   is->height_inc = 1;
   XSetIconSizes(disp, root.win, is, 1);
   XFree(is);
   EDBUG_RETURN_;
}

void
ICCCM_SetEInfo(EWin * ewin)
{
   static Atom         a = 0, aa = 0;
   CARD32              c[8];

   EDBUG(6, "ICCCM_SetEInfo");
   if (ewin->internal)
      EDBUG_RETURN_;
   if (!a)
      a = XInternAtom(disp, "ENL_INTERNAL_DATA", False);
   if (!aa)
      aa = XInternAtom(disp, "ENL_INTERNAL_DATA_BORDER", False);
   c[0] = ewin->desktop;
   c[1] = ewin->sticky;
   c[2] = ewin->x;
   c[3] = ewin->y;
   c[4] = ewin->iconified;
   if (ewin->iconified)
      ICCCM_DeIconify(ewin);
   c[5] = ewin->shaded;
   c[6] = ewin->client.w;
   c[7] = ewin->client.h;
   XChangeProperty(disp, ewin->client.win, a, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)c, 9);
   XChangeProperty(disp, ewin->client.win, aa, XA_STRING, 8, PropModeReplace,
		   (unsigned char *)ewin->border->name,
		   strlen(ewin->border->name) + 1);
   EDBUG_RETURN_;
}

void
ICCCM_SetMainEInfo(void)
{
   Atom                a;
   int                 i;
   CARD32              cc[ENLIGHTENMENT_CONF_NUM_DESKTOPS * 2];

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
     {
	cc[(i * 2)] = desks.desk[i].current_area_x;
	cc[(i * 2) + 1] = desks.desk[i].current_area_y;
     }
   a = XInternAtom(disp, "ENL_INTERNAL_AREA_DATA", False);
   XChangeProperty(disp, root.win, a, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)cc, ENLIGHTENMENT_CONF_NUM_DESKTOPS * 2);
   a = XInternAtom(disp, "ENL_INTERNAL_DESK_DATA", False);
   XChangeProperty(disp, root.win, a, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)(&desks.current), 1);
}

void
ICCCM_GetMainEInfo(void)
{
   Atom                a, a2;
   CARD32             *c = NULL;
   unsigned long       lnum, ldummy;
   int                 num, dummy, i;

   a = XInternAtom(disp, "ENL_INTERNAL_AREA_DATA", False);
   XGetWindowProperty(disp, root.win, a, 0, 10, False, XA_CARDINAL, &a2,
		      &dummy, &lnum, &ldummy, (unsigned char **)&c);
   num = (int)lnum;
   if ((num > 0) && (c))
     {
	for (i = 0; i < (num / 2); i++)
	  {
	     if (i < ENLIGHTENMENT_CONF_NUM_DESKTOPS)
	       {
		  desks.desk[i].current_area_x = c[(i * 2)];
		  desks.desk[i].current_area_y = c[(i * 2) + 1];
	       }
	  }
	XFree(c);
     }
   num = 0;
   c = NULL;

   a = XInternAtom(disp, "ENL_INTERNAL_DESK_DATA", False);
   XGetWindowProperty(disp, root.win, a, 0, 10, False, XA_CARDINAL, &a2,
		      &dummy, &lnum, &ldummy, (unsigned char **)&c);
   num = (int)lnum;
   if ((num > 0) && (c))
     {
	GotoDesktop(*c);
	XFree(c);
     }
}

int
ICCCM_GetEInfo(EWin * ewin)
{
   static Atom         a = 0, aa = 0;
   Atom                a2;
   CARD32             *c = NULL;
   char               *str = NULL;
   unsigned long       lnum, ldummy;
   int                 num, dummy;

   EDBUG(6, "ICCCM_GetEInfo");
   if (ewin->internal)
      EDBUG_RETURN(0);
   if (!a)
      a = XInternAtom(disp, "ENL_INTERNAL_DATA", False);
   if (!aa)
      aa = XInternAtom(disp, "ENL_INTERNAL_DATA_BORDER", False);
   XGetWindowProperty(disp, ewin->client.win, a, 0, 10, True, XA_CARDINAL, &a2,
		      &dummy, &lnum, &ldummy, (unsigned char **)&c);
   num = (int)lnum;
   if ((num >= 8) && (c))
     {
	if (mode.startup)
	  {
	     ewin->desktop = c[0];
	     ewin->sticky = c[1];
	     ewin->client.x = c[2];
	     ewin->client.y = c[3];
	     ewin->iconified = c[4];
	     ewin->shaded = c[5];
	     if (ewin->sticky)
		ewin->desktop = -1;
	     if (ewin->iconified)
	       {
		  ewin->client.start_iconified = 1;
		  ewin->iconified = 0;
	       }
	     ewin->client.already_placed = 1;
	     if (num >= 9)
	       {
		  ewin->client.w = c[6];
		  ewin->client.h = c[7];
	       }
	     XGetWindowProperty(disp, ewin->client.win, aa, 0, 0xffff, True,
				XA_STRING, &a2, &dummy, &lnum, &ldummy,
				(unsigned char **)&str);
	     num = (int)lnum;

	     if ((num > 0) && (str))
	       {
		  Border             *b = NULL;

		  b = (Border *) FindItem(str, 0, LIST_FINDBY_NAME,
					  LIST_TYPE_BORDER);
		  if ((ewin->border) && (strcmp(ewin->border->name, b->name)))
		     b = NULL;
		  if (b)
		    {
		       ewin->border_new = 1;
		       ewin->border = b;
		       SetFrameProperty(ewin);
		    }
	       }
	     XFree(c);
	     XFree(str);
	  }
	EDBUG_RETURN(1);
     }
   EDBUG_RETURN(0);
}
