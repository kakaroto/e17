/* modify.c
 *
 * Copyright (C) 1999 Tom Gilbert Andrew Glover
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
#include "modify.h"

void
set_pixmap_property (Pixmap p, Window root)
{

  Atom prop_root, prop_esetroot, type;
  int format;
  unsigned long length, after;
  unsigned char *data_root, *data_esetroot;

  prop_root = XInternAtom (disp, "_XROOTPMAP_ID", True);
  prop_esetroot = XInternAtom (disp, "ESETROOT_PMAP_ID", True);

  if (prop_root != None && prop_esetroot != None)
    {
      XGetWindowProperty (disp, root, prop_root, 0L, 1L, False,
			  AnyPropertyType, &type, &format, &length, &after,
			  &data_root);
      if (type == XA_PIXMAP)
	{
	  XGetWindowProperty (disp, root, prop_esetroot, 0L, 1L, False,
			      AnyPropertyType, &type, &format, &length,
			      &after, &data_esetroot);
	  if (data_root && data_esetroot)
	    {
	      if (type == XA_PIXMAP
		  && *((Pixmap *) data_root) == *((Pixmap *) data_esetroot))
		{
		  XKillClient (disp, *((Pixmap *) data_root));
		}
	    }
	}
    }
  /* This will locate the property, creating it if it doesn't exist */
  prop_root = XInternAtom (disp, "_XROOTPMAP_ID", False);
  prop_esetroot = XInternAtom (disp, "ESETROOT_PMAP_ID", False);

  /* The call above should have created it.  If that failed, we can't continue. */
  if (prop_root == None || prop_esetroot == None)
    {
      weprintf ("creation of pixmap property failed");
      exit (1);
    }
  XChangeProperty (disp, root, prop_root, XA_PIXMAP, 32, PropModeReplace,
		   (unsigned char *) &p, 1);
  XChangeProperty (disp, root, prop_esetroot, XA_PIXMAP, 32, PropModeReplace,
		   (unsigned char *) &p, 1);
  XSetCloseDownMode (disp, RetainPermanent);
  XFlush (disp);
}

void
feh_set_background (winwidget winwid, int weeble)
{
  if (0)
    {
      /* Enlightenment method */
    }
  else
    {
      Pixmap proot;
      Screen *scr;
      int x, y;

      scr = ScreenOfDisplay (disp, DefaultScreen (disp));
      imlib_context_set_image (winwid->im);

      if (weeble == SCALE)
	{
	  Imlib_Image new_im = NULL;

	  new_im = imlib_create_cropped_scaled_image (0, 0,
						      winwid->im_w,
						      winwid->im_h,
						      scr->width,
						      scr->height);
	  imlib_context_set_image (new_im);
	  x = imlib_image_get_width ();
	  y = imlib_image_get_height ();

	  proot = XCreatePixmap (disp, root, x, y, depth);
	  imlib_context_set_drawable (proot);
	  XResizeWindow (disp, root, x, y);
	  imlib_render_image_on_drawable (0, 0);
	  imlib_free_image_and_decache ();
	}
      else if (weeble == CENTER)
	{
	  Imlib_Image new_im = NULL;

	  new_im = imlib_create_cropped_scaled_image (0, 0,
						      winwid->im_w,
						      winwid->im_h,
						      winwid->im_w,
						      winwid->im_h);
	  imlib_context_set_image (new_im);
	  x = imlib_image_get_width ();
	  y = imlib_image_get_height ();

	  proot = XCreatePixmap (disp, root, scr->width, scr->height, depth);
	  imlib_context_set_drawable (proot);
	  XResizeWindow (disp, root, scr->width, scr->height);
	  imlib_render_image_part_on_drawable_at_size (0, 0,
						       x, y,
						       (scr->width - x) / 2,
						       (scr->height - y) / 2,
						       x, y);
	  imlib_free_image_and_decache ();
	}
      else if (weeble == FIT)
	{
	  float factor;
	  int new_im_w;
	  Imlib_Image new_im = NULL;

	  factor = (float) scr->height / winwid->im_h;
	  new_im_w = winwid->im_w * factor;

	  new_im = imlib_create_cropped_scaled_image (0, 0,
						      winwid->im_w,
						      winwid->im_h, new_im_w,
						      scr->height);
	  imlib_context_set_image (new_im);
	  x = imlib_image_get_width ();
	  y = imlib_image_get_height ();

	  proot = XCreatePixmap (disp, root, scr->width, scr->height, depth);
	  imlib_context_set_drawable (proot);
	  XResizeWindow (disp, root, scr->width, scr->height);
	  imlib_render_image_part_on_drawable_at_size (0, 0,
						       x, y,
						       (scr->width - x) / 2,
						       (scr->height - y) / 2,
						       x, y);
	  imlib_free_image_and_decache ();
	}
      else			/* tile it */
	{
	  int xcount, ycount;

	  proot = XCreatePixmap (disp, root, scr->width, scr->height, depth);
	  imlib_context_set_drawable (proot);
	  XResizeWindow (disp, root, scr->width, scr->height);

	  for (ycount = 0; ycount < scr->height; ycount += winwid->im_h)
	    {
	      for (xcount = 0; xcount < scr->width; xcount += winwid->im_w)
		{
		  imlib_render_image_part_on_drawable_at_size (0, 0,
							       winwid->im_w,
							       winwid->im_h,
							       xcount, ycount,
							       winwid->im_w,
							       winwid->im_h);
		}
	    }
	}
/*      set_pixmap_property (proot, root); */

      XFlush (disp);
      XSetWindowBackgroundPixmap (disp, root, proot);
      XClearWindow (disp, root);

      XFreePixmap (disp, proot);
      XDestroyWindow (disp, root);
    }
}

