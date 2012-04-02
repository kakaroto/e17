#include "efx_private.h"

typedef struct Efx_Move_Data
{
   EFX *e;
   Ecore_Animator *anim;
   Efx_Effect_Speed speed;
   Evas_Point diff;
   Evas_Point current;
   Efx_End_Cb cb;
   void *data;
} Efx_Move_Data;

static void
_obj_del(Efx_Move_Data *emd, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (emd->anim) ecore_animator_del(emd->anim);
   emd->e->move_data = NULL;
   efx_free(emd->e);
   free(emd);
}

static void
_move(Evas_Object *obj, int x, int y)
{
   Evas_Coord ox, oy;

   evas_object_geometry_get(obj, &ox, &oy, NULL, NULL);
   evas_object_move(obj, ox + x, oy + y);
}

static Eina_Bool
_move_cb(Efx_Move_Data *emd, double pos)
{
   int x, y;
   double pct;
   Eina_List *l;
   EFX *e;

   pct = ecore_animator_pos_map(pos, emd->speed, 0, 0);
   x = lround(pct * (double)emd->diff.x) - emd->current.x;
   y = lround(pct * (double)emd->diff.y) - emd->current.y;
   _move(emd->e->obj, x, y);
   efx_maps_apply(emd->e, emd->e->obj, NULL, EFX_MAPS_APPLY_ALL);
   EINA_LIST_FOREACH(emd->e->followers, l, e)
     {
        _move(e->obj, x, y);
        efx_maps_apply(emd->e, e->obj, NULL, EFX_MAPS_APPLY_ALL);
     }

   emd->current.x += x;
   emd->current.y += y;
   if (pos != 1.0) return EINA_TRUE;

   //if (emd->cb) emd->cb(emd->data, emd->ending_zoom, emd->e->obj);
   return EINA_TRUE;
}

EAPI Eina_Bool
efx_move(Evas_Object *obj, Efx_Effect_Speed speed, Evas_Point *end_point, double total_time)
{
   EFX *e;
   Efx_Move_Data *emd;
   Evas_Coord x, y;
 
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   if (!end_point) return EINA_FALSE;
   if (total_time < 0.0) return EINA_FALSE;
   if (speed > EFX_EFFECT_SPEED_SINUSOIDAL) return EINA_FALSE;
   if (end_point->x < 0) return EINA_FALSE;
   if (end_point->y < 0) return EINA_FALSE;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);

   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   INF("move: %p - (%d,%d) -> (%d,%d) over %gs: %s", obj, x, y, end_point->x, end_point->y, total_time, efx_speed_str[speed]);
   if (!total_time)
     {
        evas_object_move(obj, end_point->x, end_point->y);
        return EINA_TRUE;
     }
   if (!e->move_data)
     {
        e->move_data = calloc(1, sizeof(Efx_Move_Data));
        EINA_SAFETY_ON_NULL_RETURN_VAL(e->move_data, EINA_FALSE);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->move_data);
     }
   emd = e->move_data;
   emd->e = e;
   emd->speed = speed;
   emd->diff.x = end_point->x - x;
   emd->diff.y = end_point->y - y;
   emd->current.x = emd->current.y = 0;
/*
   emd->cb = cb;
   emd->data = (void*)data;
*/
   if (emd->anim) ecore_animator_del(emd->anim);
   ecore_animator_timeline_add(total_time, (Ecore_Timeline_Cb)_move_cb, emd);
   return EINA_TRUE;
}
