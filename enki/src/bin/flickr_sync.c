#include "main.h"

static void
_netsync_notuptodate_cb(void *data, Evas_Object *obj, void *event_info);
static void
_close_cb(void *data, Evas_Object *obj, void *event_info);
static void
_local_notuptodate_cb(void *data, Evas_Object *obj, void *event_info);
static void
_album_notinnetsync_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photos_notinflickr_cb(void *data, Evas_Object *obj, void *event_info);

static void
_photos_notuptodate_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photos_notinlocal_cb(void *data, Evas_Object *obj, void *event_info);
static void
_netsync_photos_notinlocal_photo_new_cb(void *data, Enlil_Album *album, int id);
static void
_netsync_photo_get_new_cb(void *data, Enlil_Album *album, Enlil_Photo *photo,
                          const char *url);
static void
_photos_netsync_notuptodate_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photos_netsync_notinnetsync_cb(void *data, Evas_Object *obj, void *event_info);

static void
         netsync_album_updated_cb(void *data, Enlil_Library *library,
                                  Enlil_Album *album);

Evas_Object *
flickr_sync_new(Evas_Object *win, Enlil_Album *album)
{
   Evas_Object *inwin, *pb, *bt, *tb;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   //create inwin & file selector
   inwin = elm_win_inwin_add(win);
   album_data->netsync.inwin.win = inwin;
   elm_object_style_set(inwin, "minimal");
   evas_object_show(inwin);

   tb = elm_table_add(inwin);
   album_data->netsync.inwin.tb = tb;
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, 1.0, 1.0);
   evas_object_show(tb);
   elm_win_inwin_content_set(inwin, tb);

   bt = elm_button_add(inwin);
   elm_button_label_set(bt, D_("Close"));
   evas_object_size_hint_align_set(bt, 1.0, 0.5);
   evas_object_smart_callback_add(bt, "clicked", _close_cb, album);
   evas_object_show(bt);
   elm_table_pack(tb, bt, 1, 1, 1, 1);

   pb = elm_progressbar_add(inwin);
   album_data->netsync.inwin.pb = pb;
   evas_object_size_hint_align_set(pb, 0.5, 0.5);
   evas_object_size_hint_weight_set(pb, 1.0, 0.0);
   elm_progressbar_pulse_set(pb, EINA_TRUE);
   elm_progressbar_label_set(pb, D_("Synchronising ..."));
   evas_object_hide(pb);
   elm_table_pack(tb, pb, 0, 1, 1, 1);

   flickr_sync_update(album);

   return inwin;
}

