/* menus.c
 *
 * Copyright (C) 1999 Tom Gilbert
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "feh.h"
#include "menuwidget.h"

static menuwidget
menuwidget_allocate (void)
{
  menuwidget menu = NULL;

  menu = emalloc (sizeof (_menuwidget));

  menu->win = 0;
  menu->w = 0;
  menu->h = 0;
  menu->visible = 0;
  menu->pm = 0;
  menu->im = NULL;
  menu->label = NULL;

  return menu;
}

void
menuwidget_create_window (menuwidget menu, int w, int h)
{
  XSetWindowAttributes attr;
  XClassHint *xch;
  XSizeHints sh;
  static Cursor cursor;
  static XGCValues gcvalue;
  static long mask;

  D (("In menuwidget_create_window\n"));

  xattr.border_pixel = BlackPixel (disp, vis);
  xattr.save_under = TRUE;
  xattr.backing_store = WhenMapped;
  xattr.override_redirect = TRUE;
  xattr.colormap = cmap;
  if (!mask)
    {
      mask =
	EnterNotify | LeaveNotify | PointerMotionMask | ButtonMotionMask |
	ButtonPressMask | ButtonReleaseMask | Button1MotionMask |
	Button2MotionMask | Button3MotionMask;
      gcvalue.foreground = PixColors[menuTextColor];
      cursor = XCreateFontCursor (Xdisplay, XC_left_ptr);
    }
  menu->win =
    XCreateWindow (disp, root, 0, 0, 1, 1, 0, depth, InputOutput,
		   CopyFromParent,
		   CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		   CWBorderPixel | CWColormap, &xattr);
  XDefineCursor (disp, menu->win, cursor);
  XSelectInput (disp, menu->win, mask);
  XStoreName (disp, menu->win, menu->title);

  menuwidget_register (menu);
}

void
menuwidget_render_image (menuwidget menu)
{
  if (menu->pm)
    XFreePixmap (disp, menu->pm);

  menu->pm = XCreatePixmap (disp, menu->win, menu->im_w, menu->im_h, depth);

  imlib_context_set_drawable (menu->pm);
  imlib_context_set_image (menu->im);
  imlib_render_image_on_drawable (0, 0);

  if ((menu->w != menu->im_w) || (menu->h != menu->im_h))
    {
      menu->h = menu->im_h;
      menu->w = menu->im_w;
      XResizeWindow (disp, menu->win, menu->im_w, menu->im_h);
    }

  XSetWindowBackgroundPixmap (disp, menu->win, menu->pm);
  XClearWindow (disp, menu->win);
  XFlush (disp);
}

void
menuwidget_destroy (menuwidget menu)
{
  D (("In menuwidget_destroy\n"));
  if (menu->win)
    XDestroyWindow (disp, menu->win);
  menuwidget_unregister (menu);
  if (menu->pm)
    XFreePixmap (disp, menu->pm);
  if (menu->label)
    free (menu->label);
  if (menu->im)
    {
      imlib_context_set_image (menu->im);
      imlib_free_image_and_decache ();
    }
  free (menu);
  menu = NULL;
}

int
menuwidget_loadimage (menuwidget menu, char *filename)
{
  D (("In menuwidget_loadimage: filename %s\n", filename));
  return feh_load_image (&(menu->im), filename);
}

void
menuwidget_show (menuwidget menu)
{
  XEvent ev;
  XMapWindow (disp, menu->win);
  menu->visible = 1;
}

void
menuwidget_hide (menuwidget menu)
{
  D (("In menuwidget_hide\n"));
  XUnmapWindow (disp, menu->win);
  menu->visible = 0;
}

static void
menuwidget_register (menuwidget win)
{
  D (("In menuwidget_register\n"));
  menu_num++;
  if (menus)
    menus = erealloc (menus, menu_num * sizeof (menuwidget));
  else
    menus = emalloc (menu_num * sizeof (menuwidget));
  menus[menu_num - 1] = win;
}

static void
menuwidget_unregister (menuwidget win)
{
  int i, j;
  D (("In menuwidget_unregister\n"));
  for (i = 0; i < menu_num; i++)
    {
      if (menus[i] == win)
	{
	  for (j = i; j < menu_num - 1; j++)
	    menus[j] = menus[j + 1];
	  menu_num--;
	  if (menu_num > 0)
	    menus = erealloc (menus, menu_num * sizeof (menuwidget));
	  else
	    {
	      free (menus);
	      menus = NULL;
	    }
	}
    }
}

menuwidget menuwidget_get_from_window (Window win)
{
  /* Loop through menus */
  int i;
  D (("In menuwidget_get_from_window, Window is %ld\n", win));
  for (i = 0; i < menu_num; i++)
    {
      if (menus[i]->win == win)
	return menus[i];
    }
  D (("Oh dear, returning NULL from menuwidget_get_from_window\n"));
  return NULL;
}
