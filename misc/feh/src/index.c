/* index.c
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

static char *create_index_dimension_string (int w, int h);
static char *create_index_size_string (char *file);
static char *create_index_title_string (int num, int w, int h);

/* TODO Break this up a bit ;) */
/* TODO s/bit/lot */
void
init_index_mode (void)
{
  Imlib_Image *im_main;
  Imlib_Image *im_temp;
  int w = 800, h = 600, ww = 0, hh = 0, www, hhh, xxx, yyy;
  int x = 0, y = 0;
  int bg_w = 0, bg_h = 0;
  winwidget winwid;
  Imlib_Image *bg_im = NULL;
  int tot_thumb_h;
  int text_area_h = 50;
  int title_area_h = 0;
  Imlib_Font fn = NULL;
  Imlib_Font title_fn = NULL;
  int text_area_w = 0;
  int tw = 0, th = 0;
  int fw, fh;
  int vertical = 0;
  int max_column_w = 0;
  int thumbnailcount = 0;
  feh_file *file = NULL;
  int file_num = 0;

  file_num = filelist_length (filelist);

  D (("In init_index_mode\n"));

  /* This includes the text area for index data */
  tot_thumb_h = opt.thumb_h + text_area_h;
  if (opt.title_font)
    title_area_h = 50;

  if (opt.font)
    {
      fn = imlib_load_font (opt.font);
      if (!fn)
	fn = imlib_load_font ("20thcent/6");
    }
  else
    fn = imlib_load_font ("20thcent/6");

  if (opt.title_font)
    {
      title_fn = imlib_load_font (opt.title_font);
      if (!fn)
	title_fn = imlib_load_font ("20thcent/24");
    }
  else
    title_fn = imlib_load_font ("20thcent/24");

  if ((!fn) || (!title_fn))
    eprintf ("Error loading fonts");
  imlib_context_set_font (fn);
  imlib_context_set_direction (IMLIB_TEXT_TO_RIGHT);
  imlib_context_set_color (255, 255, 255, 255);

  /* Work out how high the font is */
  imlib_get_text_size ("W", &tw, &th);
  /* For now, allow room for 3 lines with small gaps */
  text_area_h = ((th + 2) * 3) + 5;

  /* Use bg image dimensions for default size */
  if (opt.bg && opt.bg_file)
    {
      D (("Time to apply a background to blend onto\n"));
      if (feh_load_image_char (&bg_im, opt.bg_file) != 0)
	{
	  imlib_context_set_image (bg_im);
	  bg_w = imlib_image_get_width ();
	  bg_h = imlib_image_get_height ();
	}
    }

  if (!opt.limit_w && !opt.limit_h)
    {
      if (opt.bg && opt.bg_file)
	{
	  if (opt.verbose)
	    fprintf (stdout,
		     PACKAGE
		     " - No size restriction specified for index.\n"
		     " You did specify a background however, so the\n"
		     " index size has defaulted to the size of the image\n");
	  opt.limit_w = bg_w;
	  opt.limit_h = bg_h;
	}
      else
	{
	  if (opt.verbose)
	    fprintf (stdout,
		     PACKAGE
		     " - No size restriction specified for index.\n"
		     " Using defaults (width limited to 800)\n");
	  opt.limit_w = 800;
	}
    }


  /* Here we need to whiz through the files, and look at the filenames and
   * info in the selected font, work out how much space we need, and
   * calculate the size of the image we will require */

  if (opt.limit_w && opt.limit_h)
    {
      int rec_h = 0;

      w = opt.limit_w;
      h = opt.limit_h;

      /* Work out if this is big enough, and give a warning if not */

      /* Pretend we are limiting width by that specified, loop through,
       * and see it we fit in the height specified. If not, continue the
       * loop, and recommend the final value instead. Carry on and make
       * the index anyway. */

      for (file = filelist; file; file = file->next)
	{
	  text_area_w = opt.thumb_w;
	  imlib_get_text_size (file->name, &fw, &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;
	  imlib_get_text_size (create_index_dimension_string
			       (1000, 1000), &fw, &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;
	  imlib_get_text_size (create_index_size_string (file->filename), &fw,
			       &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;

	  if (text_area_w > opt.thumb_w)
	    text_area_w += 5;

	  if ((x > w - text_area_w))
	    {
	      x = 0;
	      y += tot_thumb_h;
	    }

	  x += text_area_w;
	}
      rec_h = y + tot_thumb_h;

      if (h < rec_h)
	{
	  weprintf
	    ("The image size you specified (%d by %d) is not large\n"
	     "enough to hold all the thumnails you specified (%d). To fit all\n"
	     "the thumnails, either decrease their size, choose a smaller font,\n"
	     "or use a larger image (may I recommend %d by %d?)", opt.limit_w,
	     opt.limit_h, file_num, opt.limit_w, rec_h);
	}
    }
  else if (opt.limit_h)
    {
      vertical = 1;
      h = opt.limit_h;
      /* calc w */
      for (file = filelist; file; file = file->next)
	{
	  text_area_w = opt.thumb_w;
	  /* Calc width of text */
	  imlib_get_text_size (file->name, &fw, &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;
	  imlib_get_text_size (create_index_dimension_string
			       (1000, 1000), &fw, &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;
	  imlib_get_text_size (create_index_size_string (file->filename), &fw,
			       &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;

	  if (text_area_w > opt.thumb_w)
	    text_area_w += 5;

	  if (text_area_w > max_column_w)
	    max_column_w = text_area_w;

	  if ((y > h - tot_thumb_h))
	    {
	      y = 0;
	      x += max_column_w;
	      max_column_w = 0;
	    }

	  y += tot_thumb_h;
	}
      w = x + text_area_w;
      max_column_w = 0;
    }
  else if (opt.limit_w)
    {
      w = opt.limit_w;
      /* calc h */

      for (file = filelist; file; file = file->next)
	{
	  text_area_w = opt.thumb_w;
	  imlib_get_text_size (file->name, &fw, &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;
	  imlib_get_text_size (create_index_dimension_string
			       (1000, 1000), &fw, &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;
	  imlib_get_text_size (create_index_size_string (file->filename), &fw,
			       &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;

	  if (text_area_w > opt.thumb_w)
	    text_area_w += 5;

	  if ((x > w - text_area_w))
	    {
	      x = 0;
	      y += tot_thumb_h;
	    }

	  x += text_area_w;
	}
      h = y + tot_thumb_h;
    }

  x = y = 0;

  im_main = imlib_create_image (w, h + title_area_h);

  if (!im_main)
    eprintf ("Imlib error creating index image, are you low on RAM?");

  imlib_context_set_image (im_main);
  imlib_context_set_blend (0);

  if (bg_im)
    imlib_blend_image_onto_image (bg_im, 0, 0, 0, bg_w, bg_h, 0, 0, w, h);

  for (file = filelist; file; file = file->next)
    {
      D (("   About to load image %s\n", file->filename));
      if (feh_load_image (&im_temp, file) != 0)
	{
	  if (opt.verbose)
		feh_display_status();
	  D (("   Successfully loaded %s\n", file->filename));
	  www = opt.thumb_w;
	  hhh = opt.thumb_h;
	  imlib_context_set_image (im_temp);
	  ww = imlib_image_get_width ();
	  hh = imlib_image_get_height ();
	  thumbnailcount++;
	  if (imlib_image_has_alpha ())
	    imlib_context_set_blend (1);
	  else
	    imlib_context_set_blend (0);

	  if (opt.aspect)
	    {
	      double ratio = 0.0;
	      /* Keep the aspect ratio for the thumbnail */
	      ratio = ((double) ww / hh) / ((double) www / hhh);

	      if (ratio > 1.0)
		hhh = opt.thumb_h / ratio;
	      else if (ratio != 1.0)
		www = opt.thumb_w * ratio;
	    }

	  if ((!opt.stretch) && ((www > ww) || (hhh > hh)))
	    {
	      /* Don't make the image larger unless stretch is specified */
	      www = ww;
	      hhh = hh;
	    }

	  imlib_context_set_image (im_main);

	  if (opt.alpha & opt.alpha_level)
	    {
	      /* TODO */
	      D (("Applying alpha options\n"));
	    }
	  text_area_w = opt.thumb_w;
	  /* Now draw on the info text */
	  imlib_get_text_size (file->name, &fw, &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;
	  imlib_get_text_size (create_index_dimension_string
			       (ww, hh), &fw, &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;
	  imlib_get_text_size (create_index_size_string (file->filename), &fw,
			       &fh);
	  if (fw > text_area_w)
	    text_area_w = fw;

	  if (text_area_w > opt.thumb_w)
	    text_area_w += 5;

	  if (vertical)
	    {
	      if (text_area_w > max_column_w)
		max_column_w = text_area_w;
	      if (y > h - tot_thumb_h)
		{
		  y = 0;
		  x += max_column_w;
		  max_column_w = 0;
		}
	      if (x > w - text_area_w)
		break;
	    }
	  else
	    {
	      if (x > w - text_area_w)
		{
		  x = 0;
		  y += tot_thumb_h;
		}
	      if (y > h - tot_thumb_h)
		break;
	    }

	  if (opt.aspect)
	    {
	      xxx = x + ((opt.thumb_w - www) / 2);
	      yyy = y + ((opt.thumb_h - hhh) / 2);
	    }
	  else
	    {
	      /* Ignore the aspect ratio and squash the image in */
	      xxx = x;
	      yyy = y;
	    }

	  /* Draw now */
	  imlib_blend_image_onto_image (im_temp, 0, 0, 0, ww, hh, xxx, yyy,
					www, hhh);

	  imlib_context_set_image (im_temp);
	  imlib_free_image_and_decache ();
	  imlib_context_set_image (im_main);

	  imlib_text_draw (x, y + opt.thumb_h + 2, file->name);
	  imlib_text_draw (x,
			   y + opt.thumb_h + (th + 2) +
			   2, create_index_dimension_string (ww, hh));
	  imlib_text_draw (x,
			   y + opt.thumb_h + 2 * (th +
						  2) +
			   2, create_index_size_string (file->filename));

	  if (vertical)
	    y += tot_thumb_h;
	  else
	    x += text_area_w;
	}
    }
  if (opt.verbose)
    fprintf (stdout, "\n");

  if (opt.title_font)
    {
      /* Put some other text on there... */
      imlib_context_set_font (title_fn);
      imlib_context_set_image (im_main);
      imlib_text_draw (20, h + 10,
		       create_index_title_string (thumbnailcount, w, h));
    }

  if (opt.output && opt.output_file)
    {
      imlib_context_set_image (im_main);
      imlib_save_image (opt.output_file);
      if (opt.verbose)
	{
	  int tw, th;
	  tw = imlib_image_get_width ();
	  th = imlib_image_get_height ();
	  fprintf (stdout, PACKAGE " - File saved as %s\n", opt.output_file);
	  fprintf (stdout,
		   "    - Image is %dx%d pixels and contains %d thumbnails\n",
		   tw, th, thumbnailcount);
	}
    }

  if (opt.display)
    {
      winwid = winwidget_create_from_image (im_main, PACKAGE " [index mode]");
      winwidget_show (winwid);
    }
  else
    {
      imlib_context_set_image (im_main);
      imlib_free_image_and_decache ();
    }
}


char *
chop_file_from_full_path (char *str)
{
  return (strrchr (str, '/') + 1);
}

static char *
create_index_size_string (char *file)
{
  static char str[50];
  int size = 0;
  double kbs = 0.0;
  struct stat st;
  if (stat (file, &st))
    kbs = 0.0;
  else
    {
      size = st.st_size;
      kbs = (double) size / 1000;
    }

  snprintf (str, sizeof (str), "%.2fKb", kbs);
  return str;
}

static char *
create_index_dimension_string (int w, int h)
{
  static char str[50];
  snprintf (str, sizeof (str), "%dx%d", w, h);
  return str;
}

static char *
create_index_title_string (int num, int w, int h)
{
  static char str[50];
  snprintf (str, sizeof (str),
	    PACKAGE " index - %d thumbnails, %d by %d pixels", num, w, h);
  return str;
}
