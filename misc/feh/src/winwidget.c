/* winwidget.c
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
#include "winwidget.h"

static winwidget
winwidget_allocate (void)
{
  winwidget ret = NULL;

  if ((ret = malloc (sizeof (_winwidget))) == NULL)
    exit (1);

  ret->win = 0;
  ret->w = 0;
  ret->h = 0;
  ret->im_w = 0;
  ret->im_h = 0;
  ret->visible = 0;
  ret->bg_pmap = 0;
  ret->im = NULL;
  ret->name = NULL;

  /* Zoom stuff */
  ret->zoom_mode = 0;
  ret->zx = 0;
  ret->zy = 0;
  ret->zoom = 1.0;
  ret->timeout = 0;
  ret->blank_im = NULL;

  return ret;
}

winwidget
winwidget_create_from_image (Imlib_Image * im, char *name)
{
  winwidget ret = NULL;

  D (("In winwidget_create_from_image\n"));

  if (im == NULL)
    return NULL;

  ret = winwidget_allocate ();

  ret->im = im;
  imlib_context_set_image (ret->im);
  ret->w = ret->im_w = imlib_image_get_width ();
  ret->h = ret->im_h = imlib_image_get_height ();

  winwidget_create_blank_bg (ret);

  if (name)
    ret->name = strdup (name);
  else
    ret->name = strdup (PACKAGE);

  winwidget_create_window (ret, ret->w, ret->h);
  winwidget_render_image (ret);

  return ret;
}

winwidget
winwidget_create_from_file (char *filename, char *name)
{
  winwidget ret = NULL;

  D (("In winwidget_create_from_file\n"));

  if (filename == NULL)
    return NULL;

  ret = winwidget_allocate ();
  if (name)
    ret->name = strdup (name);
  else
    ret->name = strdup (filename);

  if (opt.progressive)
    {
      progwin = ret;
      imlib_context_set_progress_function (progress);
      imlib_context_set_progress_granularity (10);
    }

  if (winwidget_loadimage (ret, filename) == 0)
    {
      if (opt.progressive)
	winwidget_destroy (ret);
      return NULL;
    }

  if (!opt.progressive)
    {
      imlib_context_set_image (ret->im);
      ret->w = ret->im_w = imlib_image_get_width ();
      ret->h = ret->im_h = imlib_image_get_height ();
      winwidget_create_window (ret, ret->w, ret->h);
      winwidget_create_blank_bg (ret);
      winwidget_render_image (ret);
    }

  return ret;
}

void
winwidget_create_blank_bg (winwidget ret)
{
  int x, y, onoff;
  /* For zooming out */

  D (("In winwidget_create_blank_bg\n"));

  if (ret->blank_im)
    {
      imlib_context_set_image (ret->blank_im);
      imlib_free_image ();
    }

  ret->blank_im = imlib_create_image (ret->w, ret->h);
  imlib_context_set_image (ret->blank_im);
  for (y = 0; y < ret->h; y += 8)
    {
      onoff = (y / 8) & 0x1;
      for (x = 0; x < ret->w; x += 8)
	{
	  if (onoff)
	    imlib_context_set_color (144, 144, 144, 255);
	  else
	    imlib_context_set_color (100, 100, 100, 255);
	  imlib_image_fill_rectangle (x, y, 8, 8);
	  onoff++;
	  if (onoff == 2)
	    onoff = 0;
	}
    }
}

void
winwidget_create_window (winwidget ret, int w, int h)
{
  XSetWindowAttributes attr;
  XClassHint *xch;
  XSizeHints sh;

  D (("In winwidget_create_window\n"));

  attr.backing_store = NotUseful;
  attr.override_redirect = False;
  attr.colormap = cm;
  attr.border_pixel = 0;
  attr.background_pixel = 0;
  attr.save_under = False;
  attr.event_mask = StructureNotifyMask | ButtonPressMask |
    ButtonReleaseMask | PointerMotionMask | EnterWindowMask |
    LeaveWindowMask | KeyPressMask | KeyReleaseMask | ButtonMotionMask |
    ExposureMask | FocusChangeMask | PropertyChangeMask |
    VisibilityChangeMask;
  ret->win =
    XCreateWindow (disp, DefaultRootWindow (disp), 0, 0, w, h, 0,
		   depth, InputOutput, vis,
		   CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		   CWColormap | CWBackPixel | CWBorderPixel | CWEventMask,
		   &attr);

  wmDeleteWindow = XInternAtom (disp, "WM_DELETE_WINDOW", False);
  XSetWMProtocols (disp, ret->win, &wmDeleteWindow, 1);
  winwidget_update_title (ret);
  xch = XAllocClassHint ();
  xch->res_name = "feh";
  xch->res_class = "feh";
  XSetClassHint (disp, ret->win, xch);
  XFree (xch);
  /* set the size hints */
  sh.flags = PSize | PMinSize | PMaxSize;
  sh.width = w;
  sh.height = h;
  sh.min_width = w;
  sh.min_height = h;
  sh.max_width = w;
  sh.max_height = h;
  XSetWMNormalHints (disp, ret->win, &sh);

  /* set the icons name property */
  XSetIconName (disp, ret->win, "feh");
  /* set the command hint */
  XSetCommand (disp, ret->win, cmdargv, cmdargc);

  winwidget_register (ret);

}

