#include "efx_private.h"

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

EAPI Eina_Bool
efx_follow(Evas_Object *obj, Evas_Object *follower)
{
   EFX *e, *ef;
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(follower, EINA_FALSE);

   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   while (e->owner) e = e->owner;

   ef = evas_object_data_get(follower, "efx-data");
   if (ef)
     {
        if (ef->owner)
          ef->owner->followers = eina_list_remove(ef->owner->followers, ef);
     }
   else
     ef = efx_new(follower);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ef, EINA_FALSE);

   ef->owner = e;
   e->followers = eina_list_append(e->followers, ef);
   INF("follow: (owner %p) || (follower %p)", obj, follower);
   return EINA_TRUE;
}

EAPI void
efx_unfollow(Evas_Object *obj)
{
   EFX *e;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   e = evas_object_data_get(obj, "efx-data");
   if (!e) return;
   if (!e->owner) return;
   INF("unfollow: (owner %p) || (follower %p)", e->owner->obj, obj);
   e->owner->followers = eina_list_remove(e->owner->followers, e);
   efx_free(e->owner);
   e->owner = NULL;
   efx_free(e);
}

EAPI Eina_List *
efx_followers_get(Evas_Object *obj)
{
   EFX *e, *f;
   Eina_List *l, *ret = NULL;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, NULL);
   e = evas_object_data_get(obj, "efx-data");
   if (!e) return NULL;
   EINA_LIST_FOREACH(e->followers, l, f)
     ret = eina_list_append(ret, f->obj);
   return ret;
}
