#include <Ecore_Getopt.h>

#include "Enlil.h"

#include "../../config.h"

static void _sync_done_cb(void *data, Enlil_Sync *sync);
static void _sync_start_cb(void *data, Enlil_Sync *sync);
static void _sync_error_cb(void *data, Enlil_Sync *sync,  Sync_Error error, const char* msg);
static void _sync_album_new_cb(void *data, Enlil_Sync *sync, Enlil_Library *library, Enlil_Album *album);
static void _sync_album_update_cb(void *data, Enlil_Sync *sync, Enlil_Library *library, Enlil_Album *album);
static void _sync_album_disapear_cb(void *data, Enlil_Sync *sync, Enlil_Library *library, Enlil_Album *album);
static void _sync_photo_new_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);
static void _sync_photo_update_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);
static void _sync_photo_disapear_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);

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

int main(int argc, char **argv)
{
    unsigned char exit_option = 0;
    char *library_path = NULL;

    enlil_init();

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

    Enlil_Sync *sync = enlil_sync_new(library_path,
            _sync_album_new_cb, _sync_album_update_cb, _sync_album_disapear_cb,
            _sync_photo_new_cb, _sync_photo_update_cb, _sync_photo_disapear_cb,
            _sync_done_cb, _sync_start_cb, _sync_error_cb, NULL);

    enlil_sync_job_all_add(sync);

    ecore_main_loop_begin();

    enlil_sync_free(&sync);

    enlil_shutdown();

    return 0;
}


static void _sync_done_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync)
{
    ecore_main_loop_quit();
}

static void _sync_start_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync)
{
    ;
}

static void _sync_error_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync,  __UNUSED__ Sync_Error error, const char* msg)
{
    printf("SYNC CB ERROR : %s\n",msg);
}

static void _sync_album_new_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync, __UNUSED__ Enlil_Library *library, __UNUSED__ Enlil_Album *album)
{
    ;
}

static void _sync_album_update_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync, __UNUSED__ Enlil_Library *library, __UNUSED__ Enlil_Album *album)
{
    ;
}

static void _sync_album_disapear_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync, __UNUSED__ Enlil_Library *library, __UNUSED__ Enlil_Album *album)
{
    ;
}

static void _sync_photo_new_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync, __UNUSED__ Enlil_Album *album, __UNUSED__ Enlil_Photo *photo)
{
    ;
}

static void _sync_photo_update_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync, __UNUSED__ Enlil_Album *album, __UNUSED__ Enlil_Photo *photo)
{
    ;
}

static void _sync_photo_disapear_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Sync *sync, __UNUSED__ Enlil_Album *album, __UNUSED__ Enlil_Photo *photo)
{
    ;
}

