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
_photos_notinnetsync_cb(void *data, Evas_Object *obj, void *event_info);

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
netsync_album_new(Evas_Object *win, Enlil_Album *album)
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

   netsync_sync_update(album);

   return inwin;
}

static int
count_photos(Enlil_Album *album, Photo_NetSync_Enum type)
{
   Eina_List *l;
   Enlil_Photo *photo;
   int nb_photos = 0;
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data && photo_data->netsync.state == type)
      {
         nb_photos++;
      }
   }
   return nb_photos;
}

static int
count_photos_tags(Enlil_Album *album, Photo_NetSync_Enum type)
{
   Eina_List *l;
   Enlil_Photo *photo;
   int nb_photos = 0;
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data && photo_data->netsync.state_tags == type)
      {
         nb_photos++;
      }
   }
   return nb_photos;
}

#define MSG(BUF, MORE, ONE)                           \
  if(nb_photos > 1)                                   \
    snprintf(BUF, sizeof(BUF), D_(MORE), nb_photos);  \
  else                                                \
    snprintf(BUF, sizeof(BUF), D_(ONE));

void
netsync_sync_update(Enlil_Album *album)
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
   elm_frame_label_set(fr, D_("Synchronise an album with the distant library"));
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
      elm_label_label_set(lbl, D_("Update"));
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
      elm_label_label_set(lbl, D_("The album is not in distant library."));
      evas_object_show(lbl);
      elm_table_pack(tb2, lbl, 0, i, 1, 1);

      bt = elm_button_add(inwin);
      album_data->netsync.inwin.bt2 = bt;
      elm_button_label_set(bt, D_("Add the album in the distant library"));
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
      elm_button_label_set(bt, D_("Download"));
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

#define LBL(LABEL)                 \
  lbl = elm_label_add(inwin);      \
  elm_label_label_set(lbl, LABEL); \
  evas_object_show(lbl);           \
  elm_table_pack(tb2, lbl, 0, i, 1, 1);

#define PAGER(TARGET)                                \
  pager = elm_pager_add(inwin);                      \
  TARGET = pager;                                    \
  evas_object_size_hint_align_set(pager, 0.5, 0.5);  \
  evas_object_size_hint_weight_set(pager, 1.0, 0.0); \
  evas_object_show(pager);                           \
  elm_table_pack(tb2, pager, 1, i, 1, 1);            \
  elm_object_style_set(pager, "fade_invisible");

#define BUTTON_PAGER(TARGET, LABEL, CB)                     \
  bt = elm_button_add(inwin);                               \
  TARGET = bt;                                              \
  evas_object_size_hint_align_set(bt, -1.0, 0.5);           \
  evas_object_size_hint_weight_set(bt, 1.0, 0.0);           \
  elm_button_label_set(bt, D_(LABEL));                      \
  evas_object_smart_callback_add(bt, "clicked", CB, album); \
  evas_object_show(bt);                                     \
  elm_pager_content_push(pager, bt);                        \

#define BUTTON(TARGET, LABEL, CB)                           \
  bt = elm_button_add(inwin);                               \
  evas_object_size_hint_align_set(bt, 0.5, 0.5);            \
  evas_object_size_hint_weight_set(bt, 1.0, 0.0);           \
  TARGET = bt;                                              \
  elm_button_label_set(bt, D_(LABEL));                      \
  evas_object_smart_callback_add(bt, "clicked", CB, album); \
  evas_object_show(bt);                                     \
  elm_table_pack(tb2, bt, 1, i, 1, 1);                      \

#define PROGRESSBAR(TARGET, LABEL, STATUS)                   \
  pb = elm_progressbar_add(inwin);                           \
  album_data->netsync.inwin.notinnetsync.pb = pb;            \
  evas_object_size_hint_align_set(pb, -1.0, 0.5);            \
  evas_object_size_hint_weight_set(pb, 1.0, 0.0);            \
  elm_progressbar_pulse_set(pb, EINA_TRUE);                  \
  elm_progressbar_label_set(pb, D_("Send in progress ...")); \
  evas_object_show(pb);                                      \
  elm_pager_content_push(pager, pb);                         \
  evas_object_size_hint_max_set(pb, 0, 0);                   \
                                                             \
  if(album_data->netsync.inwin.notinnetsync.is_updating)     \
    {                                                        \
       elm_progressbar_pulse(pb, EINA_TRUE);                 \
       elm_pager_content_promote(pager, pb);                 \
    }                                                        \
  else                                                       \
    elm_pager_content_promote(pager, bt);

