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
feh_set_background (winwidget winwid)
{
  Window woot;
  int screen;
  screen = DefaultScreen (disp);
  woot = RootWindow (disp, screen);
  XSetWindowBackgroundPixmap (disp, woot, winwid->bg_pmap);
  XClearWindow (disp, woot);
  XFlush (disp);
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
  winwid->im_w=imlib_image_get_width();
  winwid->im_h=imlib_image_get_height();
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
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_brightness (value);
  imlib_apply_color_modifier ();
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_brightness_to_rectangle (winwidget winwid, double value, int x0,
				    int y0, int width, int height)
{
  Imlib_Color_Modifier color_modifier;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_brightness (value);
  imlib_apply_color_modifier_to_rectangle (x0, y0, width, height);
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_gamma (winwidget winwid, double value)
{
  Imlib_Color_Modifier color_modifier;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_gamma (value);
  imlib_apply_color_modifier ();
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_gamma_to_rectangle (winwidget winwid, double value, int x0, int y0,
			       int width, int height)
{
  Imlib_Color_Modifier color_modifier;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_gamma (value);
  imlib_apply_color_modifier_to_rectangle (x0, y0, width, height);
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_contrast (winwidget winwid, double value)
{
  Imlib_Color_Modifier color_modifier;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_contrast (value);
  imlib_apply_color_modifier ();
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

void
feh_modify_contrast_to_rectangle (winwidget winwid, double value, int x0,
				  int y0, int width, int height)
{
  Imlib_Color_Modifier color_modifier;
  imlib_context_set_image (winwid->im);
  color_modifier = imlib_create_color_modifier ();
  imlib_context_set_color_modifier (color_modifier);
  imlib_modify_color_modifier_contrast (value);
  imlib_apply_color_modifier_to_rectangle (x0, y0, width, height);
  imlib_free_color_modifier ();
  winwidget_rerender_image (winwid);
}

/*  The main resize function */
void feh_image_resize_to (winwidget winwid, int new_x, int new_y)
{
  Imlib_Image new_im = NULL;

  imlib_context_set_image(winwid->im);

  new_im = imlib_create_cropped_scaled_image(0, 0,
    winwid->im_w, winwid->im_h,
    new_x, new_y);
  imlib_free_image_and_decache();
  winwid->im=new_im;
  imlib_context_set_image(winwid->im);
  winwid->im_w=imlib_image_get_width();
  winwid->im_h=imlib_image_get_height();
  winwidget_rerender_image(winwid);
}

void
feh_scale_minus_10per (winwidget winwid)
{
  feh_image_resize_to(winwid,
    winwid->im_w-(100*0.1),
    winwid->im_h-(100*0.1));
}   

void
feh_scale_plus_10per (winwidget winwid)
{
  feh_image_resize_to(winwid, winwid->im_w*1.1,
    winwid->im_h*1.1);  
}   

#if 0
void
feh_scale_max_y (winwidget winwid)
{
  XF86VidModeModeInfo **vm_lines;
  int idump;
  
  XF86VidModeGetAllModeLines(disp,XDefaultScreen(disp),
    &idump,&vm_lines);
    
  feh_image_resize_to(winwid, winwid->im_w,
    vm_lines[0]->vdisplay);
}   

void
feh_scale_max_x (winwidget winwid)
{
  XF86VidModeModeInfo **vm_lines;
  int idump;

  XF86VidModeGetAllModeLines(disp,XDefaultScreen(disp),
    &idump,&vm_lines);

  feh_image_resize_to(winwid, vm_lines[0]->hdisplay,
    winwid->im_h);
}

void
feh_scale_max_pect (winwidget winwid)
{
  XF86VidModeModeInfo **vm_lines;
  int idump, new_im_w;
  float factor;
  Imlib_Image new_im = NULL;
  
  XF86VidModeGetAllModeLines(disp,XDefaultScreen(disp),
    &idump,&vm_lines);
    
  factor = (float) vm_lines[0]->vdisplay / winwid->im_h;
  new_im_w = winwid->im_w * factor;
  
  feh_image_resize_to(winwid, new_im_w,
    vm_lines[0]->vdisplay); 
}   
#endif

void
feh_scale_half (winwidget winwid)
{
  feh_image_resize_to(winwid, winwid->im_w/2,
    winwid->im_h/2);
}   

void
feh_scale_double (winwidget winwid)
{
  feh_image_resize_to(winwid, winwid->im_w*2,
    winwid->im_h*2);
}   
