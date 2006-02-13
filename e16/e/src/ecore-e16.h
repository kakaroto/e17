/*
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
#ifdef USE_ECORE_X

#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>

#else

#define Ecore_X_ID       XID
#define Ecore_X_Drawable Drawable
#define Ecore_X_Window   Window
#define Ecore_X_Pixmap   Pixmap
#define Ecore_X_Atom     Atom
#define Ecore_X_Time     Time
#define Ecore_X_GC       GC

int                 ecore_x_client_message32_send(Ecore_X_Window win,
						  Ecore_X_Atom type,
						  long mask,
						  long d0, long d1, long d2,
						  long d3, long d4);

void                ecore_x_window_prop_card32_set(Ecore_X_Window win,
						   Ecore_X_Atom atom,
						   unsigned int *val,
						   unsigned int num);
int                 ecore_x_window_prop_card32_get(Ecore_X_Window win,
						   Ecore_X_Atom atom,
						   unsigned int *val,
						   unsigned int len);
int                 ecore_x_window_prop_card32_list_get(Ecore_X_Window win,
							Ecore_X_Atom atom,
							unsigned int **plst);

void                ecore_x_window_prop_xid_set(Ecore_X_Window win,
						Ecore_X_Atom atom,
						Ecore_X_Atom type,
						Ecore_X_ID * lst,
						unsigned int num);
int                 ecore_x_window_prop_xid_get(Ecore_X_Window win,
						Ecore_X_Atom atom,
						Ecore_X_Atom type,
						Ecore_X_ID * lst,
						unsigned int len);
int                 ecore_x_window_prop_xid_list_get(Ecore_X_Window win,
						     Ecore_X_Atom atom,
						     Ecore_X_Atom type,
						     Ecore_X_ID ** plst);
void                ecore_x_window_prop_xid_list_change(Ecore_X_Window win,
							Ecore_X_Atom atom,
							Ecore_X_Atom type,
							Ecore_X_ID item,
							int op);
void                ecore_x_window_prop_atom_set(Ecore_X_Window win,
						 Ecore_X_Atom atom,
						 Ecore_X_Atom * val,
						 unsigned int num);
int                 ecore_x_window_prop_atom_get(Ecore_X_Window win,
						 Ecore_X_Atom atom,
						 Ecore_X_Atom * val,
						 unsigned int len);
int                 ecore_x_window_prop_atom_list_get(Ecore_X_Window win,
						      Ecore_X_Atom atom,
						      Ecore_X_Atom ** plst);
void                ecore_x_window_prop_atom_list_change(Ecore_X_Window win,
							 Ecore_X_Atom atom,
							 Ecore_X_Atom item,
							 int op);
void                ecore_x_window_prop_window_set(Ecore_X_Window win,
						   Ecore_X_Atom atom,
						   Ecore_X_Window * val,
						   unsigned int num);
int                 ecore_x_window_prop_window_get(Ecore_X_Window win,
						   Ecore_X_Atom atom,
						   Ecore_X_Window * val,
						   unsigned int len);
int                 ecore_x_window_prop_window_list_get(Ecore_X_Window win,
							Ecore_X_Atom atom,
							Ecore_X_Window ** plst);

void                ecore_x_window_prop_string_set(Ecore_X_Window win,
						   Ecore_X_Atom atom,
						   const char *str);
char               *ecore_x_window_prop_string_get(Ecore_X_Window win,
						   Ecore_X_Atom atom);

/* Misc. */
extern Ecore_X_Atom ECORE_X_ATOM_UTF8_STRING;