void
flickr_sync_update(Enlil_Album *album)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   Evas_Object *fr, *tb2, *lbl, *bt, *sep, *pb, *pager;
   Evas_Object *inwin = album_data->netsync.inwin.win;
   Evas_Object *tb = album_data->netsync.inwin.tb;
   int i = 0;
   Eina_List *l;
   Enlil_Photo *photo;
   int nb_photos;
   char buf[1024];

   if (album_data->netsync.inwin.fr) evas_object_del(
                                                     album_data->netsync.inwin.fr);

   //
   fr = elm_frame_add(inwin);
   album_data->netsync.inwin.fr = fr;
   elm_frame_label_set(fr, D_("Synchronise an album with flickr"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, -1.0, -1.0);
   evas_object_show(fr);
   elm_table_padding_set(tb, 1, 1);
   elm_table_pack(tb, fr, 0, 0, 3, 1);

   tb2 = elm_table_add(inwin);
   evas_object_size_hint_weight_set(tb2, 1.0, 0.0);
   evas_object_size_hint_align_set(tb2, -1.0, 1.0);
   elm_table_padding_set(tb2, 15, 5);
   evas_object_show(tb2);
   elm_frame_content_set(fr, tb2);
   //

   if (album_data->netsync.album_netsync_notuptodate)
   {
      lbl = elm_label_add(inwin);
      elm_label_label_set(lbl, D_("Update the flickr album header."));
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, i, 1, 1);

      bt = elm_button_add(inwin);
      album_data->netsync.inwin.bt1 = bt;
      elm_button_label_set(bt, D_("Update"));
      evas_object_smart_callback_add(bt, "clicked", _netsync_notuptodate_cb,
                                     album);
      evas_object_size_hint_align_set(bt, 0.5, 0.5);
      evas_object_size_hint_weight_set(bt, 1.0, 0.0);

      evas_object_show(bt);
      elm_table_pack(tb2, bt, 1, i, 1, 1);

      i++;

      sep = elm_separator_add(inwin);
      evas_object_show(sep);
      elm_table_pack(tb2, sep, 0, i, 2, 1);
      i++;
   }

   if (album_data->netsync.album_notinnetsync)
   {
      lbl = elm_label_add(inwin);
      elm_label_label_set(lbl, D_("The album is not in Flickr."));
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, i, 1, 1);

      bt = elm_button_add(inwin);
      album_data->netsync.inwin.bt2 = bt;
      elm_button_label_set(bt, D_("Add the album in Flickr"));
      evas_object_size_hint_align_set(bt, 0.5, 0.5);
      evas_object_size_hint_weight_set(bt, 1.0, 0.0);

      evas_object_smart_callback_add(bt, "clicked", _album_notinnetsync_cb,
                                     album);
      evas_object_show(bt);
      elm_table_pack(tb2, bt, 1, i, 1, 1);

      i++;

      sep = elm_separator_add(inwin);
      evas_object_show(sep);
      elm_table_pack(tb2, sep, 0, i, 2, 1);
      i++;
   }

   if (album_data->netsync.album_local_notuptodate)
   {
      lbl = elm_label_add(inwin);
      elm_label_label_set(lbl, D_("Update the local album header."));
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, i, 1, 1);

      bt = elm_button_add(inwin);
      album_data->netsync.inwin.bt3 = bt;
      evas_object_size_hint_align_set(bt, 0.5, 0.5);
      evas_object_size_hint_weight_set(bt, 1.0, 0.0);

      evas_object_smart_callback_add(bt, "clicked", _local_notuptodate_cb,
                                     album);
      elm_button_label_set(bt, D_("Update"));
      evas_object_show(bt);
      elm_table_pack(tb2, bt, 1, i, 1, 1);

      i++;

      sep = elm_separator_add(inwin);
      evas_object_show(sep);
      elm_table_pack(tb2, sep, 0, i, 2, 1);
      i++;
   }

   if (album_data->netsync.photos_notinlocal)
   {
      lbl = elm_label_add(inwin);
      elm_label_label_set(lbl, D_("Some photos are not in the local album."));
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, i, 1, 1);

      pager = elm_pager_add(inwin);
      album_data->netsync.inwin.notinlocal.pager = pager;
      evas_object_size_hint_align_set(pager, 0.5, 0.5);
      evas_object_size_hint_weight_set(pager, 1.0, 0.0);
      evas_object_show(pager);
      elm_table_pack(tb2, pager, 1, i, 1, 1);
      elm_object_style_set(pager, "fade_invisible");

      bt = elm_button_add(inwin);
      album_data->netsync.inwin.notinlocal.bt = bt;
      album_data->netsync.inwin.bt4 = bt;
      evas_object_size_hint_align_set(bt, -1.0, 0.5);
      evas_object_size_hint_weight_set(bt, 1.0, 0.0);
      elm_button_label_set(bt, D_("Download Them All"));
      evas_object_smart_callback_add(bt, "clicked", _photos_notinlocal_cb,
                                     album);
      evas_object_show(bt);
      elm_pager_content_push(pager, bt);

      pb = elm_progressbar_add(inwin);
      album_data->netsync.inwin.notinlocal.pb = pb;
      evas_object_size_hint_align_set(pb, -1.0, 0.5);
      evas_object_size_hint_weight_set(pb, 1.0, 0.0);
      elm_progressbar_pulse_set(pb, EINA_TRUE);
      elm_progressbar_label_set(pb, D_("Downloads in progress ..."));
      evas_object_show(pb);
      elm_pager_content_push(pager, pb);
      evas_object_size_hint_max_set(pb, 0, 0);

      if (album_data->netsync.inwin.notinlocal.is_updating)
      {
         elm_progressbar_pulse(pb, EINA_TRUE);
         elm_pager_content_promote(pager, pb);
      }
      else
         elm_pager_content_promote(pager, bt);

      i++;

      sep = elm_separator_add(inwin);
      evas_object_show(sep);
      elm_table_pack(tb2, sep, 0, i, 2, 1);
      i++;
   }

   nb_photos = 0;
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data && photo_data->netsync.state == PHOTO_FLICKR_NOTINFLICKR)
      {
         nb_photos++;
      }
   }
   if (nb_photos > 0)
   {
      if (nb_photos > 1)
         snprintf(buf, sizeof(buf), D_("%d photos are not on Flickr"),
                  nb_photos);
      else
         snprintf(buf, sizeof(buf), D_("1 photo is not on Flickr"));

      lbl = elm_label_add(inwin);
      elm_label_label_set(lbl, buf);
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, i, 1, 1);

      pager = elm_pager_add(inwin);
      album_data->netsync.inwin.notinflickr.pager = pager;
      evas_object_size_hint_align_set(pager, 0.5, 0.5);
      evas_object_size_hint_weight_set(pager, 1.0, 0.0);
      evas_object_show(pager);
      elm_table_pack(tb2, pager, 1, i, 1, 1);
      elm_object_style_set(pager, "fade_invisible");

      bt = elm_button_add(inwin);
      album_data->netsync.inwin.notinflickr.bt = bt;
      album_data->netsync.inwin.bt5 = bt;
      evas_object_size_hint_align_set(bt, -1.0, 0.5);
      evas_object_size_hint_weight_set(bt, 1.0, 0.0);
      elm_button_label_set(bt, D_("Sending Them All"));
      evas_object_smart_callback_add(bt, "clicked",
                                     _photos_netsync_notinnetsync_cb, album);
      evas_object_show(bt);
      elm_pager_content_push(pager, bt);

      pb = elm_progressbar_add(inwin);
      album_data->netsync.inwin.notinflickr.pb = pb;
      evas_object_size_hint_align_set(pb, -1.0, 0.5);
      evas_object_size_hint_weight_set(pb, 1.0, 0.0);
      elm_progressbar_pulse_set(pb, EINA_TRUE);
      elm_progressbar_label_set(pb, D_("Send in progress ..."));
      evas_object_show(pb);
      elm_pager_content_push(pager, pb);
      evas_object_size_hint_max_set(pb, 0, 0);

      if (album_data->netsync.inwin.notinflickr.is_updating)
      {
         elm_progressbar_pulse(pb, EINA_TRUE);
         elm_pager_content_promote(pager, pb);
      }
      else
         elm_pager_content_promote(pager, bt);

      i++;

      sep = elm_separator_add(inwin);
      evas_object_show(sep);
      elm_table_pack(tb2, sep, 0, i, 2, 1);
      i++;
   }

   nb_photos = 0;
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data && photo_data->netsync.state == PHOTO_FLICKR_NOTUPTODATE)
      nb_photos++;
   }
   if (nb_photos > 0)
   {
      if (nb_photos > 1)
         snprintf(buf, sizeof(buf), D_("%d local photos need to be updated"),
                  nb_photos);
      else
         snprintf(buf, sizeof(buf), D_("1 local photo needs to be updated"));

      lbl = elm_label_add(inwin);
      elm_label_label_set(lbl, buf);
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, i, 1, 1);

      bt = elm_button_add(inwin);
      evas_object_size_hint_align_set(bt, 0.5, 0.5);
      evas_object_size_hint_weight_set(bt, 1.0, 0.0);

      album_data->netsync.inwin.bt5 = bt;
      elm_button_label_set(bt, D_("Update Them All"));
      evas_object_smart_callback_add(bt, "clicked", _photos_notuptodate_cb,
                                     album);
      evas_object_show(bt);
      elm_table_pack(tb2, bt, 1, i, 1, 1);

      i++;

      sep = elm_separator_add(inwin);
      evas_object_show(sep);
      elm_table_pack(tb2, sep, 0, i, 2, 1);
      i++;
   }

   nb_photos = 0;
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data && photo_data->netsync.state == PHOTO_FLICKR_FLICKRNOTUPTODATE)
      nb_photos++;
   }
   if (nb_photos > 0)
   {
      if (nb_photos > 1)
         snprintf(buf, sizeof(buf),
                  D_("%d flickr's photos need to be updated"), nb_photos);
      else
         snprintf(buf, sizeof(buf), D_("1 flickr's photo needs to be updated"));

      lbl = elm_label_add(inwin);
      elm_label_label_set(lbl, buf);
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, i, 1, 1);

      pager = elm_pager_add(inwin);
      album_data->netsync.inwin.flickrupdate.pager = pager;
      evas_object_size_hint_align_set(pager, 0.5, 0.5);
      evas_object_size_hint_weight_set(pager, 1.0, 0.0);
      evas_object_show(pager);
      elm_table_pack(tb2, pager, 1, i, 1, 1);
      elm_object_style_set(pager, "fade_invisible");

      bt = elm_button_add(inwin);
      album_data->netsync.inwin.flickrupdate.bt = bt;
      album_data->netsync.inwin.bt5 = bt;
      evas_object_size_hint_align_set(bt, -1.0, 0.5);
      evas_object_size_hint_weight_set(bt, 1.0, 0.0);
      elm_button_label_set(bt, D_("Update Them All"));
      evas_object_smart_callback_add(bt, "clicked",
                                     _photos_netsync_notuptodate_cb, album);
      evas_object_show(bt);
      elm_pager_content_push(pager, bt);

      pb = elm_progressbar_add(inwin);
      album_data->netsync.inwin.flickrupdate.pb = pb;
      evas_object_size_hint_align_set(pb, -1.0, 0.5);
      evas_object_size_hint_weight_set(pb, 1.0, 0.0);
      elm_progressbar_pulse_set(pb, EINA_TRUE);
      elm_progressbar_label_set(pb, D_("Updating in progress ..."));
      evas_object_show(pb);
      elm_pager_content_push(pager, pb);
      evas_object_size_hint_max_set(pb, 0, 0);

      if (album_data->netsync.inwin.flickrupdate.is_updating)
      {
         elm_progressbar_pulse(pb, EINA_TRUE);
         elm_pager_content_promote(pager, pb);
      }
      else
         elm_pager_content_promote(pager, bt);

      i++;

      sep = elm_separator_add(inwin);
      evas_object_show(sep);
      elm_table_pack(tb2, sep, 0, i, 2, 1);
      i++;
   }

   if (i != 0)
   {
      /*bt = elm_button_add(inwin);
       album_data->flickr_sync.inwin.bt6 = bt;
       elm_button_label_set(bt, D_("Update All"));
       evas_object_show(bt);
       elm_table_pack(tb2, bt, 1, i, 1, 1);
       */
   }
   else
   {
      lbl = elm_label_add(inwin);
      elm_label_label_set(lbl, D_("Everything is up to date!"));
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, 0, 1, 1);
   }
}

