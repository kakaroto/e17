/* geist_text.h

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

#ifndef GEIST_TEXT_H
#define GEIST_TEXT_H

#include "geist.h"
#include "geist_object.h"
#include "geist.h"
#include "geist_image.h"
#include "geist_imlib.h"

#define GEIST_TEXT(O) ((geist_text *) O)

#define TEXT_LINE_SPACING 3

enum text_justification {
   JUST_LEFT,
   JUST_RIGHT,
   JUST_CENTER,
   JUST_BLOCK,
   JUST_MAX
};

struct __geist_text
{
   geist_object object;
   char *fontname;
   int fontsize;
   char *text;
   gib_list *lines;
   Imlib_Font fn;
   Imlib_Image im;
   int wordwrap;
   int justification;
   gib_style *style;
   int r,g,b,a;
};

geist_object *geist_text_new(void);
geist_object *
geist_text_new_with_text(int x, int y, char *fontname, int fontsize,
                         char *text, int justification,
                         unsigned char wordwrap, int a, int r, int g, int b);
void geist_text_init(geist_text * txt);
void geist_text_free(geist_object * obj);
void geist_text_render(geist_object * obj, Imlib_Image dest);
void geist_text_render_partial(geist_object * obj, Imlib_Image dest, int x,
                               int y, int w, int h);
void geist_text_change_text(geist_text * obj, char *newtext);
void geist_text_create_image(geist_text * txt);
Imlib_Image geist_text_get_rendered_image(geist_object * obj);
geist_object *geist_text_duplicate(geist_object * obj);
void geist_text_resize(geist_object * obj, int x, int y);
GtkWidget* geist_text_display_props (geist_object *obj);
int geist_text_change_font(geist_text * txt, char *fontname, int fontsize);
void geist_text_free_lines(geist_text *txt);
void geist_text_calculate_lines(geist_text *txt);
char *
geist_text_get_justification_string(int just);
int
geist_text_get_justification_from_string(char *s);
void
geist_text_update_image(geist_text * txt, unsigned char resize);
void geist_text_update_positioning(geist_object *obj);

void geist_display_fontstyler_window(GtkWidget *widget, gpointer *obj);
void geist_hide_fontstyler_window(void);
void geist_update_fontstyler_window(geist_object *obj);
void geist_clear_fontstyler_window(void);




#endif