void
feh_blur (winwidget winwid, int radius)
{
  imlib_context_set_image (winwid->im);
  imlib_image_blur (radius);
  winwidget_rerender_image (winwid);
}

void
feh_sharpen (winwidget winwid, int radius)
{
  imlib_context_set_image (winwid->im);
  imlib_image_sharpen (radius);
  winwidget_rerender_image (winwid);
}

void
feh_tile_seamless_v (winwidget winwid)
{
  imlib_context_set_image (winwid->im);
  imlib_image_tile_vertical ();
  winwidget_rerender_image (winwid);
}

void
feh_tile_seamless_h (winwidget winwid)
{
  imlib_context_set_image (winwid->im);
  imlib_image_tile_horizontal ();
  winwidget_rerender_image (winwid);
}

void
feh_tile_seamless (winwidget winwid)
{
  imlib_context_set_image (winwid->im);
  imlib_image_tile ();
  winwidget_rerender_image (winwid);
}

void
feh_rotate_clockwise (winwidget winwid)
{
  imlib_context_set_image (winwid->im);
  imlib_image_flip_diagonal ();
  winwid->im_w = imlib_image_get_width ();
  winwid->im_h = imlib_image_get_height ();
  winwidget_rerender_image (winwid);
}

void
feh_flip_h (winwidget winwid)
{
  imlib_context_set_image (winwid->im);
  imlib_image_flip_horizontal ();
  winwidget_rerender_image (winwid);
}

