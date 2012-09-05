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
   if (e->zoom_data || e->rotate_data || e->spin_data || e->move_data || e->bumpmap_data || e->pan_data || e->fade_data || e->queue) return;
   DBG("freeing efx for %p", e->obj);
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

   _efx_log_dom = eina_log_domain_register("efx", EINA_COLOR_GREEN);
   if (_efx_log_dom < 0) goto lgerr;
   return _efx_init_count;
lgerr:
   eina_shutdown();
err:
   return --_efx_init_count;
   (void)efx_speed_str;
}

EAPI void
efx_shutdown(void)
{
   if (--_efx_init_count != 0) return;
   eina_log_domain_unregister(_efx_log_dom);
   _efx_log_dom = -1;
   eina_shutdown();
}
