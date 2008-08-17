/* libgozer.h

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

#ifndef LIBGOZER_H
#define LIBGOZER_H

enum __justify
{ JUST_LEFT,
   JUST_RIGHT,
   JUST_CENTER,
   JUST_BLOCK,
   JUST_MAX
};


struct __gozeroptions
{
   char *text;
   char *textfile;
   char *font;
   char *stylefile;
   char *output_file;
   char *background_col;
   char *bg_image;
   char *font_col;
   char *fontpath;
   char *rc_file;
   char pipe;
   char bg_tile;
   char bg_scale;
   char bg_resize;
   int bg_r, bg_g, bg_b, bg_a;
   int fn_r, fn_g, fn_b, fn_a;
   int wrap_width;
   int line_spacing;
   int justification;
   int to_stdout;
   int cgi;
   int quality;
   int x,y;
};

typedef struct __gozeroptions gozeroptions;

#ifdef __cplusplus
extern "C" {
#endif

void default_options(gozeroptions *opt);
Imlib_Image gozer_render(gozeroptions *opt);
void gozer_parse_rc_file(char *file, int user_defined, gozeroptions *opt);

#ifdef __cplusplus
}
#endif

#endif
