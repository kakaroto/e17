/* multiwindow.c
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

void
init_multiwindow_mode (void)
{
  winwidget w = NULL;
  feh_file *file;

  D (("In init_multiwindow_mode\n"));

  for (file = filelist; file; file = file->next)
    {
      char *s = NULL;
      int len = 0;
      len = strlen (PACKAGE " - ") + strlen (file->filename) + 1;
      s = emalloc (len);
      snprintf (s, len, PACKAGE " - %s", file->filename);

      if ((w = winwidget_create_from_file (file, s)) != NULL)
      {
	  w->file = file;
	  if(opt.draw_filename)
		feh_draw_filename(w);
	  if (!opt.progressive)
	    winwidget_show (w);
	  if (opt.reload > 0)
	    feh_add_unique_timer (cb_reload_timer, w, opt.reload);
	}
      else
	{
	  D (("EEEK. Couldn't load image in multiwindow mode. "
	      "I 'm not sure if this is a problem\n"));
	}
      free (s);
    }
}
