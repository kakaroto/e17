/*
 * Copyright (C) 2003-2004 Kim Woelders
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
/*
 * Feeble attempt to collect hint stuff in one place
 */
#include "E.h"

/*
 * Functions that set X11-properties from E-internals
 */

void
HintsInit(void)
{
   Atom                atom;
   Window              win;

   EDBUG(6, "HintsInit");
   win = ECreateWindow(root.win, -200, -200, 5, 5, 0);
   ICCCM_Init();
#if ENABLE_GNOME
   GNOME_SetHints(win);
#endif
#if ENABLE_EWMH
   EWMH_Init(win);
#endif
   atom = XInternAtom(disp, "ENLIGHTENMENT_VERSION", False);
   XChangeProperty(disp, root.win, atom, XA_STRING, 8, PropModeReplace,
		   (unsigned char *)e_wm_version, strlen(e_wm_version));

   Conf.hints.set_xroot_info_on_root_window = 0;

   EDBUG_RETURN_;
}

void
HintsSetClientList(void)
{
   EDBUG(6, "HintsSetClientList");
#if ENABLE_GNOME
   GNOME_SetClientList();
#endif
#if ENABLE_EWMH
   EWMH_SetClientList();
   EWMH_SetClientStacking();
#endif
   EDBUG_RETURN_;
}

void
HintsSetClientStacking(void)
{
   EDBUG(6, "HintsSetClientStacking");
#if ENABLE_EWMH
   EWMH_SetClientStacking();
#endif
   EDBUG_RETURN_;
}

void
HintsSetDesktopConfig(void)
{
   EDBUG(6, "HintsSetDesktopConfig");
#if ENABLE_GNOME
   GNOME_SetDeskCount();
   GNOME_SetDeskNames();
#endif
#if ENABLE_EWMH
   EWMH_SetDesktopCount();
   EWMH_SetDesktopNames();
   EWMH_SetWorkArea();
#endif
   EDBUG_RETURN_;
}

void
HintsSetViewportConfig(void)
{
   EDBUG(6, "HintsSetViewportConfig");
#if ENABLE_GNOME
   GNOME_SetAreaCount();
#endif
#if ENABLE_EWMH
   EWMH_SetDesktopSize();
#endif
   EDBUG_RETURN_;
}

void
HintsSetCurrentDesktop(void)
{
   EDBUG(6, "HintsSetCurrentDesktop");
#if ENABLE_GNOME
   GNOME_SetCurrentDesk();
#endif
#if ENABLE_EWMH
   EWMH_SetCurrentDesktop();
#endif
   HintsSetDesktopViewport();
   EDBUG_RETURN_;
}

void
HintsSetDesktopViewport(void)
{
   EDBUG(6, "HintsSetDesktopViewport");
#if ENABLE_GNOME
   GNOME_SetCurrentArea();
#endif
#if ENABLE_EWMH
   EWMH_SetDesktopViewport();
#endif
   EDBUG_RETURN_;
}

void
HintsSetActiveWindow(EWin * ewin)
{
   EDBUG(6, "HintsSetActiveWindow");
#if ENABLE_EWMH
   EWMH_SetActiveWindow(ewin);
#endif
   EDBUG_RETURN_;
}

void
HintsSetWindowDesktop(EWin * ewin)
{
   EDBUG(6, "HintsSetWindowDesktop");
#if ENABLE_GNOME
   GNOME_SetEwinDesk(ewin);
#endif
#if ENABLE_EWMH
   if (!ewin->menu)
      EWMH_SetWindowDesktop(ewin);
#endif
   EDBUG_RETURN_;
}

void
HintsSetWindowArea(EWin * ewin)
{
   EDBUG(6, "HintsSetWindowArea");
#if ENABLE_GNOME
   GNOME_SetEwinArea(ewin);
#endif
   EDBUG_RETURN_;
   ewin = NULL;
}

void
HintsSetWindowState(EWin * ewin)
{
   EDBUG(6, "HintsSetWindowState");
#if ENABLE_GNOME
   GNOME_SetHint(ewin);
#endif
#if ENABLE_EWMH
   if (!ewin->menu)
      EWMH_SetWindowState(ewin);
#endif
   EDBUG_RETURN_;
}