/* ICCCM */
extern Ecore_X_Atom ECORE_X_ATOM_WM_STATE;
extern Ecore_X_Atom ECORE_X_ATOM_WM_DELETE_WINDOW;
extern Ecore_X_Atom ECORE_X_ATOM_WM_TAKE_FOCUS;
extern Ecore_X_Atom ECORE_X_ATOM_WM_PROTOCOLS;
extern Ecore_X_Atom ECORE_X_ATOM_WM_CLASS;
extern Ecore_X_Atom ECORE_X_ATOM_WM_NAME;
extern Ecore_X_Atom ECORE_X_ATOM_WM_COMMAND;
extern Ecore_X_Atom ECORE_X_ATOM_WM_ICON_NAME;
extern Ecore_X_Atom ECORE_X_ATOM_WM_CLIENT_MACHINE;
extern Ecore_X_Atom ECORE_X_ATOM_WM_CHANGE_STATE;
extern Ecore_X_Atom ECORE_X_ATOM_WM_COLORMAP_WINDOWS;
extern Ecore_X_Atom ECORE_X_ATOM_WM_WINDOW_ROLE;
extern Ecore_X_Atom ECORE_X_ATOM_WM_HINTS;
extern Ecore_X_Atom ECORE_X_ATOM_WM_NORMAL_HINTS;
extern Ecore_X_Atom ECORE_X_ATOM_WM_CLIENT_LEADER;
extern Ecore_X_Atom ECORE_X_ATOM_WM_TRANSIENT_FOR;
extern Ecore_X_Atom ECORE_X_ATOM_WM_SAVE_YOURSELF;

#if 0
extern Ecore_X_Atom ECORE_X_ATOM_WM_SAVE_YOURSELF;
#endif

void                ecore_x_icccm_init(void);

void                ecore_x_icccm_delete_window_send(Ecore_X_Window win,
						     Ecore_X_Time ts);
void                ecore_x_icccm_take_focus_send(Ecore_X_Window win,
						  Ecore_X_Time ts);

void                ecore_x_icccm_title_set(Ecore_X_Window win,
					    const char *title);
char               *ecore_x_icccm_title_get(Ecore_X_Window win);

/* NETWM (EWMH) */
extern Ecore_X_Atom ECORE_X_ATOM_NET_SUPPORTED;
extern Ecore_X_Atom ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK;

extern Ecore_X_Atom ECORE_X_ATOM_NET_NUMBER_OF_DESKTOPS;
extern Ecore_X_Atom ECORE_X_ATOM_NET_VIRTUAL_ROOTS;
extern Ecore_X_Atom ECORE_X_ATOM_NET_DESKTOP_GEOMETRY;
extern Ecore_X_Atom ECORE_X_ATOM_NET_DESKTOP_NAMES;
extern Ecore_X_Atom ECORE_X_ATOM_NET_DESKTOP_VIEWPORT;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WORKAREA;
extern Ecore_X_Atom ECORE_X_ATOM_NET_CURRENT_DESKTOP;
extern Ecore_X_Atom ECORE_X_ATOM_NET_SHOWING_DESKTOP;

extern Ecore_X_Atom ECORE_X_ATOM_NET_ACTIVE_WINDOW;
extern Ecore_X_Atom ECORE_X_ATOM_NET_CLIENT_LIST;
extern Ecore_X_Atom ECORE_X_ATOM_NET_CLIENT_LIST_STACKING;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_NAME;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_VISIBLE_NAME;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ICON_NAME;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_VISIBLE_ICON_NAME;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_DESKTOP;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DESKTOP;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DOCK;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE_TOOLBAR;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE_MENU;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE_UTILITY;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE_SPLASH;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE_DIALOG;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_TYPE_NORMAL;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_MODAL;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_STICKY;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_MAXIMIZED_VERT;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_MAXIMIZED_HORZ;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_SHADED;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_SKIP_TASKBAR;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_SKIP_PAGER;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_HIDDEN;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_FULLSCREEN;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_ABOVE;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_BELOW;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STATE_DEMANDS_ATTENTION;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ALLOWED_ACTIONS;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_MOVE;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_RESIZE;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_MINIMIZE;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_SHADE;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_STICK;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_MAXIMIZE_HORZ;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_MAXIMIZE_VERT;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_FULLSCREEN;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_CHANGE_DESKTOP;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ACTION_CLOSE;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STRUT;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_STRUT_PARTIAL;

extern Ecore_X_Atom ECORE_X_ATOM_NET_FRAME_EXTENTS;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ICON;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_USER_TIME;

#if 0				/* Not used */
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_ICON_GEOMETRY;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_PID;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_HANDLED_ICONS;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_PING;
#endif
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_SYNC_REQUEST;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_SYNC_REQUEST_COUNTER;

extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_WINDOW_OPACITY;

