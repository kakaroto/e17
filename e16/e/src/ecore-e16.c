/*
 * Copyright (C) 2004 Kim Woelders
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
 * Stuff for compiling without ecore_x.
 */
#include "E.h"

#define _ATOM_GET(name) \
   XInternAtom(_ecore_x_disp, name, False)

#define _ATOM_SET_STRING(win, atom, string) \
   XChangeProperty(_ecore_x_disp, win, atom, XA_STRING, 8, PropModeReplace, \
                   (unsigned char *)string, strlen(string))
#define _ATOM_SET_STRING_LIST(win, atom, string, cnt) \
   XChangeProperty(_ecore_x_disp, win, atom, XA_STRING, 8, PropModeReplace, \
                   (unsigned char *)string, cnt)
#define _ATOM_SET_UTF8_STRING(win, atom, string) \
   XChangeProperty(_ecore_x_disp, win, atom, ECORE_X_ATOM_UTF8_STRING, 8, PropModeReplace, \
                   (unsigned char *)string, strlen(string))
#define _ATOM_SET_UTF8_STRING_LIST(win, atom, string, cnt) \
   XChangeProperty(_ecore_x_disp, win, atom, ECORE_X_ATOM_UTF8_STRING, 8, PropModeReplace, \
                   (unsigned char *)string, cnt)
#define _ATOM_SET_WINDOW(win, atom, p_wins, cnt) \
   XChangeProperty(_ecore_x_disp, win, atom, XA_WINDOW, 32, PropModeReplace, \
                   (unsigned char *)p_wins, cnt)
#define _ATOM_SET_ATOM(win, atom, p_atom, cnt) \
   XChangeProperty(_ecore_x_disp, win, atom, XA_ATOM, 32, PropModeReplace, \
                   (unsigned char *)p_atom, cnt)
#define _ATOM_SET_CARD32(win, atom, p_val, cnt) \
   XChangeProperty(_ecore_x_disp, win, atom, XA_CARDINAL, 32, PropModeReplace, \
                   (unsigned char *)p_val, cnt)

/* Window property change actions (must match _NET_WM_STATE_... ones) */
#define _PROP_CHANGE_REMOVE    0
#define _PROP_CHANGE_ADD       1
#define _PROP_CHANGE_TOGGLE    2

#ifdef USE_ECORE_X

extern Display     *_ecore_x_disp;

/* WM identification */
Ecore_X_Atom        ECORE_X_ATOM_NET_SUPPORTED = 0;
Ecore_X_Atom        ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK = 0;

/* Startup notification */
Ecore_X_Atom        ECORE_X_ATOM_NET_STARTUP_INFO_BEGIN;
Ecore_X_Atom        ECORE_X_ATOM_NET_STARTUP_INFO;

void
ecore_x_icccm_state_set_iconic(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, ECORE_X_WINDOW_STATE_HINT_ICONIC);
}

void
ecore_x_icccm_state_set_normal(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, ECORE_X_WINDOW_STATE_HINT_NORMAL);
}

void
ecore_x_icccm_state_set_withdrawn(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, ECORE_X_WINDOW_STATE_HINT_WITHDRAWN);
}

#else /* USE_ECORE_X */

/*
 * General stuff
 */

/*
 * Send client message (format 32)
 */
int
ecore_x_client_message32_send(Window win, Atom type, long mask, long d0,
			      long d1, long d2, long d3, long d4)
{
   XEvent              xev;

   xev.xclient.type = ClientMessage;
   xev.xclient.window = win;
   xev.xclient.message_type = type;
   xev.xclient.format = 32;
   xev.xclient.data.l[0] = d0;
   xev.xclient.data.l[1] = d1;
   xev.xclient.data.l[2] = d2;
   xev.xclient.data.l[3] = d3;
   xev.xclient.data.l[4] = d4;

   return XSendEvent(_ecore_x_disp, win, False, mask, &xev);
}

/*
 * Set CARD32 (array) property
 */
void
ecore_x_window_prop_card32_set(Ecore_X_Window win, Ecore_X_Atom atom,
			       unsigned int *val, unsigned int num)
{
#if SIZEOF_INT == 4
   _ATOM_SET_CARD32(win, atom, val, num);
#else
   CARD32             *c32;
   unsigned int        i;

   c32 = malloc(num * sizeof(CARD32));
   if (!c32)
      return;
   for (i = 0; i < num; i++)
      c32[i] = val[i];
   _ATOM_SET_CARD32(win, atom, c32, num);
   free(c32);
#endif
}

