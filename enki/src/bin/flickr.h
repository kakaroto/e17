#ifndef FLICKR_H_
#define FLICKR_H_


typedef enum Photo_Flickr_Enum
{
   PHOTO_FLICKR_NONE,
   PHOTO_FLICKR_NOTUPTODATE,
   PHOTO_FLICKR_FLICKRNOTUPTODATE,
   PHOTO_FLICKR_NOTINFLICKR
} Photo_Flickr_Enum;


const char *album_flickr_edje_signal_get(Enlil_Album *album);
const char *photo_flickr_edje_signal_get(Photo_Flickr_Enum e);


void flickr_job_start_cb(void *data, Enlil_Flickr_Job* job, Enlil_Album *album, Enlil_Photo *photo);
void flickr_job_done_cb(void *data, Enlil_Flickr_Job* job, Enlil_Album *album, Enlil_Photo *photo);

void flickr_album_new_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_album_flickrnotuptodate_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_album_notuptodate_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_album_notinflickr_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_album_uptodate_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_error_cb(void *data, Enlil_Root *root);

void flickr_photo_new_cb(void *data, Enlil_Album *album, const char *photo_name, const char *photo_id);
void flickr_photo_notinflickr_cb(void *data, Enlil_Album *album, Enlil_Photo *photo);
void flickr_photo_known_cb(void *data, Enlil_Album *album, Enlil_Photo *photo);

void flickr_photo_flickrnotuptodate_cb(void *data, Enlil_Photo *photo);
void flickr_photo_notuptodate_cb(void *data, Enlil_Photo *photo);
void flickr_photo_uptodate_cb(void *data, Enlil_Photo *photo);

void flickr_album_error_cb(void *data, Enlil_Album *album);
void flickr_photo_error_cb(void *data, Enlil_Photo *photo);

void flickr_photos_notinlocal_sizes_get_cb(void *data, Eina_List *sizes, Eina_Bool error);




//inwin flickr sync
Evas_Object *flickr_sync_new(Evas_Object *win, Enlil_Album *album);
void flickr_sync_update(Enlil_Album *album);



#endif /* FLICKR_H_ */
