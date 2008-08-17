/* alb_layer.h

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


#ifndef ALB_LAYER_H
#define ALB_LAYER_H

#include "alb.h"
#include "alb_object.h"
#include "alb_document.h"
#include "alb_fill.h"

#define ALB_LAYER(O) ((alb_layer *)O)

struct __alb_layer {
  gib_list *objects;
  alb_document *doc;
  int alpha;
  int x_offset;
  int y_offset;
  int visible;
  char *name;
};

alb_layer *alb_layer_new(void);
void alb_layer_free(alb_layer * l);
void alb_layer_render(alb_layer * layer,
                      Imlib_Image dest);
void alb_layer_render_partial(alb_layer * layer,
                              Imlib_Image dest,
                              int x,
                              int y,
                              int w,
                              int h);

void alb_layer_add_object(alb_layer * layer,
                          alb_object * obj);
void alb_layer_raise_object(alb_object * obj);
alb_object *alb_layer_find_clicked_object(alb_layer * layer,
                                          int x,
                                          int y);
unsigned char alb_layer_remove_object(alb_layer * lay,
                                      alb_object * obj);

#endif
