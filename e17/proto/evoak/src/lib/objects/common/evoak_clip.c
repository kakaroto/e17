#include "Evoak.h"
#include "evoak_private.h"

void
evoak_object_clip_set(Evoak_Object *o, Evoak_Object *oc)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Clip_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (oc)
     {
        if (oc->magic != EVOAK_OBJECT_MAGIC) return;
        if (oc->delete_me) return;
     }
   if (o->clip == oc) return;
   if (o->clip)
     o->clip->clipees = evas_list_remove(o->clip->clipees, o);
   o->clip = oc;
   if (oc)
     oc->clipees = evas_list_append(oc->clipees, o);
   if (!oc) p.clipper_id = 0;
   else p.clipper_id = oc->id;
   d = _evoak_proto[EVOAK_PR_OBJECT_CLIP_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_CLIP_SET, o->id, 0, d, s);
        free(d);
     }
}

Evoak_Object *
evoak_object_clip_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   return o->clip;
}

void
evoak_object_clip_unset(Evoak_Object *o, Evoak_Object *oc)
{
   evoak_object_clip_set(o, NULL);
}

const Evoak_List *
evoak_object_clipees_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   return (Evoak_List *)o->clipees;
}
