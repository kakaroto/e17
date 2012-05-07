#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static void _rotate(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj);

static void
_move2(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   Evas_Coord x;

   evas_object_geometry_get(obj, &x, NULL, NULL, NULL);
   if (!x)
     efx_move(obj, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ 100, (450 / 2) - (72 / 2) }, 1.0, _rotate, NULL);
   else
     efx_move(obj, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ 278, (450 / 2) - (72 / 2) }, 1.0, _rotate, NULL);
}

static void
_move1(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   Evas_Coord x;
   efx_realize(obj);
   evas_object_geometry_get(obj, &x, NULL, NULL, NULL);
   if (x > 225)
     efx_move(obj, EFX_EFFECT_SPEED_DECELERATE, &(Evas_Point){ 450 - 72, (450 / 2) - (72 / 2) }, 1.0, _move2, NULL);
   else
     efx_move(obj, EFX_EFFECT_SPEED_DECELERATE, &(Evas_Point){ 0, (450 / 2) - (72 / 2) }, 1.0, _move2, NULL);
}

static void
_rotate(void *data __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   Evas_Coord x;
   evas_object_geometry_get(obj, &x, NULL, NULL, NULL);
   /* a reset here is necessary to prevent successive rotations from
    * starting in the wrong place. the alternative, which is more complex but preserves orientation,
    * would be to use a container box for the movement and set a follow on the object for the rotates
    */
   efx_rotate_reset(obj);
   efx_rotate(obj, EFX_EFFECT_SPEED_SINUSOIDAL, 180, &(Evas_Point){ 225, 225 }, 1.0, _move1, NULL);
}

static Eina_Bool
_start(void *data)
{
   _move2(NULL, NULL, data);
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
   Evas *e;
   Ecore_Evas *ee;
   Evas_Object *r;

   efx_init();
   ecore_evas_init();
   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);
   ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 450, 450);
   ecore_evas_title_set(ee, "path");
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_show(ee);
   e = ecore_evas_get(ee);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 450, 450);
   evas_object_show(r);

   r = evas_object_rectangle_add(e);
   evas_object_color_set(r, 255, 0, 0, 255);
   evas_object_resize(r, 200, 200);
   evas_object_move(r, (450 / 2) - (200 / 2), (450 / 2) - (200 / 2));
   evas_object_show(r);

   r = evas_object_rectangle_add(e);
   evas_object_color_set(r, 0, 0, 255, 255);
   evas_object_resize(r, 72, 72);
   evas_object_move(r, 0, (450 / 2) - (72 / 2));
   evas_object_show(r);

   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, r);
   ecore_main_loop_begin();
   return 0;
}
