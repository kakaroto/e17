#include "config.h"
#include "Eke.h"
#include "eke_feed.h"
#include "eke_gui.h"

#define CACHE_DIR "~/.e/apps/eke/cache"

static int eke_exit_cb(void *data, int type, void *ev);
static int eke_find_theme(Eke *eke, const char *thm);

int
main(int argc, char ** argv)
{
    Eke eke;
    char *thm = NULL;
    int i, last_arg = 0;

    eke.gui.type = EKE_GUI_TYPE_EWL;
    for (i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "--edje-theme", 12)) {
            if (argc > (i + 1)) {
                i++;
                thm = strdup(argv[i]);
            }
            last_arg = i;

        } else if (!strncmp(argv[i], "--gui-edje", 10 )) {
            eke.gui.type = EKE_GUI_TYPE_EDJE;
            last_arg = i;

        } else if (!strncmp(argv[i], "--gui-ewl", 9)) {
            eke.gui.type = EKE_GUI_TYPE_EWL;
            last_arg = i;
        }
    }

    if (!thm) thm = strdup("default");
    if (!eke_find_theme(&eke, thm)) {
        printf("Unable to locate theme (%s)\n", thm);
        return 0;
    }
    FREE(thm);

    if (!eke_feed_init(CACHE_DIR)) {
        printf("Cannot setup eke\n");
        return 0;
    }
    ecore_event_handler_add(EKE_FEED_EVENT_DATA, 
                                eke_gui_value_changed_cb, &eke);
    ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,
                                eke_exit_cb, NULL);

    if (!eke_gui_init(&eke, &argc, argv)) {
        printf("Cannot setup eke_gui\n");
        eke_feed_shutdown();
        return 0;
    }

    eke_gui_create(&eke);

    /* setup cmdline feed if given */
    last_arg++;
    if (last_arg < argc) {
        Eke_Feed *feed;
        if (!strncmp(argv[last_arg], "http://", 7)) {
            feed = eke_feed_new_from_uri(argv[last_arg]);

        } else {
            printf("incorrect format for URI\n");

            eke_gui_shutdown(&eke);
            eke_feed_shutdown();
            return 0;
        }

        eke_gui_feed_register(&eke, feed);
        eke_feed_update(feed);
    }

    /* XXX setup any feeds stored in the cfg ... */
    eke_gui_begin(&eke);

    eke_gui_shutdown(&eke);
    eke_feed_shutdown();

    return 1;
}

static int
eke_exit_cb(void *data, int type, void *ev)
{
    ecore_main_loop_quit();
    return 1;
    data = NULL;
    type = 0;
    ev = NULL;
}

static int
eke_find_theme(Eke *eke, const char *thm)
{
    char path[PATH_MAX];
    struct stat stat_buf;

    /* look in the home dir */
    snprintf(path, PATH_MAX, "%s/.e/apps/" PACKAGE "/themes/%s.eet",
                                                getenv("HOME"), thm);
    if (!stat(path, &stat_buf)) { 
        eke->gui.edje.theme = strdup(path);
        return 1;
    }

    /* look in the install dir */
    snprintf(path, PATH_MAX, PACKAGE_DATA_DIR "/themes/%s.eet", thm);
    if (!stat(path, &stat_buf)) {
        eke->gui.edje.theme = strdup(path);
        return 1;
    }
    return 0;
}