void
feh_flip_v (winwidget winwid)
{
  imlib_context_set_image (winwid->im);
  imlib_image_flip_vertical ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_brightness (winwidget winwid, double value)
{
  Imlib_Color_Modifier color_modifier;
  if (value > 1.0)
    return;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_brightness (value);
  imlib_apply_color_modifier ();
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_brightness_to_rectangle (winwidget winwid, double value, int x,
				    int y, int width, int height)
{
  Imlib_Color_Modifier color_modifier;
  if (value > 1.0)
    return;
  if (width == 0 || height == 0)
    return;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_brightness (value);
  imlib_apply_color_modifier_to_rectangle (x, y, width, height);
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_gamma (winwidget winwid, double value)
{
  Imlib_Color_Modifier color_modifier;
  if (value > 0.9)
    return;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_gamma (value);
  imlib_apply_color_modifier ();
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_gamma_to_rectangle (winwidget winwid, double value, int x, int y,
			       int width, int height)
{
  Imlib_Color_Modifier color_modifier;
  if (value > 0.9)
    return;
  if (width == 0 || height == 0)
    return;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_gamma (value);
  imlib_apply_color_modifier_to_rectangle (x, y, width, height);
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_contrast (winwidget winwid, double value)
{
  Imlib_Color_Modifier color_modifier;
  if (value > 0.9)
    return;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_contrast (value);
  imlib_apply_color_modifier ();
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_contrast_to_rectangle (winwidget winwid, double value, int x,
				  int y, int width, int height)
{
  Imlib_Color_Modifier color_modifier;
  if (value > 0.9)
    return;
  if (width == 0 || height == 0)
    return;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_contrast (value);
  imlib_apply_color_modifier_to_rectangle (x, y, width, height);
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

/*  The main resize function */
void
feh_image_resize_to (winwidget winwid, int new_width, int new_height)
{
  Imlib_Image new_im = NULL;

  imlib_context_set_image (winwid->im);

  new_im = imlib_create_cropped_scaled_image (0, 0,
					      winwid->im_w, winwid->im_h,
					      new_width, new_height);
  imlib_free_image_and_decache ();
  winwid->im = new_im;
  imlib_context_set_image (winwid->im);
  winwid->im_w = imlib_image_get_width ();
  winwid->im_h = imlib_image_get_height ();
  winwidget_rerender_image (winwid);
}

void
feh_scale_minus_10per (winwidget winwid)
{
  feh_image_resize_to (winwid,
		       winwid->im_w - (100 * 0.1),
		       winwid->im_h - (100 * 0.1));
}

void
feh_scale_plus_10per (winwidget winwid)
{
  feh_image_resize_to (winwid, winwid->im_w * 1.1, winwid->im_h * 1.1);
}

void
feh_scale_max_y (winwidget winwid)
{
  Screen *scr;

  scr = ScreenOfDisplay (disp, DefaultScreen (disp));

  feh_image_resize_to (winwid, winwid->im_w, scr->height);
}

void
feh_scale_max_x (winwidget winwid)
{
  Screen *scr;

  scr = ScreenOfDisplay (disp, DefaultScreen (disp));

  feh_image_resize_to (winwid, scr->width, winwid->im_h);
}

void
feh_scale_max_pect (winwidget winwid)
{
  Screen *scr;
  int new_im_w;
  float factor;

  scr = ScreenOfDisplay (disp, DefaultScreen (disp));

  factor = (float) scr->height / winwid->im_h;
  new_im_w = winwid->im_w * factor;

  feh_image_resize_to (winwid, new_im_w, scr->height);
}

void
feh_scale_half (winwidget winwid)
{
  feh_image_resize_to (winwid, winwid->im_w / 2, winwid->im_h / 2);
}

void
feh_scale_double (winwidget winwid)
{
  feh_image_resize_to (winwid, winwid->im_w * 2, winwid->im_h * 2);
}

void
feh_crop_image (winwidget winwid, int x, int y, int width, int height)
{
  Imlib_Image new_im = NULL;
  if (width == 0 || height == 0)
    return;
  imlib_context_set_image (winwid->im);
  new_im = imlib_create_cropped_image (x, y, width, height);
  imlib_free_image_and_decache ();
  winwid->im = new_im;
  imlib_context_set_image (winwid->im);
  winwid->im_w = imlib_image_get_width ();
  winwid->im_h = imlib_image_get_height ();
  winwidget_rerender_image (winwid);
}

void
reset_modify_mode (void)
{
  opt.modify_mode = MODIFY_MODE_NONE;
  rectangles_on = 0;
}

void
toggle_modify_mode (int mode)
{
  if (opt.modify_mode == mode)
    {
      reset_modify_mode ();
    }
  else
    {
      opt.modify_mode = mode;
      rectangles_on = 1;
    }
}
