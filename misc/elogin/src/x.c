/***************************************************************************
                          x.c  -  description
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

 /* X Wrappers, because I want to be like raster */

#include "main.h"
#include "mem.h"
#include "x.h"
#include <Imlib2.h>

static Display	*disp;
static Visual	*default_vis;
static Colormap	default_cm;
int				default_depth;
Window			default_root;
Window			default_win;

static Window	focused_win = 0;
static int		mouse_x = 0, mouse_y = 0;

static void		e_handle_x_error(Display *d, XErrorEvent *ev);
static void		e_handle_x_io_error(Display *d);
#ifdef HAS_XINERAMA
static void		Elogin_XineramaCheck(void);
#endif
static void
e_handle_x_error(Display *d, XErrorEvent *ev)
{
   /* ignroe all X errors */
}

static void
e_handle_x_io_error(Display * d)
{
   /* FIXME: call clean exit handler */
   exit(1);
}

void
e_sync(void)
{
	XSync(disp, False);
}

void
e_flush(void)
{
	XFlush(disp);
}

void
e_window_destroy(Window win)
{
	XDestroyWindow(disp, win);
}

Window
e_window_new(Window parent, int x, int y, int w, int h)
{
	Window					win;
	XSetWindowAttributes	attr;
	
	attr.backing_store = NotUseful;
	attr.override_redirect = True;
	attr.colormap = default_cm;
	attr.border_pixel = 0;
	attr.background_pixmap = None;
	attr.save_under = False;
//	attr.do_not_propagate_mask = True;
	
	win = XCreateWindow(disp, default_root, mouse_x, mouse_y, 10, 10, 0, default_depth,
		InputOutput, default_vis, CWOverrideRedirect |
		CWSaveUnder | CWBackingStore | CWColormap |
		CWBackPixmap | CWBorderPixel, &attr);
		
	return win;
}

void
e_window_show(Window win)
{
	XMapWindow(disp, win);
}

Pixmap
e_pixmap_new(Window win, int w, int h, int dep)
{
	if (!win)
		win = default_win;
	if (dep == 0)
		dep = default_depth;
	return XCreatePixmap(disp, win, w, h, dep);
}

void
e_pixmap_free(Pixmap pmap)
{
	if (!pmap)
		return;
	XFreePixmap(disp, pmap);
}

void
e_window_set_background_pixmap(Window win, Pixmap pmap)
{
	XSetWindowBackgroundPixmap(disp, win, pmap);
}

void
e_window_clear(Window win)
{
	XClearWindow(disp, win);
}

void
e_pointer_xy(Window win, int *x, int *y)
{
	Window			dw;
	unsigned int		dm;
	int				wx, wy;
	
	if (win == 0)
		win = default_root;
	XQueryPointer(disp, win, &dw, &dw, &mouse_x, &mouse_y, &wx, &wy, &dm);
	if (x)
		*x = wx;
	if (y)
		*y = wy;
}

void
e_pointer_xy_set(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}

void
e_pointer_xy_get(int *x, int *y)
{
	if (x)
		*x = mouse_x;
	if (y)
		*y = mouse_y;
}

void
e_window_set_events(Window win, long mask)
{
	if (win == 0)
		win = default_root;
	XSelectInput(disp, win, mask);
}

void
e_window_add_events(Window win, long mask)
{
   XWindowAttributes   att;

   if (win == 0)
      win = default_root;
   if (XGetWindowAttributes(disp, win, &att) == True)
     {
    mask = att.your_event_mask | mask;
    e_window_set_events(win, mask);
     }
}

void
e_window_resize(Window win, int w, int h)
{
	XResizeWindow(disp, win, w, h);
}

int
e_x_get_fd(void)
{
   return ConnectionNumber(disp);
}

