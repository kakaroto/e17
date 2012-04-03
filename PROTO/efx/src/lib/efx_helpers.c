#include "efx_private.h"

Eina_Bool
efx_rotate_center_init(EFX *e, const Evas_Point *center)
{
   if (center)
     {
        if (!e->map_data.rotate_center) e->map_data.rotate_center = malloc(sizeof(Evas_Point));
        EINA_SAFETY_ON_NULL_RETURN_VAL(e->map_data.rotate_center, EINA_FALSE);
        e->map_data.rotate_center->x = center->x, e->map_data.rotate_center->y = center->y;
     }
   else
     {
        free(e->map_data.rotate_center);
        e->map_data.rotate_center = NULL;
     }
   return EINA_TRUE;
}

Eina_Bool
efx_zoom_center_init(EFX *e, const Evas_Point *center)
{
   if (center)
     {
        if (!e->map_data.zoom_center) e->map_data.zoom_center = malloc(sizeof(Evas_Point));
        EINA_SAFETY_ON_NULL_RETURN_VAL(e->map_data.zoom_center, EINA_FALSE);
        e->map_data.zoom_center->x = center->x, e->map_data.zoom_center->y = center->y;
     }
   else
     {
        free(e->map_data.zoom_center);
        e->map_data.zoom_center = NULL;
     }
   return EINA_TRUE;
}

Evas_Map *
efx_map_new(Evas_Object *obj)
{
   Evas_Map *map;

   map = evas_map_new(4);
   evas_map_smooth_set(map, EINA_FALSE);
   evas_map_util_points_populate_from_object(map, obj);
   return map;
}

void
efx_map_set(Evas_Object *obj, Evas_Map *map)
{
   evas_object_map_set(obj, map);
   evas_object_map_enable_set(obj, EINA_TRUE);
   evas_map_free(map);
}

void
efx_rotate_helper(EFX *e, Evas_Object *obj, Evas_Map *map, double degrees)
{
   double x, y, xx, yy, r;
   Evas_Coord ox, oy, w, h;

   evas_object_geometry_get(obj, &ox, &oy, &w, &h);
   if (e->map_data.rotate_center)
     {
        r = (degrees * M_PI) / 180.0;
        x = e->start.x - e->map_data.rotate_center->x;
        y = e->start.y - e->map_data.rotate_center->y;

        xx = x * cos(r);
        yy = x * sin(r);
        x = xx - (y * sin(r));
        y = yy + (y * cos(r));

        DBG("rotate: %g || %ld,%ld", degrees, lround(x + e->map_data.rotate_center->x - ox), lround(y + e->map_data.rotate_center->y - oy));
        ox = lround(x + e->map_data.rotate_center->x);
        oy = lround(y + e->map_data.rotate_center->y);
        evas_object_move(obj, ox, oy);
     }
   evas_map_util_rotate(map, degrees, ox + (w / 2), oy + (h / 2));
   //_size_debug(e->obj);
}

void
efx_maps_apply(EFX *e, Evas_Object *obj, Evas_Map *map, Eina_Bool rotate, Eina_Bool spin, Eina_Bool zoom)
{
   Eina_Bool new = EINA_FALSE;
   if ((!e->rotate_data) && (!e->spin_data) && (!e->zoom_data)) return;
   if (!map)
     {
        map = efx_map_new(e->obj);
        new = EINA_TRUE;
     }
   if (rotate && e->rotate_data) _efx_rotate_calc(e->rotate_data, obj, map);
   if (spin && e->spin_data) _efx_spin_calc(e->spin_data, obj, map);
   if (zoom && e->zoom_data) _efx_zoom_calc(e->zoom_data, obj, map);
   if (new) efx_map_set(obj, map);
}
