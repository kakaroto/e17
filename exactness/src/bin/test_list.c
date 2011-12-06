#include <Elementary.h>
#include <Elementary_Cursor.h>
#include "tsuite.h"
static void
my_show_it(void        *data,
           Evas_Object *obj __UNUSED__,
           void        *event_info __UNUSED__)
{
   elm_list_item_show(data);
}

static void
scroll_top(void        *data __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void        *event_info __UNUSED__)
{
   printf("Top edge!\n");
}

static void
scroll_bottom(void        *data __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void        *event_info __UNUSED__)
{
   printf("Bottom edge!\n");
}

static void
scroll_left(void        *data __UNUSED__,
            Evas_Object *obj __UNUSED__,
            void        *event_info __UNUSED__)
{
   printf("Left edge!\n");
}

static void
scroll_right(void        *data __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void        *event_info __UNUSED__)
{
   printf("Right edge!\n");
}

enum _api_state
{
   ITEM_PREPEND,        /* 0 */
   ITEM_INSERT_BEFORE,  /* 1 */
   ITEM_INSERT_AFTER,   /* 2 */
   ITEM_SEPARATOR_SET,  /* 3 */
   LIST_ITEM_DEL,       /* 4 */
   SCROLLER_POLICY_SET_ON,
   SCROLLER_POLICY_SET_OFF, /* Back to AUTO next */
   TOOLTIP_TEXT_SET,    /* 7 */
   TOOLTIP_UNSET,       /* 8 */
   ITEM_CURSOR_SET,     /* 9 */
   ITEM_CURSOR_STYLE_SET,
   DISABLED_SET,        /* 11 */
   MODE_SET_COMPRESS,   /* 12 */
   MODE_SET_LIMIT,      /* 13 */
   MODE_SET_EXPAND,     /* 14 */
   HORIZONTAL_SET,      /* 15 */
   BOUNCE_SET,          /* 16 */
   LIST_CLEAR,          /* 17 */
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
/** HOW TO TEST ************************
0 ITEM PREPEND
Scroll to end
1 INSERT BEFORE
Scroll to end
2 INSERT AFTER
3 INSERT SEPERATOR
Scroll to end
4 ITEM DEL
5 POLICY ON, BOUNCE_SET(TRUE, TRUE)
6 POLICY OFF
Scroll to end
7 TOOLTIP last-item
8 Cancel tootip
9 Curosr set on last item
10 Cursor style set last item
11 DISABLE last item
12 MODE COMPRESS
13 MODE LIMIT
14 MODE EXPAND
15 HORIZ SET
16 VERT MODE, BOUNCE(TRUE, FALSE) try to bounce on Y-axis
17 List clear
*** HOW TO TEST ***********************/
   Evas_Object *li = api->data;

