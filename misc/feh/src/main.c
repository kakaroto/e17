/* main.c
 *
 * Copyright (C) 2000 Tom Gilbert
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

#include "feh.h"
#include "main.h"

int
main (int argc, char **argv)
{
    D_ENTER;
  atexit (delete_rm_files);

  init_parse_options (argc, argv);

  init_x_and_imlib ();

  if (opt.montage)
    init_montage_mode ();
  else if (opt.index)
    init_index_mode ();
  else if (opt.collage)
    init_collage_mode ();
  else if (opt.multiwindow)
    init_multiwindow_mode ();
  else if (opt.list || opt.longlist)
    init_list_mode ();
  else if (opt.loadables)
    init_loadables_mode ();
  else if (opt.unloadables)
    init_unloadables_mode ();
  else
    {
      /* Slideshow mode is now the default. Because it's spiffy */
      opt.slideshow = 1;
      init_slideshow_mode ();
    }

  main_loop ();
  D_LEAVE;
  return 0;
}


void
main_loop (void)
{
  XEvent ev;
  struct timeval tval;
  fd_set fdset;
  int xfd = 0, count = 0, fdsize = 0, j = 0;
  double t3 = 0.0, pt, t1 = 0.0, t2 = 0.0;
  fehtimer ft;

  D_ENTER;
  D (("window_num is %d\n", window_num));
  if (window_num == 0)
    exit (0);

  xfd = ConnectionNumber (disp);
  fdsize = xfd + 1;
  pt = feh_get_time ();

  for (;;)
    {
      XFlush (disp);
      /* Timers */
      t1 = feh_get_time ();
      t2 = t1 - pt;
      pt = t1;
      while (XPending (disp))
	{
	  XNextEvent (disp, &ev);
	  feh_handle_event (&ev);
	  if (window_num == 0)
	    exit (0);
	}

      FD_ZERO (&fdset);
      FD_SET (xfd, &fdset);

      /* If a window has a smooth timeout set, but it is NOT still
       * zooming, wait 0.2 secs and then smooth it */
      if (opt.timeout && !opt.zoom_mode)
	{
	  t3 = 0.2;
	  tval.tv_sec = (long) t3;
	  tval.tv_usec = (long) ((t3 - ((double) tval.tv_sec)) * 1000000);
	  D (("oo Performing wait then pass-thru select\n"));
	  count = select (fdsize, &fdset, NULL, NULL, &tval);
	  if ((count < 0)
	      && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF)))
	    eprintf
	      ("Connection to X display lost. Stitched up like a kipper.");

	  for (j = 0; j < window_num; j++)
	    if (windows[j]->timeout)
	      feh_smooth_image (windows[j]);
	}
      else
	{
	  /* Timers */
	  ft = first_timer;
	  /* Don't do timers if we're zooming */
	  if (ft && !opt.zoom_mode)
	    {
	      D (("There is a timer\n"));
	      if (ft->just_added)
		{
		  D (("The timer has just been added\n"));
		  D (("ft->in = %f\n", ft->in));
		  ft->just_added = 0;
		  t1 = ft->in;
		}
	      else
		{
		  D (("The timer was not just added\n"));
		  t1 = ft->in - t2;
		  if (t1 < 0.0)
		    t1 = 0.0;
		  ft->in = t1;
		}
	      D (("I need to action a timer in %f seconds\n", t1));
	      if (XPending (disp))
		continue;
	      tval.tv_sec = (long) t1;
	      tval.tv_usec = (long) ((t1 - ((double) tval.tv_sec)) * 1000000);
	      if (tval.tv_sec < 0)
		tval.tv_sec = 0;
	      if (tval.tv_usec <= 1000)
		tval.tv_usec = 1000;
	      D (
		 ("oo Performing blocking select - waiting for timer or event\n"));
	      count = select (fdsize, &fdset, NULL, NULL, &tval);
	      if ((count < 0)
		  && ((errno == ENOMEM) || (errno == EINVAL)
		      || (errno == EBADF)))
		eprintf ("Connection to X display lost");
	      if ((ft) && (count == 0))
		feh_handle_timer ();
	    }
	  else
	    {
	      if (XPending (disp))
		continue;
	      D (("oo Performing blocking select - no timers, or zooming\n"));
	      count = select (fdsize, &fdset, NULL, NULL, NULL);
	      if ((count < 0)
		  && ((errno == ENOMEM) || (errno == EINVAL)
		      || (errno == EBADF)))
		eprintf ("Connection to X display lost");
	    }
	}
      if (window_num == 0)
	exit (0);
    }
  D_LEAVE;
}

