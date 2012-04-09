#include "efx_private.h"

/* glibc acos+asin are broken on some platforms,
 * found these on the wine mailing list
 */
static double
_acos(double x)
{
   return atan2f(sqrtf((1 - x) * (1 + x)), x);
}

static double
_asin(double x)
{
   return atan2f(x, sqrtf((1 - x) * (1 + x)));
}

EAPI void
efx_realize(Evas_Object *obj)
{
   EFX *e;
   Evas_Coord x, y, ox, oy, w, h;
   Evas_Point p1, p2;
   double zw, zh;
   Evas_Map *map;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) return;
   if (!e->map_data.rotate_center) return;
   evas_object_geometry_get(obj, &ox, &oy, &w, &h);
   map = (Evas_Map*)evas_object_map_get(obj);
   if (!map) return;
   evas_map_point_coord_get(map, 0, &p1.x, &p1.y, NULL);
   evas_map_point_coord_get(map, 2, &p2.x, &p2.y, NULL);
   x = lround((double)(p1.x + p2.x) / 2.);
   y = lround((double)(p1.y + p2.y) / 2.);
   if (e->map_data.zoom)
     zw = e->map_data.zoom * w, zh = e->map_data.zoom * h;
   else
     zw = w, zh = h;
   x = lround(x - (zw / 2.));
   y = lround(y - (zh / 2.));
   evas_object_move(obj, x, y);
   evas_object_resize(obj, lround(zw), lround(zh));
   e->map_data.zoom = 0;
   free(e->map_data.rotate_center);
   e->map_data.rotate_center = NULL;
   map = efx_map_new(obj);
   efx_maps_apply(e, obj, map, EFX_MAPS_APPLY_ALL);
   efx_map_set(obj, map);
   INF("realize: %p - (%d,%d)@%dx%d -> (%d,%d)@%dx%d", obj, ox, oy, w, h, x, y, (int)zw, (int)zh);
}
