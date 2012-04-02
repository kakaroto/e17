#include "efx_private.h"

int _efx_log_dom = -1;
static int _efx_init_count = 0;

EFX *
efx_new(Evas_Object *obj)
{
   EFX *e;
   e = calloc(1, sizeof(EFX));
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   e->obj = obj;
   evas_object_data_set(obj, "efx-data", e);
   return e;
}

void
efx_free(EFX *e)
{
   EFX *ef;
   if (e->zoom_data || e->rotate_data || e->spin_data || e->move_data) return;
   EINA_LIST_FREE(e->followers, ef)
     efx_free(ef);
   evas_object_data_del(e->obj, "efx-data");
   free(e->map_data.rotate_center);
   free(e->map_data.zoom_center);
   free(e);
}

EAPI int
efx_init(void)
{
   if (++_efx_init_count > 1) return _efx_init_count;

   if (eina_init() < 1) goto err;
   if (ecore_evas_init() < 1) goto ecerr;

   _efx_log_dom = eina_log_domain_register("efx", EINA_COLOR_GREEN);
   if (_efx_log_dom < 0) goto lgerr;
   return _efx_init_count;
lgerr:
   ecore_evas_shutdown();
ecerr:
   eina_shutdown();
err:
   return --_efx_init_count;
}

EAPI void
efx_shutdown(void)
{
   if (--_efx_init_count != 0) return;
   eina_log_domain_unregister(_efx_log_dom);
   _efx_log_dom = -1;
   ecore_evas_shutdown();
   eina_shutdown();
}

EAPI Eina_Bool
efx_follow(Evas_Object *obj, Evas_Object *follower)
{
   EFX *e, *ef;
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(follower, EINA_FALSE);

   e = evas_object_data_get(obj, "efx-data");
   if (!e) return EINA_FALSE;
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
   return EINA_FALSE;
}

EAPI void
efx_unfollow(Evas_Object *obj)
{
   EFX *e;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   e = evas_object_data_get(obj, "efx-data");
   if (!e) return;
   if (!e->owner) return;
   e->owner->followers = eina_list_remove(e->owner->followers, e);
   e->owner = NULL;
   efx_free(e);
}
