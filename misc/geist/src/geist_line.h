#ifndef GEIST_LINE_H
#define GEIST_LINE_H

#include "geist.h"
#include "geist_object.h"
#include "geist.h"
#include "geist_imlib.h"
#include "geist_image.h"

#define GEIST_LINE(O) ((geist_line *) O)

struct __geist_line
{
   geist_object object;
   struct
   {
      int x, y;
   }
   start;
   struct
   {
      int x, y;
   }
   end;
   int r, g, b, a;
};

geist_object *geist_line_new(void);
geist_object *geist_line_new_from_to(int start_x, int start_y, int end_x,
                                     int end_y, int a, int r, int g, int b);
void geist_line_init(geist_line * line);
void geist_line_free(geist_object * obj);
void geist_line_render(geist_object * obj, Imlib_Image dest);
void geist_line_render_partial(geist_object * obj, Imlib_Image dest, int x,
                               int y, int w, int h);
geist_object *geist_line_duplicate(geist_object * obj);
unsigned char geist_line_part_is_transparent(geist_object * obj, int x,

                                             int y);
void geist_line_resize(geist_object * obj, int x, int y);
GtkWidget *geist_line_display_props(geist_object * obj);
void geist_line_render_selected(geist_object * obj, Imlib_Image dest, 
                                 unsigned char multiple);
int
geist_line_get_clipped_line(geist_line * line, int *clip_x0, int *clip_y0,
                            int *clip_x1, int *clip_y1);
Imlib_Updates
geist_line_get_selection_updates(geist_object * obj);
void
geist_line_get_resize_box_coords(geist_object * obj, int resize, int *x,
                                 int *y);
int
geist_line_check_resize_click(geist_object * obj, int x, int y);
void geist_line_change_from_to(geist_line * line, int start_x, int start_y,
                          int end_x, int end_y);
unsigned char
geist_line_click_is_selection(geist_object * obj, int x, int y);
Imlib_Updates geist_line_get_updates(geist_object * obj);
void geist_line_update_dimensions_relative (geist_object *obj, 
														  int w_offset, int h_offset);
void geist_line_rotate (geist_object *obj, double angle);



#endif
