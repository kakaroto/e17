
#include <Ewl.h>

int             ewl_idle_render(void *data);

char           *xdisplay = NULL;
extern Ewd_List *ewl_window_list;
static Ewd_List *configure_list = NULL;

void            __ewl_init_parse_options(int argc, char **argv);
void            __ewl_parse_option_array(int argc, char **argv);
static int      ewl_reread_config(void *data);

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

	ecore_init();
	if (!ecore_x_init(xdisplay)) {
		fprintf(stderr, "ERRR: Cannot connect to X display!\n");
		exit(-1);
	}

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
	ecore_idle_enterer_add(ewl_idle_render, NULL);

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

	/*
	 * Call it once right away, then get it looping every half second
	 */
	ewl_reread_config(NULL);
	ecore_timer_add(0.5, ewl_reread_config, NULL);

	ewl_idle_render(NULL);
	ecore_main_loop_begin();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_idle_render - renders updates during idle times of the main loop
 * @data: this is only necessary for registering this function with ecore
 *
 * Returns TRUE to continue the timer. Renders updates to the evas's during idle
 * event times.
 */
int ewl_idle_render(void *data)
{
	Ewl_Widget     *w;
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_window_list) {
		DERROR("EWL has not been initialized. Exiting....\n");
		exit(-1);
	}

	if (ewd_list_is_empty(ewl_window_list))
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	/*
	 * Configure any widgets that need it.
	 */
	while ((w = ewd_list_remove_first(configure_list))) {
		/*
		 * Remove the flag that the widget is scheduled for
		 * configuration.
		 */
		w->flags &= ~EWL_FLAGS_CSCHEDULED;
		ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);
	}

	/*
	 * Rerender each window and realize them as necessary. Done after
	 * configuration to avoid multiple passes of the window list when no
	 * new windows have been created.
	 */
	ewd_list_goto_first(ewl_window_list);
	while ((win = EWL_WINDOW(ewd_list_next(ewl_window_list))) != NULL) {
		/*
		 * If we have any unrealized windows at this point, we want to
		 * realize and configure them to layout the children correct.
		 */
		if (VISIBLE(win) && !REALIZED(win)) {
			ewl_widget_realize(EWL_WIDGET(win));
			ewl_widget_configure(EWL_WIDGET(win));
		}

		if (win->evas)
			evas_render(win->evas);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
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

	ecore_main_loop_quit();
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
 * @data: dummy variable used for compatibility with ecore's timers
 *
 * Returns TRUE to keep the timer going. Sets up a timer loop for rereading the
 * config data.
 */
static int ewl_reread_config(void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_config_reread_and_apply();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
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
	static int longest = 0;
	Ewl_Window     *win;
	Ewl_Widget     *search;

	DENTER_FUNCTION(DLEVEL_TESTING);
	DCHECK_PARAM_PTR("w", w);

	win = ewl_window_find_window_by_widget(w);

	/*
	 * We don't need to configure if it's outside the viewable space in
	 * it's parent widget.
	 */
	if (w->parent) {
		int x, y;
		unsigned int width, height;
		Ewl_Widget *p = w->parent;

		ewl_object_get_current_geometry(EWL_OBJECT(w), &x, &y, &width,
				&height);
		if ((int)(x + width) < CURRENT_X(p) ||
				x > (int)(CURRENT_X(p) + CURRENT_W(p)) ||
				(int)(y + height) < CURRENT_Y(p) ||
				y > (int)(CURRENT_Y(p) + CURRENT_H(p)) ||
				(int)(x + width) < CURRENT_X(win) ||
				x > (int)(CURRENT_X(win) + CURRENT_W(win)) ||
				(int)(y + height) < CURRENT_Y(win) ||
				y > (int)(CURRENT_Y(win) + CURRENT_H(win))) {
			w->flags |= EWL_FLAGS_OBSCURED;
			if (w->fx_clip_box)
				evas_object_hide(w->fx_clip_box);
		}
		else {
			w->flags &= ~EWL_FLAGS_OBSCURED;
			if (w->fx_clip_box)
				evas_object_show(w->fx_clip_box);
		}
	}

	/*
	 * Check this first, and remove an obscured widget from the configure
	 * list, if it's already scheduled.
	 */
	if (w->flags & EWL_FLAGS_OBSCURED) {
		if (w->flags & EWL_FLAGS_CSCHEDULED) {
			ewd_list_goto_first(configure_list);
			while ((search = ewd_list_current(configure_list))) {
				if (search == w) {
					w->flags &= ~EWL_FLAGS_CSCHEDULED;
					ewd_list_remove(configure_list);
					break;
				}
				ewd_list_next(configure_list);
			}
		}

		DRETURN(DLEVEL_TESTING);
	}

	/*
	 * Easy case, we know this is on the list already.
	 */
	if (w->flags & EWL_FLAGS_CSCHEDULED)
		DRETURN(DLEVEL_TESTING);

	/*
	 * Need to check if the window that holds w is scheduled for
	 * configuration. This is the easiest way to test if we can avoid
	 * adding this widget to the configuration list.
	 */
	if (EWL_WIDGET(win)->flags & EWL_FLAGS_CSCHEDULED)
		DRETURN(DLEVEL_TESTING);

	/*
	 * Check for any parent scheduled for configuration.
	 */
	search = w;
	while ((search = search->parent)) {
		if (search->flags & EWL_FLAGS_CSCHEDULED)
			DRETURN(DLEVEL_TESTING);
	}

	/*
	 * No parent of this widget is queued so add it to the queue. All
	 * children widgets should have been removed by this point.
	 */
	w->flags |= EWL_FLAGS_CSCHEDULED;
	ewd_list_append(configure_list, w);

	/*
	 * Now clean off any children of this widget, they will get added
	 * later.
	 */
	ewd_list_goto_first(configure_list);
	while ((search = ewd_list_current(configure_list))) {
		Ewl_Widget *parent;

		parent = search;
		while ((parent = parent->parent)) {
			if (parent == w) {
				search->flags &= ~EWL_FLAGS_CSCHEDULED;
				ewd_list_remove(configure_list);
				break;
			}
		}

		ewd_list_next(configure_list);
	}

	/*
	 * FIXME: Remove this once we get things stabilized a bit more.
	 */
	if (ewd_list_nodes(configure_list) > longest) {
		longest = ewd_list_nodes(configure_list);
		printf("SCHEDULING CONFIGURATION OF %d WIDGETS\n", longest);
	}

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/**
 * ewl_configure_cancel_request - cancel a request to configure a widget
 * @w: the widget that no longer needs to be configured
 *
 * Returns no value. Remove the widget @w from the list of widgets that need
 * to be configured.
 */
void ewl_configure_cancel_request(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_TESTING);

	ewd_list_goto(configure_list, w);

	if (ewd_list_current(configure_list) == w)
		ewd_list_remove(configure_list);

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}
