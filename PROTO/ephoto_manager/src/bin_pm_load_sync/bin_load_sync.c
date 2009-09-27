// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include <Ecore_Getopt.h>
#include "EPhoto_Manager.h"

#include "../../config.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

static void _sync_done_cb(void *data, Photo_Manager_Sync *sync);
static void _sync_error_cb(void *data, Photo_Manager_Sync *sync,  Sync_Error error, const char* msg);
static void _sync_album_new_cb(void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
static void _sync_album_update_cb(void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
static void _sync_album_disappear_cb(void *data, Photo_Manager_Sync *sync, PM_Root *root, PM_Album *album);
static void _sync_photo_new_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo);
static void _sync_photo_update_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo);
static void _sync_photo_disappear_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo);

static void _load_done_cb(void *data, Photo_Manager_Load *load, int nb_albums, int nb_photos);
static void _load_error_cb(void *data, Photo_Manager_Load *load,  Load_Error error, const char* msg);
static void _load_album_done_cb(void *data, Photo_Manager_Load *load, PM_Root *root, PM_Album *album);


static void _monitor_album_new_cb(void *data, PM_Root *root, const char *path);
static void _monitor_album_delete_cb(void *data, PM_Root *root, const char *path);
static void _monitor_pm_delete_cb(void *data, PM_Root *root);
static void _monitor_photo_new_cb(void *data, PM_Root *root, PM_Album *album, const char *path);
static void _monitor_photo_delete_cb(void *data, PM_Root *root, PM_Album *album, const char *path);
static void _monitor_photo_update_cb(void *data, PM_Root *root, PM_Album *album, const char *path);

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

static struct timeval t0;
static struct timeval t0_sync;
static struct timeval t;

int APP_LOG_DOMAIN;
#define LOG_DOMAIN APP_LOG_DOMAIN

int main(int argc, char **argv)
{
    unsigned char exit_option = 0;
    char *root_path = NULL;

    photo_manager_init();

    LOG_DOMAIN = eina_log_domain_register("bin_load_sync", "\033[34;1m");

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

    PM_Root *root = pm_root_new(_monitor_album_new_cb, _monitor_album_delete_cb, _monitor_pm_delete_cb,
            _monitor_photo_new_cb, _monitor_photo_delete_cb, _monitor_photo_update_cb,
            NULL);
    pm_root_path_set(root, root_path);


    Photo_Manager_Sync *sync = pm_sync_new(pm_root_path_get(root),
            _sync_album_new_cb, _sync_album_update_cb, _sync_album_disappear_cb,
            _sync_photo_new_cb, _sync_photo_update_cb, _sync_photo_disappear_cb,
            _sync_done_cb, _sync_error_cb, root);
    pm_root_sync_set(root, sync);

    Photo_Manager_Load *load = pm_load_new(root,
            _load_album_done_cb,
            _load_done_cb, _load_error_cb, root);

    pm_root_monitor_start(root);

    gettimeofday(&t0, NULL);

    pm_load_run(load);
    ecore_main_loop_begin();

    pm_sync_free(&sync);
    pm_root_free(&root);
    eina_log_domain_unregister(LOG_DOMAIN);

    photo_manager_shutdown();

    return 0;
}


static void _load_done_cb(void *data, Photo_Manager_Load *load, int nb_albums, int nb_photos)
{
    PM_Root *root = (PM_Root*)data;

    pm_load_free(&load);

    gettimeofday(&t, NULL);
    long time = (t.tv_sec*1000000+t.tv_usec) - (t0.tv_sec*1000000+t0.tv_usec);
    LOG_ERR("Load Time: %ld usec (%f sec)", time, time/1000000.);

    gettimeofday(&t0_sync, NULL);
    Photo_Manager_Sync *sync = pm_root_sync_get(root);
    pm_sync_job_all_add(sync);
}


static void _load_error_cb(void *data, Photo_Manager_Load *load,  Load_Error error, const char* msg)
{
    printf("LOAD CB ERROR : %s\n",msg);
}

static void _load_album_done_cb(void *data, Photo_Manager_Load *load,PM_Root *root, PM_Album *album)
{
    //printf("PM_Album loaded\n");
    pm_album_monitor_start(album);
}



