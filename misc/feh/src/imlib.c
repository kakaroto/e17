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
  int onoff, x, y;

  D (("In init_x_and_imlib\n"));

  disp = XOpenDisplay (NULL);
  if (!disp)
    eprintf ("Cannot open display");
  vis = DefaultVisual (disp, DefaultScreen (disp));
  depth = DefaultDepth (disp, DefaultScreen (disp));
  cm = DefaultColormap (disp, DefaultScreen (disp));
  root = RootWindow (disp, DefaultScreen (disp));
  scr = ScreenOfDisplay (disp, DefaultScreen (disp));
  xid_context = XUniqueContext ();

  imlib_context_set_display (disp);
  imlib_context_set_visual (vis);
  imlib_context_set_colormap (cm);
  imlib_context_set_color_modifier (NULL);
  imlib_context_set_operation (IMLIB_OP_COPY);
  wmDeleteWindow = XInternAtom (disp, "WM_DELETE_WINDOW", False);
  imlib_context_set_dither (0);
  imlib_context_set_blend (0);

  /* Set up the font stuff */
  imlib_add_path_to_font_path (".");
  imlib_add_path_to_font_path (PREFIX "/share/feh/fonts");
  imlib_add_path_to_font_path ("./ttfonts");

  checks = imlib_create_image (CHECK_SIZE, CHECK_SIZE);

  if (!checks)
    eprintf ("Unable to create a teeny weeny imlib image. I detect problems");

  imlib_context_set_image (checks);
  for (y = 0; y < CHECK_SIZE; y += 8)
    {
      onoff = (y / 8) & 0x1;
      for (x = 0; x < CHECK_SIZE; x += 8)
	{
	  if (onoff)
	    imlib_context_set_color (144, 144, 144, 255);
	  else
	    imlib_context_set_color (100, 100, 100, 255);
	  imlib_image_fill_rectangle (x, y, 8, 8);
	  onoff++;
	  if (onoff == 2)
	    onoff = 0;
	}
    }
}

int
feh_load_image_char (Imlib_Image ** im, char *filename)
{
  feh_file file;
  int i;

  file = filelist_newitem (filename);
  i = feh_load_image (im, file);
  feh_file_free (file);
  return i;
}

