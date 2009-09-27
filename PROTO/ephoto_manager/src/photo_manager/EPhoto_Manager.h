// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#ifndef  PHOTO_MANAGER_INC
#define  PHOTO_MANAGER_INC

#include "../define.h"

#define EET_FILE "data.eet"

#include <Ecore.h>
#include <Ecore_File.h>
#include <Eet.h>
#include <string.h>
#include <Ethumb_Client.h>

/** TODO
 * Save the list of root in ~/.ephoto_manager
 * Browse albums recursively
 * Create an album for photos which are in the root directory
 * Rename the eet file
 *
 * EXIF loader
 * Supports of tags
 *
 * transformations method (rotate, B&W ...)
 *
 * thumbs : Do not add a job if a equival job is already in the list
 * idem for sync
 */

typedef struct pm_root PM_Root;
typedef struct pm_album PM_Album;
typedef struct pm_photo PM_Photo;
typedef struct pm_sync Photo_Manager_Sync;
/* CB used when a change occurs in an root folder or an album */
typedef struct Photo_Manager_Configuration Photo_Manager_Configuration;
typedef void (*Photo_Manager_Album_New_Cb)     (void *data, PM_Root *root, const char *path);
typedef void (*Photo_Manager_Album_Delete_Cb)  (void *data, PM_Root *root, const char *path);
typedef void (*Photo_Manager_Delete_Cb)        (void *data, PM_Root *root);
typedef void (*Photo_Manager_Photo_New_Cb)     (void *data, PM_Root *root, PM_Album *album, const char *path);
typedef void (*Photo_Manager_Photo_Delete_Cb)  (void *data, PM_Root *root, PM_Album *album, const char *path);
typedef void (*Photo_Manager_Photo_Update_Cb)  (void *data, PM_Root *root, PM_Album *album, const char *path);
//

EAPI    int             photo_manager_init();
EAPI    int             photo_manager_shutdown();

EAPI    PM_Root *	pm_root_new(
                Photo_Manager_Album_New_Cb album_new_cb, Photo_Manager_Album_Delete_Cb album_delete_cb,
                Photo_Manager_Delete_Cb pm_delete_cb,
                Photo_Manager_Photo_New_Cb photo_new_cb, Photo_Manager_Photo_Delete_Cb photo_delete_cb,
                Photo_Manager_Photo_Update_Cb photo_update_cb,
                void *user_data);
EAPI    void            pm_root_free(PM_Root **root);
EAPI    void            pm_root_path_set(PM_Root *root, const char *path);
EAPI    const char*     pm_root_path_get(const PM_Root *root);
EAPI    void            pm_root_album_add(PM_Root *root, PM_Album *album);
EAPI    Eina_List *     pm_root_albums_get(const PM_Root *root);
EAPI    void            pm_root_sync_set(PM_Root *root, Photo_Manager_Sync *sync);
EAPI    Photo_Manager_Sync *pm_root_sync_get(const PM_Root *root);
EAPI    void            pm_root_print(PM_Root *root);
EAPI    PM_Album *      pm_root_album_search_file_name(PM_Root *root, const char *file_name);
EAPI    void            pm_root_album_remove(PM_Root *root, PM_Album *album);
EAPI    void            pm_root_monitor_start(PM_Root *root);
EAPI    void            pm_root_monitor_stop(PM_Root *root);

EAPI    PM_Root *	pm_root_eet_albums_load(PM_Root *root);
EAPI    int             pm_root_eet_albums_save(PM_Root *root);
EAPI    PM_Album *      pm_root_eet_album_load(PM_Root *root, const char* key);
EAPI    int             pm_root_eet_album_remove(PM_Root *root, const char* key);

