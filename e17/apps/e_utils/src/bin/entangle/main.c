#include "Entangle.h"

static int entangle_cb_exit(void *data, int type, void *ev);

int
main(int argc, char ** argv)
{
    int ret = 1;

    if (!ecore_init())
    {
        fprintf(stderr, "Error initing Ecore.\n");
        goto SHUTDOWN;
    }
    ecore_app_args_set(argc, (const char **)argv);
    ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, entangle_cb_exit, NULL);

    if (!ecore_file_init())
    {
        fprintf(stderr, "Error initing Ecore_File.\n");
        goto ECORE_SHUTDOWN;
    }

    if (!ecore_evas_init())
    {
        fprintf(stderr, "Error initing Ecore_Evas.\n");
        goto ECORE_FILE_SHUTDOWN;
    }

    if (!edje_init())
    {
        fprintf(stderr, "Error initing Edje.\n");
        goto ECORE_EVAS_SHUTDOWN;
    }

    if (!eet_init())
    {
        fprintf(stderr, "Error initing Eet.\n");
        goto EDJE_SHUTDOWN;
    }

    if (!entangle_eapps_init())
    {
        fprintf(stderr, "Error initing Entangle_Eapps.\n");
        goto EET_SHUTDOWN;
    }

    if (!entangle_apps_init())
    {
        fprintf(stderr, "Error initing Entangle_Apps.\n");
        goto ENTANGLE_EAPPS_SHUTDOWN;
    }

    if (!entangle_ui_init())
    {
        fprintf(stderr, "Error initing Entangle_Ui.\n");
        goto ENTANGLE_APPS_SHUTDOWN;
    }

    ecore_main_loop_begin();
    ret = 0;

    entangle_ui_shutdown();
ENTANGLE_APPS_SHUTDOWN:
    entangle_apps_shutdown();
ENTANGLE_EAPPS_SHUTDOWN:
    entangle_eapps_shutdown();
EET_SHUTDOWN:
    eet_shutdown();
EDJE_SHUTDOWN:
    edje_shutdown();
ECORE_EVAS_SHUTDOWN:
    ecore_evas_shutdown();
ECORE_FILE_SHUTDOWN:
    ecore_file_shutdown();
ECORE_SHUTDOWN:
    ecore_shutdown();
SHUTDOWN:
    return ret;
}

static int
entangle_cb_exit(void *data __UNUSED__, int type __UNUSED__, 
                                        void *ev __UNUSED__)
{
    ecore_main_loop_quit();
    return 0;
}