int
feh_load_image (Imlib_Image ** im, feh_file file)
{
  Imlib_Load_Error err;

  D (("In feh_load_image: filename %s\n", file->filename));

  if (!file || !file->filename)
    return 0;

  /* Handle URLs */
  if ((!strncmp (file->filename, "http://", 7))
      || (!strncmp (file->filename, "ftp://", 6)))
    {
      char *tmpname = NULL;

      tmpname = feh_http_load_image (file->filename);
      if (tmpname == NULL)
	return 0;
      *im = imlib_load_image_with_error_return (tmpname, &err);
      if ((opt.slideshow) && (opt.reload == 0))
	{
	  /* Http, no reload, slideshow. Let's keep this image on hand... */
	  free (file->filename);
	  file->filename = estrdup (tmpname);
	}
      else
	{
	  /* Don't cache the image if we're doing reload + http (webcams
	   * etc) */
	  if (!opt.keep_http)
	    unlink (tmpname);
	}
      add_file_to_rm_filelist (tmpname);
      free (tmpname);
    }
  else
    {
      *im = imlib_load_image_with_error_return (file->filename, &err);
    }

  if ((err) || (!im))
    {
      /* Check error code */
      switch (err)
	{
	case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
	  weprintf ("%s - File does not exist", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
	  weprintf ("%s - Directory specified for image filename",
		    file->filename);
	  break;
	case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
	  weprintf
	    ("%s - You don't have read access to that directory",
	     file->filename);
	  break;
	case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
	  weprintf
	    ("%s - You don't have an Imlib2 loader for that file format",
	     file->filename);
	  break;
	case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
	  weprintf ("%s - Path specified is too long", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
	  weprintf ("%s - Path component does not exist", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
	  weprintf ("%s - Path component is not a directory", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
	  /* wtf? :) */
	  weprintf ("%s - Path points outside address space", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
	  weprintf ("%s - Too many levels of symbolic links", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
	  eprintf ("While loading %s - Out of memory", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
	  eprintf ("While loading %s - Out of file descriptors",
		   file->filename);
	  break;
	case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
	  weprintf ("%s - Cannot write to directory", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
	  weprintf ("%s - Cannot write - out of disk space", file->filename);
	  break;
	case IMLIB_LOAD_ERROR_UNKNOWN:
	default:
	  weprintf
	    ("While loading %s - Unknown error. Attempting to continue",
	     file->filename);
	  break;
	}
      return 0;
    }
  D (("Loaded ok\n"));
  return 1;
}

void
progress (Imlib_Image im, char percent, int update_x, int update_y,
	  int update_w, int update_h)
{
  int exists = 0;
  int dest_x = 0;
  int dest_y = 0;
  D (("In progressive loading callback\n"));
  if (!progwin)
    {
      weprintf ("progwin does not exist");
      return;
    }

  imlib_context_set_drawable (progwin->bg_pmap);
  imlib_context_set_anti_alias (0);
  imlib_context_set_dither (0);
  imlib_context_set_image (im);

  /* need to create a window for the image */
  if (progwin->im_w == 0)
    {
      progwin->w = progwin->im_w = imlib_image_get_width ();
      progwin->h = progwin->im_h = imlib_image_get_height ();
      if (!progwin->win)
	winwidget_create_window (progwin, progwin->w, progwin->h);
      else
	exists = 1;
      if (progwin->bg_pmap)
	XFreePixmap (disp, progwin->bg_pmap);
      if (opt.full_screen)
	{
	  if (progwin->gc == None)
	    {
	      XGCValues gcval;

	      gcval.foreground = BlackPixel (disp, DefaultScreen (disp));
	      progwin->gc =
		XCreateGC (disp, progwin->win, GCForeground, &gcval);
	    }
	  progwin->bg_pmap =
	    XCreatePixmap (disp, progwin->win, scr->width, scr->height,
			   depth);
	  XFillRectangle (disp, progwin->bg_pmap, progwin->gc, 0, 0,
			  scr->width, scr->height);
	}
      else
	{
	  progwin->bg_pmap =
	    XCreatePixmap (disp, progwin->win, progwin->im_w, progwin->im_h,
			   depth);
	  imlib_context_set_drawable (progwin->bg_pmap);
	  feh_draw_checks (progwin);
	}
      XSetWindowBackgroundPixmap (disp, progwin->win, progwin->bg_pmap);
      if (exists && !opt.full_screen)
	XResizeWindow (disp, progwin->win, progwin->w, progwin->h);
      XClearWindow (disp, progwin->win);
      if (!exists)
	XMapWindow (disp, progwin->win);
      XSync (disp, False);
    }
  imlib_context_set_image (im);
  imlib_context_set_drawable (progwin->bg_pmap);
  imlib_context_set_anti_alias (0);
  imlib_context_set_dither (0);
  if (imlib_image_has_alpha ())
    imlib_context_set_blend (1);
  else
    imlib_context_set_blend (0);

  if (opt.full_screen)
    {
      dest_x = (scr->width - progwin->im_w) >> 1;
      dest_y = (scr->height - progwin->im_h) >> 1;
    }

  imlib_render_image_part_on_drawable_at_size (update_x, update_y,
					       update_w, update_h,
					       dest_x + update_x,
					       dest_y + update_y, update_w,
					       update_h);
  XSetWindowBackgroundPixmap (disp, progwin->win, progwin->bg_pmap);
  XClearArea (disp, progwin->win, dest_x + update_x, dest_y + update_y,
	      update_w, update_h, False);
  XFlush (disp);
  return;
  percent = 0;
}

char *
feh_http_load_image (char *url)
{
  int pid;
  int status;
  char *tmp;
  char *tmpname;
  char num[10];
  static long int i = 1;

  snprintf (num, sizeof (num), "%04ld_", i++);
  /* Massive paranoia ;) */
  if (i > 9998)
    i = 1;

  tmp = tempnam ("/tmp", num);
  if (tmp == NULL)
    eprintf ("Error creating unique filename:");

  /* Modify tempname to make it a little more useful... */
  tmpname = estrjoin ("", tmp, "_feh_", strrchr (url, '/') + 1, NULL);
  free (tmp);

  if ((pid = fork ()) < 0)
    {
      weprintf ("open url: fork failed:");
      free (tmpname);
      return NULL;
    }
  else if (pid == 0)
    {
      execlp ("wget", "wget", url, "-O", tmpname, NULL);
      eprintf ("url: exec failed: wget:");
    }
  else
    {
      waitpid (pid, &status, 0);

      if (!WIFEXITED (status) || WEXITSTATUS (status) != 0)
	{
	  weprintf ("url: wget failed to load URL %s\n", url);
	  free (tmpname);
	  return NULL;
	}
    }
  return tmpname;
}

void
feh_draw_filename (winwidget w)
{
  static Imlib_Font fn = NULL;
  int tw = 0, th = 0;
  Imlib_Image *im = NULL;

  if (!fn)
    fn = imlib_load_font ("20thcent/16");

  if (!fn)
    {
      weprintf ("Couldn't load font for filename printing");
      return;
    }

  imlib_context_set_font (fn);
  imlib_context_set_direction (IMLIB_TEXT_TO_RIGHT);
  imlib_context_set_color (0, 0, 0, 255);
  imlib_context_set_blend(1);

  /* Work out how high the font is */
  imlib_get_text_size (w->file->filename, &tw, &th);
  printf("Printing %s\n", w->file->filename);

  im = imlib_create_image (tw, th);
  if (!im)
    eprintf ("Couldn't create image. Out of memory?");

  imlib_context_set_image (im);
  imlib_image_fill_rectangle(0,0,tw, th);
  imlib_context_set_color (255, 255, 255, 255);

  imlib_text_draw (0, 0, w->file->filename);

  imlib_context_set_drawable (w->win);
  imlib_render_image_on_drawable (0, 0);

  imlib_free_image_and_decache ();
  imlib_context_set_image(w->im);
}
