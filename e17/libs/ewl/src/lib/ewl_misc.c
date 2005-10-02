#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

extern Ecore_List *ewl_embed_list;
extern Ecore_List *ewl_window_list;

/*
 * Configuration and option related flags.
 */
static unsigned int    debug_segv = 0;
static unsigned int    use_engine = EWL_ENGINE_ALL;
static unsigned int    phase_status = 0;
static unsigned int    print_theme_keys = 0;
static unsigned int    debug_level = 0;

static Ecore_Idle_Enterer *idle_enterer = NULL;
static int _ewl_init_count = 0;

/*
 *
 */

/*
 * Queues for scheduling various actions.
 */
static Ecore_List *configure_list = NULL;
static Ecore_List *realize_list = NULL;
static Ecore_List *destroy_list = NULL;
static Ecore_List *child_add_list= NULL;

/*
 * Lists for cleaning up evas related memory at convenient times.
 */
static Ecore_List *free_evas_list = NULL;
static Ecore_List *free_evas_object_list = NULL;


int             ewl_idle_render(void *data);
static void     ewl_init_parse_options(int *argc, char **argv);
static void     ewl_init_remove_option(int *argc, char **argv, int i);
int             ewl_ecore_exit(void *data, int type, void *event);

/**
 * @return Returns no value.
 * @brief This is used by debugging macros for breakpoints
 *
 * Set a breakpoint at this function in order to retrieve backtraces from
 * warning messages.
 */
inline void ewl_print_warning()
{
	if (debug_segv) {
		char *null = NULL;
		*null = '\0';
	}
	else {
		fprintf(stderr, "***** Ewl Developer Warning ***** :\n"
			" To find where this is occurring set a breakpoint\n"
			" for the function %s.\n", __FUNCTION__);
	}
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
int ewl_init(int *argc, char **argv)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/* check if we are already initialized */
	if (++_ewl_init_count > 1)
		DRETURN_INT(_ewl_init_count, DLEVEL_STABLE);

	ewl_init_parse_options(argc, argv);

	if (!ecore_init()) {
		DERROR("Could not init ecore....\n");
		DRETURN_INT(--_ewl_init_count, DLEVEL_STABLE);
	}

	if (!edje_init()) {
		DERROR("Could not init edje....\n");
		ecore_shutdown();
		DRETURN_INT(--_ewl_init_count, DLEVEL_STABLE);
	}

	configure_list = ecore_list_new();
	realize_list = ecore_list_new();
	destroy_list = ecore_list_new();
	free_evas_list = ecore_list_new();
	free_evas_object_list = ecore_list_new();
	child_add_list = ecore_list_new();

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	/*
	 * Attempt to pick the correct engine by adjusting the bitmask
	 * relative to the success of each engines init routine.
	 */
	if (use_engine & EWL_ENGINE_X11) {
		if (!ecore_x_init(NULL))
			use_engine &= ~EWL_ENGINE_X11;
		else
			use_engine &= EWL_ENGINE_X11;
	}
#endif

#ifdef HAVE_EVAS_ENGINE_FB_H
	/*
	 * Maybe the X11 engines arent' available or they failed, so see if
	 * we should load up the FB.
	 */
	if (use_engine & EWL_ENGINE_FB) {
		if (!ecore_fb_init(NULL))
			use_engine &= ~EWL_ENGINE_FB;
		else
			use_engine &= EWL_ENGINE_FB;
	}
#endif

	if (!use_engine) {
		fprintf(stderr, "Cannot open display!\n");
		ewl_shutdown();
		DRETURN_INT(_ewl_init_count, DLEVEL_STABLE);
	}

	if (!ewl_config_init()) {
		DERROR("Could not init config data.\n");
		ewl_shutdown();
		DRETURN_INT(_ewl_init_count, DLEVEL_STABLE);
	}

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (use_engine == EWL_ENGINE_SOFTWARE_X11) {
		IF_FREE(ewl_config.evas.render_method);
		ewl_config.evas.render_method = strdup("software_x11");
	}
	else
#endif
#ifdef HAVE_EVAS_ENGINE_GL_X11_H
	if (use_engine == EWL_ENGINE_GL_X11) {
		IF_FREE(ewl_config.evas.render_method);
		ewl_config.evas.render_method = strdup("gl_x11");
	}
	else
#endif
#ifdef HAVE_EVAS_ENGINE_FB_H
	if (use_engine == EWL_ENGINE_FB) {
		IF_FREE(ewl_config.evas.render_method);
		ewl_config.evas.render_method = strdup("fb");
	}
	else
#endif
	if (!ewl_config.evas.render_method)
		ewl_config.evas.render_method = strdup("software_x11");

	if (print_theme_keys)
		ewl_config.theme.print_keys = print_theme_keys;

	if (debug_level) {
		ewl_config.debug.enable = 1;
		ewl_config.debug.level = debug_level;
	}

	if (!ewl_ev_init()) {
		DERROR("Could not init event data.\n");
		ewl_shutdown();
		DRETURN_INT(_ewl_init_count, DLEVEL_STABLE);
	}

	ewl_callbacks_init();

	if (!ewl_theme_init()) {
		ewl_shutdown();
		DRETURN_INT(_ewl_init_count, DLEVEL_STABLE);
	}

	ewl_embed_list = ecore_list_new();
	ewl_window_list = ecore_list_new();
	idle_enterer = ecore_idle_enterer_add(ewl_idle_render, NULL);

	ewl_text_context_init();

	DRETURN_INT(_ewl_init_count, DLEVEL_STABLE);
}

