#ifndef GEIST_rect_H
#define GEIST_rect_H

#include "geist.h"
#include "geist_object.h"
#include "geist.h"
#include "geist_imlib.h"
#include "geist_image.h"

#define GEIST_RECT(O) ((geist_rect *) O)

struct __geist_rect
{
   geist_object object;
   char *name;
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
void geist_rect_display_props(geist_object * obj);

#endif
