#include "Evoak.h"
#include "evoak_private.h"

static void
_evoak_event_generic_free(void *data, void *ev)
{
   free(ev);
}

void
evoak_object_pass_events_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Event_Prop_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (((o->pass_events) && (onoff)) ||
       ((!o->pass_events) && (!onoff))) return;
   o->pass_events = onoff;
   p.property = EVOAK_PROPERTY_PASS;
   p.value = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_PROP_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EVENT_PROP_SET, o->id, 0, d, s
);
        free(d);
     }
}

evoak_object_pass_events_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->pass_events;
}

void
evoak_object_repeat_events_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Event_Prop_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (((o->repeat_events) && (onoff)) ||
       ((!o->repeat_events) && (!onoff))) return;
   o->repeat_events = onoff;
   p.property = EVOAK_PROPERTY_REPEAT;
   p.value = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_PROP_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EVENT_PROP_SET, o->id, 0, d, s
);
        free(d);
     }
}

Evoak_Bool
evoak_object_repeat_events_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->repeat_events;
}