EAPI    PM_Album *      pm_album_new();
EAPI    PM_Album *      pm_album_copy_new(const PM_Album *album);
EAPI    void            pm_album_copy(const PM_Album *album_src, PM_Album *album_dest);
EAPI    void            pm_album_free(PM_Album **album);
EAPI    void            pm_album_name_set(PM_Album *album, const char* name);
EAPI    void            pm_album_file_name_set(PM_Album *album, const char* file_name);
EAPI    void            pm_album_path_set(PM_Album *album, const char* path);
EAPI    void            pm_album_time_set(PM_Album *album, long long time);
EAPI    void            pm_album_photos_set(PM_Album *album, Eina_List *photos);
EAPI    void            pm_album_root_set(PM_Album *album, PM_Root *root);
EAPI    PM_Root *	pm_album_root_get(const PM_Album *album);
EAPI    const char*     pm_album_name_get(const PM_Album *album);
EAPI    const char*     pm_album_file_name_get(const PM_Album *album);
EAPI    const char*     pm_album_path_get(const PM_Album *album);
EAPI    long long       pm_album_time_get(const PM_Album *album);
EAPI    void            pm_album_list_print(Eina_List *root);
EAPI    void            pm_album_print(PM_Album *album);
EAPI    void            pm_album_photo_add(PM_Album *album, PM_Photo *photo);
EAPI    void            pm_album_photo_remove(PM_Album *album, PM_Photo *photo);
EAPI    Eina_List *     pm_album_photos_get(PM_Album *album);
EAPI    int             pm_album_photos_count_get(PM_Album *album);
EAPI    PM_Photo *      pm_album_photo_search_file_name(PM_Album *album, const char *file_name);
EAPI    void            pm_album_monitor_start(PM_Album *album);
EAPI    void            pm_album_monitor_stop(PM_Album *album);
EAPI    void            pm_album_user_data_set(PM_Album *album, void *user_data);
EAPI    void *          pm_album_user_data_get(const PM_Album *album);

EAPI    int             pm_album_eet_global_header_save(PM_Album *album);
EAPI    int             pm_album_eet_header_save(PM_Album *album);
EAPI	int		pm_album_eet_photos_list_save(PM_Album *album);


EAPI    PM_Photo *      pm_photo_new();
EAPI    void            pm_photo_free(PM_Photo **photo);
EAPI    void            pm_photo_album_set(PM_Photo *photo, PM_Album *album);
EAPI    void            pm_photo_name_set(PM_Photo *photo, const char* name);
EAPI    void            pm_photo_path_set(PM_Photo *photo, const char* path);
EAPI    void            pm_photo_file_name_set(PM_Photo *photo, const char* file_name);
EAPI    PM_Album *      pm_photo_album_get(const PM_Photo *photo);
EAPI    void            pm_photo_time_set(PM_Photo *photo, long long time);
EAPI    const char*     pm_photo_name_get(const PM_Photo *photo);
EAPI    const char*     pm_photo_path_get(const PM_Photo *photo);
EAPI    const char*     pm_photo_file_name_get(const PM_Photo *photo);
EAPI    long long       pm_photo_time_get(const PM_Photo *photo);
EAPI    void            pm_photo_list_print(Eina_List *l_photos);
EAPI    void            pm_photo_print(PM_Photo *photo);
EAPI    int             pm_photo_is(const char *file);
EAPI    PM_Photo *      pm_photo_copy_new(PM_Photo *photo);
EAPI    void            pm_photo_copy(PM_Photo *photo_src, PM_Photo *photo_dest);
EAPI    void            pm_photo_user_data_set(PM_Photo *photo, void *user_data);
EAPI    void *          pm_photo_user_data_get(const PM_Photo *photo);
EAPI    void            pm_photo_thumb_fdo_large_set(PM_Photo *photo,const char *thumb);
EAPI    const char*     pm_photo_thumb_fdo_large_get(const PM_Photo *photo);
EAPI    void            pm_photo_thumb_fdo_normal_set(PM_Photo *photo,const char *thumb);
EAPI    const char*     pm_photo_thumb_fdo_normal_get(const PM_Photo *photo);

EAPI    PM_Photo *      pm_photo_eet_load(const char *eet_path, const char *key);
EAPI    int             pm_photo_eet_remove(const char *eet_path, const char* key);
EAPI    int             pm_photo_eet_save(PM_Photo *photo);
EAPI    Eet_Data_Descriptor * pm_photo_edd_new();

/* File manager */
EAPI    Eet_File *  pm_file_manager_open(const char *file);
EAPI    void        pm_file_manager_close(const char *file);
EAPI    void        pm_file_manager_flush();

