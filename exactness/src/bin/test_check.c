#include <Elementary.h>
#include "tsuite.h"

TEST_START(test_check)
{
   Evas_Object *bg, *bx, *ic, *ck;
   char buf[PATH_MAX];

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(ck, "Icon sized to check");
   elm_object_content_set(ck, ic);
   elm_check_state_set(ck, 1);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   ck = elm_check_add(win);
   elm_object_text_set(ck, "Icon no scale");
   elm_object_content_set(ck, ic);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);
   evas_object_show(ic);

   ck = elm_check_add(win);
   elm_object_text_set(ck, "Label Only");
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(ck, "Disabled check");
   elm_object_content_set(ck, ic);
   elm_check_state_set(ck, 1);
   elm_box_pack_end(bx, ck);
   elm_object_disabled_set(ck, 1);
   evas_object_show(ck);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   ck = elm_check_add(win);
   elm_object_content_set(ck, ic);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);
   evas_object_show(ic);

   evas_object_show(win);
}
TEST_END
