/* geist_imlib2.c
 *
 * Copyright (C) 2000 Tom Gilbert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "geist_imlib.h"

int
geist_imlib_load_image(Imlib_Image * im, char *filename)
{
   Imlib_Load_Error err;

   imlib_context_set_progress_function(NULL);
   if (!filename)
      return (0);
   *im = imlib_load_image_with_error_return(filename, &err);
   if ((err) || (!im))
   {
      /* Check error code */
      switch (err)
      {
        case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
           weprintf("%s - File does not exist", filename);
           break;
        case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
           weprintf("%s - Directory specified for image filename", filename);
           break;
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
           weprintf("%s - No read access to directory", filename);
           break;
        case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
           weprintf("%s - No Imlib2 loader for that file format", filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
           weprintf("%s - Path specified is too long", filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
           weprintf("%s - Path component does not exist", filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
           weprintf("%s - Path component is not a directory", filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
           weprintf("%s - Path points outside address space", filename);
           break;
        case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
           weprintf("%s - Too many levels of symbolic links", filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
           eprintf("While loading %s - Out of memory", filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
           eprintf("While loading %s - Out of file descriptors", filename);
           break;
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
           weprintf("%s - Cannot write to directory", filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
           weprintf("%s - Cannot write - out of disk space", filename);
           break;
        case IMLIB_LOAD_ERROR_UNKNOWN:
        default:
           weprintf
              ("While loading %s - Unknown error. Attempting to continue",
               filename);
           break;
      }
      return (0);
   }
   return (1);
}

int
geist_imlib_image_get_width(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_get_width();
}

int
geist_imlib_image_get_height(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_get_height();
}

int
geist_imlib_image_has_alpha(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_has_alpha();
}

void
geist_imlib_free_image_and_decache(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_free_image_and_decache();
}

void
geist_imlib_free_image(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_free_image();
}

const char *
geist_imlib_image_get_filename(Imlib_Image im)
{
   if (im)
   {
      imlib_context_set_image(im);
      return imlib_image_get_filename();
   }
   else
      return NULL;
}

void
geist_imlib_render_image_on_drawable(Drawable d, Imlib_Image im, int x, int y,
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
geist_imlib_render_image_on_drawable_with_rotation(Drawable d, Imlib_Image im,
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
geist_imlib_render_image_on_drawable_at_size(Drawable d, Imlib_Image im,
                                             int x, int y, int w, int h,
                                             char dither, char blend,
                                             char alias)
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
geist_imlib_render_image_on_drawable_at_size_with_rotation(Drawable d,
                                                           Imlib_Image im,
                                                           int x, int y,
                                                           int w, int h,
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
   imlib_context_set_blend(blend);
   imlib_context_set_angle(angle);
   new_im = imlib_create_rotated_image(angle);
   imlib_context_set_image(new_im);
   imlib_render_image_on_drawable_at_size(x, y, w, h);
   imlib_free_image_and_decache();
}

void
geist_imlib_render_image_part_on_drawable_at_size(Drawable d, Imlib_Image im,
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
geist_imlib_render_image_part_on_drawable_at_size_with_rotation(Drawable d,
                                                                Imlib_Image
                                                                im, int sx,
                                                                int sy,
                                                                int sw,
                                                                int sh,
                                                                int dx,
                                                                int dy,
                                                                int dw,
                                                                int dh,
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
geist_imlib_image_fill_rectangle(Imlib_Image im, int x, int y, int w, int h,
                                 int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_fill_rectangle(x, y, w, h);
}

void
geist_imlib_image_draw_rectangle(Imlib_Image im, int x, int y, int w, int h,
                                 int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_draw_rectangle(x, y, w, h);
}


void
geist_imlib_text_draw(Imlib_Image im, Imlib_Font fn, int x, int y, char *text,
                      Imlib_Text_Direction dir, int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_context_set_font(fn);
   imlib_context_set_direction(dir);
   imlib_text_draw(x, y, text);
}

char **
geist_imlib_list_fonts(int *num)
{
   return imlib_list_fonts(num);
}


void
geist_imlib_get_text_size(Imlib_Font fn, char *text, int *w, int *h,
                          Imlib_Text_Direction dir)
{
   imlib_context_set_font(fn);
   imlib_context_set_direction(dir);
   imlib_get_text_size(text, w, h);
}

Imlib_Image
geist_imlib_clone_image(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_clone_image();
}

char *
geist_imlib_image_format(Imlib_Image im)
{
   imlib_context_set_image(im);
   return imlib_image_format();
}

void
geist_imlib_blend_image_onto_image(Imlib_Image dest_image,
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
geist_imlib_blend_image_onto_image_with_rotation(Imlib_Image dest_image,
                                                 Imlib_Image source_image,
                                                 char merge_alpha, int sx,
                                                 int sy, int sw, int sh,
                                                 int dx, int dy, int dw,
                                                 int dh, double angle,
                                                 char dither, char blend,
                                                 char alias)
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
geist_imlib_create_cropped_scaled_image(Imlib_Image im, int sx, int sy,
                                        int sw, int sh, int dw, int dh,
                                        char alias)
{
   imlib_context_set_image(im);
   imlib_context_set_anti_alias(alias);
   return imlib_create_cropped_scaled_image(sx, sy, sw, sh, dw, dh);
}

void
geist_imlib_apply_color_modifier_to_rectangle(Imlib_Image im, int x, int y,
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
geist_imlib_image_set_has_alpha(Imlib_Image im, int alpha)
{
   imlib_context_set_image(im);
   imlib_image_set_has_alpha(alpha);
}

void
geist_imlib_save_image(Imlib_Image im, char *file)
{
   imlib_context_set_image(im);
   imlib_save_image(file);
}

void
geist_imlib_free_font(Imlib_Font fn)
{
   imlib_context_set_font(fn);
   imlib_free_font();
}

void
geist_imlib_image_draw_line(Imlib_Image im, int x1, int y1, int x2, int y2,
                            char make_updates, int r, int g, int b, int a)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, a);
   imlib_image_draw_line(x1, y1, x2, y2, make_updates);
}

Imlib_Image
geist_imlib_create_rotated_image(Imlib_Image im, double angle)
{
   imlib_context_set_image(im);
   return (imlib_create_rotated_image(angle));
}

void
geist_imlib_image_tile(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_image_tile();
}

void
geist_imlib_image_blur(Imlib_Image im, int radius)
{
   imlib_context_set_image(im);
   imlib_image_blur(radius);
}

void
geist_imlib_image_sharpen(Imlib_Image im, int radius)
{
   imlib_context_set_image(im);
   imlib_image_sharpen(radius);
}

DATA8 geist_imlib_image_part_is_transparent(Imlib_Image im, int x, int y)
{
   Imlib_Color c;
   int num = 0;
   int ave = 0;
   int w, h;
   int leftmost, rightmost, topmost, bottommost, i, j;

   imlib_context_set_image(im);
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   if ((x > w) || (y > h))
      return 1;

   leftmost = x - 1;
   if (leftmost < 0)
      leftmost = 0;
   rightmost = x + 1;
   if (rightmost > w)
      rightmost = w;
   topmost = y - 1;
   if (topmost < 0)
      topmost = 0;
   bottommost = y + 1;
   if (bottommost > h)
      bottommost = h;

   for (i = leftmost; i < rightmost + 1; i++)
      for (j = topmost; j < bottommost + 1; j++)
      {
         imlib_image_query_pixel(i, j, &c);
         ave += c.alpha;
         num++;
      }

   ave = ave / num;

/* TODO Make this fuzziness an OPTION */
   if (ave > TRANS_THRESHOLD)
      return 0;

   return 1;
}

void
geist_imlib_line_clip_and_draw(Imlib_Image dest, int x0, int y0, int x1,
                               int y1, int xmin, int xmax, int ymin, int ymax,
                               int r, int g, int b, int a)
{
   imlib_context_set_cliprect(xmin, ymin, xmax - xmin, ymax - ymin);
   geist_imlib_image_draw_line(dest, x0, y0, x1, y1, 0, r, g, b, a);
   imlib_context_set_cliprect(0, 0, 0, 0);
}

