/* main.c
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
#include "main.h"

int
main (int argc, char **argv)
{
  /* There must be at least one argument */
  if (argc < 2)
    show_usage ();

  atexit (delete_rm_files);

  init_parse_options (argc, argv);

  init_x_and_imlib ();

  if (opt.montage)
    init_montage_mode ();
  else if (opt.index)
    init_index_mode ();
  else if (opt.multiwindow)
    init_multiwindow_mode ();
  else
    {
      /* Slideshow mode is now the default. 'Cos its spiffy */
      opt.slideshow = 1;
      init_slideshow_mode ();
    }

  main_loop ();
  return 0;
}

void
main_loop (void)
{
  winwidget winwid = NULL;
  XEvent ev;
  int timeout = 0;
  struct timeval tval;
  fd_set fdset;
  int xfd = 0, count = 0, fdsize = 0, j = 0;
  int zoom_mode = 0;
  double t3 = 0.0, pt, t1 = 0.0, t2 = 0.0;
  fehtimer ft;

  D (("In main_loop, window_num is %d\n", window_num));
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
	  D (("In event loop - events pending\n"));
	  if (window_num == 0)
	    exit (0);
	  XNextEvent (disp, &ev);
	  switch (ev.type)
	    {
	    case KeyPress:
	      D (("Received KeyPress event\n"));
	      while (XCheckTypedWindowEvent
		     (disp, ev.xkey.window, KeyPress, &ev));
	      handle_keypress_event (&ev, ev.xkey.window);
	      break;
	    case ButtonPress:
	      D (("Received ButtonPress event\n"));
	      switch (ev.xbutton.button)
		{
		case 1:
		  D (("Button 1 Press event\n"));
		  winwid = winwidget_get_from_window (ev.xbutton.window);
		  if (winwid != NULL)
		    {
		      {
			if (opt.slideshow)
			  {
			    slideshow_change_image (winwid, SLIDE_NEXT);
			  }
		      }
		    }
		  break;
		case 2:
		  D (("Button 2 Press event\n"));
		  if (!opt.full_screen)
		    {
		      winwid = winwidget_get_from_window (ev.xbutton.window);
		      if (winwid != NULL)
			{
			  D (("  Enabling zoom mode\n"));
			  zoom_mode = 1;
			  winwid->zoom_mode = 1;
			  winwid->zx = ev.xbutton.x;
			  winwid->zy = ev.xbutton.y;
			  imlib_context_set_anti_alias (0);
			  imlib_context_set_dither (0);
			  imlib_context_set_blend (0);
			  imlib_context_set_drawable (winwid->bg_pmap);
			  imlib_context_set_image (winwid->im);
			  if (imlib_image_has_alpha ())
			    {
			      imlib_context_set_blend (1);
			      feh_draw_checks (winwid);
			    }
			  imlib_context_set_image (winwid->im);
			  if (opt.full_screen)
			    imlib_render_image_on_drawable (
							    (scr->width -
							     progwin->im_w) >>
							    1,
							    (scr->height -
							     progwin->im_h) >>
							    1);
			  else
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
		      winwid = winwidget_get_from_window (ev.xbutton.window);
		      if (winwid != NULL)
			slideshow_change_image (winwid, SLIDE_PREV);
		    }
		  break;
		case 5:
		  D (("Button 5 Press event\n"));
		  if (opt.slideshow)
		    {
		      winwid = winwidget_get_from_window (ev.xbutton.window);
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
	      switch (ev.xbutton.button)
		{
		case 1:
		  D (("Button 1 Release event\n"));
		  break;
		case 2:
		  D (("Button 2 Release event\n"));
		  winwid = winwidget_get_from_window (ev.xbutton.window);
		  if (winwid != NULL)
		    {
		      winwid->zoom_mode = 0;
		      zoom_mode = 0;
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
	      if (zoom_mode)
		{
		  winwid = winwidget_get_from_window (ev.xmotion.window);
		  if (winwid != NULL)
		    {
		      if (winwid->zoom_mode)
			{
			  int sx, sy, sw, sh, dx, dy, dw, dh;
			  while (XCheckTypedWindowEvent
				 (disp, winwid->win, MotionNotify, &ev));

			  winwid->zoom =
			    ((double) ev.xmotion.x -
			     (double) winwid->zx) / 32.0;
			  if (winwid->zoom < 0)
			    winwid->zoom =
			      1.0 +
			      ((winwid->zoom * 32.0) /
			       ((double) (winwid->zx + 1)));
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
			    }
			  imlib_context_set_anti_alias (0);
			  imlib_context_set_dither (0);
			  imlib_context_set_blend (0);
			  imlib_context_set_drawable (winwid->bg_pmap);
			  feh_draw_checks (winwid);
			  imlib_context_set_image (winwid->im);
			  if (imlib_image_has_alpha ())
			    imlib_context_set_blend (1);
			  imlib_render_image_part_on_drawable_at_size
			    (sx, sy, sw, sh, dx, dy, dw, dh);
			  XSetWindowBackgroundPixmap (disp,
						      winwid->win,
						      winwid->bg_pmap);
			  XClearWindow (disp, winwid->win);
			  XFlush (disp);
			  winwid->timeout = 1;
			  D (("A window has timeout set\n"));
			  timeout = 1;
			}
		    }
		}
	      break;
	    case ClientMessage:
	      if (ev.xclient.format == 32
		  && ev.xclient.data.l[0] == (signed) wmDeleteWindow)
		{
		  winwid = winwidget_get_from_window (ev.xclient.window);
		  if (winwid != NULL)
		    winwidget_destroy (winwid);
		}
	      break;
	    default:
	      break;
	    }
	  if (window_num == 0)
	    exit (0);
	}

      FD_ZERO (&fdset);
      FD_SET (xfd, &fdset);

      /* If a window has a smooth timeout set, but it is NOT still
       * zooming, wait 0.2 secs and then smooth it ::) */
      if (timeout && !zoom_mode)
	{
	  t3 = 0.2;
	  tval.tv_sec = (long) t3;
	  tval.tv_usec = (long) ((t3 - ((double) tval.tv_sec)) * 1000000);
	  D (("oo Performing wait then pass-thru select\n"));
	  count = select (fdsize, &fdset, NULL, NULL, &tval);
	  if ((count < 0)
	      && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF)))
	    eprintf ("Connection to X display lost");

	  for (j = 0; j < window_num; j++)
	    {
	      if (windows[j]->timeout)
		{
		  int sx, sy, sw, sh, dx, dy, dw, dh;

		  D (("Performing smoothing\n"));

		  if (windows[j]->zoom > 1.0)
		    {
		      dx = 0;
		      dy = 0;
		      dw = windows[j]->im_w;
		      dh = windows[j]->im_h;

		      sx =
			windows[j]->zx - (windows[j]->zx / windows[j]->zoom);
		      sy =
			windows[j]->zy - (windows[j]->zy / windows[j]->zoom);
		      sw = windows[j]->im_w / windows[j]->zoom;
		      sh = windows[j]->im_h / windows[j]->zoom;
		    }
		  else
		    {
		      dx =
			windows[j]->zx - (windows[j]->zx * windows[j]->zoom);
		      dy =
			windows[j]->zy - (windows[j]->zy * windows[j]->zoom);
		      dw = windows[j]->im_w * windows[j]->zoom;
		      dh = windows[j]->im_h * windows[j]->zoom;

		      sx = 0;
		      sy = 0;
		      sw = windows[j]->im_w;
		      sh = windows[j]->im_h;
		    }
		  imlib_context_set_anti_alias (1);
		  imlib_context_set_dither (1);
		  imlib_context_set_blend (0);
		  imlib_context_set_drawable (windows[j]->bg_pmap);
		  feh_draw_checks (windows[j]);
		  imlib_context_set_image (windows[j]->im);
		  if (imlib_image_has_alpha ())
		    imlib_context_set_blend (1);
		  imlib_render_image_part_on_drawable_at_size
		    (sx, sy, sw, sh, dx, dy, dw, dh);
		  XSetWindowBackgroundPixmap (disp,
					      windows[j]->win,
					      windows[j]->bg_pmap);
		  XClearWindow (disp, windows[j]->win);
		  XFlush (disp);
		  windows[j]->timeout = 0;
		  timeout = 0;
		}
	    }
	}
      else
	{
	  /* Timers */
	  ft = first_timer;
	  /* Don't do timers if we're zooming */
	  if (ft && !zoom_mode)
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
}
