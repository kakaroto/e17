/* feh_imlib2.h

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

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <stdarg.h>
#include <string.h>

int feh_imlib_image_get_width(Imlib_Image im);
int feh_imlib_image_get_height(Imlib_Image im);
int feh_imlib_image_has_alpha(Imlib_Image im);
void feh_imlib_free_image_and_decache(Imlib_Image im);
void feh_imlib_render_image_on_drawable(Drawable d, Imlib_Image im, int x,
                                        int y, char dither, char blend,

                                        char alias);
void feh_imlib_render_image_on_drawable_with_rotation(Drawable d,
                                                      Imlib_Image im, int x,
                                                      int y, double angle,
                                                      char dither, char blend,

                                                      char alias);
void feh_imlib_render_image_part_on_drawable_at_size(Drawable d,
                                                     Imlib_Image im, int sx,
                                                     int sy, int sw, int sh,
                                                     int dx, int dy, int dw,
                                                     int dh, char dither,
                                                     char blend, char alias);
void feh_imlib_render_image_part_on_drawable_at_size_with_rotation(Drawable d,
                                                                   Imlib_Image
                                                                   im, int sx,

                                                                   int sy,
                                                                   int sw,
                                                                   int sh,
                                                                   int dx,
                                                                   int dy,
                                                                   int dw,
                                                                   int dh,
                                                                   double
                                                                   angle,
                                                                   char
                                                                   dither,
                                                                   char blend,
                                                                   char
                                                                   alias);
void feh_imlib_image_fill_rectangle(Imlib_Image im, int x, int y, int w,
                                    int h, int r, int g, int b, int a);
void feh_imlib_text_draw(Imlib_Image im, Imlib_Font fn, int x, int y,
                         char *text, Imlib_Text_Direction dir, int r, int g,
                         int b, int a);
void feh_imlib_get_text_size(Imlib_Font fn, char *text, int *w, int *h,
                             Imlib_Text_Direction dir);
Imlib_Image feh_imlib_clone_image(Imlib_Image im);
char *feh_imlib_image_format(Imlib_Image im);
void feh_imlib_render_image_on_drawable_at_size(Drawable d, Imlib_Image im,
                                                int x, int y, int w, int h,
                                                char dither, char blend,

                                                char alias);
void feh_imlib_render_image_on_drawable_at_size_with_rotation(Drawable d,
                                                              Imlib_Image im,
                                                              int x, int y,
                                                              int w, int h,

                                                              double angle,
                                                              char dither,
                                                              char blend,
                                                              char alias);
void feh_imlib_blend_image_onto_image(Imlib_Image dest_image,
                                      Imlib_Image source_image,
                                      char merge_alpha, int sx, int sy,
                                      int sw, int sh, int dx, int dy, int dw,
                                      int dh, char dither, char blend,

                                      char alias);
void feh_imlib_blend_image_onto_image_with_rotation(Imlib_Image dest_image,
                                                    Imlib_Image source_image,
                                                    char merge_alpha, int sx,
                                                    int sy, int sw, int sh,
                                                    int dx, int dy, int dw,
                                                    int dh, double angle,
                                                    char dither, char blend,

                                                    char alias);
Imlib_Image feh_imlib_create_cropped_scaled_image(Imlib_Image im, int sx,
                                                  int sy, int sw, int sh,
                                                  int dw, int dh, char alias);
void feh_imlib_apply_color_modifier_to_rectangle(Imlib_Image im, int x, int y,
                                                 int w, int h, DATA8 * rtab,
                                                 DATA8 * gtab, DATA8 * btab,
                                                 DATA8 * atab);
void feh_imlib_image_set_has_alpha(Imlib_Image im, int alpha);
void feh_imlib_save_image(Imlib_Image im, char *file);
void feh_imlib_free_font(Imlib_Font fn);
void feh_imlib_free_image(Imlib_Image im);
void feh_imlib_image_draw_line(Imlib_Image im, int x1, int y1, int x2, int y2,
                               char make_updates, int r, int g, int b, int a);
void feh_imlib_image_set_has_alpha(Imlib_Image im, int alpha);
void feh_image_save_image(Imlib_Image im, char *file);
void feh_imlib_free_font(Imlib_Font fn);
Imlib_Image feh_imlib_create_rotated_image(Imlib_Image im, double angle);
void feh_imlib_image_tile(Imlib_Image im);
void feh_imlib_image_blur(Imlib_Image im, int radius);
void feh_imlib_image_sharpen(Imlib_Image im, int radius);
void feh_imlib_image_draw_rectangle(Imlib_Image im, int x, int y, int w,
                                    int h, int r, int g, int b, int a);
Imlib_Font feh_imlib_load_font(char *name);
