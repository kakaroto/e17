/* alb_poly.h

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

#ifndef ALB_POLY_H
#define ALB_POLY_H

#include "alb.h"
#include "alb_object.h"
#include "alb.h"
#include "alb_image.h"

#define ALB_POLY(O) ((alb_poly *) O)

struct __alb_poly {
  alb_object object;
  gib_list *points;
  ImlibPolygon poly;
  int filled;
  int closed;
  int need_update;
  int r, g, b, a;
};

alb_object *alb_poly_new(void);
alb_object *alb_poly_new_from_points(gib_list * points,
                                     int a,
                                     int r,
                                     int g,
                                     int b);
void alb_poly_init(alb_poly * txt);
void alb_poly_free(alb_object * obj);
void alb_poly_render(alb_object * obj,
                     Imlib_Image dest);
void alb_poly_render_partial(alb_object * obj,
                             Imlib_Image dest,
                             int x,
                             int y,
                             int w,
                             int h);
alb_object *alb_poly_duplicate(alb_object * obj);
unsigned char alb_poly_part_is_transparent(alb_object * obj,
                                           int x,
                                           int y);
void alb_poly_resize(alb_object * obj,
                     int x,
                     int y);
GtkWidget *alb_poly_display_props(alb_object * obj);
unsigned char alb_poly_has_transparency(alb_object * obj);
void alb_poly_update_imlib_polygon(alb_poly * poly);
alb_point *alb_point_new(int x,
                         int y);
void alb_poly_update_bounds(alb_poly * poly);
void alb_poly_rotate(alb_object * obj,
                     double angle);
void alb_poly_add_point(alb_poly * poly,
                        int x,
                        int y);
void alb_poly_move(alb_object * obj,
                   int x,
                   int y);
void alb_poly_get_rendered_area(alb_object * obj,
                                int *x,
                                int *y,
                                int *w,
                                int *h);
unsigned char
  alb_poly_part_is_transparent(alb_object * obj,
                               int x,
                               int y);
void
  alb_poly_move_points_relative(alb_poly * poly,
                                int x,
                                int y);


#endif