static void _sync_done_cb(void *data, Photo_Manager_Sync *sync)
{
    //PM_Root *root = (Photo_Manager*)data;
    //pm_print(pm);

    pm_file_manager_flush();

    gettimeofday(&t, NULL);
    long time = (t.tv_sec*1000000+t.tv_usec) - (t0_sync.tv_sec*1000000+t0_sync.tv_usec);
    LOG_ERR("Sync Time: %ld usec (%f sec)", time, time/1000000.);
    time = (t.tv_sec*1000000+t.tv_usec) - (t0.tv_sec*1000000+t0.tv_usec);
    LOG_ERR("Total Time: %ld usec (%f sec)", time, time/1000000.);

    //ecore_main_loop_quit();
}


static void _sync_error_cb(void *data, Photo_Manager_Sync *sync,  Sync_Error error, const char* msg)
{
    printf("SYNC CB ERROR : %s\n",msg);
}

static void _sync_album_new_cb(void *data, Photo_Manager_Sync *sync,PM_Root *root, PM_Album *album)
{
    PM_Root *_root = (PM_Root*) data;
    PM_Album *_album = pm_album_copy_new(album);
    pm_root_album_add(_root, _album);
    pm_album_monitor_start(_album);
}

static void _sync_album_update_cb(void *data, Photo_Manager_Sync *sync,PM_Root *root, PM_Album *album)
{
    PM_Root *_root = (PM_Root*) data;

    PM_Album *_album = pm_root_album_search_file_name(_root, pm_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    pm_album_copy(album, _album);
}

static void _sync_album_disappear_cb(void *data, Photo_Manager_Sync *sync,PM_Root *root, PM_Album *album)
{
    PM_Root *_root = (PM_Root*) data;

    PM_Album *_album = pm_root_album_search_file_name(_root, pm_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    pm_root_album_remove(_root, _album);
    pm_album_free(&_album);
}

static void _sync_photo_new_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo)
{
    PM_Root *_root = (PM_Root*) data;

    PM_Album *_album = pm_root_album_search_file_name(_root, pm_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    PM_Photo *_photo = pm_photo_copy_new(photo);
    pm_album_photo_add(_album, _photo);
}

static void _sync_photo_update_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo)
{
    PM_Root *_root = (PM_Root*) data;

    PM_Album *_album = pm_root_album_search_file_name(_root, pm_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    PM_Photo *_photo = pm_album_photo_search_file_name(_album, pm_photo_file_name_get(photo));
    pm_photo_copy(photo, _photo);
}

static void _sync_photo_disappear_cb(void *data, Photo_Manager_Sync *sync,PM_Album *album, PM_Photo *photo)
{
    PM_Root *_root = (PM_Root*) data;

    PM_Album *_album = pm_root_album_search_file_name(_root, pm_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    PM_Photo *_photo = pm_album_photo_search_file_name(_album, pm_photo_file_name_get(photo));
    pm_album_photo_remove(_album, _photo);
    pm_photo_free(&_photo);
}

static void _monitor_album_new_cb(void *data, PM_Root *root, const char *path)
{
    const char *file_name = ecore_file_file_get(path);

    Photo_Manager_Sync *sync = pm_root_sync_get(root);
    pm_sync_job_album_folder_add(sync, file_name);
}

static void _monitor_album_delete_cb(void *data, PM_Root *root, const char *path)
{
    //delete the album
    const char *file_name = ecore_file_file_get(path);
    Photo_Manager_Sync *sync = pm_root_sync_get(root);
    pm_sync_job_album_folder_add(sync, file_name);
}

static void _monitor_pm_delete_cb(void *data, PM_Root *root)
{
    printf("Photo_Manager delete !!!\n");
}

static void _monitor_photo_new_cb(void *data, PM_Root *root, PM_Album *album, const char *path)
{
    const char *file_name = ecore_file_file_get(path);
    Photo_Manager_Sync *sync = pm_root_sync_get(root);
    pm_sync_job_photo_file_add(sync, pm_album_name_get(album), file_name);
}

static void _monitor_photo_delete_cb(void *data, PM_Root *root, PM_Album *album, const char *path)
{
    const char *file_name = ecore_file_file_get(path);
    Photo_Manager_Sync *sync = pm_root_sync_get(root);
    pm_sync_job_photo_file_add(sync, pm_album_name_get(album), file_name);
}

static void _monitor_photo_update_cb(void *data, PM_Root *root, PM_Album *album, const char *path)
{
    const char *file_name = ecore_file_file_get(path);
    Photo_Manager_Sync *sync = pm_root_sync_get(root);
    pm_sync_job_photo_file_add(sync, pm_album_name_get(album), file_name);
}