/* Misc window ops */
extern Ecore_X_Atom ECORE_X_ATOM_NET_CLOSE_WINDOW;
extern Ecore_X_Atom ECORE_X_ATOM_NET_MOVERESIZE_WINDOW;
extern Ecore_X_Atom ECORE_X_ATOM_NET_WM_MOVERESIZE;
extern Ecore_X_Atom ECORE_X_ATOM_NET_RESTACK_WINDOW;

#if 0				/* Not yet implemented */
extern Ecore_X_Atom ECORE_X_ATOM_NET_REQUEST_FRAME_EXTENTS;
#endif

/* Startup notification */
extern Ecore_X_Atom ECORE_X_ATOM_NET_STARTUP_INFO_BEGIN;
extern Ecore_X_Atom ECORE_X_ATOM_NET_STARTUP_INFO;

void                ecore_x_netwm_init(void);

void                ecore_x_netwm_wm_identify(Ecore_X_Window root,
					      Ecore_X_Window check,
					      const char *wm_name);

void                ecore_x_netwm_desk_count_set(Ecore_X_Window root,
						 unsigned int n_desks);
void                ecore_x_netwm_desk_roots_set(Ecore_X_Window root,
						 Ecore_X_Window * vroots,
						 unsigned int n_desks);
void                ecore_x_netwm_desk_names_set(Ecore_X_Window root,
						 const char **names,
						 unsigned int n_desks);
void                ecore_x_netwm_desk_size_set(Ecore_X_Window root,
						unsigned int width,
						unsigned int height);
void                ecore_x_netwm_desk_workareas_set(Ecore_X_Window root,
						     unsigned int *areas,
						     unsigned int n_desks);
void                ecore_x_netwm_desk_current_set(Ecore_X_Window root,
						   unsigned int desk);
void                ecore_x_netwm_desk_viewports_set(Ecore_X_Window root,
						     unsigned int *origins,
						     unsigned int n_desks);
void                ecore_x_netwm_showing_desktop_set(Ecore_X_Window root,
						      int on);

void                ecore_x_netwm_client_list_set(Ecore_X_Window root,
						  Ecore_X_Window * p_clients,
						  unsigned int n_clients);
void                ecore_x_netwm_client_list_stacking_set(Ecore_X_Window root,
							   Ecore_X_Window *
							   p_clients,
							   unsigned int
							   n_clients);
void                ecore_x_netwm_client_active_set(Ecore_X_Window root,
						    Ecore_X_Window win);
void                ecore_x_netwm_name_set(Ecore_X_Window win,
					   const char *name);
int                 ecore_x_netwm_name_get(Ecore_X_Window win, char **name);
void                ecore_x_netwm_icon_name_set(Ecore_X_Window win,
						const char *name);
int                 ecore_x_netwm_icon_name_get(Ecore_X_Window win,
						char **name);
void                ecore_x_netwm_visible_name_set(Ecore_X_Window win,
						   const char *name);
int                 ecore_x_netwm_visible_name_get(Ecore_X_Window win,
						   char **name);
void                ecore_x_netwm_visible_icon_name_set(Ecore_X_Window win,
							const char *name);
int                 ecore_x_netwm_visible_icon_name_get(Ecore_X_Window win,
							char **name);

void                ecore_x_netwm_desktop_set(Ecore_X_Window win,
					      unsigned int desk);
int                 ecore_x_netwm_desktop_get(Ecore_X_Window win,
					      unsigned int *desk);

int                 ecore_x_netwm_user_time_get(Ecore_X_Window win,
						unsigned int *ts);

void                ecore_x_netwm_opacity_set(Ecore_X_Window win,
					      unsigned int opacity);
int                 ecore_x_netwm_opacity_get(Ecore_X_Window win,
					      unsigned int *opacity);

#endif

void                ecore_x_icccm_state_set_iconic(Ecore_X_Window win);
void                ecore_x_icccm_state_set_normal(Ecore_X_Window win);
void                ecore_x_icccm_state_set_withdrawn(Ecore_X_Window win);

void                ecore_x_window_prop_string_list_set(Ecore_X_Window win,
							Ecore_X_Atom atom,
							char **lst, int num);
int                 ecore_x_window_prop_string_list_get(Ecore_X_Window win,
							Ecore_X_Atom atom,
							char ***plst);
