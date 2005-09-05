#include "Entangle.h"

#include <limits.h>
#include <string.h>

static int entangle_cb_exit(void *data, int type, void *ev);
static void entangle_usage(char ** argv);

int
main(int argc, char ** argv)
{
    char *display = NULL;
    char *theme = NULL;
    int ret = 1;
    int i;

    if (!ecore_init())
    {
        fprintf(stderr, "Error initing Ecore.\n");
        goto SHUTDOWN;
    }
    ecore_app_args_set(argc, (const char **)argv);
    ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, entangle_cb_exit, NULL);

    for (i = 1; i < argc; i++)
    {
        if ((!strcmp(argv[i], "-display")) || (!strcmp(argv[i], "-d")))
        {
            if (++i < argc)
            {
                IF_FREE(display);
                display = strdup(argv[i]);
            }
            else
            {
                fprintf(stderr, "ERROR: Missing argument to -display\n");
                goto ECORE_SHUTDOWN;
            }
        }
        else if ((!strcmp(argv[i], "-theme")) || (!strcmp(argv[i], "-t")))
        {
            if (++i < argc)
            {
                IF_FREE(theme);
                theme = strdup(argv[i]);
            }
            else
            {
                fprintf(stderr, "ERROR: Missing argument to -theme\n");
                goto ECORE_SHUTDOWN;
            }
        }
        else if ((!strcmp(argv[i], "-help")) || (!strcmp(argv[i], "-h")))
        {
            entangle_usage(argv);
            return 0;
        }
    }

    /* make sure the theme is valid */
    if (theme)
    {
        char *p;
        p = strstr(theme, ".edj");

        if (p && (strlen(theme) - (p - theme) == strlen(".edj")))
        {
            if (!ecore_file_exists(theme))
            {
                char tmp[PATH_MAX];
                snprintf(tmp, PATH_MAX, PACKAGE_DATA_DIR"/data/entangle/%s", theme);
                FREE(theme);

                if (ecore_file_exists(tmp)) theme = strdup(tmp);
            }
        }
        else
        {
            char tmp[PATH_MAX];
            snprintf(tmp, PATH_MAX, PACKAGE_DATA_DIR"/data/entangle/%s.edj", theme);
            FREE(theme);

            if (ecore_file_exists(tmp)) theme = strdup(tmp);
        }
    }
    if (!theme) theme = strdup(PACKAGE_DATA_DIR"/data/entangle/default.edj");

    /* make sure the display is valid */
    if (!display) 
    {
        char *tmp = getenv("DISPLAY");
        if (tmp) display = strdup(tmp);
    }
    if (display)
    {
        char *p;
        char buf[1024];

        p = strrchr(display, ':');
        if (!p)
        {
            snprintf(buf, sizeof(buf), "%s:0.0", display);
            FREE(display);
            display = strdup(buf);
        }
        else
        {
            p = strrchr(p, '.');
            if (!p)
            {
                snprintf(buf, sizeof(buf), "%s.0", display);
                FREE(display);
                display = strdup(buf);
            }
        }
    }
    else
        display = strdup(":0.0");

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

    if (!entangle_ui_init(display, theme))
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

    IF_FREE(display);
    IF_FREE(theme);
    return ret;
}

static int
entangle_cb_exit(void *data __UNUSED__, int type __UNUSED__, 
                                        void *ev __UNUSED__)
{
    ecore_main_loop_quit();
    return 0;
}

static void
entangle_usage(char ** argv)
{
    printf("%s -- Enlightenment 17 Menu Editor\n\n"
            "Usage: %s [OPTIONS]\n"
            "  OPTIONS:\n"
            "   -display STR    \t - Use the given display variable\n"
            "   -theme STR      \t - Use the given theme variable\n"
            "   -help           \t - This help text\n"
            "\n", argv[0], argv[0]);
}


