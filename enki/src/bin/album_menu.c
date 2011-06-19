#include "main.h"
static void
_menu_dismiss_cb(void *data, Evas_Object *obj, void *event_info);
static void
_collection_album_add_cb(void *data, Evas_Object *obj, void *event_info);
static void
_collection_album_remove_cb(void *data, Evas_Object *obj, void *event_info);
static void
_collection_new_cb(void *data, Evas_Object *obj, void *event_info);

static void
_tag_album_add_cb(void *data, Evas_Object *obj, void *event_info);
static void
_tag_album_remove_cb(void *data, Evas_Object *obj, void *event_info);
static void
_tag_new_cb(void *data, Evas_Object *obj, void *event_info);

static void
_album_rename_cb(void *data, Evas_Object *obj, void *event_info);
static void
_album_exifs_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void
_album_iptcs_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void
_album_delete_cb(void *data, Evas_Object *obj, void *event_info);

static void
_sort_by_name_cb(void *data, Evas_Object *obj, void *event_info);
static void
_sort_by_date_cb(void *data, Evas_Object *obj, void *event_info);

static void
_inwin_del_cb(void *data);

Album_Menu *
album_menu_new(Evas_Object *parent, Enlil_Album *album)
{
   Evas_Object *menu;
   Elm_Menu_Item *mi = NULL;
   const Eina_List *l;
   const Enlil_Album_Collection *album_col;
   const Enlil_Collection *col;
   Album_Menu *am = calloc(1, sizeof(Album_Menu));
   am->album = album;

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", _menu_dismiss_cb, am);
   am->menu = menu;

   if (enlil_album_collections_get(album))
   {
      elm_menu_item_separator_add(menu, NULL);
      mi
               = elm_menu_item_add(menu, NULL, NULL,
                                   D_("Remove from the collection"),
                                   NULL, NULL);
   }

   EINA_LIST_FOREACH(enlil_album_collections_get(album), l, album_col)
   {
      Elm_Menu_Item *mi2;
      mi2 = elm_menu_item_add(menu, mi, NULL, album_col->name, _collection_album_remove_cb, am);
   }

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Add to the collection"), NULL,
                          NULL);
   elm_menu_item_add(menu, mi, NULL, D_("New collection"), _collection_new_cb,
                     am);
   elm_menu_item_separator_add(menu, mi);

   EINA_LIST_FOREACH(enlil_library_collections_get(enlil_album_library_get(album)), l, col)
   {
      Elm_Menu_Item *mi2;
      if(!eina_list_data_find(enlil_collection_albums_get(col), album))
      mi2 = elm_menu_item_add(menu, mi, NULL, enlil_collection_name_get(col), _collection_album_add_cb, am);
   }

   elm_menu_item_separator_add(menu, NULL);
   mi = elm_menu_item_add(menu, NULL, NULL, D_("Rename the album"),
                          _album_rename_cb, am);
   mi = elm_menu_item_add(menu, NULL, NULL, D_("Reload all exifs data"),
                          _album_exifs_reload_cb, am);
   mi = elm_menu_item_add(menu, NULL, NULL, D_("Reload all iptcs data"),
                          _album_iptcs_reload_cb, am);
   elm_menu_item_separator_add(menu, NULL);
   mi = elm_menu_item_add(menu, NULL, NULL, D_("Delete the album"),
                          _album_delete_cb, am);

   evas_object_show(menu);
   return am;
}

Album_Menu *
album_collection_menu_new(Evas_Object *parent, Enlil_Album *album)
{
   Evas_Object *menu;
   Elm_Menu_Item *mi = NULL;
   const Eina_List *l;
   const Enlil_Album_Collection *album_col;
   const Enlil_Collection *col;
   Album_Menu *am = calloc(1, sizeof(Album_Menu));
   am->album = album;

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", _menu_dismiss_cb, am);
   am->menu = menu;

   EINA_LIST_FOREACH(enlil_album_collections_get(album), l, album_col)
   {
      Elm_Menu_Item *mi2;
      mi2 = elm_menu_item_add(menu, NULL, NULL, album_col->name, NULL, am);
   }

   if (enlil_album_collections_get(album))
   {
      elm_menu_item_separator_add(menu, NULL);
      mi
               = elm_menu_item_add(menu, NULL, NULL,
                                   D_("Remove from the collection"),
                                   NULL, NULL);
   }

   EINA_LIST_FOREACH(enlil_album_collections_get(album), l, album_col)
   {
      Elm_Menu_Item *mi2;
      mi2 = elm_menu_item_add(menu, mi, NULL, album_col->name, _collection_album_remove_cb, am);
   }

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Add to the collection"), NULL,
                          NULL);
   elm_menu_item_add(menu, mi, NULL, D_("New collection"), _collection_new_cb,
                     am);
   elm_menu_item_separator_add(menu, mi);

   EINA_LIST_FOREACH(enlil_library_collections_get(enlil_album_library_get(album)), l, col)
   {
      Elm_Menu_Item *mi2;
      if(!eina_list_data_find(enlil_collection_albums_get(col), album))
      mi2 = elm_menu_item_add(menu, mi, NULL, enlil_collection_name_get(col), _collection_album_add_cb, am);
   }

   evas_object_show(menu);
   return am;
}

