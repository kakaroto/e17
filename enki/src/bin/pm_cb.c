#include "main.h"
#include "evas_object/photo_object.h"
#include "evas_object/slideshow_object.h"

void
album_new(void *data, Enlil_Album *album)
{
   char buf[PATH_MAX];
   Enlil_Data *enlil_data = (Enlil_Data *) data;
   Enlil_Library *_library = enlil_data->library;
   enlil_album_monitor_start(album);

   enlil_album_collection_process(album);

   Enlil_Album_Data *enlil_album_data = calloc(1, sizeof(Enlil_Album_Data));
   enlil_album_data->enlil_data = enlil_data;
   enlil_album_user_data_set(album, enlil_album_data, enlil_album_data_free);

   enlil_album_photos_sort_set(album, ENLIL_PHOTO_SORT_DATE);
   enlil_album_photos_sort_set(album, ENLIL_PHOTO_SORT_DATE);

   Enlil_Album *album_prev = enlil_library_album_prev_get(_library, album);
   Enlil_Album_Data *album_data_prev = enlil_album_user_data_get(album_prev);
   if (!album_prev || !album_data_prev)
   {
      list_left_append_relative(enlil_data->list_left, album, NULL);
      list_photo_album_append_relative(enlil_data->list_photo, album, NULL);
   }
   else
   {
      list_left_append_relative(enlil_data->list_left, album,
                                album_data_prev->list_album_item);
      list_photo_album_append_relative(enlil_data->list_photo, album,
                                       album_data_prev->list_photo_item);
   }

   import_album_new(album);

   //snprintf(buf, PATH_MAX, "%s %s",D_("New Album : "), enlil_album_name_get(album));
   //notify_sync_content_set(enlil_data, buf);

   enlil_data->nb_albums++;

   snprintf(buf, PATH_MAX, D_("  %d albums and %d photos."),
            enlil_data->nb_albums, enlil_data->nb_photos);
   elm_label_label_set(enlil_data->list_photo->lbl_nb_albums_photos, buf);

   Enlil_NetSync_Job
            *job =
                     enlil_netsync_job_sync_photos_append(
                                                          album,
                                                          netsync_photo_new_cb,
                                                          netsync_photo_notinnetsync_cb,
                                                          netsync_photo_notuptodate_cb,
                                                          netsync_photo_netsyncnotuptodate_cb,
                                                          netsync_photo_uptodate_cb,
                                                          netsync_photo_tags_notuptodate_cb,
                                                          netsync_photo_tags_netsyncnotuptodate_cb,
                                                          netsync_photo_tags_uptodate_cb,
                                                          enlil_data);
}

void
load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos)
{
   char buf[PATH_MAX];
   Enlil_Data *enlil_data = (Enlil_Data *) data;
   Enlil_Library *library = enlil_data->library;

   snprintf(buf, PATH_MAX, D_("Loading %d albums and %d photos"), nb_albums,
            nb_photos);

   loading_status_show(enlil_data, buf, EINA_FALSE);

   enlil_load_free(&load);
   enlil_data->load = NULL;

   Enlil_Sync *sync = enlil_library_sync_get(library);

   enlil_sync_job_all_add(sync);

   main_menu_loading_disable_set(0);
   main_menu_sync_disable_set(1);

   enlil_data->nb_photos = nb_photos;
   enlil_data->nb_albums = nb_albums;

   snprintf(buf, PATH_MAX, D_("  %d albums and %d photos."),
            enlil_data->nb_albums, enlil_data->nb_photos);
   elm_label_label_set(enlil_data->list_photo->lbl_nb_albums_photos, buf);

   photos_list_object_freeze(enlil_data->list_photo->o_list, EINA_FALSE);

   enlil_netsync_job_sync_albums_append(enlil_data->library,
                                        netsync_album_new_cb,
                                        netsync_album_notinnetsync_cb,
                                        netsync_album_notuptodate_cb,
                                        netsync_album_netsyncnotuptodate_cb,
                                        netsync_album_uptodate_cb, enlil_data);
}

