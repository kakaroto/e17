/* alb_rect.h

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

#ifndef ALB_rect_H
#define ALB_rect_H

#include "alb.h"
#include "alb_object.h"
#include "alb.h"
#include "alb_image.h"

#define ALB_RECT(O) ((alb_rect *) O)

struct __alb_rect {
  alb_object object;
  int r, g, b, a;
};

alb_object *alb_rect_new(void);
alb_object *alb_rect_new_of_size(int x,
                                 int y,
                                 int w,
                                 int h,
                                 int a,
                                 int r,
                                 int g,
                                 int b);
void alb_rect_init(alb_rect * txt);
void alb_rect_free(alb_object * obj);
void alb_rect_render(alb_object * obj,
                     Imlib_Image dest);
void alb_rect_render_partial(alb_object * obj,
                             Imlib_Image dest,
                             int x,
                             int y,
                             int w,
                             int h);
alb_object *alb_rect_duplicate(alb_object * obj);
unsigned char alb_text_part_is_transparent(alb_object * obj,
                                           int x,
                                           int y);
void alb_rect_resize(alb_object * obj,
                     int x,
                     int y);
GtkWidget *alb_rect_display_props(alb_object * obj);

#endif
