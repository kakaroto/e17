#ifndef GEIST_TEXT_H
#define GEIST_TEXT_H

#include "geist.h"
#include "geist_object.h"
#include "geist.h"
#include "geist_imlib.h"
#include "geist_image.h"

#define GEIST_TEXT(O) ((geist_text *) O)

#define TEXT_LINE_SPACING 3

enum text_justification {
   JUST_LEFT,
   JUST_RIGHT,
   JUST_CENTER,
   JUST_MAX
};

struct __geist_text
{
   geist_object object;
   char *fontname;
   int fontsize;
   char *text;
   geist_list *lines;
   Imlib_Font fn;
   Imlib_Image im;
   int r, g, b, a;
   int wordwrap;
   int justification;
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

#endif
