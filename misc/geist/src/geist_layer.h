/* geist_layer.h

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


#ifndef GEIST_LAYER_H
#define GEIST_LAYER_H

#include "geist.h"
#include "geist_object.h"
#include "geist_document.h"
#include "geist_fill.h"

#define GEIST_LAYER(O) ((geist_layer *)O)

struct __geist_layer
{
   gib_list *objects;
   geist_document *doc;
   int alpha;
   int x_offset;
   int y_offset;
   int visible;
   char *name;
};

geist_layer *geist_layer_new(void);
void geist_layer_free(geist_layer * l);
void geist_layer_render(geist_layer * layer, Imlib_Image dest);
void geist_layer_render_partial(geist_layer * layer, Imlib_Image dest, int x,
                                int y, int w, int h);

void geist_layer_add_object(geist_layer * layer, geist_object * obj);
void geist_layer_raise_object(geist_object * obj);
geist_object *geist_layer_find_clicked_object(geist_layer * layer, int x,

                                              int y);
unsigned char geist_layer_remove_object(geist_layer * lay,

                                        geist_object * obj);

#endif
