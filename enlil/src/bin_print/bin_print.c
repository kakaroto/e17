#include <Ecore_Getopt.h>
#include "Enlil.h"

#include "../../config.h"

static void _load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos);
static void _load_error_cb(void *data, Enlil_Load *load,  Load_Error error, const char* msg);
static void _load_album_done_cb(void *data, Enlil_Load *load, Enlil_Root *root, Enlil_Album *album);

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
    char *root_path = NULL;

    enlil_init();

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
        fprintf(stderr, "You must specify the location of your enlil !\n");
        return 0;
    }

    if(exit_option)
        return 0;
    //

    Enlil_Root *root = enlil_root_new(NULL, NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
    enlil_root_path_set(root, root_path);

    Enlil_Load *load = enlil_load_new(root,
            _load_album_done_cb,
            _load_done_cb, _load_error_cb, root);

    enlil_root_monitor_start(root);

    enlil_load_run(load);
    ecore_main_loop_begin();

    enlil_root_free(&root);

    enlil_shutdown();

    return 0;
}


static void _load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos)
{
    Enlil_Root *root = (Enlil_Root*)data;

    enlil_root_print(root);
    enlil_load_free(&load);

    ecore_main_loop_quit();
}


static void _load_error_cb(void *data, Enlil_Load *load,  Load_Error error, const char* msg)
{
    printf("LOAD CB ERROR : %s\n",msg);
}

static void _load_album_done_cb(void *data, Enlil_Load *load,Enlil_Root *root, Enlil_Album *album)
{
    ;//printf("Enlil_Album loaded\n");
}



