/*
 * Copyright (C) 2003-2005 Kim Woelders
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
#include "ecore-e16.h"

/*
 * Functions that set X11-properties from E-internals
 */

void
HintsInit(void)
{
   Atom                atom;
   Window              win;

   win = ECreateWindow(VRoot.win, -200, -200, 5, 5, 0);

   ICCCM_Init();
#if ENABLE_GNOME
   GNOME_SetHints(win);
#endif
#if ENABLE_EWMH
   EWMH_Init(win);
#endif
   atom = XInternAtom(disp, "ENLIGHTENMENT_VERSION", False);
   ecore_x_window_prop_string_set(VRoot.win, atom, e_wm_version);

   if (Mode.wm.window)
     {
	HintsSetWindowName(VRoot.win, "Enlightenment");
	HintsSetWindowClass(VRoot.win, "Virtual-Root", "Enlightenment");
     }
}

void
HintsSetRootHints(Window win __UNUSED__)
{
   /* Nothing done here for now */
}

void
HintsSetClientList(void)
{
#if ENABLE_GNOME
   GNOME_SetClientList();
#endif
#if ENABLE_EWMH
   EWMH_SetClientList();
   EWMH_SetClientStacking();
#endif
}

void
HintsSetClientStacking(void)
{
#if ENABLE_EWMH
   EWMH_SetClientStacking();
#endif
}

void
HintsSetDesktopConfig(void)
{
#if ENABLE_GNOME
   GNOME_SetDeskCount();
   GNOME_SetDeskNames();
#endif
#if ENABLE_EWMH
   EWMH_SetDesktopCount();
   EWMH_SetDesktopRoots();
   EWMH_SetDesktopNames();
   EWMH_SetWorkArea();
#endif
}

void
HintsSetViewportConfig(void)
{
#if ENABLE_GNOME
   GNOME_SetAreaCount();
#endif
#if ENABLE_EWMH
   EWMH_SetDesktopSize();
#endif
}

void
HintsSetCurrentDesktop(void)
{
#if ENABLE_GNOME
   GNOME_SetCurrentDesk();
#endif
#if ENABLE_EWMH
   EWMH_SetCurrentDesktop();
#endif
   HintsSetDesktopViewport();
}

void
HintsSetDesktopViewport(void)
{
#if ENABLE_GNOME
   GNOME_SetCurrentArea();
#endif
#if ENABLE_EWMH
   EWMH_SetDesktopViewport();
#endif
}

void
HintsSetActiveWindow(Window win)
{
#if ENABLE_EWMH
   EWMH_SetActiveWindow(win);
#endif
}

void
HintsSetWindowName(Window win, const char *name)
{
   ecore_x_window_prop_string_set(win, ECORE_X_ATOM_WM_NAME, name);

#if ENABLE_EWMH
   EWMH_SetWindowName(win, name);
#endif
}

void
HintsSetWindowClass(Window win, const char *name, const char *clss)
{
   XClassHint         *xch;

   xch = XAllocClassHint();
   xch->res_name = (char *)name;
   xch->res_class = (char *)clss;
   XSetClassHint(disp, win, xch);
   XFree(xch);
}

void
HintsSetWindowDesktop(const EWin * ewin)
{
#if ENABLE_GNOME
   GNOME_SetEwinDesk(ewin);
#endif
#if ENABLE_EWMH
   EWMH_SetWindowDesktop(ewin);
#endif
}

void
HintsSetWindowArea(const EWin * ewin __UNUSED__)
{
#if ENABLE_GNOME
   GNOME_SetEwinArea(ewin);
#endif
}

void
HintsSetWindowState(const EWin * ewin)
{
#if ENABLE_GNOME
   GNOME_SetHint(ewin);
#endif
#if ENABLE_EWMH
   EWMH_SetWindowState(ewin);
   EWMH_SetWindowActions(ewin);
#endif
}

void
HintsSetWindowOpacity(const EWin * ewin)
{
#if ENABLE_EWMH
   EWMH_SetWindowOpacity(ewin);
#endif
}

void
HintsSetWindowBorder(const EWin * ewin)
{
#if ENABLE_EWMH
   EWMH_SetWindowBorder(ewin);
#endif
}

/*
 * Functions that set E-internals from X11-properties
 */

void
HintsGetWindowHints(EWin * ewin)
{
#if ENABLE_GNOME
   GNOME_GetHints(ewin, 0);
#endif
#if ENABLE_EWMH
   EWMH_GetWindowHints(ewin);
#endif
}

/*
 * Functions that delete X11-properties
 */

void
HintsDelWindowHints(const EWin * ewin)
{
#if ENABLE_GNOME
   GNOME_DelHints(ewin);
#endif
#if ENABLE_EWMH
   EWMH_DelWindowHints(ewin);
#endif
}

/*
 * Functions processing received X11 messages
 */

void
HintsProcessPropertyChange(EWin * ewin, Atom atom_change)
{
   char               *name;

   name = XGetAtomName(disp, atom_change);
   if (name == NULL)
      return;

   if (!memcmp(name, "WM_", 3))
      ICCCM_ProcessPropertyChange(ewin, atom_change);
#if ENABLE_EWMH
   else if (!memcmp(name, "_NET_", 5))
      EWMH_ProcessPropertyChange(ewin, atom_change);
#endif
#if 0				/* No! - ENABLE_GNOME */
   else if (!memcmp(name, "_WIN_", 5))
      GNOME_GetHints(ewin, atom_change);
#endif
   XFree(name);
}

void
HintsProcessClientMessage(XClientMessageEvent * event)
{
   char               *name;

   name = XGetAtomName(disp, event->message_type);
   if (name == NULL)
      return;

   if (!memcmp(name, "WM_", 3))
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
}