static void
_sync_start(Enlil_Album_Data *album_data)
{
   evas_object_show(album_data->netsync.inwin.pb);
   elm_progressbar_pulse(album_data->netsync.inwin.pb, EINA_TRUE);

   if (album_data->netsync.inwin.bt1) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt1,
                                                              EINA_TRUE);
   if (album_data->netsync.inwin.bt2) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt2,
                                                              EINA_TRUE);
   if (album_data->netsync.inwin.bt3) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt3,
                                                              EINA_TRUE);
   if (album_data->netsync.inwin.bt4) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt4,
                                                              EINA_TRUE);
   if (album_data->netsync.inwin.bt5) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt5,
                                                              EINA_TRUE);
   if (album_data->netsync.inwin.bt6) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt6,
                                                              EINA_TRUE);
}

static void
_sync_stop(Enlil_Album_Data *album_data)
{
   evas_object_hide(album_data->netsync.inwin.pb);
   elm_progressbar_pulse(album_data->netsync.inwin.pb, EINA_FALSE);

   if (album_data->netsync.inwin.bt1) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt1,
                                                              EINA_FALSE);
   if (album_data->netsync.inwin.bt2) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt2,
                                                              EINA_FALSE);
   if (album_data->netsync.inwin.bt3) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt3,
                                                              EINA_FALSE);
   if (album_data->netsync.inwin.bt4) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt4,
                                                              EINA_FALSE);
   if (album_data->netsync.inwin.bt5) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt5,
                                                              EINA_FALSE);
   if (album_data->netsync.inwin.bt6) elm_object_disabled_set(
                                                              album_data->netsync.inwin.bt6,
                                                              EINA_FALSE);
}

