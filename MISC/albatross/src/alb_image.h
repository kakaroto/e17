/* alb_image.h

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

#ifndef ALB_IMAGE_H
#define ALB_IMAGE_H

#include "alb.h"
#include "alb_imlib.h"
#include "alb_object.h"

#define ALB_IMAGE(O) ((alb_image *) O)
#define FULL_OPACITY 100
#define FULL_COLOR 100

#define R 0
#define G 1
#define B 2
#define A 3

struct __alb_image {
  alb_object object;
  char *filename;
  Imlib_Image im, orig_im;
  struct {
    int image_mods[4];
  } last;
  int image_mods[4];
};

alb_object *alb_image_new(void);
alb_object *alb_image_new_from_file(int x,
                                    int y,
                                    char *filename);
void alb_image_init(alb_image * img);
void alb_image_free(alb_object * obj);
void alb_image_render(alb_object * obj,
                      Imlib_Image dest);
void alb_image_render_partial(alb_object * obj,
                              Imlib_Image dest,
                              int x,
                              int y,
                              int w,
                              int h);
void refresh_image_opacity_cb(GtkWidget * widget,
                              gpointer * obj);
int alb_image_load_file(alb_image * img,
                        char *filename);
Imlib_Image alb_image_get_rendered_image(alb_object * obj);
alb_object *alb_image_duplicate(alb_object * obj);
void alb_image_resize(alb_object * obj,
                      int x,
                      int y);
GtkWidget *alb_image_display_props(alb_object * obj);
void alb_image_change_image_mods(alb_object * obj,
                                 int p[]);
void alb_image_apply_image_mods(alb_object * obj);
void alb_image_update_sizemode(alb_object * obj);
void alb_image_update_positioning(alb_object * obj);
void alb_image_rotate(alb_object * obj,
                      double angle);
char *alb_image_get_object_list_entry(alb_object * obj,
                                      int column);
unsigned char alb_image_has_transparency(alb_object * obj);

#endif
