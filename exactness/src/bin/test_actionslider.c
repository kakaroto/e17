#include <Elementary.h>
#include "tsuite.h"

static void _pos_selected_cb(void *data __UNUSED__, Evas_Object *obj, void *event_info)
{
   printf("Selection: %s\n", (char *)event_info);
   printf("Label selected: %s\n", elm_actionslider_selected_label_get(obj));
}

static void
_position_change_magnetic_cb(void *data __UNUSED__, Evas_Object * obj, void *event_info)
{
   if (!strcmp((char *)event_info, "left"))
     elm_actionslider_magnet_pos_set(obj, ELM_ACTIONSLIDER_LEFT);
   else if (!strcmp((char *)event_info, "right"))
     elm_actionslider_magnet_pos_set(obj, ELM_ACTIONSLIDER_RIGHT);
}

static void
_magnet_enable_disable_cb(void *data __UNUSED__, Evas_Object *obj, void *event_info)
{
   if (!strcmp((char *)event_info, "left"))
      elm_actionslider_magnet_pos_set(obj, ELM_ACTIONSLIDER_CENTER);
   else if (!strcmp((char *)event_info, "right"))
      elm_actionslider_magnet_pos_set(obj, ELM_ACTIONSLIDER_NONE);
}

enum _api_state
{
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   switch(api->state)
     { /* Put all api-changes under switch */
      case API_STATE_LAST:
         break;
      default:
         return;
     }
}

static void
_api_bt_clicked(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{  /* Will add here a SWITCH command containing code to modify test-object */
   /* in accordance a->state value. */
   api_data *a = data;
   char str[128];

   printf("clicked event on API Button: api_state=<%d>\n", a->state);
   a->state++;
   set_api_state(a);
   sprintf(str, "Next API function (%u)", a->state);
   elm_object_text_set(a->bt, str);
   elm_object_disabled_set(a->bt, a->state == API_STATE_LAST);
}

TEST_START(test_actionslider)
{
   Evas_Object *bg, *bx, *as;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, 0);
   evas_object_show(bg);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   as = elm_actionslider_add(win);
   evas_object_size_hint_weight_set(as, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(as, EVAS_HINT_FILL, 0);
   elm_actionslider_indicator_pos_set(as, ELM_ACTIONSLIDER_RIGHT);
   elm_actionslider_magnet_pos_set(as, ELM_ACTIONSLIDER_RIGHT);
   elm_object_part_text_set(as, "left", "Snooze");
   elm_object_part_text_set(as, "center", NULL);
   elm_object_part_text_set(as, "right", "Stop");
   elm_actionslider_enabled_pos_set(as, ELM_ACTIONSLIDER_LEFT |
                                    ELM_ACTIONSLIDER_RIGHT);
   evas_object_smart_callback_add(as, "pos_changed",
                                  _position_change_magnetic_cb, NULL);
   evas_object_smart_callback_add(as, "selected", _pos_selected_cb, NULL);
   evas_object_show(as);
   elm_box_pack_end(bx, as);

   as = elm_actionslider_add(win);
   evas_object_size_hint_weight_set(as, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(as, EVAS_HINT_FILL, 0);
   elm_actionslider_indicator_pos_set(as, ELM_ACTIONSLIDER_CENTER);
   elm_actionslider_magnet_pos_set(as, ELM_ACTIONSLIDER_CENTER);
   elm_object_part_text_set(as, "left", "Snooze");
   elm_object_part_text_set(as, "center", NULL);
   elm_object_part_text_set(as, "right", "Stop");
   elm_actionslider_enabled_pos_set(as, ELM_ACTIONSLIDER_LEFT |
                                    ELM_ACTIONSLIDER_RIGHT);
   evas_object_smart_callback_add(as, "selected", _pos_selected_cb, NULL);
   evas_object_show(as);
   elm_box_pack_end(bx, as);

   as = elm_actionslider_add(win);
   elm_object_style_set(as, "bar");
   evas_object_size_hint_weight_set(as, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(as, EVAS_HINT_FILL, 0);
   elm_actionslider_indicator_pos_set(as, ELM_ACTIONSLIDER_LEFT);
   elm_actionslider_magnet_pos_set(as, ELM_ACTIONSLIDER_CENTER|
                                   ELM_ACTIONSLIDER_RIGHT);
   elm_actionslider_enabled_pos_set(as, ELM_ACTIONSLIDER_CENTER |
                                    ELM_ACTIONSLIDER_RIGHT);
   elm_object_part_text_set(as, "left", NULL);
   elm_object_part_text_set(as, "center", "Accept");
   elm_object_part_text_set(as, "right", "Reject");
   evas_object_smart_callback_add(as, "selected", _pos_selected_cb, NULL);
   evas_object_show(as);
   elm_box_pack_end(bx, as);

   as = elm_actionslider_add(win);
   elm_object_style_set(as, "bar");
   evas_object_size_hint_weight_set(as, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(as, EVAS_HINT_FILL, 0);
   elm_actionslider_indicator_pos_set(as, ELM_ACTIONSLIDER_LEFT);
   elm_actionslider_magnet_pos_set(as, ELM_ACTIONSLIDER_LEFT);
   elm_object_part_text_set(as, "left", NULL);
   elm_object_part_text_set(as, "center", "Accept");
   elm_object_part_text_set(as, "right", "Reject");
   elm_object_text_set(as, "Go");
   evas_object_smart_callback_add(as, "pos_changed",
                                  _position_change_magnetic_cb, NULL);
   evas_object_smart_callback_add(as, "selected", _pos_selected_cb, NULL);
   evas_object_show(as);
   elm_box_pack_end(bx, as);


   as = elm_actionslider_add(win);
   evas_object_size_hint_weight_set(as, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(as, EVAS_HINT_FILL, 0);
   elm_actionslider_indicator_pos_set(as, ELM_ACTIONSLIDER_LEFT);
   elm_actionslider_magnet_pos_set(as, ELM_ACTIONSLIDER_ALL);
   elm_object_part_text_set(as, "left", "Left");
   elm_object_part_text_set(as, "center", "Center");
   elm_object_part_text_set(as, "right", "Right");
   elm_object_text_set(as, "Go");
   evas_object_smart_callback_add(as, "selected", _pos_selected_cb, NULL);
   evas_object_show(as);
   elm_box_pack_end(bx, as);

   as = elm_actionslider_add(win);
   evas_object_size_hint_weight_set(as, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(as, EVAS_HINT_FILL, 0);
   elm_actionslider_indicator_pos_set(as, ELM_ACTIONSLIDER_CENTER);
   elm_actionslider_magnet_pos_set(as, ELM_ACTIONSLIDER_CENTER);
   elm_object_part_text_set(as, "left", "Enable");
   elm_object_part_text_set(as, "center", "Magnet");
   elm_object_part_text_set(as, "right", "Disable");
   evas_object_smart_callback_add(as, "pos_changed",
                                  _magnet_enable_disable_cb, NULL);
   evas_object_smart_callback_add(as, "selected", _pos_selected_cb, NULL);
   evas_object_show(as);
   elm_box_pack_end(bx, as);

   evas_object_resize(win, 320, 400);
   evas_object_show(win);
}
TEST_END
