/* montage.c
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

/* TODO Break this up a bit ;) */
void
init_montage_mode (void)
{
  Imlib_Image *im_main;
  Imlib_Image *im_temp;
  int ww, hh, www, hhh, xxx, yyy;
  int w = 800, h = 600;
  int x = 0, y = 0;
  int bg_w = 0, bg_h = 0;
  winwidget winwid;
  Imlib_Image *bg_im = NULL;
  feh_file *file;
  int file_num = 0;

  file_num = filelist_length (filelist);

  D (("In init_montage_mode\n"));

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
		     " - No size restriction specified for montage.\n"
		     " You did specify a background however, so the\n"
		     " montage size has defaulted to the size of the image\n");
	  opt.limit_w = bg_w;
	  opt.limit_h = bg_h;
	}
      else
	{
	  if (opt.verbose)
	    fprintf (stdout,
		     PACKAGE
		     " - No size restriction specified for montage.\n"
		     " Using defaults (width limited to 800)\n");
	  opt.limit_w = 800;
	}
    }

  if (opt.limit_w && opt.limit_h)
    {
      int im_per_col = 0;
      int im_per_row = 0;
      w = opt.limit_w;
      h = opt.limit_h;
      im_per_col = h / opt.thumb_h;
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
	      rec_im_per_col = rec_h / opt.thumb_h;
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
	      rec_h = rec_im_per_col * opt.thumb_h;
	      if (file_num % rec_im_per_row)
		rec_h += opt.thumb_h;
	    }
	  weprintf ("The image size you requested (%d by %d) is"
		    " NOT big\n      enough to fit the number of thumbnails specified"
		    " (%d).\nNot all images will be shown (only %d). May I recommend a"
		    " size of %d by %d?",
		    w, h, file_num, im_per_row * im_per_col, rec_w, rec_h);
	}
    }
  else if (opt.limit_h)
    {
      int im_per_col = 0;
      int im_per_row = 0;
      h = opt.limit_h;

      im_per_col = h / opt.thumb_h;
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
      int im_per_row = 0;
      int im_per_col = 0;
      w = opt.limit_w;

      im_per_row = w / opt.thumb_w;
      im_per_col = file_num / im_per_row;
      h = im_per_col * opt.thumb_h;
      if (file_num % im_per_row)
	h += opt.thumb_h;
      if ((bg_im) && (h < bg_h))
	h = bg_h;
      D (("   Width limited to %d - Height will be %d\n", w, h));
      D (
	 ("   The image will be %d thumbnails wide by %d high\n", im_per_row,
	  im_per_col));
    }

  im_main = imlib_create_image (w, h);

  if (!im_main)
    eprintf ("Imlib error creating image");

  imlib_context_set_image (im_main);
  imlib_context_set_blend (0);

  if (bg_im)
    imlib_blend_image_onto_image (bg_im, 0, 0, 0, bg_w, bg_h, 0, 0, w, h);

  for (file = filelist; file; file = file->next)
    {
      D (("   About to load image %s\n", file->filename));
      if (feh_load_image (&im_temp, file) != 0)
	{
	  D (("   Successfully loaded %s\n", file->filename));
	  if (opt.verbose)
	    feh_display_status ('.');
	  www = opt.thumb_w;
	  hhh = opt.thumb_h;
	  imlib_context_set_image (im_temp);
	  ww = imlib_image_get_width ();
	  hh = imlib_image_get_height ();
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
	  imlib_context_set_image (im_temp);
	  imlib_free_image_and_decache ();
	  x += opt.thumb_w;
	  if (x > w - opt.thumb_w)
	    {
	      x = 0;
	      y += opt.thumb_h;
	    }
	  if (y > h - opt.thumb_h)
	    break;
	}
      else if (opt.verbose)
	feh_display_status ('x');

    }
  if (opt.verbose)
    fprintf (stdout, "\n");

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
		   tw, th, (tw / opt.thumb_w) * (th / opt.thumb_h));
	}
    }

  if (opt.display)
    {
      winwid =
	winwidget_create_from_image (im_main, PACKAGE " [monage mode]");
      winwidget_show (winwid);
    }
  else
    {
      imlib_context_set_image (im_main);
      imlib_free_image_and_decache ();
    }
}
