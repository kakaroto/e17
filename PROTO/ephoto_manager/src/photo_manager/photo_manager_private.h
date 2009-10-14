#ifndef  PHOTO_MANAGER_PRIVATE_INC
#define  PHOTO_MANAGER_PRIVATE_INC

#include "EPhoto_Manager.h"
#include <pthread.h>
#include <errno.h>

#define EET_FILE "data.eet"
#define EET_FOLDER_ROOT_DB ".ephoto_manager/"
#define EET_FILE_ROOT_DB "roots.eet"

extern int PM_LOG_DOMAIN_99;
#define LOG_DOMAIN PM_LOG_DOMAIN_99

struct Photo_Manager_Configuration
{
    void *data;
    struct
    {
        Photo_Manager_Album_New_Cb album_new_cb;
        Photo_Manager_Album_Delete_Cb album_delete_cb;
        Photo_Manager_Delete_Cb pm_delete_cb;
        Photo_Manager_Photo_New_Cb photo_new_cb;
        Photo_Manager_Photo_Delete_Cb photo_delete_cb;
        Photo_Manager_Photo_Update_Cb photo_update_cb;
    } monitor;

    struct {
        Photo_Manager_Collection_New_Cb new_cb;
        Photo_Manager_Collection_Delete_Cb delete_cb;
        Photo_Manager_Collection_Album_New_Cb album_new_cb;
        Photo_Manager_Collection_Album_Delete_Cb album_delete_cb;
    } collection;
};

Photo_Manager_Configuration pm_conf_get(PM_Root *root);

int pm_file_manager_init();
int pm_file_manager_shutdown();

int pm_thumb_init();
int pm_thumb_shutdown();


void _pm_root_collection_album_add(PM_Root *root, PM_Album_Collection *album_col, PM_Album *album);
void _pm_root_collection_album_remove(PM_Root *root, PM_Album_Collection *album_col, PM_Album *album);

Eet_Data_Descriptor *_pm_string_edd_new();

Eet_Data_Descriptor *_pm_album_file_name_edd_new();
Eet_Data_Descriptor *_pm_photo_file_name_edd_new();

Eet_Data_Descriptor * _pm_album_header_edd_new(Eet_Data_Descriptor *edd_collection);
Eet_Data_Descriptor * _pm_album_collection_edd_new();
Eet_Data_Descriptor * _pm_collection_edd_new();

#endif   /* ----- #ifndef PHOTO_MANAGER_PRIVATE_INC  ----- */

