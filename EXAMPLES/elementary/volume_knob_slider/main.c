/*
	Simple elementary app with a slider to test the volume style
	Raoul Hecky <raoul.hecky@gmail.com>
*/

#include <Elementary.h>

int
elm_main(int argc, char **argv)
{
   Evas_Object *win, *bg, *slider, *box;

   //Load our custom edj file with our slider style
   elm_theme_extension_add(NULL, "./slider.edj");

   //Window
   win = elm_win_add(NULL, "volume-slider", ELM_WIN_BASIC);
   elm_win_title_set(win, "Volume Know style Example");
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_resize(win, 400, 400);
   evas_object_show(win);

   //Background
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   evas_object_size_hint_min_set(bg, 200, 200);
   elm_bg_color_set(bg, 10, 10, 10);

   //Box
   box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   //The slider
   slider = elm_slider_add(win);
   elm_object_style_set(slider, "knob_volume");
   evas_object_size_hint_weight_set(slider, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(box, slider);
   evas_object_show(slider);

   elm_run();

   return 0;
}
ELM_MAIN();

