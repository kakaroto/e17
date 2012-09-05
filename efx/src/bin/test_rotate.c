#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static void _rotate(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj);

static void
_rotate4(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   printf("starting rotate #4\n");
   efx_rotate(obj, EFX_EFFECT_SPEED_SINUSOIDAL, 3600, NULL, 3.0, _rotate, NULL);
}

static void
_rotate3(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   printf("starting rotate #3\n");
   efx_rotate(obj, EFX_EFFECT_SPEED_ACCELERATE, -1080, NULL, 3.0, _rotate4, NULL);
}

static void
_rotate2(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   printf("starting rotate #2\n");
   efx_rotate(obj, EFX_EFFECT_SPEED_DECELERATE, 720, NULL, 3.0, _rotate3, NULL);
}

static void
_rotate(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   printf("starting rotate\n");
   efx_rotate(obj, EFX_EFFECT_SPEED_LINEAR, 360, NULL, 3.0, _rotate2, NULL);
}

static Eina_Bool
_start(void *data)
{
   _rotate(NULL, NULL, data);
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

   efx_init();
   ecore_evas_init();
   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);
   ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 450, 450);
   ecore_evas_title_set(ee, "rotate");
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_show(ee);
   e = ecore_evas_get(ee);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 450, 450);
   evas_object_show(r);
   r = evas_object_rectangle_add(e);
   evas_object_color_set(r, 255, 0, 0, 255);
   evas_object_resize(r, 300, 300);
   evas_object_move(r, (450 / 2) - (300 / 2), (450 / 2) - (300 / 2));
   evas_object_show(r);
   ecore_timer_add(1.0, _start, r);
   ecore_main_loop_begin();
   return 0;
}
