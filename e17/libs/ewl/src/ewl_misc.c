
#include <Ewl.h>

int             ewl_idle_render(void *data);

extern Ewd_List *ewl_embed_list;

unsigned int    phase_status = 0;
char           *xdisplay = NULL;
Ewd_List *configure_list = NULL;
Ewd_List *realize_list = NULL;
Ewd_List *destroy_list = NULL;

Ewd_List *free_evas_list = NULL;
Ewd_List *free_evas_object_list = NULL;

Ewd_List *child_add_list= NULL;

void            __ewl_init_parse_options(int argc, char **argv);
void            __ewl_parse_option_array(int argc, char **argv);
int             __ewl_ecore_exit(void *data, int type, void *event);
static int      ewl_reread_config(void *data);

/**
 * @return Returns no value.
 * @brief This is used by debugging macros for breakpoints
 *
 * Set a breakpoint at this function in order to retrieve backtraces from
 * warning messages.
 */
inline void ewl_print_warning()
{
	fprintf(stderr, "***** Ewl Developer Warning ***** :\n"
		" To find where this is occurring set a breakpoint\n"
		" for the function %s\n.", __FUNCTION__);
}

/**
 * @param argc: the argc passed into the main function
 * @param argv: the argv passed into the main function
 * @return Returns no value.
 * @brief Initialize the internal variables of ewl to begin the program
 *
 * Sets up necessary internal variables for executing ewl
 * functions. This should be called before any other ewl functions are used.
 */
