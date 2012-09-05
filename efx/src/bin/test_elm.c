#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <Efx.h>
#include <Elementary.h>

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
     efx_spin_start(obj, 75, NULL);
   else
     efx_spin_start(obj, -140, NULL);
}

static void
_zoom_end(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *obj __UNUSED__)
{
   button_factor = e->zoom;
}

static void
_zoom_end2(void *data __UNUSED__, Efx_Map_Data *e, Evas_Object *obj __UNUSED__)
{
   box_factor = e->zoom;
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
   efx_rotate(data, EFX_EFFECT_SPEED_ACCELERATE, 270, NULL, 3.0, NULL, NULL);
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

static void
_button5(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   static unsigned char spin;
   Evas_Coord x, y;
   Evas_Object *win;
   win = elm_object_top_widget_get(obj);
   evas_object_geometry_get(win, NULL, NULL, &x, &y);

   if (spin++ % 2 == 0)
     efx_spin_start(obj, -30, &(Evas_Point){x / 2, y / 2});
   else
     efx_spin_start(obj, 30, &(Evas_Point){x / 2, y / 2});
}

static void
_flip(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_flip_go(data, ELM_FLIP_ROTATE_X_CENTER_AXIS);
}

static void
_bird(void *data)
{
   Evas_Coord x, y;
   Evas_Object *win;
   win = elm_object_top_widget_get(data);
   evas_object_geometry_get(win, NULL, NULL, &x, &y);
   efx_spin_start(data, 30, &(Evas_Point){x / 2, y / 2});
}

static void
_flip_start(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   if (elm_flip_front_visible_get(obj))
     evas_object_hide(data);
   else
     evas_object_show(data);
}

int
main(int argc, char *argv[])
{
   Evas_Object *win, *flip, *o, *box;

   efx_init();
   eina_log_domain_level_set("efx", EINA_LOG_LEVEL_DBG);
   elm_init(argc, argv);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   win = elm_win_add(NULL, "elm", ELM_WIN_BASIC);
   elm_win_title_set(win, "elm");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_resize(win, 500, 500);
   evas_object_show(win);

   o = elm_bg_add(win);
   EXPAND(o);
   FILL(o);
   elm_bg_color_set(o, 0, 0, 0);
   elm_win_resize_object_add(win, o);
   evas_object_show(o);

   box = elm_box_add(win);
   EXPAND(box);
   FILL(box);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   flip = elm_flip_add(win);
   EXPAND(flip);
   FILL(flip);
   elm_box_pack_end(box, flip);

   box = elm_box_add(win);
   EXPAND(box);
   FILL(box);
   elm_object_part_content_set(flip, "front", box);
   evas_object_show(box);

   o = elm_box_add(win);
   EXPAND(o);
   FILL(o);
   elm_box_pack_end(box, o);
   evas_object_show(o);
   box = o;

   o = elm_button_add(win);
   elm_object_text_set(o, "flip");
   elm_box_pack_end(box, o);
   evas_object_smart_callback_add(o, "clicked", (Evas_Smart_Cb)_flip, flip);
   evas_object_show(o);

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

   o = elm_button_add(win);
   elm_object_text_set(o, "I'm a bird!");
   evas_object_smart_callback_add(o, "clicked", (Evas_Smart_Cb)_button5, NULL);
   evas_object_smart_callback_add(flip, "animate,begin", (Evas_Smart_Cb)_flip_start, o);
   evas_object_resize(o, 100, 35);
   evas_object_move(o, 150, 125);
   ecore_job_add(_bird, o);
   evas_object_show(o);

   o = elm_button_add(win);
   elm_object_text_set(o, "flip");
   elm_object_part_content_set(flip, "back", o);
   evas_object_smart_callback_add(o, "clicked", (Evas_Smart_Cb)_flip, flip);
   evas_object_show(o);

   evas_object_show(flip);
   ecore_main_loop_begin();
   return 0;
}
