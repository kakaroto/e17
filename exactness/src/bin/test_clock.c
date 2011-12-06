#include <Elementary.h>
#include "tsuite.h"

enum _api_state
{
   CLOCK_HIDE_SEC,
   CLOCK_SHOW_AM_PM,
   CLOCK_SHOW_SEC,
   CLOCK_EDIT_MIN,
   CLOCK_EDIT_HOUR,
   CLOCK_EDIT_ALL,
   CLOCK_EDIT_ALL_ARMY,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   const Eina_List *items = elm_box_children_get(api->data);
   Evas_Object *ck = eina_list_nth(items, 0);
   if(!eina_list_count(items))
     return;

   /* use elm_box_children_get() to get list of children */
   switch(api->state)
     { /* Put all api-changes under switch */
        case CLOCK_HIDE_SEC:
           elm_clock_show_seconds_set(ck, EINA_FALSE);
           break;

          case CLOCK_SHOW_AM_PM:
           elm_clock_show_am_pm_set(ck,  EINA_TRUE);
           break;

          case CLOCK_SHOW_SEC:
           elm_clock_show_seconds_set(ck, EINA_TRUE);
           break;

          case CLOCK_EDIT_MIN:
           elm_clock_edit_set(ck, ELM_CLOCK_MIN_DECIMAL | ELM_CLOCK_MIN_UNIT);
           break;

          case CLOCK_EDIT_HOUR:
           elm_clock_edit_set(ck, ELM_CLOCK_NONE);
           elm_clock_edit_set(ck, ELM_CLOCK_HOUR_DECIMAL | ELM_CLOCK_HOUR_UNIT);
           break;

          case CLOCK_EDIT_ALL:
           elm_clock_edit_set(ck, ELM_CLOCK_ALL);
           break;

          case CLOCK_EDIT_ALL_ARMY:
           elm_clock_show_am_pm_set(ck,  EINA_FALSE);
           break;

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
   set_api_state(a);
   a->state++;
   sprintf(str, "Next API function (%u)", a->state);
   elm_object_text_set(a->bt, str);
   elm_object_disabled_set(a->bt, a->state == API_STATE_LAST);
}

TEST_START(test_clock)
{
   Evas_Object *bg, *bx, *bxx, *ck;
   unsigned int digedit;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);
   api->data = bx;
   evas_object_show(bx);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, EINA_TRUE);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, bx);

   ck = elm_clock_add(win);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(win);
   elm_clock_show_am_pm_set(ck, 1);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(win);
   elm_clock_show_seconds_set(ck, 1);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(win);
   elm_clock_show_seconds_set(ck, 1);
   elm_clock_show_am_pm_set(ck, 1);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(win);
   elm_clock_show_seconds_set(ck, 1);
   elm_clock_show_am_pm_set(ck, 0);
   elm_clock_time_set(ck, 23, 59, 57);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(win);
   elm_clock_edit_set(ck, 1);
   elm_clock_show_seconds_set(ck, 1);
   elm_clock_show_am_pm_set(ck, 1);
   elm_clock_time_set(ck, 10, 11, 12);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   ck = elm_clock_add(win);
   elm_clock_show_seconds_set(ck, 1);
   elm_clock_edit_set(ck, 1);
   digedit = ELM_CLOCK_HOUR_UNIT | ELM_CLOCK_MIN_UNIT | ELM_CLOCK_SEC_UNIT;
   elm_clock_digit_edit_set(ck, digedit);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   evas_object_show(win);
}
TEST_END

static void
_edit_bt_clicked(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *ck = data;

   if (!elm_clock_edit_get(ck)) {
	elm_object_text_set(obj, "Done");
	elm_clock_edit_set(ck, 1);
	return;
   }
   elm_object_text_set(obj, "Edit");
   elm_clock_edit_set(ck, 0);
}

static void
_hmode_bt_clicked(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *ck = data;

   if (!elm_clock_show_am_pm_get(ck)) {
	elm_object_text_set(obj, "24h");
	elm_clock_show_am_pm_set(ck, 1);
	return;
   }
   elm_object_text_set(obj, "12h");
   elm_clock_show_am_pm_set(ck, 0);
}

TEST_START(test_clock2)
{
   Evas_Object *bg, *bx, *bxx, *hbx, *ck, *bt;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);
   api->data = bx;
   evas_object_show(bx);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, bx);

   ck = elm_clock_add(win);
   elm_clock_time_set(ck, 0, 15, 3);
   elm_clock_digit_edit_set(ck, ELM_CLOCK_NONE);
   elm_clock_show_seconds_set(ck, 1);
   elm_box_pack_end(bx, ck);
   evas_object_show(ck);

   hbx = elm_box_add(win);
   evas_object_size_hint_weight_set(hbx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_horizontal_set(hbx, EINA_TRUE);
   elm_box_pack_end(bx, hbx);
   evas_object_show(hbx);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Edit");
   evas_object_smart_callback_add(bt, "clicked", _edit_bt_clicked, ck);
   elm_box_pack_end(hbx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "24h");
   evas_object_smart_callback_add(bt, "clicked", _hmode_bt_clicked, ck);
   elm_box_pack_end(hbx, bt);
   evas_object_show(bt);

   evas_object_show(win);
}
TEST_END
