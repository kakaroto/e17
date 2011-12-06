#include <Elementary.h>
#include "tsuite.h"

static void
icon_clicked(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("clicked!\n");
}

TEST_START(test_icon)
{
   Evas_Object *ic;
   char buf[PATH_MAX];

   elm_win_alpha_set(win, 1);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_no_scale_set(ic, 1);
   evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(ic, 0.5, 0.5);
   elm_win_resize_object_add(win, ic);
   evas_object_show(ic);

   evas_object_smart_callback_add(ic, "clicked", icon_clicked, NULL);

   evas_object_show(win);
}
TEST_END