/*
 * Get CARD32 (array) property
 *
 * At most len items are returned in val.
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_card32_get(Ecore_X_Window win, Ecore_X_Atom atom,
			       unsigned int *val, unsigned int len)
{
   unsigned char      *prop_ret;
   Atom                type_ret;
   unsigned long       bytes_after, num_ret;
   int                 format_ret;
   unsigned int        i;
   int                 num;

   prop_ret = NULL;
   if (XGetWindowProperty(_ecore_x_disp, win, atom, 0, 0x7fffffff, False,
			  XA_CARDINAL, &type_ret, &format_ret, &num_ret,
			  &bytes_after, &prop_ret) != Success)
      return -1;

   if (type_ret == None)
     {
	num = 0;
     }
   else if (prop_ret && type_ret == XA_CARDINAL && format_ret == 32)
     {
	if (num_ret < len)
	   len = num_ret;
	for (i = 0; i < len; i++)
	   val[i] = ((unsigned long *)prop_ret)[i];
	num = len;
     }
   else
     {
	num = -1;
     }
   if (prop_ret)
      XFree(prop_ret);

   return num;
}

#if 0				/* Unused */
/*
 * Set simple string property
 * NB! No encoding conversion done.
 */
void
ecore_x_window_prop_string_set(Ecore_X_Window win, Ecore_X_Atom atom,
			       const char *str)
{
   _ATOM_SET_STRING(win, atom, str);
}
#endif

/*
 * Get simple string property
 */
char               *
ecore_x_window_prop_string_get(Ecore_X_Window win, Ecore_X_Atom atom)
{
   XTextProperty       xtp;
   char               *str = NULL;

   if (XGetTextProperty(_ecore_x_disp, win, &xtp, atom))
     {
	int                 items;
	char              **list;
	Status              s;

	if (xtp.format == 8)
	  {
	     s = XmbTextPropertyToTextList(_ecore_x_disp, &xtp, &list, &items);
	     if ((s == Success) && (items > 0))
	       {
		  str = Estrdup(*list);
		  XFreeStringList(list);
	       }
	     else
		str = Estrdup((char *)xtp.value);
	  }
	else
	   str = Estrdup((char *)xtp.value);
	XFree(xtp.value);
     }

   return str;
}

/*
 * Set UTF-8 string property
 */
static void
_ecore_x_window_prop_string_utf8_set(Ecore_X_Window win, Ecore_X_Atom atom,
				     const char *str)
{
   _ATOM_SET_UTF8_STRING(win, atom, str);
}

/*
 * Get UTF-8 string property
 */
static char        *
_ecore_x_window_prop_string_utf8_get(Ecore_X_Window win, Ecore_X_Atom atom)
{
   char               *str;
   unsigned char      *prop_ret;
   Atom                type_ret;
   unsigned long       bytes_after, num_ret;
   int                 format_ret;

   str = NULL;
   prop_ret = NULL;
   XGetWindowProperty(_ecore_x_disp, win, atom, 0, 0x7fffffff, False,
		      ECORE_X_ATOM_UTF8_STRING, &type_ret,
		      &format_ret, &num_ret, &bytes_after, &prop_ret);
   if (prop_ret && num_ret > 0 && format_ret == 8)
     {
	str = malloc(num_ret + 1);
	if (str)
	  {
	     memcpy(str, prop_ret, num_ret);
	     str[num_ret] = '\0';
	  }
     }
   if (prop_ret)
      XFree(prop_ret);

   return str;
}
#endif /* USE_ECORE_X */

/*
 * Set X ID (array) property
 */
void
ecore_x_window_prop_xid_set(Ecore_X_Window win, Ecore_X_Atom atom,
			    Ecore_X_Atom type, Ecore_X_ID * lst,
			    unsigned int num)
{
   XChangeProperty(_ecore_x_disp, win, atom, type, 32, PropModeReplace,
		   (unsigned char *)lst, num);
}

/*
 * Get X ID (array) property
 *
 * At most len items are returned in val.
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_xid_get(Ecore_X_Window win, Ecore_X_Atom atom,
			    Ecore_X_Atom type, Ecore_X_ID * lst,
			    unsigned int len)
{
   unsigned char      *prop_ret;
   Atom                type_ret;
   unsigned long       bytes_after, num_ret;
   int                 format_ret;
   int                 num;
   unsigned            i;

   prop_ret = NULL;
   if (XGetWindowProperty(_ecore_x_disp, win, atom, 0, 0x7fffffff, False,
			  type, &type_ret, &format_ret, &num_ret,
			  &bytes_after, &prop_ret) != Success)
      return -1;

   if (type_ret == None)
     {
	num = 0;
     }
   else if (prop_ret && type_ret == type && format_ret == 32)
     {
	if (num_ret < len)
	   len = num_ret;
	for (i = 0; i < len; i++)
	   lst[i] = ((unsigned long *)prop_ret)[i];
	num = len;
     }
   else
     {
	num = -1;
     }
   if (prop_ret)
      XFree(prop_ret);

   return num;
}

/*
 * Get X ID (array) property
 *
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * The returned array must be freed with free().
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_xid_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				 Ecore_X_Atom type, Ecore_X_ID ** val)
{
   unsigned char      *prop_ret;
   Atom                type_ret;
   unsigned long       bytes_after, num_ret;
   int                 format_ret;
   Ecore_X_Atom       *alst;
   int                 num;
   unsigned            i;

   *val = NULL;
   prop_ret = NULL;
   if (XGetWindowProperty(_ecore_x_disp, win, atom, 0, 0x7fffffff, False,
			  type, &type_ret, &format_ret, &num_ret,
			  &bytes_after, &prop_ret) != Success)
      return -1;

   if (type_ret == None)
     {
	num = 0;
     }
   else if (prop_ret && type_ret == type && format_ret == 32)
     {
	alst = malloc(num_ret * sizeof(Ecore_X_ID));
	for (i = 0; i < num_ret; i++)
	   alst[i] = ((unsigned long *)prop_ret)[i];
	*val = alst;
	num = num_ret;
     }
   else
     {
	num = -1;
     }
   if (prop_ret)
      XFree(prop_ret);

   return num;
}

/*
 * Set Atom (array) property
 */