   switch(api->state)
     { /* Put all api-changes under switch */
      case ITEM_PREPEND: /* 0 */
           {
              const Eina_List *items = elm_list_items_get(li);
              elm_list_item_prepend(li, "PREPEND", NULL, NULL, NULL, NULL);
              elm_list_go(li);
              elm_list_item_bring_in(eina_list_nth(items, 0));
           }
         break;

      case ITEM_INSERT_BEFORE: /* 1 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_insert_before(li,
                        eina_list_nth(items, eina_list_count(items)-1),
                        "1-before-last", NULL, NULL, NULL, NULL);
                  elm_list_go(li);
                  elm_list_item_bring_in(eina_list_data_get(eina_list_last(items)));
                }
           }
         break;

      case ITEM_INSERT_AFTER: /* 2 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_insert_after(li,
                        eina_list_nth(items, eina_list_count(items)-2),
                        "insert-after", NULL, NULL, NULL, NULL);
                  elm_list_go(li);
                  elm_list_item_bring_in(eina_list_data_get(eina_list_last(items)));
                }
           }
         break;

      case ITEM_SEPARATOR_SET: /* 3 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_separator_set(eina_list_nth(items, eina_list_count(items)/2), EINA_TRUE);
                  elm_list_item_bring_in(eina_list_nth(items, eina_list_count(items)/2));
                  elm_list_go(li);
                }
           }
         break;

      case LIST_ITEM_DEL: /* 4 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_del(eina_list_data_get(eina_list_last(items)));
                }
           }
         break;

      case SCROLLER_POLICY_SET_ON: /* 5 */
         elm_list_bounce_set(li, EINA_TRUE, EINA_TRUE);
         elm_list_scroller_policy_set(li, ELM_SCROLLER_POLICY_ON, ELM_SCROLLER_POLICY_ON);
         break;

      case SCROLLER_POLICY_SET_OFF: /* Back to AUTO next (6) */
         elm_list_scroller_policy_set(li, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
         break;

      case TOOLTIP_TEXT_SET: /* 7 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_tooltip_text_set(eina_list_data_get(eina_list_last(items)), "Tooltip set from API");
                }
              elm_list_scroller_policy_set(li, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
           }
         break;

      case TOOLTIP_UNSET: /* 8 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_tooltip_unset(eina_list_data_get(eina_list_last(items)));
                }
           }
         break;

      case ITEM_CURSOR_SET: /* 9 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_cursor_set(eina_list_data_get(eina_list_last(items)), ELM_CURSOR_HAND2);
                }
           }
         break;

      case ITEM_CURSOR_STYLE_SET: /* 10 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_cursor_style_set(eina_list_data_get(eina_list_last(items)), "transparent");
                }
           }
         break;

      case DISABLED_SET: /* 11 */
           {
              const Eina_List *items = elm_list_items_get(li);
              if(eina_list_count(items))
                {
                  elm_list_item_disabled_set(eina_list_data_get(eina_list_last(items)), EINA_TRUE);
                }
           }
         break;

      case MODE_SET_COMPRESS: /* 12 */
         elm_list_mode_set(li, ELM_LIST_COMPRESS);
         break;

      case MODE_SET_LIMIT: /* 13 */
         elm_list_mode_set(li, ELM_LIST_LIMIT);
         break;

      case MODE_SET_EXPAND: /* 14 */
         elm_list_mode_set(li, ELM_LIST_EXPAND);
         break;

      case HORIZONTAL_SET: /* 15 */
         elm_list_mode_set(li, ELM_LIST_SCROLL); /* return to default mode */
         elm_list_horizontal_set(li, EINA_TRUE);
         break;

      case BOUNCE_SET: /* 16 */
         elm_list_horizontal_set(li, EINA_FALSE);
         elm_list_bounce_set(li, EINA_TRUE, EINA_FALSE);
         break;

      case LIST_CLEAR: /* 17 */
         elm_list_clear(li);
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

TEST_START(test_list)
{
   Evas_Object *bg, *li, *ic, *ic2, *bx, *bxx, *tb2, *bt;
   char buf[PATH_MAX];
   Elm_List_Item *it1, *it2, *it3, *it4, *it5;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   li = elm_list_add(win);
   elm_list_mode_set(li, ELM_LIST_LIMIT);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(li, EVAS_HINT_FILL, EVAS_HINT_FILL);
   api->data = li;

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, li);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 1, 1);
   it1 = elm_list_item_append(li, "Hello", ic, NULL, NULL, NULL);
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_file_set(ic, buf, NULL);
   elm_list_item_append(li, "world", ic, NULL, NULL, NULL);
   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "edit");
   elm_icon_scale_set(ic, 0, 0);
   elm_list_item_append(li, ".", ic, NULL, NULL, NULL);

   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "delete");
   elm_icon_scale_set(ic, 0, 0);
   ic2 = elm_icon_add(win);
   elm_icon_standard_set(ic2, "clock");
   elm_icon_scale_set(ic2, 0, 0);
   it2 = elm_list_item_append(li, "How", ic, ic2, NULL, NULL);

   bx = elm_box_add(win);
   elm_box_horizontal_set(bx, EINA_TRUE);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.0);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.0, EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);
   elm_list_item_append(li, "are", bx, NULL, NULL, NULL);

   elm_list_item_append(li, "you", NULL, NULL, NULL, NULL);
   it3 = elm_list_item_append(li, "doing", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "out", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "there", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "today", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "?", NULL, NULL, NULL, NULL);
   it4 = elm_list_item_append(li, "Here", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "are", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "some", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "more", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "items", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "Is this label long enough?", NULL, NULL, NULL, NULL);
   it5 = elm_list_item_append(li, "Maybe this one is even longer so we can test long long items.", NULL, NULL, NULL, NULL);

   elm_list_go(li);

   evas_object_show(li);

   tb2 = elm_table_add(win);
   evas_object_size_hint_weight_set(tb2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, tb2);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Hello");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it1);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.9, 0.5);
   elm_table_pack(tb2, bt, 0, 0, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "How");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it2);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.9, 0.5);
   elm_table_pack(tb2, bt, 0, 1, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "doing");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it3);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.9, 0.5);
   elm_table_pack(tb2, bt, 0, 2, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Here");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it4);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.9, 0.5);
   elm_table_pack(tb2, bt, 0, 3, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Maybe this...");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it5);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.9, 0.5);
   elm_table_pack(tb2, bt, 0, 4, 1, 1);
   evas_object_show(bt);

   evas_object_show(tb2);

   evas_object_resize(win, 320, 300);
   evas_object_show(win);

   evas_object_smart_callback_add(li, "scroll,edge,top", scroll_top, NULL);
   evas_object_smart_callback_add(li, "scroll,edge,bottom", scroll_bottom, NULL);
   evas_object_smart_callback_add(li, "scroll,edge,left", scroll_left, NULL);
   evas_object_smart_callback_add(li, "scroll,edge,right", scroll_right, NULL);
}
TEST_END

TEST_START(test_list_horizontal)
{
   Evas_Object *bg, *li, *ic, *ic2, *bx, *tb2, *bt;
   char buf[PATH_MAX];
   Elm_List_Item *it1, *it2, *it3, *it4;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   li = elm_list_add(win);
   elm_list_horizontal_set(li, EINA_TRUE);
   elm_list_mode_set(li, ELM_LIST_LIMIT);
   elm_win_resize_object_add(win, li);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 1, 1);
   it1 = elm_list_item_append(li, "Hello", ic, NULL, NULL, NULL);
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_file_set(ic, buf, NULL);
   elm_list_item_append(li, "world", ic, NULL, NULL, NULL);
   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "edit");
   elm_icon_scale_set(ic, 0, 0);
   elm_list_item_append(li, ".", ic, NULL, NULL, NULL);

   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "delete");
   elm_icon_scale_set(ic, 0, 0);
   ic2 = elm_icon_add(win);
   elm_icon_standard_set(ic2, "clock");
   elm_icon_scale_set(ic2, 0, 0);
   it2 = elm_list_item_append(li, "How", ic, ic2, NULL, NULL);

   bx = elm_box_add(win);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.0);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   elm_list_item_append(li, "are", bx, NULL, NULL, NULL);

   elm_list_item_append(li, "you", NULL, NULL, NULL, NULL);
   it3 = elm_list_item_append(li, "doing", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "out", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "there", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "today", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "?", NULL, NULL, NULL, NULL);

   it4 = elm_list_item_append(li, "And", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "here", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "we", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "are", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "done", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "with", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "items.", NULL, NULL, NULL, NULL);

   elm_list_go(li);

   evas_object_show(li);

   tb2 = elm_table_add(win);
   evas_object_size_hint_weight_set(tb2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, tb2);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Hello");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it1);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.5, 0.9);
   elm_table_pack(tb2, bt, 0, 0, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "How");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it2);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.5, 0.9);
   elm_table_pack(tb2, bt, 1, 0, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "doing");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it3);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.5, 0.9);
   elm_table_pack(tb2, bt, 2, 0, 1, 1);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "And");
   evas_object_smart_callback_add(bt, "clicked", my_show_it, it4);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bt, 0.5, 0.9);
   elm_table_pack(tb2, bt, 3, 0, 1, 1);
   evas_object_show(bt);

   evas_object_show(tb2);

   evas_object_resize(win, 320, 300);
   evas_object_show(win);
}
TEST_END

