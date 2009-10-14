// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"


int PM_LOG_DOMAIN_99;

static int count = 0;

/*
 * Initialise the library.
 */
int photo_manager_init()
{
    if(count>0) return ++count;

    eet_init();
    ecore_init();
    ecore_file_init();
    ethumb_client_init();

    pm_file_manager_init();
    pm_thumb_init();

    LOG_DOMAIN = eina_log_domain_register("LIB Photo Manager", "\033[34;1m");

    return ++count;
}

/*
 * Shutdown the library
 */
int photo_manager_shutdown()
{
    if(count>1) return --count;

    eina_log_domain_unregister(LOG_DOMAIN);

    pm_thumb_shutdown();
    pm_file_manager_shutdown();
    ethumb_client_shutdown();
    ecore_file_shutdown();
    ecore_shutdown();
    eet_shutdown();

    return --count;
}

Eet_Data_Descriptor *_pm_string_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, PM_String);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_String, "string", string, EET_T_STRING);

    return edd;
}