void
ecore_x_window_prop_atom_set(Ecore_X_Window win, Ecore_X_Atom atom,
			     Ecore_X_Atom * lst, unsigned int num)
{
   ecore_x_window_prop_xid_set(win, atom, XA_ATOM, lst, num);
}

/*
 * Get Atom (array) property
 *
 * At most len items are returned in val.
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_atom_get(Ecore_X_Window win, Ecore_X_Atom atom,
			     Ecore_X_Atom * lst, unsigned int len)
{
   return ecore_x_window_prop_xid_get(win, atom, XA_ATOM, lst, len);
}

/*
 * Get Atom (array) property
 *
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * The returned array must be freed with free().
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_atom_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				  Ecore_X_Atom ** plst)
{
   return ecore_x_window_prop_xid_list_get(win, atom, XA_ATOM, plst);
}

/*
 * Remove/add/toggle (0/1/2) atom list item.
 */
void
ecore_x_window_prop_atom_list_change(Ecore_X_Window win, Ecore_X_Atom atom,
				     Ecore_X_Atom item, int op)
{
   Ecore_X_Atom       *alst;
   int                 i, num;

   num = ecore_x_window_prop_atom_list_get(win, atom, &alst);
   if (num < 0)
      return;			/* Error - assuming invalid window */

   /* Is it there? */
   for (i = 0; i < num; i++)
     {
	if (alst[i] == item)
	   break;
     }

   if (i < num)
     {
	/* Was in list */
	if (op == _PROP_CHANGE_ADD)
	   goto done;
	/* Remove it */
	num--;
	for (; i < num; i++)
	   alst[i] = alst[i + 1];
     }
   else
     {
	/* Was not in list */
	if (op == _PROP_CHANGE_REMOVE)
	   goto done;
	/* Add it */
	num++;
	alst = realloc(alst, num * sizeof(Ecore_X_Atom));
	alst[i] = item;
     }

   ecore_x_window_prop_atom_set(win, atom, alst, num);

 done:
   if (alst)
      free(alst);
}

/*
 * Set Window (array) property
 */
void
ecore_x_window_prop_window_set(Ecore_X_Window win, Ecore_X_Atom atom,
			       Ecore_X_Window * lst, unsigned int num)
{
   ecore_x_window_prop_xid_set(win, atom, XA_WINDOW, lst, num);
}

/*
 * Get Window (array) property
 *
 * At most len items are returned in val.
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_window_get(Ecore_X_Window win, Ecore_X_Atom atom,
			       Ecore_X_Window * lst, unsigned int len)
{
   return ecore_x_window_prop_xid_get(win, atom, XA_WINDOW, lst, len);
}

/*
 * Get Window (array) property
 *
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * The returned array must be freed with free().
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_window_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				    Ecore_X_Window ** plst)
{
   return ecore_x_window_prop_xid_list_get(win, atom, XA_WINDOW, plst);
}

#ifndef USE_ECORE_X
/*
 * ICCCM stuff
 */
Ecore_X_Atom        ECORE_X_ATOM_WM_STATE;
Ecore_X_Atom        ECORE_X_ATOM_WM_NAME;
Ecore_X_Atom        ECORE_X_ATOM_WM_ICON_NAME;
Ecore_X_Atom        ECORE_X_ATOM_WM_CLASS;
Ecore_X_Atom        ECORE_X_ATOM_WM_WINDOW_ROLE;
Ecore_X_Atom        ECORE_X_ATOM_WM_NORMAL_HINTS;
Ecore_X_Atom        ECORE_X_ATOM_WM_HINTS;
Ecore_X_Atom        ECORE_X_ATOM_WM_COMMAND;
Ecore_X_Atom        ECORE_X_ATOM_WM_CLIENT_MACHINE;
Ecore_X_Atom        ECORE_X_ATOM_WM_CLIENT_LEADER;
Ecore_X_Atom        ECORE_X_ATOM_WM_TRANSIENT_FOR;

