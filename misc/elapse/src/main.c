#include "Elapse.h"
#include "config.h"

int main(int argc, const char *argv[])
{
    Elapse elapse;
    int i = 0;

    elapse.conf.sticky = 1;
    elapse.conf.lower = 1;
    elapse.conf.below = 1;
    elapse.conf.alpha = TRANSPARENT;
    strncpy(elapse.conf.theme, PACKAGE_DATA_DIR "/themes/elapse.edj",
            PATH_MAX);
    elapse.conf.debug = DEBUG_LEVEL;
    elapse.conf.format = NULL;

    while (++i < argc) {
        if (!strcmp(argv[i], "-h") ||
                !strcmp(argv[i], "--help")) {
            show_help();
        }

        if (!strcmp(argv[i], "-q") ||
                !strcmp(argv[i], "--quiet")) {
            elapse.conf.debug = 1;
        }

        if (!strcmp(argv[i], "-v") ||
                !strcmp(argv[i], "--verbose")) {
            if (i < argc - 1) {
                int tmp = atoi(argv[i + 1]);
                if (tmp < 0 ) elapse.conf.debug = 1;
                else {
                    elapse.conf.debug = tmp;
                    i++;
                }
            }
        }

        if (!strcmp(argv[i], "-f") ||
                !strcmp(argv[i], "--format")) {
            if (++i == argc) show_help();
            elapse.conf.format = strdup(argv[i]);
        }

        if (!strcmp(argv[i], "-s") ||
                !strcmp(argv[i], "--not-sticky")) {
            elapse.conf.sticky = 0;
        }
        if (!strcmp(argv[i], "-b") ||
                !strcmp(argv[i], "--not-below")) {
            elapse.conf.below = 0;
        }

        if (!strcmp(argv[i], "-t") ||
                !strcmp(argv[i], "--theme")) {
            if (++i == argc) show_help();
            strncpy(elapse.conf.theme, argv[i], PATH_MAX);
        }
    }

    elapse_gui_init(&elapse, argc, argv);

    debug(DEBUG_INFO, &elapse, "ecore_timer_add()");
    elapse.timer = ecore_timer_add(1, (void *)elapse_time_set, &elapse);

    debug(DEBUG_INFO, &elapse, "ecore_main_loop_begin()");
    ecore_main_loop_begin();

    debug(DEBUG_INFO, &elapse, "freeing memory");
    free(elapse.conf.format);

    debug(DEBUG_INFO, &elapse, "exiting elapse");
    return 0;
}

void show_help(void)
{
    printf("Elapse - The Enlightened Clock\n");
    printf("By DigitalFallout\n");
    printf("=-=-=-=-=-=-=-=-=\n");
    printf("Runtime Options\n");
    printf("-f   --format\t\tSpecify a time format, see strftime(3)\n");
    printf("-h   --help\t\tPrint this help screen and exit\n");
    printf("-s   --not-sticky\tDisable sticky behaviour\n");
    printf("-b   --not-below\tDisable below behaviour\n");
    printf("-q   --quiet\t\tSurpress all output\n");
    printf("-t   --theme\t\tPath to the theme you wish to use\n");
    printf("-v   --verbose\t\tSet a new verbosity level (1-4)\n");
    exit(1);
}

