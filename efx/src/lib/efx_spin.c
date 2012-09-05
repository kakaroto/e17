#include "efx_private.h"

typedef struct Efx_Spin_Data
{
   EFX *e;
   Ecore_Animator *anim;
   long dps;
   double start;
   unsigned int frame;
} Efx_Spin_Data;


static void
_obj_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   EFX *e;
   Efx_Spin_Data *esd;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) return; /* bug? */
   esd = e->spin_data;
   if (esd)
     {
        if (esd->anim) ecore_animator_del(esd->anim);
        e->spin_data = NULL;
        free(esd);
     }
   if ((!e->owner) && (!e->followers)) efx_free(e);
}

static Eina_Bool
_spin_cb(Efx_Spin_Data *esd)
{
   double fps;
   Eina_List *l;
   EFX *e;

   fps = 1.0 / ecore_animator_frametime_get();

   esd->e->map_data.rotation = (double)esd->frame * ((double)esd->dps / fps) + esd->start;
   efx_maps_apply(esd->e, esd->e->obj, NULL, EFX_MAPS_APPLY_ALL);
   EINA_LIST_FOREACH(esd->e->followers, l, e)
     {
        efx_maps_apply(e, e->obj, NULL, EFX_MAPS_APPLY_ALL);
     }
/*
   if (esd->frame % (int)fps == 0)
     DBG("frame: %u || rotate: %g", esd->frame, esd->e->map_data.rotation);
*/
   if (!fmod(esd->e->map_data.rotation, 360.0)) esd->frame = 0;
   esd->frame++; /* FIXME: this may overflow */

   return EINA_TRUE;
}

static void
_spin_stop(Evas_Object *obj, Eina_Bool reset)
{
   EFX *e;
   Efx_Spin_Data *esd;

   e = evas_object_data_get(obj, "efx-data");
   if ((!e) || (!e->spin_data)) return;
   esd = e->spin_data;
   esd->frame = 0;
   if (reset)
     {
        esd->e->map_data.rotation = esd->start = 0;
        efx_rotate_center_init(esd->e, NULL);
        _spin_cb(esd);
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, esd);
        _obj_del(NULL, NULL, e->obj, NULL);
        INF("reset spinning object %p", obj);
     }
   else
     {
        INF("stopped spinning object %p", obj);
        if (esd->anim) ecore_animator_del(esd->anim);
        free(esd);
        e->spin_data = NULL;
     }
}

void
_efx_spin_calc(void *data, void *owner, Evas_Object *obj, Evas_Map *map)
{
   Efx_Spin_Data *esd = data;
   Efx_Spin_Data *esd2 = owner;
   efx_rotate_helper(esd2 ? esd2->e : (esd ? esd->e : NULL), obj, map, (esd ? esd->e->map_data.rotation : 0) + (esd2 ? esd2->e->map_data.rotation : 0));
}

EAPI Eina_Bool
efx_spin_start(Evas_Object *obj, long dps, const Evas_Point *center)
{
   EFX *e;
   Efx_Spin_Data *esd;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(dps, EINA_FALSE);
   /* must stop rotating if object is in motion */
   efx_rotate_stop(obj);
   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   if (!efx_rotate_center_init(e, center)) return EINA_FALSE;
   esd = e->spin_data;
   if (esd)
     {
        esd->dps = dps;
        esd->start = esd->e->map_data.rotation;
        if (!esd->anim) esd->anim = ecore_animator_add((Ecore_Task_Cb)_spin_cb, esd);
        if (e->map_data.rotate_center)
          INF("spin modified: %p - %s around (%d,%d) || %lddps", obj, (dps > 0) ? "clockwise" : "counter-clockwise",
              e->map_data.rotate_center->x, e->map_data.rotate_center->y, dps);
        else
          INF("spin modified: %p - %s || %lddps", obj, (dps > 0) ? "clockwise" : "counter-clockwise", dps);
        return EINA_TRUE;
     }
   else
     {
        e->spin_data = esd = calloc(1, sizeof(Efx_Spin_Data));
        EINA_SAFETY_ON_NULL_RETURN_VAL(esd, EINA_FALSE);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->spin_data);
     }

   esd->e = e;
   esd->dps = dps;
   esd->start = e->map_data.rotation;
   if (e->map_data.rotate_center)
     INF("spin: %p - %s around (%d,%d) || %lddps", obj, (dps > 0) ? "clockwise" : "counter-clockwise",
         e->map_data.rotate_center->x, e->map_data.rotate_center->y, dps);
   else
     INF("spin: %p - %s || %lddps", obj, (dps > 0) ? "clockwise" : "counter-clockwise", dps);
   esd->anim = ecore_animator_add((Ecore_Task_Cb)_spin_cb, esd);
   return EINA_TRUE;
   (void)efx_speed_str;
}

EAPI void
efx_spin_reset(Evas_Object *obj)
{
   _spin_stop(obj, EINA_TRUE);
}
EAPI void
efx_spin_stop(Evas_Object *obj)
{
   _spin_stop(obj, EINA_FALSE);
}