#define SEPARATOR()                     \
  sep = elm_separator_add(inwin);       \
  evas_object_show(sep);                \
  elm_table_pack(tb2, sep, 0, i, 2, 1); \
  i++;

   nb_photos = count_photos(album, PHOTO_NETSYNC_NOTINNETSYNC);
   if (nb_photos > 0)
   {
      MSG(buf, "%d photos are not in the distant album", "1 photo is not is the distant album");
      LBL(buf);
      PAGER(album_data->netsync.inwin.notinnetsync.pager);
      BUTTON_PAGER(album_data->netsync.inwin.notinnetsync.bt, "Upload", _photos_netsync_notinnetsync_cb);
      PROGRESSBAR(album_data->netsync.inwin.notinnetsync.pb, "Send in progress ...", album_data->netsync.inwin.notinnetsync.is_updating);
      i++;
      SEPARATOR();
   }

   nb_photos = count_photos(album, PHOTO_NETSYNC_NOTUPTODATE);
   nb_photos += count_photos_tags(album, PHOTO_NETSYNC_TAGS_NOTUPTODATE);
   if (nb_photos > 0)
   {
      MSG(buf, "%d local photos need to be updated", "1 local photo needs to be updated");
      LBL(buf);
      BUTTON(album_data->netsync.inwin.bt5, "Update", _photos_notuptodate_cb);
      i++;
      SEPARATOR();
   }

   nb_photos = count_photos(album, PHOTO_NETSYNC_NETSYNCNOTUPTODATE);
   nb_photos += count_photos_tags(album, PHOTO_NETSYNC_TAGS_NETSYNCNOTUPTODATE);
   if (nb_photos > 0)
   {
      MSG(buf, "%d photos of the distant album need to be updated", "1 photo of the distant album needs to be updated");
      LBL(buf);
      PAGER(album_data->netsync.inwin.netsyncupdate.pager);
      BUTTON_PAGER(album_data->netsync.inwin.netsyncupdate.bt, "Update", _photos_netsync_notuptodate_cb);
      PROGRESSBAR(album_data->netsync.inwin.netsyncupdate.pb, "Updating in progress ...", album_data->netsync.inwin.netsyncupdate.is_updating);
      i++;
      SEPARATOR();
   }

   if (i != 0)
   {
      /*bt = elm_button_add(inwin);
       album_data->netsync.inwin.bt6 = bt;
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
                                                          NULL, NULL, NULL,
                                                          NULL, NULL, NULL,
                                                          NULL, album);

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

   _sync_start(album_data);
}

static void
netsync_album_updated_cb(void *data, Enlil_Library *library, Enlil_Album *album)
{
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   album_data->netsync.album_netsync_notuptodate = EINA_FALSE;
   album_data->netsync.album_local_notuptodate = EINA_FALSE;
   album_data->netsync.album_notinnetsync = EINA_FALSE;

   netsync_sync_update(album);
   photos_list_object_header_update(album_data->list_photo_item);
   elm_genlist_item_update(album_data->list_album_item);

   _sync_stop(album_data);
}

//send all photos
static void
_photos_notinnetsync_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Photo *photo;
   Eina_List *l;

   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data && photo_data->netsync.state == PHOTO_NETSYNC_NOTINNETSYNC)
      {
         upload_add(enlil_data->ul, photo);
      }
   }

   album_data->netsync.inwin.notinnetsync.is_updating = EINA_TRUE;
   elm_pager_content_promote(album_data->netsync.inwin.notinnetsync.pager,
                             album_data->netsync.inwin.notinnetsync.pb);
   elm_progressbar_pulse(album_data->netsync.inwin.notinnetsync.pb, EINA_TRUE);
}

static void
netsync_photos_notuptodate_cb(void *data, Enlil_Album *album,
                              Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   photo_data->netsync.state = PHOTO_NETSYNC_NONE;
   photo_data->netsync.state_tags = PHOTO_NETSYNC_NONE;

   netsync_sync_update(album);
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
   if (photo_data && photo_data->netsync.state == PHOTO_NETSYNC_NOTUPTODATE
            || photo_data && photo_data->netsync.state_tags == PHOTO_NETSYNC_TAGS_NOTUPTODATE)
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

   photo_data->netsync.state = PHOTO_NETSYNC_NONE;
   photo_data->netsync.state_tags = PHOTO_NETSYNC_NONE;

   netsync_sync_update(album);
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
   if (photo_data && photo_data->netsync.state == PHOTO_NETSYNC_NETSYNCNOTUPTODATE
            || photo_data && photo_data->netsync.state_tags == PHOTO_NETSYNC_TAGS_NETSYNCNOTUPTODATE)
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

   photo_data->netsync.state = PHOTO_NETSYNC_NONE;

   netsync_sync_update(album);
   photos_list_object_item_update(photo_data->list_photo_item);
   photos_list_object_header_update(album_data->list_photo_item);

   _sync_stop(album_data);
}

static void
_photo_upload_start_cb(void *data, Enlil_Photo *photo)
{
   upload_start(enlil_data->ul, photo);
}

static void
_photo_upload_done_cb(void *data, Enlil_Photo *photo)
{
   upload_done(enlil_data->ul, photo);
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
      if (photo_data && photo_data->netsync.state == PHOTO_NETSYNC_NOTINNETSYNC)
      {
         enlil_netsync_job_add_photo_append(photo, _photos_netsync_notinnetsync_done_cb,
                  _photo_upload_start_cb,
                  _photo_upload_done_cb,
                  NULL);
      }
   }

   album_data->netsync.inwin.notinnetsync.is_updating = EINA_TRUE;
   elm_pager_content_promote(album_data->netsync.inwin.notinnetsync.pager,
                             album_data->netsync.inwin.notinnetsync.pb);
   elm_progressbar_pulse(album_data->netsync.inwin.notinnetsync.pb, EINA_TRUE);
}

