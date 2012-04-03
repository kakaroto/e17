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
   int r;
   Evas_Map *map;
   double rotation;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) return;
   if (!e->map_data.rotate_center) return;
   evas_object_geometry_get(obj, &ox, &oy, &w, &h);
   r = abs(e->map_data.rotate_center->x - ox);
   rotation = fmod(e->map_data.rotation, 360.0);
   x = lround(e->map_data.rotate_center->x + r * cos(_acos((double)(ox - e->map_data.rotate_center->x) / (double)r) - (rotation / 180. * M_PI)));
   y = lround(e->map_data.rotate_center->y + r * sin(_asin((double)(oy - e->map_data.rotate_center->y) / (double)r) - (rotation / 180. * M_PI)));
   if (rotation >= 90.0) x -= w;
   if (rotation >= 180.0) y -= h;
   INF("realize: %p - (%d,%d) -> (%d,%d)", obj, ox, oy, x, y);
   evas_object_move(obj, x, y);
   free(e->map_data.rotate_center);
   e->map_data.rotate_center = NULL;
   map = efx_map_new(obj);
   efx_maps_apply(e, obj, map, EFX_MAPS_APPLY_ALL);
   efx_map_set(obj, map);
}