/**
 * @brief Cleanup internal data structures used by ewl.
 *
 * This should be called to cleanup internal EWL data structures, if using
 * ecore directly rather than using ewl_main().
 */
int ewl_shutdown()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (--_ewl_init_count)
		DRETURN_INT(_ewl_init_count, DLEVEL_STABLE);
	/*
	 * Destroy all existing widgets.
	 */
	if (ewl_embed_list) {
		Ewl_Widget *emb;

		while ((emb = ecore_list_remove_first(ewl_embed_list)))
			ewl_widget_destroy(emb);
		ewl_garbage_collect();
		ecore_list_destroy(ewl_embed_list);
		ewl_embed_list = NULL;
	}

	ewl_text_context_shutdown();

	ecore_idle_enterer_del(idle_enterer);
	idle_enterer = NULL;


	/*
	 * Shut down the various EWL subsystems cleanly.
	 */
	ewl_callbacks_shutdown();
	ewl_theme_shutdown();
	ewl_config_shutdown();

	/*
	 * Free internal accounting lists.
	 */
	if (ewl_window_list) {
		ecore_list_destroy(ewl_window_list);
		ewl_window_list = NULL;
	}
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

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (use_engine & EWL_ENGINE_X11) 
		ecore_x_shutdown();
#endif

#ifdef HAVE_EVAS_ENGINE_FB_H
	if (use_engine & EWL_ENGINE_FB)
		ecore_fb_shutdown();
