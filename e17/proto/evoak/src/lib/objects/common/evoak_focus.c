#include "Evoak.h"
#include "evoak_private.h"

void
evoak_object_focus_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Focus_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (((o->focused) && (onoff)) ||
       ((!o->focused) && (!onoff))) return;
   o->focused = onoff;
   p.onoff = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_FOCUS_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_FOCUS_SET, o->id, 0, d, s);
        free(d);
     }
}

Evoak_Bool
evoak_object_focus_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->focused;
}