Ecore_X_Atom        ECORE_X_ATOM_WM_COLORMAP_WINDOWS;

Ecore_X_Atom        ECORE_X_ATOM_WM_CHANGE_STATE;

Ecore_X_Atom        ECORE_X_ATOM_WM_PROTOCOLS;
Ecore_X_Atom        ECORE_X_ATOM_WM_DELETE_WINDOW;
Ecore_X_Atom        ECORE_X_ATOM_WM_TAKE_FOCUS;

#if 0
Ecore_X_Atom        ECORE_X_ATOM_WM_SAVE_YOURSELF;
#endif

void
ecore_x_icccm_init(void)
{
   ECORE_X_ATOM_WM_STATE = XInternAtom(disp, "WM_STATE", False);
   ECORE_X_ATOM_WM_NAME = XInternAtom(disp, "WM_NAME", False);
   ECORE_X_ATOM_WM_ICON_NAME = XInternAtom(disp, "WM_ICON_NAME", False);
   ECORE_X_ATOM_WM_CLASS = XInternAtom(disp, "WM_CLASS", False);
   ECORE_X_ATOM_WM_WINDOW_ROLE = XInternAtom(disp, "WM_WINDOW_ROLE", False);
   ECORE_X_ATOM_WM_NORMAL_HINTS = XInternAtom(disp, "WM_NORMAL_HINTS", False);
   ECORE_X_ATOM_WM_HINTS = XInternAtom(disp, "WM_HINTS", False);
   ECORE_X_ATOM_WM_COMMAND = XInternAtom(disp, "WM_COMMAND", False);
   ECORE_X_ATOM_WM_CLIENT_MACHINE =
      XInternAtom(disp, "WM_CLIENT_MACHINE", False);
   ECORE_X_ATOM_WM_CLIENT_LEADER = XInternAtom(disp, "WM_CLIENT_LEADER", False);
   ECORE_X_ATOM_WM_TRANSIENT_FOR = XInternAtom(disp, "WM_TRANSIENT_FOR", False);

   ECORE_X_ATOM_WM_COLORMAP_WINDOWS =
      XInternAtom(disp, "WM_COLORMAP_WINDOWS", False);

   ECORE_X_ATOM_WM_CHANGE_STATE = XInternAtom(disp, "WM_CHANGE_STATE", False);

   ECORE_X_ATOM_WM_PROTOCOLS = XInternAtom(disp, "WM_PROTOCOLS", False);
   ECORE_X_ATOM_WM_DELETE_WINDOW = XInternAtom(disp, "WM_DELETE_WINDOW", False);
   ECORE_X_ATOM_WM_TAKE_FOCUS = XInternAtom(disp, "WM_TAKE_FOCUS", False);
#if 0
   ECORE_X_ATOM_WM_SAVE_YOURSELF = XInternAtom(disp, "WM_SAVE_YOURSELF", False);
#endif
}

static void
ecore_x_icccm_state_set(Ecore_X_Window win, unsigned int state)
{
   unsigned long       c[2];

   c[0] = state;
   c[1] = 0;
   XChangeProperty(_ecore_x_disp, win, ECORE_X_ATOM_WM_STATE,
		   ECORE_X_ATOM_WM_STATE, 32, PropModeReplace,
		   (unsigned char *)c, 2);
}

void
ecore_x_icccm_state_set_iconic(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, IconicState);
}

void
ecore_x_icccm_state_set_normal(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, NormalState);
}

void
ecore_x_icccm_state_set_withdrawn(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, WithdrawnState);
}

static void
ecore_x_icccm_client_message_send(Ecore_X_Window win,
				  Ecore_X_Atom atom, Ecore_X_Time ts)
{
   ecore_x_client_message32_send(win, ECORE_X_ATOM_WM_PROTOCOLS, NoEventMask,
				 atom, ts, 0, 0, 0);
}

void
ecore_x_icccm_delete_window_send(Ecore_X_Window win, Ecore_X_Time ts)
{
   ecore_x_icccm_client_message_send(win, ECORE_X_ATOM_WM_DELETE_WINDOW, ts);
}

void
ecore_x_icccm_take_focus_send(Ecore_X_Window win, Ecore_X_Time ts)
{
   ecore_x_icccm_client_message_send(win, ECORE_X_ATOM_WM_TAKE_FOCUS, ts);
}

#if 0
void
ecore_x_icccm_save_yourself_send(Ecore_X_Window win, Ecore_X_Time ts)
{
   ecore_x_icccm_client_message_send(win, ECORE_X_ATOM_WM_SAVE_YOURSELF, ts);
}
#endif

char               *
ecore_x_icccm_title_get(Ecore_X_Window win)
{
   return ecore_x_window_prop_string_get(win, ECORE_X_ATOM_WM_NAME);
}

