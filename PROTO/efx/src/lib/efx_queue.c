#include "efx_private.h"

struct Efx_Queue_Data
{
   Efx_Queued_Effect effect;
   Efx_Effect_Speed speed;
   double time;
   Efx_End_Cb cb;
   void *data;

   void *effect_data;
   Eina_Bool active : 1;
   Eina_Bool complete : 1;
};

static void
_queue_advance(EFX *e)
{
   Efx_Queue_Data *eqd;
   eqd = eina_list_data_get(e->queue);
   INF("queue_advance: %p", e->obj);
   switch (eqd->effect.type)
     {
      case EFX_EFFECT_TYPE_ROTATE:
        efx_rotate(e->obj, eqd->speed, eqd->effect.effect.rotation.degrees, eqd->effect.effect.rotation.center, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = e->rotate_data;
        break;
      case EFX_EFFECT_TYPE_ZOOM:
        efx_zoom(e->obj, eqd->speed, eqd->effect.effect.zoom.start, eqd->effect.effect.zoom.end, eqd->effect.effect.zoom.center, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = e->zoom_data;
        break;
      case EFX_EFFECT_TYPE_MOVE:
        efx_move(e->obj, eqd->speed, &eqd->effect.effect.movement.point, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = e->move_data;
        break;
      case EFX_EFFECT_TYPE_PAN:
        efx_pan(e->obj, eqd->speed, &eqd->effect.effect.movement.point, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = e->pan_data;
        break;
      case EFX_EFFECT_TYPE_FADE:
      default:
        efx_fade(e->obj, eqd->speed, &eqd->effect.effect.fade.color, eqd->effect.effect.fade.alpha, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = e->fade_data;
     }
   eqd->active = EINA_TRUE;
}

void
efx_queue_process(EFX *e)
{
   Efx_Queue_Data *eqd;

   eqd = eina_list_data_get(e->queue);
   if (!eqd) return;
   if (eqd->active) return;

   _queue_advance(e);
}

Eina_Bool
efx_queue_complete(EFX *e, void *effect_data)
{
   Efx_Queue_Data *eqd;

   eqd = eina_list_data_get(e->queue);
   if (!eqd)
     {
        efx_free(e);
        return EINA_FALSE;
     }
   DBG("%p: %p", e->obj, effect_data);
   if (eqd->effect_data != effect_data) return EINA_FALSE;
   e->queue = eina_list_remove_list(e->queue, e->queue);
   if (eqd->effect.type == EFX_EFFECT_TYPE_ROTATE)
     free(eqd->effect.effect.rotation.center);
   else if (eqd->effect.type == EFX_EFFECT_TYPE_ZOOM)
     free(eqd->effect.effect.zoom.center);
   free(eqd);
   return !!e->queue;
}

EAPI void
efx_queue_run(Evas_Object *obj)
{
   EFX *e;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) return;
   efx_queue_process(e);
}

EAPI Efx_Queue_Data *
efx_queue_append(Evas_Object *obj, Efx_Effect_Speed speed, Efx_Queued_Effect *effect, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Queue_Data *eqd;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(effect, NULL);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(total_time >= 0.0, NULL);
   if (effect->type > EFX_EFFECT_TYPE_FADE) return NULL;
   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   eqd = calloc(1, sizeof(Efx_Queue_Data));
   memcpy(eqd, effect, sizeof(Efx_Queued_Effect));
   if (effect->type == EFX_EFFECT_TYPE_ROTATE)
     {
        if (effect->effect.rotation.center)
          {
             eqd->effect.effect.rotation.center = malloc(sizeof(Evas_Point));
             if (!eqd->effect.effect.rotation.center)
               {
                  free(eqd);
                  efx_free(e);
                  return NULL;
               }
             memcpy(eqd->effect.effect.rotation.center, effect->effect.rotation.center, sizeof(Evas_Point));
          }
     }
   else if (effect->type == EFX_EFFECT_TYPE_ZOOM)
     {
        if (effect->effect.zoom.center)
          {
             eqd->effect.effect.zoom.center = malloc(sizeof(Evas_Point));
             if (!eqd->effect.effect.zoom.center)
               {
                  free(eqd);
                  efx_free(e);
                  return NULL;
               }
             memcpy(eqd->effect.effect.zoom.center, effect->effect.zoom.center, sizeof(Evas_Point));
          }
     }
   eqd->speed = speed;
   eqd->time = total_time;
   eqd->cb = cb;
   eqd->data = (void*)data;

   e->queue = eina_list_append(e->queue, eqd);
   return eqd;
   (void)efx_speed_str;
}
