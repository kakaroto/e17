/* geist_object.h

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

#ifndef GEIST_OBJECT_H
#define GEIST_OBJECT_H

#include "geist.h"
#include "geist_document.h"


extern GtkWidget *obj_list;

#define GEIST_OBJECT(O) ((geist_object *) O)
#define GEIST_OBJECT_DOC(o) (((geist_object *)o)->layer->doc)


typedef enum __geist_object_type
{
   GEIST_TYPE_OBJECT,
   GEIST_TYPE_IMAGE,
   GEIST_TYPE_TEXT,
   GEIST_TYPE_RECT,
   GEIST_TYPE_LINE,
   GEIST_TYPE_POLY,
   GEIST_TYPE_MAX
}
geist_object_type;


enum __sizemode
{
   SIZEMODE_NONE,
   SIZEMODE_ZOOM,
   SIZEMODE_STRETCH,
   SIZEMODE_MAX
};

enum __alignment
{
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


enum __resize_type
{
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

typedef enum __geist_object_state
{ SELECTED = 1UL << 0, HILITED = 1UL << 1, DRAG = 1UL << 2, RESIZE =
      1UL << 3, VISIBLE = 1UL << 4
}
geist_object_state;

struct __geist_object
{
   geist_object_type type;
   char *name;
   geist_layer *layer;
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
   void (*free) (geist_object * obj);
   void (*render) (geist_object * obj, Imlib_Image im);
   void (*render_selected) (geist_object * obj, Imlib_Image im,
                            unsigned char multiple);
   void (*render_partial) (geist_object * obj, Imlib_Image im, int x, int y,
                           int w, int h);
     Imlib_Image(*get_rendered_image) (geist_object * obj);
     Imlib_Updates(*get_selection_updates) (geist_object * obj);
   geist_object *(*duplicate) (geist_object * obj);
   unsigned char (*part_is_transparent) (geist_object * obj, int x, int y);
   void (*resize_event) (geist_object * obj, int x, int y);
   GtkWidget *(*display_props) (geist_object * obj);
   void (*get_rendered_area) (geist_object * obj, int *x, int *y, int *w,
                              int *h);
   int (*check_resize_click) (geist_object * obj, int x, int y);
   void (*get_resize_box_coords) (geist_object * obj, int resize, int *x,
                                  int *y);
   unsigned char (*click_is_selection) (geist_object * obj, int x, int y);
     Imlib_Updates(*get_updates) (geist_object * obj);
	void (*update_dimensions_relative) (geist_object *obj, int w, int h);
	void (*update_position_relative) (geist_object *obj, int x, int y);
    void (*update_positioning)(geist_object * obj);
    char *(*get_object_list_entry)(geist_object *obj, int column);
    unsigned char (*has_transparency)(geist_object *obj);
    void (*rotate)(geist_object *obj, double angle);
    void (*move)(geist_object * obj, int x, int y);
};

/* allocation functions */
geist_object *geist_object_new(void);
void geist_object_init(geist_object * obj);
void geist_object_free(geist_object * obj);

geist_object_type geist_object_get_type(geist_object * obj);
void geist_object_set_type(geist_object * obj, geist_object_type type);
void geist_object_render(geist_object * obj, Imlib_Image dest);
void geist_object_render_selected(geist_object * obj, Imlib_Image dest,

                                  unsigned char multiple);
void geist_object_render_partial(geist_object * obj, Imlib_Image dest, int x,
                                 int y, int w, int h);
void geist_object_show(geist_object * obj);
void geist_object_hide(geist_object * obj);
void geist_object_raise(geist_object * obj);
void geist_object_int_free(geist_object * obj);
void geist_object_int_render(geist_object * obj, Imlib_Image dest);
void geist_object_int_render_selected(geist_object * obj, Imlib_Image dest,

                                      unsigned char multiple);
void geist_object_int_render_partial(geist_object * obj, Imlib_Image dest,
                                     int x, int y, int w, int h);