static void
_close_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   evas_object_del(album_data->netsync.inwin.win);
   album_data->netsync.inwin.win = NULL;
   album_data->netsync.inwin.fr = NULL;
}

static void
_netsync_notuptodate_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   Enlil_NetSync_Job
            *job =
                     enlil_netsync_job_update_netsync_album_header_append(
                                                                          enlil_album_library_get(
                                                                                                  album),
                                                                          album,
                                                                          netsync_album_updated_cb,
                                                                          NULL);
   if (!eina_list_data_find(album_data->netsync.jobs, job)) album_data->netsync.jobs
            = eina_list_append(album_data->netsync.jobs, job);
   _sync_start(album_data);
}

static void
_local_notuptodate_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   Enlil_NetSync_Job
            *job =
                     enlil_netsync_job_update_local_album_header_append(
                                                                        enlil_album_library_get(
                                                                                                album),
                                                                        album,
                                                                        netsync_album_updated_cb,
                                                                        NULL);
   if (!eina_list_data_find(album_data->netsync.jobs, job)) album_data->netsync.jobs
            = eina_list_append(album_data->netsync.jobs, job);
   _sync_start(album_data);
}

//
//Downloads photos which are not in the local library
//
static void
_photos_notinlocal_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   //Retrieve the list of photos which are not in the local library
   Enlil_NetSync_Job
            *job =
                     enlil_netsync_job_sync_photos_append(
                                                          album,
                                                          _netsync_photos_notinlocal_photo_new_cb,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          netsync_photo_error_cb,
                                                          album);
   if (!eina_list_data_find(album_data->netsync.jobs, job)) album_data->netsync.jobs
            = eina_list_append(album_data->netsync.jobs, job);

   album_data->netsync.inwin.notinlocal.is_updating = EINA_TRUE;
   elm_pager_content_promote(album_data->netsync.inwin.notinlocal.pager,
                             album_data->netsync.inwin.notinlocal.pb);
   elm_progressbar_pulse(album_data->netsync.inwin.notinlocal.pb, EINA_TRUE);
}

