#include "Evoak.h"
#include "evoak_private.h"

static void
_evoak_callback_call(Evoak_Object *o, Evoak_Callback_Type type, void *event_info
)
{
   Evas_List *l;

   _evoak_object_ref(o);
   o->lock++;
   for (l = o->callbacks; l; l = l->next)
     {
        Evoak_Callback *cb;

        cb = l->data;
        if (cb->just_added) cb->just_added = 0;
        else if ((!cb->delete_me) && (cb->type == type))
          cb->func(cb->data, o->evoak, o, event_info);
        if (o->delete_me) break;
     }
   o->lock--;
   if ((o->lock == 0) && (o->delete_cb))
     {
        for (l = o->callbacks; l;)
          {
             Evoak_Callback *cb;

             cb = l->data;
             if (cb->delete_me)
               {
                  Evas_List *nl;

                  nl = l->next;
                  o->callbacks = evas_list_remove_list(o->callbacks, nl);
                  free(cb);
               }
             else
               l = l->next;
          }
        o->delete_cb = 0;
     }
   _evoak_object_unref(o);
}

void
evoak_object_event_callback_add(Evoak_Object *o, Evoak_Callback_Type type, void
(*func) (void *data, Evoak *e, Evoak_Object *obj, void *event_info), const void
*data)
{
   Evoak_Callback *cb;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   cb = calloc(1, sizeof(Evoak_Callback));
   if (cb)
     {
        int changed = 0;

        cb->type = type;
        cb->func = func;
        cb->data = (void *)data;
        if (o->lock > 1) cb->just_added = 1;
        o->callbacks = evas_list_append(o->callbacks, cb);
        switch (type)
          {
           case EVOAK_CALLBACK_MOUSE_IN:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_IN)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_IN;
             break;
           case EVOAK_CALLBACK_MOUSE_OUT:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_OUT)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_OUT;
             break;
           case EVOAK_CALLBACK_MOUSE_DOWN:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_DOWN)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_DOWN;
             break;
           case EVOAK_CALLBACK_MOUSE_UP:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_UP)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_UP;
             break;
           case EVOAK_CALLBACK_MOUSE_MOVE:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_MOVE)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_MOVE;
             break;
           case EVOAK_CALLBACK_MOUSE_WHEEL:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_MOUSE_WHEEL)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_MOUSE_WHEEL;
             break;
           case EVOAK_CALLBACK_FREE:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_FREED)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_OBJ_FREED;
             break;
           case EVOAK_CALLBACK_KEY_DOWN:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_KEY_DOWN)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_KEY_DOWN;
             break;
           case EVOAK_CALLBACK_KEY_UP:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_KEY_UP)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_KEY_UP;
             break;
           case EVOAK_CALLBACK_FOCUS_IN:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_FOCUS_IN)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_OBJ_FOCUS_IN;
             break;
           case EVOAK_CALLBACK_FOCUS_OUT:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT;
             break;
           case EVOAK_CALLBACK_SHOW:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE)) changed = 1
;
             o->mask1 |= EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE;
             break;
           case EVOAK_CALLBACK_HIDE:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE)) changed = 1
;
             o->mask1 |= EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE;
             break;
           case EVOAK_CALLBACK_MOVE:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_MOVED)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_OBJ_MOVED;
             break;
           case EVOAK_CALLBACK_RESIZE:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_RESIZED)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_OBJ_RESIZED;
             break;
           case EVOAK_CALLBACK_RESTACK:
             if (!(o->mask1 & EVOAK_EVENT_MASK1_OBJ_RESTACKED)) changed = 1;
             o->mask1 |= EVOAK_EVENT_MASK1_OBJ_RESTACKED;
             break;
           default:
             break;
          }
        if (changed)
          {
             unsigned char *d;
             int s;
             Evoak_PR_Object_Event_Mask_Set p;

             p.mask1 = o->mask1;
             p.mask2 = o->mask2;
             d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MASK_SET].enc(&p, &s);
             if (d)
               {
                  _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EVENT_MASK_SET, o->i
d, 0, d, s);
                  free(d);
               }
          }
     }
}

void *
evoak_object_event_callback_del(Evoak_Object *o, Evoak_Callback_Type type, void
(*func) (void *data, Evoak *e, Evoak_Object *obj, void *event_info))
{
   Evas_List *l;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   for (l = o->callbacks; l; l = l->next)
     {
        Evoak_Callback *cb;

        cb = l->data;
        if ((cb->type == type) && (cb->func == func))
          {
             void *data;
             int changed = 0;

             if (o->lock > 0)
               {
                  o->delete_cb = 1;
                  cb->delete_me = 1;
               }
             else
               {
                  o->callbacks = evas_list_remove_list(o->callbacks, l);
                  data = cb->data;
                  free(cb);
               }
             switch (type)
               {
                case EVOAK_CALLBACK_MOUSE_IN:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_IN)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_IN;
                  break;
                case EVOAK_CALLBACK_MOUSE_OUT:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_OUT)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_OUT;
                  break;
                case EVOAK_CALLBACK_MOUSE_DOWN:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_DOWN)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_DOWN;
                  break;
                case EVOAK_CALLBACK_MOUSE_UP:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_UP)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_UP;
                  break;
                case EVOAK_CALLBACK_MOUSE_MOVE:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_MOVE)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_MOVE;
                  break;
                case EVOAK_CALLBACK_MOUSE_WHEEL:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_MOUSE_WHEEL)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_MOUSE_WHEEL;
                  break;
                case EVOAK_CALLBACK_FREE:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_FREED)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_FREED;
                  break;
                case EVOAK_CALLBACK_KEY_DOWN:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_KEY_DOWN)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_KEY_DOWN;
                  break;
                case EVOAK_CALLBACK_KEY_UP:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_KEY_UP)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_KEY_UP;
                  break;
                case EVOAK_CALLBACK_FOCUS_IN:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_FOCUS_IN)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_FOCUS_IN;
                  break;
                case EVOAK_CALLBACK_FOCUS_OUT:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT;
                  break;
                case EVOAK_CALLBACK_SHOW:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE)) changed
 = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE;
                  break;
                case EVOAK_CALLBACK_HIDE:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE)) changed
 = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE;
                  break;
                case EVOAK_CALLBACK_MOVE:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_MOVED)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_MOVED;
                  break;
                case EVOAK_CALLBACK_RESIZE:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_RESIZED)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_RESIZED;
                  break;
                case EVOAK_CALLBACK_RESTACK:
                  if ((o->mask1 & EVOAK_EVENT_MASK1_OBJ_RESTACKED)) changed = 1;
                  o->mask1 &= ~EVOAK_EVENT_MASK1_OBJ_RESTACKED;
                  break;
                default:
                  break;
               }
             if (changed)
               {
                  unsigned char *d;
                  int s;
                  Evoak_PR_Object_Event_Mask_Set p;

                  p.mask1 = o->mask1;
                  p.mask2 = o->mask2;
                  d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MASK_SET].enc(&p, &s);
                  if (d)
                    {
                       _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EVENT_MASK_SET,
 o->id, 0, d, s);
                       free(d);
                    }
               }
             return data;
          }
     }
   return NULL;
}
