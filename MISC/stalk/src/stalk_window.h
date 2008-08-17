/* stalk_window.h

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

#ifndef STALK_WINDOW_H
#define STALK_WINDOW_H

#include "stalk.h"
#include "structs.h"

struct __stalk_window
{
   GtkWidget *win, *darea, *scrollbar;
   GtkObject *adjustment;
   Pixmap pmap;
   Imlib_Image im, bg_im;
   gib_list *lines;
   int x,y,im_w, im_h;
   int offset;
};

stalk_window *stalk_window_new(int x, int y, int w, int h);
void stalk_window_set_background(stalk_window *win);
void stalk_window_set_background_partial(stalk_window *win, int x, int y,
                                        int w, int h);
void stalk_window_draw(stalk_window *win);
gib_list *stalk_generate_wrapped_lines(stalk_line *line, int wrap_width);
gboolean stalk_window_configure_event(GtkWidget * widget, GdkEventConfigure  *event, gpointer * data);
void stalk_window_change_offset(stalk_window *win, int offset);

#define STALK_WINDOW(l) ((stalk_window *) l)

#endif
