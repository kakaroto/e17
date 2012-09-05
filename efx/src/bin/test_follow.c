#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#define RECT_SIZE 75

static Eina_Bool _start2(void *r);
static void _up(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj);

static Evas_Object *
rect_create(Evas *e)
{
   Evas_Object *r;
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, RECT_SIZE, RECT_SIZE);
   evas_object_show(r);
   return r;
}

static void
_vanish(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   evas_object_del(obj);
}

static Eina_Bool
_notice(void *r3)
{
   Evas *e = evas_object_evas_get(r3);
   Evas_Object *t, *r;
   Evas_Coord x, y;
   Eina_List *l;

   l = efx_followers_get(r3);
   r = l->data;
   eina_list_free(l);

   evas_object_geometry_get(r, &x, &y, NULL, NULL);

   t = evas_object_textblock_add(e);
   {
      Evas_Textblock_Style *ts;
      ts = evas_textblock_style_new();
      evas_textblock_style_set(ts, "DEFAULT='font=Sans:style=Bold font_size=14 color=#FFF'"
              "br='\n'"
              "ps='ps'"
              "tab='\t'");
      evas_object_textblock_style_set(t, ts);
   }
   evas_object_textblock_text_markup_set(t, "Rotation center overridden by owner object");
   evas_object_resize(t, 400, 20);
   evas_object_move(t, x, y - 30 - 20);
   evas_object_show(t);
   efx_unfollow(r);
   efx_follow(r, t);
   /* we want the text to follow the movement of r, so we must do the effect before
    * setting r to follow r3
    */
   efx_fade(t, EFX_EFFECT_SPEED_LINEAR, &(Efx_Color){255, 0, 0}, 255, 0, NULL, NULL);
   efx_fade(t, EFX_EFFECT_SPEED_DECELERATE, &(Efx_Color){0, 0, 0}, 0, 3.0, _vanish, NULL);
   efx_follow(r3, r);
   return EINA_FALSE;
}

static void
_rotate4(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_rotate_reset(obj);
   efx_rotate(obj, EFX_EFFECT_SPEED_DECELERATE, -360, NULL, 2.0, NULL, NULL);
   ecore_timer_add(2.0, _notice, obj);
   ecore_timer_add(5.0, _start2, obj);
}

static void
_rotate3(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_rotate(obj, EFX_EFFECT_SPEED_LINEAR, 360, NULL, 1.3, _rotate4, NULL);
}

static void
_rotate2(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_rotate(obj, EFX_EFFECT_SPEED_LINEAR, -360, NULL, 1.3, _rotate3, NULL);
}

static void
_rotate(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_rotate(obj, EFX_EFFECT_SPEED_ACCELERATE, 360, NULL, 2.0, _rotate2, NULL);
}

static Eina_Bool
_start2(void *r)
{
   _rotate(NULL, NULL, r);
   return EINA_FALSE;
}

static void
_reset3(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_move(obj, EFX_EFFECT_SPEED_LINEAR, &(Evas_Point){0, 400 - RECT_SIZE}, 1.0, _up, NULL);
}

static void
_reset2(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_rotate(obj, EFX_EFFECT_SPEED_LINEAR, 360, &(Evas_Point){200, 200}, 1.0, _reset3, NULL);
}

static void
_reset(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_move(obj, EFX_EFFECT_SPEED_LINEAR, &(Evas_Point){200, 400 - RECT_SIZE}, 1.0, _reset2, NULL);
}

static void
_down2(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_move(obj, EFX_EFFECT_SPEED_DECELERATE, &(Evas_Point){400, 400 - RECT_SIZE}, 2.0, _reset, NULL);
}

static void
_up2(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_move(obj, EFX_EFFECT_SPEED_LINEAR, &(Evas_Point){300, 0}, 1.3, _down2, NULL);
}

static void
_down(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_move(obj, EFX_EFFECT_SPEED_LINEAR, &(Evas_Point){200, 400 - RECT_SIZE}, 1.3, _up2, NULL);
}

static void
_up(void *d __UNUSED__, Efx_Map_Data *e __UNUSED__, Evas_Object *obj)
{
   efx_move(obj, EFX_EFFECT_SPEED_ACCELERATE, &(Evas_Point){100, 0}, 2.0, _down, NULL);
}

static Eina_Bool
_start(void *r)
{
   _up(NULL, NULL, r);
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
   Evas_Object *r, *r2, *r3;

   efx_init();
   ecore_evas_init();
   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);
   ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 600, 400);
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_title_set(ee, "Follow");
   ecore_evas_show(ee);
   e = ecore_evas_get(ee);
   r = evas_object_rectangle_add(e);
   evas_object_resize(r, 600, 400);
   evas_object_show(r);

   r = rect_create(e);
   evas_object_color_set(r, 255, 0, 0, 255);
   evas_object_move(r, 0, 400 - RECT_SIZE);

   r2 = rect_create(e);
   evas_object_color_set(r2, 0, 0, 255, 255);
   evas_object_move(r2, RECT_SIZE + 100, 400 - RECT_SIZE);

   r3 = rect_create(e);
   evas_object_color_set(r3, 0, 0, 0, 255);
   evas_object_move(r3, 0, 0);

   efx_follow(r, r2);
   efx_follow(r3, r);

   ecore_timer_add(1.0, (Ecore_Task_Cb)_start, r);
   ecore_timer_add(1.0, (Ecore_Task_Cb)_start2, r3);
   ecore_main_loop_begin();
   return 0;
}
