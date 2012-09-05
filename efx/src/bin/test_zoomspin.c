#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static Eina_Bool _spin(void *data);
static void _zoom(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *obj);

static void
_zoom4(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *obj)
{
   printf("starting zoom #4 from %g\n", e->zoom);
   efx_zoom(obj, EFX_EFFECT_SPEED_SINUSOIDAL, e->rotation, 2.0, NULL, 3.0, _zoom, NULL);
}

static void
_zoom3(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *obj)
{
   printf("starting zoom #3 from %g\n", e->zoom);
   efx_zoom(obj, EFX_EFFECT_SPEED_ACCELERATE, e->zoom, 0.1, NULL, 3.0, _zoom4, NULL);
}

static void
_zoom2(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *obj)
{
   printf("starting zoom #2 from %g\n", e->zoom);
   efx_zoom(obj, EFX_EFFECT_SPEED_DECELERATE, e->zoom, 0.5, NULL, 3.0, _zoom3, NULL);
}

static void
_zoom(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *obj)
{
   printf("starting zoom from %g\n", e ? e->zoom : 1.0);
   efx_zoom(obj, EFX_EFFECT_SPEED_LINEAR, e ? e->zoom : 1.0, 1.5, NULL, 1.0, _zoom2, NULL);
}

static Eina_Bool
_spin4(void *data)
{
   printf("starting spin #4\n");
   efx_spin_start(data, -200, NULL);
   ecore_timer_add(5.0, _spin, data);
   return EINA_FALSE;
}

static Eina_Bool
_spin3(void *data)
{
   printf("starting spin #3\n");
   efx_spin_start(data, 100, NULL);
   ecore_timer_add(5.0, _spin4, data);
   return EINA_FALSE;
}

static Eina_Bool
_spin2(void *data)
{
   efx_spin_stop(data);
   printf("starting spin #2\n");
   efx_spin_start(data, -5, NULL);
   ecore_timer_add(5.0, _spin3, data);
   return EINA_FALSE;
}

static Eina_Bool
_spin(void *data)
{
   printf("starting spin\n");
   efx_spin_start(data, 15, NULL);
   ecore_timer_add(5.0, _spin2, data);
   return EINA_FALSE;
}

static Eina_Bool
_start(void *data)
{
   _zoom(NULL, NULL, data);
   _spin(data);
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
   ecore_evas_title_set(ee, "zoomspin");
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
   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, r);
   ecore_main_loop_begin();
   return 0;
}
