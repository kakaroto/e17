#include <Elementary.h>
#include <Elementary_Cursor.h>
#include "tsuite.h"

static void
_disk_sel(void *data __UNUSED__, Evas_Object * obj __UNUSED__, void *event_info)
{
   Elm_Diskselector_Item *it = event_info;
   printf("Equinox: %s\n", elm_diskselector_item_label_get(it));
}

static void
_disk_next(void *data __UNUSED__, Evas_Object * obj __UNUSED__, void *event_info)
{
   Elm_Diskselector_Item *next, *prev, *it = event_info;
   prev = elm_diskselector_item_prev_get(it);
   next = elm_diskselector_item_next_get(it);
   printf("Prev: %s, Next: %s\n", elm_diskselector_item_label_get(prev),
          elm_diskselector_item_label_get(next));
}

static void
_print_disk_info_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Diskselector_Item *it = event_info;
   printf("Selected label: %s\n", elm_diskselector_item_label_get(it));
}

static Evas_Object *
_disk_create(Evas_Object *win, Eina_Bool round)
{
   Elm_Diskselector_Item *it;
   Evas_Object *di;

   di = elm_diskselector_add(win);

   elm_diskselector_item_append(di, "January", NULL, NULL, NULL);
   elm_diskselector_item_append(di, "February", NULL, _disk_next, NULL);
   elm_diskselector_item_append(di, "March", NULL, _disk_sel, NULL);
   elm_diskselector_item_append(di, "April", NULL, NULL, NULL);
   elm_diskselector_item_append(di, "May", NULL, NULL, NULL);
   elm_diskselector_item_append(di, "June", NULL, NULL, NULL);
   elm_diskselector_item_append(di, "July", NULL, NULL, NULL);
   it = elm_diskselector_item_append(di, "August", NULL, NULL, NULL);
   elm_diskselector_item_append(di, "September", NULL, _disk_sel, NULL);
   elm_diskselector_item_append(di, "October", NULL, NULL, NULL);
   elm_diskselector_item_append(di, "November", NULL, NULL, NULL);
   elm_diskselector_item_append(di, "December", NULL, NULL, NULL);

   elm_diskselector_item_selected_set(it, EINA_TRUE);
   elm_diskselector_round_set(di, round);

   return di;
}

enum _api_state
{
   SCROLLER_POLICY_SET_ON,
   SCROLLER_POLICY_SET_OFF,
   BOUNCE_SET,
   ITEM_LABEL_SET,
   TOOLTIP_TEXT_SET,
   ITEM_TOOLTIP_UNSET,
   ITEM_CURSOR_SET,
   ITEM_CURSOR_UNSET,
   ITEM_ICON_SET,
   SELECTED_SET,
   ITEM_PREV_GET,
   ITEM_DEL_NOT_SELECTED,
   ITEM_DEL, /* delete when selected */
   CLEAR,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   const Eina_List *disks = elm_box_children_get(api->data);
   if(!eina_list_count(disks))
     return;

