/* alb_text.h

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

#ifndef ALB_TEXT_H
#define ALB_TEXT_H

#include "alb.h"
#include "alb_object.h"
#include "alb.h"
#include "alb_image.h"
#include "alb_imlib.h"

#define ALB_TEXT(O) ((alb_text *) O)

#define TEXT_LINE_SPACING 3

enum text_justification {
  JUST_LEFT,
  JUST_RIGHT,
  JUST_CENTER,
  JUST_BLOCK,
  JUST_MAX
};

struct __alb_text {
  alb_object object;
  char *fontname;
  int fontsize;
  char *text;
  gib_list *lines;
  Imlib_Font fn;
  Imlib_Image im;
  int wordwrap;
  int justification;
  gib_style *style;
  int r, g, b, a;
};

alb_object *alb_text_new(void);
alb_object *alb_text_new_with_text(int x,
                                   int y,
                                   char *fontname,
                                   int fontsize,
                                   char *text,
                                   int justification,
                                   unsigned char wordwrap,
                                   int a,
                                   int r,
                                   int g,
                                   int b);
void alb_text_init(alb_text * txt);
void alb_text_free(alb_object * obj);
void alb_text_render(alb_object * obj,
                     Imlib_Image dest);
void alb_text_render_partial(alb_object * obj,
                             Imlib_Image dest,
                             int x,
                             int y,
                             int w,
                             int h);
void alb_text_change_text(alb_text * obj,
                          char *newtext);
void alb_text_create_image(alb_text * txt);
Imlib_Image alb_text_get_rendered_image(alb_object * obj);
alb_object *alb_text_duplicate(alb_object * obj);
void alb_text_resize(alb_object * obj,
                     int x,
                     int y);
GtkWidget *alb_text_display_props(alb_object * obj);
int alb_text_change_font(alb_text * txt,
                         char *fontname,
                         int fontsize);
void alb_text_free_lines(alb_text * txt);
void alb_text_calculate_lines(alb_text * txt);
char *alb_text_get_justification_string(int just);
int
  alb_text_get_justification_from_string(char *s);
void
  alb_text_update_image(alb_text * txt,
                        unsigned char resize);
void alb_text_update_positioning(alb_object * obj);

void alb_display_fontstyler_window(GtkWidget * widget,
                                   gpointer * obj);
void alb_hide_fontstyler_window(void);
void alb_update_fontstyler_window(alb_object * obj);
void alb_clear_fontstyler_window(void);




#endif