//This method is called for each new photos
static void
_netsync_photos_notinlocal_photo_new_cb(void *data, Enlil_Album *album, int id)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   album_data->netsync.nb_photos_dl++;
   Enlil_NetSync_Job
            *job =
                     enlil_netsync_job_get_new_photo_header_append(
                                                                   album,
                                                                   id,
                                                                   _netsync_photo_get_new_cb,
                                                                   album);
   if (!eina_list_data_find(album_data->netsync.jobs, job)) album_data->netsync.jobs
            = eina_list_append(album_data->netsync.jobs, job);
}

static void
_netsync_photo_get_new_cb(void *data, Enlil_Album *album, Enlil_Photo *photo,
                          const char *url)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   download_add(enlil_data->dl, url, photo);
}

//create the album
static void
_album_notinnetsync_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   Enlil_NetSync_Job *job =
            enlil_netsync_job_add_album_append(enlil_album_library_get(album),
                                               album, netsync_album_updated_cb,
                                               NULL);

   if (!eina_list_data_find(album_data->netsync.jobs, job)) album_data->netsync.jobs
            = eina_list_append(album_data->netsync.jobs, job);

   _sync_start(album_data);
}

static void
netsync_album_updated_cb(void *data, Enlil_Library *library, Enlil_Album *album)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   album_data->netsync.album_netsync_notuptodate = EINA_FALSE;
   album_data->netsync.album_local_notuptodate = EINA_FALSE;
   album_data->netsync.album_notinnetsync = EINA_FALSE;

   flickr_sync_update(album);
   photos_list_object_header_update(album_data->list_photo_item);
   elm_genlist_item_update(album_data->list_album_item);

   _sync_stop(album_data);
}