Album_Menu *
album_tag_menu_new(Evas_Object *parent, Enlil_Album *album)
{
   Evas_Object *menu;
   Elm_Menu_Item *mi = NULL;
   const Eina_List *l, *_l;
   Enlil_Photo *photo;
   const Enlil_Tag *tag;
   Eina_Bool empty = EINA_TRUE;
   Album_Menu *am = calloc(1, sizeof(Album_Menu));
   am->album = album;

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", _menu_dismiss_cb, am);
   am->menu = menu;

   mi = elm_menu_item_add(menu, NULL, NULL,
                          D_("Remove the tag from all photos"), NULL, am);
   EINA_LIST_FOREACH(enlil_library_tags_get(enlil_album_library_get(album)), l, tag)
   {
      Eina_Bool find = EINA_FALSE;
      EINA_LIST_FOREACH(enlil_album_photos_get(album), _l, photo)
      {
         if(eina_list_data_find(enlil_tag_photos_get(tag), photo))
         {
            find = EINA_TRUE;
            break;
         }
      }
      if(find)
      {
         elm_menu_item_add(menu, mi, NULL, enlil_tag_name_get(tag), _tag_album_remove_cb, am);
         empty = EINA_FALSE;
      }
   }

   if (empty) elm_menu_item_add(menu, mi, NULL, D_("Empty"), NULL, NULL);

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Add the tag to all photos"),
                          NULL, am);
   elm_menu_item_add(menu, mi, NULL, D_("New tag"), _tag_new_cb, am);
   elm_menu_item_separator_add(menu, mi);

   EINA_LIST_FOREACH(enlil_library_tags_get(enlil_album_library_get(album)), l, tag)
   {
      Eina_Bool find = EINA_TRUE;
      EINA_LIST_FOREACH(enlil_album_photos_get(album), _l, photo)
      {
         if(!eina_list_data_find(enlil_tag_photos_get(tag), photo))
         {
            find = EINA_FALSE;
            break;
         }
      }
      if(!find)
      elm_menu_item_add(menu, mi, NULL, enlil_tag_name_get(tag), _tag_album_add_cb, am);
   }

   evas_object_show(menu);
   return am;
}

Album_Menu *
album_sorts_menu_new(Evas_Object *parent, Enlil_Album *album)
{
   Evas_Object *menu;
   Elm_Menu_Item *mi = NULL;
   char *icon = NULL;
   Album_Menu *am = calloc(1, sizeof(Album_Menu));
   am->album = album;

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", _menu_dismiss_cb, am);
   am->menu = menu;

   if (enlil_album_photos_sort_get(album) == ENLIL_PHOTO_SORT_NAME)
   {
      icon = "arrow_right";
   }
   mi = elm_menu_item_add(menu, NULL, icon, D_("Sort by name"),
                          _sort_by_name_cb, am);
   icon = NULL;

   if (enlil_album_photos_sort_get(album) == ENLIL_PHOTO_SORT_DATE)
   {
      icon = "arrow_right";
   }
   mi = elm_menu_item_add(menu, NULL, icon, D_("Sort by date"),
                          _sort_by_date_cb, am);
   icon = NULL;

   evas_object_show(menu);
   return am;
}

static void
_menu_dismiss_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *menu = data;
   album_menu_free(menu);
}

static void
_collection_album_add_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *am = data;
   Elm_Menu_Item *mi = event_info;

   enlil_album_collection_add(am->album, elm_menu_item_label_get(mi));

   album_menu_free(am);
}

