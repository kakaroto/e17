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

  if (opt.slideshow)
    {
      int success = 0;
      for (opt.cur_slide = 0; opt.cur_slide < file_num; opt.cur_slide++)
	{
	  char *s = NULL;
	  s = slideshow_create_name (files[opt.cur_slide]);
	  if ((w = winwidget_create_from_file (files[opt.cur_slide], s)) !=
	      NULL)
	    {
	      free (s);
	      winwidget_show (w);
	      success = 1;
	      break;
	    }
	  else
	    free (s);
	}
      if (!success)
	{
	  fprintf (stderr, "Error. No valid images found for loading\n");
	  exit (2);
	}
    }
  else
    {
      for (i = 0; i < file_num; i++)
	{
	  char *s = NULL;
	  int len = 0;
	  len = strlen (PACKAGE " - ") + strlen (files[i]) + 1;
	  if ((s = malloc (len)) == NULL)
	    {
	      fprintf (stderr, "Out of memory. Blargh.\n");
	    }
	  snprintf (s, len, PACKAGE " - %s", files[i]);

	  if ((w = winwidget_create_from_file (files[i], s)) != NULL)
	    winwidget_show (w);
	  free (s);
	}
    }
}

void
slideshow_next_image (winwidget winwid)
{
  int i;

  if (winwid->im)
    {
      imlib_context_set_image (winwid->im);
      imlib_free_image ();
    }

  /* The for loop prevents us looping infinitely */
  for (i = 0; i < file_num; i++)
    {
      opt.cur_slide++;
      if (opt.cur_slide >= file_num)
	opt.cur_slide = 0;
      if (opt.progressive)
	{
	  progwin = winwid;
	  imlib_context_set_progress_function (progress);
	  imlib_context_set_progress_granularity (10);
	  winwid->im_w = 0;
	  winwid->im_h = 0;
	  winwid->w = 0;
	  winwid->h = 0;
	}
      if ((feh_load_image (&(winwid->im), files[opt.cur_slide])) != 0)
	{
	  winwid->zoom_mode = 0;
	  winwid->zoom = 0.0;
	  if (!opt.progressive)
	    {
	      imlib_context_set_image (winwid->im);
	      winwid->im_w = imlib_image_get_width ();
	      winwid->im_h = imlib_image_get_height ();
	      winwidget_render_image (winwid);
	      winwidget_create_blank_bg (winwid);
	    }

	  if (winwid->name)
	    {
	      free (winwid->name);
	      winwid->name = slideshow_create_name (files[opt.cur_slide]);
	      winwidget_update_title (winwid);
	    }
	  break;
	}
    }
}

char *
slideshow_create_name (char *filename)
{
  char *s = NULL;
  int len = 0;
  len = strlen (PACKAGE " [slideshow mode] - ") + strlen (filename) + 1;
  if ((s = malloc (len)) == NULL)
    {
      fprintf (stderr, "Out of memory. Blargh.\n");
    }
  snprintf (s, len, PACKAGE " [slideshow mode] - %s", filename);
  return s;
}
