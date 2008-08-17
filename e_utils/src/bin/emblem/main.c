#include "Emblem.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define IF_FREE(x) { if (x) free(x); x = NULL; }

static void usage(void);

static int
exit_cb(void *data __UNUSED__, int type __UNUSED__, void *ev __UNUSED__)
{
    ecore_main_loop_quit();
    return 0;
}

int
main(int argc, char ** argv)
{
    int ret = 1;
    int i;
    Emblem *em;

    if (!ecore_init()) 
    {
        printf("Unable to init ecore, exiting\n");
        goto EXIT;
    }
    ecore_app_args_set(argc, (const char **)argv);
    ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);

    if (!(em = emblem_new()))
    {
        printf("Can't create Emblem struct\n");
        goto ECORE_SHUTDOWN;
    }

    /* arg handling */
    for (i = 1; i < argc; i++)
    {
        if ((!strcmp(argv[i], "-display")) || (!strcmp(argv[i], "-d")))
        {
            if (++i < argc)
            {
                em->display = strdup(argv[i]);
            }
            else
            {
                printf("Missing argument to -display\n");
                goto ECORE_SHUTDOWN;
            }
        }      
        else if ((!strcmp(argv[i], "-theme")) || (!strcmp(argv[i], "-t")))
        {
            if (++i < argc)
            {
                em->theme = strdup(argv[i]);
            }
            else
            {
                printf("Missing argument to -theme\n");
                goto ECORE_SHUTDOWN;
            }
        }
        else if ((!strcmp(argv[i], "-help")) || (!strcmp(argv[i], "-h")))
        {
            usage();
            return 0;
        }
        else
        {
            printf("%s - unrecognized option\n", argv[i]);
            usage();
            return 0;
        }
    }
    
    /* make sure theme exists */
    if (!em->theme)
    {
        em->theme = strdup(PACKAGE_DATA_DIR"/data/emblem/default.edj");
    }
    else
    {
        if (!ecore_file_exists(em->theme))
        {
            char theme[PATH_MAX];
            snprintf(theme, sizeof(theme), PACKAGE_DATA_DIR"/data/emblem/%s", em->theme);
            if (ecore_file_exists(theme))
            {
                IF_FREE(em->theme);
                em->theme = strdup(theme);
            }
            else
            {
                snprintf(theme, sizeof(theme), PACKAGE_DATA_DIR"/data/emblem/%s.edj", em->theme);
                if (ecore_file_exists(theme))
                {
                    IF_FREE(em->theme);
                    em->theme = strdup(theme);
                }
                else
                {
                    IF_FREE(em->theme);
                    em->theme = strdup(PACKAGE_DATA_DIR"/data/emblem/default.edj");
                    fprintf(stderr, "Theme does not exist! Falling back to default theme\n");
                }
            }
        }
    }

    /* if no display given grab the env var */
    if (!em->display)
    {
        char *tmp = getenv("DISPLAY");
        if (tmp) em->display = strdup(tmp);
    }
   
    /* make sure the display var is of the form name:0.0 or :0.0 */
    if (em->display)
    {
        char *p;
        char buf[1024];

        p = strrchr(em->display, ':');
        if (!p)
        {
            snprintf(buf, sizeof(buf), "%s:0.0", em->display);
            free(em->display);
            em->display = strdup(buf);
        }
        else
        {
            p = strrchr(p, '.');
            if (!p)
            {
                snprintf(buf, sizeof(buf), "%s.0", em->display);
                free(em->display);
                em->display = strdup(buf);
            }
        }
    }
    else
        em->display = strdup(":0.0");

    if (!ecore_evas_init())
    {
        printf("Unable to init ecore_evas, exiting\n");
        goto ECORE_SHUTDOWN;
    }

    if (!edje_init())
    {
        printf("Unable to init edje, exiting\n");
        goto ECORE_EVAS_SHUTDOWN;
    }

    if (!e_lib_init(em->display))
    {
        printf("Can't connect to e17, exiting\n");
        goto EDJE_SHUTDOWN;
    }

    epsilon_init();

    if (!emblem_ui_init(em))
    {
        printf("Unable to setup UI\n");
        goto ENLIGHTENMENT_SHUTDOWN;
    }
    
    ecore_main_loop_begin();
    ret = 0;

    emblem_free(em);
    emblem_ui_shutdown();

ENLIGHTENMENT_SHUTDOWN:
    e_lib_shutdown();

EDJE_SHUTDOWN:
    edje_shutdown();

ECORE_EVAS_SHUTDOWN:
    ecore_evas_shutdown();

ECORE_SHUTDOWN:
    ecore_shutdown();

EXIT:
    return ret;
}

static
void usage(void)
{
    printf("Emblem - Set the Enlightenment 17 Background\n\n"
           "Usage: emblem [OPTIONS]\n\n"
           "\tOPTIONS\n"
           "  -theme <THEME>    \t - Use the given theme\n"
           "  -display <STR>    \t - Use the given X display\n"
           "  -help             \t - This help text\n"
           "\n");
}


