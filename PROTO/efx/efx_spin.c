#include "efx_private.h"

typedef struct Efx_Spin_Data
{
   EFX *e;
   Evas_Map *map;
   Ecore_Animator *anim;
   long dps;
   double rotate;
   double start;
   unsigned int frame;
} Efx_Spin_Data;


static void
_obj_del(Efx_Spin_Data *esd, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (esd->anim) ecore_animator_del(esd->anim);
   esd->e->spin_data = NULL;
   if ((!esd->e->zoom_data) && (!esd->e->rotate_data))
     {
        eina_hash_del_by_key(_efx_object_manager, esd->e);
        free(esd->e);
     }
   free(esd);
}

static Eina_Bool
_spin_cb(Efx_Spin_Data *esd)
{
   Evas_Coord x, y, w, h;
   Evas_Map *map;
   double fps;

   evas_object_geometry_get(esd->e->obj, &x, &y, &w, &h);

   fps = 1.0 / ecore_animator_frametime_get();

   map = evas_map_new(4);
   evas_map_smooth_set(map, EINA_FALSE);
   evas_map_util_points_populate_from_object(map, esd->e->obj);
   esd->rotate = (double)esd->frame * ((double)esd->dps / fps) + esd->start;
   evas_map_util_rotate(map, esd->rotate, x + (w / 2), y + (h / 2));
   if (esd->e->zoom_data) _efx_zoom_calc(esd->e->zoom_data, map);
/*
   if (esd->frame % (int)fps == 0)
     DBG("frame: %u || rotate: %g", esd->frame, esd->rotate);
*/
   evas_object_map_set(esd->e->obj, map);
   evas_object_map_enable_set(esd->e->obj, EINA_TRUE);
   evas_map_free(map);
   if (!fmod(esd->rotate, 360.0)) esd->frame = 0;
   esd->frame++; /* FIXME: this may overflow */

   return EINA_TRUE;
}

static void
_spin_stop(Evas_Object *obj, Eina_Bool reset)
{
   EFX *e;
   Efx_Spin_Data *esd;

   e = eina_hash_find(_efx_object_manager, obj);
   if ((!e) || (!e->spin_data)) return;
   esd = e->spin_data;
   esd->frame = 0;
   if (reset)
     {
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, esd);
        _spin_cb(esd);
        esd->start = 0;
        _obj_del(esd, NULL, NULL, NULL);
        INF("reset spinning object %p", obj);
     }
   else
     {
        ecore_animator_del(esd->anim);
        esd->anim = NULL;
        esd->start = esd->rotate;
        INF("stopped spinning object %p", obj);
     }
}

void
_efx_spin_calc(void *data, Evas_Map *map)
{
   Efx_Spin_Data *esd = data;
   Evas_Coord x, y, w, h;
   evas_object_geometry_get(esd->e->obj, &x, &y, &w, &h);
   evas_map_util_rotate(map, esd->rotate, x + (w / 2), y + (h / 2));
}

Eina_Bool
efx_spin_start(Evas_Object *obj, long dps)
{
   EFX *e;
   Efx_Spin_Data *esd;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(dps, EINA_FALSE);
   /* must stop rotating if object is in motion */
   efx_rotate_stop(obj);
   e = eina_hash_find(_efx_object_manager, obj);
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   esd = e->spin_data;
   if (esd)
     {
        esd->dps = dps;
        if (!esd->anim) esd->anim = ecore_animator_add((Ecore_Task_Cb)_spin_cb, esd);
        INF("spin modified: %p - %s || %lddps", obj, (dps > 0) ? "clockwise" : "counter-clockwise", dps);
        return EINA_TRUE;
     }
   else
     e->spin_data = esd = calloc(1, sizeof(Efx_Spin_Data));
   EINA_SAFETY_ON_NULL_RETURN_VAL(esd, EINA_FALSE);

   esd->e = e;
   esd->dps = dps;
   INF("spin: %p - %s || %lddps", obj, (dps > 0) ? "clockwise" : "counter-clockwise", dps);
   esd->anim = ecore_animator_add((Ecore_Task_Cb)_spin_cb, esd);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, esd);
   return EINA_TRUE;
   (void)efx_speed_str;
}

void
efx_spin_reset(Evas_Object *obj)
{
   _spin_stop(obj, EINA_TRUE);
}
void
efx_spin_stop(Evas_Object *obj)
{
   _spin_stop(obj, EINA_FALSE);
}
