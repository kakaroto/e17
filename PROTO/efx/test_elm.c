#include <Efx.h>
#include <Elementary.h>

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

#define WEIGHT evas_object_size_hint_weight_set
#define ALIGN evas_object_size_hint_align_set
#define EXPAND(X) WEIGHT((X), EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
#define FILL(X) ALIGN((X), EVAS_HINT_FILL, EVAS_HINT_FILL)

static double button_factor = 1.0;
static double box_factor = 1.0;

static void
_button1(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   static unsigned char spin;

   if (spin++ % 2 == 0)
     efx_spin_start(obj, 75);
   else
     efx_spin_start(obj, -140);
}

static void
_zoom_end(void *data __UNUSED__, double value, Evas_Object *obj __UNUSED__)
{
   button_factor = value;
}

static void
_zoom_end2(void *data __UNUSED__, double value, Evas_Object *obj __UNUSED__)
{
   box_factor = value;
}

static void
_button2(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   static unsigned char zoom;

   if (zoom++ % 2 == 0)
     efx_zoom(obj, EFX_EFFECT_SPEED_SINUSOIDAL, button_factor, 1.5, NULL, 3.0, _zoom_end, NULL);
   else
     efx_zoom(obj, EFX_EFFECT_SPEED_DECELERATE, button_factor, 0.3, NULL, 3.0, _zoom_end, NULL);
}

static void
_button3(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   efx_rotate(data, EFX_EFFECT_SPEED_ACCELERATE, 270, 3.0, NULL, NULL);
}

static void
_button4(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   static unsigned char zoom;

   if (!zoom)
     efx_zoom(data, EFX_EFFECT_SPEED_SINUSOIDAL, 1, 1.5, NULL, 3.0, _zoom_end2, NULL);
   else if (zoom % 2 == 0)
     efx_zoom(data, EFX_EFFECT_SPEED_SINUSOIDAL, 0, 1.5, NULL, 3.0, _zoom_end2, NULL);
   else
     efx_zoom(data, EFX_EFFECT_SPEED_DECELERATE, 0, 0.3, NULL, 3.0, _zoom_end2, NULL);
   zoom++;
}

int
main(int argc, char *argv[])
{
   Evas_Object *win, *o, *box;

   efx_init();
   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);
   elm_init(argc, argv);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   win = elm_win_add(NULL, "test-elm", ELM_WIN_BASIC);
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_resize(win, 500, 500);
   evas_object_show(win);

   o = elm_bg_add(win);
   EXPAND(o);
   elm_win_resize_object_add(win, o);
   evas_object_show(o);

   box = elm_box_add(win);
   EXPAND(box);
   FILL(box);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   o = elm_button_add(win);
   elm_object_text_set(o, "test1");
   elm_box_pack_end(box, o);
   evas_object_smart_callback_add(o, "clicked", (Evas_Smart_Cb)_button1, NULL);
   evas_object_show(o);

   o = elm_button_add(win);
   elm_object_text_set(o, "test2");
   elm_box_pack_end(box, o);
   evas_object_smart_callback_add(o, "clicked", (Evas_Smart_Cb)_button2, NULL);
   evas_object_show(o);

   o = elm_button_add(win);
   elm_object_text_set(o, "test3");
   elm_box_pack_end(box, o);
   evas_object_smart_callback_add(o, "clicked", (Evas_Smart_Cb)_button3, box);
   evas_object_show(o);

   o = elm_button_add(win);
   elm_object_text_set(o, "test4");
   elm_box_pack_end(box, o);
   evas_object_smart_callback_add(o, "clicked", (Evas_Smart_Cb)_button4, box);
   evas_object_show(o);

   ecore_main_loop_begin();
   return 0;
}