void geist_object_add_to_object_list(geist_object * obj);
Imlib_Image geist_object_get_rendered_image(geist_object * obj);
Imlib_Image geist_object_int_get_rendered_image(geist_object * obj);
void geist_object_select(geist_object * obj);
Imlib_Updates geist_object_int_get_selection_updates(geist_object * obj);
Imlib_Updates geist_object_get_selection_updates(geist_object * obj);
void geist_object_unselect(geist_object * obj);
geist_object *geist_object_duplicate(geist_object * obj);
geist_object *geist_object_int_duplicate(geist_object * obj);
unsigned char geist_object_part_is_transparent(geist_object * obj, int x,

                                               int y);
void geist_object_move(geist_object * obj, int x, int y);
void geist_object_int_move(geist_object * obj, int x, int y);
unsigned char geist_object_int_part_is_transparent(geist_object * obj, int x,

                                                   int y);
int geist_object_check_resize_click(geist_object * obj, int x, int y);
void geist_object_resize(geist_object * obj, int x, int y);
void geist_object_int_resize(geist_object * obj, int x, int y);
void geist_object_resize_object(geist_object * obj, int x, int y);

GtkWidget *geist_object_int_display_props(geist_object * obj);
void geist_object_dirty_selection(geist_object * obj);
void geist_object_dirty(geist_object * obj);

void geist_object_show_properties(geist_object * obj);
void geist_object_update_sizemode(geist_object * obj);
void geist_object_update_alignment(geist_object * obj);
void geist_object_get_resize_box_coords(geist_object * obj, int resize,
                                        int *x, int *y);
char *geist_object_get_type_string(geist_object * obj);
char *geist_object_get_sizemode_string(geist_object * obj);
char *geist_object_get_alignment_string(geist_object * obj);
int geist_object_get_sizemode_from_string(char *s);
int geist_object_get_alignment_from_string(char *s);
int geist_object_get_type_from_string(char *s);
GtkWidget *geist_object_generic_properties(gib_list * list);

void geist_object_debug_print_values(geist_object * obj);
void geist_object_get_rendered_area(geist_object * obj, int *x, int *y,
                                    int *w, int *h);
void geist_object_int_get_rendered_area(geist_object * obj, int *x, int *y,
                                        int *w, int *h);


void geist_object_get_clipped_render_areas(geist_object * obj, int x, int y,
                                           int w, int h, int *sx, int *sy,
                                           int *sw, int *sh, int *dx, int *dy,
                                           int *dw, int *dh);
int geist_object_int_check_resize_click(geist_object * obj, int x, int y);
void geist_object_int_get_resize_box_coords(geist_object * obj, int resize,
                                            int *x, int *y);
unsigned char geist_object_click_is_selection(geist_object * obj, int x,

                                              int y);
unsigned char geist_object_int_click_is_selection(geist_object * obj, int x,

                                                  int y);
Imlib_Updates geist_object_int_get_updates(geist_object * obj);

void geist_object_update_position_relative(geist_object *obj, int x, int y);
void geist_object_update_dimensions_relative(geist_object *obj, int w, int h);
void geist_object_int_update_position_relative(geist_object *obj, int x, int y);
void geist_object_int_update_dimensions_relative(geist_object *obj, int w, int h);
void geist_object_update_sizemode(geist_object * obj);
void geist_object_int_update_positioning(geist_object * obj);
void geist_object_update_positioning(geist_object *obj);
char *geist_object_get_object_list_entry(geist_object *obj, int column);
char *geist_object_int_get_object_list_entry(geist_object *obj, int column);
unsigned char geist_object_has_transparency (geist_object *obj);
unsigned char geist_object_int_has_transparency (geist_object *obj);
void geist_object_rotate(geist_object *obj, double angle);
void geist_object_int_rotate(geist_object *obj, double angle);




#define geist_object_set_state(o, s) (o->state |=  s)
#define geist_object_unset_state(o, s) (o->state &= ~(s))
#define geist_object_get_state(o, s) (o->state & s)
#define geist_object_toggle_state(o, s) ((o->state & s) ? (o->state &= ~(s)) : (o->state |=  s))


#endif