void
feh_handle_event (XEvent * ev)
{
  winwidget winwid = NULL;

  D_ENTER;

  switch (ev->type)
    {
    case KeyPress:
      D (("Received KeyPress event\n"));
      while (XCheckTypedWindowEvent (disp, ev->xkey.window, KeyPress, ev));
      handle_keypress_event (ev, ev->xkey.window);
      break;
    case ButtonPress:
      D (("Received ButtonPress event\n"));
      switch (ev->xbutton.button)
	{
	case 1:
	  D (("Button 1 Press event\n"));
	  winwid = winwidget_get_from_window (ev->xbutton.window);
	  if ((winwid != NULL) && (opt.slideshow))
	    slideshow_change_image (winwid, SLIDE_NEXT);
	  break;
	case 2:
	  D (("Button 2 Press event\n"));
	  if (!opt.full_screen)
	    {
	      winwid = winwidget_get_from_window (ev->xbutton.window);
	      if (winwid != NULL)
		{
		  D (("Enabling zoom mode\n"));
		  opt.zoom_mode = 1;
		  winwid->zoom_mode = 1;
		  winwid->zx = ev->xbutton.x;
		  winwid->zy = ev->xbutton.y;
		  imlib_context_set_anti_alias (0);
		  imlib_context_set_dither (1);
		  imlib_context_set_blend (0);
		  imlib_context_set_drawable (winwid->bg_pmap);
		  imlib_context_set_image (winwid->im);
		  if (imlib_image_has_alpha ())
		    {
		      imlib_context_set_blend (1);
		      feh_draw_checks (winwid);
		    }
		  imlib_context_set_image (winwid->im);
		  imlib_render_image_on_drawable (0, 0);
		  XSetWindowBackgroundPixmap (disp, winwid->win,
					      winwid->bg_pmap);
		  XClearWindow (disp, winwid->win);
		  if (opt.draw_filename)
		    feh_draw_filename (winwid);
		  XFlush (disp);
		}
	    }
	  break;
	case 4:
	  D (("Button 4 Press event\n"));
	  if (opt.slideshow)
	    {
	      winwid = winwidget_get_from_window (ev->xbutton.window);
	      if (winwid != NULL)
		slideshow_change_image (winwid, SLIDE_PREV);
	    }
	  break;
	case 5:
	  D (("Button 5 Press event\n"));
	  if (opt.slideshow)
	    {
	      winwid = winwidget_get_from_window (ev->xbutton.window);
	      if (winwid != NULL)
		slideshow_change_image (winwid, SLIDE_NEXT);
	    }
	  break;
	default:
	  D (("Recieved other ButtonPress event\n"));
	  break;
	}
      break;
    case ButtonRelease:
      D (("Received ButtonRelease event\n"));
      switch (ev->xbutton.button)
	{
	case 1:
	  D (("Button 1 Release event\n"));
	  break;
	case 2:
	  D (("Button 2 Release event\n"));
	  winwid = winwidget_get_from_window (ev->xbutton.window);
	  if (winwid != NULL)
	    {
	      winwid->zoom_mode = 0;
	      opt.zoom_mode = 0;
	    }
	  break;
	case 3:
	  D (("Button 3 Release event\n"));
	  winwidget_destroy_all ();
	  break;
	default:
	  break;
	}
      break;
    case MotionNotify:
      /* If zoom mode is set, then a window needs zooming, 'cos
       * button 2 is pressed */
      if (opt.zoom_mode)
	{
	  winwid = winwidget_get_from_window (ev->xmotion.window);
	  if (winwid != NULL)
	    {
	      if (winwid->zoom_mode)
		{
		  int sx, sy, sw, sh, dx, dy, dw, dh;
		  while (XCheckTypedWindowEvent
			 (disp, winwid->win, MotionNotify, ev));

		  imlib_context_set_anti_alias (0);
		  imlib_context_set_dither (0);
		  imlib_context_set_blend (0);

		  winwid->zoom =
		    ((double) ev->xmotion.x - (double) winwid->zx) / 32.0;
		  if (winwid->zoom < 0)
		    winwid->zoom =
		      1.0 +
		      ((winwid->zoom * 32.0) / ((double) (winwid->zx + 1)));
		  else
		    winwid->zoom += 1.0;
		  if (winwid->zoom <= 0.0001)
		    winwid->zoom = 0.0001;
		  if (winwid->zoom > 1.0)
		    {
		      dx = 0;
		      dy = 0;
		      dw = winwid->im_w;
		      dh = winwid->im_h;

		      sx = winwid->zx - (winwid->zx / winwid->zoom);
		      sy = winwid->zy - (winwid->zy / winwid->zoom);
		      sw = winwid->im_w / winwid->zoom;
		      sh = winwid->im_h / winwid->zoom;
		      if (imlib_image_has_alpha ())
			feh_draw_checks (winwid);
		    }
		  else
		    {
		      dx = winwid->zx - (winwid->zx * winwid->zoom);
		      dy = winwid->zy - (winwid->zy * winwid->zoom);
		      dw = winwid->im_w * winwid->zoom;
		      dh = winwid->im_h * winwid->zoom;

		      sx = 0;
		      sy = 0;
		      sw = winwid->im_w;
		      sh = winwid->im_h;
		      feh_draw_checks (winwid);
		    }
		  imlib_context_set_drawable (winwid->bg_pmap);
		  imlib_context_set_image (winwid->im);
		  if (imlib_image_has_alpha ())
		    imlib_context_set_blend (1);
		  imlib_render_image_part_on_drawable_at_size
		    (sx, sy, sw, sh, dx, dy, dw, dh);
		  XSetWindowBackgroundPixmap (disp,
					      winwid->win, winwid->bg_pmap);
		  XClearWindow (disp, winwid->win);
		  XFlush (disp);
		  winwid->timeout = 1;
		  D (("A window has opt.timeout set\n"));
		  opt.timeout = 1;
		}
	    }
	}
      break;
    case ClientMessage:
      if (ev->xclient.format == 32
	  && ev->xclient.data.l[0] == (signed) wmDeleteWindow)
	{
	  winwid = winwidget_get_from_window (ev->xclient.window);
	  if (winwid != NULL)
	    winwidget_destroy (winwid);
	}
      break;
    default:
      break;
    }
  D_LEAVE;
}


