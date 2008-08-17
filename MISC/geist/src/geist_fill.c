/* geist_fill.c

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

#include "geist.h"
#include "geist_fill.h"

geist_fill *
geist_fill_new(void)
{
   geist_fill *f = NULL;

   D_ENTER(3);

   f = emalloc(sizeof(geist_fill));

   memset(f, 0, sizeof(geist_fill));

   D_RETURN(3, f);
}

void
geist_fill_free(geist_fill * f)
{
   D_ENTER(3);

   efree(f);

   D_RETURN_(3);
}

geist_fill *
geist_fill_new_coloured(int r, int g, int b, int a)
{
   geist_fill *f;
   D_ENTER(3);

   f = geist_fill_new();
   f->r = r;
   f->g = g;
   f->b = b;
   f->a = a;
   
   D_RETURN(3, f);
}

void
geist_fill_render(geist_fill * fill, Imlib_Image dest, int x, int y, int w,
                  int h)
{
   D_ENTER(3);

   gib_imlib_image_fill_rectangle(dest, x, y, w, h, fill->r, fill->g, fill->b,
                              fill->a);

   D_RETURN_(3);
}
