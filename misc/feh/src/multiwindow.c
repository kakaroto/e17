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
  int i;
  winwidget w = NULL;

  D (("In init_multiwindow_mode\n"));

  actual_file_num = file_num;

  for (i = 0; i < file_num; i++)
    {
      char *s = NULL;
      int len = 0;
      len = strlen (PACKAGE " - ") + strlen (files[i]) + 1;
      s = emalloc (len);
      snprintf (s, len, PACKAGE " - %s", files[i]);

      if ((w = winwidget_create_from_file (files[i], s)) != NULL)
	{
	  if (!opt.progressive)
	    winwidget_show (w);
	}
      else
	actual_file_num--;
      free (s);
    }
}