void
e_display_init(char *display)
{
	int revert;
	
	disp = XOpenDisplay(display);
	if (!disp)
	{
		char *d;
		d = getenv("DISPLAY");
		if (d)
			fprintf(stderr,
				"Fatal Error:\n"
				"Cannot connect ot the display nominated by your DISPLAY variable:\n"
				"%s\n",
				d);
		else
			fprintf(stderr,
				"Fatal Error:\n"
				"No DISPLAY variable set so cannot determine display to connecto to.\n");
		exit(1);
	}
	/* Error Handler */
//	XSetErrorHandler((XErrorHandler) e_handle_x_error);
	/* IO Error Handler */
	XSetIOErrorHandler((XIOErrorHandler) e_handle_x_io_error);
	
	default_vis 		= DefaultVisual(disp, DefaultScreen(disp));
	default_depth	= DefaultDepth(disp, DefaultScreen(disp));
	default_cm		= DefaultColormap(disp, DefaultScreen(disp));
	default_root		= DefaultRootWindow(disp);
	
	XGetInputFocus(disp, &focused_win, &revert);
	
#ifdef HAS_XINERAMA
	/* Setup the window in the right spot */
	Elogin_XineramaCheck();
#else
	mouse_x =(DisplayWidth(disp, DefaultScreen(disp)) - 500) / 2;
	mouse_y = (DisplayHeight(disp, DefaultScreen(disp)) - (350)) / 2;
#endif

	default_win		= e_window_new(default_root, 0, 0, 10, 10);
	
	e_window_add_events(default_win, XEV_KEY | XEV_IN_OUT | XEV_MOUSE_MOVE |
				XEV_BUTTON);
	e_pointer_xy(0, NULL, NULL);
	
	imlib_context_set_display(disp);
 	imlib_context_set_visual(default_vis);
 	imlib_context_set_colormap(default_cm);
 	imlib_context_set_drawable(default_root);
 	imlib_context_set_dither(0);
 	imlib_context_set_anti_alias(1);
 	imlib_context_set_blend(1);
}

int
e_events_pending(void)
{
	return XPending(disp);
}

Atom
e_atom_get(char *name)
{
	return XInternAtom(disp, name, False);
}

void
e_window_property_set(Window win, Atom type, Atom format, int size, void *data, int number)
{
	if (win == 0)
      	win = default_root;
      if (size != 32)
     		XChangeProperty(disp, win, type, format, size, PropModeReplace,
     			(unsigned char *)data, number);
     	else
     	{
     		long *dat;
     		int i, *ptr;
		dat = NEW(long, number);
		for (ptr = (int *)data, i = 0; i < number; i++)
			dat[i] = ptr[i];
		XChangeProperty(disp, win, type, format, size, PropModeReplace,
			(unsigned char *)dat, number);
		FREE(dat);
	}
}

void
e_get_next_event(XEvent *event)
{
	XNextEvent(disp, event);
}

#ifdef HAS_XINERAMA
static void
Elogin_XineramaCheck(void)
{
	Window	rt, ch;
	int	d;
	unsigned int	ud;
	int	pointer_x, pointer_y;
	int	num;
	XineramaScreenInfo	*screens;
	int i;
	
	XQueryPointer(disp, default_root, &rt, &ch, &pointer_x,
		&pointer_y, &d, &d, &ud);
	screens = XineramaQueryScreens(disp, &num);
	for (i = 0; i < num; i++)
                  {
                 for (i = 0; i < num; i++)
                   {
                      if (pointer_x >= screens[i].x_org)
                    {
                       if (pointer_x <=
                           (screens[i].width +
                        screens[i].x_org))
                         {
                        if (pointer_y >=
                            screens[i].y_org)
                          {
                             if (pointer_y <=
                             (screens[i].height +
                              screens[i].y_org))
                               {
                              mouse_x =
                                 ((screens
                                   [i].width -
                                   500) / 2) +
                                 screens[i].x_org;
                              mouse_y =
                                 ((screens
                                   [i].height -
                                   250) / 2) +
                                 screens[i].y_org;
                               }
                          }
                         }
                    }
                   }
                  }
                XFree(screens);
}
#endif