/***********/

static void
my_li2_clear(void        *data,
             Evas_Object *obj __UNUSED__,
             void        *event_info __UNUSED__)
{
   elm_list_clear(data);
}

static void
my_li2_sel(void        *data __UNUSED__,
           Evas_Object *obj,
           void        *event_info __UNUSED__)
{
   Elm_List_Item *it = elm_list_selected_item_get(obj);
   elm_list_item_selected_set(it, 0);
//   elm_list_item_selected_set(event_info, 0);
}

TEST_START(test_list2)
{
   Evas_Object *bg, *li, *ic, *ic2, *bx, *bx2, *bt;
   char buf[PATH_MAX];
   Elm_List_Item *it;

   bg = elm_bg_add(win);
   snprintf(buf, sizeof(buf), "%s/images/plant_01.jpg", elm_app_data_dir_get());
   elm_bg_file_set(bg, buf, NULL);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   li = elm_list_add(win);
   evas_object_size_hint_align_set(li, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_list_mode_set(li, ELM_LIST_LIMIT);
//   elm_list_multi_select_set(li, 1);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   it = elm_list_item_append(li, "Hello", ic, NULL, my_li2_sel, NULL);
   elm_list_item_selected_set(it, 1);
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_file_set(ic, buf, NULL);
   elm_list_item_append(li, "world", ic, NULL, NULL, NULL);
   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "edit");
   elm_icon_scale_set(ic, 0, 0);
   elm_list_item_append(li, ".", ic, NULL, NULL, NULL);

   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "delete");
   elm_icon_scale_set(ic, 0, 0);
   ic2 = elm_icon_add(win);
   elm_icon_standard_set(ic2, "clock");
   elm_icon_scale_set(ic2, 0, 0);
   elm_list_item_append(li, "How", ic, ic2, NULL, NULL);

   bx2 = elm_box_add(win);
   elm_box_horizontal_set(bx2, EINA_TRUE);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   elm_box_pack_end(bx2, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.0);
   elm_box_pack_end(bx2, ic);
   evas_object_show(ic);
   elm_list_item_append(li, "are", bx2, NULL, NULL, NULL);

   elm_list_item_append(li, "you", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "doing", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "out", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "there", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "today", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "?", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "Here", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "are", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "some", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "more", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "items", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "Longer label.", NULL, NULL, NULL, NULL);

   elm_list_go(li);

   elm_box_pack_end(bx, li);
   evas_object_show(li);

   bx2 = elm_box_add(win);
   elm_box_horizontal_set(bx2, EINA_TRUE);
   elm_box_homogeneous_set(bx2, EINA_TRUE);
   evas_object_size_hint_weight_set(bx2, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bx2, EVAS_HINT_FILL, EVAS_HINT_FILL);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Clear");
   evas_object_smart_callback_add(bt, "clicked", my_li2_clear, li);
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);

   evas_object_resize(win, 320, 300);
   evas_object_show(win);
}
TEST_END

