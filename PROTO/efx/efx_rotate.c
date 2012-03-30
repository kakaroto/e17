#include "efx_private.h"

typedef struct Efx_Rotate_Data
{
   EFX *e;
   Evas_Map *map;
   Ecore_Animator *anim;
   Efx_Effect_Speed speed;
   double start_degrees;
   double current_degrees;
   double degrees;
   Efx_End_Cb cb;
   void *data;
} Efx_Rotate_Data;

static void
_obj_del(Efx_Rotate_Data *erd, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (erd->anim) ecore_animator_del(erd->anim);
   erd->e->rotate_data = NULL;
   if ((!erd->e->zoom_data) && (!erd->e->spin_data))
     {
        eina_hash_del_by_key(_efx_object_manager, erd->e);
        free(erd->e);
     }
   free(erd);
}

static void
_rotate(EFX *e, Evas_Object *obj, double degrees)
{
   Evas_Coord x, y, w, h;
   Evas_Map *map;

   evas_object_geometry_get(obj, &x, &y, &w, &h);

   map = evas_map_new(4);
   evas_map_smooth_set(map, EINA_FALSE);
   evas_map_util_points_populate_from_object(map, obj);
   evas_map_util_rotate(map, degrees, x + (w / 2), y + (h / 2));
   if (e->zoom_data) _efx_zoom_calc(e->zoom_data, map);
   evas_object_map_set(obj, map);
   evas_object_map_enable_set(obj, EINA_TRUE);
   evas_map_free(map);
}

static Eina_Bool
_rotate_cb(Efx_Rotate_Data *erd, double pos)
{
   double degrees;

   degrees = ecore_animator_pos_map(pos, erd->speed, 0, 0);
   erd->e->current_rotate = degrees * erd->degrees + erd->start_degrees;
   _rotate(erd->e, erd->e->obj, erd->e->current_rotate);

   if (pos != 1.0) return EINA_TRUE;

   if (erd->cb) erd->cb(erd->data, erd->degrees, erd->e->obj);
   erd->anim = NULL;
   return EINA_TRUE;
}

static void
_rotate_stop(Evas_Object *obj, Eina_Bool reset)
{
   EFX *e;
   Efx_Rotate_Data *erd;

   e = eina_hash_find(_efx_object_manager, obj);
   if ((!e) || (!e->rotate_data)) return;
   erd = e->rotate_data;
   if (reset)
     {
        _rotate(e, obj, 0);
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, erd);
        erd->e->current_rotate = 0;
        _obj_del(erd, NULL, NULL, NULL);
        INF("reset rotating object %p", obj);
     }
   else
     {
        ecore_animator_del(erd->anim);
        erd->anim = NULL;
        INF("stopped rotating object %p", obj);
     }
}

void
_efx_rotate_calc(void *data, Evas_Map *map)
{
   Efx_Rotate_Data *erd = data;
   Evas_Coord x, y, w, h;
   evas_object_geometry_get(erd->e->obj, &x, &y, &w, &h);
   evas_map_util_rotate(map, erd->e->current_rotate, x + (w / 2), y + (h / 2));
}

Eina_Bool
efx_rotate(Evas_Object *obj, Efx_Effect_Speed speed, double degrees, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Rotate_Data *erd;
 
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   if (!degrees) return EINA_FALSE;
   if (total_time < 0.0) return EINA_FALSE;
   if (speed > EFX_EFFECT_SPEED_SINUSOIDAL) return EINA_FALSE;
   /* can't rotate a spinning object, so we stop it first */
   e = eina_hash_find(_efx_object_manager, obj);
   if (e)
     {
        if (e->spin_data) efx_spin_stop(obj);
        if (e->rotate_data)
          {
             erd = e->rotate_data;
             if (erd->anim) efx_rotate_stop(obj);
          }
     }
   else
     {
         e = efx_new(obj);
         EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
     }

   INF("rotate: %p - %g degrees over %gs: %s", obj, degrees, total_time, efx_speed_str[speed]);
   if (!total_time)
     {
        if (!e->rotate_data)
          {
             e->rotate_data = calloc(1, sizeof(Efx_Rotate_Data));
             evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->rotate_data);
          }
        EINA_SAFETY_ON_NULL_RETURN_VAL(e->rotate_data, EINA_FALSE);
        erd = e->rotate_data;
        e->current_rotate += degrees;
        _rotate(e, obj, e->current_rotate);
        return EINA_TRUE;
     }
   if (!e->rotate_data)
     {
        e->rotate_data = calloc(1, sizeof(Efx_Rotate_Data));
        EINA_SAFETY_ON_NULL_RETURN_VAL(e->rotate_data, EINA_FALSE);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->rotate_data);
     }
   erd = e->rotate_data;
   erd->e = e;
   erd->speed = speed;
   erd->degrees = degrees;
   erd->start_degrees = e->current_rotate;
   erd->cb = cb;
   erd->data = (void*)data;
   if (erd->anim) ecore_animator_del(erd->anim);
   erd->anim = ecore_animator_timeline_add(total_time, (Ecore_Timeline_Cb)_rotate_cb, erd);
   return EINA_TRUE;
}

void
efx_rotate_reset(Evas_Object *obj)
{
   _rotate_stop(obj, EINA_FALSE);
}

void
efx_rotate_stop(Evas_Object *obj)
{
   _rotate_stop(obj, EINA_FALSE);
}
