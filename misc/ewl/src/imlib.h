#ifndef _EWL_IMLIB_H_
#define _EWL_IMLIB_H_ 1

#include "includes.h"
#include "debug.h"
#include "util.h"
#include "ll.h"
#include "layout.h"

/******************************/
/* IMAGE ALLOCATION FUNCTIONS */
/******************************/
Imlib_Image ewl_imlib_load_image(char *file);
Imlib_Image ewl_imlib_clone_image(Imlib_Image im);
void        ewl_imlib_save_image(Imlib_Image im, char *file);
void        ewl_imlib_free_image(Imlib_Image im);

/*****************************/
/* IMAGE ATTRIBUTE FUNCTIONS */
/*****************************/
int         ewl_imlib_image_get_width(Imlib_Image im);
int         ewl_imlib_image_get_height(Imlib_Image im);
char       *ewl_imlib_image_format(Imlib_Image im);
char        ewl_imlib_image_has_alpha(Imlib_Image im);
void        ewl_imlib_image_set_has_alpha(Imlib_Image im, int alpha);
void        ewl_imlib_image_tile(Imlib_Image im);
void        ewl_imlib_image_blur(Imlib_Image im, int radius);
void        ewl_imlib_image_sharpen(Imlib_Image im, int radius);

/*****************************/
/* IMAGE DRAWING FUNCTIONS   */
/*****************************/
void        ewl_imlib_render_image_on_drawable(
                                          Drawable     d,
                                          Imlib_Image  im,
                                          int          x,
                                          int          y,
                                          double       angle,
                                          char         dither,
                                          char         blend,
                                          char         alias);
void        ewl_imlib_render_image_on_drawable_at_size(
                                          Drawable     d,
                                          Imlib_Image  im,
                                          int          x,
                                          int          y,
                                          int          w,
                                          int          h,
                                          double       angle,
                                          char         dither,
                                          char         blend,
                                          char         alias);
void        ewl_imlib_render_image_part_on_drawable_at_size(
                                          Drawable     d,
                                          Imlib_Image  im,
                                          int          sx,
                                          int          sy,
                                          int          sw,
                                          int          sh,
                                          int          dx,
                                          int          dy,
                                          int          dw,
                                          int          dh,
                                          double       angle,
                                          char         dither,
                                          char         blend,
                                          char         alias);
void        ewl_imlib_blend_image_onto_image(
                                          Imlib_Image  dest_image,
                                          Imlib_Image  source_image,
                                          char         merge_alpha,
                                          int          sx,
                                          int          sy,
                                          int          sw,
                                          int          sh,
                                          int          dx,
                                          int          dy,
                                          int          dw,
                                          int          dh,
                                          double       angle,
                                          char         dither,
                                          char         blend,
                                          char         alias);
Imlib_Image ewl_imlib_create_cropped_scaled_image(
                                          Imlib_Image  im,
                                          int          sx, 
                                          int          sy,
                                          int          sw,
                                          int          sh,
                                          int          dw,
                                          int          dh,
                                          char         alias);
Imlib_Image ewl_imlib_create_rotated_image(Imlib_Image im,
                                           double angle);

/*****************************/
/* POLYGON DRAWING FUNCTIONS */
/*****************************/
void        ewl_imlib_image_draw_line(Imlib_Image im,
                                      int x1, int y1, int x2, int y2,
                                      char make_updates,
                                      int r, int g, int b, int a);
void        ewl_imlib_image_draw_rectangle(Imlib_Image  im,
                                           int x, int y,
                                           int w, int h,
                                           int r, int g, int b, int a);
void        ewl_imlib_image_fill_rectangle(Imlib_Image  im,
                                           int x, int y,
                                           int w, int h,
                                           int r, int g, int b, int a);

/*****************************************/
/* MISCELLANEOUS IMAGE/POLYGON FUNCTIONS */
/*****************************************/
void        ewl_imlib_apply_color_modifier_to_rectangle(
                                           Imlib_Image im,
                                           int x, int y, int w, int h,
                                           DATA8 * rtab, DATA8 * gtab,
                                           DATA8 * btab, DATA8 * atab);

/******************/
/* TEXT FUNCTIONS */
/******************/
void        ewl_imlib_text_draw(Imlib_Image           im,
                                Imlib_Font            fn,
                                int x, int y, char *text,
                                Imlib_Text_Direction  dir, 
                                int r, int g, int b, int a);
void        ewl_imlib_get_text_size(Imlib_Font fn, char *text,
                             int *w, int *h,
                             Imlib_Text_Direction dir);

void        ewl_imlib_free_font(Imlib_Font fn);

/******************/
/* MISC FUNCTIONS */
/******************/
char       *ewl_imlib_get_error_string(char *path, Imlib_Load_Error err);



#endif /* _EWL_IMLIB_H_ */
