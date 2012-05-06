#include "efx_private.h"

typedef struct Efx_Fade_Data
{
   EFX *e;
   Efx_Effect_Speed speed;
   Ecore_Animator *anim;
   Evas_Object *clip;
   Efx_Color start;
   Efx_Color color;
   unsigned char alpha[2];
   Efx_End_Cb cb;
   void *data;
} Efx_Fade_Data;


static void
_clip_setup(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   EFX *e;
   Efx_Fade_Data *efd;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) return;
   efd = e->fade_data;
   efx_clip_setup(obj, efd->clip);
}

static void
_obj_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   EFX *e;
   Efx_Fade_Data *efd;

   e = evas_object_data_get(obj, "efx-data");
   if (!e) return; /* bug? */
   efd = e->fade_data;
   if (efd)
     {
        if (efd->anim) ecore_animator_del(efd->anim);
        e->fade_data = NULL;
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_RESIZE, (Evas_Object_Event_Cb)_clip_setup, efd);
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_MOVE, (Evas_Object_Event_Cb)_clip_setup, efd);
        if (efd->clip)
          {
             evas_object_clip_unset(efd->clip);
             evas_object_del(efd->clip);
             efd->clip = NULL;
          }
        free(efd);
     }
   if ((!e->owner) && (!e->followers)) efx_free(e);
}

static Eina_Bool
_fade_cb(Efx_Fade_Data *efd, double pos)
{
   double factor;
   unsigned char r, g, b, a;

   if (pos != 1.0)
     {
        r = efd->start.r;
        g = efd->start.g;
        b = efd->start.b;
        a = efd->alpha[0];
        factor = ecore_animator_pos_map(pos, efd->speed, 0, 0);
        if (efd->color.r != efd->start.r)
          r -= lround(factor * ((int)efd->start.r - (int)efd->color.r));
        if (efd->color.g != efd->start.g)
          g -= lround(factor * ((int)efd->start.g - (int)efd->color.g));
        if (efd->color.b != efd->start.b)
          b -= lround(factor * ((int)efd->start.b - (int)efd->color.b));
        if (efd->alpha[0] != efd->alpha[1])
          a -= lround(factor * ((int)efd->alpha[0] - (int)efd->alpha[1]));
        evas_object_color_set(efd->clip, MIN(r, a), MIN(g, a), MIN(b, a), a);
//        _color_debug(efd->clip);
        return EINA_TRUE;
     }
   else
     /* lround will usually be off by 1 at the end, so we manually set this here */
     evas_object_color_set(efd->clip, MIN(efd->color.r, efd->alpha[1]), MIN(efd->color.g, efd->alpha[1]), MIN(efd->color.b, efd->alpha[1]), efd->alpha[1]);

   efd->anim = NULL;
   EFX_QUEUE_CHECK(efd);
   return EINA_TRUE;
}

static void
_fade_stop(Evas_Object *obj, Eina_Bool reset)
{
   EFX *e;
   Efx_Fade_Data *efd;

   e = evas_object_data_get(obj, "efx-data");
   if ((!e) || (!e->fade_data)) return;
   efd = e->fade_data;
   if (reset)
     {
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, efd);
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_RESIZE, (Evas_Object_Event_Cb)_clip_setup, efd);
        evas_object_event_callback_del_full(obj, EVAS_CALLBACK_MOVE, (Evas_Object_Event_Cb)_clip_setup, efd);
        if (efx_queue_complete(efd->e, efd))
          efx_queue_process(efd->e);
        _obj_del(efd, NULL, NULL, NULL);
        INF("reset faded object %p", obj);
     }
   else
     {
        INF("stopped faded object %p", obj);
        if (efd->anim) ecore_animator_del(efd->anim);
        if (efx_queue_complete(efd->e, efd))
          efx_queue_process(efd->e);
     }
}

EAPI Eina_Bool
efx_fade(Evas_Object *obj, Efx_Effect_Speed speed, Efx_Color *ec, unsigned char alpha, double total_time, Efx_End_Cb cb, const void *data)
{
   EFX *e;
   Efx_Fade_Data *efd;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_FALSE);
   e = evas_object_data_get(obj, "efx-data");
   if (!e) e = efx_new(obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   efd = e->fade_data;
   if (!efd)
     {
        e->fade_data = efd = calloc(1, sizeof(Efx_Fade_Data));
        EINA_SAFETY_ON_NULL_RETURN_VAL(efd, EINA_FALSE);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_obj_del, e->fade_data);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_RESIZE, (Evas_Object_Event_Cb)_clip_setup, e->fade_data);
        evas_object_event_callback_add(obj, EVAS_CALLBACK_MOVE, (Evas_Object_Event_Cb)_clip_setup, e->fade_data);
        efd->clip = evas_object_rectangle_add(evas_object_evas_get(obj));
        evas_object_clip_set(obj, efd->clip);
        efx_clip_setup(obj, efd->clip);
        efx_follow(obj, efd->clip);
        evas_object_show(efd->clip);
     }

   efd->e = e;
   efd->alpha[1] = alpha;
   efd->cb = cb;
   efd->data = (void*)data;
   evas_object_color_get(efd->clip, (int*)&efd->start.r, (int*)&efd->start.g, (int*)&efd->start.b, (int*)&alpha);
   efd->alpha[0] = (unsigned char)alpha;
   if (ec)
     {
        efd->color.r = ec->r;
        efd->color.g = ec->g;
        efd->color.b = ec->b;
     }
   else efd->color = (Efx_Color){255, 255, 255};
   INF("fade: %p || %d/%d/%d/%d => %d/%d/%d/%d %s over %gs", obj, efd->start.r, efd->start.g, efd->start.b, efd->alpha[0], efd->color.r, efd->color.g, efd->color.b, efd->alpha[1], efx_speed_str[speed], total_time);
   if (total_time)
     efd->anim = ecore_animator_timeline_add(total_time, (Ecore_Timeline_Cb)_fade_cb, efd);
   else
     _fade_cb(efd, 1.0);

   return EINA_TRUE;
}

EAPI void
efx_fade_reset(Evas_Object *obj)
{
   _fade_stop(obj, EINA_TRUE);
}

EAPI void
efx_fade_stop(Evas_Object *obj)
{
   _fade_stop(obj, EINA_FALSE);
}
