#ifndef  PHOTO_MANAGER_PRIVATE_INC
#define  PHOTO_MANAGER_PRIVATE_INC

#include "EPhoto_Manager.h"
#include <pthread.h>
#include <errno.h>

extern int PM_LOG_DOMAIN_99;
#define LOG_DOMAIN PM_LOG_DOMAIN_99

struct Photo_Manager_Configuration
{
    void *data;
    Photo_Manager_Album_New_Cb album_new_cb;
    Photo_Manager_Album_Delete_Cb album_delete_cb;
    Photo_Manager_Delete_Cb pm_delete_cb;
    Photo_Manager_Photo_New_Cb photo_new_cb;
    Photo_Manager_Photo_Delete_Cb photo_delete_cb;
    Photo_Manager_Photo_Update_Cb photo_update_cb;
};

Photo_Manager_Configuration pm_conf_get(PM_Root *root);

int pm_file_manager_init();
int pm_file_manager_shutdown();

Eet_Data_Descriptor *_pm_album_file_name_edd_new();
Eet_Data_Descriptor *_pm_photo_file_name_edd_new();

Eet_Data_Descriptor * _pm_album_global_header_edd_new();
Eet_Data_Descriptor * _pm_album_header_edd_new();


#endif   /* ----- #ifndef PHOTO_MANAGER_PRIVATE_INC  ----- */