void
HintsSetWindowHints(EWin * ewin)
{
   EDBUG(6, "HintsSetWindowHints");
   HintsSetWindowDesktop(ewin);
   HintsSetWindowState(ewin);
   EDBUG_RETURN_;
}

void
HintsSetWindowBorder(EWin * ewin)
{
   static Atom         atom_set = 0;
   CARD32              val[4];

   if (!atom_set)
      atom_set = XInternAtom(disp, "_E_FRAME_SIZE", False);

   if (ewin->border)
     {
	val[0] = ewin->border->border.left;
	val[1] = ewin->border->border.right;
	val[2] = ewin->border->border.top;
	val[3] = ewin->border->border.bottom;
     }
   else
      val[0] = val[1] = val[2] = val[3] = 0;

   XChangeProperty(disp, ewin->client.win, atom_set, XA_CARDINAL, 32,
		   PropModeReplace, (unsigned char *)&val, 4);
}

/*
 * Functions that set E-internals from X11-properties
 */

void
HintsGetWindowHints(EWin * ewin)
{
   EDBUG(6, "HintsGetWindowHints");
#if ENABLE_GNOME
   GNOME_GetHints(ewin, 0);
#endif
#if ENABLE_EWMH
   EWMH_GetWindowHints(ewin);
#endif
   EDBUG_RETURN_;
}

/*
 * Functions that delete X11-properties
 */

void
HintsDelWindowHints(EWin * ewin)
{
   EDBUG(6, "HintsDelWindowHints");
#if ENABLE_GNOME
   GNOME_DelHints(ewin);
#endif
#if ENABLE_EWMH
   EWMH_DelWindowHints(ewin);
#endif
   EDBUG_RETURN_;
}

/*
 * Functions processing received X11 messages
 */

void
HintsProcessPropertyChange(EWin * ewin, Atom atom_change)
{
   EDBUG(6, "HintsHandlePropertyChange");
#if ENABLE_GNOME
   GNOME_GetHints(ewin, atom_change);
#endif
#if ENABLE_EWMH
   EWMH_ProcessPropertyChange(ewin, atom_change);
#endif
   EDBUG_RETURN_;
}

void
HintsProcessClientMessage(XClientMessageEvent * event)
{
   char               *name;

   EDBUG(6, "HintsHandleClientMessage");

   name = XGetAtomName(disp, event->message_type);
   if (name == NULL)
      EDBUG_RETURN_;

   if (!memcmp(name, "ENL_", 4))
      HandleComms(event);
   else if (!memcmp(name, "WM_", 3))
      ICCCM_ProcessClientMessage(event);
#if ENABLE_EWMH
   else if (!memcmp(name, "_NET_", 5))
      EWMH_ProcessClientMessage(event);
#endif
#if ENABLE_GNOME
   else if (!memcmp(name, "_WIN_", 5))
      GNOME_ProcessClientMessage(event);
#endif
   XFree(name);
   EDBUG_RETURN_;
}

void
HintsSetRootInfo(Window win, Pixmap pmap, int color)
{
   static Atom         a = 0, aa = 0, aaa = 0;
   static Window       alive_win = 0;

   EDBUG(6, "HintsSetRootInfo");

   if (!a)
     {
	a = XInternAtom(disp, "_XROOTPMAP_ID", False);
	aa = XInternAtom(disp, "_XROOTCOLOR_PIXEL", False);
	aaa = XInternAtom(disp, "_XROOTWINDOW", False);
     }
   if (!alive_win)
     {
	alive_win = ECreateWindow(root.win, -100, -100, 1, 1, 0);
	XChangeProperty(disp, alive_win, aaa, XA_WINDOW, 32, PropModeReplace,
			(unsigned char *)&alive_win, 1);
	XChangeProperty(disp, root.win, aaa, XA_WINDOW, 32, PropModeReplace,
			(unsigned char *)&alive_win, 1);
     }

   if (Conf.hints.set_xroot_info_on_root_window)
      win = root.win;
   XChangeProperty(disp, win, a, XA_PIXMAP, 32, PropModeReplace,
		   (unsigned char *)&pmap, 1);
   XChangeProperty(disp, win, aa, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)&color, 1);

   EDBUG_RETURN_;
}