//send all photos
static void
_photos_notinflickr_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Photo *photo;
   Eina_List *l;

   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data && photo_data->netsync.state == PHOTO_FLICKR_NOTINFLICKR)
      {
         /* upload_add(enlil_data->ul, photo); */
      }
   }

   album_data->netsync.inwin.notinflickr.is_updating = EINA_TRUE;
   elm_pager_content_promote(album_data->netsync.inwin.notinflickr.pager,
                             album_data->netsync.inwin.notinflickr.pb);
   elm_progressbar_pulse(album_data->netsync.inwin.notinflickr.pb, EINA_TRUE);
}

static void
netsync_photos_notuptodate_cb(void *data, Enlil_Album *album,
                              Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   photo_data->netsync.state = PHOTO_FLICKR_NONE;

   flickr_sync_update(album);
   photos_list_object_item_update(photo_data->list_photo_item);
   photos_list_object_header_update(album_data->list_photo_item);

   _sync_stop(album_data);
}

static void
_photos_notuptodate_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Photo *photo;
   Eina_List *l;

EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   if (photo_data && photo_data->netsync.state == PHOTO_FLICKR_NOTUPTODATE)
   {
      Enlil_NetSync_Job *job = enlil_netsync_job_update_local_photo_header_append(album,
               photo,
               netsync_photos_notuptodate_cb,
               photo);
   }
}
}

static void
netsync_photos_netsync_notuptodate_cb(void *data, Enlil_Album *album,
                                      Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   photo_data->netsync.state = PHOTO_FLICKR_NONE;

   flickr_sync_update(album);
   photos_list_object_item_update(photo_data->list_photo_item);
   photos_list_object_header_update(album_data->list_photo_item);

   _sync_stop(album_data);
}

static void
_photos_netsync_notuptodate_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Photo *photo;
   Eina_List *l;

EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   if (photo_data && photo_data->netsync.state == PHOTO_FLICKR_FLICKRNOTUPTODATE)
   {
      Enlil_NetSync_Job *job = enlil_netsync_job_update_netsync_photo_header_append(album,
               photo,
               netsync_photos_netsync_notuptodate_cb,
               photo);
   }
}
}

static void
_photos_netsync_notinnetsync_done_cb(void *data, Enlil_Album *album,
                                     Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   photo_data->netsync.state = PHOTO_FLICKR_NONE;

   flickr_sync_update(album);
   photos_list_object_item_update(photo_data->list_photo_item);
   photos_list_object_header_update(album_data->list_photo_item);

   _sync_stop(album_data);
}

static void
_photos_netsync_notinnetsync_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   Enlil_Photo *photo;
   Eina_List *l;

   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data && photo_data->netsync.state == PHOTO_FLICKR_NOTINFLICKR)
      {
         enlil_netsync_job_add_photo_append(photo, _photos_netsync_notinnetsync_done_cb, NULL);
      }
   }

   album_data->netsync.inwin.notinflickr.is_updating = EINA_TRUE;
   elm_pager_content_promote(album_data->netsync.inwin.notinflickr.pager,
                             album_data->netsync.inwin.notinflickr.pb);
   elm_progressbar_pulse(album_data->netsync.inwin.notinflickr.pb, EINA_TRUE);
}

