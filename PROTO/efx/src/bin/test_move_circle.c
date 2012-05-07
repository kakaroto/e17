#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static Evas *e;

static Evas_Object *
rect_create(void)
{
   Evas_Object *r;
   r = evas_object_rectangle_add(e);
   evas_object_color_set(r, 255, 0, 0, 255);
   evas_object_resize(r, 36, 36);
   evas_object_move(r, 25, (450 / 2) - (36 / 2));
   evas_object_show(r);
   return r;
}

static void
_move1(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   efx_move_circle(r, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ 225, 225 }, 360, 2.0, NULL, NULL);
}

static Eina_Bool
_start(Evas_Object *r)
{
   _move1(NULL, NULL, r);
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
   ecore_evas_title_set(ee, "move_circle");
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_show(ee);
   e = ecore_evas_get(ee);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 450, 450);
   evas_object_show(r);

   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 400, 36);
   evas_object_move(r, 25, (450 / 2) - (36 / 2));
   evas_object_color_set(r, 0, 255, 0, 255);
   evas_object_show(r);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 36, 400);
   evas_object_move(r, (450 / 2) - (36 / 2), 25);
   evas_object_color_set(r, 0, 255, 0, 255);
   evas_object_show(r);

   r = rect_create();

   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, r);
   ecore_main_loop_begin();
   return 0;
}
