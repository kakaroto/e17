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

/* Main is here! Main kicks ass. FEAR MAIN :) */
int
main (int argc, char **argv)
{
  /* There must be at least one argument */
  if (argc < 2)
    show_usage ();

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
  static int x = -9999, y = -9999;
  winwidget winwid = NULL;
  XEvent ev;
  int timeout = 0;
  struct timeval tval;
  fd_set fdset;
  double t1;
  int xfd = 0, count = 0, fdsize = 0, j = 0;
  /* A global zoom mode to save cpu on motionnotify */
  int zoom_mode = 0;
  int rectangle_drawing_mode = 0;
  unsigned char rec_set = 0;
  int rec_x = 0;
  int rec_y = 0;

  D (("In main_loop, window_num is %d\n", window_num));
  if (window_num == 0)
    exit (0);

  xfd = ConnectionNumber (disp);
  fdsize = xfd + 1;

  for (;;)
    {
      XFlush (disp);
      while (XPending (disp))
	{
	  D (("In event loop - events pending\n"));
	  if (window_num == 0)
	    exit (0);
	  XNextEvent (disp, &ev);
	  switch (ev.type)
	    {
	    case Expose:
	      D (("Received Expose event\n"));
	      break;
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
			/* Draw some selection boundaries */
			if ((rectangles_on) && (!rectangle_drawing_mode))
			  {
			    rectangle_drawing_mode = 1;
			    winwid->rectangle_drawing_mode = 1;
			    /* reset the rectangle dimensions */
			    winwid->rec_x = 0;
			    winwid->rec_y = 0;
			    winwid->rec_w = 0;
			    winwid->rec_h = 0;
			    rec_set = 0;
			    rec_x = 0;
			    rec_y = 0;
			  }
			else if (opt.slideshow)
			  {
			    slideshow_change_image (winwid, SLIDE_NEXT);
			  }
		      }
		    }
		  break;
		case 2:
		  D (("Button 2 Press event\n"));
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
		      imlib_render_image_on_drawable (0, 0);
		      XSetWindowBackgroundPixmap (disp, winwid->win,
						  winwid->bg_pmap);
		      XClearWindow (disp, winwid->win);
		      XFlush (disp);
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
		  winwid = winwidget_get_from_window (ev.xbutton.window);
		  if (winwid != NULL)
		    {
		      if ((rectangles_on) && (rectangle_drawing_mode))
			{
			  rectangle_drawing_mode = 0;
			  winwid->rectangle_drawing_mode = 0;

			  /* Maybe we could just redraw the "dirty"
			   * area, it would be less expensive */
			  XClearWindow (disp, winwid->win);

			  x = -9999;
			  y = -9999;
			}
		    }
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
	      if (rectangle_drawing_mode)
		{
		  winwid = winwidget_get_from_window (ev.xmotion.window);
		  if ((winwid) && (winwid->rectangle_drawing_mode))
		    {
		      static GC gc = 0;
		      static XGCValues gcv;
		      while (XCheckTypedWindowEvent
			     (disp, winwid->win, MotionNotify, &ev));

		      if (x == -9999)
			{
			  D (("Creating GC\n"));
			  /* Create/recreate the gc */
			  if (gc)
			    XFreeGC (disp, gc);
			  gcv.function = GXxor;
			  /* LineSolid, LineDoubleDash, LineOnOffDash */
			  gcv.line_style=LineOnOffDash;
			  gcv.foreground =
			    WhitePixel (disp, DefaultScreen (disp));
			  gc =
			    XCreateGC (disp, winwid->win,
				       GCFunction | GCForeground | GCLineStyle, &gcv);
			}
		      else
			{
			  D (("Overwriting old rectangle\n"));
			  /* Overwrite old rectangle */
			  XDrawRectangle (disp, winwid->win, gc,
					  winwid->rec_x, winwid->rec_y,
					  winwid->rec_w, winwid->rec_h);
			}
		      x = ev.xmotion.x;
		      y = ev.xmotion.y;
		      if (x < 0)
			x = 0;
		      if (y < 0)
			y = 0;
		      if (!rec_set)
			{
			  rec_x = x;
			  rec_y = y;
			  rec_set = 1;
			}
		      winwid->rec_x = rec_x;
		      winwid->rec_y = rec_y;
		      winwid->rec_w = x - winwid->rec_x;
		      winwid->rec_h = y - winwid->rec_y;
		      /* if im drawing, left or up, lets swap */
		      if (winwid->rec_h < 0)
			{
			  winwid->rec_y += winwid->rec_h;
			  winwid->rec_h += -(2 * winwid->rec_h);
			}
		      if (winwid->rec_w < 0)
			{
			  winwid->rec_x += winwid->rec_w;
			  winwid->rec_w += -(2 * winwid->rec_w);
			}
		      /* Boundry checking */
		      /* Yu */
		      if (winwid->rec_y < 0)
			winwid->rec_y = 0;
		      /* Xl */
		      if (winwid->rec_x < 0)
			winwid->rec_x = 0;
		      /* Xr */
		      if (winwid->rec_x + winwid->rec_w > winwid->im_w)
			winwid->rec_w = (winwid->im_w - 1) - winwid->rec_x;
		      /* Yd */
		      if (winwid->rec_y + winwid->rec_h > winwid->im_h)
			winwid->rec_h = (winwid->im_h - 1) - winwid->rec_y;

		      XDrawRectangle (disp, winwid->win, gc,
				      winwid->rec_x, winwid->rec_y,
				      winwid->rec_w, winwid->rec_h);
		    }
		}
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

	  D (("Performing select, timeout is %d\n", timeout));
	  /* If a window has a smooth timeout set, but it is NOT still
	   * zooming, wait 0.2 secs and then smooth it ::) */
	  if (timeout && !zoom_mode)
	    {
	      t1 = 0.2;
	      tval.tv_sec = (long) t1;
	      tval.tv_usec = (long) ((t1 - ((double) tval.tv_sec)) * 1000000);
	      FD_ZERO (&fdset);
	      FD_SET (xfd, &fdset);
	      D (("oo Performing wait then pass-thru select\n"));
	      count = select (fdsize, &fdset, NULL, NULL, &tval);
	      if ((count < 0)
		  && ((errno == ENOMEM) || (errno == EINVAL)
		      || (errno == EBADF)))
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
			    windows[j]->zx -
			    (windows[j]->zx / windows[j]->zoom);
			  sy =
			    windows[j]->zy -
			    (windows[j]->zy / windows[j]->zoom);
			  sw = windows[j]->im_w / windows[j]->zoom;
			  sh = windows[j]->im_h / windows[j]->zoom;
			}
		      else
			{
			  dx =
			    windows[j]->zx -
			    (windows[j]->zx * windows[j]->zoom);
			  dy =
			    windows[j]->zy -
			    (windows[j]->zy * windows[j]->zoom);
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
	  else if (!XPending (disp))
	    {
	      /* Ok, there's no stuff I need to go back and do. (No point
	       * waiting for new events from X when I know there are some
	       * in the queue), so I can sit and block while waiting for
	       * new events */
	      FD_ZERO (&fdset);
	      FD_SET (xfd, &fdset);
	      D (("oo Performing blocking select\n"));
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