void
load_error_cb(void *data, Enlil_Load *load, Load_Error error, const char *msg)
{
   printf("LOAD CB ERROR : %s\n", msg);
}

void
load_album_done_cb(void *data, Enlil_Load *load, Enlil_Library *library,
                   Enlil_Album *album)
{
   Eina_List *l;
   Enlil_Photo *photo;
   Enlil_Photo *photo_prev = NULL;

   Enlil_Data *enlil_data = (Enlil_Data *) data;
   enlil_album_monitor_start(album);

   Enlil_Album_Data *enlil_album_data = calloc(1, sizeof(Enlil_Album_Data));
   enlil_album_data->enlil_data = enlil_data;
   enlil_album_user_data_set(album, enlil_album_data, enlil_album_data_free);

   enlil_album_collection_process(album);

   list_left_add(enlil_data->list_left, album);
   list_photo_album_add(enlil_data->list_photo, album);

   //photos_list_object_freeze(enlil_data->list_photo->o_list, 1);
EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
{
   Enlil_Photo_Data *enlil_photo_data = calloc(1, sizeof(Enlil_Photo_Data));
   enlil_photo_user_data_set(photo, enlil_photo_data, enlil_photo_data_free);
   enlil_photo_data->enlil_data = enlil_data;

   if(!photo_prev)
   list_photo_photo_append_relative(enlil_data->list_photo, album, photo, NULL);
   else
   {
      Enlil_Photo_Data *photo_data_prev = enlil_photo_user_data_get(photo_prev);
      list_photo_photo_append_relative(enlil_data->list_photo, album, photo,
               photo_data_prev->list_photo_item);
   }
   photo_prev = photo;

   map_photo_add(enlil_data->map, photo);

   enlil_photo_tag_process(photo);

   if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO && !enlil_photo_exif_loaded_get(photo))
   {
      enlil_photo_data->clear_exif_data = EINA_TRUE;
      enlil_photo_data->exif_job = enlil_exif_job_append(photo, exif_load_done, photo);
   }
   if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO && !enlil_photo_iptc_loaded_get(photo))
   {
      enlil_photo_data->clear_iptc_data = EINA_TRUE;
      enlil_photo_data->iptc_job = enlil_iptc_job_append(photo, iptc_load_done, photo);
   }
}
//photos_list_object_freeze(enlil_data->list_photo->o_list, 0);
}

void
sync_done_cb(void *data, Enlil_Sync *sync)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;

   sync_status_show(enlil_data, D_("Synchronisation done"), EINA_FALSE);

   enlil_file_manager_flush();

   if (enlil_sync_jobs_count_get(sync) <= 0) main_menu_sync_disable_set(0);
}

void
sync_start_cb(void *data, Enlil_Sync *sync)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;

   sync_status_show(enlil_data, D_("   Synchronisation ..."), EINA_TRUE);
}

void
sync_error_cb(void *data, Enlil_Sync *sync, Sync_Error error, const char *msg)
{
   printf("SYNC CB ERROR : %s\n", msg);

   if (enlil_sync_jobs_count_get(sync) <= 0) main_menu_sync_disable_set(0);
}

void
sync_album_new_cb(void *data, Enlil_Sync *sync, Enlil_Library *library,
                  Enlil_Album *album)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;
   Enlil_Library *_library = enlil_data->library;

   Enlil_Album *_album = enlil_album_copy_new(album);
   enlil_library_album_add(_library, _album);
   album_new(data, _album);
   //Enlil_Album_Data *album_data = enlil_album_user_data_get(_album);

   //Enlil_NetSync_Job *job =
   enlil_netsync_job_sync_albums_append(enlil_data->library,
                                        netsync_album_new_cb,
                                        netsync_album_notinnetsync_cb,
                                        netsync_album_notuptodate_cb,
                                        netsync_album_netsyncnotuptodate_cb,
                                        netsync_album_uptodate_cb, enlil_data);
}

