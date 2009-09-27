
#include <Ecore_Getopt.h>

#include "EPhoto_Manager.h"

#include "../../config.h"

static void _sync_done_cb(void *data, Photo_Manager_Sync *sync);
static void _sync_error_cb(void *data, Photo_Manager_Sync *sync,  Sync_Error error, const char* msg);
static void _sync_album_new_cb(void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
static void _sync_album_update_cb(void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
static void _sync_album_disapear_cb(void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
static void _sync_photo_new_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo);
static void _sync_photo_update_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo);
static void _sync_photo_disapear_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo);

static const Ecore_Getopt options = {
    "Test PM_Photo manager",
    NULL,
    VERSION,
    "(C) 2009 Test PM_Photo manager, see AUTHORS.",
    "LGPL with advertisement, see COPYING",
    "\n\n",
        1,
        {
            ECORE_GETOPT_VERSION('V', "version"),
            ECORE_GETOPT_COPYRIGHT('R', "copyright"),
            ECORE_GETOPT_LICENSE('L', "license"),
            ECORE_GETOPT_STORE_STR('a', "pm", "Specify the location of your pm"),
            ECORE_GETOPT_HELP('h', "help"),
            ECORE_GETOPT_SENTINEL
        }
};

int main(int argc, char **argv)
{
    unsigned char exit_option = 0;
    char *root_path = NULL;

    photo_manager_init();

    //ecore_getopt
    Ecore_Getopt_Value values[] = {
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_STR(root_path),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
    };
    ecore_app_args_set(argc, (const char **) argv);
    int nonargs = ecore_getopt_parse(&options, values, argc, argv);
    if (nonargs < 0)
        return 1;
    else if (nonargs != argc)
    {
        fputs("Invalid non-option argument", stderr);
        ecore_getopt_help(stderr, &options);
        return 1;
    }
    if(!root_path)
    {
        fprintf(stderr, "You must specify the location of your pm !\n");
        return 0;
    }

    if(exit_option)
        return 0;
    //

    Photo_Manager_Sync *sync = pm_sync_new(root_path,
            _sync_album_new_cb, _sync_album_update_cb, _sync_album_disapear_cb,
            _sync_photo_new_cb, _sync_photo_update_cb, _sync_photo_disapear_cb,
            _sync_done_cb, _sync_error_cb, NULL);

    pm_sync_job_all_add(sync);

    ecore_main_loop_begin();

    pm_sync_free(&sync);

    photo_manager_shutdown();

    return 0;
}


static void _sync_done_cb(void *data, Photo_Manager_Sync *sync)
{
    ecore_main_loop_quit();
}


static void _sync_error_cb(void *data, Photo_Manager_Sync *sync,  Sync_Error error, const char* msg)
{
    printf("SYNC CB ERROR : %s\n",msg);
}

static void _sync_album_new_cb(void *data, Photo_Manager_Sync *sync,PM_Root *root, PM_Album *album)
{
    ;
}

static void _sync_album_update_cb(void *data, Photo_Manager_Sync *sync,PM_Root *root, PM_Album *album)
{
    ;
}

static void _sync_album_disapear_cb(void *data, Photo_Manager_Sync *sync,PM_Root *root, PM_Album *album)
{
    ;
}

static void _sync_photo_new_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo)
{
    ;
}

static void _sync_photo_update_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo)
{
    ;
}

static void _sync_photo_disapear_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo)
{
    ;
}