#endif

	ecore_shutdown();

	DRETURN_INT(_ewl_init_count, DLEVEL_STABLE);
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

	ecore_main_loop_begin();
	ewl_shutdown();

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

	if (ecore_list_is_empty(ewl_embed_list))
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	edje_freeze();

	/*
	 * Clean out the unused widgets first, to avoid them being drawn or
	 * unnecessary work done from configuration. Then display new widgets,
	 * finally layout the widgets.
	 */
	if (!ecore_list_is_empty(destroy_list) ||
			!ecore_list_is_empty(free_evas_list) ||
			!ecore_list_is_empty(free_evas_object_list))
		ewl_garbage_collect();

	if (!ecore_list_is_empty(realize_list))
		ewl_realize_queue();

	/*
	 * Freeze events on the evases to reduce overhead
	 */
	ecore_list_goto_first(ewl_embed_list);
	while ((emb = ecore_list_next(ewl_embed_list)) != NULL) {
		if (REALIZED(emb) && emb->evas)
			evas_event_freeze(emb->evas);
	}

	if (!ecore_list_is_empty(configure_list))
		ewl_configure_queue();

	edje_thaw();

	/*
	 * Allow each embed to render itself, this requires thawing the evas.
	 */
	ecore_list_goto_first(ewl_embed_list);
	while ((emb = ecore_list_next(ewl_embed_list)) != NULL) {
		if (REALIZED(emb) && emb->evas) {
			evas_event_thaw(emb->evas);
			evas_render(emb->evas);
		}
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
static void ewl_init_parse_options(int *argc, char **argv)
{
	int i;
	int matched = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!argc || !argv)
		DRETURN(DLEVEL_STABLE);

	i = 0;
	while (i < *argc) {
		if (!strcmp(argv[i], "--ewl-segv")) {
			debug_segv = 1;
			matched++;
		}
		if (!strcmp(argv[i], "--ewl-theme")) {
			if (i + 1 < *argc) {
				ewl_theme_name_set(argv[i + 1]);
				matched++;
			}
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-print-theme-keys")) {
			print_theme_keys = 1;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-software-x11")) {
			use_engine = EWL_ENGINE_SOFTWARE_X11;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-gl-x11")) {
			use_engine = EWL_ENGINE_GL_X11;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-fb")) {
			use_engine = EWL_ENGINE_FB;
			matched++;
		}
		else if (!strcmp(argv[i], "--ewl-debug")) {
			if (i + i < *argc) {
				debug_level = atoi(argv[i + 1]);
				matched++;
			} else {
				debug_level = 1;
			}
			matched ++;
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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_init_remove_option(int *argc, char **argv, int i)
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
 * @param w: the widget to register for configuration
 * @return Returns no value.
 * @brief Ask for a widget to be configured during idle loop
 *
 * Ask for the widget @a w to be configured when the main idle loop is executed.
 */
void ewl_configure_request(Ewl_Widget * w)
{
	static int longest = 0;
	Ewl_Embed      *emb;
	Ewl_Widget     *search;

	DENTER_FUNCTION(DLEVEL_TESTING);
	DCHECK_PARAM_PTR("w", w);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_CPROCESS))
		DRETURN(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);
	if (!emb)
		DRETURN(DLEVEL_STABLE);

	/*
	 * We don't need to configure if it's outside the viewable space in
	 * it's parent widget.
	 */
	if (w->parent) {
		int obscured = 0;
		int x, y;
		int width, height;
		Ewl_Widget *p = w->parent;

		ewl_object_current_geometry_get(EWL_OBJECT(w), &x, &y, &width,
				&height);

		if ((x + width) < CURRENT_X(p))
			obscured = 1;

		if (x > (CURRENT_X(p) + CURRENT_W(p)))
			obscured = 1;

		if ((y + height) < CURRENT_Y(p))
			obscured = 1;

		if (y > (CURRENT_Y(p) + CURRENT_H(p)))
			obscured = 1;

		if ((x + width) < CURRENT_X(emb))
			obscured = 1;

		if (x > (CURRENT_X(emb) + CURRENT_W(emb)))
			obscured = 1;

		if ((y + height) < CURRENT_Y(emb))
			obscured = 1;

		if (y > (CURRENT_Y(emb) + CURRENT_H(emb)))
			obscured = 1;

		if (obscured) {
			ewl_object_visible_add(EWL_OBJECT(w),
					EWL_FLAG_VISIBLE_OBSCURED);
			if (w->fx_clip_box)
				evas_object_hide(w->fx_clip_box);
			if (w->theme_object)
				evas_object_hide(w->theme_object);
			/* FIXME: This might be a good idea.
			if (w->theme_object)
				edje_object_freeze(w->theme_object);
			*/
		}
		else {
			ewl_object_visible_remove(EWL_OBJECT(w),
					EWL_FLAG_VISIBLE_OBSCURED);
			if (w->fx_clip_box)
				evas_object_show(w->fx_clip_box);
			if (w->theme_object)
				evas_object_show(w->theme_object);
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
	if (ewl_object_visible_has(EWL_OBJECT(w), EWL_FLAG_VISIBLE_OBSCURED)) {
		if (ewl_object_queued_has(EWL_OBJECT(w),
					EWL_FLAG_QUEUED_CSCHEDULED)) {
			ecore_list_goto_first(configure_list);
			while ((search = ecore_list_current(configure_list))) {
				if (search == w) {
					ewl_object_queued_remove(EWL_OBJECT(w),
						EWL_FLAG_QUEUED_CSCHEDULED);
					ecore_list_remove(configure_list);
					break;
				}
				ecore_list_next(configure_list);
			}
		}

		DRETURN(DLEVEL_TESTING);
	}

	/*
	 * Easy case, we know this is on the list already.
	 */
	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_CSCHEDULED))
		DRETURN(DLEVEL_TESTING);

	/*
	 * Need to check if the embed that holds w is scheduled for
	 * configuration. This is the easiest way to test if we can avoid
	 * adding this widget to the configuration list.
	 */
	if (ewl_object_queued_has(EWL_OBJECT(emb),
			EWL_FLAG_QUEUED_CSCHEDULED))
		DRETURN(DLEVEL_TESTING);

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

	/*
	 * Now clean off any children of this widget, they will get added
	 * later.
	 */
	/* FIXME: This is a big source of slow down on long lists of widgets,
	 * might not be worth it
	ecore_list_goto_first(configure_list);
	while ((search = ecore_list_current(configure_list))) {
		Ewl_Widget *parent;

		parent = search;
		while ((parent = parent->parent)) {
			if (parent == w) {
				ewl_object_queued_remove(EWL_OBJECT(search),
						EWL_FLAG_QUEUED_CSCHEDULED);
				ecore_list_remove(configure_list);
				break;
			}
		}

		ecore_list_next(configure_list);
	}
	*/

	/*
	 * FIXME: Remove this once we get things stabilize a bit more.
	 */
	if (ecore_list_nodes(configure_list) > longest) {
		longest = ecore_list_nodes(configure_list);
	}

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

void ewl_configure_queue()
{
	Ewl_Widget *w;

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

		ewl_object_queued_add(EWL_OBJECT(w), EWL_FLAG_QUEUED_CPROCESS);
		ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);
		ewl_object_queued_remove(EWL_OBJECT(w),
					 EWL_FLAG_QUEUED_CPROCESS);
	}
}

