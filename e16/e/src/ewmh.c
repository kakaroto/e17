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

#if DEBUG_EWMH
#undef EDBUG
#define EDBUG(a,b) printf(b "\n")
#endif

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

Atom                _NET_ACTIVE_WINDOW;
Atom                _NET_CLIENT_LIST;
Atom                _NET_CLIENT_LIST_STACKING;

/* Misc window ops */
Atom                _NET_CLOSE_WINDOW;

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

/* Window state property change actions */
#define _NET_WM_STATE_REMOVE    0
#define _NET_WM_STATE_ADD       1
#define _NET_WM_STATE_TOGGLE    2

/* EWMH flags (somewhat messy) */
#define NET_WM_FLAG_MAXIMIZED_VERT  0x01
#define NET_WM_FLAG_MAXIMIZED_HORZ  0x02

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

   E_XA_UTF8_STRING = XInternAtom(disp, "UTF8_STRING", False);;

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
#if 0				/* Not implemented */
   _ATOM_INIT(_NET_WM_STATE_FULLSCREEN);
#endif
   _ATOM_INIT(_NET_WM_STATE_ABOVE);
   _ATOM_INIT(_NET_WM_STATE_BELOW);

   _ATOM_SET_ATOM(_NET_SUPPORTED, root.win, atom_list, atom_count);

   /* Set WM info properties */
   _ATOM_SET_UTF8_STRING(_NET_WM_NAME, root.win, e_wm_name);

   _ATOM_SET_WINDOW(_NET_SUPPORTING_WM_CHECK, root.win, &win_wm_check, 1);
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
   _ATOM_SET_CARD32(_NET_NUMBER_OF_DESKTOPS, root.win, &val, 1);

   for (i = 0; i < Conf.desks.num; i++)
     {
	wl[i] = desks.desk[i].win;
     }
   _ATOM_SET_WINDOW(_NET_VIRTUAL_ROOTS, root.win, &wl, Conf.desks.num);

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

   _ATOM_SET_UTF8_STRING_LIST(_NET_DESKTOP_NAMES, root.win, buf, s - buf);

   EDBUG_RETURN_;
}

void
EWMH_SetDesktopSize(void)
{
   CARD32              size[2];
   int                 ax, ay;

   EDBUG(6, "EWMH_SetDesktopSize");
   GetAreaSize(&ax, &ay);
   size[0] = ax * root.w;
   size[1] = ay * root.h;
   _ATOM_SET_CARD32(_NET_DESKTOP_GEOMETRY, root.win, &size, 2);
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
	     p_coord[4 * i + 2] = root.w;
	     p_coord[4 * i + 3] = root.h;
	  }
	_ATOM_SET_CARD32(_NET_WORKAREA, root.win, p_coord, n_coord);
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
   _ATOM_SET_CARD32(_NET_CURRENT_DESKTOP, root.win, &val, 1);
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
	     p_coord[2 * i] = desks.desk[i].current_area_x * root.w;
	     p_coord[2 * i + 1] = desks.desk[i].current_area_y * root.h;
	  }
	_ATOM_SET_CARD32(_NET_DESKTOP_VIEWPORT, root.win, p_coord, n_coord);
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
   int                 i, nwin, num;
   EWin              **lst;

   EDBUG(6, "EWMH_SetClientList");

   /* Mapping order */
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   wl = NULL;
   nwin = 0;
   if (lst)
     {
	wl = Emalloc(sizeof(Window) * num);
	for (i = 0; i < num; i++)
	   wl[nwin++] = lst[i]->client.win;
	_ATOM_SET_WINDOW(_NET_CLIENT_LIST, root.win, wl, nwin);
	Efree(lst);
     }

   /* Stacking order */
   lst = (EWin **) EwinListGetStacking(&num);
   /* FIXME: num must be unchanged here! Check! */
   if (num != nwin)
      printf("*** ERROR: no=%d nn=%d\n", nwin, num);
   if (num > nwin)
      wl = Erealloc(wl, num * sizeof(Window));
   nwin = 0;
   for (i = num - 1; i >= 0; i--)
      wl[nwin++] = lst[i]->client.win;
   _ATOM_SET_WINDOW(_NET_CLIENT_LIST_STACKING, root.win, wl, nwin);

   if (wl)
      Efree(wl);

   EDBUG_RETURN_;
}

void
EWMH_SetActiveWindow(const EWin * ewin)
{
   static Window       win_last_set;
   Window              win;

   EDBUG(6, "EWMH_SetActiveWindow");
   win = (ewin) ? ewin->client.win : None;
   if (win != win_last_set)
     {
	_ATOM_SET_WINDOW(_NET_ACTIVE_WINDOW, root.win, &win, 1);
	win_last_set = win;
     }
   EDBUG_RETURN_;
}

