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
 * Extended Window Manager Hints.
 */
#include "E.h"

#define _ATOM_INIT(atom) atom = XInternAtom(disp, #atom, False); \
    atom_list[atom_count++] = atom

#define _ATOM_SET_UTF8_STRING(atom, win, string) \
   XChangeProperty(disp, win, atom, E_XA_UTF8_STRING, 8, PropModeReplace, \
                   (unsigned char *)string, strlen(string))
#define _ATOM_SET_UTF8_STRING_LIST(atom, win, string, cnt) \
   XChangeProperty(disp, win, atom, E_XA_UTF8_STRING, 8, PropModeReplace, \
                   (unsigned char *)string, cnt)
#define _ATOM_SET_WINDOW(atom, win, p_wins, cnt) \
   XChangeProperty(disp, win, atom, XA_WINDOW, 32, PropModeReplace, \
                   (unsigned char *)p_wins, cnt)
#define _ATOM_SET_ATOM(atom, win, p_atom, cnt) \
   XChangeProperty(disp, win, atom, XA_ATOM, 32, PropModeReplace, \
                   (unsigned char *)p_atom, cnt)
#define _ATOM_SET_CARD32(atom, win, p_val, cnt) \
   XChangeProperty(disp, win, atom, XA_CARDINAL, 32, PropModeReplace, \
                   (unsigned char *)p_val, cnt)

/* Will become predefined? */
Atom                E_XA_UTF8_STRING;

/* Window manager info */
Atom                _NET_SUPPORTED;
Atom                _NET_SUPPORTING_WM_CHECK;

/* Desktop status/requests */
Atom                _NET_NUMBER_OF_DESKTOPS;
Atom                _NET_DESKTOP_GEOMETRY;
Atom                _NET_DESKTOP_NAMES;
Atom                _NET_CURRENT_DESKTOP;
Atom                _NET_DESKTOP_VIEWPORT;
Atom                _NET_WORKAREA;
Atom                _NET_VIRTUAL_ROOTS;
Atom                _NET_SHOWING_DESKTOP;

Atom                _NET_ACTIVE_WINDOW;
Atom                _NET_CLIENT_LIST;
Atom                _NET_CLIENT_LIST_STACKING;

/* Misc window ops */
Atom                _NET_CLOSE_WINDOW;

#if 0				/* Not yet implemented */
Atom                _NET_MOVERESIZE_WINDOW;
#endif

/*
 * _NET_WM_MOVERESIZE
 *
 * Client message
 */
Atom                _NET_WM_MOVERESIZE;

/* Move/resize actions */
#define _NET_WM_MOVERESIZE_SIZE_TOPLEFT     0
#define _NET_WM_MOVERESIZE_SIZE_TOP         1
#define _NET_WM_MOVERESIZE_SIZE_TOPRIGHT    2
#define _NET_WM_MOVERESIZE_SIZE_RIGHT       3
#define _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT 4
#define _NET_WM_MOVERESIZE_SIZE_BOTTOM      5
#define _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT  6
#define _NET_WM_MOVERESIZE_SIZE_LEFT        7
#define _NET_WM_MOVERESIZE_MOVE             8
#define _NET_WM_MOVERESIZE_SIZE_KEYBOARD    9
#define _NET_WM_MOVERESIZE_MOVE_KEYBOARD   10

/*
 * Application Window Properties
 */
Atom                _NET_WM_NAME;
Atom                _NET_WM_ICON_NAME;

#if 0				/* Not yet implemented */
Atom                _NET_WM_VISIBLE_NAME;
Atom                _NET_WM_VISIBLE_ICON_NAME;
#endif
Atom                _NET_WM_DESKTOP;

/* _NET_WM_WINDOW_TYPE (window property) */
Atom                _NET_WM_WINDOW_TYPE;

Atom                _NET_WM_WINDOW_TYPE_DESKTOP;
Atom                _NET_WM_WINDOW_TYPE_DOCK;
Atom                _NET_WM_WINDOW_TYPE_TOOLBAR;
Atom                _NET_WM_WINDOW_TYPE_MENU;
Atom                _NET_WM_WINDOW_TYPE_UTILITY;
Atom                _NET_WM_WINDOW_TYPE_SPLASH;
Atom                _NET_WM_WINDOW_TYPE_DIALOG;
Atom                _NET_WM_WINDOW_TYPE_NORMAL;