void ewl_init(int argc, char **argv)
{
	char           *xdisplay = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	configure_list = ewd_list_new();
	realize_list = ewd_list_new();
	destroy_list = ewd_list_new();
	free_evas_list = ewd_list_new();
	free_evas_object_list = ewd_list_new();
	child_add_list = ewd_list_new();

	__ewl_init_parse_options(argc, argv);

	ecore_init();
	edje_init();
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

	if (!ewl_theme_init()) {
		DERROR("Could not init theme data. Exiting....");
		exit(-1);
	}

	ewl_embed_list = ewd_list_new();
	ecore_idle_enterer_add(ewl_idle_render, NULL);
	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, __ewl_ecore_exit,
			NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief The main execution loop of EWL
 * 
 * This is the  main execution loop of ewl. It dispatches
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
 * @param data: this is only necessary for registering this function with ecore
 * @return Returns TRUE to continue the timer.
 * @brief Renders updates during idle times of the main loop
 *
 * Renders updates to the evas's during idle event times. Should not be called
 * publically unless a re-render is absolutely necessary.
 */
int ewl_idle_render(void *data)
{
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_embed_list) {
		DERROR("EWL has not been initialized. Exiting....\n");
		exit(-1);
	}

	if (ewd_list_is_empty(ewl_embed_list))
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	edje_freeze();

	/*
	 * Clean out the unused widgets first, to avoid them being drawn or
	 * unnecessary work done from configuration. Then display new widgets,
	 * finally layout the widgets.
	 */
	if (!ewd_list_is_empty(destroy_list))
		ewl_garbage_collect();

	if (!ewd_list_is_empty(realize_list))
		ewl_realize_queue();

	if (!ewd_list_is_empty(configure_list))
		ewl_configure_queue();

	edje_thaw();

	/*
	 * Allow each embed to render itself.
	 */
	ewd_list_goto_first(ewl_embed_list);
	while ((emb = ewd_list_next(ewl_embed_list)) != NULL) {
		if (emb->evas)
			evas_render(emb->evas);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
	data = NULL;
}

/**
 * @return Returns no value.
 * @brief Notifies ewl to quit at the end of this pass of the main loop
 *
 * Sets ewl to exit the main execution loop after this time
 * through the loop has been completed.
 */
void ewl_main_quit(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_main_loop_quit();
	ewl_callbacks_deinit();

	ewd_list_destroy(configure_list);
	ewd_list_destroy(realize_list);
	ewd_list_destroy(destroy_list);
	ewd_list_destroy(free_evas_list);
	ewd_list_destroy(free_evas_object_list);
	ewd_list_destroy(child_add_list);

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
 * @brief A timer function used to reread the config options
 * @return Returns TRUE to keep the timer going.
 * @param data: dummy variable used for compatibility with ecore's timers
 *
 * Sets up a timer loop for rereading the config data.
 */
static int ewl_reread_config(void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_config_reread_and_apply();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to register for configuration
 * @return Returns no value.
 * @brief Ask for a widget to be configured during idle loop
 *
 * Ask for the widget @w to be configured when the main idle loop is executed.
 */
void ewl_configure_request(Ewl_Widget * w)
{
	static int longest = 0;
	Ewl_Embed      *emb;
	Ewl_Widget     *search;

	DENTER_FUNCTION(DLEVEL_TESTING);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_find_by_widget(w);
	if (!emb)
		DRETURN(DLEVEL_STABLE);

	/*
	 * We don't need to configure if it's outside the viewable space in
	 * it's parent widget.
	 */
	if (w->parent) {
		int x, y;
		int width, height;
		Ewl_Widget *p = w->parent;

		ewl_object_get_current_geometry(EWL_OBJECT(w), &x, &y, &width,
				&height);
		if ((int)(x + width) < CURRENT_X(p) ||
				x > (int)(CURRENT_X(p) + CURRENT_W(p)) ||
				(int)(y + height) < CURRENT_Y(p) ||
				y > (int)(CURRENT_Y(p) + CURRENT_H(p)) ||
				(int)(x + width) < CURRENT_X(emb) ||
				x > (int)(CURRENT_X(emb) + CURRENT_W(emb)) ||
				(int)(y + height) < CURRENT_Y(emb) ||
				y > (int)(CURRENT_Y(emb) + CURRENT_H(emb))) {
			ewl_object_add_visible(EWL_OBJECT(w),
					EWL_FLAG_VISIBLE_OBSCURED);
			if (w->fx_clip_box)
				evas_object_hide(w->fx_clip_box);
			/* FIXME: This might be a good idea.
			if (w->theme_object)
				edje_object_freeze(w->theme_object);
			*/
		}
		else {
			ewl_object_remove_visible(EWL_OBJECT(w),
					EWL_FLAG_VISIBLE_OBSCURED);
			if (w->fx_clip_box)
				evas_object_show(w->fx_clip_box);
			/* FIXME: This might be a good idea.
			if (w->theme_object)
				edje_object_thaw(w->theme_object);
			*/
		}
	}

	/*
	 * Check this first, and remove an obscured widget from the configure
	 * list, if it's already scheduled.
	 */
	if (ewl_object_has_visible(EWL_OBJECT(w), EWL_FLAG_VISIBLE_OBSCURED)) {
		if (ewl_object_has_queued(EWL_OBJECT(w),
					EWL_FLAG_QUEUED_CSCHEDULED)) {
			ewd_list_goto_first(configure_list);
			while ((search = ewd_list_current(configure_list))) {
				if (search == w) {
					ewl_object_remove_queued(EWL_OBJECT(w),
						EWL_FLAG_QUEUED_CSCHEDULED);
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
	if (ewl_object_has_queued(EWL_OBJECT(w), EWL_FLAG_QUEUED_CSCHEDULED))
		DRETURN(DLEVEL_TESTING);

	/*
	 * Need to check if the embed that holds w is scheduled for
	 * configuration. This is the easiest way to test if we can avoid
	 * adding this widget to the configuration list.
	 */
	if (ewl_object_has_queued(EWL_OBJECT(emb),
			EWL_FLAG_QUEUED_CSCHEDULED))
		DRETURN(DLEVEL_TESTING);

	/*
	 * Check for any parent scheduled for configuration.
	 */
	search = w;
	while ((search = search->parent)) {
		if (ewl_object_has_queued(EWL_OBJECT(search),
					EWL_FLAG_QUEUED_CSCHEDULED))
			DRETURN(DLEVEL_TESTING);
	}

	/*
	 * No parent of this widget is queued so add it to the queue. All
	 * children widgets should have been removed by this point.
	 */
	ewl_object_add_queued(EWL_OBJECT(w), EWL_FLAG_QUEUED_CSCHEDULED);
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
				ewl_object_remove_queued(EWL_OBJECT(search),
						EWL_FLAG_QUEUED_CSCHEDULED);
				ewd_list_remove(configure_list);
				break;
			}
		}

		ewd_list_next(configure_list);
	}

	/*
	 * FIXME: Remove this once we get things stabilize a bit more.
	 */
	if (ewd_list_nodes(configure_list) > longest) {
		longest = ewd_list_nodes(configure_list);
	}

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

void ewl_configure_queue()
{
	Ewl_Widget *w;

	/*
	 * Configure any widgets that need it.
	 */
	while ((w = ewd_list_remove_first(configure_list))) {
		/*
		 * Remove the flag that the widget is scheduled for
		 * configuration.
		 */
		ewl_object_remove_queued(EWL_OBJECT(w),
				EWL_FLAG_QUEUED_CSCHEDULED);
		ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);
	}
}

/**
 * @param w: the widget that no longer needs to be configured
 * @return Returns no value.
 * @brief Cancel a request to configure a widget
 *
 * Remove the widget @w from the list of widgets that need to be configured.
 */
void ewl_configure_cancel_request(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_TESTING);

	ewd_list_goto(configure_list, w);

	if (ewd_list_current(configure_list) == w)
		ewd_list_remove(configure_list);

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/**
 * @param w: widget to schedule for realization
 * @return Returns no value.
 * @brief Schedule a widget to be realized at idle time
 *
 * Places a widget on the queue to be realized at a later time.
 */
void ewl_realize_request(Ewl_Widget *w)
{
	Ewl_Widget *search;

	if (ewl_object_has_queued(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED))
		return;

	ewl_object_add_queued(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED);

	/*
	 * Search the list for a child widget of this widget.
	 */
	ewd_list_goto_first(realize_list);
	while ((search = ewd_list_current(realize_list))) {
		Ewl_Widget *parent;

		parent = search;
		while ((parent = parent->parent)) {
			if (parent == w) {
				ewd_list_insert(realize_list, w);
				return;
			}
		}

		ewd_list_next(realize_list);
	}

	ewd_list_append(realize_list, w);
}

void ewl_realize_queue()
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_enter_realize_phase();

	/*
	 * First realize any widgets that require it, this looping should
	 * avoid deep recursion, and works from top to bottom.
	 */
	ewd_list_goto_first(realize_list);
	while ((w = ewd_list_remove_first(realize_list))) {
		if (VISIBLE(w) && !REALIZED(w)) {
			ewl_object_remove_queued(EWL_OBJECT(w),
					EWL_FLAG_QUEUED_RSCHEDULED);
			ewl_widget_realize(EWL_WIDGET(w));
			ewd_list_prepend(child_add_list, w);
		}
	}

	/*
	 * Work our way back up the chain of widgets to resize from bottom to
	 * top.
	 */
	while ((w = ewd_list_remove_first(child_add_list))) {
		if (w->parent)
			ewl_container_call_child_add(EWL_CONTAINER(w->parent),
						     w);
	}

	ewl_exit_realize_phase();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Marks that EWL is currently realizing a widget.
 */
void ewl_enter_realize_phase()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	phase_status |= EWL_FLAG_QUEUED_RSCHEDULED;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Marks that EWL is not realizing a widget.
 */
void ewl_exit_realize_phase()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	phase_status &= ~EWL_FLAG_QUEUED_RSCHEDULED;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns TRUE if currently realizing a widget, FALSE otherwise.
 * @brief Checks if EWL is currently in the process of realizing widgets.
 */
int ewl_in_realize_phase()
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DRETURN_INT((phase_status & EWL_FLAG_QUEUED_RSCHEDULED), DLEVEL_STABLE);
}

void ewl_destroy_request(Ewl_Widget *w)
{
	if (ewl_object_has_queued(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	ewl_object_add_queued(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED);

	/*
	 * Must prepend to ensure children are freed before parents.
	 */
	ewd_list_prepend(destroy_list, w);

	/*
	 * Schedule child widgets for destruction.
	 */
	if (ewl_object_get_recursive(EWL_OBJECT(w)))
		ewl_container_destroy(EWL_CONTAINER(w));
}

void ewl_evas_destroy(Evas *evas)
{
	ewd_list_append(free_evas_list, evas);
}

void ewl_evas_object_destroy(Evas_Object *obj)
{
	ewd_list_append(free_evas_object_list, obj);
}

void ewl_garbage_collect()
{
	Evas *evas;
	Ewl_Widget *w;
	Evas_Object *obj;

	DENTER_FUNCTION(DLEVEL_STABLE);

	while ((w = ewd_list_remove_first(destroy_list))) {
		ewl_callback_call(w, EWL_CALLBACK_DESTROY);
		ewl_callback_del_type(w, EWL_CALLBACK_DESTROY);
		FREE(w);
	}

	while ((obj = ewd_list_remove_first(free_evas_object_list)))
		evas_object_del(obj);

	while ((evas = ewd_list_remove_first(free_evas_object_list)))
		evas_free(evas);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int __ewl_ecore_exit(void *data, int type, void *event)
{
	ewl_main_quit();

	return 1;
}
