#include "Evoak.h"
#include "evoak_private.h"

static void  _evoak_object_layer_del(Evoak_Object *o);
static void  _evoak_object_layer_add(Evoak_Object *o);

static void
_evoak_object_layer_del(Evoak_Object *o)
{
   o->layer_data->objects = evas_list_remove(o->layer_data->objects, o);
   if (!o->layer_data->objects)
     {
        o->evoak->layers = evas_list_remove_list(o->evoak->layers, o->layer_data
->link);
        free(o->layer_data);
     }
   o->layer_data = NULL;
}

static void
_evoak_object_layer_add(Evoak_Object *o)
{
   Evas_List *l, *ll;
   Evoak_Layer *lay;

   for (l = o->evoak->layers; l; l = l->next)
     {
        lay = l->data;
        if (lay->layer == o->layer)
          {
             lay->objects = evas_list_append(lay->objects, o);
             o->link = evas_list_last(lay->objects);
             o->layer_data = lay;
             return;
          }
        else if (lay->layer > o->layer)
          {
             /* insert new layer before */
             lay = calloc(1, sizeof(Evoak_Layer));
             lay->evoak = o->evoak;
             lay->layer = o->layer;
             o->evoak->layers = evas_list_prepend_relative(o->evoak->layers, lay
, l->data);
             lay->link = l->prev;
             lay->objects = evas_list_append(lay->objects, o);
             o->link = evas_list_last(lay->objects);
             o->layer_data = lay;
             return;
          }
     }
   lay = calloc(1, sizeof(Evoak_Layer));
   lay->evoak = o->evoak;
   lay->layer = o->layer;
   o->evoak->layers = evas_list_append(o->evoak->layers, lay);
   lay->link = evas_list_last(o->evoak->layers);
   lay->objects = evas_list_append(lay->objects, o);
   o->link = evas_list_last(lay->objects);
   o->layer_data = lay;
}

void
evoak_object_layer_set(Evoak_Object *o, int l)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Layer_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
   if (o->layer == l)
     {
        evoak_object_raise(o);
        return;
     }
   _evoak_object_layer_del(o);
   o->layer = l;
   _evoak_object_layer_add(o);
   p.layer = l;
   d = _evoak_proto[EVOAK_PR_OBJECT_LAYER_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_LAYER_SET, o->id, 0, d, s);
        free(d);
     }
}

int
evoak_object_layer_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->layer;
}