static void
_collection_album_remove_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *am = data;
   Elm_Menu_Item *mi = event_info;
   const Eina_List *l;
   Enlil_Album_Collection *album_col;

   EINA_LIST_FOREACH(enlil_album_collections_get(am->album), l, album_col)
   {
      if(album_col->name == elm_menu_item_label_get(mi))
      break;
   }

   enlil_album_collection_remove(am->album, album_col);

   album_menu_free(am);
}

static void
_collection_new_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *album_menu = data;
   inwin_collection_new_new(_inwin_del_cb, album_menu, album_menu->album);
}

static void
_tag_album_add_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *am = data;
   Elm_Menu_Item *mi = event_info;
   Eina_List *l;
   const Eina_List *_l;
   Enlil_Photo_Tag *photo_tag;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(enlil_album_photos_get(am->album), l, photo)
   {
      EINA_LIST_FOREACH(enlil_photo_tags_get(photo), _l, photo_tag)
      {
         if(photo_tag->name == elm_menu_item_label_get(mi))
         break;
      }

      if(!photo_tag)
      enlil_photo_tag_add(photo, elm_menu_item_label_get(mi));
   }

   album_menu_free(am);
}

static void
_tag_album_remove_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *am = data;
   Elm_Menu_Item *mi = event_info;
   Eina_List *l;
   const Eina_List *_l;
   Enlil_Photo_Tag *photo_tag;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(enlil_album_photos_get(am->album), l, photo)
   {
      EINA_LIST_FOREACH(enlil_photo_tags_get(photo), _l, photo_tag)
      {
         if(photo_tag->name == elm_menu_item_label_get(mi))
         break;
      }

      if(photo_tag)
      enlil_photo_tag_remove(photo, photo_tag);
   }

   album_menu_free(am);
}

static void
_tag_new_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *album_menu = data;
   inwin_album_tag_new_new(_inwin_del_cb, album_menu, album_menu->album);
}

static void
_album_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *album_menu = data;
   inwin_album_delete_new(_inwin_del_cb, album_menu, album_menu->album);
}

static void
_album_rename_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *album_menu = data;
   inwin_album_rename_new(_inwin_del_cb, album_menu, album_menu->album);
}

static void
_album_exifs_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *album_menu = data;
   Eina_List *l;
   Enlil_Photo *photo;

EINA_LIST_FOREACH(enlil_album_photos_get(album_menu->album), l, photo)
enlil_exif_job_append(photo, exif_load_done, photo);
}

static void
_album_iptcs_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Album_Menu *album_menu = data;
   Eina_List *l;
   Enlil_Photo *photo;

EINA_LIST_FOREACH(enlil_album_photos_get(album_menu->album), l, photo)
enlil_iptc_job_append(photo, iptc_load_done, photo);
}

static void
_sort_by_name_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eina_List *l;
   Enlil_Photo *photo;
   Album_Menu *album_menu = data;
   PL_Child_Item *prev_item = NULL;
   enlil_album_photos_sort_set(album_menu->album, ENLIL_PHOTO_SORT_NAME);

   photos_list_object_freeze(enlil_data->list_photo->o_list, 1);
   EINA_LIST_FOREACH(enlil_album_photos_get(album_menu->album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

      photos_list_object_child_move_after(photo_data->list_photo_item, prev_item);
      prev_item = photo_data->list_photo_item;
   }
   photos_list_object_freeze(enlil_data->list_photo->o_list, 0);
   enlil_album_eet_photos_list_save(album_menu->album);
   enlil_album_eet_header_save(album_menu->album);
}

static void
_sort_by_date_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eina_List *l;
   Enlil_Photo *photo;
   Album_Menu *album_menu = data;
   PL_Child_Item *prev_item = NULL;
   enlil_album_photos_sort_set(album_menu->album, ENLIL_PHOTO_SORT_DATE);

   photos_list_object_freeze(enlil_data->list_photo->o_list, 1);
   EINA_LIST_FOREACH(enlil_album_photos_get(album_menu->album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

      photos_list_object_child_move_after(photo_data->list_photo_item, prev_item);
      prev_item = photo_data->list_photo_item;
   }
   photos_list_object_freeze(enlil_data->list_photo->o_list, 0);
   enlil_album_eet_photos_list_save(album_menu->album);
   enlil_album_eet_header_save(album_menu->album);
}

void
album_menu_free(Album_Menu *album_menu)
{
   ASSERT_RETURN_VOID(album_menu != NULL);

   evas_object_del(album_menu->menu);

   if (album_menu->inwin) evas_object_del(album_menu->inwin);
}

static void
_inwin_del_cb(void *data)
{
   album_menu_free(data);
}

