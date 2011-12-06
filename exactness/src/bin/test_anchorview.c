#include <Elementary.h>
#include "tsuite.h"

static void
my_anchorview_bt(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *av = data;
   elm_anchorview_hover_end(av);
}

static void
my_anchorview_anchor(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *av = data;
   Elm_Entry_Anchorview_Info *ei = event_info;
   Evas_Object *bt, *bx;

   bt = elm_button_add(obj);
   elm_object_text_set(bt, ei->name);
   elm_hover_content_set(ei->hover, "middle", bt);
   evas_object_show(bt);

   // hints as to where we probably should put hover contents (buttons etc.).
   if (ei->hover_top)
     {
	bx = elm_box_add(obj);
	bt = elm_button_add(obj);
	elm_object_text_set(bt, "Top 1");
	elm_box_pack_end(bx, bt);
	evas_object_smart_callback_add(bt, "clicked", my_anchorview_bt, av);
	evas_object_show(bt);
	bt = elm_button_add(obj);
	elm_object_text_set(bt, "Top 2");
	elm_box_pack_end(bx, bt);
	evas_object_smart_callback_add(bt, "clicked", my_anchorview_bt, av);
	evas_object_show(bt);
	bt = elm_button_add(obj);
	elm_object_text_set(bt, "Top 3");
	elm_box_pack_end(bx, bt);
	evas_object_smart_callback_add(bt, "clicked", my_anchorview_bt, av);
	evas_object_show(bt);
	elm_hover_content_set(ei->hover, "top", bx);
	evas_object_show(bx);
     }
   if (ei->hover_bottom)
     {
	bt = elm_button_add(obj);
	elm_object_text_set(bt, "Bot");
	elm_hover_content_set(ei->hover, "bottom", bt);
	evas_object_smart_callback_add(bt, "clicked", my_anchorview_bt, av);
	evas_object_show(bt);
     }
   if (ei->hover_left)
     {
	bt = elm_button_add(obj);
	elm_object_text_set(bt, "Left");
	elm_hover_content_set(ei->hover, "left", bt);
	evas_object_smart_callback_add(bt, "clicked", my_anchorview_bt, av);
	evas_object_show(bt);
     }
   if (ei->hover_right)
     {
	bt = elm_button_add(obj);
	elm_object_text_set(bt, "Right");
	elm_hover_content_set(ei->hover, "right", bt);
	evas_object_smart_callback_add(bt, "clicked", my_anchorview_bt, av);
	evas_object_show(bt);
     }
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

TEST_START(test_anchorview)
{
   Evas_Object *bg, *bx, *av;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);  /* Resize BOX to size of window */
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   av = elm_anchorview_add(win);
   elm_anchorview_hover_style_set(av, "popout");
   elm_anchorview_hover_parent_set(av, win);
   elm_object_text_set(av,
		       "This is an entry widget in this window that<br>"
		       "uses markup <b>like this</> for styling and<br>"
		       "formatting <em>like this</>, as well as<br>"
		       "<a href=X><link>links in the text</></a>, so enter text<br>"
		       "in here to edit it. By the way, links are<br>"
		       "called <a href=anc-02>Anchors</a> so you will need<br>"
		       "to refer to them this way. <item relsize=16x16 vsize=full href=emoticon/guilty-smile></item>");
   evas_object_size_hint_weight_set(av, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(av, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(av, "anchor,clicked", my_anchorview_anchor, av);
   elm_box_pack_end(bx, av);
   evas_object_show(av);
   evas_object_show(bx);

   evas_object_resize(win, 320, 300);

   elm_object_focus_set(win, EINA_TRUE);
   evas_object_show(win);
}
TEST_END