   switch(api->state)
     { /* Put all api-changes under switch */
      case SCROLLER_POLICY_SET_ON: /* 0 */
           {  /* Get first disk */
              Evas_Object *disk = eina_list_nth(disks, 0);
              elm_diskselector_scroller_policy_set(disk, ELM_SCROLLER_POLICY_ON, ELM_SCROLLER_POLICY_ON);
           }
         break;

      case SCROLLER_POLICY_SET_OFF: /* 1 */
           {  /* Get first disk */
              Evas_Object *disk = eina_list_nth(disks, 0);
              elm_diskselector_scroller_policy_set(disk, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
           }
         break;

      case BOUNCE_SET: /* 2 */
           {  /* Get second disk, cancel bounce */
              Evas_Object *disk = eina_list_nth(disks, 1);
              elm_diskselector_bounce_set(disk, EINA_FALSE, EINA_FALSE);
           }
         break;

      case ITEM_LABEL_SET: /* 3 */
         elm_diskselector_item_label_set(elm_diskselector_selected_item_get(eina_list_nth(disks, 0)), "Label from API");
         break;

      case TOOLTIP_TEXT_SET: /* 4 */
         elm_diskselector_item_tooltip_text_set(elm_diskselector_selected_item_get(eina_list_nth(disks, 0)), "Tooltip from API");
         break;

      case ITEM_TOOLTIP_UNSET: /* 5 */
         elm_diskselector_item_tooltip_unset(elm_diskselector_selected_item_get(eina_list_nth(disks, 0)));
         break;

      case ITEM_CURSOR_SET: /* 6 */
         elm_diskselector_item_cursor_set(elm_diskselector_selected_item_get(eina_list_nth(disks, 0)), ELM_CURSOR_HAND2);
         break;

      case ITEM_CURSOR_UNSET: /* 7 */
         elm_diskselector_item_cursor_unset(elm_diskselector_selected_item_get(eina_list_nth(disks, 0)));
         break;

      case ITEM_ICON_SET: /* 8 */
           {  /* Set icon of selected item in first disk */
              char buf[PATH_MAX];
              Evas_Object *ic = elm_icon_add(elm_object_parent_widget_get(eina_list_nth(disks, 0)));
              snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
              elm_icon_file_set(ic, buf, NULL);
              elm_diskselector_item_icon_set(elm_diskselector_selected_item_get(eina_list_nth(disks, 0)), ic);
              evas_object_show(ic);
           }
         break;

      case SELECTED_SET: /* 9 */
           {  /* Select NEXT item of third disk */
              elm_diskselector_item_selected_set(elm_diskselector_item_next_get(elm_diskselector_selected_item_get(eina_list_nth(disks, 2))), EINA_TRUE);
           }
         break;

      case ITEM_PREV_GET: /* 10 */
           {  /* Select PREV item of third disk */
              elm_diskselector_item_selected_set(elm_diskselector_item_prev_get(elm_diskselector_selected_item_get(eina_list_nth(disks, 2))), EINA_TRUE);
           }
         break;

      case ITEM_DEL_NOT_SELECTED: /* 11 */
           {  /* Remove selected item of third disk */
              elm_diskselector_item_del(elm_diskselector_item_prev_get(elm_diskselector_selected_item_get(eina_list_nth(disks, 2))));
           }
         break;

      case ITEM_DEL: /* 12 */
           {  /* Remove selected item of first disk */
              elm_diskselector_item_del(elm_diskselector_selected_item_get(eina_list_nth(disks, 1)));
           }
         break;

      case CLEAR: /* 13 */
         elm_diskselector_clear(eina_list_nth(disks, 0));
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

TEST_START(test_diskselector)
{
   Evas_Object *bg, *bx, *bxx, *disk, *ic;
   Elm_Diskselector_Item *it;
   char buf[PATH_MAX];
   int idx = 0;

   char *month_list[] = {
      "Jan", "Feb", "Mar",
      "Apr", "May", "Jun",
      "Jul", "Aug", "Sep",
      "Oct", "Nov", "Dec"
   };
   char date[3];

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   api->data = bx;
   evas_object_show(bx);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, bx);

   disk = _disk_create(win, EINA_TRUE);
   evas_object_size_hint_weight_set(disk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(disk, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, disk);
   evas_object_show(disk);
   evas_object_smart_callback_add(disk, "selected", _print_disk_info_cb, NULL);
   it = elm_diskselector_selected_item_get(disk);
   elm_diskselector_item_selected_set(it, EINA_FALSE);

   disk = _disk_create(win, EINA_FALSE);
   evas_object_size_hint_weight_set(disk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(disk, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, disk);
   evas_object_show(disk);
   evas_object_smart_callback_add(disk, "selected", _print_disk_info_cb, NULL);
   it = elm_diskselector_first_item_get(disk);
   it = elm_diskselector_item_next_get(it);
   elm_diskselector_item_selected_set(it, EINA_TRUE);

   disk = _disk_create(win, EINA_FALSE);
   evas_object_size_hint_weight_set(disk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(disk, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, disk);
   evas_object_show(disk);
   evas_object_smart_callback_add(disk, "selected", _print_disk_info_cb, NULL);
   elm_diskselector_side_label_length_set(disk, 4);

   disk = elm_diskselector_add(win);
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 1, 1);
   elm_diskselector_item_append(disk, "Sunday", ic, NULL, NULL);
   elm_diskselector_item_append(disk, "Monday", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "Tuesday", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "Wednesday", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "Thursday", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "Friday", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "Saturday", NULL, NULL, NULL);
   elm_diskselector_round_set(disk, EINA_TRUE);
   evas_object_size_hint_weight_set(disk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(disk, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, disk);
   evas_object_show(disk);
   evas_object_smart_callback_add(disk, "selected", _print_disk_info_cb, NULL);

   disk = elm_diskselector_add(win);
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 1, 1);
   elm_diskselector_item_append(disk, "머리스타일", ic, NULL, NULL);
   elm_diskselector_item_append(disk, "プロが伝授する", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "生上访要求政府", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "English", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "والشريعة", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "עִבְרִית", NULL, NULL, NULL);
   elm_diskselector_item_append(disk, "Grüßen", NULL, NULL, NULL);
   elm_diskselector_round_set(disk, EINA_TRUE);
   evas_object_size_hint_weight_set(disk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(disk, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, disk);
   evas_object_show(disk);
   evas_object_smart_callback_add(disk, "selected", _print_disk_info_cb, NULL);

   // displayed item number setting example
   disk = elm_diskselector_add(win);
   elm_diskselector_display_item_num_set(disk, 5);
   printf("Number of Items in DiskSelector : %d\n", elm_diskselector_display_item_num_get(disk));

   for (idx = 0; idx < (int)(sizeof(month_list) / sizeof(month_list[0])); idx++)
     {
        it = elm_diskselector_item_append(disk, month_list[idx], NULL, NULL, NULL);
     }

   elm_diskselector_item_selected_set(it, EINA_TRUE);
   elm_diskselector_round_set(disk, EINA_TRUE);
   evas_object_size_hint_weight_set(disk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(disk, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, disk);
   evas_object_show(disk);
   evas_object_smart_callback_add(disk, "selected", _print_disk_info_cb, NULL);

   // displayed item number setting example
   disk = elm_diskselector_add(win);
   elm_diskselector_display_item_num_set(disk, 7);
   printf("Number of Items in DiskSelector : %d\n", elm_diskselector_display_item_num_get(disk));

   for (idx = 1; idx < 31; idx++)
     {
        snprintf(date, sizeof(date), "%d", idx);
        it = elm_diskselector_item_append(disk, date, NULL, NULL, NULL);
     }

   elm_diskselector_item_selected_set(it, EINA_TRUE);
   elm_diskselector_round_set(disk, EINA_TRUE);
   evas_object_size_hint_weight_set(disk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(disk, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, disk);
   evas_object_show(disk);
   evas_object_smart_callback_add(disk, "selected", _print_disk_info_cb, NULL);


   evas_object_resize(win, 320, 480);
   evas_object_show(win);
}
TEST_END