void
HintsSetRootInfo(Window win, Pixmap pmap, unsigned int color)
{
   static Atom         a = 0, aa = 0;
   Ecore_X_Pixmap      pm;

   if (!a)
     {
	a = XInternAtom(disp, "_XROOTPMAP_ID", False);
	aa = XInternAtom(disp, "_XROOTCOLOR_PIXEL", False);
     }

   if (Conf.hints.set_xroot_info_on_root_window)
      win = VRoot.win;

   pm = pmap;
   ecore_x_window_prop_xid_set(win, a, XA_PIXMAP, &pm, 1);

   ecore_x_window_prop_card32_set(win, aa, &color, 1);
}

void
EHintsSetInfo(const EWin * ewin)
{
   static Atom         a = 0, aa = 0;
   int                 c[9];

   if (EwinIsInternal(ewin))
      return;

   if (!a)
      a = XInternAtom(disp, "ENL_INTERNAL_DATA", False);
   if (!aa)
      aa = XInternAtom(disp, "ENL_INTERNAL_DATA_BORDER", False);

   c[0] = EoGetDesk(ewin);
   c[1] = EoIsSticky(ewin);
   c[2] = EoGetX(ewin);
   c[3] = EoGetY(ewin);
   c[4] = ewin->state.iconified;
   c[5] = ewin->state.shaded;
   c[6] = ewin->client.w;
   c[7] = ewin->client.h;
   c[8] = ewin->state.docked;

   ecore_x_window_prop_card32_set(ewin->client.win, a, (unsigned int *)c, 9);

   ecore_x_window_prop_string_set(ewin->client.win, aa,
				  ewin->normal_border->name);

   if (EventDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap set einf  %#lx: %4d+%4d %4dx%4d: %s\n",
	      ewin->client.win, ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetName(ewin));
}

int
EHintsGetInfo(EWin * ewin)
{
   static Atom         a = 0, aa = 0;
   char               *str;
   int                 num;
   int                 c[9];

   if (EwinIsInternal(ewin))
      return 0;

   if (!a)
      a = XInternAtom(disp, "ENL_INTERNAL_DATA", False);
   if (!aa)
      aa = XInternAtom(disp, "ENL_INTERNAL_DATA_BORDER", False);

   num =
      ecore_x_window_prop_card32_get(ewin->client.win, a, (unsigned int *)c, 9);
   if (num < 8)
      return 0;

   EoSetDesk(ewin, c[0]);
   EoSetSticky(ewin, c[1]);
   ewin->client.x = c[2];
   ewin->client.y = c[3];
   ewin->state.iconified = c[4];
   ewin->state.shaded = c[5];
   ewin->client.w = c[6];
   ewin->client.h = c[7];
   if (num >= 9)		/* Compatibility */
      ewin->state.docked = c[8];

   ewin->client.grav = NorthWestGravity;
   if (ewin->state.iconified)
     {
	ewin->client.start_iconified = 1;
	ewin->state.iconified = 0;
     }
   ewin->state.placed = 1;

   str = ecore_x_window_prop_string_get(ewin->client.win, aa);
   if (str)
      EwinSetBorderByName(ewin, str);
   Efree(str);

   if (EventDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap get einf  %#lx: %4d+%4d %4dx%4d: %s\n",
	      ewin->client.win, ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetName(ewin));

   return 0;
}

void
EHintsSetDeskInfo(void)
{
   Atom                a;
   int                 i, ax, ay, n_desks;
   unsigned int       *c;

   n_desks = DesksGetNumber();
   if (n_desks <= 0)
      return;

   c = Emalloc(2 * n_desks * sizeof(unsigned int));
   if (!c)
      return;

   for (i = 0; i < n_desks; i++)
     {
	DeskGetArea(i, &ax, &ay);
	c[(i * 2)] = ax;
	c[(i * 2) + 1] = ay;
     }

   a = XInternAtom(disp, "ENL_INTERNAL_AREA_DATA", False);
   ecore_x_window_prop_card32_set(VRoot.win, a, c, 2 * n_desks);

   a = XInternAtom(disp, "ENL_INTERNAL_DESK_DATA", False);
   c[0] = DesksGetCurrent();
   ecore_x_window_prop_card32_set(VRoot.win, a, c, 1);

   Efree(c);
}

void
EHintsGetDeskInfo(void)
{
   Atom                a;
   unsigned int       *c;
   int                 num, i, n_desks;

   n_desks = DesksGetNumber();
   c = Emalloc(2 * n_desks * sizeof(unsigned int));
   if (!c)
      return;

   a = XInternAtom(disp, "ENL_INTERNAL_AREA_DATA", False);
   num = ecore_x_window_prop_card32_get(VRoot.win, a, c, 2 * n_desks);
   if (num > 0)
     {
	for (i = 0; i < (num / 2); i++)
	   DeskSetArea(i, c[(i * 2)], c[(i * 2) + 1]);
     }

   a = XInternAtom(disp, "ENL_INTERNAL_DESK_DATA", False);
   num = ecore_x_window_prop_card32_get(VRoot.win, a, c, 1);
   if (num > 0)
     {
	DesksSetCurrent(c[0]);
     }
   else
     {
	/* Used to test if we should run cmd_init */
	Mode.wm.session_start = 1;
     }

   Efree(c);
}

void
EHintsSetInfoOnAll(void)
{
   int                 i, num;
   EWin               *const *lst;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("SetEInfoOnAll\n");

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
      if (!EwinIsInternal(lst[i]))
	 EHintsSetInfo(lst[i]);

   EHintsSetDeskInfo();
}
