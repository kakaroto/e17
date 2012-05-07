#include "efx_private.h"

struct Efx_Queue_Data
{
   EFX *e;
   Efx_Queued_Effect effect;
   Efx_Effect_Speed speed;
   double time;
   Efx_End_Cb cb;
   void *data;

   Eina_List *subeffects;
   void *effect_data;
   Eina_Bool active : 1;
};

static void
_queue_advance(Efx_Queue_Data *eqd)
{
   Efx_Queue_Data *run;
   Eina_List *l;

   INF("queue_advance: %p", eqd->e->obj);
   switch (eqd->effect.type)
     {
      case EFX_EFFECT_TYPE_ROTATE:
        efx_rotate(eqd->e->obj, eqd->speed, eqd->effect.effect.rotation.degrees, eqd->effect.effect.rotation.center, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = eqd->e->rotate_data;
        break;
      case EFX_EFFECT_TYPE_ZOOM:
        efx_zoom(eqd->e->obj, eqd->speed, eqd->effect.effect.zoom.start, eqd->effect.effect.zoom.end, eqd->effect.effect.zoom.center, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = eqd->e->zoom_data;
        break;
      case EFX_EFFECT_TYPE_MOVE:
        efx_move(eqd->e->obj, eqd->speed, &eqd->effect.effect.movement.point, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = eqd->e->move_data;
        break;
      case EFX_EFFECT_TYPE_PAN:
        efx_pan(eqd->e->obj, eqd->speed, &eqd->effect.effect.movement.point, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = eqd->e->pan_data;
        break;
      case EFX_EFFECT_TYPE_FADE:
        efx_fade(eqd->e->obj, eqd->speed, &eqd->effect.effect.fade.color, eqd->effect.effect.fade.alpha, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = eqd->e->fade_data;
        break;
      case EFX_EFFECT_TYPE_RESIZE:
      default:
        efx_resize(eqd->e->obj, eqd->speed, eqd->effect.effect.resize.point, eqd->effect.effect.resize.w, eqd->effect.effect.resize.h, eqd->time, eqd->cb, eqd->data);
        eqd->effect_data = eqd->e->resize_data;
     }
   eqd->active = EINA_TRUE;
   EINA_LIST_FOREACH(eqd->subeffects, l, run)
     _queue_advance(run);
}

void
efx_queue_process(EFX *e)
{
   Efx_Queue_Data *eqd;

   eqd = eina_list_data_get(e->queue);
   if (!eqd) return;
   if (eqd->active) return;

   _queue_advance(eqd);
}

void
eqd_free(Efx_Queue_Data *eqd)
{
   Efx_Queue_Data *sub;
   if (!eqd) return;
   if (eqd->effect.type == EFX_EFFECT_TYPE_ROTATE)
     free(eqd->effect.effect.rotation.center);
   else if (eqd->effect.type == EFX_EFFECT_TYPE_ZOOM)
     free(eqd->effect.effect.zoom.center);
   else if (eqd->effect.type == EFX_EFFECT_TYPE_RESIZE)
     free(eqd->effect.effect.resize.point);
   EINA_LIST_FREE(eqd->subeffects, sub)
     eqd_free(sub);
   free(eqd);
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
   eqd_free(eqd);
   return !!e->queue;
}

Efx_Queue_Data *
eqd_new(EFX *e, Efx_Effect_Speed speed, const Efx_Queued_Effect *effect, double total_time, Efx_End_Cb cb, const void *data)
{
   Efx_Queue_Data *eqd;

   eqd = calloc(1, sizeof(Efx_Queue_Data));
   memcpy(&eqd->effect, effect, sizeof(Efx_Queued_Effect));
   eqd->e = e;
   if (effect->type == EFX_EFFECT_TYPE_ROTATE)
     {
        if (effect->effect.rotation.center)
          {
             eqd->effect.effect.rotation.center = malloc(sizeof(Evas_Point));
             if (!eqd->effect.effect.rotation.center) goto error;
             memcpy(eqd->effect.effect.rotation.center, effect->effect.rotation.center, sizeof(Evas_Point));
          }
     }
   else if (effect->type == EFX_EFFECT_TYPE_ZOOM)
     {
        if (effect->effect.zoom.center)
          {
             eqd->effect.effect.zoom.center = malloc(sizeof(Evas_Point));
             if (!eqd->effect.effect.zoom.center) goto error;
             memcpy(eqd->effect.effect.zoom.center, effect->effect.zoom.center, sizeof(Evas_Point));
          }
     }
   else if (effect->type == EFX_EFFECT_TYPE_RESIZE)
     {
        if (effect->effect.resize.point)
          {
             eqd->effect.effect.resize.point = malloc(sizeof(Evas_Point));
             if (!eqd->effect.effect.resize.point) goto error;
             memcpy(eqd->effect.effect.resize.point, effect->effect.resize.point, sizeof(Evas_Point));
          }
     }
   eqd->speed = speed;
   eqd->time = total_time;
   eqd->cb = cb;
   eqd->data = (void*)data;
   return eqd;
error:
   free(eqd);
   efx_free(e);
   return NULL;
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
efx_queue_append(Evas_Object *obj, Efx_Effect_Speed speed, const Efx_Queued_Effect *effect, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Queue_Data *eqd;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(effect, NULL);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(total_time >= 0.0, NULL);
   if (effect->type > EFX_EFFECT_TYPE_RESIZE) return NULL;
   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   eqd = eqd_new(e, speed, effect, total_time, cb, data);
   EINA_SAFETY_ON_NULL_RETURN_VAL(eqd, NULL);

   e->queue = eina_list_append(e->queue, eqd);
   return eqd;
   (void)efx_speed_str;
}

EAPI Efx_Queue_Data *
efx_queue_prepend(Evas_Object *obj, Efx_Effect_Speed speed, const Efx_Queued_Effect *effect, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Queue_Data *eqd, *eqd2;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(effect, NULL);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(total_time >= 0.0, NULL);
   if (effect->type > EFX_EFFECT_TYPE_RESIZE) return NULL;
   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   eqd = eqd_new(e, speed, effect, total_time, cb, data);
   EINA_SAFETY_ON_NULL_RETURN_VAL(eqd, NULL);

   if (e->queue)
     {
        eqd2 = eina_list_data_get(e->queue);
        if (eqd2->active)
          e->queue = eina_list_append_relative_list(e->queue, eqd, e->queue);
        else
          e->queue = eina_list_prepend(e->queue, eqd);
     }
   else
     e->queue = eina_list_append(e->queue, eqd);
   return eqd;
   (void)efx_speed_str;
}

EAPI void
efx_queue_promote(Evas_Object *obj, Efx_Queue_Data *eqd)
{
   EFX *e;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   EINA_SAFETY_ON_NULL_RETURN(eqd);
   e = evas_object_data_get(obj, "efx-data");
   EINA_SAFETY_ON_NULL_RETURN(e);
   EINA_SAFETY_ON_NULL_RETURN(e->queue);
   EINA_SAFETY_ON_TRUE_RETURN(eqd->active);

   if (e->queue->data == eqd) return;

   e->queue = eina_list_remove(e->queue, eqd);
   e->queue = eina_list_append_relative_list(e->queue, eqd, e->queue);
}

EAPI void
efx_queue_demote(Evas_Object *obj, Efx_Queue_Data *eqd)
{
   EFX *e;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   EINA_SAFETY_ON_NULL_RETURN(eqd);
   e = evas_object_data_get(obj, "efx-data");
   EINA_SAFETY_ON_NULL_RETURN(e);
   EINA_SAFETY_ON_NULL_RETURN(e->queue);
   EINA_SAFETY_ON_TRUE_RETURN(eqd->active);

   if (eina_list_last(e->queue)->data == eqd) return;

   e->queue = eina_list_demote_list(e->queue, eina_list_data_find_list(e->queue, eqd));
}

EAPI void
efx_queue_delete(Evas_Object *obj, Efx_Queue_Data *eqd)
{
   EFX *e;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   EINA_SAFETY_ON_NULL_RETURN(eqd);
   e = evas_object_data_get(obj, "efx-data");
   EINA_SAFETY_ON_NULL_RETURN(e);
   EINA_SAFETY_ON_NULL_RETURN(e->queue);
   EINA_SAFETY_ON_TRUE_RETURN(eqd->active);

   e->queue = eina_list_remove(e->queue, eqd);
   eqd_free(eqd);
}

EAPI void
efx_queue_clear(Evas_Object *obj)
{
   EFX *e;
   Efx_Queue_Data *eqd;
   Eina_List *l;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   e = evas_object_data_get(obj, "efx-data");
   EINA_SAFETY_ON_NULL_RETURN(e);
   if (!e->queue) return;

   eqd = e->queue->data;
   if (eqd->active)
     l = e->queue->next;
   else
     l = e->queue;
   EINA_LIST_FREE(l, eqd)
     eqd_free(eqd);
}

EAPI Eina_Bool
efx_queue_effect_attach(Efx_Queue_Data *eqd, Efx_Effect_Speed speed, const Efx_Queued_Effect *effect, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Queue_Data *sub;

   EINA_SAFETY_ON_NULL_RETURN_VAL(eqd, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(effect, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(total_time >= 0.0, EINA_FALSE);
   if (effect->type > EFX_EFFECT_TYPE_RESIZE) return EINA_FALSE;
   e = eqd->e;

   sub = eqd_new(e, speed, effect, total_time, cb, data);
   EINA_SAFETY_ON_NULL_RETURN_VAL(sub, EINA_FALSE);

   eqd->subeffects = eina_list_append(eqd->subeffects, sub);
   return EINA_TRUE;
   (void)efx_speed_str;
}
