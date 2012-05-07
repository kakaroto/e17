#include "efx_private.h"

typedef struct Efx_Move_Data
{
   EFX *e;
   Ecore_Animator *anim;
   Efx_Effect_Speed speed;
   Evas_Point change;
   Evas_Point current;
   int degrees;
   Efx_End_Cb cb;
   void *data;
} Efx_Move_Data;

static void
_obj_del(Efx_Move_Data *emd, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (emd->anim) ecore_animator_del(emd->anim);
   emd->e->move_data = NULL;
   if ((!emd->e->owner) && (!emd->e->followers)) efx_free(emd->e);
   free(emd);
}

static void
_move(Evas_Object *obj, int x, int y)
{
   Evas_Coord ox, oy;

   evas_object_geometry_get(obj, &ox, &oy, NULL, NULL);
   evas_object_move(obj, ox + x, oy + y);
   //DBG("%p to (%d,%d)", obj, ox + x, oy + y);
}

static Eina_Bool
_move_circle_cb(Efx_Move_Data *emd, double pos)
{
   double pct, degrees;
   Eina_List *l;
   EFX *e;
   double x, y, r, rad;
   Evas_Coord xx, yy, ox, oy, w, h;


   pct = ecore_animator_pos_map(pos, emd->speed, 0, 0);
   degrees = pct * emd->degrees;
   evas_object_geometry_get(emd->e->obj, &ox, &oy, &w, &h);
   r = (degrees * M_PI) / 180.0;
   rad = sqrt((emd->current.x - emd->e->map_data.move_center->x) * (emd->current.x - emd->e->map_data.move_center->x) +
              (emd->current.y - emd->e->map_data.move_center->y) * (emd->current.y - emd->e->map_data.move_center->y));
   x = emd->current.x + (rad - (double)w / 2.) * cos(r);
   y = emd->current.y + (rad - (double)h / 2.) * sin(r);
   x -= (double)w / 2.;
   y -= (double)h / 2.;
   xx = lround(x);
   yy = lround(y);
   //DBG("move: %g || %g,%g", degrees, x, y);
   evas_object_move(emd->e->obj, xx, yy);
   efx_maps_apply(emd->e, emd->e->obj, NULL, EFX_MAPS_APPLY_ALL);
   EINA_LIST_FOREACH(emd->e->followers, l, e)
     {
        _move(e->obj, xx - ox, yy - oy);
        efx_maps_apply(e, e->obj, NULL, EFX_MAPS_APPLY_ALL);
     }

   if (pos != 1.0) return EINA_TRUE;

   EFX_QUEUE_CHECK(emd);
   return EINA_TRUE;
}

static Eina_Bool
_move_cb(Efx_Move_Data *emd, double pos)
{
   int x, y;
   double pct;
   Eina_List *l;
   EFX *e;

   pct = ecore_animator_pos_map(pos, emd->speed, 0, 0);
   x = lround(pct * (double)emd->change.x) - emd->current.x;
   y = lround(pct * (double)emd->change.y) - emd->current.y;
   _move(emd->e->obj, x, y);
   efx_maps_apply(emd->e, emd->e->obj, NULL, EFX_MAPS_APPLY_ALL);
   EINA_LIST_FOREACH(emd->e->followers, l, e)
     {
        _move(e->obj, x, y);
        efx_maps_apply(e, e->obj, NULL, EFX_MAPS_APPLY_ALL);
     }

   emd->current.x += x;
   emd->current.y += y;
   if (pos != 1.0) return EINA_TRUE;

   emd->anim = NULL;
   EFX_QUEUE_CHECK(emd);
   return EINA_TRUE;
}

EAPI Eina_Bool
efx_move(Evas_Object *obj, Efx_Effect_Speed speed, const Evas_Point *end_point, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Move_Data *emd;
   Evas_Coord x, y;
 
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   if (!end_point) return EINA_FALSE;
   if (total_time < 0.0) return EINA_FALSE;
   if (speed > EFX_EFFECT_SPEED_SINUSOIDAL) return EINA_FALSE;

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
   emd->change.x = end_point->x - x;
   emd->change.y = end_point->y - y;
   emd->current.x = emd->current.y = 0;
   emd->cb = cb;
   emd->data = (void*)data;
   if (emd->anim) ecore_animator_del(emd->anim);
   emd->anim = ecore_animator_timeline_add(total_time, (Ecore_Timeline_Cb)_move_cb, emd);
   return EINA_TRUE;
}


EAPI Eina_Bool
efx_move_circle(Evas_Object *obj, Efx_Effect_Speed speed, const Evas_Point *center, int degrees, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Move_Data *emd;
   Evas_Coord x, y;
 
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   if (!degrees) return EINA_FALSE;
   if (!center) return EINA_FALSE;
   if (total_time < 0.0) return EINA_FALSE;
   if (speed > EFX_EFFECT_SPEED_SINUSOIDAL) return EINA_FALSE;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   if (!efx_move_center_init(e, center)) return EINA_FALSE;

   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   INF("move: %p - (%d,%d) %d over %gs: %s", obj, x, y, degrees, total_time, efx_speed_str[speed]);
   if (!total_time)
     {
     //   evas_object_move(obj, end_point->x, end_point->y);
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
   emd->current.x = x, emd->current.y = y;
   emd->degrees = degrees;
   emd->cb = cb;
   emd->data = (void*)data;
   if (emd->anim) ecore_animator_del(emd->anim);
   emd->anim = ecore_animator_timeline_add(total_time, (Ecore_Timeline_Cb)_move_circle_cb, emd);
   return EINA_TRUE;
}