/*
 * Functions that set X11-properties from E-window internals
 */

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
		 ewin->ewmh_flags & NET_WM_FLAG_MAXIMIZED_VERT);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_MAXIMIZED_HORZ,
		 ewin->ewmh_flags & NET_WM_FLAG_MAXIMIZED_HORZ);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_SKIP_PAGER,
		 ewin->skip_ext_pager);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_ABOVE,
		 ewin->layer >= 6);
   atom_list_set(atom_list, len, &atom_count, _NET_WM_STATE_BELOW,
		 ewin->layer <= 2);
   _ATOM_SET_ATOM(_NET_WM_STATE, ewin->client.win, atom_list, atom_count);
   EDBUG_RETURN_;
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
   ewin->ewmh.wm_name = Estrndup(val, size);

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
   ewin->ewmh.wm_icon_name = Estrndup(val, size);

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

   ewin->ewmh_flags = 0;

   n_atoms = 0;
   p_atoms = AtomGet(ewin->client.win, _NET_WM_STATE, XA_ATOM, &n_atoms);
   n_atoms /= sizeof(Atom);	/* Silly */
   if (!p_atoms)
      goto done;

   /* We must clear/set all according to not present/present */
   ewin->sticky = ewin->shaded = 0;
   ewin->skiptask = ewin->skip_ext_pager = 0;
   ewin->ewmh_flags = 0;
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
	   ewin->ewmh_flags |= NET_WM_FLAG_MAXIMIZED_VERT;
	else if (atom == _NET_WM_STATE_MAXIMIZED_HORZ)
	   ewin->ewmh_flags |= NET_WM_FLAG_MAXIMIZED_HORZ;
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

void
EWMH_GetWindowHints(EWin * ewin)
{
   EDBUG(6, "EWMH_GetWindowHints");
   EWMH_GetWindowName(ewin);
   EWMH_GetWindowIconName(ewin);
   EWMH_GetWindowDesktop(ewin);
   EWMH_GetWindowState(ewin);
   EWMH_GetWindowType(ewin);
/*  EWMH_GetWindowIcons(ewin);  TBD */
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
	SetCurrentArea(event->data.l[0] / root.w, event->data.l[1] / root.h);
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
	   UnShadeEwin(ewin);
	FocusToEWin(ewin, FOCUS_SET);
     }
   else if (event->message_type == _NET_CLOSE_WINDOW)
     {
	KillEwin(ewin, 0);
     }
   else if (event->message_type == _NET_WM_DESKTOP)
     {
	if ((unsigned)event->data.l[0] == 0xFFFFFFFF)
	  {
	     if (!ewin->sticky)
		MakeWindowSticky(ewin);
	  }
	else
	  {
	     if (ewin->sticky)
		MakeWindowUnSticky(ewin);
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
		MakeWindowSticky(ewin);
	     else
		MakeWindowUnSticky(ewin);
	     ewin->sticky = action;
	  }
	else if (atom == _NET_WM_STATE_SHADED)
	  {
	     action = do_set(ewin->shaded, action);
	     if (action)
		ShadeEwin(ewin);
	     else
		UnShadeEwin(ewin);
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
	     int                 maskbits;

	     if (atom2 == _NET_WM_STATE_MAXIMIZED_VERT || atom2 == _NET_WM_STATE_MAXIMIZED_HORZ)	/* (ok - ok) */
	       {
		  func = MaxSize;
		  maskbits = NET_WM_FLAG_MAXIMIZED_VERT |
		     NET_WM_FLAG_MAXIMIZED_HORZ;
	       }
	     else if (atom == _NET_WM_STATE_MAXIMIZED_VERT)
	       {
		  func = MaxHeight;
		  maskbits = NET_WM_FLAG_MAXIMIZED_VERT;
	       }
	     else
	       {
		  func = MaxWidth;
		  maskbits = NET_WM_FLAG_MAXIMIZED_HORZ;
	       }

	     if (ewin->ewmh_flags & maskbits)
	       {
		  if (action != _NET_WM_STATE_ADD)
		    {
		       ewin->ewmh_flags &= ~maskbits;
		       ewin->toggle = 1;
		    }
	       }
	     else
	       {
		  if (action != _NET_WM_STATE_REMOVE)
		    {
		       ewin->ewmh_flags |= maskbits;
		       ewin->toggle = 0;
		    }
	       }
	     func(ewin, "available");
	     RememberImportantInfoForEwin(ewin);
	     EWMH_SetWindowState(ewin);
	     ewin->toggle = 0;
	  }
#if 0				/* Not yet implemented */
	else if (atom == _NET_WM_STATE_FULLSCREEN)
	  {
	  }
#endif
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

   EDBUG_RETURN_;
}