#endif /* USE_ECORE_X */

#ifndef USE_ECORE_X
/*
 * _NET_WM hints (EWMH)
 */
Ecore_X_Atom        ECORE_X_ATOM_UTF8_STRING;

/* Window manager info */
Ecore_X_Atom        ECORE_X_ATOM_NET_SUPPORTED;
Ecore_X_Atom        ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK;

/* Desktop status/requests */
Ecore_X_Atom        ECORE_X_ATOM_NET_NUMBER_OF_DESKTOPS;
Ecore_X_Atom        ECORE_X_ATOM_NET_VIRTUAL_ROOTS;
Ecore_X_Atom        ECORE_X_ATOM_NET_DESKTOP_NAMES;
Ecore_X_Atom        ECORE_X_ATOM_NET_DESKTOP_GEOMETRY;
Ecore_X_Atom        ECORE_X_ATOM_NET_WORKAREA;
Ecore_X_Atom        ECORE_X_ATOM_NET_CURRENT_DESKTOP;
Ecore_X_Atom        ECORE_X_ATOM_NET_DESKTOP_VIEWPORT;
Ecore_X_Atom        ECORE_X_ATOM_NET_SHOWING_DESKTOP;

Ecore_X_Atom        ECORE_X_ATOM_NET_ACTIVE_WINDOW;
Ecore_X_Atom        ECORE_X_ATOM_NET_CLIENT_LIST;
Ecore_X_Atom        ECORE_X_ATOM_NET_CLIENT_LIST_STACKING;

/* Client window props/client messages */
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_NAME;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_VISIBLE_NAME;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_ICON_NAME;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_VISIBLE_ICON_NAME;

Ecore_X_Atom        ECORE_X_ATOM_NET_WM_DESKTOP;

Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE;

Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DESKTOP;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DOCK;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE_TOOLBAR;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE_MENU;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE_UTILITY;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE_SPLASH;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DIALOG;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_TYPE_NORMAL;

Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE;

Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_MODAL;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_STICKY;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_MAXIMIZED_VERT;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_MAXIMIZED_HORZ;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_SHADED;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_SKIP_TASKBAR;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_SKIP_PAGER;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_HIDDEN;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_FULLSCREEN;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_ABOVE;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_BELOW;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STATE_DEMANDS_ATTENTION;

Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STRUT;
Ecore_X_Atom        ECORE_X_ATOM_NET_FRAME_EXTENTS;

#if 0				/* Not used */
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_ALLOWED_ACTIONS;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_STRUT_PARTIAL;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_ICON_GEOMETRY;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_ICON;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_PID;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_HANDLED_ICONS;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_USER_TIME;

Ecore_X_Atom        ECORE_X_ATOM_NET_WM_PING;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_SYNC_REQUEST;
#endif

Ecore_X_Atom        ECORE_X_ATOM_NET_WM_WINDOW_OPACITY;

/* Misc window ops */
Ecore_X_Atom        ECORE_X_ATOM_NET_CLOSE_WINDOW;
Ecore_X_Atom        ECORE_X_ATOM_NET_WM_MOVERESIZE;

#if 0				/* Not yet implemented */
Ecore_X_Atom        ECORE_X_ATOM_NET_MOVERESIZE_WINDOW;
Ecore_X_Atom        ECORE_X_ATOM_NET_RESTACK_WINDOW;
Ecore_X_Atom        ECORE_X_ATOM_NET_REQUEST_FRAME_EXTENTS;
#endif

/* Startup notification */
Ecore_X_Atom        ECORE_X_ATOM_NET_STARTUP_INFO_BEGIN;
Ecore_X_Atom        ECORE_X_ATOM_NET_STARTUP_INFO;

