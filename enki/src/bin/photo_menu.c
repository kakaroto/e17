#include "main.h"

static void _menu_dismiss_cb(void *data, Evas_Object *obj, void *event_info);
static void _tag_photo_add_cb(void *data, Evas_Object *obj, void *event_info);
static void _tag_photo_remove_cb(void *data, Evas_Object *obj, void *event_info);
static void _tag_new_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_thumb_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_iptc_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_exif_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_move_album_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_move_album_list_cb(void *data, Evas_Object *obj, void *event_info);
static void _wall_app_set_cb(void *data, Evas_Object *obj, void *event_info);
static void _library_photo1_set_cb(void *data, Evas_Object *obj, void *event_info);
static void _library_photo2_set_cb(void *data, Evas_Object *obj, void *event_info);
static void _inwin_del_cb(void *data);

Photo_Menu *photo_menu_new(Evas_Object *parent, Enlil_Photo *photo, Eina_List *photos)
{
   Evas_Object *menu;
   Elm_Menu_Item *mi = NULL;
   const Eina_List *l, *_l;
   const Enlil_Tag *tag;
   Enlil_Photo *_photo;
   Eina_Bool empty = EINA_TRUE;
   Photo_Menu * am = calloc(1, sizeof(Photo_Menu));
   am->photo = photo;
   am->photos = photos;

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", _menu_dismiss_cb, am);
   am->menu = menu;

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Remove the tag"), NULL, NULL);

   Enlil_Album *album = enlil_photo_album_get(eina_list_data_get(photos));
   EINA_LIST_FOREACH(enlil_library_tags_get(enlil_album_library_get(album)),l, tag)
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
	     elm_menu_item_add(menu, mi, NULL, enlil_tag_name_get(tag), _tag_photo_remove_cb, am);
	     empty = EINA_FALSE;
	  }
     }

   if(empty)
      elm_menu_item_add(menu, mi, NULL, D_("Empty"), NULL, NULL);

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Add the tag"), NULL, NULL);
   elm_menu_item_add(menu, mi, NULL, D_("New tag"), _tag_new_cb, am);
   elm_menu_item_separator_add(menu, mi);

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
	 elm_menu_item_add(menu, mi, NULL, enlil_tag_name_get(tag), _tag_photo_add_cb, am);
   }

   if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO)
     {
	elm_menu_item_separator_add(menu, NULL);
	mi = elm_menu_item_add(menu, NULL, NULL, D_("Set as wallpaper"), NULL, NULL);
	elm_menu_item_add(menu, mi, NULL, D_("To the application"), _wall_app_set_cb, am);


	mi = elm_menu_item_add(menu, NULL, NULL, D_("Set as library's photo"), NULL, NULL);
	elm_menu_item_add(menu, mi, NULL, D_("Front photo"), _library_photo1_set_cb, am);
	elm_menu_item_add(menu, mi, NULL, D_("Back photo"), _library_photo2_set_cb, am);
     }


   elm_menu_item_separator_add(menu, NULL);
   mi = elm_menu_item_add(menu, NULL, NULL, D_("Move to the album"), NULL, NULL);

   int i= 0;
   EINA_LIST_FOREACH(enlil_library_albums_get(enlil_album_library_get(enlil_photo_album_get(photo))), l, album)
     {
	 Elm_Menu_Item *mi2;
	 if(album != enlil_photo_album_get(photo))
	   mi2 = elm_menu_item_add(menu, mi, NULL, enlil_album_name_get(album), _photo_move_album_cb, am);
	 if(++i>15)
	   {
	       mi2 = elm_menu_item_add(menu, mi, NULL, D_("..."), _photo_move_album_list_cb, am);
	       break;
	   }
     }


   elm_menu_item_separator_add(menu, NULL);
   mi = elm_menu_item_add(menu, NULL, NULL, D_("Recreate the thumbnail"), _photo_thumb_reload_cb, am);
   elm_menu_item_add(menu, NULL, NULL, D_("Reload the exifs data"), _photo_exif_reload_cb, am);
   elm_menu_item_add(menu, NULL, NULL, D_("Reload the IPTC data"), _photo_iptc_reload_cb, am);

   elm_menu_item_separator_add(menu, NULL);
   if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO)
     mi = elm_menu_item_add(menu, NULL, NULL, D_("Delete the photo"), _photo_delete_cb, am);
   else
     mi = elm_menu_item_add(menu, NULL, NULL, D_("Delete the video"), _photo_delete_cb, am);

   evas_object_show(menu);
   return am;
}

