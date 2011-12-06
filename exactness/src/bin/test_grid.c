#include <Elementary.h>
#include "tsuite.h"

static void
_ch_grid(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   int x, y, w, h;

   elm_grid_pack_get(obj, &x, &y, &w, &h);
   elm_grid_pack_set(obj, x - 1, y - 1, w + 2, h + 2);
}

struct _Api_Data
{
   Evas_Object *grid;
   Evas_Object *child;
};
typedef struct _Api_Data Api_Data;

enum _api_state
{
   GRID_PACK_SET,
   GRID_UNPACK,
   GRID_SIZE,
   GRID_CLEAR,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   Api_Data *dt = api->data;
   int w, h;

   switch(api->state)
     { /* Put all api-changes under switch */
      case GRID_PACK_SET: /* 0 */
         elm_grid_pack_set(dt->child, 5, 15, 60, 40);
         break;

      case GRID_UNPACK: /* 1 */
         elm_grid_unpack(dt->grid, dt->child);
         evas_object_del(dt->child);
         break;

      case GRID_SIZE: /* 2 */
         elm_grid_size_get(dt->grid, &w, &h);
         printf("size w=<%d> h=<%d>\n", w, h);
         w = h = 100; /* grid size returns wrong values */
         w += 30;
         h += 10;

         elm_grid_size_set(dt->grid, w, h);
         break;

      case GRID_CLEAR: /* 3 */
         elm_grid_clear(dt->grid, EINA_TRUE);
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

TEST_START(test_grid)
{
   Evas_Object *bg, *gd, *bt, *rc, *en;
   Api_Data *dt = malloc(sizeof(Api_Data));
   api->data = dt;
   api->free_data = EINA_TRUE;

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   gd = elm_grid_add(win);
   elm_grid_size_set(gd, 100, 100);
   elm_win_resize_object_add(win, gd);
   evas_object_size_hint_weight_set(gd, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   dt->grid = gd;
   evas_object_show(gd);

   en = elm_entry_add(win);
   elm_entry_scrollable_set(en, EINA_TRUE);
   elm_entry_entry_set(en, "Entry text");
   elm_entry_single_line_set(en, 1);
   elm_grid_pack(gd, en, 50, 10, 40, 10);
   evas_object_show(en);

   en = elm_entry_add(win);
   elm_entry_scrollable_set(en, EINA_TRUE);
   elm_entry_entry_set(en, "Entry text 2");
   elm_entry_single_line_set(en, 1);
   elm_grid_pack(gd, en, 60, 20, 30, 10);
   evas_object_show(en);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_grid_pack(gd, api->bt, 30, 0, 40, 10);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button");
   elm_grid_pack(gd, bt,  0,  0, 20, 20);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button");
   elm_grid_pack(gd, bt, 10, 10, 40, 20);
   dt->child = bt;
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button");
   elm_grid_pack(gd, bt, 10, 30, 20, 50);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Button");
   elm_grid_pack(gd, bt, 80, 80, 20, 20);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Change");
   elm_grid_pack(gd, bt, 40, 40, 20, 20);
   evas_object_smart_callback_add(bt, "clicked", _ch_grid, gd);
   evas_object_show(bt);

   rc = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(rc, 128, 0, 0, 128);
   elm_grid_pack(gd, rc, 40, 70, 20, 10);
   evas_object_show(rc);

   rc = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(rc, 0, 128, 0, 128);
   elm_grid_pack(gd, rc, 60, 70, 10, 10);
   evas_object_show(rc);

   rc = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(rc, 0, 0, 128, 128);
   elm_grid_pack(gd, rc, 40, 80, 10, 10);
   evas_object_show(rc);

   rc = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(rc, 128, 0, 128, 128);
   elm_grid_pack(gd, rc, 50, 80, 10, 10);
   evas_object_show(rc);

   rc = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(rc, 128, 64, 0, 128);
   elm_grid_pack(gd, rc, 60, 80, 10, 10);
   evas_object_show(rc);

   evas_object_resize(win, 480, 480);
   evas_object_show(win);
}
TEST_END
