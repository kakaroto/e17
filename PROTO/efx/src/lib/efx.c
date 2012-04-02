#include "efx_private.h"

Eina_Hash *_efx_object_manager = NULL;
int _efx_log_dom = -1;
static int _efx_init_count = 0;


static void
_hash_clear(void)
{
   if (_efx_object_manager) eina_hash_free(_efx_object_manager);
   _efx_object_manager = NULL;
}

static Eina_Bool
_hash_setup(void)
{
   _efx_object_manager = eina_hash_pointer_new(NULL);
   return !!_efx_object_manager;
}

int
efx_init(void)
{
   if (++_efx_init_count > 1) return _efx_init_count;

   if (eina_init() < 1) goto err;
   if (ecore_evas_init() < 1) goto ecerr;

   if (!_hash_setup()) goto herr;

   _efx_log_dom = eina_log_domain_register("efx", EINA_COLOR_GREEN);
   if (_efx_log_dom < 0) goto lgerr;
   return _efx_init_count;
lgerr:
   _hash_clear();
herr:
   ecore_evas_shutdown();
ecerr:
   eina_shutdown();
err:
   return --_efx_init_count;
}

void
efx_shutdown(void)
{
   if (--_efx_init_count != 0) return;
   eina_log_domain_unregister(_efx_log_dom);
   _efx_log_dom = -1;
   _hash_clear();
   ecore_evas_shutdown();
   eina_shutdown();
}

EFX *
efx_new(Evas_Object *obj)
{
   EFX *e;
   e = calloc(1, sizeof(EFX));
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   e->obj = obj;
   eina_hash_direct_add(_efx_object_manager, obj, e);
   return e;
}

void
efx_free(EFX *e, const Evas_Object *obj)
{
   eina_hash_del_by_key(_efx_object_manager, obj);
   free(e->rotate.center);
   free(e);
}

Eina_Bool
efx_rotate_center_init(EFX *e, const Evas_Point *center)
{
   if (center)
     {
        if (!e->rotate.center) e->rotate.center = malloc(sizeof(Evas_Point));
        EINA_SAFETY_ON_NULL_RETURN_VAL(e->rotate.center, EINA_FALSE);
        e->rotate.center->x = center->x, e->rotate.center->y = center->y;
     }
   else
     {
        free(e->rotate.center);
        e->rotate.center = NULL;
     }
   return EINA_TRUE;
}
