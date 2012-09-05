#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

static Evas *e;
static Eina_List *objs = NULL;

static void _move1(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r);
static Eina_Bool _start(Evas_Object *r);

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
_restart(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   evas_object_del(r);
   r = rect_create();
   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, r);
}

static void
_del(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   evas_object_del(r);
}

static void
_finish(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   switch (eina_list_count(objs))
     {
      case 4:
        efx_move(r, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ -100, -100 }, 1.0, _restart, NULL);
        break;
      case 3:
        efx_move(r, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ 550, -100 }, 1.0, _del, NULL);
        break;
      case 2:
        efx_move(r, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ -100, 550 }, 1.0, _del, NULL);
        break;
      default:
        efx_move(r, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ 550, 550 }, 1.0, _del, NULL);
     }
   objs = eina_list_remove_list(objs, objs);
}

static void
_center(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   efx_rotate(r, EFX_EFFECT_SPEED_LINEAR, 360, NULL, 1.5, NULL, NULL);
   efx_move(r, EFX_EFFECT_SPEED_DECELERATE, &(Evas_Point){ 203, 203 }, 1.5, _finish, NULL);
}


static void
_move4(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   switch (eina_list_count(objs))
     {
      case 3: /* red */
        {
           Evas_Object *o;

           o = rect_create();
           _move1(NULL, NULL, o);
        }
        efx_move(r, EFX_EFFECT_SPEED_DECELERATE, &(Evas_Point){ 25, 25 }, 1.0, _center, NULL);
        break;
      default: /* green */
        _center(NULL, NULL, r);
     }
}

static void
_move3(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   switch (eina_list_count(objs))
     {
      case 2: /* red */
        {
           Evas_Object *o;

           o = rect_create();
           _move1(NULL, NULL, o);
        }
      case 3: /* green */
        efx_move(r, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ 25, 350 }, 1.0, _move4, NULL);
        break;
      default: /* blue */
        _center(NULL, NULL, r);
     }
}

static void
_move2(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   switch (eina_list_count(objs))
     {
      case 1: /* red */
        {
           Evas_Object *o;

           o = rect_create();
           _move1(NULL, NULL, o);
        }
      case 2: /* green */
      case 3: /* blue */
        efx_move(r, EFX_EFFECT_SPEED_DECELERATE, &(Evas_Point){ 350, 350 }, 1.0, _move3, NULL);
        break;
      default: /* black */
        _center(NULL, NULL, r);
     }
}

static void
_move1(void *data __UNUSED__, Efx_Map_Data *emd __UNUSED__, Evas_Object *r)
{
   efx_move(r, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){ 350, 25 }, 1.0, _move2, NULL);
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
   ecore_evas_title_set(ee, "move");
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_show(ee);
   e = ecore_evas_get(ee);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 450, 450);
   evas_object_show(r);

   r = rect_create();

   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, r);
   ecore_main_loop_begin();
   return 0;
}