/* _NET_WM_STATE (window property and client message) */
Atom                _NET_WM_STATE;

Atom                _NET_WM_STATE_MODAL;
Atom                _NET_WM_STATE_STICKY;
Atom                _NET_WM_STATE_MAXIMIZED_VERT;
Atom                _NET_WM_STATE_MAXIMIZED_HORZ;
Atom                _NET_WM_STATE_SHADED;
Atom                _NET_WM_STATE_SKIP_TASKBAR;
Atom                _NET_WM_STATE_SKIP_PAGER;
Atom                _NET_WM_STATE_HIDDEN;
Atom                _NET_WM_STATE_FULLSCREEN;
Atom                _NET_WM_STATE_ABOVE;
Atom                _NET_WM_STATE_BELOW;

Atom                _NET_WM_WINDOW_OPACITY;

/* Window state property change actions */
#define _NET_WM_STATE_REMOVE    0
#define _NET_WM_STATE_ADD       1
#define _NET_WM_STATE_TOGGLE    2

Atom                _NET_WM_STRUT;

#if 0				/* Not yet implemented */
Atom                _NET_WM_ALLOWED_ACTIONS;
Atom                _NET_WM_ICON_GEOMETRY;
Atom                _NET_WM_ICON;
Atom                _NET_WM_PID;
Atom                _NET_WM_HANDLED_ICONS;

Atom                _NET_WM_PING;
#endif

#ifndef ENABLE_HINTS_GNOME
Atom                _G_WIN_LAUER;
#endif

/*
 * Set/clear Atom in list
 */
static void
atom_list_set(Atom * atoms, int size, int *count, Atom atom, int set)
{
   int                 i, n, in_list;

   n = *count;

   /* Check if atom is in list or not (+get index) */
   for (i = 0; i < n; i++)
      if (atoms[i] == atom)
	 break;
   in_list = i < n;

   if (set && !in_list)
     {
	/* Add it (if space left) */
	if (n < size)
	   atoms[n++] = atom;
	*count = n;
     }
   else if (!set && in_list)
     {
	/* Remove it */
	atoms[i] = atoms[--n];
	*count = n;
     }
}

/*
 * Initialize EWMH stuff
 */
