/* slideshow.c
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
init_slideshow_mode (void)
{
  winwidget w = NULL;
  int success = 0;
  char *s = NULL;

  D (("In init_slideshow_mode\n"));

  for (opt.cur_slide = 0; opt.cur_slide < file_num; opt.cur_slide++)
    {
      s = slideshow_create_name (files[opt.cur_slide]);
      if ((w = winwidget_create_from_file (files[opt.cur_slide], s)) != NULL)
	{
	  free (s);
	  success = 1;
	  if (!opt.progressive)
	    winwidget_show (w);
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

void
slideshow_next_image (winwidget winwid)
{
  int i;

  /* The for loop prevents us looping infinitely */
  for (i = 0; i < file_num; i++)
    {
      if (winwid->im)
	{
	  /* I would leave these in the cache, but its a big mem 
	   * penalty for large slideshows. */
	  imlib_context_set_image (winwid->im);
	  imlib_free_image_and_decache ();
	}

      opt.cur_slide++;
      if (opt.cur_slide >= file_num)
	{
	  opt.cur_slide = 0;
	  /* If we keep images in cache, we need to uncomment the next
	   * line */
	  /* Been through them all, so they are cached. Cached images
	   * don't call the progress callback function */
	  /* opt.progressive=0; */
	}
      D (("file_num %d, currently %d\n", file_num, opt.cur_slide));
      if (opt.progressive)
	{
	  progwin = winwid;
	  imlib_context_set_progress_function (progress);
	  imlib_context_set_progress_granularity (10);
	  winwid->im_w = 0;
	  winwid->im_h = 0;
	  winwid->w = 0;
	  winwid->h = 0;
	  D (("Setting stuff to zero\n"));
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
