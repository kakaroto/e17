/* alb_line.h

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

#ifndef ALB_LINE_H
#define ALB_LINE_H

#include "alb.h"
#include "alb_object.h"
#include "alb.h"
#include "alb_image.h"

#define ALB_LINE(O) ((alb_line *) O)

struct __alb_line {
  alb_object object;
  struct {
    int x, y;
  } start;
  struct {
    int x, y;
  } end;
  int r, g, b, a;
};

alb_object *alb_line_new(void);
alb_object *alb_line_new_from_to(int start_x,
                                 int start_y,
                                 int end_x,
                                 int end_y,
                                 int a,
                                 int r,
                                 int g,
                                 int b);
void alb_line_init(alb_line * line);
void alb_line_free(alb_object * obj);
void alb_line_render(alb_object * obj,
                     Imlib_Image dest);
void alb_line_render_partial(alb_object * obj,
                             Imlib_Image dest,
                             int x,
                             int y,
                             int w,
                             int h);
alb_object *alb_line_duplicate(alb_object * obj);
unsigned char alb_line_part_is_transparent(alb_object * obj,
                                           int x,
                                           int y);
void alb_line_resize(alb_object * obj,
                     int x,
                     int y);
GtkWidget *alb_line_display_props(alb_object * obj);
void alb_line_render_selected(alb_object * obj,
                              Imlib_Image dest,
                              unsigned char multiple);
int
  alb_line_get_clipped_line(alb_line * line,
                            int *clip_x0,
                            int *clip_y0,
                            int *clip_x1,
                            int *clip_y1);

Imlib_Updates alb_line_get_selection_updates(alb_object * obj);
void
  alb_line_get_resize_box_coords(alb_object * obj,
                                 int resize,
                                 int *x,
                                 int *y);
int
  alb_line_check_resize_click(alb_object * obj,
                              int x,
                              int y);
void alb_line_change_from_to(alb_line * line,
                             int start_x,
                             int start_y,
                             int end_x,
                             int end_y);
unsigned char
  alb_line_click_is_selection(alb_object * obj,
                              int x,
                              int y);
Imlib_Updates alb_line_get_updates(alb_object * obj);
void alb_line_update_dimensions_relative(alb_object * obj,
                                         int w_offset,
                                         int h_offset);
void alb_line_rotate(alb_object * obj,
                     double angle);
void
  alb_line_get_rendered_area(alb_object * obj,
                             int *x,
                             int *y,
                             int *w,
                             int *h);



#endif
