/* modify.h
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

#include "X11/extensions/xf86vmode.h"

void feh_set_background (winwidget winwid, int weeble);
void feh_blur (winwidget winwid, int radius);
void feh_sharpen (winwidget winwid, int radius);
void feh_tile_seamless_v (winwidget winwid);
void feh_tile_seamless_h (winwidget winwid);
void feh_tile_seamless (winwidget winwid);
void feh_rotate_clockwise (winwidget winwid);
void feh_flip_h (winwidget winwid);
void feh_flip_v (winwidget winwid);
void feh_modify_brightness (winwidget winwid, double value);
void feh_modify_brightness_to_rectangle (winwidget winwid, double value,
					 int x0, int y0, int width,
					 int height);
void feh_modify_gamma (winwidget winwid, double value);
void feh_modify_gamma_to_rectangle (winwidget winwid, double value, int x0,
				    int y0, int width, int height);
void feh_modify_contrast (winwidget winwid, double value);
void feh_modify_contrast_to_rectangle (winwidget winwid, double value, int x0,
				       int y0, int width, int height);
void feh_image_resize_to (winwidget winwid, int new_x, int new_y);
void feh_scale_minus_10per (winwidget winwid);
void feh_scale_plus_10per (winwidget winwid);
void feh_scale_max_y (winwidget winwid);
void feh_scale_max_x (winwidget winwid);
void feh_scale_max_pect (winwidget winwid);
void feh_scale_half (winwidget winwid);
void feh_scale_double (winwidget winwid);
void set_pixmap_property (Pixmap p, Window woot);
