#include <Elementary.h>
#include "tsuite.h"

static Elm_Genlist_Item_Class itci;
char *gli_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   char buf[256];
   int j = (long)data;
   snprintf(buf, sizeof(buf), "%c%c",
            'A' + ((j >> 4) & 0xf),
            'a' + ((j     ) & 0xf)
            );
   return strdup(buf);
}

void
index_changed2(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   // called on a change but delayed in case multiple changes happen in a
   // short timespan
   elm_genlist_item_top_bring_in(elm_index_item_data_get(event_info));
}

void
index_changed(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   // this is calld on every change, no matter how often
   // elm_genlist_item_bring_in(event_info);
}

void
index_selected(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   // called on final select
   elm_genlist_item_top_bring_in(elm_index_item_data_get(event_info));
}

struct _Idx_Data_Type
{
   Evas_Object *id;  /* Pointer to Index */
   Elm_Object_Item *item; /* Item we use for search */
};
typedef struct _Idx_Data_Type Idx_Data_Type;

enum _api_state
{
   INDEX_LEVEL_SET,
   INDEX_ACTIVE_SET,
   INDEX_APPEND_RELATIVE,
   INDEX_PREPEND,
   INDEX_ITEM_DEL,
   INDEX_ITEM_FIND,
   INDEX_CLEAR,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   Idx_Data_Type *d = api->data;
   switch(api->state)
     { /* Put all api-changes under switch */
      case INDEX_LEVEL_SET: /* 0 */
         elm_index_active_set(d->id, EINA_TRUE);
         elm_index_item_level_set(d->id, (elm_index_item_level_get(d->id) ? 0 : 1));
         break;

      case INDEX_ACTIVE_SET: /* 1 */
         elm_index_active_set(d->id, EINA_FALSE);
         break;

      case INDEX_APPEND_RELATIVE: /* 2 */
             elm_index_item_append_relative(d->id, "W", d->item, elm_index_item_find(d->id, d->item));
         break;

      case INDEX_PREPEND: /* 3 */
             elm_index_item_prepend(d->id, "D", d->item);
         break;

      case INDEX_ITEM_DEL: /* 4 */
         elm_index_item_del(d->id, elm_index_item_find(d->id, d->item));
         break;

      case INDEX_ITEM_FIND: /* 5 */
           {
              Elm_Object_Item *i = elm_index_item_find(d->id, d->item);
              if(i)
                {
                   printf("Item Find - Found Item.\n");
                   elm_index_item_del(d->id, i);
                }
           }
         break;

      case INDEX_CLEAR: /* 6 */
         elm_index_item_clear(d->id);
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

TEST_START(test_index)
{
   Evas_Object *bg, *bxx, *gl, *id;
   Elm_Object_Item *glit;
   int i, j;
   Idx_Data_Type *dt = malloc(sizeof(Idx_Data_Type));
   api->data = dt;
   api->free_data = EINA_TRUE;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   gl = elm_genlist_add(win);
   evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(gl);

   dt->id = id = elm_index_add(win);
   evas_object_size_hint_weight_set(id, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(id, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, id);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, gl);

   evas_object_show(id);

   itci.item_style     = "default";
   itci.func.text_get = gli_text_get;
   itci.func.content_get  = NULL;
   itci.func.state_get = NULL;
   itci.func.del       = NULL;

   j = 0;
   for (i = 0; i < 100; i++)
     {
        glit = elm_genlist_item_append(gl, &itci,
                                       (void *)(long)j/* item data */,
                                       NULL/* parent */,
                                       ELM_GENLIST_ITEM_NONE,
                                       NULL/* func */, NULL/* func data */);
        if (!(j & 0xf))
          {
             char buf[32];

             snprintf(buf, sizeof(buf), "%c", 'A' + ((j >> 4) & 0xf));
             elm_index_item_append(id, buf, glit);
          }
        j += 2;
     }
   evas_object_smart_callback_add(id, "delay,changed", index_changed2, NULL);
   evas_object_smart_callback_add(id, "changed", index_changed, NULL);
   evas_object_smart_callback_add(id, "selected", index_selected, NULL);
   elm_index_item_go(id, 0);

   evas_object_resize(win, 320, 480);
   evas_object_show(win);
}
TEST_END

/***********/

typedef struct _Test_Index2_Elements
{
   Evas_Object *entry, *lst, *id;
} Test_Index2_Elements;

void
test_index2_del(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   /* FIXME it won't be called if elm_test main window is closed */
   free(data);
}

int
test_index2_cmp(const void *data1, const void *data2)
{
   const char *label1, *label2;
   const Elm_Object_Item *list_it1 = data1;
   const Elm_Object_Item *list_it2 = data2;

   label1 = elm_list_item_label_get(list_it1);
   label2 = elm_list_item_label_get(list_it2);

   return strcasecmp(label1, label2);
}

int
test_index2_icmp(const void *data1, const void *data2)
{
   const char *label1, *label2;
   const Elm_Object_Item *index_it1 = data1;
   const Elm_Object_Item *index_it2 = data2;

   label1 = elm_index_item_letter_get(index_it1);
   label2 = elm_index_item_letter_get(index_it2);

   return strcasecmp(label1, label2);
}

void
test_index2_it_add(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Test_Index2_Elements *gui = data;
   Elm_Object_Item *list_it;
   const char *label;
   char letter[2];

   label = elm_entry_entry_get(gui->entry);
   snprintf(letter, sizeof(letter), "%c", label[0]);
   list_it = elm_list_item_sorted_insert(gui->lst, label, NULL, NULL, NULL,
                                         NULL, test_index2_cmp);
   elm_index_item_sorted_insert(gui->id, letter, list_it, test_index2_icmp,
                                test_index2_cmp);
   elm_list_go(gui->lst);
   /* FIXME it's not showing the recently added item */
   elm_list_item_show(list_it);
}

void
test_index2_it_del(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Test_Index2_Elements *gui = data;
   const char *label, *label_next;
   Elm_Object_Item *lit, *lit_next;

   lit = elm_list_selected_item_get(obj);
   lit_next = elm_list_item_next(lit);

   if (!lit_next)
     {
        elm_index_item_del(gui->id, elm_index_item_find(gui->id, lit));
        elm_list_item_del(lit);
        return;
     }

   label = elm_list_item_label_get(lit);
   label_next = elm_list_item_label_get(lit_next);

   if (label[0] == label_next[0])
     {
        Elm_Object_Item *iit;
        iit = elm_index_item_find(gui->id, lit);
        elm_index_item_data_set(iit, lit_next);
     }
   else
     elm_index_item_del(gui->id, elm_index_item_find(gui->id, lit));

   elm_list_item_del(lit);
}

void
test_index2_id_changed(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   elm_list_item_show(event_info);
}

TEST_START(test_index2)
{
   Evas_Object *bg, *box, *bt;
   Test_Index2_Elements *gui;

   gui = malloc(sizeof(*gui));

   evas_object_smart_callback_add(win, "delete,request", test_index2_del, gui);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   gui->id = elm_index_add(win);
   evas_object_size_hint_weight_set(gui->id, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, gui->id);
   evas_object_smart_callback_add(gui->id, "delay,changed",
                                  test_index2_id_changed, NULL);
   evas_object_show(gui->id);

   gui->entry = elm_entry_add(win);
   elm_entry_scrollable_set(gui->entry, EINA_TRUE);
   elm_entry_entry_set(gui->entry, "Label");
   elm_entry_single_line_set(gui->entry, EINA_TRUE);
   evas_object_size_hint_weight_set(gui->entry, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_fill_set(gui->entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, gui->entry);
   evas_object_show(gui->entry);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Add");
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_fill_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, bt);
   evas_object_smart_callback_add(bt, "clicked", test_index2_it_add, gui);
   evas_object_show(bt);

   gui->lst = elm_list_add(win);
   elm_box_pack_end(box, gui->lst);
   evas_object_size_hint_weight_set(gui->lst, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(gui->lst, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(gui->lst, "selected", test_index2_it_del,
                                  gui);
   elm_list_go(gui->lst);
   evas_object_show(gui->lst);

   evas_object_resize(win, 320, 480);
   evas_object_show(win);
}
TEST_END