/* Synchronisation Files <-> .eet */
typedef enum Sync_Error Sync_Error;
typedef void (*PM_Sync_Album_New_Cb)      (void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
typedef void (*PM_Sync_Album_Update_Cb)   (void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
typedef void (*PM_Sync_Album_Disappear_Cb) (void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
typedef void (*PM_Sync_Photo_New_Cb)      (void *data, Photo_Manager_Sync *sync, PM_Album *album, PM_Photo *photo);
typedef void (*PM_Sync_Photo_Update_Cb)   (void *data, Photo_Manager_Sync *sync, PM_Album *album, PM_Photo *photo);
typedef void (*PM_Sync_Photo_Disappear_Cb) (void *data, Photo_Manager_Sync *sync, PM_Album *album, PM_Photo *photo);
typedef void (*PM_Sync_Done_Cb)           (void *data, Photo_Manager_Sync *sync);
typedef void (*PM_Sync_Error_Cb)          (void *data, Photo_Manager_Sync *sync, Sync_Error error, const char* msg);

enum Sync_Error
{
    Sync_Error_Eet_Save_Failed
};

EAPI    Photo_Manager_Sync *   pm_sync_new(const char *path,
        PM_Sync_Album_New_Cb album_new_cb,
        PM_Sync_Album_Update_Cb album_update_cb,
        PM_Sync_Album_Disappear_Cb album_disappear_cb,
        PM_Sync_Photo_New_Cb photo_new_cb,
        PM_Sync_Photo_Update_Cb photo_update_cb,
        PM_Sync_Photo_Disappear_Cb photo_disappear_cb,
        PM_Sync_Done_Cb done_cb,
        PM_Sync_Error_Cb error_cb,
        void *user_data);
EAPI    void            pm_sync_free(Photo_Manager_Sync **sync);
EAPI    void            pm_sync_job_all_add(Photo_Manager_Sync *sync);
EAPI    void            pm_sync_job_album_folder_add(Photo_Manager_Sync *sync, const char *folder);
EAPI    void            pm_sync_job_photo_file_add(Photo_Manager_Sync *sync, const char *folder, const char *file);
//


/* Load the files from the .eet*/
typedef struct pm_load Photo_Manager_Load;
typedef struct PM_Load_Configuration PM_Load_Configuration;
typedef enum Load_Error Load_Error;
typedef void (*PM_Load_Conf_Album_Done_Cb) (void *data, Photo_Manager_Load *load, PM_Root *root, PM_Album *album);
typedef void (*PM_Load_Conf_Done_Cb)       (void *data, Photo_Manager_Load *load, int nb_albums, int nb_photos);
typedef void (*PM_Load_Conf_Error_Cb)      (void *data, Photo_Manager_Load *load, Load_Error error, const char* msg);

enum Load_Error
{
    Load_Error_Eet_Save_Failed
};

EAPI    Photo_Manager_Load *       pm_load_new(PM_Root *root,
        PM_Load_Conf_Album_Done_Cb album_done_cb,
        PM_Load_Conf_Done_Cb done_cb,
        PM_Load_Conf_Error_Cb error_cb,
        void *user_data);
EAPI    void                pm_load_free(Photo_Manager_Load **load);
EAPI    void                pm_load_run(Photo_Manager_Load *load);
//

/* thumbnails and images manager */
typedef struct Photo_Manager_Thumb Photo_Manager_Thumb;
typedef void (*PM_Thumb_Fdo_Normal_Cb) (void *data, Photo_Manager_Thumb *pm_thumb, PM_Photo *photo, const char *file);
typedef void (*PM_Thumb_Fdo_Large_Cb) (void *data, Photo_Manager_Thumb *pm_thumb, PM_Photo *photo, const char *file);

EAPI    Photo_Manager_Thumb *  pm_thumb_new(PM_Thumb_Fdo_Normal_Cb fdo_normal_cb,
                        PM_Thumb_Fdo_Large_Cb fdo_large_cb, void *data);
EAPI    const char* pm_thumb_photo_fdo_normal_get(Photo_Manager_Thumb *thumb, PM_Photo *photo);
EAPI    const char* pm_thumb_photo_fdo_large_get(Photo_Manager_Thumb *thumb, PM_Photo *photo);
EAPI    void        pm_thumb_free(Photo_Manager_Thumb **thumb);

#endif   /* ----- #ifndef PHOTO_MANAGER_INC  ----- */