void
winwidget_update_title (winwidget ret)
{
  if (ret->name)
    XStoreName (disp, ret->win, ret->name);
  else
    XStoreName (disp, ret->win, "feh");
}

void
winwidget_render_image (winwidget winwid)
{
  if (winwid->bg_pmap)
    XFreePixmap (disp, winwid->bg_pmap);

  winwid->bg_pmap =
    XCreatePixmap (disp, winwid->win, winwid->im_w, winwid->im_h, depth);

  imlib_context_set_drawable (winwid->bg_pmap);
  imlib_context_set_image (winwid->blank_im);
  imlib_render_image_on_drawable (0, 0);
  imlib_context_set_image (winwid->im);
  imlib_render_image_on_drawable (0, 0);

  if ((winwid->w != winwid->im_w) || (winwid->h != winwid->im_h))
    {
      winwid->h = winwid->im_h;
      winwid->w = winwid->im_w;
      XResizeWindow (disp, winwid->win, winwid->im_w, winwid->im_h);
    }

  XSetWindowBackgroundPixmap (disp, winwid->win, winwid->bg_pmap);
  XClearWindow (disp, winwid->win);
  XFlush (disp);
}

void
winwidget_destroy (winwidget winwid)
{
  D (("In winwidget_destroy\n"));
  winwidget_unregister (winwid);
  if (winwid->visible)
    winwidget_hide (winwid);
  if (winwid->bg_pmap)
    XFreePixmap (disp, winwid->bg_pmap);
  if (winwid->name)
    free (winwid->name);
  if (winwid->im)
    {
      imlib_context_set_image (winwid->im);
      imlib_free_image_and_decache ();
    }
  if (winwid->blank_im)
    {
      imlib_context_set_image (winwid->blank_im);
      imlib_free_image_and_decache ();
    }
  XDestroyWindow (disp, winwid->win);
  free (winwid);
  winwid = NULL;
}

void
winwidget_destroy_all (void)
{
  int i;
  D (("In winwidget_destroy_all\n"));

  /* Have to DESCEND the list here, 'cos of the way _unregister works.
   * I'll re-implement the list at some point. A singly-linked list
   * beckons :) */
  for (i = window_num - 1; i >= 0; i--)
    winwidget_destroy (windows[i]);
}

int
winwidget_loadimage (winwidget winwid, char *filename)
{
  D (("In winwidget_loadimage: filename %s\n", filename));

  return feh_load_image (&(winwid->im), filename);
}

void
winwidget_show (winwidget winwid)
{
  XEvent ev;

  D (("Got Here\n"));
  XMapWindow (disp, winwid->win);
  /* wait for the window to map */
  D (("In winwidget_show: Waiting for window to map\n"));
  XMaskEvent (disp, StructureNotifyMask, &ev);
  D (("In winwidget_show: Window mapped\n"));
  D (("Got Here\n"));
  winwid->visible = 1;
}

void
winwidget_hide (winwidget winwid)
{
  D (("In winwidget_hide\n"));
  XUnmapWindow (disp, winwid->win);
  winwid->visible = 0;
}

static void
winwidget_register (winwidget win)
{
  D (("In winwidget_register\n"));
  window_num++;
  if (windows)
    windows = realloc (windows, window_num * sizeof (winwidget));
  else
    windows = malloc (window_num * sizeof (winwidget));
  windows[window_num - 1] = win;
}

static void
winwidget_unregister (winwidget win)
{
  int i, j;
  D (("In winwidget_unregister\n"));

  for (i = 0; i < window_num; i++)
    {
      if (windows[i] == win)
	{
	  for (j = i; j < window_num - 1; j++)
	    windows[j] = windows[j + 1];
	  window_num--;
	  if (window_num > 0)
	    windows = realloc (windows, window_num * sizeof (winwidget));
	  else
	    {
	      free (windows);
	      windows = NULL;
	    }
	}
    }
}

winwidget
winwidget_get_from_window (Window win)
{
  /* Loop through windows */
  int i;
  D (("In winwidget_get_from_window, Window is %ld\n", win));

  for (i = 0; i < window_num; i++)
    {
      if (windows[i]->win == win)
	return windows[i];
    }
  D (("Oh dear, returning NULL from winwidget_get_from_window\n"));
  return NULL;
}
