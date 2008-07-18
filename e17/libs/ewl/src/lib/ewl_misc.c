/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include <locale.h>
#include "ewl_base.h"
#include "ewl_icon_theme.h"
#include "ewl_io_manager.h"
#include "ewl_text_context.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

#include <Evas.h>

#include <Edje.h>
#include <Efreet.h>
#include <Efreet_Mime.h>

#ifdef __GLIBC__
#include <execinfo.h>
#endif

extern Ecore_List *ewl_embed_list;
extern Ecore_List *ewl_window_list;

/*
 * Configuration and option related flags.
 */
static Ecore_Idle_Enterer *idle_enterer = NULL;
static Ecore_Idler *ewl_garbage_collect = NULL;
static int ewl_init_count = 0;

/*
 * store a list of shutdown functions to call
 */
static Ecore_List *shutdown_queue = NULL;

/*
 * Queues for scheduling various actions.
 */
static Ecore_List *obscure_list = NULL;
static Ecore_List *reveal_list = NULL;
static Ecore_List *realize_list = NULL;
static Ecore_List *destroy_list = NULL;
static Ecore_List *child_add_list= NULL;

/*
 * Default size per-buffer in the configure queue management.
 */
#define EWL_CONFIGURE_QUEUE_SIZE 4092

typedef struct Ewl_Configure_Queue Ewl_Configure_Queue;

/**
 * @brief Memory buffer for tracking widgets ready for a configure pass
 */
struct Ewl_Configure_Queue
{
        int end;
        Ewl_Widget *buffer[EWL_CONFIGURE_QUEUE_SIZE];
};

/*
 * Lists of active and inactive buffers
 * FIXME: Need to occassionally reduce the inactive buffer list
 */
Ecore_List *configure_active = NULL;
Ecore_List *configure_available = NULL;

/*
 * Lists for cleaning up evas related memory at convenient times.
 */
static Ecore_List *free_evas_list = NULL;
static Ecore_List *free_evas_object_list = NULL;

static Ecore_List *shutdown_hooks = NULL;

static int ewl_idle_render(void *data);
static void ewl_init_parse_options(int *argc, char **argv);
static void ewl_init_remove_option(int *argc, char **argv, int i);
static void ewl_configure_queue_run(void);
static void ewl_realize_queue(void);
static int ewl_garbage_collect_idler(void *data);
static void ewl_configure_cancel_request(Ewl_Widget *w);

/**
 * @return Returns no value.
 * @brief This is used by debugging macros for breakpoints
 *
 * Set a breakpoint at this function in order to retrieve backtraces from
 * warning messages.
 */
void
ewl_print_warning(void)
{
        fprintf(stderr, "\n***** Ewl Developer Warning ***** :\n"
                " To find where this is occurring set a breakpoint\n"
                " for the function ewl_print_warning.\n");
}

/**
 * @return Returns no value.
 * @brief This will cause EWL to SEGV. (Handy for debugging)
 */
void
ewl_segv(void)
{
        if (ewl_config_cache.segv) {
                char *null = NULL;
                *null = '\0';
        }
}

/**
 * @returns Returns no value.
 * @brief This will print a backtrace at the given point.
 */
void
ewl_backtrace(void)
{
#ifdef __GLIBC__
        void *array[128];
        size_t size;
        char **strings;
        size_t i;

        if (!ewl_config_cache.backtrace)
                return;

        fprintf(stderr, "\n***** Backtrace *****\n");
        size = backtrace(array, 128);
        strings = backtrace_symbols(array, size);
        for (i = 0; i < size; i++)
                fprintf(stderr, "%s\n", strings[i]);

        FREE(strings);
#endif
}

/**
 * @param ptr: the pointer to cast
 * @return The integer value of the pointer
 *
 * This function casts a pointer into an integer. If there should be an
 * information lost, i.e. the content of the pointer does not fit into
 * an integer, it will print a runtime warning.
 */
int
ewl_cast_pointer_to_integer(void *ptr)
{
        const unsigned int imask = ~0;
        const unsigned long int mask = ~((unsigned long int) imask);

        DENTER_FUNCTION(DLEVEL_STABLE);

        if (((unsigned long) ptr) & mask)
                DWARNING("Information lost while casting a pointer to an int");

        DRETURN_INT((int)((long int)ptr), DLEVEL_STABLE);
}

/**
 * @param argc: the argc passed into the main function
 * @param argv: the argv passed into the main function
 * @return Returns 1 or greater on success, 0 otherwise.
 * @brief Initialize the internal variables of ewl to begin the program
 *
 * Sets up necessary internal variables for executing ewl
 * functions. This should be called before any other ewl functions are used.
 */