void
EWMH_Init(Window win_wm_check)
{
   Atom                atom_list[64];
   int                 atom_count;

   EDBUG(6, "EWMH_Init");

   E_XA_UTF8_STRING = XInternAtom(disp, "UTF8_STRING", False);

   atom_count = 0;

   _ATOM_INIT(_NET_SUPPORTED);
   _ATOM_INIT(_NET_SUPPORTING_WM_CHECK);

   _ATOM_INIT(_NET_NUMBER_OF_DESKTOPS);
   _ATOM_INIT(_NET_DESKTOP_GEOMETRY);
   _ATOM_INIT(_NET_DESKTOP_NAMES);
   _ATOM_INIT(_NET_CURRENT_DESKTOP);
   _ATOM_INIT(_NET_DESKTOP_VIEWPORT);
   _ATOM_INIT(_NET_WORKAREA);
   _ATOM_INIT(_NET_VIRTUAL_ROOTS);
   _ATOM_INIT(_NET_SHOWING_DESKTOP);

   _ATOM_INIT(_NET_ACTIVE_WINDOW);
   _ATOM_INIT(_NET_CLIENT_LIST);
   _ATOM_INIT(_NET_CLIENT_LIST_STACKING);

   _ATOM_INIT(_NET_CLOSE_WINDOW);
   _ATOM_INIT(_NET_WM_MOVERESIZE);

   _ATOM_INIT(_NET_WM_NAME);
   _ATOM_INIT(_NET_WM_ICON_NAME);
   _ATOM_INIT(_NET_WM_DESKTOP);

   _ATOM_INIT(_NET_WM_WINDOW_TYPE);
   _ATOM_INIT(_NET_WM_WINDOW_TYPE_DESKTOP);
   _ATOM_INIT(_NET_WM_WINDOW_TYPE_DOCK);
   _ATOM_INIT(_NET_WM_WINDOW_TYPE_TOOLBAR);
   _ATOM_INIT(_NET_WM_WINDOW_TYPE_MENU);
   _ATOM_INIT(_NET_WM_WINDOW_TYPE_UTILITY);
   _ATOM_INIT(_NET_WM_WINDOW_TYPE_SPLASH);
   _ATOM_INIT(_NET_WM_WINDOW_TYPE_DIALOG);
   _ATOM_INIT(_NET_WM_WINDOW_TYPE_NORMAL);

   _ATOM_INIT(_NET_WM_STATE);
   _ATOM_INIT(_NET_WM_STATE_MODAL);
   _ATOM_INIT(_NET_WM_STATE_STICKY);
   _ATOM_INIT(_NET_WM_STATE_MAXIMIZED_VERT);
   _ATOM_INIT(_NET_WM_STATE_MAXIMIZED_HORZ);
   _ATOM_INIT(_NET_WM_STATE_SHADED);
   _ATOM_INIT(_NET_WM_STATE_SKIP_TASKBAR);
   _ATOM_INIT(_NET_WM_STATE_SKIP_PAGER);
   _ATOM_INIT(_NET_WM_STATE_HIDDEN);
   _ATOM_INIT(_NET_WM_STATE_FULLSCREEN);
   _ATOM_INIT(_NET_WM_STATE_ABOVE);
   _ATOM_INIT(_NET_WM_STATE_BELOW);

   _ATOM_INIT(_NET_WM_WINDOW_OPACITY);

   _ATOM_INIT(_NET_WM_STRUT);

   _ATOM_SET_ATOM(_NET_SUPPORTED, VRoot.win, atom_list, atom_count);

   /* Set WM info properties */
   _ATOM_SET_UTF8_STRING(_NET_WM_NAME, VRoot.win, e_wm_name);

   _ATOM_SET_WINDOW(_NET_SUPPORTING_WM_CHECK, VRoot.win, &win_wm_check, 1);
   _ATOM_SET_WINDOW(_NET_SUPPORTING_WM_CHECK, win_wm_check, &win_wm_check, 1);
   _ATOM_SET_UTF8_STRING(_NET_WM_NAME, win_wm_check, e_wm_name);

   EWMH_SetDesktopCount();
   EWMH_SetDesktopNames();
   EWMH_SetDesktopSize();
   EWMH_SetWorkArea();

   EDBUG_RETURN_;
}

/*
 * Desktops
 */

void
EWMH_SetDesktopCount(void)
{
   int                 i;
   CARD32              val;
   Window              wl[ENLIGHTENMENT_CONF_NUM_DESKTOPS];

   EDBUG(6, "EWMH_SetDesktopCount");

   val = Conf.desks.num;
   _ATOM_SET_CARD32(_NET_NUMBER_OF_DESKTOPS, VRoot.win, &val, 1);

   for (i = 0; i < Conf.desks.num; i++)
     {
	wl[i] = desks.desk[i].win;
     }
   _ATOM_SET_WINDOW(_NET_VIRTUAL_ROOTS, VRoot.win, &wl, Conf.desks.num);

   EDBUG_RETURN_;
}

void
EWMH_SetDesktopNames(void)
{
   char                buf[10 * ENLIGHTENMENT_CONF_NUM_DESKTOPS], *s;
   int                 i;

   EDBUG(6, "EWMH_SetDesktopNames");

   s = buf;
   for (i = 0; i < Conf.desks.num; i++)
      s += sprintf(s, "Desk-%d", i) + 1;

   _ATOM_SET_UTF8_STRING_LIST(_NET_DESKTOP_NAMES, VRoot.win, buf, s - buf);

   EDBUG_RETURN_;
}

void
EWMH_SetDesktopSize(void)
{
   CARD32              size[2];
   int                 ax, ay;

   EDBUG(6, "EWMH_SetDesktopSize");
   GetAreaSize(&ax, &ay);
   size[0] = ax * VRoot.w;
   size[1] = ay * VRoot.h;
   _ATOM_SET_CARD32(_NET_DESKTOP_GEOMETRY, VRoot.win, &size, 2);
   EDBUG_RETURN_;
}

void
EWMH_SetWorkArea(void)
{
   CARD32             *p_coord;
   int                 n_coord, i;

   EDBUG(6, "EWMH_SetWorkArea");

   n_coord = 4 * Conf.desks.num;
   p_coord = Emalloc(n_coord * sizeof(CARD32));
   if (p_coord)
     {
	for (i = 0; i < Conf.desks.num; i++)
	  {
	     p_coord[4 * i] = 0;
	     p_coord[4 * i + 1] = 0;
	     p_coord[4 * i + 2] = VRoot.w;
	     p_coord[4 * i + 3] = VRoot.h;
	  }
	_ATOM_SET_CARD32(_NET_WORKAREA, VRoot.win, p_coord, n_coord);
	Efree(p_coord);
     }
   EDBUG_RETURN_;
}