/***********/

static void
_bt_clicked(void        *data __UNUSED__,
            Evas_Object *obj __UNUSED__,
            void        *event_info __UNUSED__)
{
   printf("button was clicked\n");
}

static void
_it_clicked(void        *data __UNUSED__,
            Evas_Object *obj __UNUSED__,
            void        *event_info __UNUSED__)
{
   printf("item was clicked\n");
}

TEST_START(test_list3)
{
   Evas_Object *bg, *li, *ic, *ic2, *bx;
   char buf[PATH_MAX];

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   li = elm_list_add(win);
   elm_win_resize_object_add(win, li);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_list_mode_set(li, ELM_LIST_COMPRESS);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_file_set(ic, buf, NULL);
   ic2 = elm_button_add(win);
   elm_object_text_set(ic2, "Click me");
   evas_object_smart_callback_add(ic2, "clicked", _bt_clicked, NULL);
   evas_object_propagate_events_set(ic2, 0);
   elm_list_item_append(li, "Hello", ic, ic2, _it_clicked, NULL);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_file_set(ic, buf, NULL);
   ic2 = elm_button_add(win);
   elm_object_text_set(ic2, "Click me");
   evas_object_smart_callback_add(ic2, "clicked", _bt_clicked, NULL);
   elm_list_item_append(li, "world", ic, ic2, _it_clicked, NULL);

   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "edit");
   elm_icon_scale_set(ic, 0, 0);
   elm_list_item_append(li, ".", ic, NULL, NULL, NULL);

   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "delete");
   elm_icon_scale_set(ic, 0, 0);
   ic2 = elm_icon_add(win);
   elm_icon_standard_set(ic2, "clock");
   elm_icon_scale_set(ic2, 0, 0);
   elm_list_item_append(li, "How", ic, ic2, NULL, NULL);

   bx = elm_box_add(win);
   elm_box_horizontal_set(bx, EINA_TRUE);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.0);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/logo_small.png", elm_app_data_dir_get());
   elm_icon_file_set(ic, buf, NULL);
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.0, EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, ic);
   evas_object_show(ic);

   elm_list_item_append(li, "are", bx, NULL, NULL, NULL);
   elm_list_item_append(li, "you", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "doing", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "out", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "there", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "today", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "?", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "Here", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "are", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "some", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "more", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "items", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "Is this label long enough?", NULL, NULL, NULL, NULL);
   elm_list_item_append(li, "Maybe this one is even longer so we can test long long items.", NULL, NULL, NULL, NULL);

   elm_list_go(li);

   evas_object_show(li);

   evas_object_resize(win, 320, 300);
   evas_object_show(win);
}
TEST_END