void
sync_album_update_cb(void *data, Enlil_Sync *sync, Enlil_Library *library,
                     Enlil_Album *album)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;
   Enlil_Library *_library = enlil_data->library;

   Enlil_Album
            *_album =
                     enlil_library_album_search_file_name(
                                                          _library,
                                                          enlil_album_file_name_get(
                                                                                    album));
   ASSERT_RETURN_VOID(_album != NULL);

   enlil_album_copy(album, _album);

   list_left_update(enlil_data->list_left, _album);
   Enlil_Album_Data *enlil_album_data = enlil_album_user_data_get(_album);
   photos_list_object_header_update(enlil_album_data->list_photo_item);

   enlil_netsync_job_sync_albums_append(enlil_data->library,
                                        netsync_album_new_cb,
                                        netsync_album_notinnetsync_cb,
                                        netsync_album_notuptodate_cb,
                                        netsync_album_netsyncnotuptodate_cb,
                                        netsync_album_uptodate_cb, enlil_data);

   //snprintf(buf, PATH_MAX, "%s %s",D_("Update Album : "), enlil_album_name_get(album));
   //notify_sync_content_set(enlil_data, buf);
}

void
sync_album_disappear_cb(void *data, Enlil_Sync *sync, Enlil_Library *library,
                        Enlil_Album *album)
{
   char buf[PATH_MAX];
   Enlil_Data *enlil_data = (Enlil_Data *) data;
   Enlil_Library *_library = enlil_data->library;
   Enlil_Album
            *_album =
                     enlil_library_album_search_file_name(
                                                          _library,
                                                          enlil_album_file_name_get(
                                                                                    album));
   ASSERT_RETURN_VOID(_album != NULL);

   //snprintf(buf, PATH_MAX, "%s %s",D_("Delete Album : "), enlil_album_name_get(album));
   //notify_sync_content_set(enlil_data, buf);

   enlil_album_free(&_album);

   enlil_data->nb_albums--;

   snprintf(buf, PATH_MAX, D_("  %d albums and %d photos."),
            enlil_data->nb_albums, enlil_data->nb_photos);
   elm_label_label_set(enlil_data->list_photo->lbl_nb_albums_photos, buf);
}

void
sync_photo_new_cb(void *data, Enlil_Sync *sync, Enlil_Album *album,
                  Enlil_Photo *photo)
{
   char buf[PATH_MAX];
   Enlil_Data *enlil_data = (Enlil_Data *) data;
   Enlil_Library *_library = enlil_data->library;

   Enlil_Album
            *_album =
                     enlil_library_album_search_file_name(
                                                          _library,
                                                          enlil_album_file_name_get(
                                                                                    album));
   Enlil_Album_Data *album_data = enlil_album_user_data_get(_album);
   ASSERT_RETURN_VOID(_album != NULL);

   Enlil_Photo
            *_photo =
                     enlil_album_photo_search_file_name(
                                                        _album,
                                                        enlil_photo_file_name_get(
                                                                                  photo));

   //In some case the photo can already exist
   //For example if we call sync_photo_new_cb() from sync_photo_update_cb()
   if (!_photo)
   {
      _photo = enlil_photo_copy_new(photo);
      enlil_album_photo_add(_album, _photo);
   }

   Enlil_Photo_Data *enlil_photo_data = calloc(1, sizeof(Enlil_Photo_Data));
   enlil_photo_user_data_set(_photo, enlil_photo_data, enlil_photo_data_free);
   enlil_photo_data->enlil_data = enlil_data;

   //insert after
   Enlil_Photo *photo_prev = enlil_album_photo_prev_get(_album, _photo);
   if (!photo_prev)
      list_photo_photo_append_relative(enlil_data->list_photo, _album, _photo,
                                       NULL);
   else
   {
      Enlil_Photo_Data *photo_data_prev = enlil_photo_user_data_get(photo_prev);
      list_photo_photo_append_relative(enlil_data->list_photo, _album, _photo,
                                       photo_data_prev->list_photo_item);
   }

   map_photo_add(enlil_data->map, _photo);

   //snprintf(buf, PATH_MAX, D_("New Photo %s in the album %s "), enlil_photo_name_get(_photo), enlil_album_name_get(_album));
   //notify_sync_content_set(enlil_data, buf);

   enlil_data->nb_photos++;

   snprintf(buf, PATH_MAX, D_("  %d albums and %d photos."),
            enlil_data->nb_albums, enlil_data->nb_photos);
   elm_label_label_set(enlil_data->list_photo->lbl_nb_albums_photos, buf);

   enlil_photo_tag_process(_photo);
   if (enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO)
   {
      enlil_photo_data->exif_job
               = enlil_exif_job_append(_photo, exif_load_done, _photo);
      enlil_photo_data->clear_exif_data = EINA_TRUE;
   }
   if (enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO)
   {
      enlil_photo_data->iptc_job
               = enlil_iptc_job_append(_photo, iptc_load_done, _photo);
      enlil_photo_data->clear_iptc_data = EINA_TRUE;
   }

   enlil_netsync_job_sync_photos_append(
                                        _album,
                                        netsync_photo_new_cb,
                                        netsync_photo_notinnetsync_cb,
                                        netsync_photo_notuptodate_cb,
                                        netsync_photo_netsyncnotuptodate_cb,
                                        netsync_photo_uptodate_cb,
                                        netsync_photo_tags_notuptodate_cb,
                                        netsync_photo_tags_netsyncnotuptodate_cb,
                                        netsync_photo_tags_uptodate_cb,
                                        enlil_data);
}

