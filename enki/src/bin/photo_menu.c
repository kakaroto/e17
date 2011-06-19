#include "main.h"
#include "objects.h"

static void
_tag_photo_add_cb(void *data, Evas_Object *obj, void *event_info);
static void
_tag_photo_remove_cb(void *data, Evas_Object *obj, void *event_info);
static void
_tag_new_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_thumb_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_iptc_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_exif_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_move_album_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_move_album_list_cb(void *data, Evas_Object *obj, void *event_info);
static void
_wall_app_set_cb(void *data, Evas_Object *obj, void *event_info);
static void
_library_photo1_set_cb(void *data, Evas_Object *obj, void *event_info);
static void
_library_photo2_set_cb(void *data, Evas_Object *obj, void *event_info);
static void
_inwin_del_cb(void *data);

Evas_Object *_edje;

static Eina_Bool _init = EINA_FALSE;

void
photo_menu_init(List_Photo *enlil_photo, Evas_Object *edje)
{
   Evas_Object *bt;
   Eina_List *photos = NULL, *_l;
   const Eina_List *l;
   PL_Child_Item *child;
   Enlil_Photo *photo, *_photo;
   Enlil_Tag *tag;
   Elm_List_Item *item;
   Eina_Bool empty;

   _edje = edje;

   //build the list of selected photos
   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
   {
      photo = photos_list_object_item_data_get(child);
      photos = eina_list_append(photos, photo);
   }
   //

   if (!_init)
   {
      _init = EINA_TRUE;
      bt
               = edje_object_part_external_object_get(edje,
                                                      "object.photo_menu.tools.reload_thumb");
      evas_object_smart_callback_add(bt, "clicked", _photo_thumb_reload_cb,
                                     enlil_photo);

      bt
               = edje_object_part_external_object_get(edje,
                                                      "object.photo_menu.tools.reload_exifs");
      evas_object_smart_callback_add(bt, "clicked", _photo_exif_reload_cb,
                                     enlil_photo);

      bt
               = edje_object_part_external_object_get(edje,
                                                      "object.photo_menu.tools.reload_iptcs");
      evas_object_smart_callback_add(bt, "clicked", _photo_iptc_reload_cb,
                                     enlil_photo);

      bt
               = edje_object_part_external_object_get(edje,
                                                      "object.photo_menu.wallpaper.application");
      evas_object_smart_callback_add(bt, "clicked", _wall_app_set_cb,
                                     enlil_photo);

      bt = edje_object_part_external_object_get(edje,
                                                "object.photo_menu.delete");
      evas_object_smart_callback_add(bt, "clicked", _photo_delete_cb,
                                     enlil_photo);

      bt = edje_object_part_external_object_get(edje,
                                                "object.photo_menu.album.move");
      evas_object_smart_callback_add(bt, "clicked", _photo_move_album_list_cb,
                                     enlil_photo);

      bt
               = edje_object_part_external_object_get(edje,
                                                      "object.photo_menu.library.photo.front");
      evas_object_smart_callback_add(bt, "clicked", _library_photo1_set_cb,
                                     enlil_photo);

      bt
               = edje_object_part_external_object_get(edje,
                                                      "object.photo_menu.library.photo.back");
      evas_object_smart_callback_add(bt, "clicked", _library_photo2_set_cb,
                                     enlil_photo);
   }

   //tags to remove
   bt
            = edje_object_part_external_object_get(edje,
                                                   "object.photo_menu.tags_remove.list");
   elm_list_clear(bt);
   Enlil_Album *album = enlil_photo_album_get(eina_list_data_get(photos));
   empty = EINA_TRUE;
   EINA_LIST_FOREACH(enlil_library_tags_get(enlil_album_library_get(album)), l, tag)
   {
      Eina_Bool find = EINA_FALSE;
      EINA_LIST_FOREACH(photos, _l, _photo)
      {
         if(eina_list_data_find(enlil_tag_photos_get(tag), _photo))
         {
            find = EINA_TRUE;
            break;
         }
      }
      if(find)
      {
         elm_list_item_append(bt, enlil_tag_name_get(tag), NULL, NULL, _tag_photo_remove_cb, enlil_photo);
         empty = EINA_FALSE;
      }
   }
   if (empty) elm_list_item_append(bt, D_("No tags"), NULL, NULL, NULL, NULL);
   elm_list_go(bt);
   //

   //add tags
   bt = edje_object_part_external_object_get(edje,
                                             "object.photo_menu.tags_add.list");
   elm_list_clear(bt);
   elm_list_item_append(bt, D_("New tag"), NULL, NULL, _tag_new_cb, enlil_photo);

   EINA_LIST_FOREACH(enlil_library_tags_get(enlil_album_library_get(album)), l, tag)
   {
      Eina_Bool find = EINA_TRUE;
      EINA_LIST_FOREACH(photos, _l, _photo)
      {
         if(!eina_list_data_find(enlil_tag_photos_get(tag), _photo))
         {
            find = EINA_FALSE;
            break;
         }
      }
      if(!find)
      elm_list_item_append(bt, enlil_tag_name_get(tag), NULL, NULL, _tag_photo_add_cb, enlil_photo);
   }
   elm_list_go(bt);
   //

   eina_list_free(photos);
}

