#ifndef NETSYNC_H_
#define NETSYNC_H_

typedef enum Photo_NetSync_Enum
{
   PHOTO_NETSYNC_NONE,
   PHOTO_NETSYNC_NOTUPTODATE,
   PHOTO_NETSYNC_NETSYNCNOTUPTODATE,
   PHOTO_NETSYNC_NOTINNETSYNC,
   PHOTO_NETSYNC_TAGS_NOTUPTODATE,
   PHOTO_NETSYNC_TAGS_NETSYNCNOTUPTODATE,
} Photo_NetSync_Enum;

const char *
album_netsync_edje_signal_get(Enlil_Album *album);
const char *
photo_netsync_edje_signal_get(Photo_NetSync_Enum e);

//callbarks
void
netsync_login_failed_cb(void *data, const char *username, const char *password);
void
netsync_job_error_cb(void *data, Enlil_NetSync_Job *job, Enlil_Album *album,
                     Enlil_Photo *photo, const char *error);
void
netsync_job_start_cb(void *data, Enlil_NetSync_Job *job, Enlil_Album *album,
                     Enlil_Photo *photo);
void
netsync_job_done_cb(void *data, Enlil_NetSync_Job *job, Enlil_Album *album,
                    Enlil_Photo *photo);
void
netsync_job_add_cb(void *data, Enlil_NetSync_Job *job, Enlil_Album *album,
                   Enlil_Photo *photo);

void
netsync_album_new_cb(void *data, Enlil_Library *root, int album_id);
void
netsync_album_netsyncnotuptodate_cb(void *data, Enlil_Library *root,
                                    Enlil_Album *album);
void
netsync_album_notuptodate_cb(void *data, Enlil_Library *root,
                             Enlil_Album *album);
void
netsync_album_notinnetsync_cb(void *data, Enlil_Library *root,
                              Enlil_Album *album);
void
netsync_album_uptodate_cb(void *data, Enlil_Library *root, Enlil_Album *album);

void
netsync_photo_known_cb(void *data, Enlil_Album *album, Enlil_Photo *photo);

void
netsync_album_error_cb(void *data, Enlil_Album *album);

void
netsync_photos_notinlocal_sizes_get_cb(void *data, Eina_List *sizes,
                                       Eina_Bool error);

void
netsync_photo_new_cb(void *data, Enlil_Album *album, int id);
void
netsync_photo_notinnetsync_cb(void *data, Enlil_Album *album,
                              Enlil_Photo *photo);
void
         netsync_photo_notuptodate_cb(void *data, Enlil_Album *album,
                                      Enlil_Photo *photo);
void
netsync_photo_netsyncnotuptodate_cb(void *data, Enlil_Album *album,
                                    Enlil_Photo *photo);
void
netsync_photo_uptodate_cb(void *data, Enlil_Album *album, Enlil_Photo *photo);
void
netsync_photo_error_cb(void *data, Enlil_Photo *photo);

void
netsync_photo_tags_notuptodate_cb(void *data, Enlil_Album *album,
                                  Enlil_Photo *photo);
void
netsync_photo_tags_netsyncnotuptodate_cb(void *data, Enlil_Album *album,
                                         Enlil_Photo *photo);
void
netsync_photo_tags_uptodate_cb(void *data, Enlil_Album *album,
                               Enlil_Photo *photo);
//

//inwin netsync sync
Evas_Object *
netsync_album_new(Evas_Object *win, Enlil_Album *album);
void
netsync_sync_update(Enlil_Album *album);
//

#endif /* NETSYNC_H_ */
