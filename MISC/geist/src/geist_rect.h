/* geist_rect.h

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

#ifndef GEIST_rect_H
#define GEIST_rect_H

#include "geist.h"
#include "geist_object.h"
#include "geist.h"
#include "geist_image.h"

#define GEIST_RECT(O) ((geist_rect *) O)

struct __geist_rect
{
   geist_object object;
   int r, g, b, a;
};

geist_object *geist_rect_new(void);
geist_object *geist_rect_new_of_size(int x, int y, int w, int h, int a, int r,
                                     int g, int b);
void geist_rect_init(geist_rect * txt);
void geist_rect_free(geist_object * obj);
void geist_rect_render(geist_object * obj, Imlib_Image dest);
void geist_rect_render_partial(geist_object * obj, Imlib_Image dest, int x,
                               int y, int w, int h);
geist_object *geist_rect_duplicate(geist_object * obj);
unsigned char geist_text_part_is_transparent(geist_object * obj, int x,

                                             int y);
void geist_rect_resize(geist_object * obj, int x, int y);
GtkWidget *geist_rect_display_props(geist_object * obj);

#endif