void
sync_photo_update_cb(void *data, Enlil_Sync *sync, Enlil_Album *album,
                     Enlil_Photo *photo)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;
   Enlil_Library *_library = enlil_data->library;

   Enlil_Album
            *_album =
                     enlil_library_album_search_file_name(
                                                          _library,
                                                          enlil_album_file_name_get(
                                                                                    album));
   ASSERT_RETURN_VOID(_album != NULL);

   Enlil_Photo
            *_photo =
                     enlil_album_photo_search_file_name(
                                                        _album,
                                                        enlil_photo_file_name_get(
                                                                                  photo));
   enlil_photo_copy(photo, _photo);

   enlil_photo_thumb_fdo_normal_set(_photo, NULL);
   enlil_photo_thumb_fdo_large_set(_photo, NULL);

   Enlil_Photo_Data *enlil_photo_data = enlil_photo_user_data_get(_photo);
   if (enlil_photo_data)
   {
      enlil_photo_data->cant_create_thumb--;
      photos_list_object_item_update(enlil_photo_data->list_photo_item);

      //snprintf(buf, PATH_MAX, D_("Update Photo %s from the album %s "), enlil_photo_name_get(_photo), enlil_album_name_get(_album));
      //notify_sync_content_set(enlil_data, buf);

      enlil_photo_data->exif_job
               = enlil_exif_job_append(_photo, exif_load_done, _photo);
      enlil_photo_data->iptc_job
               = enlil_iptc_job_append(_photo, iptc_load_done, _photo);
   }
   else
   {
      sync_photo_new_cb(data, sync, album, photo);
   }
}

void
sync_photo_disappear_cb(void *data, Enlil_Sync *sync, Enlil_Album *album,
                        Enlil_Photo *photo)
{
   char buf[PATH_MAX];
   Enlil_Data *enlil_data = (Enlil_Data *) data;
   Enlil_Library *_library = enlil_data->library;

   Enlil_Album
            *_album =
                     enlil_library_album_search_file_name(
                                                          _library,
                                                          enlil_album_file_name_get(
                                                                                    album));
   ASSERT_RETURN_VOID(_album != NULL);
   Enlil_Photo
            *_photo =
                     enlil_album_photo_search_file_name(
                                                        _album,
                                                        enlil_photo_file_name_get(
                                                                                  photo));

   //snprintf(buf, PATH_MAX, D_("Delete Photo %s from the album %s"), enlil_photo_name_get(_photo), enlil_album_name_get(_album));
   //notify_sync_content_set(enlil_data, buf);

   enlil_album_photo_remove(_album, _photo);
   enlil_photo_free(&_photo);

   enlil_data->nb_photos--;

   snprintf(buf, PATH_MAX, D_("  %d albums and %d photos."),
            enlil_data->nb_albums, enlil_data->nb_photos);
   elm_label_label_set(enlil_data->list_photo->lbl_nb_albums_photos, buf);
}