void
EWMH_SetCurrentDesktop(void)
{
   CARD32              val;

   EDBUG(6, "EWMH_SetCurrentDesktop");
   val = desks.current;
   _ATOM_SET_CARD32(_NET_CURRENT_DESKTOP, VRoot.win, &val, 1);
   EDBUG_RETURN_;
}

void
EWMH_SetDesktopViewport(void)
{
   CARD32             *p_coord;
   int                 n_coord, i;

   EDBUG(6, "EWMH_SetDesktopViewport");
   n_coord = 2 * Conf.desks.num;
   p_coord = Emalloc(n_coord * sizeof(CARD32));
   if (p_coord)
     {
	for (i = 0; i < Conf.desks.num; i++)
	  {
	     p_coord[2 * i] = desks.desk[i].current_area_x * VRoot.w;
	     p_coord[2 * i + 1] = desks.desk[i].current_area_y * VRoot.h;
	  }
	_ATOM_SET_CARD32(_NET_DESKTOP_VIEWPORT, VRoot.win, p_coord, n_coord);
	Efree(p_coord);
     }
   EDBUG_RETURN_;
}

/*
 * Window status
 */

void
EWMH_SetClientList(void)
{
   Window             *wl;
   int                 i, num;
   EWin              **lst;

   EDBUG(6, "EWMH_SetClientList");

   /* Mapping order */
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (num > 0)
     {
	wl = Emalloc(num * sizeof(Window));
	for (i = 0; i < num; i++)
	   wl[i] = lst[i]->client.win;
	_ATOM_SET_WINDOW(_NET_CLIENT_LIST, VRoot.win, wl, num);
	Efree(wl);
     }
   else
     {
	_ATOM_SET_WINDOW(_NET_CLIENT_LIST, VRoot.win, NULL, 0);
     }
   if (lst)
      Efree(lst);

   EDBUG_RETURN_;
}

void
EWMH_SetClientStacking(void)
{
   Window             *wl;
   int                 i, num;
   EWin               *const *lst;

   EDBUG(6, "EWMH_SetClientStacking");

   /* Stacking order */
   lst = EwinListGetStacking(&num);
   if (num > 0)
     {
	wl = Emalloc(num * sizeof(Window));
	for (i = 0; i < num; i++)
	   wl[i] = lst[num - i - 1]->client.win;
	_ATOM_SET_WINDOW(_NET_CLIENT_LIST_STACKING, VRoot.win, wl, num);
	Efree(wl);
     }
   else
     {
	_ATOM_SET_WINDOW(_NET_CLIENT_LIST_STACKING, VRoot.win, NULL, 0);
     }

   EDBUG_RETURN_;
}

void
EWMH_SetActiveWindow(Window win)
{
   static Window       win_last_set;

   EDBUG(6, "EWMH_SetActiveWindow");
   if (win != win_last_set)
     {
	_ATOM_SET_WINDOW(_NET_ACTIVE_WINDOW, VRoot.win, &win, 1);
	win_last_set = win;
     }
   EDBUG_RETURN_;
}

void
EWMH_SetShowingDesktop(int on)
{
   CARD32              val;

   EDBUG(6, "EWMH_SetShowingDesktop");
   val = on;
   _ATOM_SET_CARD32(_NET_SHOWING_DESKTOP, VRoot.win, &val, 1);
   EDBUG_RETURN_;
}

/*
 * Functions that set X11-properties from E-window internals
 */

void
EWMH_SetWindowName(Window win, const char *name)
{
   const char         *str;

   EDBUG(6, "EWMH_SetWindowName");
   str = EstrInt2Enc(name, 1);
   _ATOM_SET_UTF8_STRING(_NET_WM_NAME, win, str);
   EstrInt2EncFree(str, 1);
   EDBUG_RETURN_;
}

