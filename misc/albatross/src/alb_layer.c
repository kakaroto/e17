/* alb_layer.c

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

#include "alb_layer.h"

alb_layer *
alb_layer_new(void)
{
  alb_layer *l = NULL;

  D_ENTER(3);

  l = emalloc(sizeof(alb_layer));

  memset(l, 0, sizeof(alb_layer));

  l->visible = TRUE;
  l->name = estrdup("New layer");

  D_RETURN(3, l);
}

void
alb_layer_free(alb_layer * layer)
{
  gib_list *l;

  D_ENTER(3);

  for (l = layer->objects; l; l = l->next)
    alb_object_free(((alb_object *) l->data));

  gib_list_free(layer->objects);

  efree(layer);

  D_RETURN_(3);
}

void
alb_layer_render(alb_layer * layer,
                 Imlib_Image dest)
{
  gib_list *l;

  D_ENTER(3);

  if (layer->visible) {
    D(3, ("rendering layer %p\n", layer));
    for (l = layer->objects; l; l = l->next)
      alb_object_render(((alb_object *) l->data), dest);
  }

  D_RETURN_(3);
}

void
alb_layer_render_partial(alb_layer * layer,
                         Imlib_Image dest,
                         int x,
                         int y,
                         int w,
                         int h)
{
  gib_list *l;
  alb_object *obj;
  int ox, oy, ow, oh;

  D_ENTER(3);

  if (layer->visible) {
    D(3, ("rendering layer %p\n", layer));
    for (l = layer->objects; l; l = l->next) {
      obj = ((alb_object *) l->data);
      alb_object_get_rendered_area(obj, &ox, &oy, &ow, &oh);
      if (RECTS_INTERSECT(x, y, w, h, ox, oy, ow, oh)) {
        D(5,
          ("rects intersect %d,%d %dx%d and %d,%d %dx%d\n", x, y, w, h, ox,
           oy, ow, oh));
        alb_object_render_partial(obj, dest, x, y, w, h);
      }
    }
  }

  D_RETURN_(3);
}


void
alb_layer_add_object(alb_layer * layer,
                     alb_object * obj)
{
  D_ENTER(3);

  if (!obj)
    D_RETURN_(3);

  layer->objects = gib_list_add_end(layer->objects, obj);
  obj->layer = layer;

  D_RETURN_(3);
}

alb_object *
alb_layer_find_clicked_object(alb_layer * layer,
                              int x,
                              int y)
{
  gib_list *l;
  alb_object *ret = NULL, *obj;

  D_ENTER(4);

  for (l = layer->objects; l; l = l->next) {
    obj = ((alb_object *) l->data);
    if (alb_object_get_state(obj, VISIBLE)
        && alb_object_click_is_selection(obj, x, y))
      ret = obj;
  }

  D_RETURN(4, ret);
}

void
alb_layer_raise_object(alb_object * obj)
{
  gib_list *ll;
  alb_object *ob;

  D_ENTER(4);

  for (ll = obj->layer->objects; ll; ll = ll->next) {
    ob = (alb_object *) ll->data;
    if (ob == obj) {
      D(4, ("Found object %p - popping to end of list\n", obj));
      obj->layer->objects = gib_list_pop_to_end(obj->layer->objects, ll);
      D_RETURN_(4);
    }
  }

  D_RETURN_(4);
}

unsigned char
alb_layer_remove_object(alb_layer * lay,
                        alb_object * obj)
{
  gib_list *l;

  D_ENTER(3);

  for (l = lay->objects; l; l = l->next) {
    if (ALB_OBJECT(l->data) == obj) {
      lay->objects = gib_list_unlink(lay->objects, l);
      obj->layer = NULL;
      efree(l);
      D_RETURN(3, TRUE);
    }
  }

  D_RETURN(3, FALSE);
}
