#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static void _fade(void *data, Efx_Map_Data *e, Evas_Object *obj);

static void
_fade4(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   printf("starting fade #4\n");
   efx_fade(obj, EFX_EFFECT_SPEED_SINUSOIDAL, &(Efx_Color){0, 255, 0}, 255, 3.0, _fade, NULL);
}

static void
_fade3(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   printf("starting fade #3\n");
   efx_fade(obj, EFX_EFFECT_SPEED_ACCELERATE, &(Efx_Color){0, 0, 0}, 255, 3.0, _fade4, NULL);
}

static void
_fade2(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   printf("starting fade #2\n");
   efx_fade(obj, EFX_EFFECT_SPEED_DECELERATE, &(Efx_Color){0, 0, 0}, 0, 3.0, _fade3, NULL);
}

static void
_fade(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   printf("starting fade\n");
   efx_fade(obj, EFX_EFFECT_SPEED_LINEAR, &(Efx_Color){255, 0, 0}, 255, 3.0, _fade2, NULL);
}

static Eina_Bool
_start(void *data)
{
   _fade(NULL, NULL, data);
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
   Evas *e;
   Evas_Object *r;

   if (!efx_init())
     return -1;

   if (!ecore_evas_init())
     goto shutdown_efx;

   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);

   ee = ecore_evas_new(NULL, 10, 10, 0, 0, NULL);
   if (!ee)
     goto shutdown_ecore_evas;

   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_title_set(ee, "Fade");

   e = ecore_evas_get(ee);

   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 450, 450);
   evas_object_show(r);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 300, 300);
   evas_object_move(r, (450 / 2) - (300 / 2), (450 / 2) - (300 / 2));
   evas_object_show(r);

   ecore_evas_resize(ee, 450, 450);
   ecore_evas_show(ee);

   ecore_timer_add(1.0, _start, r);

   ecore_main_loop_begin();

   ecore_evas_shutdown();
   efx_shutdown();

   return 0;

 shutdown_ecore_evas:
   ecore_evas_shutdown();
 shutdown_efx:
   efx_shutdown();

   return -1;
}