void
monitor_album_new_cb(void *data, Enlil_Library *library, const char *path)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;

   if (!enlil_data->load)
   {
      const char *file_name = ecore_file_file_get(path);
      Enlil_Sync *sync = enlil_library_sync_get(enlil_data->library);

      enlil_sync_job_album_folder_add(sync, file_name);

      main_menu_sync_disable_set(1);
   }
}

void
monitor_album_update_cb(void *data, Enlil_Library *library, Enlil_Album *album)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;

   if (!enlil_data->load)
   {
      const char *file_name = enlil_album_file_name_get(album);
      Enlil_Sync *sync = enlil_library_sync_get(enlil_data->library);

      enlil_sync_job_album_folder_add(sync, file_name);

      main_menu_sync_disable_set(1);
   }
}

void
monitor_album_delete_cb(void *data, Enlil_Library *library, const char *path)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;

   if (!enlil_data->load)
   {
      const char *file_name = ecore_file_file_get(path);
      Enlil_Sync *sync = enlil_library_sync_get(library);
      enlil_sync_job_album_folder_add(sync, file_name);

      main_menu_sync_disable_set(1);
   }
}

void
monitor_enlil_delete_cb(void *data, Enlil_Library *library)
{
   printf("Enlil delete !!!\n");
}

void
monitor_photo_new_cb(void *data, Enlil_Library *library, Enlil_Album *album,
                     const char *path)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;

   if (!enlil_data->load)
   {
      const char *file_name = ecore_file_file_get(path);
      Enlil_Sync *sync = enlil_library_sync_get(enlil_data->library);

      enlil_sync_job_photo_file_add(sync, enlil_album_file_name_get(album),
                                    file_name);
      main_menu_sync_disable_set(1);
   }
}

void
monitor_photo_delete_cb(void *data, Enlil_Library *library, Enlil_Album *album,
                        const char *path)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;

   if (!enlil_data->load)
   {
      const char *file_name = ecore_file_file_get(path);
      Enlil_Sync *sync = enlil_library_sync_get(library);
      enlil_sync_job_photo_file_add(sync, enlil_album_file_name_get(album),
                                    file_name);
      main_menu_sync_disable_set(1);
   }
}

void
monitor_photo_update_cb(void *data, Enlil_Library *library, Enlil_Album *album,
                        const char *path)
{
   Enlil_Data *enlil_data = (Enlil_Data *) data;

   if (!enlil_data->load)
   {
      const char *file_name = ecore_file_file_get(path);
      Enlil_Sync *sync = enlil_library_sync_get(enlil_data->library);

      enlil_sync_job_photo_file_add(sync, enlil_album_file_name_get(album),
                                    file_name);
      main_menu_sync_disable_set(1);
   }
}

void
thumb_done_cb(void *data, Enlil_Photo *photo, const char *file)
{
   Eina_List *l;
   Slideshow_Item *item;

   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   if (photo_data->list_photo_item) photos_list_object_item_update(
                                                                   photo_data->list_photo_item);
   if (enlil_data->map) map_photo_update(enlil_data->map, photo);
   if (photo_data->library_item) elm_gengrid_item_update(
                                                         photo_data->library_item);

EINA_LIST_FOREACH(photo_data->slideshow_object_items, l, item)
{
   slideshow_object_item_update(item);
}
}

void
thumb_error_cb(void *data, Enlil_Photo *photo)
{
   Enlil_Photo_Data *enlil_photo_data = enlil_photo_user_data_get(photo);

   if (enlil_photo_data->cant_create_thumb != -1) enlil_photo_data->cant_create_thumb++;
}

void
collection_new_cb(void *data, Enlil_Library *library, Enlil_Collection *col)
{
   Enlil_Data *enlil_data = data;

   Enlil_Collection_Data *col_data = calloc(1, sizeof(Enlil_Collection_Data));
   col_data->col = col;
   col_data->enlil_data = enlil_data;

   enlil_collection_user_data_set(col, col_data, enlil_collection_data_free);
   list_left_col_add(enlil_data->list_left, col);
}

