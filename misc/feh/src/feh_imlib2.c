/* feh_imlib2.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "feh_imlib2.h"
#include "utils.h"


int
feh_imlib_image_get_width(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_get_width();
}

int
feh_imlib_image_get_height(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_get_height();
}

int
feh_imlib_image_has_alpha(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_has_alpha();
}

void
feh_imlib_free_image_and_decache(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_free_image_and_decache();
}

void
feh_imlib_free_image(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_free_image();
}

void
feh_imlib_render_image_on_drawable(Drawable d, Imlib_Image im, int x, int y,
                                   char dither, char blend, char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(d);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(0);
   imlib_render_image_on_drawable(x, y);
}

void
feh_imlib_render_image_on_drawable_with_rotation(Drawable d, Imlib_Image im,
                                                 int x, int y, double angle,
                                                 char dither, char blend,
                                                 char alias)
{
   Imlib_Image new_im;

   imlib_context_set_image(im);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(angle);
   imlib_context_set_drawable(d);
   new_im = imlib_create_rotated_image(angle);
   imlib_context_set_image(new_im);
   imlib_render_image_on_drawable(x, y);
   imlib_free_image();
}

void
feh_imlib_render_image_on_drawable_at_size(Drawable d, Imlib_Image im, int x,
                                           int y, int w, int h, char dither,
                                           char blend, char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(d);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(0);
   imlib_render_image_on_drawable_at_size(x, y, w, h);
}

void
feh_imlib_render_image_on_drawable_at_size_with_rotation(Drawable d,
                                                         Imlib_Image im,
                                                         int x, int y, int w,
                                                         int h, double angle,
                                                         char dither,
                                                         char blend,
                                                         char alias)
{
   Imlib_Image new_im;

   imlib_context_set_image(im);
   imlib_context_set_drawable(d);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(angle);
   new_im = imlib_create_rotated_image(angle);
   imlib_context_set_image(new_im);
   imlib_render_image_on_drawable_at_size(x, y, w, h);
   imlib_free_image_and_decache();
}

void
feh_imlib_render_image_part_on_drawable_at_size(Drawable d, Imlib_Image im,
                                                int sx, int sy, int sw,
                                                int sh, int dx, int dy,
                                                int dw, int dh, char dither,
                                                char blend, char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_drawable(d);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(0);
   imlib_render_image_part_on_drawable_at_size(sx, sy, sw, sh, dx, dy, dw,
                                               dh);
}

void
feh_imlib_render_image_part_on_drawable_at_size_with_rotation(Drawable d,
                                                              Imlib_Image im,
                                                              int sx, int sy,
                                                              int sw, int sh,
                                                              int dx, int dy,
                                                              int dw, int dh,
                                                              double angle,
                                                              char dither,
                                                              char blend,
                                                              char alias)
{
   Imlib_Image new_im;

   imlib_context_set_image(im);
   imlib_context_set_drawable(d);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_angle(angle);
   imlib_context_set_blend(blend);
   new_im = imlib_create_rotated_image(angle);
   imlib_context_set_image(new_im);
   imlib_render_image_part_on_drawable_at_size(sx, sy, sw, sh, dx, dy, dw,
                                               dh);
   imlib_free_image_and_decache();
}

void
feh_imlib_image_fill_rectangle(Imlib_Image im, int x, int y, int w, int h,
                               int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_fill_rectangle(x, y, w, h);
}

void
feh_imlib_image_draw_rectangle(Imlib_Image im, int x, int y, int w, int h,
                               int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_draw_rectangle(x, y, w, h);
}


void
feh_imlib_text_draw(Imlib_Image im, Imlib_Font fn, int x, int y, char *text,
                    Imlib_Text_Direction dir, int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_context_set_font(fn);
   imlib_context_set_direction(dir);
   imlib_text_draw(x, y, text);
}

void
feh_imlib_get_text_size(Imlib_Font fn, char *text, int *w, int *h,
                        Imlib_Text_Direction dir)
{
   imlib_context_set_font(fn);
   imlib_context_set_direction(dir);
   imlib_get_text_size(text, w, h);
}

Imlib_Image
feh_imlib_clone_image(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_clone_image();
}

char *
feh_imlib_image_format(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_format();
}

void
feh_imlib_blend_image_onto_image(Imlib_Image dest_image,
                                 Imlib_Image source_image, char merge_alpha,
                                 int sx, int sy, int sw, int sh, int dx,
                                 int dy, int dw, int dh, char dither,
                                 char blend, char alias)
{
   imlib_context_set_image(dest_image);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(0);
   imlib_blend_image_onto_image(source_image, merge_alpha, sx, sy, sw, sh, dx,
                                dy, dw, dh);
}

void
feh_imlib_blend_image_onto_image_with_rotation(Imlib_Image dest_image,
                                               Imlib_Image source_image,
                                               char merge_alpha, int sx,
                                               int sy, int sw, int sh, int dx,
                                               int dy, int dw, int dh,
                                               double angle, char dither,
                                               char blend, char alias)
{
   imlib_context_set_image(dest_image);
   imlib_context_set_anti_alias(alias);
   imlib_context_set_dither(dither);
   imlib_context_set_blend(blend);
   imlib_context_set_angle(angle);
   imlib_blend_image_onto_image_at_angle(source_image, merge_alpha, sx, sy,
                                         sw, sh, dx, dy, (int) angle,
                                         (int) angle);
   return;
   dw = 0;
   dh = 0;
}

Imlib_Image
feh_imlib_create_cropped_scaled_image(Imlib_Image im, int sx, int sy, int sw,
                                      int sh, int dw, int dh, char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_anti_alias(alias);
   return imlib_create_cropped_scaled_image(sx, sy, sw, sh, dw, dh);
}

void
feh_imlib_apply_color_modifier_to_rectangle(Imlib_Image im, int x, int y,
                                            int w, int h, DATA8 * rtab,
                                            DATA8 * gtab, DATA8 * btab,
                                            DATA8 * atab)
{
   Imlib_Color_Modifier cm;

   imlib_context_set_image(im);
   cm = imlib_create_color_modifier();
   imlib_context_set_color_modifier(cm);
   imlib_set_color_modifier_tables(rtab, gtab, btab, atab);
   imlib_apply_color_modifier_to_rectangle(x, y, w, h);
   imlib_free_color_modifier();
}

void
feh_imlib_image_set_has_alpha(Imlib_Image im, int alpha)
{
   imlib_context_set_image(im);
   imlib_image_set_has_alpha(alpha);
}

void
feh_imlib_save_image(Imlib_Image im, char *file)
{
   char *tmp;

   imlib_context_set_image(im);
   tmp = strrchr(file, '.');
   if (tmp)
      imlib_image_set_format(tmp + 1);
   imlib_save_image(file);
}

void
feh_imlib_free_font(Imlib_Font fn)
{
   imlib_context_set_font(fn);
   imlib_free_font();
}

void
feh_imlib_image_draw_line(Imlib_Image im, int x1, int y1, int x2, int y2,
                          char make_updates, int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_draw_line(x1, y1, x2, y2, make_updates);
}

Imlib_Image
feh_imlib_create_rotated_image(Imlib_Image im, double angle)
{
   imlib_context_set_image(im);
   return (imlib_create_rotated_image(angle));
}

void
feh_imlib_image_tile(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_image_tile();
}

void
feh_imlib_image_blur(Imlib_Image im, int radius)
{
   imlib_context_set_image(im);
   imlib_image_blur(radius);
}

void
feh_imlib_image_sharpen(Imlib_Image im, int radius)
{
   imlib_context_set_image(im);
   imlib_image_sharpen(radius);
}

Imlib_Font
feh_imlib_load_font(char *name)
{
   Imlib_Font fn;

   if (fn = imlib_load_font(name))
      return fn;
   weprintf("couldn't load font %s, attempting to fall back to fixed.", name);
   if (fn = imlib_load_font("fixed"))
      return fn;
   weprintf("failed to even load fixed! Attempting to find any font.");
   return imlib_load_font("*");
}