void
feh_smooth_image (winwidget w)
{
  int sx, sy, sw, sh, dx, dy, dw, dh;

  D_ENTER;

  if (w->zoom > 1.0)
    {
      dx = 0;
      dy = 0;
      dw = w->im_w;
      dh = w->im_h;

      sx = w->zx - (w->zx / w->zoom);
      sy = w->zy - (w->zy / w->zoom);
      sw = w->im_w / w->zoom;
      sh = w->im_h / w->zoom;
    }
  else
    {
      dx = w->zx - (w->zx * w->zoom);
      dy = w->zy - (w->zy * w->zoom);
      dw = w->im_w * w->zoom;
      dh = w->im_h * w->zoom;

      sx = 0;
      sy = 0;
      sw = w->im_w;
      sh = w->im_h;
    }
  imlib_context_set_anti_alias (1);
  imlib_context_set_dither (1);
  imlib_context_set_blend (0);
  imlib_context_set_drawable (w->bg_pmap);
  feh_draw_checks (w);
  imlib_context_set_image (w->im);
  if (imlib_image_has_alpha ())
    imlib_context_set_blend (1);
  imlib_render_image_part_on_drawable_at_size
    (sx, sy, sw, sh, dx, dy, dw, dh);
  XSetWindowBackgroundPixmap (disp, w->win, w->bg_pmap);
  XClearWindow (disp, w->win);
  XFlush (disp);
  w->timeout = 0;
  opt.timeout = 0;
  D_LEAVE;
}
