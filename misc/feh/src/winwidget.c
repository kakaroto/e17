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

  ret = emalloc (sizeof (_winwidget));

  ret->win = 0;
  ret->w = 0;
  ret->h = 0;
  ret->im_w = 0;
  ret->im_h = 0;
  ret->visible = 0;
  ret->bg_pmap = 0;
  ret->im = NULL;
  ret->name = NULL;
  ret->file = NULL;

  /* Zoom stuff */
  ret->zoom_mode = 0;
  ret->zx = 0;
  ret->zy = 0;
  ret->zoom = 1.0;
  ret->timeout = 0;

  ret->gc = None;

  return ret;
}

winwidget winwidget_create_from_image (Imlib_Image * im, char *name)
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

  if (name)
    ret->name = estrdup (name);
  else
    ret->name = estrdup (PACKAGE);

  winwidget_create_window (ret, ret->w, ret->h);
  winwidget_render_image (ret);

  return ret;
}

winwidget winwidget_create_from_file (feh_file * file, char *name)
{
  winwidget ret = NULL;

  D (("In winwidget_create_from_file\n"));

  if (!file || !file->filename)
    return NULL;

  ret = winwidget_allocate ();
  if (name)
    ret->name = estrdup (name);
  else
    ret->name = estrdup (file->filename);

  if (opt.progressive)
    {
      D (("Progressive loading enabled\n"));
      progwin = ret;
      imlib_context_set_progress_function (progress);
      imlib_context_set_progress_granularity (10);
    }

  if (winwidget_loadimage (ret, file) == 0)
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
      winwidget_render_image (ret);
    }

  return ret;
}