///////////////////////////////////////////////////////////////////////////////////////

struct Pginfo
{
   Evas_Object *pager, *win;
};

static void
test_list4_back_cb(void        *data,
                   Evas_Object *obj __UNUSED__,
                   void        *event_info __UNUSED__)
{
   struct Pginfo *info = data;
   if (!info) return;

   elm_pager_content_pop(info->pager);
}

static void
test_list4_swipe(void        *data,
                 Evas_Object *obj __UNUSED__,
                 void        *event_info)
{
   Evas_Object *box, *entry, *button;
   struct Pginfo *info = data;
   char *item_data;
   if ((!event_info) || (!data)) return;

   item_data = elm_list_item_data_get(event_info);

   box = elm_box_add(info->win);
   elm_box_horizontal_set(box, EINA_FALSE);
   elm_box_homogeneous_set(box, EINA_FALSE);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(box);

   entry = elm_entry_add(info->win);
   elm_entry_scrollable_set(entry, EINA_TRUE);
   elm_entry_editable_set(entry, EINA_FALSE);
   elm_entry_entry_set(entry, item_data);
   evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(entry);

   button = elm_button_add(info->win);
   elm_object_text_set(button, "back");
   evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(button, EVAS_HINT_FILL, 0);
   evas_object_smart_callback_add(button, "clicked", test_list4_back_cb, info);
   evas_object_show(button);

   elm_box_pack_start(box, entry);
   elm_box_pack_end(box, button);

   elm_pager_content_push(info->pager, box);
}

