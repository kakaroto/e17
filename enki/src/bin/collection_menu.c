#include "main.h"
static void
_menu_dismiss_cb(void *data, Evas_Object *obj, void *event_info);
static void
_collection_del_cb(void *data, Evas_Object *obj, void *event_info);

Collection_Menu *
collection_menu_new(Evas_Object *parent, Enlil_Collection *col)
{
   Evas_Object *menu;
   Collection_Menu *cm = calloc(1, sizeof(Collection_Menu));
   cm->col = col;

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", _menu_dismiss_cb, cm);
   cm->menu = menu;

   elm_menu_item_add(menu, NULL, NULL, D_("Delete the collection"),
                     _collection_del_cb, cm);

   evas_object_show(menu);
   return cm;
}

static void
_menu_dismiss_cb(void *data, Evas_Object *obj, void *event_info)
{
   Collection_Menu *col_menu = data;
   collection_menu_free(col_menu);
}

static void
_collection_del_cb(void *data, Evas_Object *obj, void *event_info)
{
   Collection_Menu *col_menu = data;
   Enlil_Collection_Data *col_data =
            enlil_collection_user_data_get(col_menu->col);

   enlil_library_collection_del(col_data->enlil_data->library, col_menu->col);

   collection_menu_free(col_menu);
}

void
collection_menu_free(Collection_Menu *col_menu)
{
   ASSERT_RETURN_VOID(col_menu != NULL);

   evas_object_del(col_menu->menu);
}