void
collection_delete_cb(void *data, Enlil_Library *library, Enlil_Collection *col)
{
   //Enlil_Data *enlil_data = data;
   Enlil_Collection_Data *col_data = enlil_collection_user_data_get(col);

   elm_genlist_item_del(col_data->list_col_item);
   col_data->list_col_item = NULL;
}

void
collection_album_new_cb(void *data, Enlil_Library *library,
                        Enlil_Collection *col, Enlil_Album *album)
{
   Enlil_Data *enlil_data = data;
   //Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   list_left_col_album_add(enlil_data->list_left, col, album);
}

void
collection_album_delete_cb(void *data, Enlil_Library *library,
                           Enlil_Collection *col, Enlil_Album *album)
{
   Enlil_Data *enlil_data = data;
   //Enlil_Collection_Data *col_data = enlil_collection_user_data_get(col);

   list_left_col_album_remove(enlil_data->list_left, col, album);
}

void
tag_new_cb(void *data, Enlil_Library *library, Enlil_Tag *tag)
{
   Enlil_Data *enlil_data = data;
   Enlil_Tag_Data *tag_data = calloc(1, sizeof(Enlil_Tag_Data));

   tag_data->tag = tag;
   tag_data->enlil_data = enlil_data;

   enlil_tag_user_data_set(tag, tag_data, enlil_tag_data_free);
   list_left_tag_add(enlil_data->list_left, tag);
}

void
tag_delete_cb(void *data, Enlil_Library *library, Enlil_Tag *tag)
{
   //Enlil_Data *enlil_data = data;
   Enlil_Tag_Data *tag_data = enlil_tag_user_data_get(tag);

   elm_genlist_item_del(tag_data->list_tag_item);
   tag_data->list_tag_item = NULL;
}

void
tag_photo_new_cb(void *data, Enlil_Library *library, Enlil_Tag *tag,
                 Enlil_Photo *photo)
{
   Enlil_Data *enlil_data = data;
   Enlil_Tag_Data *tag_data = enlil_tag_user_data_get(tag);
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if (tag_data->list_tag_item
            == elm_genlist_selected_item_get(enlil_data->list_left->gl_tags)) photos_list_object_item_show(
                                                                                                           photo_data->list_photo_item);
}

void
tag_photo_delete_cb(void *data, Enlil_Library *library, Enlil_Tag *tag,
                    Enlil_Photo *photo)
{
   Enlil_Data *enlil_data = data;
   Enlil_Tag_Data *tag_data = enlil_tag_user_data_get(tag);
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   if (tag_data->list_tag_item
            == elm_genlist_selected_item_get(enlil_data->list_left->gl_tags)) photos_list_object_item_hide(
                                                                                                           photo_data->list_photo_item);
}

void
exif_load_done(void *data, Enlil_Exif_Job *job, Eina_List *exifs)
{
   char buf[PATH_MAX];
   Evas_Coord w, h;
   Enlil_Photo *photo = data;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   photo_data->exif_job = NULL;
   enlil_photo_exif_loaded_set(photo, EINA_TRUE);
   enlil_photo_exifs_set(photo, exifs);

   //get the resolution
   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
            enlil_photo_file_name_get(photo));
   GET_PHOTO_RESOLUTION(buf, enlil_data->win->win, w, h);
   enlil_photo_size_w_set(photo, w);
   enlil_photo_size_h_set(photo, h);
   enlil_photo_eet_save(photo);

   panel_image_exifs_update(photo);

   map_photo_update(enlil_data->map, photo);

   if (enlil_album_photos_sort_get(enlil_photo_album_get(photo))
            == ENLIL_PHOTO_SORT_DATE)
   {
      Enlil_Photo *photo_prev =
               enlil_album_photo_prev_get(enlil_photo_album_get(photo), photo);
      if (!photo_prev)
         photos_list_object_child_move_after(photo_data->list_photo_item, NULL);
      else
      {
         Enlil_Photo_Data *photo_data_prev =
                  enlil_photo_user_data_get(photo_prev);
         photos_list_object_child_move_after(photo_data->list_photo_item,
                                             photo_data_prev->list_photo_item);
      }
   }

   if (photo_data->clear_exif_data)
   {
      photo_data->clear_exif_data = EINA_FALSE;
      enlil_photo_exif_clear(photo);
   }
}

