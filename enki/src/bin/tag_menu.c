#include "main.h"

static void
_menu_dismiss_cb(void *data, Evas_Object *obj, void *event_info);
static void
_tag_del_cb(void *data, Evas_Object *obj, void *event_info);

Tag_Menu *
tag_menu_new(Evas_Object *parent, Enlil_Tag *tag)
{
   Evas_Object *menu;
   Tag_Menu *cm = calloc(1, sizeof(Tag_Menu));
   cm->tag = tag;

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", _menu_dismiss_cb, cm);
   cm->menu = menu;

   elm_menu_item_add(menu, NULL, NULL, D_("Delete the tag"), _tag_del_cb, cm);

   evas_object_show(menu);
   return cm;
}

static void
_menu_dismiss_cb(void *data, Evas_Object *obj, void *event_info)
{
   Tag_Menu *tag_menu = data;
   tag_menu_free(tag_menu);
}

static void
_tag_del_cb(void *data, Evas_Object *obj, void *event_info)
{
   Tag_Menu *tag_menu = data;
   Enlil_Tag_Data *tag_data = enlil_tag_user_data_get(tag_menu->tag);

   enlil_library_tag_del(tag_data->enlil_data->library, tag_menu->tag);

   tag_menu_free(tag_menu);
}

void
tag_menu_free(Tag_Menu *tag_menu)
{
   ASSERT_RETURN_VOID(tag_menu != NULL);

   evas_object_del(tag_menu->menu);
}