static void _menu_dismiss_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *menu = data;
   photo_menu_free(menu);
}

static void _tag_photo_add_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *am = data;
   Elm_Menu_Item *mi = event_info;
   Eina_List *l;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(am->photos, l, photo)
      enlil_photo_tag_add(photo, elm_menu_item_label_get(mi));

   photo_menu_free(am);
}

static void _tag_photo_remove_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *am = data;
   Elm_Menu_Item *mi = event_info;
   const Eina_List *l, *l2;
   Enlil_Photo_Tag *tag_col;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(am->photos, l, photo)
     {
	EINA_LIST_FOREACH(enlil_photo_tags_get(photo), l2, tag_col)
	  {
	     if(tag_col->name == elm_menu_item_label_get(mi))
	       break;
	  }
	if(tag_col)
	  enlil_photo_tag_remove(photo, tag_col);
     }

   photo_menu_free(am);
}

static void _tag_new_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   inwin_tag_new_new(_inwin_del_cb, photo_menu, photo_menu->photos);
}

static void _photo_move_album_list_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   inwin_photo_move_album_new(_inwin_del_cb, photo_menu, photo_menu->photos);
}

static void _photo_move_album_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   Elm_Menu_Item *mi = event_info;
   char buf[PATH_MAX], buf2[PATH_MAX];
   Eina_List *l;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(photo_menu->photos, l, photo)
     {
	Enlil_Album *album = enlil_library_album_search_name(enlil_album_library_get(enlil_photo_album_get(photo)),
	      elm_menu_item_label_get(mi));
	ASSERT_RETURN_VOID(album != NULL);

	snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
	      enlil_photo_file_name_get(photo));

	snprintf(buf2, PATH_MAX, "%s/%s/%s", enlil_album_path_get(album), enlil_album_file_name_get(album),
	      enlil_photo_file_name_get(photo));

	ecore_file_mv(buf, buf2);
     }
}

static void _photo_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   inwin_photo_delete_new(enlil_data->win->win, _inwin_del_cb, photo_menu, photo_menu->photos);
}

static void _photo_thumb_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   Eina_List *l;
   Enlil_Photo_Data *photo_data;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(photo_menu->photos, l, photo)
     {
	photo_data = enlil_photo_user_data_get(photo);
	enlil_photo_thumb_fdo_normal_set(photo, NULL);
	enlil_photo_thumb_fdo_large_set(photo, NULL);
	photos_list_object_item_update(photo_data->list_photo_item);
	map_photo_update(enlil_data->map, photo);
     }
}

static void _photo_exif_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   Eina_List *l;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(photo_menu->photos, l, photo)
     {
	enlil_exif_job_prepend(photo, exif_load_done, photo);
     }
}

static void _photo_iptc_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   Eina_List *l;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(photo_menu->photos, l, photo)
     {
	enlil_iptc_job_prepend(photo, iptc_load_done, photo);
     }
}

static void _wall_app_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   char buf[PATH_MAX];
   Enlil_String s;
   Eet_Data_Descriptor *edd;

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo_menu->photo),
	 enlil_photo_file_name_get(photo_menu->photo));
   enlil_win_bg_set(enlil_data->win, buf);

   s.string = eina_stringshare_add(buf);
   edd = enlil_string_edd_new();

   snprintf(buf, PATH_MAX, "%s %s", APP_NAME" background", enlil_library_path_get(enlil_data->library));
   enlil_eet_app_data_save(edd, buf, &s);
   eet_data_descriptor_free(edd);
   eina_stringshare_del(s.string);
}

static void _library_photo1_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   enlil_library_photo_set(enlil_data->library, photo_menu->photo, 1);
   main_menu_update_libraries_list();
}

static void _library_photo2_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   Photo_Menu *photo_menu = data;
   enlil_library_photo_set(enlil_data->library, photo_menu->photo, 2);
   main_menu_update_libraries_list();
}

void photo_menu_free(Photo_Menu *photo_menu)
{
   ASSERT_RETURN_VOID(photo_menu != NULL);

   if(photo_menu->photos)
     eina_list_free(photo_menu->photos);
   evas_object_del(photo_menu->menu);
}

static void _inwin_del_cb(void *data)
{
   photo_menu_free(data);
}

