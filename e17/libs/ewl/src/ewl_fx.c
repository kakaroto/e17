
#include <Ewl.h>


static inline void __ewl_fx_timer_func(int val, void *data);
static inline void __ewl_fx_func_start(Ewl_Widget * w, void *ev_data,
				       void *user_data);
static inline void __ewl_fx_func_stop(Ewl_Widget * w, void *ev_data,
				      void *user_data);

void            __ewl_fx_widget_appearance_changed(Ewl_Widget * w,
						   void *ev_data,
						   void *user_data);

static Ewd_Hash *fx_protos = NULL;
static Ewd_Hash *fx_timers = NULL;
static int      fx_group_id = -1;


/**
 * ewl_fx_init - initialize any fx variables that must be set up ahead of time
 * 
 * Returns no value. The programmer should not call this, it is used by
 * ewl_init, and nothing else.
 */
int ewl_fx_init(void)
{
	int             count;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Create hash tables for tracking fx prototypes and timers.
	 */
	fx_protos = ewd_hash_new(ewd_str_hash, ewd_str_compare);
	fx_timers = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	/*
	 * Create a path group for searching for fx modules.
	 */
	fx_group_id = ewd_path_group_new("fx");

	/*
	 * Determine the number of paths that need to be added.
	 */
	count = ewl_config_get_int("system", "/fx/paths/count");

	if (count) {
		char            key[PATH_MAX];
		char           *path;
		int             i;

		/*
		 * Loop through and add each path to the modules.
		 */
		for (i = 0; i < count; i++) {
			snprintf(key, PATH_MAX, "/fx/paths/%i", i);

			path = ewl_config_get_str("system", key);

			ewd_path_group_add(fx_group_id, path);
		}
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * ewl_fx_deinit - deinit any fx variables that must be set up ahead of time
 * 
 * Returns no value. The programmer should not call this, it is used by
 * ewl_deinit, and nothing else.
 */
int ewl_fx_deinit(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewd_path_group_del(fx_group_id);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * ewl_fx_init_widget - initialize fx for a specified widget
 * @w: the widget to setup fx variables
 *
 * Returns no value. Internal variables for fx to be used are setup on @w.
 */
void ewl_fx_init_widget(Ewl_Widget * w)
{
	char            wname[PATH_MAX];
	char            key[PATH_MAX];
	int             count, i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Need to be notified when the widget changes appearance so that we
	 * can reload the fx as needed.
	 */
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_fx_widget_appearance_changed, NULL);

	/*
	 * The appearance string is necessary for determining which fx apply
	 * to this widget.
	 */
	if (!w->appearance) {
		D(DLEVEL_STABLE,
		  "Widget does not have a appearance string\n");
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Check for an error in the appearance string.
	 */
	/*
	if (strncmp(w->appearance, "/appearance/", 12)) {
		D(DLEVEL_STABLE,
		  "Widget %p has this:\n\n\t%s\n\nWierd appearance string\n",
		  w, w->appearance);
		DRETURN(DLEVEL_STABLE);
	}
	*/

	/*
	 * Determine the key for fx to load for this widget.
	 */
	sscanf(w->appearance, "/%[^/]/", wname);

	/*
	 * Now determine how many fx to apply to this widget.
	 */
	snprintf(key, PATH_MAX, "/user/%s/count", wname);
	count = ewl_config_get_int("fx", key);

	/*
	 * No fx to apply, so our work is done.
	 */
	if (!count) {
		D(DLEVEL_STABLE, "No effects for widget\n");
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Load all of the fx for this widget.
	 */
	for (i = 0; i < count; i++) {
		char           *name;
		int             start, end;
		int             cb_count, j;

		/*
		 * Determine the name of the fx module to load.
		 */
		snprintf(key, PATH_MAX, "/user/%s/%i/name", wname, i);
		name = ewl_config_get_str("fx", key);

		/*
		 * Now determine the number of callbacks for that specific
		 * effect.
		 */
		snprintf(key, PATH_MAX, "/user/%s/%i/callbacks/count", wname,
			 i);
		cb_count = ewl_config_get_int("fx", key);

		/*
		 * Read in and setup each of the callbacks for the specified
		 * effect module.
		 */
		for (j = 0; j < cb_count; j++) {
			/*
			 * Determine the starting event.
			 */
			snprintf(key, PATH_MAX,
				 "/user/%s/%i/callbacks/%i/cb_start", wname, i,
				 j);
			start = ewl_config_get_int("fx", key);

			/*
			 * Determine the ending event.
			 */
			snprintf(key, PATH_MAX,
				 "/user/%s/%i/callbacks/%i/cb_stop", wname, i,
				 j);
			end = ewl_config_get_int("fx", key);

			/*
			 * Now add the effect to the widget with the
			 * correct name, starting event, and ending event.
			 */
			ewl_fx_add(w, name, start, end);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_fx_deinit_widget - deinitialize fx for a specified widget
 * @w: the widget to remove fx variables
 *
 * Returns no value. Frees fx variables attached to the widget @w.
 */
void ewl_fx_deinit_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_fx_add_proto - add a prototype for executing fx
 * @name: the identifier for the fx
 * @fx_start: the function called when the fx begin
 * @fx_cont: the function called at specified intervals
 * @fx_stop: the function called when the fx stop
 * @modifies: a bit mask that specifies which properties the fx alter
 *
 * Returns -1 on error, 0 if a cached version of the proto is found, 1 if a
 * newly allocated prototype is used. This should be used if a programmer
 * wishes to include their own plugins.
 */
int
ewl_fx_add_proto(char *name,
		 Ewl_FX_Function fx_start,
		 Ewl_FX_Timer_Function fx_cont,
		 Ewl_FX_Function fx_stop, Ewl_FX_Modifies modifies)
{
	Ewl_FX_Proto   *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	/*
	 * Make sure this effect prototype has not already been added.
	 */
	fxp = ewd_hash_get(fx_protos, name);
	if (fxp)
		DRETURN_INT(0, DLEVEL_STABLE);

	/*
	 * Allocate a new effect prototype.
	 */
	fxp = NEW(Ewl_FX_Proto, 1);
	ZERO(fxp, Ewl_FX_Proto, 1);

	/*
	 * Setup the prototype fields for the effect
	 */
	fxp->fx_start = fx_start;
	fxp->fx_cont = fx_cont;
	fxp->fx_stop = fx_stop;
	fxp->modifies = modifies;
	fxp->name = strdup(name);

	/*
	 * Add it to the prototype table.
	 */
	ewd_hash_set(fx_protos, fxp->name, fxp);

	DRETURN_INT(1, DLEVEL_STABLE);
}


/**
 * ewl_fx_del_proto - delete the prototype for the fx with the specified @name
 * @name: the name identifier for the fx prototype to remove.
 *
 * Returns TRUE on success, FALSE on failure.
 */
int ewl_fx_del_proto(char *name)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * ewl_fx_proto_get - retrieve a prototype for the plugin of the specified @name
 * @name: the name identifier for the desired fx prototype
 *
 * Returns a pointer to the found fx prototype on success, NULL on failure.
 */
Ewl_FX_Proto   *ewl_fx_proto_get(char *name)
{
	Ewl_FX_Proto   *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	/*
	 * Find the prototype in the global table.
	 */
	fxp = ewd_hash_get(fx_protos, name);

	DRETURN_PTR(fxp, DLEVEL_STABLE);
}


/**
 * ewl_fx_add - add fx to a specified widget
 * @w: the widget to add the fx
 * @name: the name of the fx to add
 * @cb_start: the callback to indicate the fx have started
 * @cb_stop: the callback to indicate the fx have stopped
 *
 * Returns -1 on error, 0 if the fx have already been started, 1 if the fx are
 * newly started.
 */
int
ewl_fx_add(Ewl_Widget * w, char *name, Ewl_Callback_Type cb_start,
	   Ewl_Callback_Type cb_stop)
{
	Ewl_FX_Proto   *fxp;
	Ewl_FX_Pending *pend;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	/*
	 * Retrieve any prototypes for this effect.
	 */
	fxp = ewd_hash_get(fx_protos, name);

	/*
	 * Load the plugin if it hasn't already been loaded.
	 */
	if (!fxp)
		fxp = ewl_fx_plugin_load(name);

	/*
	 * An error occurred loading the plugin.
	 */
	if (!fxp)
		DRETURN_INT(0, DLEVEL_STABLE);

	/*
	 * If the fx list hasn't been created, then create it.
	 */
	if (!w->fx)
		w->fx = ewd_list_new();
	else {
		ewd_list_goto_first(w->fx);

		/*
		 * Loop through the list of fx and return if it's already on
		 * the list.
		 */
		while ((pend = ewd_list_next(w->fx))) {
			if (!strcmp(pend->name, name) &&
			    pend->cb_start == cb_start &&
			    pend->cb_stop == cb_stop)
				DRETURN_INT(0, DLEVEL_STABLE);
		}
	}

	/*
	 * Allocate and setup the new fx pending structure.
	 */
	pend = NEW(Ewl_FX_Pending, 1);
	ZERO(pend, Ewl_FX_Pending, 1);

	pend->name = strdup(name);
	pend->cb_start = cb_start;
	pend->cb_stop = cb_stop;

	ewd_list_append(w->fx, pend);

	/*
	 * Add the callback for for the start and stop functions.
	 */
	if (cb_start != EWL_CALLBACK_NONE)
		ewl_callback_append(w, cb_start, __ewl_fx_func_start, pend);

	if (cb_stop != EWL_CALLBACK_NONE)
		ewl_callback_append(w, cb_stop, __ewl_fx_func_stop, pend);

	/*
	 * Stop the effect before the widget gets destroyed.
	 */
	if (cb_stop != EWL_CALLBACK_DESTROY)
		ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
				     __ewl_fx_func_stop, pend);

	D(DLEVEL_STABLE, "Effect added to widget properly");

	DRETURN_INT(1, DLEVEL_STABLE);
}


/**
 * ewl_fx_del - delete the specified fx from the widget @w
 * @w: the widget to delete fx
 * @name: the name of the fx to remove from @w
 * @cb_start: the callback to indicate the start of the fx
 * @cb_stop: the callback to indicate the stop of the fx
 *
 * Returns -1 on error, 0 if the fx are not running, 1 if the fx are running
 * and have been stopped.
 */
int
ewl_fx_del(Ewl_Widget * w, char *name, Ewl_Callback_Type cb_start,
	   Ewl_Callback_Type cb_stop)
{
	Ewl_FX_Pending *pend;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	/*
	 * If there aren't any fx just return before doing anything.
	 */
	if (!w->fx || ewd_list_is_empty(w->fx))
		DRETURN_INT(0, DLEVEL_STABLE);

	ewd_list_goto_first(w->fx);

	/*
	 * Search the list for the desired effect.
	 */
	while ((pend = ewd_list_current(w->fx)) != NULL) {
		/*
		 * If the effect is found remove it from the list.
		 */
		if (pend->name && !strcmp(pend->name, name) &&
		    pend->cb_start == cb_start && pend->cb_stop == cb_stop) {
			ewd_list_remove(w->fx);
			break;
		}

		ewd_list_next(w->fx);
	}

	/*
	 * No pending effect found, so return without further work.
	 */
	if (!pend)
		DRETURN_INT(0, DLEVEL_STABLE);

	/*
	 * Stop the effect if it's executing.
	 */
	if (pend->pending)
		ewl_fx_stop(w, pend);

	/*
	 * Find any queued instances of this effect and remove them.
	 */
	if (pend->queued) {
		Ewl_FX_Pending *pend2;

		ewd_list_goto_first(w->fx_queues);

		/*
		 * Remove all instances, as their may be multiple.
		 */
		while ((pend2 = ewd_list_current(w->fx_queues)) != NULL) {
			if (pend2 == pend) {
				ewd_list_remove(w->fx_queues);
				break;
			}

			ewd_list_next(w->fx_queues);
		}
	}

	/*
	 * Free the data used by the effect.
	 */
	IF_FREE(pend->name);
	FREE(pend);

	DRETURN_INT(1, DLEVEL_STABLE);
}


/**
 * ewd_fx_del_all - remove all fx from the specified widget
 * @w: the widget to remove fx
 *
 * Returns no value. Any fx currently running are stopped and all fx are
 * deleted from the widget.
 */
void ewl_fx_del_all(Ewl_Widget * w)
{
	Ewl_FX_Pending *pend;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Destroy the queues of fx.
	 */
	if (w->fx_queues) {
		ewd_list_destroy(w->fx_queues);
		w->fx_queues = NULL;
	}

	/*
	 * Remove all pending fx from the widget.
	 */
	if (w->fx && !ewd_list_is_empty(w->fx)) {
		while ((pend = ewd_list_remove_last(w->fx))) {
			/*
			 * Delete the callbacks associated with the fx.
			 */
			if (pend->cb_start != EWL_CALLBACK_NONE)
				ewl_callback_del(w, pend->cb_start,
						 __ewl_fx_func_start);

			if (pend->cb_stop != EWL_CALLBACK_NONE)
				ewl_callback_del(w, pend->cb_stop,
						 __ewl_fx_func_stop);
		}

		ewd_list_destroy(w->fx);
		w->fx = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_fx_timer_add - add a timer for the specified fx on the a widget
 * @w: the widget to add the fx timer
 * @name: the name of the fx to add the timer
 * @timeout: the time until the effect has stepped to it's maximum value
 * @fps: the number of frames per second that will be displayed
 * @value_span: the range of values that will be stepped through
 * @data: user specified data to pass to the timer function
 *
 * Returns no value. Sets up a timer to be executed at regular intervals to
 * allow animation of fx.
 */
void
ewl_fx_timer_add(Ewl_Widget * w, char *name, double timeout, int fps,
		 int value_span, void *data)
{
	Ewl_FX_Timer   *timer;
	Ewl_FX_Pending *pend = NULL;
	Ewl_FX_Proto   *fxp;
	int             l, ufps;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("name", name);

	/*
	 * Find the prototype for this effect. This is necessary to go any
	 * further.
	 */
	fxp = ewd_hash_get(fx_protos, name);

	if (!fxp) {
		D(DLEVEL_STABLE,
		  "Can't add timer since fx proto does not exist.\n");
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Make sure the widget has been initialized.
	 */
	if (!w->fx || ewd_list_is_empty(w->fx)) {
		D(DLEVEL_STABLE,
		  "Trying to add effect to a widget which doesnt"
		  "want have info about the effect\n");
	} else {
		/*
		 * Find the specified effect on the fx list.
		 */
		ewd_list_goto_first(w->fx);

		while ((pend = ewd_list_next(w->fx)) != NULL)
			if (!strcmp(pend->name, name))
				break;
	}

	/*
	 * Allocate a timer for performing the effect.
	 */
	timer = NEW(Ewl_FX_Timer, 1);
	ZERO(timer, Ewl_FX_Timer, 1);

	/*
	 * Allow the user to override the programmers chosen fps if that value
	 * is specified and smaller than the programmers fps.
	 */
	ufps = ewl_config_get_int("system", "/fx/fps");
	if (ufps && ufps < fps)
		fps = ufps;

	/*
	 * Prepare the timer for use by the Ecore timer loop.
	 */
	timer->widget = w;
	timer->func = fxp->fx_cont;
	timer->interval = 1 / (float) fps;
	timer->count = 0;
	timer->hits = timeout * fps;
	timer->step = (float) value_span / (timer->hits + 0.5);
	timer->data = data;
	timer->pend = pend;

	/*
	 * Give the timer a name that is unique to the widget. In this case,
	 * the name of the effect concatenated with the address of the widget.
	 */
	l = strlen(name);
	l += 20;
	timer->name = NEW(char, l);

	snprintf(timer->name, l, "%s%p", name, w);

	/*
	 * Start the timer and add it to the table of timers.
	 */
	ecore_add_event_timer(timer->name, timer->interval,
			      __ewl_fx_timer_func, 0, timer);

	ewd_hash_set(fx_timers, timer->name, timer);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_fx_timer_del - remove a timer of a specified name from a widget
 * @w: the widget to remove the timer
 * @name: the name of the timer attached to @w that is to be removed
 *
 * Returns no value. Removes the timer specified by @name from the widget @w.
 */
void ewl_fx_timer_del(Ewl_Widget * w, char *name)
{
	Ewl_FX_Timer   *timer;
	char            name2[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("name", name);

	snprintf(name2, PATH_MAX, "%s%p", name, w);

	D(DLEVEL_STABLE, "attempting to remove fx");

	/*
	 * Check to see if the timer is on the table. If not, then we can't
	 * remove it.
	 */
	timer = ewd_hash_remove(fx_timers, name2);
	if (!timer) {
		D(DLEVEL_STABLE,
		  "Can't remove fx timer since it does not exists\n");
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Stop the timer from running.
	 */
	ecore_del_event_timer(timer->name);

	/*
	 * Free the memory used by the timer.
	 */
	FREE(timer->name);
	FREE(timer);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_fx_clip_box_get_color - retrieve the current color of the fx clip box
 * @w: the widget to check the clip box color
 * @r: a pointer to the integer to store the red value
 * @g: a pointer to the integer to store the green value
 * @b: a pointer to the integer to store the blue value
 * @a: a pointer to the integer to store the alpha value
 *
 * Returns no value. The RGBA values of @w's fx clip box are stored into any
 * non-NULL pointers @r, @g, @b, and @a respectively.
 */
void ewl_fx_clip_box_get_color(Ewl_Widget * w, int *r, int *g, int *b, int *a)
{
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = ewl_window_find_window_by_widget(w);

	/*
	 * Once the widget is realized, grab the color of it's fx clip box.
	 */
	if (win->evas && w->fx_clip_box)
		evas_object_color_get(w->fx_clip_box, r, g, b, a);
	else {
		if (r)
			*r = -1;
		if (g)
			*g = -1;
		if (b)
			*b = -1;
		if (a)
			*a = -1;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_fx_clip_box_set_color - set the color for a widgets fx clip box
 * @w: the widget to set the color
 * @r: the red value to be set
 * @g: the green value to be set
 * @b: the blue value to be set
 * @a: the alpha value to be set
 *
 * Returns no value. Sets the RGBA colors of @w's fx clib box to @r, @g, @b,
 * and @a respectively.
 */
void ewl_fx_clip_box_set_color(Ewl_Widget * w, int r, int g, int b, int a)
{
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = ewl_window_find_window_by_widget(w);

	/*
	 * Change the color of the fx clip box if the widget is realized.
	 */
	if (win->evas && w->fx_clip_box)
		evas_object_color_set(w->fx_clip_box, r, g, b, a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_fx_plugin_load - load a plugin of a specified name
 * @name: the name of the plugin to load
 *
 * Returns a pointer to the fx prototype on success, NULL on failure.
 */
Ewl_FX_Proto   *ewl_fx_plugin_load(char *name)
{
	Ewd_Plugin     *plugin;
	Ewl_FX_Load_Function load;
	Ewl_FX_Proto   *fxp = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	/*
	 * Search the fx_group_id paths to find the plugin, if not found, then
	 * we can't start it.
	 */
	plugin = ewd_plugin_load(fx_group_id, name);

	if (!plugin) {
		DWARNING("Could not load fx plugin.\n");
		DRETURN_INT(0, DLEVEL_STABLE);
	}

	/*
	 * Attempt to find the load function from that plugin, and call it to
	 * get the prototype for the effect..
	 */
	load = ewd_plugin_call(plugin, "load");

	if (load)
		fxp = load();

	/*
	 * If no prototype was returned, or no load function was found, stop
	 * trying to use this effect.
	 */
	if (!fxp) {
		DWARNING("Symbol load() failed to be called in plugin\n"
			 "Please contact the author.\n");
		DRETURN_INT(0, DLEVEL_STABLE);
	}

	/*
	 * Add the prototype plugin to the hash table.
	 */
	fxp->plugin = plugin;
	ewd_hash_set(fx_protos, name, fxp);

	DRETURN_PTR(fxp, DLEVEL_STABLE);
}


/**
 * ewl_fx_get_available - get a list of available plugins
 * 
 *
 * Returns a list of available plugins on success, NULL on failure.
 */
Ewd_List       *ewl_fx_get_available(void)
{
	Ewd_List       *avail;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Get a list of all available paths in fx_group_id.
	 */
	avail = ewd_path_group_available(fx_group_id);

	DRETURN_PTR(avail, DLEVEL_STABLE);
}


/**
 * ewl_fx_start - start pending fx on a widget
 * @w: the widget to start pending fx
 * @pend: the pending effect to start
 *
 * Returns no value. Starts the pending fx @pend on widget @w.
 */
void ewl_fx_start(Ewl_Widget * w, Ewl_FX_Pending * pend)
{
	Ewl_FX_Pending *pend2;
	Ewl_FX_Proto   *proto, *proto2;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("pend", pend);

	/*
	 * We don't wan't to start the effect if :
	 * a) It's pending allready.
	 * b) It's queued allready.
	 */
	if (!w->fx || ewd_list_is_empty(w->fx) || pend->pending)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Fetch the requested effect proto
	 */
	proto = ewd_hash_get(fx_protos, pend->name);

	ewd_list_goto_first(w->fx);

	/*
	 * Now find out what other effects are pending
	 * if we already have similiar effects pending we want
	 * to que this one to start after the other one(s) stops.
	 */
	while ((pend2 = ewd_list_next(w->fx)) != NULL) {
		proto2 = ewd_hash_get(fx_protos, pend2->name);

		if (!pend2->pending)
			continue;

		if (!(proto->modifies & proto2->modifies))
			continue;

		/*
		 * The current effect is either :
		 * a) pending.
		 * b) queued, if this is the case we know there is already
		 *    a pending effect that wants to modify the same thing
		 *    and there is no longer any reason to traverse the list,
		 *    just add it to the que.
		 */
		pend->queued = 1;

		if (!w->fx_queues)
			w->fx_queues = ewd_list_new();

		ewd_list_append(w->fx_queues, pend);
		break;
	}

	/*
	 * Ok traversing the list of effects we didn't find any effect
	 * that would interfear with this effect, so just now just start it
	 * and mark it pending
	 */
	if (!pend->queued) {
		pend->pending = 1;

		proto->fx_start(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_fx_stop - stop a pending effect.
 * @w: the widget to stop the pending effect on.
 * @pend: the pending effect to stop.
 *
 * Returns no value. Here we stop the pending effect and search through
 * queued effects and start matching effects and remove them from the que list.
 */
void ewl_fx_stop(Ewl_Widget * w, Ewl_FX_Pending * pend)
{
	Ewl_FX_Pending *pend2, *pend3;
	Ewl_FX_Proto   *proto, *proto2, *proto3;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("pend", pend);

	/*
	 * Make sure all the necessary data structures have been setup,
	 * otherwise it's pointless to be here.
	 */
	if (!w->fx || ewd_list_is_empty(w->fx) ||
	    (!pend->pending && !pend->queued))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Retrieve the prototype for this pending effect.
	 */
	pend->pending = 0;
	proto = ewd_hash_get(fx_protos, pend->name);

	if (!proto)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Call the stop function for that particular prototype.
	 */
	proto->fx_stop(w);

	/*
	 * If there aren't any queued fx, our job is done.
	 */
	if (!w->fx_queues || ewd_list_is_empty(w->fx_queues))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Remove pend from the already queued fx if it is on the queue.
	 */
	if (pend->queued && ewd_list_goto(w->fx_queues, pend)) {
		ewd_list_remove(w->fx_queues);
		pend->queued = 0;
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Search all of the fx queues for the deleted prototype.
	 */
	ewd_list_goto_first(w->fx_queues);
	while ((pend2 = ewd_list_current(w->fx_queues)) != NULL) {
		proto2 = ewd_hash_get(fx_protos, pend2->name);

		/*
		 * If they modify same thing it may be possible this
		 * can be the one we should start.
		 * Otherwise continue searching.
		 */
		if (proto->modifies & proto2->modifies) {
			ewd_list_goto_first(w->fx);

			while ((pend3 = ewd_list_next(w->fx)) != NULL) {
				if (!pend3->pending || pend3 == pend)
					continue;

				proto3 = ewd_hash_get(fx_protos, pend3->name);

				/* If they match we do not want to
				 * start the queued effect because they interfear*/
				if (proto3->modifies & proto2->modifies)
					continue;

				/* Reaching this far means we don't
				 * interfear with any other effects pending
				 * next step is to remove the effect from the
				 * que and start it.
				 */
				break;
			}

			/* Remove the effect from the que and start it. */
			if (pend3) {
				ewd_list_remove(w->fx_queues);

				pend2->queued = 0;

				ewl_fx_start(w, pend2);

				break;
			}
		}

		ewd_list_next(w->fx_queues);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * The timer function determines when to stop the timer or add the ecore timer
 * again when needed.
 */
static inline void __ewl_fx_timer_func(int val, void *data)
{
	Ewl_FX_Timer   *timer;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	timer = data;

	/*
	 * Do nothing if no function for the timer.
	 */
	if (!timer->func)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Add the timer again if the number of times it's been executed has
	 * not reached the total needed.
	 */
	if (!timer->hits || timer->count <= timer->hits)
		ecore_add_event_timer(timer->name, timer->interval,
				      __ewl_fx_timer_func, ++timer->count,
				      timer);

	/*
	 * Execute the timer function.
	 */
	timer->func(timer);

	/*
	 * Stop the fx now that the number of hits has been reached.
	 */
	if (timer->hits && timer->count > timer->hits)
		ewl_fx_stop(timer->widget, timer->pend);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * Start the fx timer on the specified event.
 */
static inline void
__ewl_fx_func_start(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_fx_start(w, user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * Stop the fx timer on the specified event.
 */
static inline void
__ewl_fx_func_stop(Ewl_Widget * w, void *ev_data, void *user_data)
{

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_fx_stop(w, user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * Change the appearance of the widget.
 */
void
__ewl_fx_widget_appearance_changed(Ewl_Widget * w, void *ev_data,
				   void *user_data)
{
	char            wname[PATH_MAX];
	char            key[PATH_MAX];
	int             count, i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_fx_del_all(w);

	/*
	 * Check for a blank appearance string which will cause a segfault
	 * later.
	 */
	if (!w->appearance) {
		DWARNING("Widget does not have a appearance string\n");
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Determine the number of fx that need to be setup for the widgets.
	 */
	sscanf(w->appearance, "/%[^/]/", wname);
	snprintf(key, PATH_MAX, "/user/%s/count", wname);
	count = ewl_config_get_int("fx", key);

	/*
	 * If no fx for this widget then return.
	 */
	if (!count) {
		D(DLEVEL_STABLE, "No effects for widget");
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Load each of the fx in the db.
	 */
	for (i = 0; i < count; i++) {
		char           *name;
		int             start, end;
		int             cb_count, j;

		/*
		 * Determine the name of the current effect being loaded.
		 */
		snprintf(key, PATH_MAX, "/user/%s/%i/name", wname, i);
		name = ewl_config_get_str("fx", key);

		/*
		 * Determine the number of callbacks for this effect.
		 */
		snprintf(key, PATH_MAX, "/user/%s/%i/callbacks/count", wname,
			 i);
		cb_count = ewl_config_get_int("fx", key);

		/*
		 * For each count of the callbacks load them for the
		 * widgets.
		 */
		for (j = 0; j < cb_count; j++) {
			/*
			 * Determine the event that starts this effect.
			 */
			snprintf(key, PATH_MAX,
				 "/user/%s/%i/callbacks/%i/cb_start", wname, i,
				 j);
			start = ewl_config_get_int("fx", key);

			/*
			 * Determine the event that stops this effect.
			 */
			snprintf(key, PATH_MAX,
				 "/user/%s/%i/callbacks/%i/cb_stop", wname, i,
				 j);
			end = ewl_config_get_int("fx", key);

			/*
			 * Register the effect for execution.
			 */
			ewl_fx_add(w, name, start, end);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
