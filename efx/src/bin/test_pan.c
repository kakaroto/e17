#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore_Evas.h>

static Evas_Coord w, h;
static Eina_Bool _pan(void *o);
static void _pan2(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *o);

static Evas_Object *
rect_create(Evas *e)
{
   Evas_Object *r;
   r = evas_object_rectangle_add(e);
   evas_object_color_set(r, 255, 0, 0, 255);
   evas_object_resize(r, w * .25, h * .25);
   evas_object_show(r);
   return r;
}

static void
_pan3(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *o)
{
   efx_pan(o, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){200, 200}, 3.0, NULL, NULL);
   efx_zoom(o, EFX_EFFECT_SPEED_ACCELERATE, e ? e->zoom : 1.0, 1.5, NULL, 3.0, _pan2, NULL);
}

static void
_pan2(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *o)
{
   efx_pan(o, EFX_EFFECT_SPEED_DECELERATE, &(Evas_Point){-200, -200}, 3.0, NULL, NULL);
   efx_zoom(o, EFX_EFFECT_SPEED_DECELERATE, e->zoom, 1.0, NULL, 3.0, _pan3 , NULL);
}

static Eina_Bool
_pan(void *o)
{
   _pan3(NULL, NULL, o);
   return EINA_FALSE;
}

static Eina_Bool
_spin(void *o)
{
   efx_spin_start(o, 120, NULL);
   //efx_rotate(o, EFX_EFFECT_SPEED_LINEAR, 1080, NULL, 9.0, NULL, NULL);
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
   Evas_Object *o, *r;

   efx_init();
   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);
   ecore_evas_init();

   ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 450, 450);
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_title_set(ee, "Pan");
   ecore_evas_show(ee);

   e = ecore_evas_get(ee);

   r = evas_object_rectangle_add(e);
   evas_object_color_set(r, 0, 0, 0, 0);
   evas_object_resize(r, 99999, 99999);
   evas_object_move(r, -49999, -49999);
   evas_object_show(r);

   o = evas_object_image_add(e);
   evas_object_image_file_set(o, BUILD_DATADIR "/ss.png", NULL);
   if (evas_object_image_load_error_get(o))
     evas_object_image_file_set(o, PACKAGE_DATADIR "/ss.png", NULL);
   if (evas_object_image_load_error_get(o))
     {
        fprintf(stderr, "Could not locate image!\n");
        return -1;
     }
   evas_object_image_size_get(o, &w, &h);
   evas_object_image_fill_set(o, 0, 0, w, h);
   evas_object_image_size_set(o, w, h);
   evas_object_resize(o, w, h);
   evas_object_move(o, 0, 0);
   evas_object_show(o);
   efx_pan_init(o);


   r = rect_create(e);
   evas_object_move(r, 100, 100);
   efx_follow(o, r);
 
   ecore_timer_add(1.0, _pan, o);
   ecore_timer_add(1.0, _spin, r);

   ecore_main_loop_begin();

   return 0;
}
