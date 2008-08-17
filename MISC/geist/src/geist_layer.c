/* geist_layer.c

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

#include "geist_layer.h"

geist_layer *
geist_layer_new(void)
{
   geist_layer *l = NULL;

   D_ENTER(3);

   l = emalloc(sizeof(geist_layer));

   memset(l, 0, sizeof(geist_layer));

   l->visible = TRUE;
   l->name = estrdup("New layer");

   D_RETURN(3, l);
}

void
geist_layer_free(geist_layer * layer)
{
   gib_list *l;

   D_ENTER(3);

   for (l = layer->objects; l; l = l->next)
      geist_object_free(((geist_object *) l->data));

   gib_list_free(layer->objects);

   efree(layer);

   D_RETURN_(3);
}

void
geist_layer_render(geist_layer * layer, Imlib_Image dest)
{
   gib_list *l;

   D_ENTER(3);

   if (layer->visible)
   {
      D(3, ("rendering layer %p\n", layer));
      for (l = layer->objects; l; l = l->next)
         geist_object_render(((geist_object *) l->data), dest);
   }

   D_RETURN_(3);
}

void
geist_layer_render_partial(geist_layer * layer, Imlib_Image dest, int x,
                           int y, int w, int h)
{
   gib_list *l;
   geist_object *obj;
   int ox, oy, ow, oh;

   D_ENTER(3);

   if (layer->visible)
   {
      D(3, ("rendering layer %p\n", layer));
      for (l = layer->objects; l; l = l->next)
      {
         obj = ((geist_object *) l->data);
         geist_object_get_rendered_area(obj, &ox, &oy, &ow, &oh);
         if (RECTS_INTERSECT(x, y, w, h, ox, oy, ow, oh))
         {
            D(5,
              ("rects intersect %d,%d %dx%d and %d,%d %dx%d\n", x, y, w, h,
               ox, oy, ow, oh));
            geist_object_render_partial(obj, dest, x, y, w, h);
         }
      }
   }

   D_RETURN_(3);
}


void
geist_layer_add_object(geist_layer * layer, geist_object * obj)
{
   D_ENTER(3);

   if (!obj)
      D_RETURN_(3);

   layer->objects = gib_list_add_end(layer->objects, obj);
   obj->layer = layer;

   D_RETURN_(3);
}

geist_object *
geist_layer_find_clicked_object(geist_layer * layer, int x, int y)
{
   gib_list *l;
   geist_object *ret = NULL, *obj;

   D_ENTER(4);

   for (l = layer->objects; l; l = l->next)
   {
      obj = ((geist_object *) l->data);
      if (geist_object_get_state(obj, VISIBLE)
          && geist_object_click_is_selection(obj, x, y))
         ret = obj;
   }

   D_RETURN(4, ret);
}

void
geist_layer_raise_object(geist_object * obj)
{
   gib_list *ll;
   geist_object *ob;

   D_ENTER(4);

   for (ll = obj->layer->objects; ll; ll = ll->next)
   {
      ob = (geist_object *) ll->data;
      if (ob == obj)
      {
         D(4, ("Found object %p - popping to end of list\n", obj));
         obj->layer->objects = gib_list_pop_to_end(obj->layer->objects, ll);
         D_RETURN_(4);
      }
   }

   D_RETURN_(4);
}

unsigned char
geist_layer_remove_object(geist_layer * lay, geist_object * obj)
{
   gib_list *l;

   D_ENTER(3);

   for (l = lay->objects; l; l = l->next)
   {
      if (GEIST_OBJECT(l->data) == obj)
      {
         lay->objects = gib_list_unlink(lay->objects, l);
         obj->layer = NULL;
         efree(l);
         D_RETURN(3, TRUE);
      }
   }

   D_RETURN(3, FALSE);
}