/**
 * @param w: the widget that no longer needs to be configured
 * @return Returns no value.
 * @brief Cancel a request to configure a widget
 *
 * Remove the widget @a w from the list of widgets that need to be configured.
 */
void ewl_configure_cancel_request(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_TESTING);

	ecore_list_goto(configure_list, w);

	if (ecore_list_current(configure_list) == w)
		ecore_list_remove(configure_list);

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
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	if (!ewl_object_flags_get(EWL_OBJECT(w), EWL_FLAG_PROPERTY_TOPLEVEL)) {
		if (!w->parent || !REALIZED(w->parent))
			DRETURN(DLEVEL_STABLE);
	}

	ewl_object_queued_add(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED);
	ecore_list_append(realize_list, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget that no longer needs to be realized
 * @return Returns no value.
 * @brief Cancel a request to realize a widget
 *
 * Remove the widget @a w from the list of widgets that need to be realized.
 */
void ewl_realize_cancel_request(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_TESTING);

	ecore_list_goto(realize_list, w);

	if (ecore_list_current(realize_list) == w)
		ecore_list_remove(realize_list);

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

void ewl_realize_queue()
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
			ewl_widget_realize(EWL_WIDGET(w));
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
 * @return Returns no value.
 * @brief Marks that EWL is currently realizing a widget.
 */
void ewl_realize_phase_enter()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	phase_status |= EWL_FLAG_QUEUED_RSCHEDULED;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Marks that EWL is not realizing a widget.
 */
void ewl_realize_phase_exit()
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

unsigned int ewl_engine_mask_get()
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DRETURN_INT(use_engine, DLEVEL_STABLE);
}

void ewl_destroy_request(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_CSCHEDULED))
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

void ewl_evas_destroy(Evas *evas)
{
	ecore_list_append(free_evas_list, evas);
}

void ewl_evas_object_destroy(Evas_Object *obj)
{
	ecore_list_append(free_evas_object_list, obj);
}

void ewl_garbage_collect()
{
	Evas *evas;
	Ewl_Widget *w;
	Evas_Object *obj;

	DENTER_FUNCTION(DLEVEL_STABLE);

	while ((w = ecore_list_remove_first(destroy_list))) {
		if (ewl_object_queued_has(EWL_OBJECT(w),
					  EWL_FLAG_QUEUED_CSCHEDULED))
			ewl_configure_cancel_request(w);
		ewl_callback_call(w, EWL_CALLBACK_DESTROY);
		ewl_callback_del_type(w, EWL_CALLBACK_DESTROY);
		FREE(w);
	}

	while ((obj = ecore_list_remove_first(free_evas_object_list)))
		evas_object_del(obj);

	while ((evas = ecore_list_remove_first(free_evas_object_list)))
		evas_free(evas);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int ewl_ecore_exit(void *data __UNUSED__, int type __UNUSED__,
					void *event __UNUSED__)
{
	ewl_main_quit();

	return 1;
}

#ifdef DEBUG_MALLOCDEBUG
char *strdup(const char *str)
{
	char *dst = malloc(strlen(str) + 1);
	if (dst)
		strcpy(dst, str);

	return dst;
}
#endif
