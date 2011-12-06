#include <Elementary.h>
#include "tsuite.h"

enum _api_state
{
   BOX_PACK_START,
   BOX_PACK_BEFORE,
   BOX_PACK_AFTER,
   BOX_PADDING_SET,
   BOX_ALIGN_SET,
   BOX_HOMOGENEOUS_SET,
   BOX_UNPACK_ALL,
   BOX_CLEAR,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   const Eina_List *items = elm_box_children_get(api->data);
   if(!eina_list_count(items))
     return;

   /* use elm_box_children_get() to get list of children */
   switch(api->state)
     { /* Put all api-changes under switch */
      case BOX_PACK_START:  /* Move last item to begining */
         elm_box_unpack(api->data, eina_list_data_get(eina_list_last(items)));
         elm_box_pack_start(api->data, eina_list_data_get(eina_list_last(items)));
         break;

      case BOX_PACK_BEFORE:
         if(eina_list_count(items) > 1)
               {  /* Put last item before the one preceeding it */
                  elm_box_unpack(api->data, eina_list_data_get(eina_list_last(items)));
                  elm_box_pack_before(api->data,
                        eina_list_data_get(eina_list_last(items)),
                        eina_list_nth(items, eina_list_count(items)-2));
               }
         break;

      case BOX_PACK_AFTER:
         if(eina_list_count(items) > 1)
               {  /* Put item before last to last */
                  elm_box_unpack(api->data, eina_list_nth(items,
                           eina_list_count(items)-2));
                  elm_box_pack_after(api->data,
                        eina_list_nth(items, eina_list_count(items)-2),
                        eina_list_data_get(eina_list_last(items)));
               }
         break;

      case BOX_PADDING_SET:
         elm_box_padding_set(api->data, 30, 15);
         break;

      case BOX_ALIGN_SET:
         elm_box_align_set(api->data, 0.25, 0.75);
         break;

      case BOX_HOMOGENEOUS_SET:
         elm_box_homogeneous_set(api->data, EINA_TRUE);
         break;

      case BOX_UNPACK_ALL:
           {
              Eina_List *l;
              Evas_Object *data;
              elm_box_unpack_all(api->data);
              EINA_LIST_REVERSE_FOREACH(items, l, data)
                 elm_box_pack_end(api->data, data);
           }
         break;

      case BOX_CLEAR:
         elm_box_clear(api->data);
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

TEST_START(test_box_vert)
{
   Evas_Object *bg, *bx, *bxx, *ic;
   char buf[PATH_MAX];

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   bx = elm_box_add(win);
   api->data = bx;
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bx);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, bx);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/icon_01.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/icon_02.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.0, 0.5);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/icon_03.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, EVAS_HINT_EXPAND, 0.5);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   evas_object_show(win);
}
TEST_END

static void
_del_cb(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   elm_box_unpack(data, obj);
   evas_object_move(obj, 0, 0);
   evas_object_color_set(obj, 128, 64, 0, 128);
//   evas_object_del(obj);
}

TEST_START(test_box_vert2)
{
   Evas_Object *bg, *bx, *bt;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);
   elm_box_padding_set(bx, 10, 10);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bx);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button 1");
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_smart_callback_add(bt, "clicked", _del_cb, bx);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button 2");
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_smart_callback_add(bt, "clicked", _del_cb, bx);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button 3");
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_smart_callback_add(bt, "clicked", _del_cb, bx);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button 4");
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_smart_callback_add(bt, "clicked", _del_cb, bx);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button 5");
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_smart_callback_add(bt, "clicked", _del_cb, bx);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   evas_object_show(win);
}
TEST_END

TEST_START(test_box_horiz)
{
   Evas_Object *bg, *bx, *bxx, *ic;
   char buf[PATH_MAX];

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   bx = elm_box_add(win);
   elm_box_horizontal_set(bx, EINA_TRUE);
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

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/icon_01.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/icon_02.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.0);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/icon_03.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.0, EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   evas_object_show(win);
}
TEST_END

typedef struct
{
   Eina_List *transitions;
   Evas_Object *box;
   Evas_Object_Box_Layout last_layout;
} Transitions_Data;

static void
_test_box_transition_change(void *data)
{
   Transitions_Data *tdata = data;
   Elm_Box_Transition *layout_data;
   Evas_Object_Box_Layout next_layout;

   if (!data) return;
   next_layout = eina_list_data_get(tdata->transitions);
   layout_data = elm_box_transition_new(2.0, tdata->last_layout,
                                        NULL, NULL, next_layout, NULL, NULL,
                                        _test_box_transition_change, tdata);
   elm_box_layout_set(tdata->box, elm_box_layout_transition, layout_data, elm_box_transition_free);
   tdata->last_layout = next_layout;

   tdata->transitions = eina_list_demote_list(tdata->transitions, tdata->transitions);
}

static void
_win_del(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Transitions_Data *tdata = data;
   if (!data) return;
   free(tdata);
}

void
test_box_transition(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *win, *bg, *bx, *bt;
   Transitions_Data *tdata;

   win = elm_win_add(NULL, "box-transition", ELM_WIN_BASIC);
   elm_win_title_set(win, "Box Transition");
   elm_win_autodel_set(win, EINA_TRUE);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   evas_object_show(bg);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);

   evas_object_show(bx);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button 1");
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   evas_object_smart_callback_add(bt, "clicked", _del_cb, bx);
   elm_box_pack_end(bx, bt);
   evas_object_resize(bt, 100, 100);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button 2");
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   evas_object_smart_callback_add(bt, "clicked", _del_cb, bx);
   elm_box_pack_end(bx, bt);
   evas_object_resize(bt, 100, 100);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button 3");
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   evas_object_smart_callback_add(bt, "clicked", _del_cb, bx);
   elm_box_pack_end(bx, bt);
   evas_object_resize(bt, 100, 100);
   evas_object_show(bt);

   tdata = calloc(1, sizeof(Transitions_Data));
   tdata->box = bx;
   tdata->last_layout = evas_object_box_layout_horizontal;
   tdata->transitions = eina_list_append(tdata->transitions,
         evas_object_box_layout_vertical);
   tdata->transitions = eina_list_append(tdata->transitions,
         evas_object_box_layout_horizontal);
   tdata->transitions = eina_list_append(tdata->transitions,
         evas_object_box_layout_stack);
   tdata->transitions = eina_list_append(tdata->transitions,
         evas_object_box_layout_homogeneous_vertical);
   tdata->transitions = eina_list_append(tdata->transitions,
         evas_object_box_layout_homogeneous_horizontal);
   tdata->transitions = eina_list_append(tdata->transitions,
         evas_object_box_layout_flow_vertical);
   tdata->transitions = eina_list_append(tdata->transitions,
         evas_object_box_layout_flow_horizontal);
   tdata->transitions = eina_list_append(tdata->transitions,
         evas_object_box_layout_stack);

   evas_object_resize(win, 300, 300);
   evas_object_resize(bx, 300, 300);
   evas_object_smart_callback_add(win, "delete,request", _win_del, tdata);
   evas_object_show(win);
   elm_box_layout_set(bx, evas_object_box_layout_horizontal, NULL, NULL);
   _test_box_transition_change(tdata);
}
