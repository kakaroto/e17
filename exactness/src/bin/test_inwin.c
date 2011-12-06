#include <Elementary.h>
#include "tsuite.h"

enum _api_state
{
   CONTENT_UNSET,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   Evas_Object *t;
   switch(api->state)
     {
      case CONTENT_UNSET:
         t = elm_win_inwin_content_unset(api->data);
         evas_object_del(t);
         t = elm_label_add(elm_object_parent_widget_get(api->data));
         elm_object_text_set(t, "Content was unset.<br>DONE!");
         elm_win_inwin_content_set(api->data, t);
         evas_object_show(t);

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

TEST_START(test_inwin)
{
   Evas_Object *bg, *bxx, *inwin, *lb;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   inwin = elm_win_inwin_add(win);
   api->data = inwin;
   evas_object_show(inwin);

   bxx = elm_box_add(inwin);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   api->bt = elm_button_add(inwin);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   lb = elm_label_add(win);
   elm_object_text_set(lb,
		       "This is an \"inwin\" - a window in a<br>"
		       "window. This is handy for quick popups<br>"
		       "you want centered, taking over the window<br>"
		       "until dismissed somehow. Unlike hovers they<br>"
		       "don't hover over their target.");
   elm_box_pack_end(bxx, lb);
   elm_win_inwin_content_set(inwin, bxx);
   evas_object_show(lb);

   evas_object_resize(win, 320, 240);
   evas_object_show(win);
}
TEST_END

TEST_START(test_inwin2)
{
   Evas_Object *bg, *inwin, *lb;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   inwin = elm_win_inwin_add(win);
   elm_object_style_set(inwin, "minimal_vertical");
   evas_object_show(inwin);

   lb = elm_label_add(win);
   elm_object_text_set(lb,
		       "This is an \"inwin\" - a window in a<br>"
		       "window. This is handy for quick popups<br>"
		       "you want centered, taking over the window<br>"
		       "until dismissed somehow. Unlike hovers they<br>"
		       "don't hover over their target.<br>"
		       "<br>"
		       "This inwin style compacts itself vertically<br>"
		       "to the size of its contents minimum size.");
   elm_win_inwin_content_set(inwin, lb);
   evas_object_show(lb);

   evas_object_resize(win, 320, 240);
   evas_object_show(win);
}
TEST_END
