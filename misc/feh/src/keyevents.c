/* keyevents.c
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
#include "modify.h"

void
handle_keypress_event (XEvent * ev, Window win)
{
  int len;
  static char kbuf[20];
  KeySym keysym;
  XKeyEvent *kev;
  winwidget winwid = NULL;

  D (("In handle_keypress_event\n"));

  winwid = winwidget_get_from_window (win);
  if (winwid == NULL)
    return;

  kev = (XKeyEvent *) ev;
  len =
    XLookupString (&ev->xkey, (char *) kbuf, sizeof (kbuf), &keysym, NULL);

  switch (keysym)
    {
    case XK_Left:
      if (opt.slideshow)
	slideshow_change_image (winwid, SLIDE_PREV);
      break;
    case XK_Right:
      if (opt.slideshow)
	slideshow_change_image (winwid, SLIDE_NEXT);
      break;
    case XK_Delete:
      /* I could do with some confirmation here */
      /* How about holding ctrl? */
      if (opt.slideshow)
	{
	  if (kev->state & ControlMask)
	    {
	      unlink (files[opt.cur_slide]);
	      files[opt.cur_slide] = NULL;
	      actual_file_num--;
	      slideshow_change_image (winwid, SLIDE_NEXT);
	    }
	}
      break;
    case XK_Home:
    case XK_KP_Home:
      if (opt.slideshow)
	slideshow_change_image (winwid, SLIDE_FIRST);
      break;
    case XK_End:
    case XK_KP_End:
      if (opt.slideshow)
	slideshow_change_image (winwid, SLIDE_LAST);
      break;
    default:
      break;
    }

  if (len <= 0 || len > (int) sizeof (kbuf))
    return;

  kbuf[len] = '\0';

  switch (*kbuf)
    {
    case 'n':
    case 'N':
    case ' ':
      if (opt.slideshow)
	slideshow_change_image (winwid, SLIDE_NEXT);
      break;
    case 'p':
    case 'P':
    case '\b':
      if (opt.slideshow)
	slideshow_change_image (winwid, SLIDE_PREV);
      break;
    case 'q':
    case 'Q':
      winwidget_destroy_all ();
      break;
    default:
      break;
    }
}