void
ecore_x_netwm_init(void)
{
   ECORE_X_ATOM_UTF8_STRING = XInternAtom(_ecore_x_disp, "UTF8_STRING", False);

   ECORE_X_ATOM_NET_SUPPORTED = _ATOM_GET("_NET_SUPPORTED");
   ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK = _ATOM_GET("_NET_SUPPORTING_WM_CHECK");

   ECORE_X_ATOM_NET_NUMBER_OF_DESKTOPS = _ATOM_GET("_NET_NUMBER_OF_DESKTOPS");
   ECORE_X_ATOM_NET_VIRTUAL_ROOTS = _ATOM_GET("_NET_VIRTUAL_ROOTS");
   ECORE_X_ATOM_NET_DESKTOP_GEOMETRY = _ATOM_GET("_NET_DESKTOP_GEOMETRY");
   ECORE_X_ATOM_NET_DESKTOP_NAMES = _ATOM_GET("_NET_DESKTOP_NAMES");
   ECORE_X_ATOM_NET_DESKTOP_VIEWPORT = _ATOM_GET("_NET_DESKTOP_VIEWPORT");
   ECORE_X_ATOM_NET_WORKAREA = _ATOM_GET("_NET_WORKAREA");
   ECORE_X_ATOM_NET_CURRENT_DESKTOP = _ATOM_GET("_NET_CURRENT_DESKTOP");
   ECORE_X_ATOM_NET_SHOWING_DESKTOP = _ATOM_GET("_NET_SHOWING_DESKTOP");

   ECORE_X_ATOM_NET_ACTIVE_WINDOW = _ATOM_GET("_NET_ACTIVE_WINDOW");
   ECORE_X_ATOM_NET_CLIENT_LIST = _ATOM_GET("_NET_CLIENT_LIST");
   ECORE_X_ATOM_NET_CLIENT_LIST_STACKING =
      _ATOM_GET("_NET_CLIENT_LIST_STACKING");

   ECORE_X_ATOM_NET_WM_NAME = _ATOM_GET("_NET_WM_NAME");
   ECORE_X_ATOM_NET_WM_VISIBLE_NAME = _ATOM_GET("_NET_WM_VISIBLE_NAME");
   ECORE_X_ATOM_NET_WM_ICON_NAME = _ATOM_GET("_NET_WM_ICON_NAME");
   ECORE_X_ATOM_NET_WM_VISIBLE_ICON_NAME =
      _ATOM_GET("_NET_WM_VISIBLE_ICON_NAME");

   ECORE_X_ATOM_NET_WM_DESKTOP = _ATOM_GET("_NET_WM_DESKTOP");

   ECORE_X_ATOM_NET_WM_WINDOW_TYPE = _ATOM_GET("_NET_WM_WINDOW_TYPE");

   ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DESKTOP =
      _ATOM_GET("_NET_WM_WINDOW_TYPE_DESKTOP");
   ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DOCK = _ATOM_GET("_NET_WM_WINDOW_TYPE_DOCK");
   ECORE_X_ATOM_NET_WM_WINDOW_TYPE_TOOLBAR =
      _ATOM_GET("_NET_WM_WINDOW_TYPE_TOOLBAR");
   ECORE_X_ATOM_NET_WM_WINDOW_TYPE_MENU = _ATOM_GET("_NET_WM_WINDOW_TYPE_MENU");
   ECORE_X_ATOM_NET_WM_WINDOW_TYPE_UTILITY =
      _ATOM_GET("_NET_WM_WINDOW_TYPE_UTILITY");
   ECORE_X_ATOM_NET_WM_WINDOW_TYPE_SPLASH =
      _ATOM_GET("_NET_WM_WINDOW_TYPE_SPLASH");
   ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DIALOG =
      _ATOM_GET("_NET_WM_WINDOW_TYPE_DIALOG");
   ECORE_X_ATOM_NET_WM_WINDOW_TYPE_NORMAL =
      _ATOM_GET("_NET_WM_WINDOW_TYPE_NORMAL");

   ECORE_X_ATOM_NET_WM_STATE = _ATOM_GET("_NET_WM_STATE");

   ECORE_X_ATOM_NET_WM_STATE_MODAL = _ATOM_GET("_NET_WM_STATE_MODAL");
   ECORE_X_ATOM_NET_WM_STATE_STICKY = _ATOM_GET("_NET_WM_STATE_STICKY");
   ECORE_X_ATOM_NET_WM_STATE_MAXIMIZED_VERT =
      _ATOM_GET("_NET_WM_STATE_MAXIMIZED_VERT");
   ECORE_X_ATOM_NET_WM_STATE_MAXIMIZED_HORZ =
      _ATOM_GET("_NET_WM_STATE_MAXIMIZED_HORZ");
   ECORE_X_ATOM_NET_WM_STATE_SHADED = _ATOM_GET("_NET_WM_STATE_SHADED");
   ECORE_X_ATOM_NET_WM_STATE_SKIP_TASKBAR =
      _ATOM_GET("_NET_WM_STATE_SKIP_TASKBAR");
   ECORE_X_ATOM_NET_WM_STATE_SKIP_PAGER = _ATOM_GET("_NET_WM_STATE_SKIP_PAGER");
   ECORE_X_ATOM_NET_WM_STATE_HIDDEN = _ATOM_GET("_NET_WM_STATE_HIDDEN");
   ECORE_X_ATOM_NET_WM_STATE_FULLSCREEN = _ATOM_GET("_NET_WM_STATE_FULLSCREEN");
   ECORE_X_ATOM_NET_WM_STATE_ABOVE = _ATOM_GET("_NET_WM_STATE_ABOVE");
   ECORE_X_ATOM_NET_WM_STATE_BELOW = _ATOM_GET("_NET_WM_STATE_BELOW");
   ECORE_X_ATOM_NET_WM_STATE_DEMANDS_ATTENTION =
      _ATOM_GET("_NET_WM_STATE_DEMANDS_ATTENTION");

   ECORE_X_ATOM_NET_WM_STRUT = _ATOM_GET("_NET_WM_STRUT");
   ECORE_X_ATOM_NET_FRAME_EXTENTS = _ATOM_GET("_NET_FRAME_EXTENTS");

#if 0				/* Not used */
   ECORE_X_ATOM_NET_WM_ALLOWED_ACTIONS = _ATOM_GET("_NET_WM_ALLOWED_ACTIONS");
   ECORE_X_ATOM_NET_WM_STRUT_PARTIAL = _ATOM_GET("_NET_WM_STRUT_PARTIAL");
   ECORE_X_ATOM_NET_WM_ICON_GEOMETRY = _ATOM_GET("_NET_WM_ICON_GEOMETRY");
   ECORE_X_ATOM_NET_WM_ICON = _ATOM_GET("_NET_WM_ICON");
   ECORE_X_ATOM_NET_WM_PID = _ATOM_GET("_NET_WM_PID");
   ECORE_X_ATOM_NET_WM_HANDLED_ICONS = _ATOM_GET("_NET_WM_HANDLED_ICONS");
   ECORE_X_ATOM_NET_WM_USER_TIME = _ATOM_GET("_NET_WM_USER_TIME");

   ECORE_X_ATOM_NET_WM_PING = _ATOM_GET("_NET_WM_PING");
   ECORE_X_ATOM_NET_WM_SYNC_REQUEST = _ATOM_GET("_NET_WM_SYNC_REQUEST");
#endif

   ECORE_X_ATOM_NET_WM_WINDOW_OPACITY = _ATOM_GET("_NET_WM_WINDOW_OPACITY");

   ECORE_X_ATOM_NET_CLOSE_WINDOW = _ATOM_GET("_NET_CLOSE_WINDOW");
   ECORE_X_ATOM_NET_WM_MOVERESIZE = _ATOM_GET("_NET_WM_MOVERESIZE");

#if 0				/* Not yet implemented */
   ECORE_X_ATOM_NET_MOVERESIZE_WINDOW = _ATOM_GET("_NET_MOVERESIZE_WINDOW");
   ECORE_X_ATOM_NET_RESTACK_WINDOW = _ATOM_GET("_NET_RESTACK_WINDOW");
   ECORE_X_ATOM_NET_REQUEST_FRAME_EXTENTS =
      _ATOM_GET("_NET_REQUEST_FRAME_EXTENTS");
#endif

   ECORE_X_ATOM_NET_STARTUP_INFO_BEGIN = _ATOM_GET("_NET_STARTUP_INFO_BEGIN");
   ECORE_X_ATOM_NET_STARTUP_INFO = _ATOM_GET("_NET_STARTUP_INFO");
}

