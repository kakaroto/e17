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
  feh_file *file = NULL, *last = NULL;

  D (("In init_slideshow_mode\n"));

  for (file = filelist; file; file = file->next)
    {
      if (last)
	{
	  filelist = filelist_remove_file (filelist, last);
	  last = NULL;
	}
      current_file = file;
      s = slideshow_create_name (file->filename);
      if ((w = winwidget_create_from_file (file, s)) != NULL)
	{
	  free (s);
	  success = 1;
	  w->file = file;
	  if (opt.draw_filename)
	    feh_draw_filename (w);
	  if (!opt.progressive)
	    winwidget_show (w);
	  if (opt.slideshow_delay > 0)
	    feh_add_timer (cb_slide_timer, w, opt.slideshow_delay,
			   "SLIDE_CHANGE");
	  else if (opt.reload > 0)
	    feh_add_unique_timer (cb_reload_timer, w, opt.reload);
	  break;
	}
      else
	{
	  free (s);
	  last = file;
	}
    }
  if (!success)
    show_mini_usage ();
}

void
cb_slide_timer (void *data)
{
  D (("In cb_slide_timer\n"));
  slideshow_change_image ((winwidget) data, SLIDE_NEXT);
}

void
cb_reload_timer (void *data)
{
  winwidget w = (winwidget) data;
  D (("In cb_reload_timer\n"));

  if (w->im)
    {
      imlib_context_set_image (w->im);
      imlib_free_image_and_decache ();
    }

  if (opt.progressive)
    {
      /* Yeah, we have to do this stuff for progressive loading, so
       * the callback knows it's got to create a new image... */
      progwin = w;
      imlib_context_set_progress_function (progress);
      imlib_context_set_progress_granularity (10);
      w->im_w = 0;
      w->im_h = 0;
      w->w = 0;
      w->h = 0;
    }
  if ((feh_load_image (&(w->im), w->file)) != 0)
    {
      w->zoom_mode = 0;
      w->zoom = 0.0;
      if (!opt.progressive)
	{
	  imlib_context_set_image (w->im);
	  w->im_w = imlib_image_get_width ();
	  w->im_h = imlib_image_get_height ();
	  winwidget_render_image (w);
	}
      if (opt.draw_filename)
	feh_draw_filename (w);
    }
  else
    eprintf ("Couldn't reload image. Is it still there?");

  feh_add_unique_timer (cb_reload_timer, w, opt.reload);
}

