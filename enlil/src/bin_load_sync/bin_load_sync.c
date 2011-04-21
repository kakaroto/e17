#include <Ecore_Getopt.h>
#include "Enlil.h"

#include "../../config.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "../define.h"

static void _sync_done_cb(void *data, Enlil_Sync *_sync);
static void _sync_start_cb(void *data, Enlil_Sync *_sync);
static void _sync_error_cb(void *data, Enlil_Sync *_sync,  Sync_Error error, const char* msg);
static void _sync_album_new_cb(void *data, Enlil_Sync *_sync, Enlil_Library *library, Enlil_Album *album);
static void _sync_album_update_cb(void *data, Enlil_Sync *_sync, Enlil_Library *library, Enlil_Album *album);
static void _sync_album_disappear_cb(void *data, Enlil_Sync *_sync, Enlil_Library *library, Enlil_Album *album);
static void _sync_photo_new_cb(void *data, Enlil_Sync *_sync,Enlil_Album *album, Enlil_Photo *photo);
static void _sync_photo_update_cb(void *data, Enlil_Sync *_sync,Enlil_Album *album, Enlil_Photo *photo);
static void _sync_photo_disappear_cb(void *data, Enlil_Sync *_sync,Enlil_Album *album, Enlil_Photo *photo);

static void _load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos);
static void _load_error_cb(void *data, Enlil_Load *load,  Load_Error error, const char* msg);
static void _load_album_done_cb(void *data, Enlil_Load *load, Enlil_Library *library, Enlil_Album *album);


static void _monitor_album_new_cb(void *data, Enlil_Library *library, const char *path);
static void _monitor_album_delete_cb(void *data, Enlil_Library *library, const char *path);
static void _monitor_enlil_delete_cb(void *data, Enlil_Library *library);
static void _monitor_photo_new_cb(void *data, Enlil_Library *library, Enlil_Album *album, const char *path);
static void _monitor_photo_delete_cb(void *data, Enlil_Library *library, Enlil_Album *album, const char *path);
static void _monitor_photo_update_cb(void *data, Enlil_Library *library, Enlil_Album *album, const char *path);

static const Ecore_Getopt options = {
    "Test Enlil_Photo manager",
    NULL,
    VERSION,
    "(C) 2009 Test Enlil_Photo manager, see AUTHORS.",
    "LGPL with advertisement, see COPYING",
    "\n\n",
        1,
        {
            ECORE_GETOPT_VERSION('V', "version"),
            ECORE_GETOPT_COPYRIGHT('R', "copyright"),
            ECORE_GETOPT_LICENSE('L', "license"),
            ECORE_GETOPT_STORE_STR('l', "library", "Specify the location of a library"),
            ECORE_GETOPT_HELP('h', "help"),
            ECORE_GETOPT_SENTINEL
        }
};

double t0;
double t0_sync;
double t;

int APP_LOG_DOMAIN;
#define LOG_DOMAIN APP_LOG_DOMAIN

int main(int argc, char **argv)
{
    unsigned char exit_option = 0;
    char *library_path = NULL;

    enlil_init();

    LOG_DOMAIN = eina_log_domain_register("bin_load_sync", "\033[34;1m");

    //ecore_getopt
    Ecore_Getopt_Value values[] = {
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_STR(library_path),
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
    if(!library_path)
    {
        fprintf(stderr, "You must specify the location of your enlil !\n");
        return 0;
    }

    if(exit_option)
        return 0;
    //

    Enlil_Library *library = enlil_library_new(_monitor_album_new_cb, _monitor_album_delete_cb, _monitor_enlil_delete_cb,
            _monitor_photo_new_cb, _monitor_photo_delete_cb, _monitor_photo_update_cb,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    enlil_library_path_set(library, library_path);


    Enlil_Sync *_sync = enlil_sync_new(enlil_library_path_get(library),
            _sync_album_new_cb, _sync_album_update_cb, _sync_album_disappear_cb,
            _sync_photo_new_cb, _sync_photo_update_cb, _sync_photo_disappear_cb,
            _sync_done_cb, _sync_start_cb, _sync_error_cb, library);
    enlil_library_sync_set(library, _sync);

    Enlil_Load *load = enlil_load_new(library,
            _load_album_done_cb,
            _load_done_cb, _load_error_cb, library);

    enlil_library_monitor_start(library);

    t0 = ecore_time_get();

    enlil_load_run(load);
    ecore_main_loop_begin();

    enlil_sync_free(&_sync);
    enlil_library_free(&library);
    eina_log_domain_unregister(LOG_DOMAIN);

    enlil_shutdown();

    return 0;
}


static void _load_done_cb(void *data, Enlil_Load *load, __UNUSED__ int nb_albums, __UNUSED__ int nb_photos)
{
    Enlil_Library *library = (Enlil_Library*)data;

    enlil_load_free(&load);

    t = ecore_time_get();
    double _time = t - t0;
    LOG_ERR("Load Time: %f sec)", _time);

    t0_sync = ecore_time_get();
    Enlil_Sync *_sync = enlil_library_sync_get(library);
    enlil_sync_job_all_add(_sync);
}


static void _load_error_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Load *load, __UNUSED__ Load_Error error, const char* msg)
{
    printf("LOAD CB ERROR : %s\n",msg);
}

static void _load_album_done_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Load *load, __UNUSED__ Enlil_Library *library, Enlil_Album *album)
{
    //printf("Enlil_Album loaded\n");
    enlil_album_monitor_start(album);
}