TEST_START(test_list4)
{
   Evas_Object *bg, *li, *ic, *ic2, *pager;
   static struct Pginfo info = {NULL, NULL};
   char buf[PATH_MAX];

   info.win = win;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   pager = elm_pager_add(win);
   elm_win_resize_object_add(win, pager);
   evas_object_size_hint_weight_set(pager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pager, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(pager);
   info.pager = pager;

   li = elm_list_add(win);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_list_mode_set(li, ELM_LIST_COMPRESS);
   evas_object_smart_callback_add(li, "swipe", test_list4_swipe, &info);
   elm_pager_content_push(pager, li);

   static char pf_data[] = "Pink Floyd were formed in 1965, and originally consisted of university"                       \
                           "students Roger Waters, Nick Mason, Richard Wright, and Syd Barrett. The group were a popular" \
                           "fixture on London's underground music scene, and under Barrett's leadership released two "    \
                           "charting singles, \"Arnold Layne\" and \"See Emily Play\", and a successful debut album, "    \
                           "ThePiper at the Gates of Dawn. In 1968, guitarist and singer David Gilmour joined the "       \
                           "line-up. Barrett was soon removed, due to his increasingly erratic behaviour. Following "     \
                           "Barrett's departure, bass player and singer Roger Waters became the band's lyricist and "     \
                           "conceptual leader, with Gilmour assuming lead guitar and much of the vocals. With this "      \
                           "line-up, Floyd went on to achieve worldwide critical and commercial success with the "        \
                           "conceptalbums The Dark Side of the Moon, Wish You Were Here, Animals, and The Wall.";
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/mystrale.jpg", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_file_set(ic, buf, NULL);
   elm_list_item_append(li, "Pink Floyd", ic, NULL, NULL, &pf_data);

   static char ds_data[] = "Dire Straits were a British rock band, formed by Mark Knopfler "                          \
                           "(lead vocals and lead guitar), his younger brother David Knopfler (rhythm guitar and "    \
                           "backing vocals), John Illsley (bass guitar and backing vocals), and Pick Withers (drums " \
                           "and percussion), and managed by Ed Bicknell, active between 1977 and 1995. Although the " \
                           "band was formed in an era when punk rock was at the forefront, Dire Straits played a more "
                           "bluesy style, albeit with a stripped-down sound that appealed to audiences weary of the "   \
                           "overproduced stadium rock of the 1970s.[citation needed] In their early days, Mark and "    \
                           "David requested that pub owners turn down their sound so that patrons could converse "      \
                           "while the band played, an indication of their unassuming demeanor. Despite this oddly "     \
                           "self-effacing approach to rock and roll, Dire Straits soon became hugely successful, with " \
                           "their first album going multi-platinum globally.";
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/mystrale_2.jpg", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_file_set(ic, buf, NULL);
   elm_list_item_append(li, "Dire Straits", ic, NULL, NULL, &ds_data);

   static char uh_data[] = "Uriah Heep are an English hard rock band. The band released several "                       \
                           "commercially successful albums in the 1970s such as Uriah Heep Live (1973), but their "     \
                           "audience declined by the 1980s, to the point where they became essentially a cult band in " \
                           "the United States and United Kingdom. Uriah Heep maintain a significant following in "      \
                           "Germany, the Netherlands, Scandinavia, the Balkans, Japan and Russia, where they still "    \
                           "perform at stadium-sized venues.";
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/icon_17.png", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 1, 1);
   elm_icon_file_set(ic, buf, NULL);
   elm_list_item_append(li, "Uriah Heep", ic, NULL, NULL, &uh_data);

   static char r_data[] = "Rush is a Canadian rock band formed in August 1968, in the Willowdale "                       \
                          "neighbourhood of Toronto, Ontario. The band is composed of bassist, keyboardist, and lead "   \
                          "vocalist Geddy Lee, guitarist Alex Lifeson, and drummer and lyricist Neil Peart. The band "   \
                          "and its membership went through a number of re-configurations between 1968 and 1974, "        \
                          "achieving their current form when Peart replaced original drummer John Rutsey in July 1974, " \
                          "two weeks before the group's first United States tour.";
   ic = elm_icon_add(win);
   snprintf(buf, sizeof(buf), "%s/images/icon_21.png", elm_app_data_dir_get());
   elm_icon_scale_set(ic, 0, 0);
   elm_icon_file_set(ic, buf, NULL);
   ic2 = elm_icon_add(win);
   elm_icon_standard_set(ic2, "clock");
   elm_icon_scale_set(ic2, 0, 0);
   elm_list_item_append(li, "Rush", ic, ic2, NULL, &r_data);

   elm_list_go(li);

   evas_object_show(li);
   evas_object_resize(win, 320, 300);
   evas_object_show(win);
}
TEST_END

/////////////////////////////////////////////////////////////////////////////////////////
struct list5_data_cb
{
   Evas_Object *win, *list;
};

static void
test_list5_item_del(void        *data,
                    Evas_Object *obj __UNUSED__,
                    void        *event_info __UNUSED__)
{
   elm_list_item_del(data);
}

static void
test_list5_swipe(void        *data __UNUSED__,
                 Evas_Object *obj __UNUSED__,
                 void        *event_info)
{
   Evas_Object *button;
   struct list5_data_cb *info = elm_list_item_data_get(event_info);

   if (elm_list_item_end_get(event_info)) return;

   button = elm_button_add(info->win);
   elm_object_text_set(button, "delete");
   evas_object_propagate_events_set(button, 0);
   evas_object_smart_callback_add(button, "clicked", test_list5_item_del,
                                  event_info);
   elm_list_item_end_set(event_info, button);
}

TEST_START(test_list5)
{
   Evas_Object *bg, *li;
   static struct list5_data_cb info;

   info.win = win;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   li = elm_list_add(win);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_list_mode_set(li, ELM_LIST_COMPRESS);
   evas_object_smart_callback_add(li, "swipe", test_list5_swipe, NULL);
   elm_win_resize_object_add(win, li);
   evas_object_show(li);
   info.list = li;

   elm_list_item_append(li, "Network", NULL, NULL, NULL, &info);
   elm_list_item_append(li, "Audio", NULL, NULL, NULL, &info);

   elm_list_go(li);
   evas_object_resize(win, 320, 300);
   evas_object_show(win);
}
TEST_END

