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
void
init_index_mode (void)
{
  Imlib_Image *im_main;
  Imlib_Image *im_temp;
  int w = 800, h = 600, i, ww = 0, hh = 0, www, hhh, xxx, yyy;
  int x = 0, y = 0;
  int bg_w = 0, bg_h = 0;
  winwidget winwid;
  Imlib_Image *bg_im = NULL;
  int tot_thumb_h;
  int text_area_h = 50;
  int title_area_h = 0;
  Imlib_Font fn = NULL;
  Imlib_Font title_fn = NULL;
  int im_per_col = 0;
  int im_per_row = 0;

  D (("In init_index_mode\n"));

  /* This includes the text area for index data */
  tot_thumb_h = opt.thumb_h + text_area_h;
  if (opt.title_font)
    title_area_h = 50;

  /* Time to set up the font stuff */
  imlib_add_path_to_font_path (".");
  imlib_add_path_to_font_path (PREFIX"/share/feh/fonts/");
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

  imlib_context_set_font (fn);
  imlib_context_set_direction (IMLIB_TEXT_TO_RIGHT);
  imlib_context_set_color (255, 255, 255, 255);
  if (!fn)
    fprintf (stderr, "Error loading font\n");

  /* Use bg image dimensions for default size */
  if (opt.bg && opt.bg_file)
    {
      D (("Time to apply a background to blend onto\n"));
      if (feh_load_image (&bg_im, opt.bg_file) != 0)
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

  if (opt.limit_w && opt.limit_h)
    {
      w = opt.limit_w;
      h = opt.limit_h;
      im_per_col = h / tot_thumb_h;
      im_per_row = w / opt.thumb_w;
      D (("   Limiting width to %d and height to %d\n", w, h));
      D (
	 ("   The image will be %d thumbnails wide by %d high\n", im_per_row,
	  im_per_col));
      D (
	 ("   You asked for %d thumbnails, this image is big enough for %d\n",
	  file_num, im_per_row * im_per_col));
      if (file_num > (im_per_row * im_per_col))
	{
	  int rec_w = 0;
	  int rec_h = 0;
	  int rec_im_per_col = 0;
	  int rec_im_per_row = 0;

	  if (w > h)
	    {
	      rec_h = 2 * h;
	      rec_im_per_col = rec_h / tot_thumb_h;
	      rec_im_per_row = file_num / rec_im_per_col;
	      rec_w = rec_im_per_row * opt.thumb_w;
	      if (file_num % rec_im_per_col)
		rec_w += opt.thumb_w;
	    }
	  else
	    {
	      rec_w = 2 * w;
	      rec_im_per_row = rec_w / opt.thumb_w;
	      rec_im_per_col = file_num / rec_im_per_row;
	      rec_h = rec_im_per_col * tot_thumb_h;
	      if (file_num % rec_im_per_row)
		rec_h += tot_thumb_h;
	    }
	  fprintf (stderr,
		   PACKAGE
		   " - WARNING! The image size you requested (%d by %d) is"
		   " NOT big enough to fit the number of thumbnails specified"
		   " (%d).\nNot all images will be shown (only %d). May I recommend a"
		   " size of %d by %d?\n",
		   w, h, file_num, im_per_row * im_per_col, rec_w, rec_h);
	}
    }
  else if (opt.limit_h)
    {
      h = opt.limit_h;

      im_per_col = h / tot_thumb_h;
      im_per_row = file_num / im_per_col;
      w = im_per_row * opt.thumb_w;
      if (file_num % im_per_col)
	w += opt.thumb_w;
      if ((bg_im) && (w < bg_w))
	w = bg_w;
      D (("   Width will be %d - Height limited to %d\n", w, h));
      D (
	 ("   The image will be %d thumbnails wide by %d high\n", im_per_row,
	  im_per_col));
    }
  else if (opt.limit_w)
    {
      w = opt.limit_w;

      im_per_row = w / opt.thumb_w;
      im_per_col = file_num / im_per_row;
      h = im_per_col * tot_thumb_h;
      if (file_num % im_per_row)
	h += tot_thumb_h;
      if ((bg_im) && (h < bg_h))
	h = bg_h;
      D (("   Width limited to %d - Height will be %d\n", w, h));
      D (
	 ("   The image will be %d thumbnails wide by %d high\n", im_per_row,
	  im_per_col));
    }

  im_main = imlib_create_image (w, h + title_area_h);

  if (!im_main)
    {
      fprintf (stderr, "Imlib error creating image\n");
      exit (2);
    }

  imlib_context_set_image (im_main);
  imlib_context_set_blend (0);

  if (bg_im)
    imlib_blend_image_onto_image (bg_im, 0, 0, 0, bg_w, bg_h, 0, 0, w, h);
  imlib_context_set_blend (1);

  for (i = 0; i < file_num; i++)
    {
      D (("   About to load image %s\n", files[i]));
      if (opt.verbose)
	{
	  if (i)
	    {
	      if (!(i % 50))
		fprintf (stdout, "\n ");
	      else if (!(i % 10))
		fprintf (stdout, " ");
	    }
	  else
	    fprintf (stdout, " ");

	  fprintf (stdout, ".");
	  fflush (stdout);
	}
      if (feh_load_image (&im_temp, files[i]) != 0)
	{
	  D (("   Successfully loaded %s\n", files[i]));
	  www = opt.thumb_w;
	  hhh = opt.thumb_h;
	  imlib_context_set_image (im_temp);
	  ww = imlib_image_get_width ();
	  hh = imlib_image_get_height ();

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

	  imlib_context_set_image (im_main);

	  if (opt.alpha & opt.alpha_level)
	    {
	      /* TODO */
	      D (("Applying alpha options\n"));
	    }
	  imlib_blend_image_onto_image (im_temp, 0, 0, 0, ww, hh, xxx, yyy,
					www, hhh);
	  /* Now draw on the info text */
	  D (("Drawing test at %d,%d\n", x, y + opt.thumb_h + 2));
	  imlib_text_draw (x, y + opt.thumb_h + 10,
			   chop_file_from_full_path (files[i]));
	  imlib_text_draw (x, y + opt.thumb_h + 20,
			   create_index_dimension_string (ww, hh));
	  imlib_text_draw (x, y + opt.thumb_h + 30,
			   create_index_size_string (files[i]));
	  imlib_context_set_image (im_temp);
	  imlib_free_image_and_decache ();
	  x += opt.thumb_w;
	  if (x > w - opt.thumb_w)
	    {
	      x = 0;
	      y += tot_thumb_h;
	    }
	  if (y > h - tot_thumb_h)
	    break;
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
		       create_index_title_string (im_per_row * im_per_col, w,
						  h));
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
		   tw, th, (tw / opt.thumb_w) * (th / tot_thumb_h));
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
