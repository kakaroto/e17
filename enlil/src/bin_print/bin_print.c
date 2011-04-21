#include <Ecore_Getopt.h>
#include "Enlil.h"

#include "../../config.h"

static void _load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos);
static void _load_error_cb(void *data, Enlil_Load *load,  Load_Error error, const char* msg);
static void _load_album_done_cb(void *data, Enlil_Load *load, Enlil_Library *library, Enlil_Album *album);

static const Ecore_Getopt options = {
    "Test Enlil, a photo manager",
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

    Enlil_Library *library = enlil_library_new(NULL, NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
    enlil_library_path_set(library, library_path);

    Enlil_Load *load = enlil_load_new(library,
            _load_album_done_cb,
            _load_done_cb, _load_error_cb, library);

    enlil_library_monitor_start(library);

    enlil_load_run(load);
    ecore_main_loop_begin();

    enlil_library_free(&library);

    enlil_shutdown();

    return 0;
}


static void _load_done_cb(void *data, Enlil_Load *load, __UNUSED__ int nb_albums, __UNUSED__ int nb_photos)
{
    Enlil_Library *library = (Enlil_Library*)data;

    enlil_library_print(library);
    enlil_load_free(&load);

    ecore_main_loop_quit();
}


static void _load_error_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Load *load,  __UNUSED__ Load_Error error, const char* msg)
{
    printf("LOAD CB ERROR : %s\n",msg);
}

static void _load_album_done_cb(__UNUSED__ void *data, __UNUSED__ Enlil_Load *load, __UNUSED__ Enlil_Library *library, __UNUSED__ Enlil_Album *album)
{
    ;//printf("Enlil_Album loaded\n");
}



