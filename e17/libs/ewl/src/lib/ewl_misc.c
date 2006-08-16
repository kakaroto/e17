#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

#ifdef __GLIBC__
#include <execinfo.h>
#endif

extern Ecore_List *ewl_embed_list;
extern Ecore_List *ewl_window_list;

/*
 * Configuration and option related flags.
 */
static unsigned int phase_status = 0;

static Ecore_Idle_Enterer *idle_enterer = NULL;
static Ecore_Idler *ewl_garbage_collect = NULL;
static int ewl_init_count = 0;

/*
 * Queues for scheduling various actions.
 */
static Ecore_List *obscure_list = NULL;
static Ecore_List *reveal_list = NULL;
static Ecore_List *configure_list = NULL;
static Ecore_List *realize_list = NULL;
static Ecore_List *destroy_list = NULL;
static Ecore_List *child_add_list= NULL;

/*
 * Lists for cleaning up evas related memory at convenient times.
 */
static Ecore_List *free_evas_list = NULL;
static Ecore_List *free_evas_object_list = NULL;

int ewl_idle_render(void *data);
static void ewl_init_parse_options(int *argc, char **argv);
static void ewl_init_remove_option(int *argc, char **argv, int i);
int ewl_ecore_exit(void *data, int type, void *event);

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
	if (ewl_config.debug.segv) {
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
		
	if (!ewl_config.debug.backtrace) return;

	fprintf(stderr, "\n***** Backtrace *****\n");
	size = backtrace(array, 128);
	strings = backtrace_symbols(array, size);
	for (i = 0; i < size; i++)
		fprintf(stderr, "%s\n", strings[i]);

	FREE(strings);
#endif
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
	DENTER_FUNCTION(DLEVEL_STABLE);

	/* check if we are already initialized */
	if (++ewl_init_count > 1)
		DRETURN_INT(ewl_init_count, DLEVEL_STABLE);

	if (!evas_init()) {
		DERROR("Could not init evas....\n");
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ecore_init()) {
		DERROR("Could not init ecore....\n");
		evas_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ecore_desktop_init()) {
		DERROR("Could not init ecore_desktop...\n");
		evas_shutdown();
		ecore_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ecore_string_init()) {
		DERROR("Could not init ecore strings....\n");
		evas_shutdown();
		ecore_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ewl_config_init()) {
		DERROR("Could not init config data.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ewl_engines_init()) {
		DERROR("Could not init engine data.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	ewl_init_parse_options(argc, argv);

	/* we create the engine we will be working with here so that it is
	 * initialized before we start to use it. */
	if (!ewl_engine_new(ewl_config.engine_name))
	{
		DERROR("Unable to initialize engine.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!edje_init()) {
		DERROR("Could not init edje....\n");
		evas_shutdown();
		ecore_string_shutdown();
		ecore_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	/*
	 * Global freeze on edje events while edje's are being manipulated.
	 */
	edje_freeze();

	reveal_list = ecore_list_new();
	obscure_list = ecore_list_new();
	configure_list = ecore_list_new();
	realize_list = ecore_list_new();
	destroy_list = ecore_list_new();
	free_evas_list = ecore_list_new();
	free_evas_object_list = ecore_list_new();
	child_add_list = ecore_list_new();
	if ((!reveal_list) || (!obscure_list) || (!configure_list)
			|| (!realize_list) || (!destroy_list)
			|| (!free_evas_list) || (!free_evas_object_list)
			|| (!child_add_list)) {
		DERROR("Unable to create internal configuration, out of memory?\n");
		evas_shutdown();
		ecore_string_shutdown();
		ecore_shutdown();
		edje_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ewl_dnd_init()) {
		DERROR("Count not init dnd.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ewl_ev_init()) {
		DERROR("Could not init event data.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ewl_callbacks_init()) {
		DERROR("Could not init callback system.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ewl_theme_init()) {
		DERROR("Could not setup theme system.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!(ewl_embed_list = ecore_list_new())) {
		DERROR("Could not allocate embed list, out of memory?\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!(ewl_window_list = ecore_list_new())) {
		DERROR("Could not allocate window list, out of memory?\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!(idle_enterer = ecore_idle_enterer_add(ewl_idle_render, NULL))) {
		DERROR("Could not create idle enterer.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

	if (!ewl_text_context_init()) {
		DERROR("Could not init text context system.\n");
		ewl_shutdown();
		DRETURN_INT(--ewl_init_count, DLEVEL_STABLE);
	}

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
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (--ewl_init_count)
		DRETURN_INT(ewl_init_count, DLEVEL_STABLE);
	/*
	 * Destroy all existing widgets.
	 */
	if (ewl_embed_list) {
		Ewl_Widget *emb;

		while ((emb = ecore_list_remove_first(ewl_embed_list)))
			ewl_widget_destroy(emb);
		while (ewl_garbage_collect_idler(NULL) > 0);
		ecore_list_destroy(ewl_embed_list);
		ewl_embed_list = NULL;
	}

	ewl_text_context_shutdown();

	if (idle_enterer) {
		ecore_idle_enterer_del(idle_enterer);
		idle_enterer = NULL;
	}

	/*
	 * Shut down the various EWL subsystems cleanly.
	 */
	ewl_embed_shutdown();
	ewl_callbacks_shutdown();
	ewl_theme_shutdown();
	ewl_engines_shutdown();
	ewl_config_shutdown();
	ewl_dnd_shutdown();

	/*
	 * Free internal accounting lists.
	 */
	if (ewl_window_list) {
		ecore_list_destroy(ewl_window_list);
		ewl_window_list = NULL;
	}
	ecore_list_destroy(reveal_list);
	reveal_list = NULL;
	ecore_list_destroy(obscure_list);
	obscure_list = NULL;
	ecore_list_destroy(configure_list);
	configure_list = NULL;
	ecore_list_destroy(realize_list);
	realize_list = NULL;
	ecore_list_destroy(destroy_list);
	destroy_list = NULL;
	ecore_list_destroy(free_evas_list);
	free_evas_list = NULL;
	ecore_list_destroy(free_evas_object_list);
	free_evas_object_list = NULL;
	ecore_list_destroy(child_add_list);
	child_add_list = NULL;

	edje_shutdown();
	evas_shutdown();

	ecore_string_shutdown();
	ecore_desktop_shutdown();
	ecore_shutdown();

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
int
ewl_idle_render(void *data __UNUSED__)
{
	Ewl_Widget *w;
	Ewl_Embed  *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_embed_list) {
		DERROR("EWL has not been initialized. Exiting....\n");
		exit(-1);
	}

	if (ecore_list_is_empty(ewl_embed_list))
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	/*
	 * Freeze events on the evases to reduce overhead
	 */
	ecore_list_goto_first(ewl_embed_list);
	while ((emb = ecore_list_next(ewl_embed_list)) != NULL) {
		ewl_embed_freeze(emb);
	}

	/*
	 * Clean out the unused widgets first, to avoid them being drawn or
	 * unnecessary work done from configuration. Then display new widgets,
	 * finally layout the widgets.
	 */
	if (!ecore_list_is_empty(destroy_list) ||
			!ecore_list_is_empty(free_evas_list) ||
			!ecore_list_is_empty(free_evas_object_list))
		ewl_garbage_collect = ecore_idler_add(ewl_garbage_collect_idler,
						      NULL);;

	if (!ecore_list_is_empty(realize_list))
		ewl_realize_queue();

	while (!ecore_list_is_empty(configure_list)) {
		ewl_configure_queue();

		/*
		 * Reclaim obscured objects at this point
		 */
		while ((w = ecore_list_remove_first(obscure_list))) {
			/*
			 * Ensure the widget is still obscured, then mark it
			 * revealed so that the obscure will succeed (and mark
			 * it obscured again.
			 */
			if (!OBSCURED(w)) {
				ewl_widget_obscure(w);
			}
		}

		/*
		 * Allocate objects to revealed widgets.
		 */
		while ((w = ecore_list_remove_first(reveal_list))) {
			/*
			 * Follow the same logic as the obscure loop.
			 */
			if (OBSCURED(w)) {
				ewl_widget_reveal(w);
			}
		}
	}

	/*
	 * Our work is done, allow edje events to be triggered.
	 */
	edje_thaw();

	/*
	 * Allow each embed to render itself, this requires thawing the evas.
	 */
	ecore_list_goto_first(ewl_embed_list);
	while ((emb = ecore_list_next(ewl_embed_list)) != NULL) {
		if (REALIZED(emb) && emb->evas) {
			double render_time = 0;

			ewl_embed_thaw(emb);
			if (ewl_config.evas.render_debug) {
				printf("Entering render\n");
				render_time = ecore_time_get();
			}

			ewl_engine_canvas_render(emb);

			if (ewl_config.evas.render_debug) {
				printf("Render time: %f seconds\n",
						ecore_time_get() - render_time);
			}
		}
	}

	/*
	 * Global freeze on edje events while edje's are being manipulated.
	 */
	edje_freeze();

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
	int i;
	int matched = 0;
	Ecore_List *engines;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!argc || !argv)
		DRETURN(DLEVEL_STABLE);

	engines = ewl_engine_names_get();

	i = 0;
	while (i < *argc) {
		if (!strcmp(argv[i], "--ewl-segv")) {
			ewl_config.debug.segv = 1;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-backtrace")) {
			ewl_config.debug.backtrace = 1;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-theme")) {
			if (i + 1 < *argc) {
				ewl_theme_name_set(argv[i + 1]);
				matched++;
			}
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-print-theme-keys")) {
			ewl_config.theme.print_keys = 1;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-print-theme-signals")) {
			ewl_config.theme.print_signals = 1;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-print-gc-reap")) {
			ewl_config.debug.gc_reap = 1;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-debug")) {
			if (i + i < *argc) {
				ewl_config.debug.level = atoi(argv[i + 1]);
				matched++;
			} else {
				ewl_config.debug.level = 1;
			}
			ewl_config.debug.enable = 1;
			matched ++;
		}
		else if (!strcmp(argv[i], "--ewl-debug-paint")) {
			ewl_config.evas.render_debug = 1;
			matched ++;
		}
		else if (!strcmp(argv[i], "--ewl-help")) {
			ewl_print_help();
			ecore_list_destroy(engines);
			exit(0);
			matched ++;
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

				ecore_list_goto_first(engines);
				while ((name = ecore_list_next(engines)))
				{
					if (!strcmp(eng, name))
					{
						IF_FREE(ewl_config.engine_name);
						ewl_config.engine_name = strdup(name);
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
	ecore_list_destroy(engines);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
	ecore_list_goto_first(names);
	while ((name = ecore_list_next(names)))
	{
		char *t;
		while ((t = strchr(name, '_')))
			*t = '-';

		printf("\t--ewl-%s\n", name);
		FREE(name);
	}
	ecore_list_destroy(names);
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

	DENTER_FUNCTION(DLEVEL_TESTING);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!VISIBLE(w))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Widget scheduled for destruction
	 */
	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Widget already scheduled for configure
	 */
	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_CSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Widget already in configure callback
	 */
	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_CPROCESS))
		DRETURN(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);
	if (!emb)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Check for any parent scheduled for configuration.
	 */
	search = w;
	while ((search = search->parent)) {
		if (ewl_object_queued_has(EWL_OBJECT(search),
					EWL_FLAG_QUEUED_CSCHEDULED))
			DRETURN(DLEVEL_TESTING);
	}

	/*
	 * No parent of this widget is queued so add it to the queue. All
	 * children widgets should have been removed by this point.
	 */
	ewl_object_queued_add(EWL_OBJECT(w), EWL_FLAG_QUEUED_CSCHEDULED);
	ecore_list_append(configure_list, w);

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/**
 * @internal
 * @return Returns no value
 * @brief Configure all the widgets that need to be configured
 */
void
ewl_configure_queue(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Configure any widgets that need it.
	 */
	while ((w = ecore_list_remove_first(configure_list))) {
		if (ewl_object_flags_get(EWL_OBJECT(w),
					 EWL_FLAG_PROPERTY_TOPLEVEL)) {
			ewl_object_size_request(EWL_OBJECT(w),
					ewl_object_current_w_get(EWL_OBJECT(w)),
					ewl_object_current_h_get(EWL_OBJECT(w)));
		}

		/*
		 * Remove the flag that the widget is scheduled for
		 * configuration.
		 */
		ewl_object_queued_remove(EWL_OBJECT(w),
				EWL_FLAG_QUEUED_CSCHEDULED);

		/*
		 * Items that are off screen should be queued to give up their
		 * evas objects for reuse. Items returning from offscreen are
		 * queued to receive new evas objects.
		 */
		if (!ewl_widget_onscreen_is(w)) {
			if (!OBSCURED(w))
				ecore_list_prepend(obscure_list, w);
		}
		else {
			if (OBSCURED(w))
				ecore_list_prepend(reveal_list, w);

			ewl_object_queued_add(EWL_OBJECT(w),
				EWL_FLAG_QUEUED_CPROCESS);
			if (REALIZED(w) && VISIBLE(w) && !OBSCURED(w))
				ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);
			ewl_object_queued_remove(EWL_OBJECT(w),
				EWL_FLAG_QUEUED_CPROCESS);
		}
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
void
ewl_configure_cancel_request(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_TESTING);

	ecore_list_goto(configure_list, w);
	if (ecore_list_current(configure_list) == w)
		ecore_list_remove(configure_list);

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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	if (!ewl_object_flags_get(EWL_OBJECT(w), EWL_FLAG_PROPERTY_TOPLEVEL)) {
		Ewl_Object *o = EWL_OBJECT(w->parent);
		if (!o)
			DRETURN(DLEVEL_STABLE);

		if (!ewl_object_queued_has(EWL_OBJECT(o), 
				EWL_FLAG_QUEUED_RPROCESS)) {
			if (!REALIZED(o))
				DRETURN(DLEVEL_STABLE);
		}
	}

	ewl_object_queued_add(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED);
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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ecore_list_goto(realize_list, w);
	if (ecore_list_current(realize_list) == w)
	{
		ewl_object_queued_remove(EWL_OBJECT(w),
					 EWL_FLAG_QUEUED_RSCHEDULED);
		ecore_list_remove(realize_list);
	}

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/**
 * @internal
 * @return Returns no value
 * @brief Realize all widgets that need to be realized
 */
void
ewl_realize_queue(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_realize_phase_enter();

	/*
	 * First realize any widgets that require it, this looping should
	 * avoid deep recursion, and works from top to bottom since widgets
	 * can't be placed on this list unless their parent has been realized
	 * or they are a toplevel widget.
	 */
	ecore_list_goto_first(realize_list);
	while ((w = ecore_list_remove_first(realize_list))) {
		if (VISIBLE(w) && !REALIZED(w)) {
			ewl_object_queued_add(EWL_OBJECT(w), 
						EWL_FLAG_QUEUED_RPROCESS);
			ewl_widget_realize(EWL_WIDGET(w));
			ewl_object_queued_remove(EWL_OBJECT(w), 
						EWL_FLAG_QUEUED_RPROCESS);
			ecore_list_prepend(child_add_list, w);
		}
	}

	/*
	 * Work our way back up the chain of widgets to resize from bottom to
	 * top.
	 */
	while ((w = ecore_list_remove_first(child_add_list))) {
		/*
		 * Check visibility in case the realize callback changed it.
		 */
		if (VISIBLE(w))
			ewl_callback_call(w, EWL_CALLBACK_SHOW);

		/*
		 * Give the top level widgets their initial preferred size.
		 */
		if (ewl_object_flags_get(EWL_OBJECT(w),
					 EWL_FLAG_PROPERTY_TOPLEVEL)) {
			ewl_object_size_request(EWL_OBJECT(w),
				ewl_object_current_w_get(EWL_OBJECT(w)),
				ewl_object_current_h_get(EWL_OBJECT(w)));
		}

		/*
		 * Indicate widget no longer on the realize queue.
		 */
		ewl_object_queued_remove(EWL_OBJECT(w),
					 EWL_FLAG_QUEUED_RSCHEDULED);
	}

	ewl_realize_phase_exit();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value.
 * @brief Marks that EWL is currently realizing a widget.
 */
void
ewl_realize_phase_enter(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	phase_status |= EWL_FLAG_QUEUED_RSCHEDULED;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value.
 * @brief Marks that EWL is not realizing a widget.
 */
void
ewl_realize_phase_exit(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	phase_status &= ~EWL_FLAG_QUEUED_RSCHEDULED;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns TRUE if currently realizing a widget, FALSE otherwise.
 * @brief Checks if EWL is currently in the process of realizing widgets.
 */
int
ewl_in_realize_phase(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT((phase_status & EWL_FLAG_QUEUED_RSCHEDULED), DLEVEL_STABLE);
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
	DCHECK_PARAM_PTR("w", w);

	if (DESTROYED(w))
		DRETURN(DLEVEL_STABLE);

	if (CONFIGURED(w))
		ewl_configure_cancel_request(w);

	ewl_object_queued_add(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED);

	/*
	 * Must prepend to ensure children are freed before parents.
	 */
	ecore_list_prepend(destroy_list, w);

	/*
	 * Schedule child widgets for destruction.
	 */
	if (ewl_object_recursive_get(EWL_OBJECT(w)))
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
ewl_evas_destroy(Evas *evas)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("evas", evas);

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
ewl_evas_object_destroy(Evas_Object *obj)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("obj", obj);

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
int
ewl_garbage_collect_idler(void *data __UNUSED__)
{
	Evas *evas;
	Ewl_Widget *w;
	Evas_Object *obj;
	int cleanup;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cleanup = 0;

	if (ewl_config.debug.gc_reap)
		printf("---\n");

	while ((cleanup < EWL_GC_LIMIT) &&
			(w = ecore_list_remove_first(destroy_list))) {
		if (ewl_object_queued_has(EWL_OBJECT(w),
					  EWL_FLAG_QUEUED_CSCHEDULED))
			ewl_configure_cancel_request(w);
	
		ewl_callback_call(w, EWL_CALLBACK_DESTROY);
		ewl_callback_del_type(w, EWL_CALLBACK_DESTROY);
		ewl_widget_free(w);
		cleanup++;
	}

	if (ewl_config.debug.gc_reap)
		printf("Destroyed %d EWL objects\n", cleanup);

	cleanup = 0;

	while ((obj = ecore_list_remove_first(free_evas_object_list))) {
		evas_object_del(obj);
		cleanup++;
	}
	if (ewl_config.debug.gc_reap)
		printf("Destroyed %d Evas Objects\n", cleanup);

	cleanup = 0;

	/* make sure the widget and object lists are clear before trying to
	 * remove the evas canvas */
	if ((ecore_list_nodes(free_evas_object_list) == 0)
			&& (ecore_list_nodes(destroy_list) == 0)) {
		while ((evas = ecore_list_remove_first(free_evas_list))) {
			evas_free(evas);
			cleanup++;
		}
	}

	if (ewl_config.debug.gc_reap)
		printf("Destroyed %d Evas\n", cleanup);

	if (ewl_config.debug.gc_reap)
		printf("---\n");

	if (!ecore_list_nodes(destroy_list))
		ewl_garbage_collect = NULL;

	DRETURN_INT(ecore_list_nodes(destroy_list), DLEVEL_STABLE);
}

int
ewl_ecore_exit(void *data __UNUSED__, int type __UNUSED__,
					void *event __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_main_quit();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

#ifdef DEBUG_MALLOCDEBUG
char *
strdup(const char *str)
{
	char *dst = malloc(strlen(str) + 1);
	if (dst)
		strcpy(dst, str);

	return dst;
}
#endif

/**
 * @return Returns a string with a number of spaces equal to the current
 * debug level
 * @brief Creates a string used to indent debug messages
 */
char *
ewl_debug_get_indent(void)
{
	char *indent = NULL;
	
	if (ewl_config.debug.indent_lvl < 0)
		ewl_config.debug.indent_lvl = 0;

	indent = calloc((ewl_config.debug.indent_lvl * 2) + 2, sizeof(char *)); 
	memset(indent, ' ', (ewl_config.debug.indent_lvl * 2) + 1);
	return indent;
}

