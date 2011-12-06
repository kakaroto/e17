#include <Elementary.h>
#include "tsuite.h"

static void
_bt_repeated(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   static int count;
   char buf[16];

   snprintf(buf, sizeof(buf), "count=%d", count++);
   if (count >= 10000)
     count = 0;
   elm_object_text_set(obj, buf);
}

static void
_bt_clicked(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   int param = (int)(data);

   printf("clicked event on Button:%d\n", param);
}

static void
_bt_unpressed(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   int param = (int)(data);

   printf("unpressed event on Button:%d\n", param);
}

enum _api_state
{
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api __UNUSED__)
{
   return;
}

static void
_api_bt_clicked(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{  /* Will add here a SWITCH command containing code to modify test-object */
   /* in accordance a->state value. */
   api_data *a = data;
   char str[128];

   printf("clicked event on API Button: api_state=<%d>\n", a->state);
   set_api_state(a);
   a->state++;
   sprintf(str, "Next API function (%u)", a->state);
   elm_object_text_set(a->bt, str);
   elm_object_disabled_set(a->bt, a->state == API_STATE_LAST);
}

TEST_START(test_button)
{
   Evas_Object *bg, *bx, *bxx, *scr,*ic, *bt;
   char buf[PATH_MAX];

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);
   scr = elm_scroller_add(win);
   elm_scroller_bounce_set(scr, EINA_FALSE, EINA_TRUE);
   elm_scroller_policy_set(scr, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
   evas_object_show(scr);
   evas_object_size_hint_weight_set(scr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_object_content_set(scr, bx);
   api->data = bx;
   evas_object_show(bx);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, bx);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   bt = elm_button_add(win);
   elm_object_text_set(bt, "Icon sized to button");
   elm_object_content_set(bt, ic);
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)1);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)1);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   evas_object_show(ic);

   ic = elm_icon_add(bx);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   bt = elm_button_add(win);
   elm_object_text_set(bt, "Icon no scale");
   elm_object_content_set(bt, ic);
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)2);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)2);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   bt = elm_button_add(win);
   elm_object_text_set(bt, "Disabled Button");
   elm_object_content_set(bt, ic);
   elm_object_disabled_set(bt, 1);
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)3);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)3);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   bt = elm_button_add(win);
   elm_object_content_set(bt, ic);
   elm_object_disabled_set(bt, 1);
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)4);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)4);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   evas_object_show(ic);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Label Only");
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)5);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)5);
   elm_box_pack_end(bx, bt);
   evas_object_smart_callback_add(bt, "repeated", _bt_repeated, NULL);
   elm_button_autorepeat_set(bt, 1);
   elm_button_autorepeat_initial_timeout_set(bt, 2.0);
   elm_button_autorepeat_gap_timeout_set(bt, 0.5);
   evas_object_show(bt);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   bt = elm_button_add(win);
   elm_object_content_set(bt, ic);
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)6);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)6);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   evas_object_show(ic);

   bt = elm_button_add(win);
   elm_object_style_set(bt, "anchor");
   elm_object_text_set(bt, "Anchor style");
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)7);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)7);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   bt = elm_button_add(win);
   elm_object_style_set(bt, "anchor");
   elm_object_content_set(bt, ic);
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)8);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)8);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   bt = elm_button_add(win);
   elm_object_access_info_set(bt, "This is some custom text describing this widget");
   elm_object_style_set(bt, "anchor");
   elm_object_content_set(bt, ic);
   elm_object_disabled_set(bt, 1);
   evas_object_smart_callback_add(bt, "clicked", _bt_clicked, (void *)9);
   evas_object_smart_callback_add(bt, "unpressed", _bt_unpressed, (void *)9);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   evas_object_show(ic);
}
TEST_END
