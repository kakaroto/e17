void
evoak_object_raise(Evoak_Object *o)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Stack p;
   
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
//   if (!o->link->next) return;
   o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->lin
k);
   o->layer_data->objects = evas_list_append(o->layer_data->objects, o);
   o->link = evas_list_last(o->layer_data->objects);
   p.relative_id = 0;
   p.relative = EVOAK_RELATIVE_ABOVE;
   d = _evoak_proto[EVOAK_PR_OBJECT_STACK].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_STACK, o->id, 0, d, s);
        free(d);
     }
}

void
evoak_object_lower(Evoak_Object *o)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Stack p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
//   if (!o->link->prev) return;
   o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->lin
k);
   o->layer_data->objects = evas_list_append(o->layer_data->objects, o);
   o->link = o->layer_data->objects;
   p.relative_id = 0;
   p.relative = EVOAK_RELATIVE_BELOW;
   d = _evoak_proto[EVOAK_PR_OBJECT_STACK].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_STACK, o->id, 0, d, s);
        free(d);
     }
}

void
evoak_object_stack_above(Evoak_Object *o, Evoak_Object *above)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Stack p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
   if (!above)
     {
        evoak_object_raise(o);
        return;
     }
   if (above->magic != EVOAK_OBJECT_MAGIC) return;
   if (above->delete_me) return;
//   if ((o->link->prev) && (o->link->prev->data == above)) return;
   o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->lin
k);
   o->layer_data->objects = evas_list_append_relative(o->layer_data->objects, o,
 above);
   o->link = above->link->next;
   p.relative_id = above->id;
   p.relative = EVOAK_RELATIVE_ABOVE;
   d = _evoak_proto[EVOAK_PR_OBJECT_STACK].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_STACK, o->id, 0, d, s);
        free(d);
     }
}

void
evoak_object_stack_below(Evoak_Object *o, Evoak_Object *below)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Stack p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->swallow) return;
   if (!below)
     {
        evoak_object_lower(o);
        return;
     }
   if (below->magic != EVOAK_OBJECT_MAGIC) return;
   if (below->delete_me) return;
//   if ((o->link->next) && (o->link->next->data == below)) return;
   o->layer_data->objects = evas_list_remove_list(o->layer_data->objects, o->lin
k);
   o->layer_data->objects = evas_list_prepend_relative(o->layer_data->objects, o
, below);
   o->link = below->link->prev;
   p.relative_id = below->id;
   p.relative = EVOAK_RELATIVE_BELOW;
   d = _evoak_proto[EVOAK_PR_OBJECT_STACK].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_STACK, o->id, 0, d, s);
        free(d);
     }
}

Evoak_Object *
evoak_object_above_get(Evoak_Object *o)
{
   Evas_List *l, *ll;
   int first = 1;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   for (l = o->layer_data->link; l; l = l->next)
     {
        Evoak_Layer *lay;

        lay = l->data;
        if (first) ll = o->link->next;
        else ll = lay->objects;
        first = 0;
        for (; ll; ll = ll->next)
          {
             Evoak_Object *eo;

             eo = ll->data;
             if (!eo->delete_me) return eo;
          }
     }
   return NULL;
}

Evoak_Object *
evoak_object_below_get(Evoak_Object *o)
{
   Evas_List *l, *ll;
   int first = 1;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   for (l = o->layer_data->link; l; l = l->prev)
     {
        Evoak_Layer *lay;

        lay = l->data;
        if (first) ll = o->link->prev;
        else ll = lay->objects;
        first = 0;
        for (; ll; ll = ll->prev)
          {
             Evoak_Object *eo;

             eo = ll->data;
             if (!eo->delete_me) return eo;
          }
     }
}

Evoak_Object *
evoak_object_bottom_get(Evoak *ev)
{
   Evas_List *l, *ll;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   for (l = ev->layers; l; l = l->next)
     {
        Evoak_Layer *lay;

        lay = l->data;
        for (ll = lay->objects; ll; ll = ll->next)
          {
             Evoak_Object *eo;

             eo = ll->data;
             if (!eo->delete_me) return eo;
          }
     }
   return NULL;
}

Evoak_Object *
evoak_object_top_get(Evoak *ev)
{
   Evas_List *l, *ll;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   for (l = evas_list_last(ev->layers); l; l = l->prev)
     {
        Evoak_Layer *lay;

        lay = l->data;
        for (ll = evas_list_last(lay->objects); ll; ll = ll->prev)
          {
             Evoak_Object *eo;

             eo = ll->data;
             if (!eo->delete_me) return eo;
          }
     }
   return NULL;
}
