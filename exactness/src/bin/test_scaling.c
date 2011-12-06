#include <Elementary.h>
#include "tsuite.h"
TEST_START(test_scaling)
{
   Evas_Object *bg, *bx, *bt;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Scale: 0.5");
   elm_object_scale_set(bt, 0.5);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Scale: 0.75");
   elm_object_scale_set(bt, 0.75);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Scale: 1.0");
   elm_object_scale_set(bt, 1.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Scale: 1.5");
   elm_object_scale_set(bt, 1.5);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Scale: 2.0");
   elm_object_scale_set(bt, 2.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Scale: 3.0");
   elm_object_scale_set(bt, 3.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   evas_object_resize(win, 320, 320);
   evas_object_show(win);
}
TEST_END

TEST_START(test_scaling2)
{
   Evas_Object *bg, *bx, *fr, *lb;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   fr = elm_frame_add(win);
   elm_object_scale_set(fr, 0.5);
   elm_object_text_set(fr, "Scale: 0.5");
   lb = elm_label_add(win);
   elm_object_text_set(lb,
		       "Parent frame scale<br>"
		       "is 0.5. Child should<br>"
		       "inherit it.");
   elm_object_content_set(fr, lb);
   evas_object_show(lb);
   elm_box_pack_end(bx, fr);
   evas_object_show(fr);

   fr = elm_frame_add(win);
   elm_object_text_set(fr, "Scale: 1.0");
   lb = elm_label_add(win);
   elm_object_text_set(lb,
		       "Parent frame scale<br>"
		       "is 1.0. Child should<br>"
		       "inherit it.");
   elm_object_content_set(fr, lb);
   evas_object_show(lb);
   elm_object_scale_set(fr, 1.0);
   elm_box_pack_end(bx, fr);
   evas_object_show(fr);

   fr = elm_frame_add(win);
   elm_object_text_set(fr, "Scale: 2.0");
   lb = elm_label_add(win);
   elm_object_text_set(lb,
		       "Parent frame scale<br>"
		       "is 2.0. Child should<br>"
		       "inherit it.");
   elm_object_content_set(fr, lb);
   evas_object_show(lb);
   elm_object_scale_set(fr, 2.0);
   elm_box_pack_end(bx, fr);
   evas_object_show(fr);

   evas_object_resize(win, 320, 320);
   evas_object_show(win);
}
TEST_END
