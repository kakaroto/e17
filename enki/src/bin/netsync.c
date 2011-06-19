#include "main.h"

void
netsync_album_header_get_cb(void *data, Enlil_Library *library,
                            Enlil_Album *album)
{
   album_new(enlil_data, album);
}

void
netsync_album_new_cb(void *data, Enlil_Library *library, int album_id)
{
   enlil_netsync_job_get_new_album_header_append(library, album_id,
                                                 netsync_album_header_get_cb,
                                                 data);
}

void
netsync_album_netsyncnotuptodate_cb(void *data, Enlil_Library *library,
                                    Enlil_Album *album)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   album_data->netsync.album_netsync_notuptodate = EINA_TRUE;

   if (photos_list_object_header_object_get(album_data->list_photo_item)) edje_object_signal_emit(
                                                                                                  album_data->netsync.icon,
                                                                                                  album_netsync_edje_signal_get(
                                                                                                                                album),
                                                                                                  "");

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
netsync_album_notuptodate_cb(void *data, Enlil_Library *library,
                             Enlil_Album *album)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   album_data->netsync.album_local_notuptodate = EINA_TRUE;

   if (photos_list_object_header_object_get(album_data->list_photo_item)) edje_object_signal_emit(
                                                                                                  album_data->netsync.icon,
                                                                                                  album_netsync_edje_signal_get(
                                                                                                                                album),
                                                                                                  "");

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
netsync_album_notinnetsync_cb(void *data, Enlil_Library *library,
                              Enlil_Album *album)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   album_data->netsync.album_notinnetsync = EINA_TRUE;

   if (photos_list_object_header_object_get(album_data->list_photo_item)) edje_object_signal_emit(
                                                                                                  album_data->netsync.icon,
                                                                                                  album_netsync_edje_signal_get(
                                                                                                                                album),
                                                                                                  "");

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
netsync_album_uptodate_cb(void *data, Enlil_Library *library,
                          Enlil_Album *album)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
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
netsync_error_cb(void *data, Enlil_Library *library)
{
   printf("ERROR\n");
}

void
netsync_photo_new_cb(void *data, Enlil_Album *album, int id)
{
   printf("photo not in local %d\n", id);
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   album_data->netsync.photos_notinlocal = EINA_TRUE;

   if (photos_list_object_header_object_get(album_data->list_photo_item)) edje_object_signal_emit(
                                                                                                  album_data->netsync.icon,
                                                                                                  album_netsync_edje_signal_get(
                                                                                                                                album),
                                                                                                  "");
}

void
netsync_photo_notinnetsync_cb(void *data, Enlil_Album *album,
                              Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   photo_data->netsync.state = PHOTO_NETSYNC_NOTINNETSYNC;

   Evas_Object
            *o =
                     (Evas_Object *) photos_list_object_item_object_get(
                                                                        photo_data->list_photo_item);
   if (o) photo_object_netsync_state_set(
                                         o,
                                         photo_netsync_edje_signal_get(
                                                                       photo_data->netsync.state));
}

void
netsync_photo_notuptodate_cb(void *data, Enlil_Album *album, Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   photo_data->netsync.state = PHOTO_NETSYNC_NOTUPTODATE;

   Evas_Object
            *o =
                     (Evas_Object *) photos_list_object_item_object_get(
                                                                        photo_data->list_photo_item);
   if (o) photo_object_netsync_state_set(
                                         o,
                                         photo_netsync_edje_signal_get(
                                                                       photo_data->netsync.state));
}

void
netsync_photo_netsyncnotuptodate_cb(void *data, Enlil_Album *album,
                                    Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   photo_data->netsync.state = PHOTO_NETSYNC_NETSYNCNOTUPTODATE;
   Evas_Object
            *o =
                     (Evas_Object *) photos_list_object_item_object_get(
                                                                        photo_data->list_photo_item);
   if (o) photo_object_netsync_state_set(
                                         o,
                                         photo_netsync_edje_signal_get(
                                                                       photo_data->netsync.state));
}

void
netsync_photo_uptodate_cb(void *data, Enlil_Album *album, Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   ASSERT_RETURN_VOID(photo_data != NULL);
   photo_data->netsync.state = PHOTO_NETSYNC_NONE;

   Evas_Object
            *o =
                     (Evas_Object *) photos_list_object_item_object_get(
                                                                        photo_data->list_photo_item);
   if (o) photo_object_netsync_state_set(
                                         o,
                                         photo_netsync_edje_signal_get(
                                                                       photo_data->netsync.state));
}

void
netsync_photo_tags_notuptodate_cb(void *data, Enlil_Album *album,
                                  Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   photo_data->netsync.state_tags = PHOTO_NETSYNC_TAGS_NOTUPTODATE;

   Evas_Object
            *o =
                     (Evas_Object *) photos_list_object_item_object_get(
                                                                        photo_data->list_photo_item);
   if (o) photo_object_netsync_state_set(
                                         o,
                                         photo_netsync_edje_signal_get(
                                                                       photo_data->netsync.state_tags));
}

