
#include <Ewl.h>


extern Ewl_Widget *last_selected;
extern Ewl_Widget *last_key;
extern Ewl_Widget *last_focused;
extern Ewl_Widget *dnd_widget;

void            __ewl_widget_show(Ewl_Widget * w, void *ev_data,
				  void *user_data);
void            __ewl_widget_hide(Ewl_Widget * w, void *ev_data,
				  void *user_data);
void            __ewl_widget_realize(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_widget_unrealize(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_widget_configure(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_widget_theme_update(Ewl_Widget * w, void *event_data,
					  void *user_data);
void            __ewl_widget_destroy(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_widget_reparent(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void            __ewl_widget_enable(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void            __ewl_widget_disable(Ewl_Widget * w, void *ev_data,
				     void *user_data);

static inline void __ewl_widget_ebits_destroy(Ewl_Widget *w);
static inline void __ewl_widget_cleanup_fx_clip(Ewl_Widget *w);

/**
 * ewl_widget_init - initialize a widgets to default values and callbacks
 * @w: the widget to initialize
 * @appearance: the key for the widgets theme appearance
 *
 * Returns no value. The widget @w is initialized to default values and is
 * assigned the default callbacks. The appearance key is assigned for easy
 * access to theme information.
 */
void ewl_widget_init(Ewl_Widget * w, char *appearance)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	DCHECK_PARAM_PTR("appearance", appearance);

	/*
	 * Set size fields on the object 
	 */
	ewl_object_init(EWL_OBJECT(w));

	/*
	 * Set up the necessary theme structures 
	 */
	ewl_theme_init_widget(w);

	w->state = EWL_STATE_NORMAL;

	/*
	ewl_fx_init_widget(w);
	*/

	/*
	 * Add the common callbacks that all widgets must perform
	 */
	ewl_callback_append(w, EWL_CALLBACK_SHOW, __ewl_widget_show, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE, __ewl_widget_hide, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_widget_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_UNREALIZE, __ewl_widget_unrealize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_widget_configure,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_widget_theme_update, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, __ewl_widget_destroy,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT, __ewl_widget_reparent,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_WIDGET_ENABLE, __ewl_widget_enable,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_WIDGET_DISABLE,
			    __ewl_widget_disable, NULL);

	ewl_widget_set_appearance(w, appearance);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_widget_realize - realize the specified widget
 * @w: the widget to realize
 *
 * Returns no value. The specified widget is realized (ie. actually displayed to
 * the screen).
 */
void ewl_widget_realize(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	w->visible |= EWL_VISIBILITY_REALIZED;

	if (w->parent && !REALIZED(w->parent))
		ewl_widget_realize(w->parent);

	ewl_callback_call(w, EWL_CALLBACK_REALIZE);
	ewl_widget_show(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_widget_unrealize - unrealize the specified widget
 * @w: the widget to unrealize
 *
 * Returns no value. The specified widget is unrealized (ie. no longer
 * displayed to the screen).
 */
void ewl_widget_unrealize(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	w->visible &= ~EWL_VISIBILITY_REALIZED;

	ewl_callback_call(w, EWL_CALLBACK_UNREALIZE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_widget_show - mark a widget as visible
 * @w: the widget to be marked as visible
 *
 * Returns no value. Marks the widget as visible so that it will be displayed
 * the next time through the rendering loop.
 */
void ewl_widget_show(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	w->visible |= EWL_VISIBILITY_SHOWN;

	if (w->parent && (REALIZED(w->parent) && !REALIZED(w)))
		ewl_widget_realize(w);

	ewl_callback_call(w, EWL_CALLBACK_SHOW);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_widget_hide - mark a widget as invisible
 * @w: the widget to be marked as invisible
 *
 * Returns no value. Marks the widget as invisible so that it will not be
 * displayed the next time through the rendering loop.
 */
void ewl_widget_hide(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!VISIBLE(w))
		DRETURN(DLEVEL_STABLE);

	w->visible = EWL_VISIBILITY_HIDDEN;

	ewl_callback_call(w, EWL_CALLBACK_HIDE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_widget_destroy - destroy the specified widget
 * @w: the widget to be destroyed
 *
 * Returns no value. The widget calls it's destroy callback to do any clean up
 * necessary and then free's the widget.
 */
void ewl_widget_destroy(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (last_selected == w)
		last_selected = NULL;

	if (last_key == w)
		last_key = NULL;

	if (last_focused == w)
		last_focused = NULL;

	if (dnd_widget == w)
		dnd_widget = NULL;

	ewl_callback_call(w, EWL_CALLBACK_DESTROY);

	ewl_callback_del_type(w, EWL_CALLBACK_DESTROY);

	FREE(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_widget_configure - initiate configuring of the specified widget
 * @w: the widget to configure
 *
 * Returns no value. The configure callback is triggered for the specified
 * widget, this should adjust the widgets size and position.
 */
void ewl_widget_configure(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (REALIZED(w) && VISIBLE(w))
		ewl_configure_request(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_widget_theme_update - initiate theme update of the specified widget
 * @w: the widget to update the theme
 *
 * Returns no value. The theme update callback is triggered for the specified
 * widget, this should adjust the widgets appearance.
 */
void ewl_widget_theme_update(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!REALIZED(w) || !w->appearance || !*w->appearance)
		DRETURN(DLEVEL_STABLE);

	ewl_callback_call(w, EWL_CALLBACK_THEME_UPDATE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_widget_reparent - initiate reparent of the specified widget
 * @w: the widget to reparent
 *
 * Returns no value. The reparent callback is triggered for the specified
 * widget, this should adjust the widgets attributes based on the new parent.
 */
void ewl_widget_reparent(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_call(w, EWL_CALLBACK_REPARENT);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_widget_set_data - attach the specified key / value pair to the widget
 * @w: the widget to own the key value pair
 * @k: the key that is associated with the data
 * @v: the data that is to be tracked
 *
 * Returns no value. Assigns a key / value pair with @k as the key and @v as
 * the value to the specified widget @w.
 */
void ewl_widget_set_data(Ewl_Widget * w, void *k, void *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (!w->data)
		w->data = ewd_hash_new(ewd_direct_hash, ewd_direct_compare);

	ewd_hash_set(w->data, k, v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_widget_del_data - remove the specified key / value pair from the widget
 * @w: the widget that owns the key value pair
 * @k: the key that is associated with the data
 *
 * Returns no value. Removes a key / value pair with @k as the key from the
 * specified widget @w.
 */
void ewl_widget_del_data(Ewl_Widget * w, void *k)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (!w->data)
		DRETURN(DLEVEL_STABLE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_widget_get_data - retrieve the specified key / value pair from the widget
 * @w: the widget that owns the key value pair
 * @k: the key that is associated with the data
 *
 * Returns the value associated with @k on success, NULL on failure. Retrieves a
 * key / value pair with @k as the key from the specified widget @w.
 */
void           *ewl_widget_get_data(Ewl_Widget * w, void *k)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	if (!w->data)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	DRETURN_PTR(ewd_hash_get(w->data, k), DLEVEL_STABLE);
}

/**
 * ewl_widget_set_appearance - change the appearance of the specified widget
 * @w: the widget to change the appearance
 * @appearance: the new key for the widgets appearance
 *
 * Returns no value. Changes the key associated with the widgets appearance
 * and calls the theme update callback to initiate the change.
 */
void ewl_widget_set_appearance(Ewl_Widget * w, char *appearance)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	IF_FREE(w->appearance);

	/*
	 * The base appearance is used for determining the theme key of the
	 * widget
	 */
	w->appearance = strdup(appearance);

	ewl_widget_rebuild_appearance(w);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_widget_get_appearance - retrieve the appearance key of the widget
 * @w: the widget to retrieve the appearance key
 *
 * Returns a pointer to the appearance key string on success, NULL on failure.
 */
char           *ewl_widget_get_appearance(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("w", w, NULL);

	return (w->appearance ? strdup(w->appearance) : NULL);
}

/**
 * ewl_widget_update_appearance - update the appearance of the widget to a state
 * @w: the widget to update the appearance
 * @state: the new state of the widget
 *
 * Returns no value. Changes the appearance of the widget @w depending on the
 * state string passed by @state.
 */
void ewl_widget_update_appearance(Ewl_Widget * w, char *state)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("state", state);

	/*
	 * Set up the ebits object on the widgets evas
	 */
	if (!w->ebits_object)
		ewl_widget_theme_update(w);

	if (!w->ebits_object)
		DRETURN(DLEVEL_STABLE);

	ebits_set_named_bit_state(w->ebits_object, "Base", state);

	DRETURN(DLEVEL_STABLE);
}

/**
 * ewl_widget_set_parent - change the parent of the specified widget
 * @w: the widget to change the parent
 * @p: the new parent of the widget
 *
 * Returns no value. Changes the parent of the widget @w, to the container @p.
 * The reparent callback is triggered to notify children of @w of the change
 * in parent.
 */
void ewl_widget_set_parent(Ewl_Widget * w, Ewl_Widget * p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("p", p);

	/*
	 * A widget cannot be the child of multiple widgets, so remove it
	 * from a previous parent before adding to this parent.
	 */
	if (w->parent)
		ewl_container_remove_child(EWL_CONTAINER(w->parent), w);

	w->parent = p;

	ewl_widget_reparent(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_widget_enable(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!(w->state & EWL_STATE_DISABLED))
		return;
	else {
		w->state = EWL_STATE_NORMAL;
		ewl_callback_call(w, EWL_CALLBACK_WIDGET_ENABLE);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_widget_disable(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->state & EWL_STATE_DISABLED)
		return;
	else {
		w->state = EWL_STATE_DISABLED;
		ewl_callback_call(w, EWL_CALLBACK_WIDGET_DISABLE);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_widget_rebuild_appearance(Ewl_Widget *w)
{
	char *base;
	char path[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("w->appearance", w->appearance);

	base = strrchr(w->appearance, '/');
	if (base) {
		*base = '\0';
		base++;
	}
	else
		base = w->appearance;

	snprintf(path, PATH_MAX, "%s/%s",
			(w->parent ? w->parent->appearance : ""), base);

	base = w->appearance;
	FREE(base);

	w->appearance = strdup(path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the series of operations common to every widget when
 * they are destroyed. This should ALWAYS be the the last callback
 * in the chain.
 */
void __ewl_widget_destroy(Ewl_Widget * w, void *ev_data, void *data)
{
	Ewl_Window     *win;
	Ewd_List       *destroy_cbs;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * First find it's parent window so we can destroy the evas objects.
	 */
	win = ewl_window_find_window_by_widget(w);

	/*
	 * First remove the parents reference to this widget to avoid bad
	 * references.
	 */
	if (w->parent)
		ewl_container_remove_child(EWL_CONTAINER(w->parent), w);

	__ewl_widget_ebits_destroy(w);
	__ewl_widget_cleanup_fx_clip(w);

	/*
	 * Free up appearance related information
	 */
	ewl_theme_deinit_widget(w);
	IF_FREE(w->appearance);

	/*
	 * Clear out the callbacks, this is a bit tricky because we don't want
	 * to continue using this widget after the callbacks have been
	 * deleted. So we remove the callbacks of type destroy and then clear
	 * the remaining callbacks. This preserves the list of the destroy
	 * type so we don't get a segfault.
	 */
	destroy_cbs = w->callbacks[EWL_CALLBACK_DESTROY];
	w->callbacks[EWL_CALLBACK_DESTROY] = NULL;
	ewl_callback_clear(w);
	w->callbacks[EWL_CALLBACK_DESTROY] = destroy_cbs;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Every widget must show it's fx_clip_box to be seen
 */
void __ewl_widget_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->fx_clip_box)
		evas_object_show(w->fx_clip_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Every widget must hide it's fx_clip_box in order to hide
 */
void __ewl_widget_hide(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->fx_clip_box)
		evas_object_hide(w->fx_clip_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for realizing a widget
 */
void __ewl_widget_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;
	Ewl_Container  *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = ewl_window_find_window_by_widget(w);

	/*
	 * Create the fx clip box where special fx can be drawn to affect the
	 * entire contents of the widget
	 */
	w->fx_clip_box = evas_object_rectangle_add(win->evas);
	evas_object_layer_set(w->fx_clip_box, LAYER(w));

	pc = EWL_CONTAINER(w->parent);

	/*
	 * Clip the fx_clip_box to the parent clip_box.
	 */
	if (pc && pc->clip_box)
		evas_object_clip_set(w->fx_clip_box, pc->clip_box);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for unrealizing a widget
 */
void __ewl_widget_unrealize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;
	Ewl_Container  *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = ewl_window_find_window_by_widget(w);

	pc = EWL_CONTAINER(w->parent);

	/*
	 * Unclip the fx_clip_box to the parent clip_box.
	 */
	if (pc && pc->clip_box)
		evas_object_clip_unset(w->fx_clip_box);

	/*
	 * Destroy the clip box used for fx.
	 */
	if (w->fx_clip_box) {
		evas_object_del(w->fx_clip_box);
	}

	w->visible &= ~EWL_VISIBILITY_REALIZED;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for configuring a widget
 */
void __ewl_widget_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = ewl_window_find_window_by_widget(w);

	/*
	 * Adjust the clip box to display the widget.
	 */
	if (w->fx_clip_box) {
		evas_object_move(w->fx_clip_box,
				CURRENT_X(w) - INSET_LEFT(w),
				CURRENT_Y(w) - INSET_TOP(w));
		evas_object_resize(w->fx_clip_box,
				CURRENT_W(w) + INSET_LEFT(w) + INSET_RIGHT(w),
				CURRENT_H(w) + INSET_TOP(w) + INSET_BOTTOM(w));
	}

	/*
	 * Move the base ebits object to the correct size and position
	 */
	if (w->ebits_object) {
		ebits_move(w->ebits_object,
				CURRENT_X(w) - INSET_LEFT(w),
				CURRENT_Y(w) - INSET_TOP(w));
		ebits_resize(w->ebits_object,
				CURRENT_W(w) + INSET_LEFT(w) + INSET_RIGHT(w),
				CURRENT_H(w) + INSET_TOP(w) + INSET_BOTTOM(w));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * This simplifies updating the appearance of the widget
 */
void __ewl_widget_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             len;
	char           *i = NULL;
	char           *key = NULL;
	char           *visible;
	Ewl_Window     *win = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	__ewl_widget_ebits_destroy(w);

	/*
	 * Calculate the length of the base key string, then allocate
	 * the memory for it plus room for placing /visible at the end.
	 */
	len = strlen(w->appearance) + 14;
	key = NEW(char, len);
	snprintf(key, len, "%s/base/visible", w->appearance);

	/*
	 * Determine the widgets visibility, return if not visible
	 */
	visible = ewl_theme_data_get_str(w, key);

	if (!visible || !strncasecmp(visible, "no", 2)) {
		FREE(key);
		DRETURN(DLEVEL_STABLE);
	}

	/*
	 * Retrieve the path to the ebits file that will be loaded
	 * return if no file to be loaded.
	 */
	snprintf(key, len, "%s/base", w->appearance);

	i = ewl_theme_image_get(w, key);
	FREE(key);

	if (i) {
		win = ewl_window_find_window_by_widget(w);
		if (!win)
			DRETURN(DLEVEL_STABLE);

		/*
		 * Load the ebits object
		 */
		w->ebits_object = ebits_load(i);
		FREE(i);
	}

	/*
	 * Set up the ebits object on the widgets evas
	 */
	if (w->ebits_object) {
		int             i_l = 0, i_r = 0, i_t = 0, i_b = 0;

		ebits_add_to_evas(w->ebits_object, win->evas);
		ebits_set_layer(w->ebits_object, LAYER(w));
		if (w->fx_clip_box)
			ebits_set_clip(w->ebits_object, w->fx_clip_box);
		ebits_show(w->ebits_object);

		/*
		 * Set the insets based on cached information from the
		 * ebit, this can be overwritten later.
		 */
		ebits_get_insets(w->ebits_object, &i_l, &i_r, &i_t, &i_b);
		ewl_object_set_insets(EWL_OBJECT(w), i_l, i_r, i_t, i_b);

		if (w->state & EWL_STATE_DISABLED)
			ebits_set_named_bit_state(w->ebits_object, "Base",
					"disabled");

		/*
		 * Propagate minimum sizes from the bit theme to the widget.
		 */
		ebits_get_min_size(w->ebits_object, &i_l, &i_t);

		if (MINIMUM_W(w) == EWL_OBJECT_MIN_SIZE)
			ewl_object_set_minimum_w(EWL_OBJECT(w), i_l);

		if (MINIMUM_H(w) == EWL_OBJECT_MIN_SIZE)
			ewl_object_set_minimum_h(EWL_OBJECT(w), i_t);

		/*
		 * Propagate maximum sizes from the bit theme to the widget.
		 */
		ebits_get_max_size(w->ebits_object, &i_l, &i_t);

		if (MAXIMUM_W(w) == EWL_OBJECT_MAX_SIZE)
			ewl_object_set_maximum_w(EWL_OBJECT(w), i_l);

		if (MAXIMUM_H(w) == EWL_OBJECT_MAX_SIZE)
			ewl_object_set_maximum_h(EWL_OBJECT(w), i_t);
	}

	DRETURN(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for reparenting a widget
 */
void __ewl_widget_reparent(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Evas           *oevas;
	Ewl_Container  *pc;
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	pc = EWL_CONTAINER(w->parent);

	if (pc)
		LAYER(w) = LAYER(pc) + 5;
	else
		LAYER(w) = -1000;

	ewl_widget_rebuild_appearance(w);

	/*
	 * If the new parent is on a different evas, we must re-realize it.
	 */
	if (REALIZED(w) && w->fx_clip_box) {
		oevas = evas_object_evas_get(w->fx_clip_box);
		win = ewl_window_find_window_by_widget(w);
		if (oevas != win->evas)
			ewl_widget_unrealize(w);
	}
	else {
		/*
		 * Set up the clip box again if necessary
		 */
		if (pc && pc->clip_box && w->fx_clip_box)
			evas_object_clip_set(w->fx_clip_box, pc->clip_box);
	}

	if (REALIZED(pc) && VISIBLE(w) && !REALIZED(w))
		ewl_widget_realize(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void __ewl_widget_enable(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_update_appearance(w, "normal");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_widget_disable(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_update_appearance(w, "disabled");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static inline void __ewl_widget_ebits_destroy(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Destroy old image (if any) 
	 */
	if (w->ebits_object) {
		ebits_hide(w->ebits_object);
		ebits_unset_clip(w->ebits_object);
		ebits_free(w->ebits_object);
		w->ebits_object = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static inline void __ewl_widget_cleanup_fx_clip(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Destroy the fx_clip_box of the widget
	 */
	if (w->fx_clip_box) {
		evas_object_hide(w->fx_clip_box);
		evas_object_clip_unset(w->fx_clip_box);
		evas_object_del(w->fx_clip_box);
		w->fx_clip_box = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
