#include "main.h"
#include "upload.h"

static int
_progress_cb(void *data, Enlil_Photo *photo, long int ultotal, long int ulnow);
static void
_done_cb(void *data, Enlil_Photo *photo, int status);
static void
_start_cb(void *data, Enlil_Photo *photo);
static void
_error_cb(void *data, Enlil_Photo *photo);
static void
_album_create_done_cb(void *data, Enlil_Photo *photo, int status);

Upload *
upload_new(Evas_Object *parent)
{
   Evas_Object *bx, *lbl, *pb, *notify;
   Upload *ul;

   ul = calloc(1, sizeof(Upload));

   notify = elm_notify_add(parent);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM_RIGHT);
   elm_win_resize_object_add(parent, notify);
   evas_object_size_hint_weight_set(notify, -1.0, -1.0);
   evas_object_size_hint_align_set(notify, -1.0, -1.0);

   bx = elm_box_add(parent);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);
   evas_object_show(bx);
   elm_notify_content_set(notify, bx);

   lbl = elm_label_add(bx);
   elm_label_label_set(lbl, D_("Uploading ..."));
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, 1.0, 1.0);
   elm_box_pack_end(bx, lbl);
   evas_object_show(lbl);

   pb = elm_progressbar_add(bx);
   elm_progressbar_label_set(pb, "Photo");
   evas_object_size_hint_weight_set(pb, 1.0, 1.0);
   evas_object_size_hint_align_set(pb, 1.0, 1.0);
   elm_box_pack_end(bx, pb);
   evas_object_show(pb);

   ul->main = notify;
   ul->lbl = lbl;
   ul->pb = pb;

   return ul;
}

void
upload_free(Upload **_ul)
{
   ASSERT_RETURN_VOID(_ul != NULL);
   Upload *ul = *_ul;

   ASSERT_RETURN_VOID(ul != NULL);
   evas_object_del(ul->main);
}

void
upload_start(Upload *ul, Enlil_Photo *photo)
{
   evas_object_show(ul->main);

   elm_progressbar_label_set(ul->pb, enlil_photo_name_get(photo));
   elm_progressbar_pulse_set(ul->pb, 1);
   elm_progressbar_pulse(ul->pb, 1);
}

void
upload_done(Upload *ul, Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Album *album = enlil_photo_album_get(photo);
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   evas_object_hide(ul->main);

   photo_data->netsync.state = PHOTO_NETSYNC_NONE;
}

