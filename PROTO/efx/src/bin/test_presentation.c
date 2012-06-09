#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static Evas *e;

static void _del(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r);
static Evas_Object *_create(void);
static Eina_Bool _start(Evas_Object *bg);

static void
_del(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   evas_object_del(r);
   r = _create();
   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, r);
}

static Evas_Object *
_create(void)
{
   Evas_Object *bg, *efl, *is, *cool, *img;

   bg = evas_object_rectangle_add(e);
   evas_object_color_set(bg, 0, 0, 0, 255);
   evas_object_resize(bg, 4500, 4500);
   evas_object_show(bg);
   bg = evas_object_rectangle_add(e);
   evas_object_resize(bg, 450, 450);
   evas_object_show(bg);
   efx_pan_init(bg);

   efl = evas_object_text_add(e);
   evas_object_text_font_set(efl, "Sans:style=Bold", 32);
   evas_object_color_set(efl, 0, 0, 0, 255);
   evas_object_move(efl, 10, 400);
   evas_object_text_text_set(efl, "EFL");
   evas_object_show(efl);

   is = evas_object_text_add(e);
   evas_object_text_font_set(is, "Sans:style=Bold", 32);
   evas_object_color_set(is, 0, 0, 0, 255);
   evas_object_move(is, 50, 100);
   evas_object_text_text_set(is, "IS");
   evas_object_show(is);
   efx_rotate(is, EFX_EFFECT_SPEED_DECELERATE, -90, NULL, 0, NULL, NULL);

   cool = evas_object_text_add(e);
   evas_object_text_font_set(cool, "Sans:style=Bold", 32);
   evas_object_color_set(cool, 0, 0, 0, 255);
   evas_object_move(cool, 200, 200);
   evas_object_text_text_set(cool, "COOL");
   evas_object_show(cool);
   efx_rotate(cool, EFX_EFFECT_SPEED_DECELERATE, 270, NULL, 0, NULL, NULL);

   img = evas_object_image_add(e);
   evas_object_image_file_set(img, BUILD_DATADIR "/logo_black_128.png", NULL);
   if (evas_object_image_load_error_get(img))
     evas_object_image_file_set(img, PACKAGE_DATADIR "/logo_black_128.png", NULL);
   if (evas_object_image_load_error_get(img))
     evas_object_image_file_set(img, "data/logo_black_128.png", NULL);
   if (evas_object_image_load_error_get(img))
     {
        fprintf(stderr, "Could not locate image!\n");
        exit(-1);
     }
   {
      int w, h;
      evas_object_image_size_get (img, &w, &h);
      evas_object_image_fill_set(img, 0, 0, w, h);
      evas_object_image_size_set(img, w, h);
      evas_object_resize(img, w, h);
   }
   evas_object_move(img, 300, 300);
   evas_object_show(img);

   efx_follow(bg, efl);
   efx_follow(bg, is);
   efx_follow(bg, cool);
   efx_follow(bg, img);

   efx_pan(bg, EFX_EFFECT_SPEED_DECELERATE, EFX_POINT(-215, 200), 0.0, NULL, NULL);
   efx_zoom(bg, EFX_EFFECT_SPEED_DECELERATE, 1.0, 2.0, EFX_POINT(300, 100), 0.0, NULL, NULL);
   return bg;
}

static Eina_Bool
_start(Evas_Object *bg)
{
   Efx_Queue_Data *eqd;
   eqd = efx_queue_append(bg, EFX_EFFECT_SPEED_SINUSOIDAL,
     EFX_QUEUED_EFFECT(EFX_EFFECT_ROTATE(90, NULL)), 2.0, NULL, NULL);
   efx_queue_effect_attach(eqd, EFX_EFFECT_SPEED_SINUSOIDAL,
     EFX_QUEUED_EFFECT(EFX_EFFECT_PAN(100, -300)), 2.0, NULL, NULL);
   eqd = efx_queue_append(bg, EFX_EFFECT_SPEED_DECELERATE,
     EFX_QUEUED_EFFECT(EFX_EFFECT_ROTATE(-360, NULL)), 2.0, NULL, NULL);
   efx_queue_effect_attach(eqd, EFX_EFFECT_SPEED_DECELERATE,
     EFX_QUEUED_EFFECT(EFX_EFFECT_PAN(100, 100)), 2.0, NULL, NULL);
   eqd = efx_queue_append(bg, EFX_EFFECT_SPEED_DECELERATE,
     EFX_QUEUED_EFFECT(EFX_EFFECT_ROTATE(-90, NULL)), 2.0, NULL, NULL);
   efx_queue_effect_attach(eqd, EFX_EFFECT_SPEED_DECELERATE,
     EFX_QUEUED_EFFECT(EFX_EFFECT_PAN(200, 200)), 2.0, NULL, NULL);
   efx_queue_effect_attach(eqd, EFX_EFFECT_SPEED_DECELERATE,
     EFX_QUEUED_EFFECT(EFX_EFFECT_ZOOM(0, 1.0, NULL)), 2.0, NULL, NULL);
   eqd = efx_queue_append(bg, EFX_EFFECT_SPEED_LINEAR,
     EFX_QUEUED_EFFECT(EFX_EFFECT_PAN(-185, -200)), 2.0, NULL, NULL);
   efx_queue_effect_attach(eqd, EFX_EFFECT_SPEED_LINEAR,
     EFX_QUEUED_EFFECT(EFX_EFFECT_ZOOM(0, 0.5, NULL)), 2.0, _del, NULL);
   efx_queue_run(bg);
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
   Evas_Object *bg;

   efx_init();
   ecore_evas_init();
   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);
   ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 450, 450);
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_title_set(ee, "presentation");
   ecore_evas_show(ee);
   e = ecore_evas_get(ee);
   bg = _create();

   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, bg);
   ecore_main_loop_begin();
   return 0;
}