int
ewl_init(int *argc, char **argv)
{
        const char *locale;

        DENTER_FUNCTION(DLEVEL_STABLE);

        /* check if we are already initialized */
        if (++ewl_init_count > 1)
                DRETURN_INT(ewl_init_count, DLEVEL_STABLE);

        /* set the locale for string collation if it isn't already set */
        locale = setlocale(LC_COLLATE, NULL);
        if (strcmp(locale, "C") || strcmp(locale, "POSIX")) {
                setlocale(LC_COLLATE, "");
        }

        shutdown_queue = ecore_list_new();
        if (!shutdown_queue) {
                fprintf(stderr, "Could not create Ewl shutdown queue.\n");
                goto ERROR;
        }

        if (!evas_init()) {
                fprintf(stderr, "Could not initialize Evas.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, evas_shutdown);

        if (!ecore_init()) {
                fprintf(stderr, "Could not initialize Ecore.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ecore_shutdown);

        if (!efreet_init()) {
                fprintf(stderr, "Could not initialize Efreet.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, efreet_shutdown);

        if (!efreet_mime_init()) {
                fprintf(stderr, "Could not initialize Efreet_Mime.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, efreet_mime_shutdown);

        if (!ecore_string_init()) {
                fprintf(stderr, "Could not initialize Ecore Strings.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ecore_string_shutdown);

        if (!edje_init()) {
                fprintf(stderr, "Could not initialize Edje.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, edje_shutdown);

        reveal_list = ecore_list_new();
        obscure_list = ecore_list_new();
        configure_active = ecore_list_new();
        configure_available = ecore_list_new();
        realize_list = ecore_list_new();
        destroy_list = ecore_list_new();
        free_evas_list = ecore_list_new();
        free_evas_object_list = ecore_list_new();
        child_add_list = ecore_list_new();
        ewl_embed_list = ecore_list_new();
        ewl_window_list = ecore_list_new();
        shutdown_hooks = ecore_list_new();
        if ((!reveal_list) || (!obscure_list) || (!configure_active)
                        || (!configure_available)
                        || (!realize_list) || (!destroy_list)
                        || (!free_evas_list) || (!free_evas_object_list)
                        || (!child_add_list) || (!ewl_embed_list)
                        || (!ewl_window_list) || (!shutdown_hooks)) {
                fprintf(stderr, "Unable to initialize internal configuration."
                                " Out of memory?\n");
                goto ERROR;
        }

        /*
         * Cleanup the queue buffers when the management lists get freed.
         */
        ecore_list_free_cb_set(configure_active, free);
        ecore_list_free_cb_set(configure_available, free);

        if (!ewl_config_init()) {
                fprintf(stderr, "Could not initialize Ewl Config.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ewl_config_shutdown);

        if (!ewl_engines_init()) {
                fprintf(stderr, "Could not intialize Ewl Engines.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ewl_engines_shutdown);

        /* handle any command line options */
        ewl_init_parse_options(argc, argv);

        /* initialize this _after_ we've handled the command line options */
        ewl_config_cache_init();

        /* we create the engine we will be working with here so that it is
         * initialized before we start to use it. */
        if (!ewl_engine_new(ewl_config_string_get(ewl_config,
                                        EWL_CONFIG_ENGINE_NAME), argc, argv)) {
                fprintf(stderr, "Could not initialize Ewl Engine.\n");
                goto ERROR;
        }

        if (!ewl_callbacks_init()) {
                fprintf(stderr, "Could not initialize Ewl Callback system.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ewl_callbacks_shutdown);

        /* allocate the two window callbacks */
        EWL_CALLBACK_EXPOSE = ewl_callback_type_add();
        EWL_CALLBACK_DELETE_WINDOW = ewl_callback_type_add();

        if (!ewl_theme_init()) {
                fprintf(stderr, "Could not setup Ewl Theme system.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ewl_theme_shutdown);

        if (!ewl_icon_theme_init()) {
                fprintf(stderr, "Could not initialize Ewl Icon Theme system.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ewl_icon_theme_shutdown);

        if (!ewl_dnd_init()) {
                fprintf(stderr, "Could not initialize Ewl DND support.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ewl_dnd_shutdown);

        if (!ewl_io_manager_init()) {
                fprintf(stderr, "Could not initialize Ewl IO Manager.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ewl_io_manager_shutdown);

        if (!ewl_text_context_init()) {
                fprintf(stderr, "Could not initialize Ewl Text Context system.\n");
                goto ERROR;
        }
        ecore_list_prepend(shutdown_queue, ewl_text_context_shutdown);

        if (!(idle_enterer = ecore_idle_enterer_add(ewl_idle_render, NULL))) {
                fprintf(stderr, "Could not create Idle Enterer.\n");
                goto ERROR;
        }

        DRETURN_INT(ewl_init_count, DLEVEL_STABLE);

ERROR:
        ewl_shutdown();

        DRETURN_INT(ewl_init_count, DLEVEL_STABLE);
}

/**
 * @brief Cleanup internal data structures used by ewl.
 *
 * This should be called to cleanup internal EWL data structures, if using
 * ecore directly rather than using ewl_main().
 */
int
ewl_shutdown(void)
{
        Ewl_Shutdown_Hook hook;
        void (*shutdown)(void);

        DENTER_FUNCTION(DLEVEL_STABLE);

        if (--ewl_init_count)
                DRETURN_INT(ewl_init_count, DLEVEL_STABLE);

        while ((hook = ecore_list_first_remove(shutdown_hooks)))
                hook();
        IF_FREE_LIST(shutdown_hooks);

        /*
         * Destroy all existing widgets.
         */
        if (ewl_embed_list)
        {
                Ewl_Widget *emb;

                while ((emb = ecore_list_first_remove(ewl_embed_list)))
                        ewl_widget_destroy(emb);

                while (ewl_garbage_collect_idler(NULL) > 0)
                        ;
                IF_FREE_LIST(ewl_embed_list);
        }

        if (idle_enterer)
        {
                ecore_idle_enterer_del(idle_enterer);
                idle_enterer = NULL;
        }

        /*
         * Free internal accounting lists.
         */
        IF_FREE_LIST(ewl_window_list);
        IF_FREE_LIST(reveal_list);
        IF_FREE_LIST(obscure_list);
        IF_FREE_LIST(configure_active);
        IF_FREE_LIST(configure_available);
        IF_FREE_LIST(realize_list);
        IF_FREE_LIST(destroy_list);
        IF_FREE_LIST(free_evas_list);
        IF_FREE_LIST(free_evas_object_list);
        IF_FREE_LIST(child_add_list);

        /* shutdown all the subsystems */
        while ((shutdown = ecore_list_first_remove(shutdown_queue)))
                shutdown();
        IF_FREE_LIST(shutdown_queue);

        DRETURN_INT(ewl_init_count, DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief The main execution loop of EWL
 *
 * This is the  main execution loop of ewl. It dispatches
 * incoming events and renders updates to the evas's used by ewl.
 */
void
ewl_main(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        ecore_main_loop_begin();
        ewl_shutdown();

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief One iteration of the main execution loop of EWL
 *
 * This is one iteration of the main execution loop of ewl. It dispatches
 * all of the current incoming events and renders updates on the queue to the evas's used by ewl.
 * Remember to clean up with ewl_shutdown () afterwards.
 */
void
ewl_main_iterate (void)
{
  ecore_main_loop_iterate ();
}


/**
 * @internal
 * @param data: this is only necessary for registering this function with ecore
 * @return Returns TRUE to continue the timer.
 * @brief Renders updates during idle times of the main loop
 *
 * Renders updates to the evas's during idle event times. Should not be called
 * publically unless a re-render is absolutely necessary.
 *
 * Overall Application workflow:
 * 1. Setup some widgets in the application.
 * 2. Enter the main loop.
 * 3. Realize widgets starting at the top working down.
 * 4. Show widgets starting at the bottom and working to the top notifying each
 *    parent of the childs size.
 * 5. Hide obscured widgets.
 * 6. Show revealed widgets.
 * 7. Move widgets around and size them.
 * 8. Render the display.
 * 9. Repeat steps 2-6 until program exits.
 */
static int
ewl_idle_render(void *data __UNUSED__)
{
        Ewl_Widget *w;
        Ewl_Embed  *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if (!ewl_embed_list) {
                DERROR("EWL has not been initialized. Exiting....\n");
                ewl_main_quit();
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        if (ecore_list_empty_is(ewl_embed_list))
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        /*
         * Freeze events on the evases to reduce overhead
         */
        ecore_list_first_goto(ewl_embed_list);
        while ((emb = ecore_list_next(ewl_embed_list)) != NULL)
                if (REALIZED(emb))
                        ewl_embed_freeze(emb);

        /*
         * Clean out the unused widgets first, to avoid them being drawn or
         * unnecessary work done from configuration. Then display new widgets,
         * finally layout the widgets.
         */
        if (!ecore_list_empty_is(destroy_list) ||
                        !ecore_list_empty_is(free_evas_list) ||
                        !ecore_list_empty_is(free_evas_object_list))
                ewl_garbage_collect = ecore_idler_add(ewl_garbage_collect_idler,
                                                      NULL);

        if (!ecore_list_empty_is(realize_list))
                ewl_realize_queue();

        while (!ecore_list_empty_is(configure_active)) {
                ewl_configure_queue_run();

                /*
                 * Reclaim obscured objects at this point
                 */
                while ((w = ecore_list_first_remove(obscure_list))) {
                        /*
                         * Ensure the widget is still obscured, then mark it
                         * revealed so that the obscure will succeed (and mark
                         * it obscured again.
                         */
                        if (REVEALED(w))
                                ewl_widget_obscure(w);
                }

                /*
                 * Allocate objects to revealed widgets.
                 */
                while ((w = ecore_list_first_remove(reveal_list))) {
                        /*
                         * Follow the same logic as the obscure loop.
                         */
                        if (!REVEALED(w))
                                ewl_widget_reveal(w);
                }
        }

        /*
         * Allow each embed to render itself, this requires thawing the evas.
         */
        ecore_list_first_goto(ewl_embed_list);
        while ((emb = ecore_list_next(ewl_embed_list)) != NULL) {
                ewl_embed_thaw(emb);

                if (REALIZED(emb)) {
                        double render_time = 0;

                        if (ewl_config_cache.evas_render) {
                                printf("Entering render\n");
                                render_time = ecore_time_get();
                        }

                        ewl_engine_canvas_render(emb);

                        if (ewl_config_cache.evas_render)
                                printf("Render time: %f seconds\n",
                                                ecore_time_get() - render_time);
                }
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Notifies ewl to quit at the end of this pass of the main loop
 *
 * Sets ewl to exit the main execution loop after this time
 * through the loop has been completed.
 */
void
ewl_main_quit(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        ecore_main_loop_quit();

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * ewl_init_parse_options - parse the options passed to ewl_init
 * @argc: the argc passed to the main function
 * @argv: the argv passed to the main function
 *
 * Returns no value. Parses the arguments of the program into sections that
 * ewl knows how to deal with.
 */
static void
ewl_init_parse_options(int *argc, char **argv)
{
        int i, matched = 0;
        Ecore_List *engines;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if (!argc || !argv)
                DRETURN(DLEVEL_STABLE);

        engines = ewl_engine_names_get();

        i = 0;
        while (i < *argc) {
                if (!strcmp(argv[i], "--ewl-segv")) {
                        ewl_config_int_set(ewl_config, EWL_CONFIG_DEBUG_SEGV,
                                                1, EWL_STATE_TRANSIENT);
                        matched++;
                }
                else if (!strcmp(argv[i], "--ewl-backtrace")) {
                        ewl_config_int_set(ewl_config,
                                        EWL_CONFIG_DEBUG_BACKTRACE, 1,
                                        EWL_STATE_TRANSIENT);
                        matched++;
                }
                else if (!strcmp(argv[i], "--ewl-theme")) {
                        if (i + 1 < *argc) {
                                ewl_config_string_set(ewl_config,
                                                        EWL_CONFIG_THEME_NAME,
                                                        argv[i + 1],
                                                        EWL_STATE_TRANSIENT);
                                matched++;
                        }
                        matched++;
                }
                else if (!strcmp(argv[i], "--ewl-print-theme-keys")) {
                        ewl_config_int_set(ewl_config,
                                        EWL_CONFIG_THEME_PRINT_KEYS, 1,
                                        EWL_STATE_TRANSIENT);
                        matched++;
                }
                else if (!strcmp(argv[i], "--ewl-print-theme-signals")) {
                        ewl_config_int_set(ewl_config,
                                        EWL_CONFIG_THEME_PRINT_SIGNALS, 1,
                                        EWL_STATE_TRANSIENT);
                        matched++;
                }
                else if (!strcmp(argv[i], "--ewl-print-gc-reap")) {
                        ewl_config_int_set(ewl_config,
                                        EWL_CONFIG_DEBUG_GC_REAP, 1,
                                        EWL_STATE_TRANSIENT);
                        matched++;
                }
                else if (!strcmp(argv[i], "--ewl-debug")) {
                        if ((i + 1) < *argc) {
                                ewl_config_int_set(ewl_config,
                                        EWL_CONFIG_DEBUG_LEVEL, atoi(argv[i + 1]),
                                        EWL_STATE_TRANSIENT);
                                matched++;
                        } else {
                                ewl_config_int_set(ewl_config,
                                        EWL_CONFIG_DEBUG_LEVEL, 1,
                                        EWL_STATE_TRANSIENT);
                        }
                        ewl_config_int_set(ewl_config,
                                        EWL_CONFIG_DEBUG_ENABLE, 1,
                                        EWL_STATE_TRANSIENT);
                        matched ++;
                }
                else if (!strcmp(argv[i], "--ewl-debug-paint")) {
                        ewl_config_int_set(ewl_config,
                                        EWL_CONFIG_DEBUG_EVAS_RENDER, 1,
                                        EWL_STATE_TRANSIENT);
                        matched ++;
                }
                else if (!strcmp(argv[i], "--ewl-help")) {
                        ewl_print_help();
                        matched ++;

                        /* this has to exit. otherwise we end up returning
                         * FALSE from ewl_init which triggers the app to
                         * spit out an error */
                        exit(0);
                }
                else if (!strncmp(argv[i], "--ewl-", 6)) {
                        unsigned int len;

                        len = strlen("--ewl-");
                        if (strlen(argv[i]) > len)
                        {
                                char *eng;
                                char *name;

                                eng = strdup(argv[i] + len);

                                while ((name = strchr(eng, '-')))
                                        *name = '_';

                                ecore_list_first_goto(engines);
                                while ((name = ecore_list_next(engines)))
                                {
                                        if (!strcmp(eng, name))
                                        {
                                                ewl_config_string_set(ewl_config,
                                                        EWL_CONFIG_ENGINE_NAME, name,
                                                        EWL_STATE_TRANSIENT);
                                                matched ++;

                                                break;
                                        }
                                }

                                FREE(eng);
                        }
                }

                if (matched > 0) {
                        while (matched) {
                                ewl_init_remove_option(argc, argv, i);
                                matched--;
                        }
                }
                else
                        i++;
        }
        IF_FREE_LIST(engines);

        DRETURN(DLEVEL_STABLE);
}

static void
ewl_init_remove_option(int *argc, char **argv, int i)
{
        int j;

        DENTER_FUNCTION(DLEVEL_STABLE);

        *argc = *argc - 1;
        for (j = i; j < *argc; j++)
                argv[j] = argv[j + 1];
        argv[j] = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value
 * @brief Print out the Ewl help text
 */
void
ewl_print_help(void)
{
        Ecore_List *names;
        char *name;

        DENTER_FUNCTION(DLEVEL_STABLE);

        printf("EWL Help\n"
                "\t--ewl-backtrace           Print a stack trace warnings occur.\n"
                "\t--ewl-debug <level>       Set the debugging printf level.\n"
                "\t--ewl-debug-paint         Enable repaint debugging.\n"
                "\t--ewl-help                Print this help message.\n"
                "\t--ewl-print-gc-reap       Print garbage collection stats.\n"
                "\t--ewl-print-theme-keys    Print theme keys matched widgets.\n"
                "\t--ewl-print-theme-signals Print theme keys matched widgets.\n"
                "\t--ewl-segv                Trigger crash when warning printed.\n"
                "\t--ewl-theme <theme>       Set the theme to use for widgets.\n"
                " AVAILABLE ENGINES\n");

        names = ewl_engine_names_get();
        while ((name = ecore_list_first_remove(names)))
        {
                char *t;
                while ((t = strchr(name, '_')))
                        *t = '-';

                printf("\t--ewl-%s\n", name);
                FREE(name);
        }
        IF_FREE_LIST(names);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to register for configuration
 * @return Returns no value.
 * @brief Ask for a widget to be configured during idle loop
 *
 * Ask for the widget @a w to be configured when the main idle loop is executed.
 */
void
ewl_configure_request(Ewl_Widget * w)
{
        Ewl_Embed *emb;
        Ewl_Widget *search;
        Ewl_Configure_Queue *queue_buffer;

        DENTER_FUNCTION(DLEVEL_TESTING);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!VISIBLE(w))
                DRETURN(DLEVEL_STABLE);

        /*
         * Widget scheduled for destruction, configuration, or is being called
         * within a configure callback.
         */
        if (ewl_widget_queued_has(w, (EWL_FLAG_QUEUED_SCHEDULED_DESTROY |
                                EWL_FLAG_QUEUED_SCHEDULED_CONFIGURE |
                                EWL_FLAG_QUEUED_PROCESS_CONFIGURE)))
                DRETURN(DLEVEL_STABLE);

        /*
         * Check for any parent scheduled for configuration, and look for the
         * top level widget in this branch.
         */
        search = w;
        while (search->parent) {
                search = search->parent;
                if (ewl_widget_queued_has(search,
                                        EWL_FLAG_QUEUED_SCHEDULED_CONFIGURE))
                        DRETURN(DLEVEL_TESTING);
        }

        /*
         * Verify top level widget is not queued for configure.
         */
        if (ewl_widget_queued_has(search, EWL_FLAG_QUEUED_SCHEDULED_CONFIGURE))
                DRETURN(DLEVEL_TESTING);

        /*
         * Stop processing if this widget doesn't have a valid embed parent.
         */
        if (!ewl_widget_toplevel_get(search))
                DRETURN(DLEVEL_STABLE);
        emb = EWL_EMBED(search);

        /*
         * No parent of this widget is queued so add it to the queue. All
         * children widgets should have been removed by this point.
         */
        ewl_widget_queued_add(w, EWL_FLAG_QUEUED_SCHEDULED_CONFIGURE);

        queue_buffer = ecore_list_last_goto(configure_active);

        /*
         * If the last buffer is full, it's not useful to us and we need a new
         * one.
         */
        if (queue_buffer) {
                if (queue_buffer->end >= EWL_CONFIGURE_QUEUE_SIZE)
                        queue_buffer = NULL;
        }

        if (!queue_buffer) {
                /*
                 * Attempt to use a previously allocated buffer first, fallback
                 * to allocating one.
                 */
                if (!ecore_list_empty_is(configure_available)) {
                        queue_buffer = ecore_list_first_remove(configure_available);
                }
                else {
                        queue_buffer = NEW(Ewl_Configure_Queue, 1);
                }
                ecore_list_append(configure_active, queue_buffer);
        }

        /*
         * Add the widget to the end of the queue.
         */
        if (queue_buffer)
                queue_buffer->buffer[queue_buffer->end++] = w;

        DLEAVE_FUNCTION(DLEVEL_TESTING);
}

static void
ewl_configure_queue_widget_run(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        if (ewl_widget_toplevel_get(EWL_OBJECT(w))) {
                ewl_object_size_request(EWL_OBJECT(w),
                                ewl_object_current_w_get(EWL_OBJECT(w)),
                                ewl_object_current_h_get(EWL_OBJECT(w)));
        }

        /*
         * Remove the flag that the widget is scheduled for
         * configuration.
         */
        ewl_widget_queued_remove(w, EWL_FLAG_QUEUED_SCHEDULED_CONFIGURE);

        /*
         * Items that are off screen should be queued to give up their
         * evas objects for reuse. Items returning from offscreen are
         * queued to receive new evas objects.
         */
        if (!ewl_widget_onscreen_is(w)) {
                if (REVEALED(w))
                        ecore_list_prepend(obscure_list, w);
        }
        else {
                if (!REVEALED(w))
                        ecore_list_prepend(reveal_list, w);

                ewl_widget_queued_add(w, EWL_FLAG_QUEUED_PROCESS_CONFIGURE);
                if (REALIZED(w) && VISIBLE(w) && REVEALED(w))
                        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);
                ewl_widget_queued_remove(w, EWL_FLAG_QUEUED_PROCESS_CONFIGURE);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value
 * @brief Configure all the widgets that need to be configured
 */
static void
ewl_configure_queue_run(void)
{
        Ewl_Configure_Queue *queue_buffer;

        DENTER_FUNCTION(DLEVEL_STABLE);

        /*
         * Configure any widgets that need it.
         */
        while ((queue_buffer = ecore_list_first_remove(configure_active))) {
                int i;
                for (i = 0; i < queue_buffer->end; i++) {
                        Ewl_Widget *w;

                        w = queue_buffer->buffer[i];
                        ewl_configure_queue_widget_run(w);
                }

                /*
                 * Add to the available list re-initialized.
                 */
                queue_buffer->end = 0;
                ecore_list_prepend(configure_available, queue_buffer);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget that no longer needs to be configured
 * @return Returns no value.
 * @brief Cancel a request to configure a widget
 *
 * Remove the widget @a w from the list of widgets that need to be configured.
 */
static void
ewl_configure_cancel_request(Ewl_Widget *w)
{
        Ewl_Configure_Queue *queue_buffer;
        DENTER_FUNCTION(DLEVEL_TESTING);

        ecore_list_first_goto(configure_active);
        while ((queue_buffer = ecore_list_next(configure_active))) {
                int i;
                for (i = 0; i < queue_buffer->end; i++) {
                        Ewl_Widget *tmp;

                        tmp = queue_buffer->buffer[i];
                        if (tmp == w) {
                                ewl_widget_queued_remove(w,
                                                         EWL_FLAG_QUEUED_SCHEDULED_CONFIGURE);
                                if (i < queue_buffer->end - 1)
                                        memmove(queue_buffer->buffer + i,
                                                queue_buffer->buffer + i + 1,
                                                (queue_buffer->end - i - 1)
                                                * sizeof(void *));

                                queue_buffer->end--;
                        }
                }
        }

        DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/**
 * @internal
 * @param w: widget to schedule for realization
 * @return Returns no value.
 * @brief Schedule a widget to be realized at idle time
 *
 * Places a widget on the queue to be realized at a later time.
 */
void
ewl_realize_request(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (ewl_widget_queued_has(w, EWL_FLAG_QUEUED_SCHEDULED_REVEAL))
                DRETURN(DLEVEL_STABLE);

        if (!ewl_widget_flags_get(w, EWL_FLAG_PROPERTY_TOPLEVEL)) {
                Ewl_Widget *p = w->parent;
                if (!p)
                        DRETURN(DLEVEL_STABLE);

                if (!ewl_widget_queued_has(p, EWL_FLAG_QUEUED_PROCESS_REVEAL)) {
                        if (!REALIZED(p))
                                DRETURN(DLEVEL_STABLE);
                }
        }

        ewl_widget_queued_add(w, EWL_FLAG_QUEUED_SCHEDULED_REVEAL);
        ecore_list_append(realize_list, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget that no longer needs to be realized
 * @return Returns no value.
 * @brief Cancel a request to realize a widget
 *
 * Remove the widget @a w from the list of widgets that need to be realized.
 */
void
ewl_realize_cancel_request(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_TESTING);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ecore_list_goto(realize_list, w);
        if (ecore_list_current(realize_list) == w)
        {
                ewl_widget_queued_remove(w, EWL_FLAG_QUEUED_SCHEDULED_REVEAL);
                ecore_list_remove(realize_list);
        }

        DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/**
 * @internal
 * @return Returns no value
 * @brief Realize all widgets that need to be realized
 */
static void
ewl_realize_queue(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        /*
         * First realize any widgets that require it, this looping should
         * avoid deep recursion, and works from top to bottom since widgets
         * can't be placed on this list unless their parent has been realized
         * or they are a toplevel widget.
         */
        while ((w = ecore_list_first_remove(realize_list))) {
                if (VISIBLE(w) && !REALIZED(w)) {
                        ewl_widget_queued_add(w,
                                                EWL_FLAG_QUEUED_PROCESS_REVEAL);
                        ewl_widget_realize(EWL_WIDGET(w));
                        ewl_widget_queued_remove(w,
                                                EWL_FLAG_QUEUED_PROCESS_REVEAL);
                        ecore_list_prepend(child_add_list, w);
                }
        }

        /*
         * Work our way back up the chain of widgets to resize from bottom to
         * top.
         */
        while ((w = ecore_list_first_remove(child_add_list))) {
                /*
                 * Check visibility in case the realize callback changed it.
                 */
                if (VISIBLE(w))
                        ewl_callback_call(w, EWL_CALLBACK_SHOW);

                /*
                 * Give the top level widgets their initial preferred size.
                 */
                if (ewl_widget_flags_get(w,
                                         EWL_FLAG_PROPERTY_TOPLEVEL)) {
                        ewl_object_size_request(EWL_OBJECT(w),
                                ewl_object_current_w_get(EWL_OBJECT(w)),
                                ewl_object_current_h_get(EWL_OBJECT(w)));
                }

                /*
                 * Indicate widget no longer on the realize queue.
                 */
                ewl_widget_queued_remove(w, EWL_FLAG_QUEUED_SCHEDULED_REVEAL);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to destroy
 * @return Returns no value
 * @brief Queues the widget to be destroyed.
 *
 * NOTE you should be using ewl_widget_destroy instead of calling
 * ewl_destroy_request directly.
 */
void
ewl_destroy_request(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        if (DESTROYED(w))
                DRETURN(DLEVEL_STABLE);

        if (CONFIGURED(w))
                ewl_configure_cancel_request(w);

        ewl_widget_queued_add(w, EWL_FLAG_QUEUED_SCHEDULED_DESTROY);

        /*
         * Must prepend to ensure children are freed before parents.
         */
        ecore_list_prepend(destroy_list, w);

        /*
         * Schedule child widgets for destruction.
         */
        if (ewl_widget_recursive_get(w))
                ewl_container_destroy(EWL_CONTAINER(w));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param evas: evas to queue for destruction
 * @return Returns no value.
 * @brief Queues an evas to be destroyed at a later time.
 */
void
ewl_canvas_destroy(void *evas)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(evas);

        ecore_list_append(free_evas_list, evas);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param obj: evas object to queue for destruction
 * @return Returns no value.
 * @brief Queues an evas object to be destroyed at a later time.
 */
void
ewl_canvas_object_destroy(void *obj)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);

        ecore_list_append(free_evas_object_list, obj);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

#define EWL_GC_LIMIT 300

/**
 * @internal
 * @param data: Unused.
 * @return Returns TRUE if objects remain to be freed, otherwise false.
 * @brief Free's all widgets that have been marked for destruction.
 */
static int
ewl_garbage_collect_idler(void *data __UNUSED__)
{
        Evas *evas;
        Ewl_Widget *w;
        Evas_Object *obj;
        int cleanup;

        DENTER_FUNCTION(DLEVEL_STABLE);

        cleanup = 0;
        if (ewl_config_cache.gc_reap) printf("---\n");

        while ((cleanup < EWL_GC_LIMIT) &&
                        (w = ecore_list_first_remove(destroy_list))) {
                if (ewl_widget_queued_has(w,
                                          EWL_FLAG_QUEUED_SCHEDULED_CONFIGURE))
                        ewl_configure_cancel_request(w);

                ewl_callback_call(w, EWL_CALLBACK_DESTROY);
                ewl_callback_del_type(w, EWL_CALLBACK_DESTROY);
                ewl_widget_free(w);
                cleanup++;
        }

        if (ewl_config_cache.gc_reap)
                printf("Destroyed %d EWL objects\n", cleanup);
        cleanup = 0;

        while ((obj = ecore_list_first_remove(free_evas_object_list))) {
                evas_object_del(obj);
                cleanup++;
        }

        if (ewl_config_cache.gc_reap)
                printf("Destroyed %d Evas Objects\n", cleanup);
        cleanup = 0;

        /* make sure the widget and object lists are clear before trying to
         * remove the evas canvas */
        if ((ecore_list_count(free_evas_object_list) == 0)
                        && (ecore_list_count(destroy_list) == 0)) {
                while ((evas = ecore_list_first_remove(free_evas_list))) {
                        evas_free(evas);
                        cleanup++;
                }
        }

        if (ewl_config_cache.gc_reap)
                printf("Destroyed %d Evas\n---\n", cleanup);

        /* We set the ewl_garbage_collect to NULL because when we return 0
         * (because destroy_list is empty) ecore will cleanup the idler
         * memory for us. */
        if (!ecore_list_count(destroy_list))
                ewl_garbage_collect = NULL;

        DRETURN_INT(ecore_list_count(destroy_list), DLEVEL_STABLE);
}

#ifdef DEBUG_MALLOCDEBUG
char *
strdup(const char *str)
{
        char *dst;

        DENTER_FUNCTION(DLEVEL_STABLE);

        dst = malloc(strlen(str) + 1);
        if (dst) strcpy(dst, str);

        DRETURN_PTR(dst, DLEVEL_STABLE);
}
#endif

/**
 * @param mod_dir: do we add or remove from the indent
 * @return Returns a string with a number of spaces equal to the current
 * debug level
 * @brief Creates a string used to indent debug messages
 */
void
ewl_debug_indent_print(int mod_dir)
{
        int indent;
        static int ewl_debug_indent_lvl = 0;

        if (mod_dir < 0) ewl_debug_indent_lvl --;

        if (ewl_debug_indent_lvl < 0)
                ewl_debug_indent_lvl = 0;

        for (indent = 0; indent < (ewl_debug_indent_lvl << 1) + 2; indent++)
                fputc(' ', stderr);

        if (mod_dir > 0) ewl_debug_indent_lvl ++;
}

/**
 * @param hook: The shutdown callback to call
 * @return Returns no value
 * @brief Allows a widget to hookup a shutdown callback that will be called
 * when Ewl is shutdown.
 */
void
ewl_shutdown_add(Ewl_Shutdown_Hook hook)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(hook);

        ecore_list_prepend(shutdown_hooks, hook);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


