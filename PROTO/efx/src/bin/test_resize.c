#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static Evas *e;

static Eina_Bool _start(void *d __UNUSED__);
static void _del(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r);

static Evas_Object *
rect_create(void)
{
   Evas_Object *r;
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 72, 72);
   evas_object_move(r, 25, 350);
   evas_object_show(r);
   return r;
}

static void
_del(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   evas_object_del(r);
   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, NULL);
}

static Eina_Bool
_start(void *d __UNUSED__)
{
   Efx_Queue_Data *eqd;
   Evas_Object *r;
   r = rect_create();
   efx_fade(r, EFX_EFFECT_SPEED_ACCELERATE, &(Efx_Color){255, 0, 0}, 255, 0.0, NULL, NULL);
   efx_queue_append(r, EFX_EFFECT_SPEED_LINEAR,
     EFX_QUEUED_EFFECT(EFX_EFFECT_RESIZE(EFX_POINT(25, 25), 150, 400)), 2.0, NULL, NULL);
   eqd = efx_queue_append(r, EFX_EFFECT_SPEED_LINEAR,
     EFX_QUEUED_EFFECT(EFX_EFFECT_RESIZE(NULL, 72, 72)), 2.0, NULL, NULL);
   efx_queue_effect_attach(eqd, EFX_EFFECT_SPEED_LINEAR,
     EFX_QUEUED_EFFECT(EFX_EFFECT_FADE(0, 0, 0, 0)), 2.0, _del, NULL);
   efx_queue_effect_attach(eqd, EFX_EFFECT_SPEED_LINEAR,
     EFX_QUEUED_EFFECT(EFX_EFFECT_ROTATE(360, NULL)), 2.0, NULL, NULL);
   efx_queue_run(r);
   return EINA_FALSE;
}

static void
_end(Ecore_Evas *ee __UNUSED__)
{
   ecore_main_loop_quit();
}

int
main(void)
{
   Ecore_Evas *ee;
   Evas_Object *r;

   efx_init();
   ecore_evas_init();
   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);
   ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 450, 450);
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_title_set(ee, "resize");
   ecore_evas_show(ee);
   e = ecore_evas_get(ee);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 450, 450);
   evas_object_show(r);

   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, NULL);
   ecore_main_loop_begin();
   return 0;
}
