
#include "main.h"

static void _done_cb(void *data, Enlil_Photo *photo, int status);
static int _progress_cb(void *data, Enlil_Photo *photo, long int dltotal, long int dlnow);
static void _start_cb(void *data, Enlil_Photo *photo);


Download *download_new(Evas_Object *parent)
{
    Evas_Object *bx, *lbl, *pb, *notify;
    Download *dl;

    dl = calloc(1, sizeof(Download));

    notify = elm_notify_add(parent);
    elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM_RIGHT);
    elm_win_resize_object_add(parent, notify);
    evas_object_size_hint_weight_set(notify, -1.0, -1.0);
    evas_object_size_hint_align_set(notify, -1.0, -1.0);

    bx = elm_box_add(notify);
    evas_object_size_hint_weight_set(bx, 1.0, 1.0);
    evas_object_size_hint_align_set(bx, -1.0, -1.0);
    evas_object_show(bx);
    elm_notify_content_set(notify, bx);

    lbl = elm_label_add(bx);
    elm_label_label_set(lbl, D_("Downloading ..."));
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

    dl->main = notify;
    dl->lbl = lbl;
    dl->pb = pb;

    return dl;
}

void download_free(Download **_dl)
{
    ASSERT_RETURN_VOID(_dl != NULL);
    Download *dl = *_dl;

    ASSERT_RETURN_VOID(dl != NULL);
    evas_object_del(dl->main);
}

void download_add(Download *dl, const char *source, Enlil_Photo *photo)
{
    ASSERT_RETURN_VOID(dl != NULL);
    ASSERT_RETURN_VOID(source != NULL);
    ASSERT_RETURN_VOID(photo != NULL);

    enlil_download_add(photo, source, _start_cb, _progress_cb, _done_cb, dl);
}


static void _start_cb(void *data, Enlil_Photo *photo)
{
    Download *dl = data;

    evas_object_show(dl->main);

    elm_progressbar_label_set(dl->pb, enlil_photo_name_get(photo));
    elm_progressbar_pulse_set(dl->pb, 0);
}

static void _done_cb(void *data, Enlil_Photo *photo, int status)
{
    Download *dl = data;

    evas_object_hide(dl->main);
}

static int _progress_cb(void *data, Enlil_Photo *photo, long int dltotal, long int dlnow)
{
    Download *dl = data;

    elm_progressbar_value_set(dl->pb, (double)dlnow / dltotal);

    return 0;
}


