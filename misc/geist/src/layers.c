#include "layers.h"

geist_layer *
geist_layer_new(void)
{
   geist_layer *l = NULL;

   D_ENTER(3);

   l = emalloc(sizeof(geist_layer));

   memset(l, 0, sizeof(geist_layer));

   l->visible = TRUE;

   D_RETURN(3, l);
}

void
geist_layer_free(geist_layer * layer)
{
   geist_list *l;

   D_ENTER(3);

   for (l = layer->objects; l; l = l->next)
      geist_object_free(((geist_object *) l->data));

   geist_list_free(layer->objects);

   efree(layer);

   D_RETURN_(3);
}

void
geist_layer_render(geist_layer * layer, Imlib_Image dest)
{
   geist_list *l;

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
   geist_list *l;
   geist_object *obj;

   D_ENTER(3);

   if (layer->visible)
   {
      D(3, ("rendering layer %p\n", layer));
      for (l = layer->objects; l; l = l->next)
      {
         obj = ((geist_object *) l->data);
         if (RECTS_INTERSECT
             (x, y, w, h, obj->x + obj->rendered_x, obj->y + obj->rendered_y,
              obj->rendered_w, obj->rendered_h))
         {
            D(4,
              ("rects intersect: %d,%d %dx%d   and   %d,%d %dx%d\n", x, y, w,
               h, obj->x, obj->y, obj->w, obj->h));
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

   layer->objects = geist_list_add_end(layer->objects, obj);
   obj->layer = layer;

   D_RETURN_(3);
}

geist_object *
geist_layer_find_clicked_object(geist_layer * layer, int x, int y)
{
   geist_list *l;
   geist_object *ret = NULL, *obj;

   D_ENTER(4);

   for (l = layer->objects; l; l = l->next)
   {
      obj = ((geist_object *) l->data);
      if (geist_object_get_state(obj, VISIBLE)
          &&
          (XY_IN_RECT
           (x, y, obj->x + obj->rendered_x, obj->y + obj->rendered_y,
            obj->rendered_w, obj->rendered_h)))
      {
         if (!geist_object_part_is_transparent
             (obj, x - (obj->x + obj->rendered_x),
              y - (obj->y + obj->rendered_y)))
            ret = obj;
      }
   }

   D_RETURN(4, ret);
}

void
geist_layer_raise_object(geist_object * obj)
{
   geist_list *ll;
   geist_object *ob;

   D_ENTER(4);

   for (ll = obj->layer->objects; ll; ll = ll->next)
   {
      ob = (geist_object *) ll->data;
      if (ob == obj)
      {
         D(4, ("Found object %p - popping to end of list\n", obj));
         obj->layer->objects = geist_list_pop_to_end(obj->layer->objects, ll);
         D_RETURN_(4);
      }
   }

   D_RETURN_(4);
}

unsigned char
geist_layer_remove_object(geist_layer * lay, geist_object * obj)
{
   geist_list *l;

   D_ENTER(3);

   for (l = lay->objects; l; l = l->next)
   {
      if (GEIST_OBJECT(l->data) == obj)
      {
         lay->objects = geist_list_unlink(lay->objects, l);
         obj->layer = NULL;
         efree(l);
         D_RETURN(3, TRUE);
      }
   }

   D_RETURN(3, FALSE);
}
