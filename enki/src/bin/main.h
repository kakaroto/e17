#ifndef MAIN_H
#define MAIN_H


#include <Elementary.h>
#include <string.h>

#include <Enlil.h>

#include "../../config.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "../define.h"

#define D_(s) gettext(s)

#define THEME_BIG PACKAGE_DATA_DIR"/themes/default.edj"
#define THEME_SMALL PACKAGE_DATA_DIR"/themes/default_small.edj"

extern int APP_LOG_DOMAIN;
#define LOG_DOMAIN APP_LOG_DOMAIN

extern const char *media_player;

extern const char* Theme;


#include "enlil_data.h"
#include "flickr.h"
#include "objects.h"


extern Enlil_Photo *current_photo;
extern Enlil_Data *enlil_data;
extern Evas_Object *global_object;


void library_set(const char *root_path);
void close_cb(void *data, Evas_Object *obj, void *event_info);

void select_list_photo();
void select_menu();

/**cb called when a enlil object is destroyed */
void enlil_album_data_free(Enlil_Album *album, void *_data);
void enlil_photo_data_free(Enlil_Photo *photo, void *_data);
void enlil_collection_data_free(Enlil_Collection *col, void *_data);
void enlil_tag_data_free(Enlil_Tag *tag, void *_data);;
void enlil_geocaching_data_free(Enlil_Geocaching *gp, void *_data);

/* notifications windows */
void notify_sync_content_set(Enlil_Data *enlil_data, const char *msg, Eina_Bool loading);
void notify_load_content_set(Enlil_Data *enlil_data, const char *msg, Eina_Bool loading);


/* Photo manager callbacks */
void sync_done_cb(void *data, Enlil_Sync *sync);
void sync_start_cb(void *data, Enlil_Sync *sync);
void sync_error_cb(void *data, Enlil_Sync *sync,  Sync_Error error, const char* msg);
void sync_album_new_cb(void *data, Enlil_Sync *sync, Enlil_Library*root, Enlil_Album *album);
void sync_album_update_cb(void *data, Enlil_Sync *sync, Enlil_Library*root, Enlil_Album *album);
void sync_album_disappear_cb(void *data, Enlil_Sync *sync, Enlil_Library*root, Enlil_Album *album);
void sync_photo_new_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);
void sync_photo_update_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);
void sync_photo_disappear_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);

void load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos);
void load_error_cb(void *data, Enlil_Load *load,  Load_Error error, const char* msg);
void load_album_done_cb(void *data, Enlil_Load *load, Enlil_Library*root, Enlil_Album *album);

void monitor_album_new_cb(void *data, Enlil_Library*root, const char *path);
void monitor_album_delete_cb(void *data, Enlil_Library*root, const char *path);
void monitor_album_update_cb(void *data, Enlil_Library *root, Enlil_Album *album);
void monitor_enlil_delete_cb(void *data, Enlil_Library*root);
void monitor_photo_new_cb(void *data, Enlil_Library*root, Enlil_Album *album, const char *path);
void monitor_photo_delete_cb(void *data, Enlil_Library*root, Enlil_Album *album, const char *path);
void monitor_photo_update_cb(void *data, Enlil_Library*root, Enlil_Album *album, const char *path);

void thumb_done_cb(void *data, Enlil_Photo *photo, const char *file);
void thumb_error_cb(void *data, Enlil_Photo *photo);

void collection_new_cb(void *data, Enlil_Library *root, Enlil_Collection *col);
void collection_delete_cb(void *data, Enlil_Library *root, Enlil_Collection *col);
void collection_album_new_cb(void *data, Enlil_Library *root, Enlil_Collection *col, Enlil_Album *album);
void collection_album_delete_cb(void *data, Enlil_Library *root, Enlil_Collection *col, Enlil_Album *album);

void tag_new_cb(void *data, Enlil_Library *root, Enlil_Tag *tag);
void tag_delete_cb(void *data, Enlil_Library *root, Enlil_Tag *tag);
void tag_photo_new_cb(void *data, Enlil_Library *root, Enlil_Tag *tag, Enlil_Photo *photo);
void tag_photo_delete_cb(void *data, Enlil_Library *root, Enlil_Tag *tag, Enlil_Photo *photo);

void exif_load_done(void *data, Enlil_Exif_Job *job, Eina_List *exifs);
void iptc_load_done(void *data, Enlil_IPTC_Job *job, Eina_List *iptcs);

void geocaching_done_cb(void *data, Eina_Hash *db);
void geocaching_remove_marker_cb(void *data, Eina_Hash *db);




#endif
