/* alb_object.h

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

#ifndef ALB_OBJECT_H
#define ALB_OBJECT_H

#include "alb.h"
#include "alb_document.h"


#define ALB_OBJECT(O) ((alb_object *) O)
#define ALB_OBJECT_DOC(o) (((alb_object *)o)->layer->doc)


typedef enum __alb_object_type {
  ALB_TYPE_OBJECT,
  ALB_TYPE_IMAGE,
  ALB_TYPE_TEXT,
  ALB_TYPE_RECT,
  ALB_TYPE_LINE,
  ALB_TYPE_POLY,
  ALB_TYPE_MAX
} alb_object_type;


enum __sizemode {
  SIZEMODE_NONE,
  SIZEMODE_ZOOM,
  SIZEMODE_STRETCH,
  SIZEMODE_MAX
};

enum __alignment {
  ALIGN_NONE,
  ALIGN_HCENTER,
  ALIGN_VCENTER,
  ALIGN_CENTER,
  ALIGN_LEFT,
  ALIGN_RIGHT,
  ALIGN_TOP,
  ALIGN_BOTTOM,
  ALIGN_MAX
};


enum __resize_type {
  RESIZE_NONE,
  RESIZE_LEFT,
  RESIZE_TOPLEFT,
  RESIZE_TOP,
  RESIZE_TOPRIGHT,
  RESIZE_RIGHT,
  RESIZE_BOTTOMRIGHT,
  RESIZE_BOTTOM,
  RESIZE_BOTTOMLEFT
};

typedef enum __alb_object_state { SELECTED = 1UL << 0, HILITED =
    1UL << 1, DRAG = 1UL << 2, RESIZE = 1UL << 3, VISIBLE = 1UL << 4
} alb_object_state;

struct __alb_object {
  alb_object_type type;
  char *name;
  alb_layer *layer;
  struct {
    int alias;
  } last;
  int alias;
  /* Object position/size */
  int x;
  int y;
  int w;
  int h;
  /* x,y offset of rendered image within object */
  int rendered_x;
  int rendered_y;
  /* Actual size of rendered image/shape */
  int rendered_w;
  int rendered_h;
  /* where was the object clicked? (for dragging etc) */
  int clicked_x;
  int clicked_y;
  /* type of resize being performed */
  int resize;
  /* sizing mode */
  int sizemode;
  /* alignment */
  int alignment;
  /* object state */
  unsigned long int state;
  void (*free) (alb_object * obj);
  void (*render) (alb_object * obj,
                  Imlib_Image im);
  void (*render_selected) (alb_object * obj,
                           Imlib_Image im,
                           unsigned char multiple);
  void (*render_partial) (alb_object * obj,
                          Imlib_Image im,
                          int x,
                          int y,
                          int w,
                          int h);
    Imlib_Image(*get_rendered_image) (alb_object * obj);
    Imlib_Updates(*get_selection_updates) (alb_object * obj);
  alb_object *(*duplicate) (alb_object * obj);
  unsigned char (*part_is_transparent) (alb_object * obj,
                                        int x,
                                        int y);
  void (*resize_event) (alb_object * obj,
                        int x,
                        int y);
  GtkWidget *(*display_props) (alb_object * obj);
  void (*get_rendered_area) (alb_object * obj,
                             int *x,
                             int *y,
                             int *w,
                             int *h);
  int (*check_resize_click) (alb_object * obj,
                             int x,
                             int y);
  void (*get_resize_box_coords) (alb_object * obj,
                                 int resize,
                                 int *x,
                                 int *y);
  unsigned char (*click_is_selection) (alb_object * obj,
                                       int x,
                                       int y);
    Imlib_Updates(*get_updates) (alb_object * obj);
  void (*update_dimensions_relative) (alb_object * obj,
                                      int w,
                                      int h);
  void (*update_position_relative) (alb_object * obj,
                                    int x,
                                    int y);
  void (*update_positioning) (alb_object * obj);
  char *(*get_object_list_entry) (alb_object * obj,
                                  int column);
  unsigned char (*has_transparency) (alb_object * obj);
  void (*rotate) (alb_object * obj,
                  double angle);
  void (*move) (alb_object * obj,
                int x,
                int y);
  void (*resize_done) (alb_object * obj);
};

/* allocation functions */
alb_object *alb_object_new(void);
void alb_object_init(alb_object * obj);
void alb_object_free(alb_object * obj);

alb_object_type alb_object_get_type(alb_object * obj);
void alb_object_set_type(alb_object * obj,
                         alb_object_type type);
void alb_object_render(alb_object * obj,
                       Imlib_Image dest);
void alb_object_render_selected(alb_object * obj,
                                Imlib_Image dest,
                                unsigned char multiple);
void alb_object_render_partial(alb_object * obj,
                               Imlib_Image dest,
                               int x,
                               int y,
                               int w,
                               int h);
void alb_object_show(alb_object * obj);
void alb_object_hide(alb_object * obj);
void alb_object_raise(alb_object * obj);
void alb_object_int_free(alb_object * obj);
void alb_object_int_render(alb_object * obj,
                           Imlib_Image dest);
