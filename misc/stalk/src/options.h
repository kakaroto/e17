/* options.h

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

#ifndef OPTIONS_H
#define OPTIONS_H

struct __stalkoptions
{
   int debug_level;
   int w,h,x,y;
   int max_lines;
   gib_list *files;
   gib_list *lines;
   char *fontstr;
   Imlib_Font font;
   char *stylefile;
   gib_style *style;
   int fn_r, fn_g, fn_b, fn_a;
   int bg_r, bg_g, bg_b, bg_a;
   int wrap;
   int trans;
   char *bg_image;
};

void init_parse_options(int argc, char **argv);
extern stalkoptions opt;
void show_usage(void);
void show_version(void);
void show_mini_usage(void);

#endif
