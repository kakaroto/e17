/* geist_style.h

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


#ifndef GEIST_STYLE_H
#define GEIST_STYLE_H

#include "geist.h"
#include "geist_object.h"
#include "geist_document.h"
#include "geist_list.h"

#define GEIST_FILL(O) ((geist_fill *)O)

struct __geist_style_bit
{
   int x_offset, y_offset;
   int r,g,b,a;
};

struct __geist_style
{
   geist_list *bits;
   char *name;
};

geist_style_bit *geist_style_bit_new(int x_offset, int y_offset, int r, int g, int b, int a);
geist_style *geist_style_new(char *name);
void geist_style_bit_free(geist_style_bit *s);
void geist_style_free(geist_style *s);
geist_style *geist_style_dup(geist_style *s);
void geist_dup_style_bit(void *dest, void *data);

#endif
