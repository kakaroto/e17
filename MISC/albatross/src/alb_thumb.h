/* alb_thumb.h

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

#ifndef alb_thumb_H
#define alb_thumb_H

#include "alb.h"
#include "alb_imlib.h"
#include "alb_object.h"

#define alb_thumb(O) ((alb_thumb *) O)
#define FULL_OPACITY 100
#define FULL_COLOR 100

#define R 0
#define G 1
#define B 2
#define A 3

struct __alb_thumb {
  alb_object object;
  char *filename;
  char *cachefilename;
  int cache_invalidate;
  Imlib_Image im, orig_im;
  struct {
    int image_mods[4];
  } last;
  int image_mods[4];
};

alb_object *alb_thumb_new(void);
alb_object *alb_thumb_new_from_file(int x,
                                    int y,
                                    int w,
                                    int h,
                                    char *filename);
void alb_thumb_init(alb_thumb * img);
void alb_thumb_free(alb_object * obj);
void alb_thumb_render(alb_object * obj,
                      Imlib_Image dest);
void alb_thumb_render_partial(alb_object * obj,
                              Imlib_Image dest,
                              int x,
                              int y,
                              int w,
                              int h);
void refresh_image_opacity_cb(GtkWidget * widget,
                              gpointer * obj);
int alb_thumb_load_file(int w,
                        int h,
                        alb_thumb * img,
                        char *filename);
Imlib_Image alb_thumb_get_rendered_image(alb_object * obj);
alb_object *alb_thumb_duplicate(alb_object * obj);
void alb_thumb_resize(alb_object * obj,
                      int x,
                      int y);
GtkWidget *alb_thumb_display_props(alb_object * obj);
void alb_thumb_change_image_mods(alb_object * obj,
                                 int p[]);
void alb_thumb_apply_image_mods(alb_object * obj);
void alb_thumb_update_sizemode(alb_object * obj);
void alb_thumb_update_positioning(alb_object * obj);
void alb_thumb_rotate(alb_object * obj,
                      double angle);
char *alb_thumb_get_object_list_entry(alb_object * obj,
                                      int column);
unsigned char alb_thumb_has_transparency(alb_object * obj);
void alb_thumb_save_to_cache(alb_thumb * t);
char *alb_thumb_get_cache_filename(alb_thumb * t);
void alb_thumb_resize_done(alb_object *obj);
void alb_thumb_reload_file(alb_thumb *t);
gib_list *
alb_thumb_tesselate_constrain_w(int w, int *h, int item_w, int item_h, int h_space, int v_space, int count);

#endif