void
EWMH_SetWindowDesktop(const EWin * ewin)
{
   CARD32              val;

   EDBUG(6, "EWMH_SetWindowDesktop");
   if (ewin->sticky == 1)
      val = 0xFFFFFFFF;
   else
      val = ewin->desktop;
   _ATOM_SET_CARD32(_NET_WM_DESKTOP, ewin->client.win, &val, 1);
   EDBUG_RETURN_;
}

void
EWMH_SetWindowState(const EWin * ewin)
{
   Atom                atom_list[64];
   int                 len = sizeof(atom_list) / sizeof(Atom);
   int                 atom_count;

   EDBUG(6, "EWMH_SetWindowState");
   atom_count = 0;
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_STICKY,
		 ewin->sticky);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_SHADED,
		 ewin->shaded);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_SKIP_TASKBAR,
		 ewin->skiptask);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_HIDDEN,
		 ewin->iconified || ewin->shaded);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_MAXIMIZED_VERT,
		 ewin->st.maximized_vert);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_MAXIMIZED_HORZ,
		 ewin->st.maximized_horz);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_FULLSCREEN,
		 ewin->st.fullscreen);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_SKIP_PAGER,
		 ewin->skip_ext_pager);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_ABOVE,
		 ewin->layer >= 6);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_BELOW,
		 ewin->layer <= 2);
   _ATOM_SET_ATOM(_NET_WM_STATE, ewin->client.win, atom_list, atom_count);
   EDBUG_RETURN_;
}

void
EWMH_SetWindowOpacity(EWin * ewin, unsigned int opacity)
{
   CARD32              val = opacity;

   ewin->props.opacity = opacity;
   _ATOM_SET_CARD32(_NET_WM_WINDOW_OPACITY, ewin->win, &val, 1);
}

/*
 * Functions that set E-window internals from X11-properties
 */

void
EWMH_GetWindowName(EWin * ewin)
{
   char               *val;
   int                 size;

   EDBUG(6, "EWMH_GetWindowName");

   val = AtomGet(ewin->client.win, _NET_WM_NAME, E_XA_UTF8_STRING, &size);
   if (!val)
      goto done;

   if (ewin->ewmh.wm_name)
      Efree(ewin->ewmh.wm_name);
   ewin->ewmh.wm_name = EstrUtf82Int(val, size);

   Efree(val);
   EwinChange(ewin, EWIN_CHANGE_NAME);

 done:
   EDBUG_RETURN_;
}

void
EWMH_GetWindowIconName(EWin * ewin)
{
   char               *val;
   int                 size;

   EDBUG(6, "EWMH_GetWindowIconName");

   val = AtomGet(ewin->client.win, _NET_WM_ICON_NAME, E_XA_UTF8_STRING, &size);
   if (!val)
      goto done;

   if (ewin->ewmh.wm_icon_name)
      Efree(ewin->ewmh.wm_icon_name);
   ewin->ewmh.wm_icon_name = EstrUtf82Int(val, size);

   Efree(val);
   EwinChange(ewin, EWIN_CHANGE_ICON_NAME);

 done:
   EDBUG_RETURN_;
}

void
EWMH_GetWindowDesktop(EWin * ewin)
{
   CARD32             *val;
   int                 size;

   EDBUG(6, "EWMH_GetWindowDesktop");

   val = AtomGet(ewin->client.win, _NET_WM_DESKTOP, XA_CARDINAL, &size);
   if (!val)
      goto done;

   if ((unsigned)val[0] == 0xFFFFFFFF)
     {
	/* It is possible to distinguish between "sticky" and "on all desktops". */
	/* E doesn't */
	ewin->sticky = 1;
     }
   else
     {
	ewin->desktop = val[0];
	ewin->sticky = 0;
     }
   Efree(val);
   EwinChange(ewin, EWIN_CHANGE_DESKTOP);

 done:
   EDBUG_RETURN_;
}

