/* thumbnail.h

Copyright (C) 1999-2003 Tom Gilbert.

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

#ifndef THIMBNAIL_H
#define THIMBNAIL_H


#include "feh.h"
#include "filelist.h"
#include "winwidget.h"

#define FEH_THUMB(l) ((feh_thumbnail *) l)

typedef struct thumbnail
{
   int x;
   int y;
   int w;
   int h;
   feh_file *file;
   unsigned char exists;
   struct feh_thumbnail *next;
}
feh_thumbnail;


feh_thumbnail *feh_thumbnail_new(feh_file * fil, int x, int y, int w, int h);
feh_file *feh_thumbnail_get_file_from_coords(int x, int y);
feh_thumbnail *feh_thumbnail_get_thumbnail_from_coords(int x, int y);
feh_thumbnail *feh_thumbnail_get_from_file(feh_file * file);
void feh_thumbnail_mark_removed(feh_file * file, int deleted);

#endif
