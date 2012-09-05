#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static Evas *e;
static Eina_List *objs = NULL;

static Eina_Bool _start(void *d __UNUSED__);
static void _del(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r);
static void _center(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r);
static void _create(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r);

static Efx_End_Cb callbacks[4][6] =
{
   {_create, _create, _create, _center, NULL, _del},
   {NULL, NULL, _center, NULL, NULL, _del},
   {NULL, _center, NULL, NULL, NULL, _del},
   {_center, NULL, NULL, NULL, NULL, _del}
};

static Evas_Point points[] =
{
   {-100, -100},
   {550, -100},
   {-100, 550},
   {550, 550}
};

static Evas_Object *
rect_create(void)
{
   Evas_Object *r;
   r = evas_object_rectangle_add(e);
   objs = eina_list_append(objs, r);
   switch (eina_list_count(objs))
     {
      case 1:
        evas_object_color_set(r, 255, 0, 0, 255);
        break;
      case 2:
        evas_object_color_set(r, 0, 255, 0, 255);
        break;
      case 3:
        evas_object_color_set(r, 0, 0, 255, 255);
        break;
      default:
        evas_object_color_set(r, 0, 0, 0, 255);
     }
   evas_object_resize(r, 72, 72);
   evas_object_move(r, 25, 25);
   evas_object_show(r);
   return r;
}

static void
_del(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   objs = eina_list_remove_list(objs, objs);
   evas_object_del(r);
   if (objs) return;
   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, NULL);
}

static void
_center(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   efx_rotate(r, EFX_EFFECT_SPEED_LINEAR, 360, NULL, 1.5, NULL, NULL);
}

static void
_create(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   r = rect_create();
   efx_queue_append(r, EFX_EFFECT_SPEED_ACCELERATE,
     EFX_QUEUED_EFFECT(EFX_EFFECT_MOVE(350, 25)),
     1.0, callbacks[eina_list_count(objs) - 1][0], NULL);
   if (eina_list_count(objs) < 4)
     efx_queue_append(r, EFX_EFFECT_SPEED_DECELERATE,
       EFX_QUEUED_EFFECT(EFX_EFFECT_MOVE(350, 350)),
       1.0, callbacks[eina_list_count(objs) - 1][1], NULL);
   if (eina_list_count(objs) < 3)
     efx_queue_append(r, EFX_EFFECT_SPEED_ACCELERATE,
       EFX_QUEUED_EFFECT(EFX_EFFECT_MOVE(25, 350)),
       1.0, callbacks[eina_list_count(objs) - 1][2], NULL);
   if (eina_list_count(objs) < 2)
     efx_queue_append(r, EFX_EFFECT_SPEED_DECELERATE,
       EFX_QUEUED_EFFECT(EFX_EFFECT_MOVE(25, 25)),
       1.0, callbacks[eina_list_count(objs) - 1][3], NULL);
   efx_queue_append(r, EFX_EFFECT_SPEED_DECELERATE,
     EFX_QUEUED_EFFECT(EFX_EFFECT_MOVE(203, 203)),
     1.5, callbacks[eina_list_count(objs) - 1][4], NULL);
   efx_queue_append(r, EFX_EFFECT_SPEED_ACCELERATE,
     EFX_QUEUED_EFFECT(EFX_EFFECT_MOVE(points[eina_list_count(objs) - 1].x, points[eina_list_count(objs) - 1].y)),
     1.0, callbacks[eina_list_count(objs) - 1][5], NULL);
   efx_queue_run(r);
}


static Eina_Bool
_start(void *d __UNUSED__)
{
   _create(NULL, NULL, NULL);
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
   ecore_evas_title_set(ee, "queue");
   ecore_evas_show(ee);
   e = ecore_evas_get(ee);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 450, 450);
   evas_object_show(r);

   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, NULL);
   ecore_main_loop_begin();
   return 0;
}