void alb_object_int_render_selected(alb_object * obj,
                                    Imlib_Image dest,
                                    unsigned char multiple);
void alb_object_int_render_partial(alb_object * obj,
                                   Imlib_Image dest,
                                   int x,
                                   int y,
                                   int w,
                                   int h);
Imlib_Image alb_object_get_rendered_image(alb_object * obj);
Imlib_Image alb_object_int_get_rendered_image(alb_object * obj);
void alb_object_select(alb_object * obj);
Imlib_Updates alb_object_int_get_selection_updates(alb_object * obj);
Imlib_Updates alb_object_get_selection_updates(alb_object * obj);
void alb_object_unselect(alb_object * obj);
alb_object *alb_object_duplicate(alb_object * obj);
alb_object *alb_object_int_duplicate(alb_object * obj);
unsigned char alb_object_part_is_transparent(alb_object * obj,
                                             int x,
                                             int y);
void alb_object_move(alb_object * obj,
                     int x,
                     int y);
void alb_object_int_move(alb_object * obj,
                         int x,
                         int y);
unsigned char alb_object_int_part_is_transparent(alb_object * obj,
                                                 int x,
                                                 int y);
int alb_object_check_resize_click(alb_object * obj,
                                  int x,
                                  int y);
void alb_object_resize(alb_object * obj,
                       int x,
                       int y);
void alb_object_int_resize(alb_object * obj,
                           int x,
                           int y);
void alb_object_resize_object(alb_object * obj,
                              int x,
                              int y);

GtkWidget *alb_object_int_display_props(alb_object * obj);
void alb_object_dirty_selection(alb_object * obj);
void alb_object_dirty(alb_object * obj);

void alb_object_show_properties(alb_object * obj);
void alb_object_update_sizemode(alb_object * obj);
void alb_object_update_alignment(alb_object * obj);
void alb_object_get_resize_box_coords(alb_object * obj,
                                      int resize,
                                      int *x,
                                      int *y);
char *alb_object_get_type_string(alb_object * obj);
char *alb_object_get_sizemode_string(alb_object * obj);
char *alb_object_get_alignment_string(alb_object * obj);
int alb_object_get_sizemode_from_string(char *s);
int alb_object_get_alignment_from_string(char *s);
int alb_object_get_type_from_string(char *s);
GtkWidget *alb_object_generic_properties(gib_list * list);

void alb_object_debug_print_values(alb_object * obj);
void alb_object_get_rendered_area(alb_object * obj,
                                  int *x,
                                  int *y,
                                  int *w,
                                  int *h);
void alb_object_int_get_rendered_area(alb_object * obj,
                                      int *x,
                                      int *y,
                                      int *w,
                                      int *h);


void alb_object_get_clipped_render_areas(alb_object * obj,
                                         int x,
                                         int y,
                                         int w,
                                         int h,
                                         int *sx,
                                         int *sy,
                                         int *sw,
                                         int *sh,
                                         int *dx,
                                         int *dy,
                                         int *dw,
                                         int *dh);
int alb_object_int_check_resize_click(alb_object * obj,
                                      int x,
                                      int y);
void alb_object_int_get_resize_box_coords(alb_object * obj,
                                          int resize,
                                          int *x,
                                          int *y);
unsigned char alb_object_click_is_selection(alb_object * obj,
                                            int x,
                                            int y);
unsigned char alb_object_int_click_is_selection(alb_object * obj,
                                                int x,
                                                int y);
Imlib_Updates alb_object_int_get_updates(alb_object * obj);

void alb_object_update_position_relative(alb_object * obj,
                                         int x,
                                         int y);
void alb_object_update_dimensions_relative(alb_object * obj,
                                           int w,
                                           int h);
void alb_object_int_update_position_relative(alb_object * obj,
                                             int x,
                                             int y);
void alb_object_int_update_dimensions_relative(alb_object * obj,
                                               int w,
                                               int h);
void alb_object_update_sizemode(alb_object * obj);
void alb_object_int_update_positioning(alb_object * obj);
void alb_object_update_positioning(alb_object * obj);
char *alb_object_get_object_list_entry(alb_object * obj,
                                       int column);
char *alb_object_int_get_object_list_entry(alb_object * obj,
                                           int column);
unsigned char alb_object_has_transparency(alb_object * obj);
unsigned char alb_object_int_has_transparency(alb_object * obj);
void alb_object_rotate(alb_object * obj,
                       double angle);
void alb_object_int_rotate(alb_object * obj,
                           double angle);
void alb_object_resize_done (alb_object * obj);
void alb_object_int_resize_done (alb_object * obj);




#define alb_object_set_state(o, s) (o->state |=  s)
#define alb_object_unset_state(o, s) (o->state &= ~(s))
#define alb_object_get_state(o, s) (o->state & s)
#define alb_object_toggle_state(o, s) ((o->state & s) ? (o->state &= ~(s)) : (o->state |=  s))


#endif