/*
 * WM identification
 */
void
ecore_x_netwm_wm_identify(Ecore_X_Window root, Ecore_X_Window check,
			  const char *wm_name)
{
   _ATOM_SET_WINDOW(root, ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK, &check, 1);
   _ATOM_SET_WINDOW(check, ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK, &check, 1);
   _ATOM_SET_UTF8_STRING(check, ECORE_X_ATOM_NET_WM_NAME, wm_name);
   /* This one isn't mandatory */
   _ATOM_SET_UTF8_STRING(root, ECORE_X_ATOM_NET_WM_NAME, wm_name);
}

/*
 * Desktop configuration and status
 */

void
ecore_x_netwm_desk_count_set(Ecore_X_Window root, unsigned int n_desks)
{
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_NUMBER_OF_DESKTOPS,
				  &n_desks, 1);
}

void
ecore_x_netwm_desk_roots_set(Ecore_X_Window root, unsigned int n_desks,
			     Ecore_X_Window * vroots)
{
   _ATOM_SET_WINDOW(root, ECORE_X_ATOM_NET_VIRTUAL_ROOTS, vroots, n_desks);
}

void
ecore_x_netwm_desk_names_set(Ecore_X_Window root, unsigned int n_desks,
			     const char **names)
{
   char                ss[32], *buf;
   const char         *s;
   unsigned int        i;
   int                 l, len;

   buf = NULL;
   len = 0;

   for (i = 0; i < n_desks; i++)
     {
	s = (names) ? names[i] : NULL;
	if (!s)
	  {
	     /* Default to "Desk-<number>" */
	     sprintf(ss, "Desk-%d", i);
	     s = ss;
	  }

	l = strlen(s) + 1;
	buf = realloc(buf, len + l);
	memcpy(buf + len, s, l);
	len += l;
     }

   _ATOM_SET_UTF8_STRING_LIST(root, ECORE_X_ATOM_NET_DESKTOP_NAMES, buf, len);

   free(buf);
}

void
ecore_x_netwm_desk_size_set(Ecore_X_Window root, unsigned int width,
			    unsigned int height)
{
   unsigned int        size[2];

   size[0] = width;
   size[1] = height;
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_DESKTOP_GEOMETRY, size,
				  2);
}