void
winwidget_create_window (winwidget ret, int w, int h)
{
  XSetWindowAttributes attr;
  XClassHint *xch;
  MWMHints mwmhints;
  Atom prop = None;

  D (("In winwidget_create_window\n"));

  if (opt.full_screen)
    {
      if (scr == NULL)
	{
	  scr = ScreenOfDisplay (disp, DefaultScreen (disp));
	}
      w = scr->width;
      h = scr->height;
    }

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

  if (opt.borderless || opt.full_screen)
    {
      prop = XInternAtom (disp, "_MOTIF_WM_HINTS", True);
      if (prop == None)
	{
	  weprintf ("Window Manager does not support MWM hints. "
		    "To get a borderless window I have to bypass your wm.");
	  attr.override_redirect = True;
	  mwmhints.flags = 0;
	}
      else
	{
	  mwmhints.flags = MWM_HINTS_DECORATIONS;
	  mwmhints.decorations = 0;
	}
    }
  else
    mwmhints.flags = 0;

  ret->win =
    XCreateWindow (disp, DefaultRootWindow (disp), 0, 0, w, h, 0,
		   depth, InputOutput, vis,
		   CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		   CWColormap | CWBackPixel | CWBorderPixel | CWEventMask,
		   &attr);

  if (mwmhints.flags)
    {
      XChangeProperty (disp, ret->win, prop, prop, 32,
		       PropModeReplace, (unsigned char *) &mwmhints,
		       PROP_MWM_HINTS_ELEMENTS);
    }

  XSetWMProtocols (disp, ret->win, &wmDeleteWindow, 1);
  winwidget_update_title (ret);
  xch = XAllocClassHint ();
  xch->res_name = "feh";
  xch->res_class = "feh";
  XSetClassHint (disp, ret->win, xch);
  XFree (xch);

  /* Size hints */
  if (opt.full_screen)
    {
      XSizeHints xsz;

      xsz.flags = USPosition;
      xsz.x = 0;
      xsz.y = 0;
      XSetWMNormalHints (disp, ret->win, &xsz);
    }
  else
  {
      XSizeHints xsz;
      
      xsz.flags = PSize | PMinSize | PMaxSize;
      xsz.width = w;
      xsz.height = h;
      xsz.min_width = w;
      xsz.max_width = w;
      xsz.min_height = h;
      xsz.max_height = h;
      XSetWMNormalHints (disp, ret->win, &xsz);
  }
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
winwidget_setup_pixmaps (winwidget winwid)
{
  D (("In winwidget_setup_pixmaps\n"));

  if (opt.full_screen)
    {
      if (!(winwid->bg_pmap))
	{
	  if (winwid->gc == None)
	    {
	      XGCValues gcval;

	      gcval.foreground = BlackPixel (disp, DefaultScreen (disp));
	      winwid->gc =
		XCreateGC (disp, winwid->win, GCForeground, &gcval);
	    }
	  winwid->bg_pmap =
	    XCreatePixmap (disp, winwid->win, scr->width, scr->height, depth);
	}
      XFillRectangle (disp, winwid->bg_pmap, winwid->gc, 0, 0, scr->width,
		      scr->height);
    }
  else
    {
      if (winwid->bg_pmap)
	XFreePixmap (disp, winwid->bg_pmap);

      winwid->bg_pmap =
	XCreatePixmap (disp, winwid->win, winwid->im_w, winwid->im_h, depth);
    }

}

void
winwidget_render_image (winwidget winwid)
{
  int x = 0, y = 0;

  D (("In winwidget_render_image\n"));

  winwidget_setup_pixmaps (winwid);

  if (!opt.full_screen)
    {
      /* resize window if the image size has changed */
      if ((winwid->w != winwid->im_w) || (winwid->h != winwid->im_h))
	{
	  winwid->h = winwid->im_h;
	  winwid->w = winwid->im_w;
	  XResizeWindow (disp, winwid->win, winwid->im_w, winwid->im_h);
	}
    }

  imlib_context_set_drawable (winwid->bg_pmap);
  imlib_context_set_image (winwid->im);
  imlib_context_set_blend (0);
  if (!opt.full_screen && imlib_image_has_alpha ())
    {
      feh_draw_checks (winwid);
      imlib_context_set_blend (1);
      imlib_context_set_image (winwid->im);
    }

  if (opt.full_screen)
    {
      x = (scr->width - winwid->im_w) >> 1;
      y = (scr->height - winwid->im_h) >> 1;
    }
  imlib_render_image_on_drawable (x, y);

  XSetWindowBackgroundPixmap (disp, winwid->win, winwid->bg_pmap);
  XClearWindow (disp, winwid->win);
  XFlush (disp);
}

void
feh_draw_checks (winwidget win)
{
  int x, y;

  if (opt.full_screen)
    return;

  imlib_context_set_image (checks);
  imlib_context_set_drawable (win->bg_pmap);

  for (y = 0; y < win->h; y += CHECK_SIZE)
    for (x = 0; x < win->w; x += CHECK_SIZE)
      imlib_render_image_on_drawable (x, y);
}

void
winwidget_destroy (winwidget winwid)
{
  D (("In winwidget_destroy\n"));
  winwidget_unregister (winwid);
  if (winwid->win)
    XDestroyWindow (disp, winwid->win);
  if (winwid->bg_pmap)
    XFreePixmap (disp, winwid->bg_pmap);
  if (winwid->name)
    free (winwid->name);
  if (winwid->im)
    {
      imlib_context_set_image (winwid->im);
      imlib_free_image_and_decache ();
    }
  free (winwid);
  winwid = NULL;
}

void
winwidget_destroy_all (void)
{
  int i;
  D (("In winwidget_destroy_all\n"));
  /* Have to DESCEND the list here, 'cos of the way _unregister works.
   * I'll re-implement the list at some point. A linked list
   * beckons :) */
  for (i = window_num - 1; i >= 0; i--)
    winwidget_destroy (windows[i]);
}

int
winwidget_loadimage (winwidget winwid, feh_file * file)
{
  D (("In winwidget_loadimage: filename %s\n", file->filename));
  return feh_load_image (&(winwid->im), file);
}

void
winwidget_show (winwidget winwid)
{
  XEvent ev;
  XMapWindow (disp, winwid->win);
  /* wait for the window to map */
  D (("In winwidget_show: Waiting for window to map\n"));
  XMaskEvent (disp, StructureNotifyMask, &ev);
  D (("In winwidget_show: Window mapped\n"));
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
  D (("In winwidget_register, window %p\n", win));
  window_num++;
  if (windows)
    windows = erealloc (windows, window_num * sizeof (winwidget));
  else
    windows = emalloc (window_num * sizeof (winwidget));
  windows[window_num - 1] = win;

  XSaveContext (disp, win->win, xid_context, (XPointer) win);
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
	    windows = erealloc (windows, window_num * sizeof (winwidget));
	  else
	    {
	      free (windows);
	      windows = NULL;
	    }
	}
    }
  XDeleteContext (disp, win->win, xid_context);
}

winwidget winwidget_get_from_window (Window win)
{
  winwidget ret = NULL;

  D (("About to XFindContext\n"));
  if (XFindContext (disp, win, xid_context, (XPointer *) & ret) != XCNOENT)
    return ret;
  else
    return NULL;
}
