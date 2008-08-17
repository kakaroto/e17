/* geist_line.h

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

#ifndef GEIST_LINE_H
#define GEIST_LINE_H

#include "geist.h"
#include "geist_object.h"
#include "geist.h"
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
void
geist_line_get_rendered_area(geist_object * obj, int *x, int *y, int *w,
                                   int *h);



#endif
