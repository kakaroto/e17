/* geist_image.h

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

#ifndef GEIST_IMAGE_H
#define GEIST_IMAGE_H

#include "geist.h"
#include "geist_imlib.h"
#include "geist_object.h"

#define GEIST_IMAGE(O) ((geist_image *) O)
#define FULL_OPACITY 100
#define FULL_COLOR 100

#define R 0
#define G 1
#define B 2
#define A 3

struct __geist_image
{
   geist_object object;
   char *filename;
   Imlib_Image im, orig_im;
   struct {
      int image_mods[4];
   } last;
   int image_mods[4];
};

geist_object *geist_image_new(void);
geist_object *geist_image_new_from_file(int x, int y, char *filename);
void geist_image_init(geist_image * img);
void geist_image_free(geist_object * obj);
void geist_image_render(geist_object * obj, Imlib_Image dest);
void geist_image_render_partial(geist_object * obj, Imlib_Image dest, int x,
                                int y, int w, int h);
void refresh_image_opacity_cb(GtkWidget * widget, gpointer * obj);
int geist_image_load_file(geist_image * img, char *filename);
Imlib_Image geist_image_get_rendered_image(geist_object * obj);
geist_object *geist_image_duplicate(geist_object * obj);
void geist_image_resize(geist_object * obj, int x, int y);
GtkWidget * geist_image_display_props(geist_object *obj);
void geist_image_change_image_mods(geist_object * obj, int p[]);
void geist_image_apply_image_mods(geist_object * obj);
void geist_image_update_sizemode(geist_object * obj);
void geist_image_update_positioning(geist_object *obj);
void geist_image_rotate(geist_object *obj, double angle);
char *geist_image_get_object_list_entry(geist_object * obj, int column);
unsigned char geist_image_has_transparency(geist_object * obj);

#endif