void
EWMH_GetWindowState(EWin * ewin)
{
   Atom               *p_atoms, atom;
   int                 i, n_atoms;

   EDBUG(6, "EWMH_GetWindowState");

   n_atoms = 0;
   p_atoms = AtomGet(ewin->client.win, _NET_WM_STATE, XA_ATOM, &n_atoms);
   n_atoms /= sizeof(Atom);	/* Silly */
   if (!p_atoms)
      goto done;

   /* We must clear/set all according to not present/present */
   ewin->sticky = ewin->shaded = 0;
   ewin->skiptask = ewin->skip_ext_pager = 0;
   ewin->st.maximized_horz = ewin->st.maximized_vert = 0;
   ewin->st.fullscreen = 0;
/* ewin->layer = No ... TBD */

   for (i = 0; i < n_atoms; i++)
     {
	atom = p_atoms[i];
	if (atom == _NET_WM_STATE_STICKY)
	   ewin->sticky = 1;
	else if (atom == _NET_WM_STATE_SHADED)
	   ewin->shaded = 1;
	else if (atom == _NET_WM_STATE_SKIP_TASKBAR)
	   ewin->skiptask = 1;
	else if (atom == _NET_WM_STATE_SKIP_PAGER)
	   ewin->skip_ext_pager = 1;
	else if (atom == _NET_WM_STATE_HIDDEN)
	   ;			/* ewin->iconified = 1; No - WM_STATE does this */
	else if (atom == _NET_WM_STATE_MAXIMIZED_VERT)
	   ewin->st.maximized_vert = 1;
	else if (atom == _NET_WM_STATE_MAXIMIZED_HORZ)
	   ewin->st.maximized_horz = 1;
	else if (atom == _NET_WM_STATE_FULLSCREEN)
	   ewin->st.fullscreen = 1;
	else if (atom == _NET_WM_STATE_ABOVE)
	   ewin->layer = 6;
	else if (atom == _NET_WM_STATE_BELOW)
	   ewin->layer = 2;
     }
   Efree(p_atoms);

 done:
   EDBUG_RETURN_;
}

static void
EWMH_GetWindowType(EWin * ewin)
{
   Atom               *p_atoms, atom;
   int                 n_atoms;

   EDBUG(6, "EWMH_GetWindowType");

   n_atoms = 0;
   p_atoms = AtomGet(ewin->client.win, _NET_WM_WINDOW_TYPE, XA_ATOM, &n_atoms);
   n_atoms /= sizeof(Atom);	/* Silly */
   if (!p_atoms)
      goto done;

   atom = p_atoms[0];
   if (atom == _NET_WM_WINDOW_TYPE_DESKTOP)
     {
	ewin->layer = 0;
	ewin->sticky = 1;
#if 0				/* Should be configurable */
	ewin->focusclick = 1;
#endif
	ewin->skipfocus = 1;
	ewin->fixedpos = 1;
	EwinSetBorderByName(ewin, "BORDERLESS", 0);
     }
   else if (atom == _NET_WM_WINDOW_TYPE_DOCK)
     {
	ewin->skiptask = 1;
	ewin->skipwinlist = 1;
	ewin->skipfocus = 1;
	ewin->sticky = 1;
	ewin->never_use_area = 1;
     }
   else if (atom == _NET_WM_WINDOW_TYPE_UTILITY)
     {
	/* Epplets hit this */
	ewin->skiptask = 1;
	ewin->skipwinlist = 1;
	ewin->skipfocus = 1;
	ewin->never_use_area = 1;
     }
#if 0				/* Not used by E (yet?) */
   else if (atom == _NET_WM_WINDOW_TYPE_TOOLBAR)
     {
     }
   else if (atom == _NET_WM_WINDOW_TYPE_MENU)
     {
     }
   else if (atom == _NET_WM_WINDOW_TYPE_SPLASH)
     {
     }
   else if (atom == _NET_WM_WINDOW_TYPE_DIALOG)
     {
     }
   else if (atom == _NET_WM_WINDOW_TYPE_NORMAL)
     {
     }
#endif
   Efree(p_atoms);

 done:
   EDBUG_RETURN_;
}

static void
EWMH_GetWindowMisc(EWin * ewin)
{
   CARD32             *val;
   int                 size;

   EDBUG(6, "EWMH_GetWindowMisc");

   val = AtomGet(ewin->client.win, _NET_SUPPORTING_WM_CHECK, XA_WINDOW, &size);
   if (val)
     {
	ewin->props.vroot = 1;
	Efree(val);
     }
}

static void
EWMH_GetWindowOpacity(EWin * ewin)
{
   CARD32             *val;
   int                 size;

   EDBUG(6, "EWMH_GetWindowOpacity");

   val = AtomGet(ewin->client.win, _NET_WM_WINDOW_OPACITY, XA_CARDINAL, &size);
   if (val)
     {
	ewin->props.opacity = val[0];
	EWMH_SetWindowOpacity(ewin, val[0]);
	Efree(val);
     }

   EDBUG_RETURN_;
}