static void _sync_done_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *_sync)
{
    //Enlil_Root *library = (Enlil*)data;
    //enlil_print(enlil);

    enlil_file_manager_flush();

    t = ecore_time_get();
    double _time = t - t0_sync;
    LOG_ERR("Sync Time: %f sec", _time);
    _time = t - t0;
    LOG_ERR("Total Time: %f sec", _time);

    //ecore_main_loop_quit();
}

static void _sync_start_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *_sync)
{
   //Enlil_Root *library = (Enlil*)data;
   t0_sync = ecore_time_get();
}

static void _sync_error_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *_sync,  __UNUSED__ Sync_Error error, const char* msg)
{
    printf("SYNC CB ERROR : %s\n",msg);
}

static void _sync_album_new_cb(void *data, __UNUSED__ Enlil_Sync *_sync, __UNUSED__ Enlil_Library *library, Enlil_Album *album)
{
    Enlil_Library *_library = (Enlil_Library*) data;
    Enlil_Album *_album = enlil_album_copy_new(album);
    enlil_library_album_add(_library, _album);
    enlil_album_monitor_start(_album);
}

static void _sync_album_update_cb(void *data, __UNUSED__ Enlil_Sync *_sync, __UNUSED__ Enlil_Library *library, Enlil_Album *album)
{
    Enlil_Library *_library = (Enlil_Library*) data;

    Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    enlil_album_copy(album, _album);
}

static void _sync_album_disappear_cb(void *data, __UNUSED__ Enlil_Sync *_sync, __UNUSED__ Enlil_Library *library, Enlil_Album *album)
{
    Enlil_Library *_library = (Enlil_Library*) data;

    Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    enlil_library_album_remove(_library, _album);
    enlil_album_free(&_album);
}

static void _sync_photo_new_cb(void *data, __UNUSED__ Enlil_Sync *_sync,Enlil_Album *album, Enlil_Photo *photo)
{
    Enlil_Library *_library = (Enlil_Library*) data;

    Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    Enlil_Photo *_photo = enlil_photo_copy_new(photo);
    enlil_album_photo_add(_album, _photo);
}

static void _sync_photo_update_cb(void *data, __UNUSED__ Enlil_Sync *_sync,Enlil_Album *album, Enlil_Photo *photo)
{
    Enlil_Library *_library = (Enlil_Library*) data;

    Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    Enlil_Photo *_photo = enlil_album_photo_search_file_name(_album, enlil_photo_file_name_get(photo));
    enlil_photo_copy(photo, _photo);
}

static void _sync_photo_disappear_cb(void *data, __UNUSED__ Enlil_Sync *_sync,Enlil_Album *album, Enlil_Photo *photo)
{
    Enlil_Library *_library = (Enlil_Library*) data;

    Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));
    ASSERT_RETURN_VOID(_album != NULL);

    Enlil_Photo *_photo = enlil_album_photo_search_file_name(_album, enlil_photo_file_name_get(photo));
    enlil_album_photo_remove(_album, _photo);
    enlil_photo_free(&_photo);
}

static void _monitor_album_new_cb(__UNUSED__ void *data, Enlil_Library *library, const char *path)
{
    const char *file_name = ecore_file_file_get(path);

    Enlil_Sync *_sync = enlil_library_sync_get(library);
    enlil_sync_job_album_folder_add(_sync, file_name);
}

static void _monitor_album_delete_cb(__UNUSED__ void *data, Enlil_Library *library, const char *path)
{
    //delete the album
    const char *file_name = ecore_file_file_get(path);
    Enlil_Sync *_sync = enlil_library_sync_get(library);
    enlil_sync_job_album_folder_add(_sync, file_name);
}

static void _monitor_enlil_delete_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Library *library)
{
    printf("Enlil delete !!!\n");
}

static void _monitor_photo_new_cb(__UNUSED__ void *data, Enlil_Library *library, Enlil_Album *album, const char *path)
{
    const char *file_name = ecore_file_file_get(path);
    Enlil_Sync *_sync = enlil_library_sync_get(library);
    enlil_sync_job_photo_file_add(_sync, enlil_album_file_name_get(album), file_name);
}

static void _monitor_photo_delete_cb(__UNUSED__ void *data, Enlil_Library *library, Enlil_Album *album, const char *path)
{
    const char *file_name = ecore_file_file_get(path);
    Enlil_Sync *_sync = enlil_library_sync_get(library);
    enlil_sync_job_photo_file_add(_sync, enlil_album_file_name_get(album), file_name);
}

static void _monitor_photo_update_cb(__UNUSED__ void *data, Enlil_Library *library, Enlil_Album *album, const char *path)
{
    const char *file_name = ecore_file_file_get(path);
    Enlil_Sync *_sync = enlil_library_sync_get(library);
    enlil_sync_job_photo_file_add(_sync, enlil_album_file_name_get(album), file_name);
}