void
ecore_x_netwm_desk_workareas_set(Ecore_X_Window root, unsigned int n_desks,
				 unsigned int *areas)
{
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_WORKAREA, areas,
				  4 * n_desks);
}

void
ecore_x_netwm_desk_current_set(Ecore_X_Window root, unsigned int desk)
{
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_CURRENT_DESKTOP, &desk,
				  1);
}

void
ecore_x_netwm_desk_viewports_set(Ecore_X_Window root, unsigned int n_desks,
				 unsigned int *origins)
{
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_DESKTOP_VIEWPORT,
				  origins, 2 * n_desks);
}

void
ecore_x_netwm_showing_desktop_set(Ecore_X_Window root, int on)
{
   unsigned int        val;

   val = (on) ? 1 : 0;
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_SHOWING_DESKTOP, &val,
				  1);
}

/*
 * Client status
 */

/* Mapping order */
void
ecore_x_netwm_client_list_set(Ecore_X_Window root, unsigned int n_clients,
			      Ecore_X_Window * p_clients)
{
   _ATOM_SET_WINDOW(root, ECORE_X_ATOM_NET_CLIENT_LIST, p_clients, n_clients);
}

/* Stacking order */
void
ecore_x_netwm_client_list_stacking_set(Ecore_X_Window root,
				       unsigned int n_clients,
				       Ecore_X_Window * p_clients)
{
   _ATOM_SET_WINDOW(root, ECORE_X_ATOM_NET_CLIENT_LIST_STACKING, p_clients,
		    n_clients);
}

void
ecore_x_netwm_client_active_set(Ecore_X_Window root, Ecore_X_Window win)
{
   _ATOM_SET_WINDOW(root, ECORE_X_ATOM_NET_ACTIVE_WINDOW, &win, 1);
}

/*
 * Client window properties
 */

void
ecore_x_netwm_name_set(Ecore_X_Window win, const char *name)
{
   _ecore_x_window_prop_string_utf8_set(win, ECORE_X_ATOM_NET_WM_NAME, name);
}

char               *
ecore_x_netwm_name_get(Ecore_X_Window win)
{
   return _ecore_x_window_prop_string_utf8_get(win, ECORE_X_ATOM_NET_WM_NAME);
}

void
ecore_x_netwm_visible_name_set(Ecore_X_Window win, const char *name)
{
   _ecore_x_window_prop_string_utf8_set(win, ECORE_X_ATOM_NET_WM_VISIBLE_NAME,
					name);
}

char               *
ecore_x_netwm_visible_name_get(Ecore_X_Window win)
{
   return _ecore_x_window_prop_string_utf8_get(win,
					       ECORE_X_ATOM_NET_WM_VISIBLE_NAME);
}

void
ecore_x_netwm_icon_name_set(Ecore_X_Window win, const char *name)
{
   _ecore_x_window_prop_string_utf8_set(win, ECORE_X_ATOM_NET_WM_ICON_NAME,
					name);
}

char               *
ecore_x_netwm_icon_name_get(Ecore_X_Window win)
{
   return _ecore_x_window_prop_string_utf8_get(win,
					       ECORE_X_ATOM_NET_WM_ICON_NAME);
}

void
ecore_x_netwm_visible_icon_name_set(Ecore_X_Window win, const char *name)
{
   _ecore_x_window_prop_string_utf8_set(win,
					ECORE_X_ATOM_NET_WM_VISIBLE_ICON_NAME,
					name);
}

char               *
ecore_x_netwm_visible_icon_name_get(Ecore_X_Window win)
{
   return _ecore_x_window_prop_string_utf8_get(win,
					       ECORE_X_ATOM_NET_WM_VISIBLE_ICON_NAME);
}

void
ecore_x_netwm_desktop_set(Ecore_X_Window win, unsigned int desk)
{
   ecore_x_window_prop_card32_set(win, ECORE_X_ATOM_NET_WM_DESKTOP, &desk, 1);
}

int
ecore_x_netwm_desktop_get(Ecore_X_Window win, unsigned int *desk)
{
   return ecore_x_window_prop_card32_get(win, ECORE_X_ATOM_NET_WM_DESKTOP,
					 desk, 1);
}

void
ecore_x_netwm_opacity_set(Ecore_X_Window win, unsigned int opacity)
{
   ecore_x_window_prop_card32_set(win, ECORE_X_ATOM_NET_WM_WINDOW_OPACITY,
				  &opacity, 1);
}

int
ecore_x_netwm_opacity_get(Ecore_X_Window win, unsigned int *opacity)
{
   return ecore_x_window_prop_card32_get(win,
					 ECORE_X_ATOM_NET_WM_WINDOW_OPACITY,
					 opacity, 1);
}

#endif /* USE_ECORE_X */
