#ifndef IMLIB_WRAP_H
#define IMLIB_WRAP_H

#include <X11/Xlib.h>
#include <Imlib2.h>
#include "geist_list.h"

struct __font_style_bit
{
   int x_offset, y_offset;
   int r,g,b,a;
};

struct __font_style
{
   geist_list *bits;
   char *name;
};

typedef struct __font_style font_style;
typedef struct __font_style_bit font_style_bit;

int imlib_wrap_image_get_width(Imlib_Image im);
int imlib_wrap_image_get_height(Imlib_Image im);
int imlib_wrap_image_has_alpha(Imlib_Image im);
const char *imlib_wrap_image_get_filename(Imlib_Image im);
void imlib_wrap_free_image_and_decache(Imlib_Image im);
void imlib_wrap_render_image_on_drawable(Drawable d, Imlib_Image im, int x,
                                          int y, char dither, char blend,

                                          char alias);
void imlib_wrap_render_image_part_on_drawable_at_size(Drawable d,
                                                       Imlib_Image im, int sx,
                                                       int sy, int sw, int sh,
                                                       int dx, int dy, int dw,
                                                       int dh, char dither,

                                                       char blend,
                                                       char alias);
void imlib_wrap_image_fill_rectangle(Imlib_Image im, int x, int y, int w,
                                      int h, int r, int g, int b, int a);
void imlib_wrap_text_draw(Imlib_Image im, Imlib_Font fn, font_style *s,int x, int y,
                           char *text, Imlib_Text_Direction dir, int r, int g,
                           int b, int a);
void imlib_wrap_get_text_size(Imlib_Font fn, char *text, font_style *s, int *w, int *h,
                               Imlib_Text_Direction dir);
Imlib_Image imlib_wrap_clone_image(Imlib_Image im);
char *imlib_wrap_image_format(Imlib_Image im);
char **imlib_wrap_list_fonts(int *num);
void imlib_wrap_render_image_on_drawable_at_size(Drawable d, Imlib_Image im,
                                                  int x, int y, int w, int h,
                                                  char dither, char blend,

                                                  char alias);
void imlib_wrap_blend_image_onto_image(Imlib_Image dest_image,
                                        Imlib_Image source_image,
                                        char merge_alpha, int sx, int sy,
                                        int sw, int sh, int dx, int dy,
                                        int dw, int dh, char dither,
                                        char blend, char alias);
Imlib_Image imlib_wrap_create_cropped_scaled_image(Imlib_Image im, int sx,
                                                    int sy, int sw, int sh,
                                                    int dw, int dh,

                                                    char alias);
void imlib_wrap_apply_color_modifier_to_rectangle(Imlib_Image im, int x,
                                                   int y, int w, int h,
                                                   DATA8 * rtab, DATA8 * gtab,
                                                   DATA8 * btab,

                                                   DATA8 * atab);
void imlib_wrap_image_set_has_alpha(Imlib_Image im, int alpha);
void imlib_wrap_save_image(Imlib_Image im, char *file);
void imlib_wrap_free_font(Imlib_Font fn);
void imlib_wrap_free_image(Imlib_Image im);
void imlib_wrap_image_draw_line(Imlib_Image im, int x1, int y1, int x2,
                                 int y2, char make_updates, int r, int g,
                                 int b, int a);
void imlib_wrap_image_set_has_alpha(Imlib_Image im, int alpha);
void imlib_wrap_save_image(Imlib_Image im, char *file);
void imlib_wrap_free_font(Imlib_Font fn);
void imlib_wrap_image_tile(Imlib_Image im);
void imlib_wrap_image_blur(Imlib_Image im, int radius);
void imlib_wrap_image_sharpen(Imlib_Image im, int radius);
void imlib_wrap_image_draw_rectangle(Imlib_Image im, int x, int y, int w,
                                      int h, int r, int g, int b, int a);
void imlib_wrap_line_clip_and_draw(Imlib_Image dest, int x0, int y0, int x1,
                               int y1, int cx, int cy, int cw, int ch,
                               int r, int g, int b, int a);
void imlib_wrap_image_fill_polygon(Imlib_Image im, ImlibPolygon poly, int r,
                                    int g, int b, int a, unsigned char alias,
                                    int cx, int cy, int cw, int ch);
void imlib_wrap_image_draw_polygon(Imlib_Image im, ImlibPolygon poly, int r,
                                    int g, int b, int a, unsigned char closed,
                                    unsigned char alias, int cx, int cy,
                                    int cw, int ch);
font_style *font_style_new(char *name);
void font_style_free(font_style * s);
font_style_bit *font_style_bit_new(int x_offset, int y_offset, int r, int g, int b, int a);
void font_style_bit_free(font_style_bit * s);
font_style *font_style_new_from_ascii(char *file);

#endif
