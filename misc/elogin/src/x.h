/***************************************************************************
                          x.h  -  description
                             -------------------
    begin                : Sun Apr 9 2000
    copyright            : (C) 2000 by Chris Thomas
    email                : x5rings@fsck.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef E_X_H
#define E_X_H 1

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/keysymdef.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>

#ifdef HAS_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif

#define XEV_NONE              NoEventMask
#define XEV_KEY               KeyPressMask | KeyReleaseMask
#define XEV_BUTTON            ButtonPressMask | ButtonReleaseMask
#define XEV_KEY_PRESS         KeyPressMask
#define XEV_KEY_RELEASE       KeyReleaseMask
#define XEV_BUTTON_PRESS      ButtonPressMask
#define XEV_BUTTON_RELEASE    ButtonReleaseMask
#define XEV_IN_OUT            EnterWindowMask | LeaveWindowMask
#define XEV_MOUSE_MOVE		PointerMotionMask


int				default_depth;
Window			default_root;
Window			default_win;

void			e_sync(void);
void			e_flush(void);
void			e_window_destroy(Window win);
Window			e_window_new(Window parent, int x, int y, int w, int h);
void			e_window_show(Window win);
Pixmap			e_pixmap_new(Window win, int w, int h, int dep);
void			e_pixmap_free(Pixmap pmap);
void			e_window_set_background_pixmap(Window win, Pixmap pmap);
void			e_window_clear(Window win);
void			e_pointer_xy(Window win, int *x, int *y);
void			e_pointer_xy_set(int x, int y);
void			e_pointer_xy_get(int *x, int *y);
void			e_window_set_events(Window win, long mask);
void			e_window_add_events(Window win, long mask);
void			e_window_resize(Window win, int w, int h);
int				e_x_get_fd(void);
void			e_display_init(char *display);
int				e_events_pending(void);
Atom			e_atom_get(char *name);
void			e_window_property_set(Window win, Atom type, Atom format, int size, void *data, int number);
void			e_get_next_event(XEvent *event);

#define E_ATOM(atom, name) \
if (!atom) atom = e_atom_get(name);

#endif
