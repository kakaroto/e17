#include <Ewl.h>


Ewl_Widget *last_selected = NULL;
Ewl_Widget *last_key = NULL;
Ewl_Widget *last_focused = NULL;
Ewl_Widget *dnd_widget = NULL;

static void ewl_widget_rebuild_appearance(Ewl_Widget *w);
static void ewl_widget_get_theme_padding(Ewl_Widget *w, int *l, int *r,
					 int *t, int *b);
static void ewl_widget_get_theme_insets(Ewl_Widget *w, int *l, int *r,
					int *t, int *b);

/**
 * @param w: the widget to initialize
 * @param appearance: the key for the widgets theme appearance
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a widget to default values and callbacks
 *
 * The widget w is initialized to default values and is
 * assigned the default callbacks. The appearance key is assigned for easy
 * access to theme information.
 */
int ewl_widget_init(Ewl_Widget * w, char *appearance)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);
	DCHECK_PARAM_PTR_RET("appearance", appearance, FALSE);

	/*
	 * Set size fields on the object 
	 */
	if (!ewl_object_init(EWL_OBJECT(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	/*
	 * Set up the necessary theme structures 
	 */
	if (!ewl_theme_init_widget(w))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_object_remove_state(EWL_OBJECT(w), EWL_FLAGS_STATE_MASK);
	LAYER(w) = 10;

	/*
	 * Add the common callbacks that all widgets must perform
	 */
	ewl_callback_append(w, EWL_CALLBACK_SHOW, ewl_widget_show_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE, ewl_widget_hide_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, ewl_widget_realize_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_UNREALIZE, ewl_widget_unrealize_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_widget_configure_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, ewl_widget_destroy_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT, ewl_widget_reparent_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_WIDGET_ENABLE, ewl_widget_enable_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_WIDGET_DISABLE,
			ewl_widget_disable_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN, ewl_widget_focus_in_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT, ewl_widget_focus_out_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_widget_mouse_down_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, ewl_widget_mouse_up_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
			    ewl_widget_mouse_move_cb, NULL);

	ewl_widget_set_appearance(w, appearance);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to realize
 * @return Returns no value.
 * @brief Realize the specified widget.
 *
 * The specified widget is realized, ie. actually displayed to the screen.
 */
void ewl_widget_realize(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	/*
	 * The parent's realize function will get us here again.
	 */
	if (w->parent && !REALIZED(w->parent))
		ewl_widget_realize(w->parent);
	else if (w->parent || ewl_object_get_toplevel(EWL_OBJECT(w))) {
		ewl_callback_call(w, EWL_CALLBACK_REALIZE);
		ewl_widget_show(w);
	}

	/*
	 * If somehow the child doesn't cause the parent to get a configure
	 * request, this will catch it.
	 */
	if (w->parent)
		ewl_widget_configure(w->parent);
	else
		ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to unrealize
 * @return Returns no value.
 * @brief Unrealize the specified widget
 *
 * The specified widget is unrealized, ie. no longer displayed to the screen.
 */
void ewl_widget_unrealize(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	ewl_callback_call(w, EWL_CALLBACK_UNREALIZE);

	ewl_object_remove_visible(EWL_OBJECT(w), EWL_FLAG_VISIBLE_REALIZED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be marked as visible
 * @return Returns no value.
 * @brief mark a widget as visible
 *
 * Marks the widget as visible so that it will be displayed the next time
 * through the rendering loop.
 */
void ewl_widget_show(Ewl_Widget * w)
{
	Ewl_Container *pc;
	unsigned int flags = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	pc = EWL_CONTAINER(w->parent);
	if (HIDDEN(w))
		ewl_object_add_visible(EWL_OBJECT(w), EWL_FLAG_VISIBLE_SHOWN);

	if (REALIZED(w)) {
		ewl_callback_call(w, EWL_CALLBACK_SHOW);
	}
	else if (pc) {
		flags = ewl_object_get_flags(EWL_OBJECT(pc),
					     EWL_FLAG_QUEUED_RSCHEDULED |
					     EWL_FLAG_VISIBLE_REALIZED);
	}
	else {
		flags = ewl_object_get_flags(EWL_OBJECT(w),
					     EWL_FLAG_PROPERTY_TOPLEVEL);
	}

	if (flags)
		ewl_realize_request(w);

	if (w->parent)
		ewl_widget_configure(w->parent);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param w: the widget to be marked as invisible
 * @return Returns no value.
 * @brief Mark a widget as invisible
 *
 * Marks the widget as invisible so that it will not be displayed the next time
 * through the rendering loop.
 */
void ewl_widget_hide(Ewl_Widget * w)
{
	Ewl_Container *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (HIDDEN(w))
		DRETURN(DLEVEL_STABLE);

	pc = EWL_CONTAINER(w->parent);
	if (pc)
		ewl_container_call_child_remove(pc, w);

	ewl_object_remove_visible(EWL_OBJECT(w), EWL_FLAG_VISIBLE_SHOWN);
	ewl_callback_call(w, EWL_CALLBACK_HIDE);

	if (w->parent)
		ewl_widget_configure(w->parent);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be destroyed
 * @return Returns no value.
 * @brief Destroy the specified widget
 *
 * The widget calls it's destroy callback to do any clean up necessary and then
 * free's the widget.
 */
void ewl_widget_destroy(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (ewl_object_has_queued(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	if (last_selected == w)
		last_selected = NULL;

	if (last_key == w)
		last_key = NULL;

	if (last_focused == w)
		last_focused = NULL;

	if (dnd_widget == w)
		dnd_widget = NULL;

	if (w->parent)
		ewl_container_remove_child(EWL_CONTAINER(w->parent), w);

	ewl_destroy_request(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param w: the widget to configure
 * @return Returns no value.
 * @brief Initiate configuring of the specified widget
 *
 * The configure callback is triggered for the specified widget, this should
 * adjust the widgets size and position.
 */
void ewl_widget_configure(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (/* FIXME: !REALIZED(w) | */ !VISIBLE(w))
		DRETURN(DLEVEL_STABLE);

	ewl_configure_request(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to update the theme
 * @return Returns no value.
 * @brief Initiate theme update of the specified widget
 *
 * The theme update callback is triggered for the specified widget, this should
 * adjust the widgets appearance.
 */
void ewl_widget_theme_update(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!REALIZED(w) || !w->appearance || !*w->appearance)
		DRETURN(DLEVEL_STABLE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param w: the widget to reparent
 * @return Returns no value.
 * @brief initiate reparent of the specified widget
 *
 * The reparent callback is triggered for the specified widget, this should
 * adjust the widgets attributes based on the new parent.
 */
void ewl_widget_reparent(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_call(w, EWL_CALLBACK_REPARENT);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to own the key value pair
 * @param k: the key that is associated with the data
 * @param v: the data that is to be tracked
 * @return Returns no value.
 * @brief Attach the specified key / value pair to the widget
 *
 * Assigns a key / value pair with k as the key and v as the value to the
 * specified widget w.
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
 * @param w: the widget that owns the key value pair
 * @param k: the key that is associated with the data
 * @return Returns no value.
 * @brief Remove the specified key / value pair from the widget
 *
 * Removes a key / value pair with k as the key from the specified widget w.
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
 * @param w: the widget that owns the key value pair
 * @param k: the key that is associated with the data
 * @return Returns the value associated with k on success, NULL on failure.
 * @brief retrieve the specified key / value pair from the widget
 *
 * Retrieves a key / value pair with k as the key from the specified widget w.
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
 * @param w: the widget to change the appearance
 * @param appearance: the new key for the widgets appearance
 * @return Returns no value.
 * @brief Change the appearance of the specified widget
 *
 * Changes the key associated with the widgets appearance and calls the theme
 * update callback to initiate the change.
 */
void ewl_widget_set_appearance(Ewl_Widget * w, char *appearance)
{
	char *current;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Only continue if the appearance has changed.
	 */
	if (w->appearance) {
		current = strrchr(w->appearance, '/');
		if (current) {
			current++;
			if (!strcmp(current, appearance))
				DRETURN(DLEVEL_STABLE);
		}

		FREE(w->appearance);
	}

	/*
	 * The base appearance is used for determining the theme key of the
	 * widget
	 */
	w->appearance = strdup(appearance);

	ewl_widget_rebuild_appearance(w);
	if (REALIZED(w)) {
		ewl_widget_unrealize(w);
		ewl_widget_realize(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve the appearance key
 * @return Returns a pointer to the appearance key string on success, NULL on
 * failure.
 * @brief Retrieve the appearance key of the widget
 */
char           *ewl_widget_get_appearance(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("w", w, NULL);

	return (w->appearance ? strdup(w->appearance) : NULL);
}

/**
 * @param w: the widget to update the appearance
 * @param state: the new state of the widget
 * @return Returns no value.
 * @brief Update the appearance of the widget to a state
 *
 * Changes the appearance of the widget depending on the state string passed by
 * the state parameter.
 */
void ewl_widget_set_state(Ewl_Widget * w, char *state)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("state", state);

	if (w->bit_state && !strcmp(w->bit_state, state))
		DRETURN(DLEVEL_STABLE);

	IF_FREE(w->bit_state);
	w->bit_state = strdup(state);

	if (w->theme_object)
		edje_object_signal_emit(w->theme_object, state, "EWL");

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param w: the widget to change the parent
 * @param p: the new parent of the widget
 * @return Returns no value.
 * @brief change the parent of the specified widget
 *
 * Changes the parent of the widget w, to the container p. The reparent
 * callback is triggered to notify children of w of the change in parent.
 */
void ewl_widget_set_parent(Ewl_Widget * w, Ewl_Widget * p)
{
	Ewl_Widget *tmp;
	Ewl_Container *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	op = EWL_CONTAINER(w->parent);
	if (op == EWL_CONTAINER(p))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Verify this will not result in recursively nested widgets.
	 */
	tmp = p;
	while (tmp) {
		if (tmp == w) {
			ewl_print_warning();
			DRETURN(DLEVEL_STABLE);
		}
		tmp = tmp->parent;
	}

	w->parent = p;

	/*
	 * A widget cannot be the child of multiple widgets, so remove it
	 * from a previous parent before adding to this parent.
	 */
	if (op) {
		ewl_container_remove_child(op, w);
		if (!p)
			ewl_callback_del(w, EWL_CALLBACK_DESTROY,
				 ewl_widget_child_destroy_cb);
	}

	/*
	 * A widget that has not had a previous parent needs the parent
	 * destruction callback added.
	 */
	if (p) {
		if (!op)
			ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
					ewl_widget_child_destroy_cb, NULL);
		if (VISIBLE(w)) {
			if (REALIZED(w)) {
				ewl_callback_call(w, EWL_CALLBACK_REPARENT);
				ewl_container_call_child_add(EWL_CONTAINER(p),
							     w);
			}
			else {
				ewl_realize_request(w);
			}
		}

		ewl_widget_configure(p);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to re-enable
 * @return Returns no value.
 * @brief Re-enable a disabled widget
 *
 * Re-enables a previously disabled widget.
 */
void ewl_widget_enable(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!ewl_object_has_state(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		return;
	else {
		ewl_object_remove_state(EWL_OBJECT(w), EWL_FLAGS_STATE_MASK);
		ewl_object_add_state(EWL_OBJECT(w), EWL_FLAG_STATE_NORMAL);
		ewl_callback_call(w, EWL_CALLBACK_WIDGET_ENABLE);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to disable
 * @return Returns no value. Disables a specified widget.
 * @brief Prevent a widget from receiving any events
 *
 * This prevents that widget from receiving any user input events.
 */
void ewl_widget_disable(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (ewl_object_has_state(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		return;
	else {
		ewl_object_remove_state(EWL_OBJECT(w), EWL_FLAGS_STATE_MASK);
		ewl_object_add_state(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED);
		ewl_callback_call(w, EWL_CALLBACK_WIDGET_DISABLE);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to change relative layers
 * @param layer: the new relative layer of the widget
 * @return Returns no value.
 * @brief Set the relative layer to it's parent
 *
 * Changes the current layer of @a w relative to it's parent. The default
 * value is 5.
 */
void ewl_widget_set_layer(Ewl_Widget *w, int layer)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Modify the current layer of the objects if realized.
	 */
	if (REALIZED(w) && w->fx_clip_box) {
		int temp;

		temp = evas_object_layer_get(w->fx_clip_box) + layer - LAYER(w);
		evas_object_layer_set(w->fx_clip_box, temp);
		if (w->theme_object)
			evas_object_layer_set(w->theme_object, temp);
	}

	LAYER(w) = layer;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve current relative layer
 * @return Returns a widgets current layer relative to it's parent.
 * @brief Retrieve a widgets layer relative to it's parent.
 */
int ewl_widget_get_layer(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);

	DRETURN_INT(LAYER(w), DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve sum of layers
 * @return Returns the absolute layer the widget is placed on.
 * @brief Sums the layers of a widgets parents to determine it's absolute layer.
 */
int ewl_widget_get_layer_sum(Ewl_Widget *w)
{
	int sum = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);

	while (!REALIZED(w) && w->parent) {
		sum += LAYER(w);
		w = w->parent;
	}

	if (REALIZED(w) && w->fx_clip_box)
		sum += evas_object_layer_get(w->fx_clip_box);
	else
		sum += LAYER(w);

	DRETURN_INT(sum, DLEVEL_STABLE);
}

/**
 * @param w: the widget to be moved to the front of the focus list
 * @return Returns no value.
 * @brief Changes the order in the embed so @a w receives focus first on tab.
 *
 * This moves the widget @a w to the front of the tab order list in the embed
 * that holds it. This is the recommended method for manipulating tab order,
 * The embed versions should only be accessed internally if you understand
 * their ramifications.
 */
void ewl_widget_push_tab_order(Ewl_Widget *w)
{
	Ewl_Embed *e;

	DCHECK_PARAM_PTR("w", w);
	DENTER_FUNCTION(DLEVEL_STABLE);

	e = ewl_embed_find_by_widget(w);
	ewl_embed_push_tab_order(EWL_EMBED(e), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to display ancestry tree
 * @return Returns no value.
 * @brief Prints to stdout the tree of widgets that are parents of a widget.
 */
void ewl_widget_print_tree(Ewl_Widget *w)
{
	int i = 0;

	while (w) {
		int j;
		for (j = 0; j < i; j++)
			printf("\t");

		ewl_widget_print(w);

		w = w->parent;
		i++;
	}
}

/**
 * @param w: the widget to print info
 * @return Returs no value.
 * @brief Prints info for debugging a widget's state information.
 */
void ewl_widget_print(Ewl_Widget *w)
{
	printf("%p:%s geometry (%d, %d) %d x %d, %s, %s\n",
			w, w->appearance,
			ewl_object_get_current_x(EWL_OBJECT(w)),
			ewl_object_get_current_y(EWL_OBJECT(w)),
			ewl_object_get_current_w(EWL_OBJECT(w)),
			ewl_object_get_current_h(EWL_OBJECT(w)),
			(VISIBLE(w) ? "visible" : "not visible"),
			(REALIZED(w) ? "realized" : "not realized"));
}

/**
 * @param w: the widget to mark as internally used
 * @param val: a boolean to indicate the state of the internal flag
 * @return Returns no value.
 * @brief Marks a widget as one used internally to another container.
 *
 * Indicate whether a widget is to be used as an internal decoration on
 * another widget. This allows for using some of the standard container
 * functions to access the contents of complex widgets w/o fear of damaging
 * internal layout structure.
 */
void ewl_widget_set_internal(Ewl_Widget *w, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (val)
		ewl_object_add_flags(EWL_OBJECT(w), EWL_FLAG_PROPERTY_INTERNAL,
				EWL_FLAGS_PROPERTY_MASK);
	else
		ewl_object_remove_flags(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_INTERNAL,
				EWL_FLAGS_PROPERTY_MASK);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to query the state of the internal flag
 * @return Returns TRUE if the widget is marked internal, otherwise FALSE.
 * @brief Checks the widget for the internal flag.
 */
unsigned int ewl_widget_is_internal(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_object_has_flags(EWL_OBJECT(w), EWL_FLAG_PROPERTY_INTERNAL,
				EWL_FLAGS_PROPERTY_MASK))
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to mark as unclipped
 * @param val: the state of the clipping flag
 * @return Returns no value.
 * @brief Marks whether the widget should be clipped at it's boundaries.
 */
void ewl_widget_set_clipped(Ewl_Widget *w, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (val)
		ewl_object_remove_flags(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP,
				EWL_FLAGS_VISIBLE_MASK);
	else
		ewl_object_add_flags(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP,
				EWL_FLAGS_VISIBLE_MASK);

	if (!REALIZED(w) || (val && w->fx_clip_box) ||
			(!val && !w->fx_clip_box))
		DRETURN(DLEVEL_STABLE);

	if (val) {
		Ewl_Embed *emb;

		emb = ewl_embed_find_by_widget(w);
		if (!emb || !emb->evas)
			DRETURN(DLEVEL_STABLE);

		w->fx_clip_box = evas_object_rectangle_add(emb->evas);
		ewl_widget_configure(w);
	}
	else {
		ewl_evas_object_destroy(w->fx_clip_box);
		w->fx_clip_box = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to check if it clips it's theme object
 * @return Returns TRUE if the widget clips, otherwise FALSE.
 * @brief Checks if a widget clips it's theme object.
 */
unsigned int ewl_widget_is_clipped(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_object_has_flags(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP,
			EWL_FLAGS_VISIBLE_MASK))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to receive keyboard focus
 * @return Returns no value.
 * @brief Changes the keyboard focus to the widget @a w.
 */
void ewl_widget_send_focus(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	last_key = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the currnetly focused widget.
 * @brief Retrieve the currently focused widget.
 */
Ewl_Widget *ewl_widget_get_focused()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(last_key, DLEVEL_STABLE);
}

static void ewl_widget_rebuild_appearance(Ewl_Widget *w)
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

	FREE(w->appearance);
	w->appearance = strdup(path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the series of operations common to every widget when
 * they are destroyed. This should ALWAYS be the the last callback
 * in the chain.
 */
void ewl_widget_destroy_cb(Ewl_Widget * w, void *ev_data, void *data)
{
	Ewd_List       *destroy_cbs;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * First remove the parents reference to this widget to avoid bad
	 * references.
	 */
	if (w->parent)
		ewl_container_remove_child(EWL_CONTAINER(w->parent), w);

	ewl_widget_unrealize(w);

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
	destroy_cbs = EWL_CALLBACK_LIST_POINTER(w, EWL_CALLBACK_DESTROY);
	EWL_CALLBACK_LIST_ASSIGN(w, EWL_CALLBACK_DESTROY, NULL);
	ewl_callback_clear(w);
	EWL_CALLBACK_LIST_ASSIGN(w, EWL_CALLBACK_DESTROY, destroy_cbs);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Every widget must show it's fx_clip_box to be seen
 */
void ewl_widget_show_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->fx_clip_box)
		evas_object_show(w->fx_clip_box);
	if (w->theme_object)
		evas_object_show(w->theme_object);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Every widget must hide it's fx_clip_box in order to hide
 */
void ewl_widget_hide_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->fx_clip_box)
		evas_object_hide(w->fx_clip_box);
	if (w->theme_object)
		evas_object_hide(w->theme_object);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for realizing a widget
 */
void ewl_widget_realize_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             len;
	int             l = 0, r = 0, t = 0, b = 0;
	int             i_l = 0, i_r = 0, i_t = 0, i_b = 0;
	int             p_l = 0, p_r = 0, p_t = 0, p_b = 0;
	char           *i = NULL;
	char           *key = NULL;
	char           *group = NULL;
	Evas_Coord      width, height;
	Ewl_Embed      *emb = NULL;
	Ewl_Container  *pc = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_find_by_widget(w);

	/*
	 * Create the fx clip box where special fx can be drawn to affect the
	 * entire contents of the widget
	 */
	if (!ewl_object_get_flags(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP))
		w->fx_clip_box = evas_object_rectangle_add(emb->evas);

	if (w->fx_clip_box)
		evas_object_layer_set(w->fx_clip_box,
				ewl_widget_get_layer_sum(w));

	pc = EWL_CONTAINER(w->parent);

	/*
	 * Clip the fx_clip_box to the parent clip_box.
	 */
	if (pc && pc->clip_box && w->fx_clip_box)
		evas_object_clip_set(w->fx_clip_box, pc->clip_box);

	/*
	 * Get and save the current insets and padding of the widget, this
	 * will be used to calculate any added at runtime.
	 */
	if (w->theme_object) {
		ewl_widget_get_theme_insets(w, &i_l, &i_r, &i_t, &i_b);
		ewl_widget_get_theme_padding(w, &p_l, &p_r, &p_t, &p_b);
	}

	ewl_object_get_insets(EWL_OBJECT(w), &l, &r, &t, &b);

	i_l = l - i_l;
	i_r = r - i_r;
	i_t = t - i_t;
	i_b = b - i_b;

	ewl_object_get_padding(EWL_OBJECT(w), &l, &r, &t, &b);

	p_l = l - p_l;
	p_r = r - p_r;
	p_t = t - p_t;
	p_b = b - p_b;

	/*
	 * Calculate the length of the base key string, then allocate
	 * the memory for it plus room for placing /visible at the end.
	 */
	len = strlen(w->appearance) + 7;
	key = NEW(char, len);
	if (!key)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Retrieve the path to the theme file that will be loaded
	 * return if no file to be loaded. Also get the group name in the
	 * theme file.
	 */
	snprintf(key, len, "%s/file", w->appearance);
	i = ewl_theme_image_get(w, key);

	snprintf(key, len, "%s/group", w->appearance);
	group = ewl_theme_data_get_str(w, key);

	FREE(key);

	if (group) {
		emb = ewl_embed_find_by_widget(w);
		if (!emb)
			DRETURN(DLEVEL_STABLE);

		/*
		 * Load the theme object
		 */
		w->theme_object = edje_object_add(emb->evas);
		
		edje_object_file_set(w->theme_object, i, group);
		FREE(group);
	}

	IF_FREE(i);

	/*
	 * Set up the theme object on the widgets evas
	 */
	if (w->theme_object) {

		evas_object_layer_set(w->theme_object,
				ewl_widget_get_layer_sum(w));
		if (w->fx_clip_box)
			evas_object_clip_set(w->theme_object, w->fx_clip_box);
		evas_object_show(w->theme_object);

		/*
		 * Set the insets based on cached information from the
		 * ebit, this can be overwritten later.
		 */
		ewl_widget_get_theme_insets(w, &l, &r, &t, &b);
		ewl_object_set_insets(EWL_OBJECT(w), l + i_l, r + i_r, t + i_t,
				b + i_b);

		ewl_widget_get_theme_padding(w, &l, &r, &t, &b);
		ewl_object_set_padding(EWL_OBJECT(w), l + p_l, r + p_r, t + p_t,
				b + p_b);

		if (ewl_object_has_state(EWL_OBJECT(w),
					EWL_FLAG_STATE_DISABLED))
			edje_object_signal_emit(w->theme_object, "disabled",
						"EWL");

		/*
		 * Propagate minimum sizes from the bit theme to the widget.
		 */
		edje_object_size_min_get(w->theme_object, &width, &height);
		i_l = (int)(width);
		i_t = (int)(height);

		if (i_l > 0 && MINIMUM_W(w) == EWL_OBJECT_MIN_SIZE)
			ewl_object_set_minimum_w(EWL_OBJECT(w), i_l);

		if (i_t > 0 && MINIMUM_H(w) == EWL_OBJECT_MIN_SIZE)
			ewl_object_set_minimum_h(EWL_OBJECT(w), i_t);

		/*
		 * Propagate maximum sizes from the bit theme to the widget.
		 */
		edje_object_size_max_get(w->theme_object, &width, &height);
		i_l = (int)(width);
		i_t = (int)(height);

		if (i_l > 0 && MAXIMUM_W(w) == EWL_OBJECT_MAX_SIZE)
			ewl_object_set_maximum_w(EWL_OBJECT(w), i_l);

		if (i_t > 0 && MAXIMUM_H(w) == EWL_OBJECT_MAX_SIZE)
			ewl_object_set_maximum_h(EWL_OBJECT(w), i_t);
	}

	ewl_object_add_visible(EWL_OBJECT(w), EWL_FLAG_VISIBLE_REALIZED);
	ewl_widget_configure(w);

	DRETURN(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for unrealizing a widget
 */
void ewl_widget_unrealize_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * First find it's parent embed so we can destroy the evas objects.
	 */
	emb = ewl_embed_find_by_widget(w);

	/*
	 * Destroy the clip box used for fx.
	 */
	if (w->fx_clip_box) {
		ewl_evas_object_destroy(w->fx_clip_box);
		w->fx_clip_box = NULL;
	}

	/*
	 * Destroy old image (if any) 
	 */
	if (w->theme_object) {
		ewl_evas_object_destroy(w->theme_object);
		w->theme_object = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for configuring a widget
 */
void ewl_widget_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_find_by_widget(w);

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
	 * Move the base theme object to the correct size and position
	 */
	if (w->theme_object) {
		evas_object_move(w->theme_object,
				CURRENT_X(w) - INSET_LEFT(w),
				CURRENT_Y(w) - INSET_TOP(w));
		evas_object_resize(w->theme_object,
				CURRENT_W(w) + INSET_LEFT(w) + INSET_RIGHT(w),
				CURRENT_H(w) + INSET_TOP(w) + INSET_BOTTOM(w));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for reparenting a widget
 */
void ewl_widget_reparent_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             layer;
	Ewl_Container  *pc;
	Ewl_Embed      *emb;
	Evas           *oevas = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	pc = EWL_CONTAINER(w->parent);

	ewl_widget_rebuild_appearance(w);

	/*
	 * If the new parent is on a different evas, we must re-realize it.
	 */
	if (REALIZED(w)) {
		if (w->fx_clip_box)
			oevas = evas_object_evas_get(w->fx_clip_box);
		else if (w->theme_object)
			oevas = evas_object_evas_get(w->theme_object);

		emb = ewl_embed_find_by_widget(w);
		if (!emb || oevas != emb->evas)
			ewl_widget_unrealize(w);
		else {

			/*
			 * Set up the clip box again if necessary
			 */
			if (pc && pc->clip_box && w->fx_clip_box)
				evas_object_clip_set(w->fx_clip_box,
						pc->clip_box);

			layer = ewl_widget_get_layer_sum(EWL_WIDGET(pc)) +
				LAYER(w);
			if (w->fx_clip_box)
				evas_object_layer_set(w->fx_clip_box, layer);
			if (w->theme_object)
				evas_object_layer_set(w->theme_object, layer);
		}
	}

	if (REALIZED(pc) && VISIBLE(w) && !REALIZED(w))
		ewl_realize_request(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void ewl_widget_enable_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_set_state(w, "normal");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_widget_disable_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_set_state(w, "disabled");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_focus_in_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	if (ewl_object_has_state(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	if (ewl_object_has_state(EWL_OBJECT(w), EWL_FLAG_STATE_PRESSED))
		ewl_widget_set_state(w, "clicked");
	else
		ewl_widget_set_state(w, "hilited");
}

void
ewl_widget_focus_out_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	if (ewl_object_has_state(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	ewl_widget_set_state(w, "normal");
}

void
ewl_widget_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	if (ewl_object_has_state(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	ewl_widget_set_state(w, "clicked");
}

void
ewl_widget_mouse_up_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	if (ewl_object_has_state(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	if (ewl_object_has_state(EWL_OBJECT(w), EWL_FLAG_STATE_HILITED)) {
		ewl_widget_set_state(w, "hilited");
		ewl_callback_call_with_event_data(w, EWL_CALLBACK_CLICKED,
						  ev_data);
	} else
		ewl_widget_set_state(w, "normal");
}

void
ewl_widget_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	if (w->theme_object) {
		edje_object_signal_emit(w->theme_object, "mouse,move", "EWL");
	}
}

static void
ewl_widget_get_theme_padding(Ewl_Widget *w, int *l, int *r, int *t, int *b)
{
	const char *key;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Read in the padding values from the edje file
	 */
	key = edje_object_data_get(w->theme_object, "pad/left");
	if (key && l) {
		*l = atoi(key);
	}

	key = edje_object_data_get(w->theme_object, "pad/right");
	if (key && r) {
		*r = atoi(key);
	}

	key = edje_object_data_get(w->theme_object, "pad/top");
	if (key && t) {
		*t = atoi(key);
	}

	key = edje_object_data_get(w->theme_object, "pad/bottom");
	if (key && b) {
		*b = atoi(key);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_widget_get_theme_insets(Ewl_Widget *w, int *l, int *r, int *t, int *b)
{
	const char *key;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Read in the inset values from the edje file
	 */
	key = edje_object_data_get(w->theme_object, "inset/left");
	if (key && l) {
		*l = atoi(key);
	}

	key = edje_object_data_get(w->theme_object, "inset/right");
	if (key && r) {
		*r = atoi(key);
	}

	key = edje_object_data_get(w->theme_object, "inset/top");
	if (key && t) {
		*t = atoi(key);
	}

	key = edje_object_data_get(w->theme_object, "inset/bottom");
	if (key && b) {
		*b = atoi(key);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_child_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->parent)
		ewl_container_remove_child(EWL_CONTAINER(w->parent), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