static void
EWMH_GetWindowStrut(EWin * ewin)
{
   CARD32             *val;
   int                 size;

   EDBUG(6, "EWMH_GetWindowStrut");

   val = AtomGet(ewin->client.win, _NET_WM_STRUT, XA_CARDINAL, &size);
   if (val)
     {
	if (size > 4)
	  {
	     ewin->strut.left = val[0];
	     ewin->strut.right = val[1];
	     ewin->strut.top = val[2];
	     ewin->strut.bottom = val[3];
	  }
	Efree(val);
     }

   EDBUG_RETURN_;
}

void
EWMH_GetWindowHints(EWin * ewin)
{
   EDBUG(6, "EWMH_GetWindowHints");
   EWMH_GetWindowMisc(ewin);
   EWMH_GetWindowOpacity(ewin);
   EWMH_GetWindowName(ewin);
   EWMH_GetWindowIconName(ewin);
   EWMH_GetWindowDesktop(ewin);
   EWMH_GetWindowState(ewin);
   EWMH_GetWindowType(ewin);
/*  EWMH_GetWindowIcons(ewin);  TBD */
   EWMH_GetWindowStrut(ewin);
   EDBUG_RETURN_;
}

/*
 * Delete all (_NET_...) properties set on window
 */
void
EWMH_DelWindowHints(const EWin * ewin)
{
   EDBUG(6, "EWMH_DelWindowHints");
   XDeleteProperty(disp, ewin->client.win, _NET_WM_DESKTOP);
   XDeleteProperty(disp, ewin->client.win, _NET_WM_STATE);
   EDBUG_RETURN_;
}

/*
 * Process configuration requests from clients
 */
static int
do_set(int is_set, int action)
{
   switch (action)
     {
     case _NET_WM_STATE_REMOVE:
	return 0;
	break;
     case _NET_WM_STATE_ADD:
	return 1;
	break;
     case _NET_WM_STATE_TOGGLE:
	return !is_set;
	break;
     }
   return -1;
}

