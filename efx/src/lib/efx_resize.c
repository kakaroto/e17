#include "efx_private.h"

typedef struct Efx_Resize_Data
{
   EFX *e;
   Efx_Effect_Speed speed;
   Ecore_Animator *anim;
   int w, h;
   int start_w, start_h;
   Efx_End_Cb cb;
   void *data;
} Efx_Resize_Data;

static void
_obj_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   EFX *e;
   Efx_Resize_Data *erd;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) return; /* bug? */
   erd = e->resize_data;
   if (erd)
     {
        if (erd->anim) ecore_animator_del(erd->anim);
        e->resize_data = NULL;
        free(erd);
     }
   if ((!e->owner) && (!e->followers)) efx_free(e);
}

static Eina_Bool
_resize_cb(Efx_Resize_Data *erd, double pos)
{
   double factor;

   if (pos != 1.0)
     {
        factor = ecore_animator_pos_map(pos, erd->speed, 0, 0);
        evas_object_resize(erd->e->obj, lround(factor * (erd->w - erd->start_w)) + erd->start_w, lround(factor * (erd->h - erd->start_h)) + erd->start_h);
        return EINA_TRUE;
     }
   else
     /* lround will usually be off by 1 at the end, so we manually set this here */
     evas_object_resize(erd->e->obj, erd->w, erd->h);

   erd->anim = NULL;
   EFX_QUEUE_CHECK(erd);
   return EINA_TRUE;
}

static void
_resize_stop(Evas_Object *obj, Eina_Bool reset)
{
   EFX *e;
   Efx_Resize_Data *erd;

   e = evas_object_data_get(obj, "efx-data");
   if ((!e) || (!e->resize_data)) return;
   erd = e->resize_data;
   if (reset)
     {
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, erd);
        if (efx_queue_complete(erd->e, erd))
          efx_queue_process(erd->e);
        _obj_del(erd, NULL, NULL, NULL);
        INF("reset resized object %p", obj);
     }
   else
     {
        INF("stopped resized object %p", obj);
        if (erd->anim) ecore_animator_del(erd->anim);
        if (efx_queue_complete(erd->e, erd))
          efx_queue_process(erd->e);
     }
}

EAPI Eina_Bool
efx_resize(Evas_Object *obj, Efx_Effect_Speed speed, const Evas_Point *position, int w, int h, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Resize_Data *erd;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(w < 0, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(h < 0, EINA_FALSE);
   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   erd = e->resize_data;
   if (!erd)
     {
        e->resize_data = erd = calloc(1, sizeof(Efx_Resize_Data));
        EINA_SAFETY_ON_NULL_RETURN_VAL(erd, EINA_FALSE);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->resize_data);
     }

   erd->e = e;
   erd->w = w;
   erd->h = h;
   erd->cb = cb;
   erd->data = (void*)data;
   evas_object_geometry_get(obj, NULL, NULL, &erd->start_w, &erd->start_h);
   INF("resize: %p || %dx%d => %dx%d %s over %gs", obj, erd->start_w, erd->start_h, w, h, efx_speed_str[speed], total_time);
   if (position)
     {
        if (!efx_move(obj, speed, position, total_time, NULL, NULL))
          {
             evas_object_event_callback_del_full(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->resize_data);
             free(erd);
             e->resize_data = NULL;
             efx_free(e);
             return EINA_FALSE;
          }
     }
   if (total_time)
     erd->anim = ecore_animator_timeline_add(total_time, (Ecore_Timeline_Cb)_resize_cb, erd);
   else
     _resize_cb(erd, 1.0);

   return EINA_TRUE;
}

EAPI void
efx_resize_reset(Evas_Object *obj)
{
   _resize_stop(obj, EINA_TRUE);
}

EAPI void
efx_resize_stop(Evas_Object *obj)
{
   _resize_stop(obj, EINA_FALSE);
}