static void
_tag_photo_add_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   Eina_List *l;
   PL_Child_Item *child;
   Enlil_Photo *photo;

   //build the list of selected photos
   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
   {
      photo = photos_list_object_item_data_get(child);
      enlil_photo_tag_add(photo, elm_list_item_label_get(elm_list_selected_item_get(obj)));
   }

   photo_menu_init(enlil_photo, _edje);
}

static void
_tag_photo_remove_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   const Eina_List *l, *l2;
   Enlil_Photo_Tag *tag_col;
   PL_Child_Item *child;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
   {
      photo = photos_list_object_item_data_get(child);
      EINA_LIST_FOREACH(enlil_photo_tags_get(photo), l2, tag_col)
      {
         if(tag_col->name == elm_list_item_label_get(elm_list_selected_item_get(obj)))
         break;
      }
      if(tag_col)
      enlil_photo_tag_remove(photo, tag_col);
   }

   photo_menu_init(enlil_photo, _edje);
}

static void
_tag_new_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   Eina_List *photos = NULL, *l;
   PL_Child_Item *child;
   Enlil_Photo *photo;

   //build the list of selected photos
   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
   {
      photo = photos_list_object_item_data_get(child);
      photos = eina_list_append(photos, photo);
   }
   //
   inwin_tag_new_new(_inwin_del_cb, photos, photos);
}

static void
_photo_move_album_list_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   Eina_List *photos = NULL, *l;
   PL_Child_Item *child;
   Enlil_Photo *photo;

   //build the list of selected photos
   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
   {
      photo = photos_list_object_item_data_get(child);
      photos = eina_list_append(photos, photo);
   }
   //
   inwin_photo_move_album_new(_inwin_del_cb, photos, photos);
}

static void
_photo_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   Eina_List *photos = NULL, *l;
   PL_Child_Item *child;
   Enlil_Photo *photo;

   //build the list of selected photos
   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
   {
      photo = photos_list_object_item_data_get(child);
      photos = eina_list_append(photos, photo);
   }
   //
   inwin_photo_delete_new(enlil_data->win->win, _inwin_del_cb, photos, photos);
}

static void
_photo_thumb_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   PL_Child_Item *child;
   Eina_List *l;

   Enlil_Photo_Data *photo_data;
   Enlil_Photo *photo;

EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
{
   photo = photos_list_object_item_data_get(child);
   photo_data = enlil_photo_user_data_get(photo);
   enlil_photo_thumb_fdo_normal_set(photo, NULL);
   enlil_photo_thumb_fdo_large_set(photo, NULL);
   photos_list_object_item_update(photo_data->list_photo_item);
   map_photo_update(enlil_data->map, photo);
}
}

static void
_photo_exif_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   Eina_List *l;
   Enlil_Photo *photo;
   PL_Child_Item *child;

EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
{
   photo = photos_list_object_item_data_get(child);
   enlil_exif_job_prepend(photo, exif_load_done, photo);
}
}

static void
_photo_iptc_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   Eina_List *l;
   Enlil_Photo *photo;
   PL_Child_Item *child;

EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
{
   photo = photos_list_object_item_data_get(child);
   enlil_iptc_job_prepend(photo, iptc_load_done, photo);
}
}

static void
_wall_app_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Enlil_String s;
   Eet_Data_Descriptor *edd;
   List_Photo *enlil_photo = data;
   Eina_List *l;
   Enlil_Photo *photo;
   PL_Child_Item *child;

EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
{
   photo = photos_list_object_item_data_get(child);
   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
            enlil_photo_file_name_get(photo));
   enlil_win_bg_set(enlil_data->win, buf);

   s.string = eina_stringshare_add(buf);
   edd = enlil_string_edd_new();

   snprintf(buf, PATH_MAX, "%s %s", APP_NAME " background", enlil_library_path_get(enlil_data->library));
   enlil_eet_app_data_save(edd, buf, &s);
   eet_data_descriptor_free(edd);
   eina_stringshare_del(s.string);
   break;
}
}

static void
_library_photo1_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   Eina_List *l;
   Enlil_Photo *photo;
   PL_Child_Item *child;

   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
   {
      photo = photos_list_object_item_data_get(child);
      enlil_library_photo_set(enlil_data->library, photo, 1);
      break;
   }
   main_menu_update_libraries_list();
}

static void
_library_photo2_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = data;
   Eina_List *l;
   Enlil_Photo *photo;
   PL_Child_Item *child;

   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_photo->o_list), l, child)
   {
      photo = photos_list_object_item_data_get(child);
      enlil_library_photo_set(enlil_data->library, photo, 2);
      break;
   }
   main_menu_update_libraries_list();
}

void
photo_menu_free(Photo_Menu *photo_menu)
{
   ASSERT_RETURN_VOID(photo_menu != NULL);

   if (photo_menu->photos) eina_list_free(photo_menu->photos);
   evas_object_del(photo_menu->menu);
}

static void
_inwin_del_cb(void *data)
{
   Eina_List *photos = data;
   eina_list_free(photos);
   photo_menu_init(enlil_data->list_photo, _edje);
}