void
EWMH_ProcessClientMessage(XClientMessageEvent * event)
{
   EWin               *ewin;

   EDBUG(6, "EWMH_ProcessClientMessage");

   /*
    * The ones that don't target an application window
    */
   if (event->message_type == _NET_CURRENT_DESKTOP)
     {
	GotoDesktop(event->data.l[0]);
	goto done;
     }
   else if (event->message_type == _NET_DESKTOP_VIEWPORT)
     {
	SetCurrentArea(event->data.l[0] / VRoot.w, event->data.l[1] / VRoot.h);
	goto done;
     }
   else if (event->message_type == _NET_SHOWING_DESKTOP)
     {
	Eprintf("EWMH_ProcessClientMessage: _NET_SHOWING_DESKTOP: %ld\n",
		event->data.l[0]);
	EWMH_SetShowingDesktop(event->data.l[0]);
	goto done;
     }

   /*
    * The ones that do target an application window
    */
   ewin = FindItem(NULL, event->window, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin == NULL)
      goto done;

   if (event->message_type == _NET_ACTIVE_WINDOW)
     {
	if (ewin->iconified)
	   DeIconifyEwin(ewin);
	RaiseEwin(ewin);
	if (ewin->shaded)
	   EwinUnShade(ewin);
	FocusToEWin(ewin, FOCUS_SET);
     }
   else if (event->message_type == _NET_CLOSE_WINDOW)
     {
	ActionsCall(ACTION_KILL, ewin, NULL);
     }
   else if (event->message_type == _NET_WM_DESKTOP)
     {
	if ((unsigned)event->data.l[0] == 0xFFFFFFFF)
	  {
	     if (!ewin->sticky)
		EwinStick(ewin);
	  }
	else
	  {
	     if (ewin->sticky)
		EwinUnStick(ewin);
	     else
		MoveEwinToDesktop(ewin, event->data.l[0]);
	  }
     }
   else if (event->message_type == _NET_WM_STATE)
     {
	/*
	 * It is assumed(!) that only the MAXIMIZE H/V ones can be set
	 * in one message.
	 */
	int                 action;
	Atom                atom, atom2;

	action = event->data.l[0];
	atom = event->data.l[1];
	atom2 = event->data.l[2];
	if (atom == _NET_WM_STATE_STICKY)
	  {
	     action = do_set(ewin->sticky, action);
	     if (action)
		EwinStick(ewin);
	     else
		EwinUnStick(ewin);
	     ewin->sticky = action;
	  }
	else if (atom == _NET_WM_STATE_SHADED)
	  {
	     action = do_set(ewin->shaded, action);
	     if (action)
		EwinShade(ewin);
	     else
		EwinUnShade(ewin);
	     ewin->shaded = action;
	  }
	else if (atom == _NET_WM_STATE_SKIP_TASKBAR)
	  {
	     action = do_set(ewin->skiptask, action);
	     ewin->skiptask = action;
	     /* Set _NET_WM_STATE ? */
	  }
	else if (atom == _NET_WM_STATE_SKIP_PAGER)
	  {
	     action = do_set(ewin->skip_ext_pager, action);
	     ewin->skip_ext_pager = action;
	     /* Set _NET_WM_STATE ? */
	  }
	else if (atom == _NET_WM_STATE_MAXIMIZED_VERT ||
		 atom == _NET_WM_STATE_MAXIMIZED_HORZ)
	  {
	     void                (*func) (EWin *, const char *);
	     int                 maxh, maxv;

	     maxh = ewin->st.maximized_horz;
	     maxv = ewin->st.maximized_vert;
	     if (atom2 == _NET_WM_STATE_MAXIMIZED_VERT || atom2 == _NET_WM_STATE_MAXIMIZED_HORZ)	/* (ok - ok) */
	       {
		  func = MaxSize;
		  maxh = do_set(maxh, action);
		  maxv = do_set(maxv, action);
	       }
	     else if (atom == _NET_WM_STATE_MAXIMIZED_VERT)
	       {
		  func = MaxHeight;
		  maxv = do_set(maxv, action);
	       }
	     else
	       {
		  func = MaxWidth;
		  maxh = do_set(maxh, action);
	       }

	     if ((ewin->st.maximized_horz == maxh) &&
		 (ewin->st.maximized_vert == maxv))
		goto done;

	     if ((ewin->st.maximized_horz && !maxh) ||
		 (ewin->st.maximized_vert && !maxv))
		ewin->toggle = 1;
	     else
		ewin->toggle = 0;

	     func(ewin, "available");
	     EWMH_SetWindowState(ewin);
	  }
	else if (atom == _NET_WM_STATE_FULLSCREEN)
	  {
	     action = do_set(ewin->st.fullscreen, action);
	     if (ewin->st.fullscreen == action)
		goto done;

	     ActionsCall(ACTION_FULLSCREEN, ewin, (action) ? "on" : "off");
	  }
	else if (atom == _NET_WM_STATE_ABOVE)
	  {
	     action = do_set(ewin->layer >= 6, action);
	     if (action)
		ActionsCall(ACTION_SET_LAYER, ewin, "6");
	     else
		ActionsCall(ACTION_SET_LAYER, ewin, "4");
	  }
	else if (atom == _NET_WM_STATE_BELOW)
	  {
	     action = do_set(ewin->layer <= 2, action);
	     if (action)
		ActionsCall(ACTION_SET_LAYER, ewin, "2");
	     else
		ActionsCall(ACTION_SET_LAYER, ewin, "4");
	  }
     }
   else if (event->message_type == _NET_WM_MOVERESIZE)
     {
	switch (event->data.l[2])
	  {
	  case _NET_WM_MOVERESIZE_SIZE_KEYBOARD:
	     /* doResize(NULL); */
	     break;
	  case _NET_WM_MOVERESIZE_MOVE_KEYBOARD:
	     /* doMove(NULL); */
	     break;
	  }
     }

 done:
   EDBUG_RETURN_;
}

/*
 * Process received window property change
 */
void
EWMH_ProcessPropertyChange(EWin * ewin, Atom atom_change)
{
   EDBUG(6, "EWMH_ProcessPropertyChange");

   if (atom_change == _NET_WM_NAME)
      EWMH_GetWindowName(ewin);
   else if (atom_change == _NET_WM_ICON_NAME)
      EWMH_GetWindowIconName(ewin);
   else if (atom_change == _NET_WM_STRUT)
      EWMH_GetWindowStrut(ewin);
   else if (atom_change == _NET_WM_WINDOW_OPACITY)
      EWMH_GetWindowOpacity(ewin);

   EDBUG_RETURN_;
}
