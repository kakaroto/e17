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

  main_loop ();
  return 0;
}


void
main_loop (void)
{
  winwidget winwid = NULL;
  XEvent ev;
  int timeout = 0;
  int x = -9999, y = -9999;
  struct timeval tval;
  fd_set fdset;
  double t1;
  int xfd, count, fdsize, j;

  D (("In main_loop, window_num is %d\n", window_num));
  if (window_num == 0)
    exit (0);
  for (;;)
    {
      XFlush (disp);
      while (XPending (disp))
	{
      D(("In event loop - events pending\n"));
	  if (window_num == 0)
	    exit (0);
	  XNextEvent (disp, &ev);
	  switch (ev.type)
	    {
	    case Expose:
	      D (("Received Expose event\n"));
	      break;
	    case ButtonPress:
	      D (("Received ButtonPress event\n"));
	      x = ev.xbutton.x;
	      y = ev.xbutton.y;
	      switch (ev.xbutton.button)
		{
		case 1:
		  D (("Button 1 Press event\n"));
		  if (opt.slideshow)
		    {
		      winwid = winwidget_get_from_window (ev.xbutton.window);
		      if (winwid != NULL)
			slideshow_next_image (winwid);
		    }
		  break;
		case 2:
		  D (("Button 2 Press event\n"));
		  winwid = winwidget_get_from_window (ev.xbutton.window);
		  if (winwid != NULL)
		    {
		      D (("  Enabling zoom mode\n"));
		      winwid->zoom_mode = 1;
		      winwid->zx = x;
		      winwid->zy = y;
		      imlib_context_set_anti_alias (0);
		      imlib_context_set_dither (0);
		      imlib_context_set_blend (1);
		      imlib_context_set_drawable (winwid->bg_pmap);
		      imlib_context_set_image (winwid->blank_im);
		      imlib_render_image_on_drawable (0, 0);
		      imlib_context_set_image (winwid->im);
		      imlib_render_image_on_drawable (0, 0);
		      XSetWindowBackgroundPixmap (disp, winwid->win,
						  winwid->bg_pmap);
		      XClearWindow (disp, winwid->win);
		      XFlush (disp);
		    }
		  break;
		default:
		  D (("Recieved other ButtonPress event\n"));
		  break;
		}
	      break;
	    case ButtonRelease:
	      D (("Received ButtonRelease event\n"));
	      x = ev.xbutton.x;
	      y = ev.xbutton.y;
	      switch (ev.xbutton.button)
		{
		case 2:
		  D (("Button 2 Release event\n"));
		  winwid = winwidget_get_from_window (ev.xbutton.window);
		  if (winwid != NULL)
		    {
		      winwid->zoom_mode = 0;
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
	      D (("Received MotionNotify event\n"));
	      winwid = winwidget_get_from_window (ev.xmotion.window);
	      if (winwid != NULL)
		{
		  while (XCheckTypedWindowEvent
			 (disp, winwid->win, MotionNotify, &ev));
		  x = ev.xmotion.x;
		  y = ev.xmotion.y;
		  if (winwid->zoom_mode)
		    {
		      int sx, sy, sw, sh, dx, dy, dw, dh;

		      imlib_context_set_anti_alias (0);
		      imlib_context_set_dither (0);
		      imlib_context_set_blend (1);

		      winwid->zoom =
			((double) x - (double) winwid->zx) / 32.0;
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
		      imlib_context_set_drawable (winwid->bg_pmap);
		      imlib_context_set_image (winwid->blank_im);
		      imlib_render_image_on_drawable (0, 0);
		      imlib_context_set_image (winwid->im);
		      imlib_render_image_part_on_drawable_at_size
			(sx, sy, sw, sh, dx, dy, dw, dh);
		      XSetWindowBackgroundPixmap (disp, winwid->win,
						  winwid->bg_pmap);
		      XClearWindow (disp, winwid->win);
		      XFlush (disp);
		      winwid->timeout = 1;
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
	  t1 = 0.2;
	  tval.tv_sec = (long) t1;
	  tval.tv_usec = (long) ((t1 - ((double) tval.tv_sec)) * 1000000);
	  xfd = ConnectionNumber (disp);
	  fdsize = xfd + 1;
	  FD_ZERO (&fdset);
	  FD_SET (xfd, &fdset);

	  /* See if any windows need updating */
	  for (j = 0; j < window_num; j++)
	    if (windows[j]->timeout)
	      {
		timeout = 1;
		D (("A window has timeout set\n"));
		break;
	      }
	  D(("Performing select, timeout is %d\n",timeout));
	  if (timeout)
	    count = select (fdsize, &fdset, NULL, NULL, &tval);
	  else
	    count = select (fdsize, &fdset, NULL, NULL, NULL);
	  if (count < 0)
	    {
	      if ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF))
		exit (1);
	    }
	  else
	    {
	      for (j = 0; j < window_num; j++)
		{
		  if ((count == 0) && (windows[j]->timeout))
		    {
		      int sx, sy, sw, sh, dx, dy, dw, dh;

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
		      imlib_context_set_blend (1);
		      imlib_context_set_drawable (windows[j]->bg_pmap);
		      imlib_context_set_image (windows[j]->blank_im);
		      imlib_render_image_on_drawable (0, 0);
		      imlib_context_set_image (windows[j]->im);
		      imlib_render_image_part_on_drawable_at_size
			(sx, sy, sw, sh, dx, dy, dw, dh);
		      XSetWindowBackgroundPixmap (disp, windows[j]->win,
						  windows[j]->bg_pmap);
		      XClearWindow (disp, windows[j]->win);
		      XFlush (disp);
		      windows[j]->timeout = 0;
		      timeout=0;
		    }
		}
	    }
	}
      if (window_num == 0)
	exit (0);
    }
}
