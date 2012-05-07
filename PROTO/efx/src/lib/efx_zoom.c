#include "efx_private.h"

typedef struct Efx_Zoom_Data
{
   EFX *e;
   Ecore_Animator *anim;
   Efx_Effect_Speed speed;
   double ending_zoom;
   double starting_zoom;
   Efx_End_Cb cb;
   void *data;
} Efx_Zoom_Data;

static void
_obj_del(Efx_Zoom_Data *ezd, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (ezd->anim) ecore_animator_del(ezd->anim);
   ezd->e->zoom_data = NULL;
   if ((!ezd->e->owner) && (!ezd->e->followers)) efx_free(ezd->e);
   free(ezd);
}

static void
_zoom_center_calc(EFX *e, Evas_Object *obj, Evas_Coord *x, Evas_Coord *y)
{
   Evas_Coord w, h;
   if (e->map_data.zoom_center)
     {
        *x = e->map_data.zoom_center->x;
        *y = e->map_data.zoom_center->y;
     }
   else
     {
        evas_object_geometry_get(obj, x, y, &w, &h);
        *x += (w / 2);
        *y += (h / 2);
     }
}

static void
_zoom(EFX *e, Evas_Object *obj, double zoom)
{
   Evas_Map *map;
   Evas_Coord x, y;

   map = efx_map_new(obj);
   _zoom_center_calc(e, e->obj, &x, &y);
   //DBG("ZOOM %p: %g: %d,%d", obj, zoom, x, y);
   evas_map_util_zoom(map, zoom, zoom, x, y);
   efx_maps_apply(e, obj, map, EFX_MAPS_APPLY_ROTATE_SPIN);
   efx_map_set(obj, map);
}

static Eina_Bool
_zoom_cb(Efx_Zoom_Data *ezd, double pos)
{
   double zoom;
   Eina_List *l;
   EFX *e;

   zoom = ecore_animator_pos_map(pos, ezd->speed, 0, 0);
   ezd->e->map_data.zoom = (zoom * (ezd->ending_zoom - ezd->starting_zoom)) + ezd->starting_zoom;
   //DBG("total: %g || zoom (pos %g): %g || endzoom: %g || startzoom: %g", ezd->e->map_data.zoom, zoom, pos, ezd->ending_zoom, ezd->starting_zoom);
   efx_maps_apply(ezd->e, ezd->e->obj, NULL, EFX_MAPS_APPLY_ALL);
   EINA_LIST_FOREACH(ezd->e->followers, l, e)
     efx_maps_apply(e, e->obj, NULL, EFX_MAPS_APPLY_ALL);

   if (pos != 1.0) return EINA_TRUE;

   ezd->anim = NULL;
   EFX_QUEUE_CHECK(ezd);
   return EINA_TRUE;
}

static void
_zoom_stop(Evas_Object *obj, Eina_Bool reset)
{
   EFX *e;
   Efx_Zoom_Data *ezd;

   e = evas_object_data_get(obj, "efx-data");
   if ((!e) || (!e->zoom_data)) return;
   ezd = e->zoom_data;
   if (reset)
     {
        _zoom(e, obj, 1.0);
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, ezd);
        if (efx_queue_complete(ezd->e, ezd))
          efx_queue_process(ezd->e);
        _obj_del(ezd, NULL, NULL, NULL);
        INF("reset zooming object %p", obj);
     }
   else
     {
        ecore_animator_del(ezd->anim);
        ezd->anim = NULL;
        INF("stopped zooming object %p", obj);
        if (efx_queue_complete(ezd->e, ezd))
          efx_queue_process(ezd->e);
     }
}

void
_efx_zoom_calc(void *data, void *owner, Evas_Object *obj, Evas_Map *map)
{
   Efx_Zoom_Data *ezd = data;
   Efx_Zoom_Data *ezd2 = owner;
   Evas_Coord x, y;
   double zoom;
   if ((ezd2 && (ezd2->e->map_data.zoom <= 0)) || (ezd && (ezd->e->map_data.zoom <= 0))) return;
   _zoom_center_calc(ezd2 ? ezd2->e : ezd->e, ezd2 ? ezd2->e->obj : obj, &x, &y);
   zoom = ezd ? ezd->e->map_data.zoom : 0;
   if (ezd2) zoom += ezd2->e->map_data.zoom;
   //DBG("zoom: %g @ (%d,%d)", zoom, x, y);
   evas_map_util_zoom(map, zoom, zoom, x, y);
}

EAPI Eina_Bool
efx_zoom(Evas_Object *obj, Efx_Effect_Speed speed, double starting_zoom, double ending_zoom, const Evas_Point *zoom_point, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Zoom_Data *ezd;
 
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   if (ending_zoom <= 0.0) return EINA_FALSE;
   if (starting_zoom < 0.0) return EINA_FALSE;
   if (total_time < 0.0) return EINA_FALSE;
   if (speed > EFX_EFFECT_SPEED_SINUSOIDAL) return EINA_FALSE;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   if (!efx_zoom_center_init(e, zoom_point)) return EINA_FALSE;
   INF("zoom: %p - %g%%->%g%% over %gs: %s", obj, (starting_zoom ?: e->map_data.zoom) * 100.0, ending_zoom * 100.0, total_time, efx_speed_str[speed]);
   if (!e->zoom_data)
     {
        e->zoom_data = calloc(1, sizeof(Efx_Zoom_Data));
        EINA_SAFETY_ON_NULL_RETURN_VAL(e->zoom_data, EINA_FALSE);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->zoom_data);
     }
   ezd = e->zoom_data;
   ezd->e = e;
   ezd->speed = speed;
   ezd->ending_zoom = ending_zoom;
   ezd->starting_zoom = starting_zoom ?: ezd->e->map_data.zoom;
   ezd->cb = cb;
   ezd->data = (void*)data;
   if (!total_time)
     {
        _zoom_cb(ezd, 1.0);
        return EINA_TRUE;
     }
   if (ezd->anim) ecore_animator_del(ezd->anim);
   ezd->anim = ecore_animator_timeline_add(total_time, (Ecore_Timeline_Cb)_zoom_cb, ezd);
   return EINA_TRUE;
}

EAPI void
efx_zoom_reset(Evas_Object *obj)
{
   _zoom_stop(obj, EINA_TRUE);
}

EAPI void
efx_zoom_stop(Evas_Object *obj)
{
   _zoom_stop(obj, EINA_FALSE);
}
