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
  D (("In main_loop\n"));
  if (window_num == 0)
    exit (0);
  while (1)
    {
      do
	{
	  XNextEvent (disp, &ev);
	  switch (ev.type)
	    {
	    case Expose:
	      break;
	    case ButtonRelease:
	      switch (ev.xbutton.button)
		{
		case 1:
		  break;
		case 2:
		  winwid = winwidget_get_from_window (ev.xexpose.window);
		  if (winwid != NULL)
		    winwidget_destroy (winwid);
		  break;
		case 3:
		  winwidget_destroy_all();
		  break;
		default:
		  break;
		}
	      break;
	    case MotionNotify:
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
	}
      while (XPending (disp));
      if (window_num == 0)
	exit (0);
    }
}
