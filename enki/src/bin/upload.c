#include "main.h"
#include "upload.h"

static int _progress_cb(void *data, Enlil_Photo *photo, long int ultotal, long int ulnow);
static void _done_cb(void *data, Enlil_Photo *photo, int status);
static void _start_cb(void *data, Enlil_Photo *photo);
static void _error_cb(void *data, Enlil_Photo *photo);
static void _album_create_done_cb(void *data, Enlil_Photo *photo, int status);


Upload *upload_new(Evas_Object *parent)
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

void upload_free(Upload **_ul)
{
    ASSERT_RETURN_VOID(_ul != NULL);
    Upload *ul = *_ul;

    ASSERT_RETURN_VOID(ul != NULL);
    evas_object_del(ul->main);
}

void upload_add(Upload *ul, Enlil_Photo *photo)
{
    ASSERT_RETURN_VOID(ul != NULL);
    ASSERT_RETURN_VOID(photo != NULL);

    Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

    if(enlil_photo_netsync_id_get(photo))
    {
        Enlil_Flickr_Job *job = enlil_flickr_job_sync_photo_update_flickr_append(photo, _start_cb, _progress_cb, _done_cb, _error_cb, ul);
        if(!eina_list_data_find(photo_data->netsync.jobs, job))
            photo_data->netsync.jobs = eina_list_append(photo_data->netsync.jobs, job);
    }
    else
    {
       /* Enlil_Flickr_Job *job = enlil_flickr_job_sync_photo_upload_flickr_prepend(photo, _start_cb, _progress_cb, _done_cb, flickr_photo_error_cb, ul);
        if(!eina_list_data_find(photo_data->netsync.jobs, job))
            photo_data->netsync.jobs = eina_list_append(photo_data->netsync.jobs, job);*/
    }
}


/**
 * Create a new album in flickr
 * when we create a new album generally we have to upload a photo first
 * because in Flickr an album can not be empty
 * Consequently we use the uplead interface
 */
void upload_album_create_add(Upload *ul, Enlil_Album *album)
{
    ASSERT_RETURN_VOID(ul != NULL);
    ASSERT_RETURN_VOID(album != NULL);

    Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

    Enlil_Flickr_Job *job = enlil_flickr_job_sync_album_header_create_flickr_append(album,
            _start_cb,
            _progress_cb,
            _album_create_done_cb,
            _error_cb,
            ul);
    if(!eina_list_data_find(album_data->netsync.jobs, job))
        album_data->netsync.jobs = eina_list_append(album_data->netsync.jobs, job);
}




static void _start_cb(void *data, Enlil_Photo *photo)
{
    Upload *ul = data;

    evas_object_show(ul->main);

    elm_progressbar_label_set(ul->pb, enlil_photo_name_get(photo));
    elm_progressbar_pulse_set(ul->pb, 1);
    elm_progressbar_pulse(ul->pb, 1);
}

static void _done_cb(void *data, Enlil_Photo *photo, int status)
{
    Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
    Upload *ul = data;
    Enlil_Album* album = enlil_photo_album_get(photo);
    Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

    evas_object_hide(ul->main);

    photo_data->netsync.state = PHOTO_FLICKR_NONE;

    /*Enlil_Flickr_Job *job = enlil_flickr_job_cmp_photo_append(photo,
            flickr_photo_flickrnotuptodate_cb,
            flickr_photo_notuptodate_cb,
            flickr_photo_uptodate_cb,
            flickr_photo_error_cb,
            enlil_data);
            */
    //if(!eina_list_data_find(photo_data->netsync.jobs, job))
    //    photo_data->netsync.jobs = eina_list_append(photo_data->netsync.jobs, job);

    if(album_data)
    {
        if(album_data->netsync.inwin.win)
            flickr_sync_update(album);
    }
}


static void _album_create_done_cb(void *data, Enlil_Photo *photo, int status)
{
    Enlil_Album *album = enlil_photo_album_get(photo);
    Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
    Upload *ul = data;

    evas_object_hide(ul->main);

    album_data->netsync.album_netsync_notuptodate = EINA_FALSE;
    album_data->netsync.album_local_notuptodate = EINA_FALSE;
    album_data->netsync.album_notinnetsync = EINA_FALSE;

    flickr_sync_update(album);
    photos_list_object_header_update(album_data->list_photo_item);

    /*Enlil_Flickr_Job *job = enlil_flickr_job_sync_album_header_append(album, netsync_album_new_cb,
            netsync_album_notinnetsync_cb, netsync_album_notuptodate_cb,
            netsync_album_netsyncnotuptodate_cb, netsync_album_uptodate_cb,
            flickr_error_cb, enlil_data);
    if(!eina_list_data_find(album_data->netsync.jobs, job))
        album_data->netsync.jobs = eina_list_append(album_data->netsync.jobs, job);
*/
    /*job = enlil_flickr_job_sync_album_photos_append(album,
            netsync_photo_new_cb,
            netsync_photo_notinflickr_cb,
            flickr_photo_known_cb,
            netsync_error_cb,
            enlil_data);
    if(!eina_list_data_find(album_data->netsync.jobs, job))
        album_data->netsync.jobs = eina_list_append(album_data->netsync.jobs, job);*/
}


static int _progress_cb(void *data, Enlil_Photo *photo, long int ultotal, long int ulnow)
{
    //currently thes callback didn't work
    //Upload *ul = data;

    //elm_progressbar_value_set(ul->pb, (double)ulnow / ultotal);

    return 0;
}

static void _error_cb(void *data, Enlil_Photo *photo)
{
    printf("PHOTO ERROR %s\n", enlil_photo_name_get(photo));

    Upload *ul = data;

    evas_object_hide(ul->main);
}

