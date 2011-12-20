#include <Elementary.h>
#include "tsuite.h"

enum _api_state
{
   STATE_MARK_MONTHLY,
   STATE_MARK_WEEKLY,
   STATE_SUNDAY_HIGHLIGHT,
   STATE_SELECT_DATE_DISABLED_WITH_MARKS,
   STATE_SELECT_DATE_DISABLED_NO_MARKS,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   const Eina_List *items = elm_box_children_get(api->data);
   static Elm_Calendar_Mark *m = NULL;
   if(!eina_list_count(items))
     return;

   switch(api->state)
     { /* Put all api-changes under switch */
      case STATE_MARK_MONTHLY:
           {
              Evas_Object *cal = eina_list_nth(items, 0);
              time_t sec_per_day = (60*60*24);
              time_t sec_per_year = sec_per_day * 365;
              time_t the_time = (sec_per_year * 41) + (sec_per_day * 9); /* Set date to DEC 31, 2010 */
              elm_calendar_min_max_year_set(cal, 2010, 2011);
              m = elm_calendar_mark_add(cal, "checked", gmtime(&the_time), ELM_CALENDAR_MONTHLY);
              elm_calendar_selected_time_set(cal, gmtime(&the_time));
           }
         break;
      case STATE_MARK_WEEKLY:
           {
              Evas_Object *cal = eina_list_nth(items, 0);
              time_t sec_per_day = (60*60*24);
              time_t sec_per_year = sec_per_day * 365;
              time_t the_time = (sec_per_year * 41) + (sec_per_day * 4); /* Set date to DEC 26, 2010 */
              elm_calendar_mark_del(m);
              m = elm_calendar_mark_add(cal, "checked", gmtime(&the_time), ELM_CALENDAR_WEEKLY);
              elm_calendar_selected_time_set(cal, gmtime(&the_time));
           }
         break;
      case STATE_SUNDAY_HIGHLIGHT:
           {
              Evas_Object *cal = eina_list_nth(items, 0);
              time_t sec_per_day = (60*60*24);
              time_t sec_per_year = sec_per_day * 365;
              time_t the_time = (sec_per_year * 41) + (sec_per_day * 3); /* Set date to DEC 25, 2010 */
              /* elm_calendar_mark_del(m); */
              m = elm_calendar_mark_add(cal, "holiday", gmtime(&the_time), ELM_CALENDAR_WEEKLY);
              elm_calendar_selected_time_set(cal, gmtime(&the_time));
           }
         break;
      case STATE_SELECT_DATE_DISABLED_WITH_MARKS:
           {
              Evas_Object *cal = eina_list_nth(items, 0);
              time_t sec_per_day = (60*60*24);
              time_t sec_per_year = sec_per_day * 365;
              time_t the_time = (sec_per_year * 41) + (sec_per_day * 10); /* Set date to JAN 01, 2011 */
              elm_calendar_day_selection_enabled_set(cal, EINA_FALSE);
              elm_calendar_selected_time_set(cal, gmtime(&the_time));
           }
         break;
      case STATE_SELECT_DATE_DISABLED_NO_MARKS:
           {
              Evas_Object *cal = eina_list_nth(items, 0);
              time_t sec_per_day = (60*60*24);
              time_t sec_per_year = sec_per_day * 365;
              time_t the_time = (sec_per_year * 41) + (sec_per_day * 40); /* Set date to FEB 01, 2011 */
              elm_calendar_marks_clear(cal);
              elm_calendar_day_selection_enabled_set(cal, EINA_FALSE);
              elm_calendar_selected_time_set(cal, gmtime(&the_time));
           }
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

/* A simple test, just displaying calendar in it's default state */
TEST_START(test_calendar)
{
   Evas_Object *bg, *bx, *bxx, *cal;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   api->data = bx;
   evas_object_show(bx);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, bx);

   cal = elm_calendar_add(win);
   evas_object_size_hint_weight_set(cal, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, cal);

   time_t sec_per_day = (60*60*24);
   time_t sec_per_year = sec_per_day * 365;
   time_t the_time = (sec_per_year * 41) + (sec_per_day * 9); /* Set date to DEC 31, 2010 */
   elm_calendar_selected_time_set(cal, gmtime(&the_time));
   elm_calendar_min_max_year_set(cal, 2010, 2010);

   evas_object_show(cal);

   evas_object_show(win);
}
TEST_END