void
iptc_load_done(void *data, Enlil_IPTC_Job *job, Eina_List *iptcs)
{
   Enlil_Photo *photo = data;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   photo_data->iptc_job = NULL;
   enlil_photo_iptc_loaded_set(photo, EINA_TRUE);
   enlil_photo_iptcs_set(photo, iptcs);
   enlil_photo_eet_save(photo);

   panel_image_iptcs_update(photo);

   if (photo_data->clear_iptc_data)
   {
      photo_data->clear_iptc_data = EINA_FALSE;
      enlil_photo_iptc_clear(photo);
   }
}

static Eina_Bool
_geocaching_foreach_cb(const Eina_Hash *hash, const void *key, void *data,
                       void *fdata)
{
   Enlil_Geocaching *gp = data;
   Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   if (!gp_data)
   {
      gp_data = calloc(1, sizeof(Geocaching_Data));
      enlil_geocaching_user_data_set(gp, gp_data, enlil_geocaching_data_free);
      map_geocaching_add(enlil_data->map, gp);
   }
   else
      map_geocaching_update(enlil_data->map, gp);

   return EINA_TRUE;
}

void
geocaching_done_cb(void *data, Eina_Hash *db)
{
   eina_hash_foreach(db, _geocaching_foreach_cb, NULL);
}

static Eina_Bool
_geocaching_remove_marker_foreach_cb(const Eina_Hash *hash, const void *key,
                                     void *data, void *fdata)
{
   Enlil_Geocaching *gp = data;
   Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   if (gp_data) map_geocaching_remove(enlil_data->map, gp);

   return EINA_TRUE;
}

void
geocaching_remove_marker_cb(void *data, Eina_Hash *db)
{
   eina_hash_foreach(db, _geocaching_remove_marker_foreach_cb, NULL);
}

void
album_version_header_increase_cb(void *data, Enlil_Album *album)
{
   //Enlil_NetSync_Job *job =
   enlil_netsync_job_sync_album_append(enlil_data->library, album,
                                       netsync_album_notuptodate_cb,
                                       netsync_album_netsyncnotuptodate_cb,
                                       netsync_album_uptodate_cb, enlil_data);
}

void
photo_version_header_increase_cb(void *data, Enlil_Photo *photo)
{
   Enlil_NetSync_Job
            *job =
                     enlil_netsync_job_sync_photos_append(
                                                          enlil_photo_album_get(
                                                                                photo),
                                                          netsync_photo_new_cb,
                                                          netsync_photo_notinnetsync_cb,
                                                          netsync_photo_notuptodate_cb,
                                                          netsync_photo_netsyncnotuptodate_cb,
                                                          netsync_photo_uptodate_cb,
                                                          netsync_photo_tags_notuptodate_cb,
                                                          netsync_photo_tags_netsyncnotuptodate_cb,
                                                          netsync_photo_tags_uptodate_cb,
                                                          enlil_data);
}

void
photo_version_tags_increase_cb(void *data, Enlil_Photo *photo)
{
   Enlil_NetSync_Job
            *job =
                     enlil_netsync_job_sync_photos_append(
                                                          enlil_photo_album_get(
                                                                                photo),
                                                          netsync_photo_new_cb,
                                                          netsync_photo_notinnetsync_cb,
                                                          netsync_photo_notuptodate_cb,
                                                          netsync_photo_netsyncnotuptodate_cb,
                                                          netsync_photo_uptodate_cb,
                                                          netsync_photo_tags_notuptodate_cb,
                                                          netsync_photo_tags_netsyncnotuptodate_cb,
                                                          netsync_photo_tags_uptodate_cb,
                                                          enlil_data);
}

