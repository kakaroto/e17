
#include <Ewl.h>

void            ewl_idle_render(void *data);

char           *xdisplay = NULL;
extern Ewd_List *ewl_window_list;
static Ewd_List *configure_list = NULL;

void            __ewl_init_parse_options(int argc, char **argv);
void            __ewl_parse_option_array(int argc, char **argv);
static void     ewl_reread_config(int val, void *data);

static int      __ewl_configure_check(Ewl_Widget * ppar, Ewl_Widget * w);

/**
 * ewl_print_warning - this is used by debugging macros for breakpoints
 *
 * Returns no value. Set a breakpoint at this function in order to retrieve
 * backtraces from warning messages.
 */
inline void ewl_print_warning()
{
	fprintf(stderr, "***** Ewl Developer Warning ***** :\n"
		" To find where this is occurring set a breakpoint\n"
		" for the function %s\n.", __FUNCTION__);
}

/**
 * ewl_init - initialize the internal variables of ewl to begin the program
 * @argc: the argc passed into the main function
 * @argv: the argv passed into the main function
 *
 * Returns no value. Sets up necessary internal variables for executing ewl
 * functions. This should be called before any other ewl functions are used.
 */
void ewl_init(int argc, char **argv)
{
	char           *xdisplay = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	configure_list = ewd_list_new();
	__ewl_init_parse_options(argc, argv);

	if (!ecore_display_init(xdisplay)) {
		fprintf(stderr, "ERRR: Cannot connect to X display!\n");
		exit(-1);
	}

	ecore_event_filter_init();
	ecore_event_signal_init();
	ecore_event_x_init();

	if (!ewl_config_init()) {
		DERROR("Couldn not init config data. Exiting....");
		exit(-1);
	}

	if (!ewl_ev_init()) {
		DERROR("Could not init event data. Exiting....");
		exit(-1);
	}

	ewl_callbacks_init();

	/*
	if (!ewl_fx_init()) {
		DERROR("Could not init fx data. Exiting....");
		exit(-1);
	}
	*/

	if (!ewl_theme_init()) {
		DERROR("Could not init theme data. Exiting....");
		exit(-1);
	}

	ewl_window_list = ewd_list_new();
	ecore_event_filter_idle_handler_add(ewl_idle_render, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_main - the main execution loop of ewl
 * 
 * Returns no value. This is the  main execution loop of ewl. It dispatches
 * incoming events and renders updates to the evas's used by ewl.
 */
void ewl_main(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_reread_config(0, NULL);

	ewl_idle_render(NULL);
	ecore_event_loop();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_idle_render - renders updates during idle times of the main loop
 * @data: this is only necessary for registering this function with ecore
 *
 * Returns no value. Renders updates to the evas's during idle event times.
 */
void ewl_idle_render(void *data)
{
	Ewl_Window     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_window_list) {
		DERROR("EWL has not been initialized. Exiting....\n");
		exit(-1);
	}

	if (ewd_list_is_empty(ewl_window_list))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Configure any widgets that need it.
	 */
	while ((w = ewd_list_remove_first(configure_list)))
		ewl_callback_call(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE);

	/*
	 * Rerender each window and realize them as necessary.
	 */
	ewd_list_goto_first(ewl_window_list);
	while ((w = EWL_WINDOW(ewd_list_next(ewl_window_list))) != NULL) {
		/*
		 * If we have any unrealized windows at this point, we want to
		 * realize and configure them to layout the children correct.
		 */
		if (VISIBLE(w) && !REALIZED(w)) {
			ewl_widget_realize(EWL_WIDGET(w));
			ewl_widget_configure(EWL_WIDGET(w));
		}

		if (w->evas)
			evas_render(w->evas);
	}

	DRETURN(DLEVEL_STABLE);
	data = NULL;
}

/**
 * ewl_main_quit - notifies ewl to quit at the end of this pass of the main loop
 *
 * Returns no value. Sets ewl to exit the main execution loop after this time
 * through the loop has been completed.
 */
void ewl_main_quit(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_event_loop_quit();
	ewl_callbacks_deinit();
	ewd_list_destroy(configure_list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * __ewl_init_parse_options - parse the options passed to ewl_init
 * @argc: the argc passed to the main function
 * @argv: the argv passed to the main function
 *
 * Returns no value. Parses the arguments of the program into sections that
 * ewl knows how to deal with.
 */
void __ewl_init_parse_options(int argc, char **argv)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	__ewl_parse_option_array(argc, argv);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * __ewl_parse_option_array - parses the argument arrays into options
 * @argc: the argc passed to the main function
 * @argv: the argv passed to the main function
 *
 * Returns no value. Parses the options passed to the main program and
 * processes any ewl related options.
 */
void __ewl_parse_option_array(int argc, char **argv)
{
	/*
	char            stropts[] =
	    "a:A:b:BcC:dD:e:f:Fg:hH:iIklL:mM:nNo:O:pPqQrR:sS:tT:uUvVwW:xXy:zZ1:2:3:4:56:78:90:";

	static struct option lopts[] = {
		{"ewl_display", 1, 0, '$'},
		{0, 0, 0, 0}
	};
	int             optch = 0, cmdx = 0;
	*/

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	while ((optch =
		 getopt_long_only(argc, argv, stropts, lopts, &cmdx)) != EOF) {
		switch (optch) {
		case 0:
			break;
		case '$':
			xdisplay = optarg;
			break;
		default:
			break;
		}
	}
	*/

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_reread_config - a timer function used to reread the config options
 * @val: dummy variable used for compatibility with ecore's timers
 * @data: dummy variable used for compatibility with ecore's timers
 *
 * Returns no value. Sets up a timer loop for rereading the config data.
 */
static void ewl_reread_config(int val, void *data)
{
	ewl_config_reread_and_apply();

	ecore_add_event_timer("reread_config_timer", 0.5,
			      ewl_reread_config, 0, NULL);
}

/**
 * ewl_configure_request - ask for a widget to be configured during idle loop
 * @w: the widget to register for configuration
 *
 * Returns no value. Ask for the widget @w to be configured when the main idle
 * loop is executed.
 */
void ewl_configure_request(Ewl_Widget * w)
{
	Ewl_Window     *win;
	Ewl_Widget     *search;

	DENTER_FUNCTION(DLEVEL_TESTING);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Need to check if the window that holds w is scheduled for
	 * configuration. This is the easiest way to test if we can avoid
	 * adding this widget to the configuration list.
	 */
	win = ewl_window_find_window_by_widget(w);

	ewd_list_goto_first(configure_list);
	while ((search = ewd_list_current(configure_list))) {

		/*
		 * Check the window.
		 */
		if (search == EWL_WIDGET(win))
			DRETURN(DLEVEL_TESTING);

		/*
		 * If a parent to this widget is already queued then we don't
		 * need to queue this widget.
		 */
		if (__ewl_configure_check(search, w))
			DRETURN(DLEVEL_TESTING);

		/*
		 * Now remove any widgets that are children of this widget
		 */
		if (__ewl_configure_check(w, search))
			ewd_list_remove(configure_list);

		ewd_list_next(configure_list);
	}

	/*
	 * No parent of this widget is queued so add it to the queue. All
	 * children widgets should have been removed by this point.
	 */
	ewd_list_append(configure_list, w);

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/*
 * Check to see if @ppar is a parent to @w at some level.
 */
static int __ewl_configure_check(Ewl_Widget * ppar, Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_TESTING);

	while (w) {
		if (w == ppar)
			DRETURN_INT(TRUE, DLEVEL_TESTING);
		w = w->parent;
	}

	DRETURN_INT(FALSE, DLEVEL_TESTING);
}