void
slideshow_change_image (winwidget winwid, int change)
{
  int success = 0;
  feh_file *last = NULL;
  int i = 0, file_num = 0;

  D (("In slideshow_change_image\n"));

  file_num = filelist_length (filelist);

  /* Without this, clicking a one-image slideshow reloads it. Not very
   * intelligent behaviour :-) */
  if (file_num < 2)
    return;

  /* Ok. I do this in such an odd way to ensure that if the last or first
   * image is not loadable, it will go through in the right direction to
   * find the correct one. Otherwise SLIDE_LAST would try the last file,
   * then loop forward to find a loadable one. */
  if (change == SLIDE_FIRST)
    {
      current_file = filelist_last (filelist);
      change = SLIDE_NEXT;
    }
  else if (change == SLIDE_LAST)
    {
      current_file = filelist;
      change = SLIDE_PREV;
    }

  /* The for loop prevents us looping infinitely */
  for (i = 0; i < file_num; i++)
    {
      if (winwid->im)
	{
	  /* I would leave these in the cache, but its a big mem 
	   * penalty for large slideshows. (In fact it brought down
	   * ljlane's box ;-) */
	  imlib_context_set_image (winwid->im);
	  imlib_free_image_and_decache ();
	}
      if (change == SLIDE_NEXT)
	{
	  if (current_file->next)
	    current_file = current_file->next;
	  else
	    current_file = filelist;
	}
      else if (change == SLIDE_PREV)
	{
	  if (current_file->prev)
	    current_file = current_file->prev;
	  else
	    current_file = filelist_last (current_file);
	}
      if (last)
	{
	  filelist = filelist_remove_file (filelist, last);
	  last = NULL;
	}
      if (opt.progressive)
	{
	  /* Yeah, we have to do this stuff for progressive loading, so
	   * the callback knows it's got to create a new image... */
	  progwin = winwid;
	  imlib_context_set_progress_function (progress);
	  imlib_context_set_progress_granularity (10);
	  winwid->im_w = 0;
	  winwid->im_h = 0;
	  winwid->w = 0;
	  winwid->h = 0;
	}
      if (winwid->name)
	free (winwid->name);
      winwid->name = slideshow_create_name (current_file->filename);
      winwidget_update_title (winwid);
      if ((feh_load_image (&(winwid->im), current_file)) != 0)
	{
	  success = 1;
	  winwid->zoom_mode = 0;
	  winwid->zoom = 0.0;
	  winwid->file = current_file;
	  if (!opt.progressive)
	    {
	      imlib_context_set_image (winwid->im);
	      winwid->im_w = imlib_image_get_width ();
	      winwid->im_h = imlib_image_get_height ();
	      winwidget_render_image (winwid);
	    }
	  if (opt.draw_filename)
	    feh_draw_filename (winwid);
	  break;
	}
      else
	last = current_file;
    }
  if (!success)
    {
      /* We didn't manage to load any files. Maybe the last one in the
       * show was deleted? */
      eprintf ("No more slides in show");
    }
  if (opt.slideshow_delay > 0)
    feh_add_timer (cb_slide_timer, winwid, opt.slideshow_delay,
		   "SLIDE_CHANGE");
}

char *
slideshow_create_name (char *filename)
{
  char *s = NULL;
  int len = 0;
  len = strlen (PACKAGE " [slideshow mode] - ") + strlen (filename) + 1;
  s = emalloc (len);
  snprintf (s, len, PACKAGE " [%d of %d] - %s",
	    filelist_num (filelist, current_file) + 1,
	    filelist_length (filelist), filename);
  return s;
}

void
feh_action_run (winwidget w)
{
  char *sys;
  D (("In feh_action_run\n"));
  D (("   Running action %s\n", opt.action));

  sys = feh_printf (opt.action, w);

  if (opt.verbose)
    fprintf (stderr, "Running action -->%s<--\n", sys);
  system (sys);
}

char *
feh_printf (char *str, winwidget w)
{
  char *c;
  char buf[20];
  static char ret[4096];
  D (("In feh_printf\n"));

  ret[0] = '\0';

  for (c = str; *c != '\0'; c++)
    {
      if (*c == '%')
	{
	  c++;
	  switch (*c)
	    {
	    case 'f':
	      strcat (ret, w->file->filename);
	      break;
	    case 'n':
	      strcat (ret, w->file->name);
	      break;
	    case 'w':
	      if (!w->file->info)
		feh_file_info_load (w->file);
	      snprintf (buf, sizeof (buf), "%d", w->file->info->width);
	      strcat (ret, buf);
	      break;
	    case 'h':
	      if (!w->file->info)
		feh_file_info_load (w->file);
	      snprintf (buf, sizeof (buf), "%d", w->file->info->height);
	      strcat (ret, buf);
	      break;
	    case 's':
	      if (!w->file->info)
		feh_file_info_load (w->file);
	      snprintf (buf, sizeof (buf), "%d", w->file->info->size);
	      strcat (ret, buf);
	      break;
	    case 'p':
	      if (!w->file->info)
		feh_file_info_load (w->file);
	      snprintf (buf, sizeof (buf), "%d", w->file->info->pixels);
	      strcat (ret, buf);
	      break;
	    case 't':
	      if (!w->file->info)
		feh_file_info_load (w->file);
	      strcat (ret, w->file->info->format);
	      break;
	    default:
	      strncat (ret, c, 1);
	      break;
	    }
	}
      else
	strncat (ret, c, 1);
    }
  return ret;
}