void
netsync_photo_tags_netsyncnotuptodate_cb(void *data, Enlil_Album *album,
                                         Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   photo_data->netsync.state_tags = PHOTO_NETSYNC_TAGS_NETSYNCNOTUPTODATE;
   Evas_Object
            *o =
                     (Evas_Object *) photos_list_object_item_object_get(
                                                                        photo_data->list_photo_item);
   if (o) photo_object_netsync_state_set(
                                         o,
                                         photo_netsync_edje_signal_get(
                                                                       photo_data->netsync.state_tags));
}

void
netsync_photo_tags_uptodate_cb(void *data, Enlil_Album *album,
                               Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   ASSERT_RETURN_VOID(photo_data != NULL);
   //	photo_data->netsync.state = PHOTO_NETSYNC_NONE;
   //
   //	Evas_Object *o = (Evas_Object *)photos_list_object_item_object_get(photo_data->list_photo_item);
   //	if(o)
   //		photo_object_netsync_state_set(o, photo_netsync_edje_signal_get(photo_data->netsync.state));
}

void
netsync_album_error_cb(void *data, Enlil_Album *album)
{
   printf("ALBUM ERROR \n");
}

void
netsync_photo_error_cb(void *data, Enlil_Photo *photo)
{
   printf("PHOTO ERROR %s\n", enlil_photo_name_get(photo));
}

void
netsync_job_start_cb(void *data, Enlil_NetSync_Job *job, Enlil_Album *album,
                     Enlil_Photo *photo)
{
   if (photo)
   {
      album = enlil_photo_album_get(photo);

      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

      if (photo_data)
      {
         photo_data->netsync.is_sync = EINA_TRUE;
         Evas_Object
                  *o =
                           (Evas_Object *) photos_list_object_item_object_get(
                                                                              photo_data->list_photo_item);
         if (o) photo_object_netsync_state_set(o, "animated");
      }
   }

   if (album)
   {
      Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

      album_data->netsync.is_sync = EINA_TRUE;
      if (album_data && album_data->netsync.icon) edje_object_signal_emit(
                                                                          album_data->netsync.icon,
                                                                          "animated",
                                                                          "");
   }
}

void
netsync_job_add_cb(void *data, Enlil_NetSync_Job *job, Enlil_Album *album,
                   Enlil_Photo *photo)
{
   if (photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (!eina_list_data_find(photo_data->netsync.jobs, job)) photo_data->netsync.jobs
               = eina_list_append(photo_data->netsync.jobs, job);
   }

   if (album)
   {
      Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
      if (!eina_list_data_find(album_data->netsync.jobs, job)) album_data->netsync.jobs
               = eina_list_append(album_data->netsync.jobs, job);
   }
}

void
netsync_login_failed_cb(void *data, const char *username, const char *password)
{
   inwin_login_failed_new();
}

void
netsync_job_error_cb(void *data, Enlil_NetSync_Job *job, Enlil_Album *album,
                     Enlil_Photo *photo, const char *error)
{
   char buf[PATH_MAX];
   const char *prefix = "";

   if (album) prefix = "Synchronization of an album";
   if (photo) prefix = "Synchronization of a photo";
   if (!album && !photo) prefix = "Synchronization";

   snprintf(buf, sizeof(buf), "%s failed with the error :\n %s", prefix, error);

   inwin_netsync_error_new(buf);
}

void
netsync_job_done_cb(void *data, Enlil_NetSync_Job *job, Enlil_Album *album,
                    Enlil_Photo *photo)
{
   if (photo)
   {
      album = enlil_photo_album_get(photo);

      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

      if (photo_data)
      {
         Evas_Object
                  *o =
                           (Evas_Object *) photos_list_object_item_object_get(
                                                                              photo_data->list_photo_item);
         if (o) photo_object_netsync_state_set(
                                               o,
                                               photo_netsync_edje_signal_get(
                                                                             photo_data->netsync.state));

         if (job) photo_data->netsync.jobs
                  = eina_list_remove(photo_data->netsync.jobs, job);
         photo_data->netsync.is_sync = EINA_FALSE;
      }
   }

   if (album)
   {
      Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

      if (album_data && album_data->netsync.icon)
      {
         const char *signal = album_netsync_edje_signal_get(album);
         edje_object_signal_emit(album_data->netsync.icon, signal, "");
      }

      if (album_data && job)
      {
         album_data->netsync.jobs = eina_list_remove(album_data->netsync.jobs,
                                                     job);
      }

      if (album_data) album_data->netsync.is_sync = EINA_FALSE;
   }
}

