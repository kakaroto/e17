/* imlib.c
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
init_x_and_imlib (void)
{
  D (("In init_x_and_imlib\n"));
  disp = XOpenDisplay (NULL);
  if (!disp)
    {
      fprintf (stderr, "Cannot open display\n");
      exit (1);
    }
  vis = DefaultVisual (disp, DefaultScreen (disp));
  depth = DefaultDepth (disp, DefaultScreen (disp));
  cm = DefaultColormap (disp, DefaultScreen (disp));
  imlib_context_set_display (disp);
  imlib_context_set_visual (vis);
  imlib_context_set_colormap (cm);
  imlib_context_set_color_modifier (NULL);
  imlib_context_set_operation (IMLIB_OP_COPY);
}

int
feh_load_image (Imlib_Image ** im, char *filename)
{
  Imlib_Load_Error err;
  unsigned char must_exit = 0;

  D (("In feh_load_image: filename %s\n", filename));

  if (!filename)
    return 0;

  *im = imlib_load_image_with_error_return (filename, &err);

  if ((err) || (!im))
    {
      /* Check error code */
      fprintf (stderr,
	       PACKAGE " - error while loading image: %s\n      ", filename);
      switch (err)
	{
	case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
	  fprintf (stderr, "File does not exist\n");
	  break;
	case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
	  fprintf (stderr, "Directory specified for image filename\n");
	  break;
	case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
	  fprintf (stderr, "You do not have read access to that directory\n");
	  break;
	case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
	  fprintf (stderr,
		   "Imlib2 cannot find a loader for that file format\n");
	  break;
	case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
	  fprintf (stderr, "Path specified is too long\n");
	  break;
	case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
	  fprintf (stderr, "Path component does not exist\n");
	  break;
	case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
	  fprintf (stderr, "Path component is not a directory\n");
	  break;
	case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
	  /* wtf? :) */
	  fprintf (stderr, "Path points outside address space\n");
	  break;
	case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
	  fprintf (stderr, "Too many levels of symbolic links\n");
	  break;
	case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
	  fprintf (stderr, "Out of memory\n");
	  must_exit = 1;
	  break;
	case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
	  fprintf (stderr, "Out of file descriptors\n");
	  must_exit = 1;
	  break;
	case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
	  fprintf (stderr, "Cannot write to directory\n");
	  break;
	case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
	  fprintf (stderr, "Cannot write - out of disk space\n");
	  break;
	case IMLIB_LOAD_ERROR_UNKNOWN:
	default:
	  fprintf (stderr, "Unknown error. Attempting to continue\n");
	  break;
	}
      if (must_exit)
	exit (1);
      return 0;
    }
  D (("Loaded ok\n"));
  return 1;
}

/* Yeah, ok. I don't use this yet, 'cos it screws with the way I create 
 * my windows atm */
void
progress (Imlib_Image im, char percent, int update_x, int update_y,
	  int update_w, int update_h)
{
  D (("In progressive loading callback\n"));
  if (!progwin)
    return;

  imlib_context_set_drawable (progwin->bg_pmap);
  imlib_context_set_anti_alias (0);
  imlib_context_set_dither (0);
  imlib_context_set_blend (0);
  /* first time it's called */
  if (progwin->im_w == 0)
    {
      imlib_context_set_image (im);
      progwin->w = progwin->im_w = imlib_image_get_width ();
      progwin->h = progwin->im_h = imlib_image_get_height ();
      if (progwin->bg_pmap)
	XFreePixmap (disp, progwin->bg_pmap);
      progwin->bg_pmap =
	XCreatePixmap (disp, progwin->win, progwin->im_w, progwin->im_h,
		       depth);
      winwidget_create_blank_bg (progwin);
      imlib_context_set_drawable (progwin->bg_pmap);

      imlib_context_set_image (progwin->blank_im);
      imlib_render_image_on_drawable (0, 0);
      XSetWindowBackgroundPixmap (disp, progwin->win, progwin->bg_pmap);
      XResizeWindow (disp, progwin->win, progwin->im_w, progwin->im_h);
      XMapWindow (disp, progwin->win);
      XSync (disp, False);
    }
  imlib_context_set_anti_alias (0);
  imlib_context_set_dither (0);
  imlib_context_set_blend (1);
  imlib_blend_image_onto_image (im, 0,
				update_x, update_y,
				update_w, update_h,
				update_x, update_y, update_w, update_h);
  imlib_context_set_blend (0);
  imlib_render_image_part_on_drawable_at_size (update_x, update_y,
					       update_w, update_h,
					       update_x, update_y,
					       update_w, update_h);
  XSetWindowBackgroundPixmap (disp, progwin->win, progwin->bg_pmap);
  XClearArea (disp, progwin->win, update_x, update_y, update_w, update_h,
	      False);
  XFlush (disp);
}
