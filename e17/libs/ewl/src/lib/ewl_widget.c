#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static Ecore_Hash *ewl_widget_name_table = NULL;

static void ewl_widget_theme_padding_get(Ewl_Widget *w, int *l, int *r,
					 int *t, int *b);
static void ewl_widget_theme_insets_get(Ewl_Widget *w, int *l, int *r,
					int *t, int *b);

/**
 * @brief Allocate a new widget.
 * @return Returns a newly allocated widget on success, NULL on failure.
 *
 * Do not use this function unless you know what you are doing! It is only
 * intended to easily create custom widgets that are not containers.
 */
Ewl_Widget *ewl_widget_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Widget, 1);
	if (w) {
		if (!ewl_widget_init(w)) {
			FREE(w);
			w = NULL;
		}
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param w: the widget to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a widget to default values and callbacks
 *
 * The widget w is initialized to default values and is
 * assigned the default callbacks.
 */
int ewl_widget_init(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	/*
	 * Set size fields on the object 
	 */
	if (!ewl_object_init(EWL_OBJECT(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	/*
	 * Set up the necessary theme structures 
	 */
	if (!ewl_theme_widget_init(w))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_object_state_remove(EWL_OBJECT(w), EWL_FLAGS_STATE_MASK);
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

	ewl_widget_inherit(w, "widget");

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to name
 * @param name: the new name for the widget
 * @return Returns no value.
 * @brief Name the specified widget.
 */
void ewl_widget_name_set(Ewl_Widget * w, const char *name)
{
	char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!ewl_widget_name_table)
		ewl_widget_name_table = ecore_hash_new(ecore_str_hash, 
							ecore_str_compare);

	t = strdup(name);
	ewl_attach_name_set(w, t);

	if (ewl_widget_name_table)
		ecore_hash_set(ewl_widget_name_table, t, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve the name
 * @return Returns an pointer to an allocated name string on success.
 * @brief Get the name for the specified widget.
 */
const char *ewl_widget_name_get(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	DRETURN_PTR(ewl_attach_name_get(w), DLEVEL_STABLE);
}

/**
 * @param name: the name of the widget to retrieve
 * @return Returns an pointer a matched widget on success.
 * @brief Find a widget identified by a name.
 */
Ewl_Widget *ewl_widget_name_find(const char * name)
{
	Ewl_Widget *match = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	if (ewl_widget_name_table)
		match = EWL_WIDGET(ecore_hash_get(ewl_widget_name_table, 
							(void *)name));

	DRETURN_PTR(match, DLEVEL_STABLE);
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

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED))
		ewl_realize_cancel_request(w);

	/*
	 * The parent's realize function will get us here again.
	 */
	if (w->parent && !REALIZED(w->parent))
		ewl_widget_realize(w->parent);
	else if (w->parent || ewl_object_toplevel_get(EWL_OBJECT(w))) {
		ewl_callback_call(w, EWL_CALLBACK_REALIZE);
		ewl_object_visible_add(EWL_OBJECT(w),
				       EWL_FLAG_VISIBLE_REALIZED);
	}

	ewl_widget_show(w);

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

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED))
		ewl_realize_cancel_request(w);

	if (!REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	ewl_callback_call(w, EWL_CALLBACK_UNREALIZE);
	ewl_object_visible_remove(EWL_OBJECT(w), EWL_FLAG_VISIBLE_REALIZED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to mark as revealed
 * @return Returns no value.
 * @brief Indicate a widget is revealed.
 */
void ewl_widget_reveal(Ewl_Widget *w)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR("w", w);

        ewl_object_visible_remove(EWL_OBJECT(w), EWL_FLAG_VISIBLE_OBSCURED);

        emb = ewl_embed_widget_find(w);
        if (emb && emb->evas) {
                ewl_callback_call(w, EWL_CALLBACK_REVEAL);
        }

        ewl_widget_configure(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to mark as obscured
 * @return Returns no value.
 * @brief Indicate a widget is obscured.
 */
void ewl_widget_obscure(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR("w", w);

        ewl_object_visible_add(EWL_OBJECT(w), EWL_FLAG_VISIBLE_OBSCURED);

        if (REALIZED(w) || ewl_object_queued_has(EWL_OBJECT(w),
                                EWL_FLAG_QUEUED_RSCHEDULED))
                ewl_callback_call(w, EWL_CALLBACK_OBSCURE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be marked as visible
 * @return Returns no value.
 * @brief mark a widget as visible
 *
 * Marks the widget as visible so that it will be displayed the next time
 * through the rendering loop. Note that the show callback may be delayed
 * until the widget has been realized.
 */
void ewl_widget_show(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Nothing to do if already visible.
	 */
	if (VISIBLE(w))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Flag that this is a visible widget
	 */
	ewl_object_visible_add(EWL_OBJECT(w), EWL_FLAG_VISIBLE_SHOWN);

	/*
	 * If realized, go about our business, otherwise queue for realize.
	 */
	if (REALIZED(w))
		ewl_callback_call(w, EWL_CALLBACK_SHOW);
	else
		ewl_realize_request(w);

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
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Already hidden? Then no work to be done.
	 */
	if (HIDDEN(w))
		DRETURN(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);
	ewl_embed_info_widgets_cleanup(emb, w);

	ewl_object_visible_remove(EWL_OBJECT(w), EWL_FLAG_VISIBLE_SHOWN);

	if (REALIZED(w))
		ewl_callback_call(w, EWL_CALLBACK_HIDE);

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

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED))
		DRETURN(DLEVEL_STABLE);

	ewl_widget_hide(w);
	if (w->parent)
		ewl_container_child_remove(EWL_CONTAINER(w->parent), w);
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

	if (!VISIBLE(w))
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

	ewl_callback_call_with_event_data(w, EWL_CALLBACK_REPARENT, w->parent);

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
void ewl_widget_data_set(Ewl_Widget * w, void *k, void *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (!w->data)
		w->data = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	ecore_hash_set(w->data, k, v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param w: the widget that owns the key value pair
 * @param k: the key that is associated with the data
 * @return Returns the deleted value.
 * @brief Remove the specified key / value pair from the widget and
 * return the value.
 *
 * Removes a key / value pair with k as the key from the specified
 * widget w and return the value. @c NULL is returned if there is no
 * stored data or if an error occurs.
 */
void *ewl_widget_data_del(Ewl_Widget * w, void *k)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	if (!w->data)
	        DRETURN_PTR(NULL, DLEVEL_STABLE);

	DRETURN_PTR(ecore_hash_remove(w->data, k), DLEVEL_STABLE);
}


/**
 * @param w: the widget that owns the key value pair
 * @param k: the key that is associated with the data
 * @return Returns the value associated with k on success, NULL on failure.
 * @brief retrieve the specified key / value pair from the widget
 *
 * Retrieves a key / value pair with k as the key from the specified widget w.
 */
void           *ewl_widget_data_get(Ewl_Widget * w, void *k)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	if (!w->data)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	DRETURN_PTR(ecore_hash_get(w->data, k), DLEVEL_STABLE);
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
void ewl_widget_appearance_set(Ewl_Widget * w, char *appearance)
{
	int al;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("appearance", appearance);

	/* make sure we have something to do */
	if (w->appearance && !strcmp(appearance, w->appearance))
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	IF_FREE(w->appearance);
	al = strlen(appearance) + 1;

	/*
	 * The base appearance is used for determining the theme key of the
	 * widget.
	 */
	w->appearance = (char *)malloc(al);
	if (!w->appearance)
		DRETURN(DLEVEL_STABLE);

	snprintf(w->appearance, al, "%s",  appearance);

	/*
	 * Recreate the visible components of the widget if necessary.
	 */
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
char *ewl_widget_appearance_get(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	DRETURN_PTR((w->appearance ? strdup(w->appearance) : NULL), DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve the full appearance key
 * @return Returns a pointer to the full appearance path string on success, NULL on
 * failure.
 * @brief Retrieve the appearance path key of the widget
 */
char *ewl_widget_appearance_path_get(Ewl_Widget * w)
{
	char *ret = NULL, *tmp;
	int len;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	if (w->parent)
		tmp = ewl_widget_appearance_path_get(w->parent);
	else
		tmp = strdup("");

	len = strlen(tmp) + 2; /* one for the / one for the \0 */
	len += (w->appearance ? strlen(w->appearance) : 0);

	ret = malloc(sizeof(char) * len);
	snprintf(ret, len, "%s/%s", tmp, 
			(w->appearance ? w->appearance : ""));
	FREE(tmp);

	DRETURN_PTR(ret, DLEVEL_STABLE);
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
void ewl_widget_state_set(Ewl_Widget * w, char *state)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("state", state);

	if (w->bit_state && !strcmp(w->bit_state, state))
		DRETURN(DLEVEL_STABLE);

	if (w->bit_state)
		ecore_string_release(w->bit_state);
	w->bit_state = ecore_string_instance(state);

	if (w->theme_object)
		edje_object_signal_emit(w->theme_object, state, "EWL");

	ewl_callback_call(w, EWL_CALLBACK_STATE_CHANGED);

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
void ewl_widget_parent_set(Ewl_Widget * w, Ewl_Widget * p)
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

	/*
	 * Set this to the new parent here to avoid infinite recursion when
	 * called from ewl_container_child_remove.
	 */
	w->parent = p;

	/*
	 * A widget cannot be the child of multiple widgets, so remove it
	 * from a previous parent before adding to this parent.
	 */
	if (op) {
		ewl_container_child_remove(op, w);
		if (!p)
			ewl_callback_del(w, EWL_CALLBACK_DESTROY,
				 ewl_widget_child_destroy_cb);
	}
	/*
	 * A widget that has not had a previous parent needs the parent
	 * destruction callback added.
	 */
	else if (p) {
		ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
				     ewl_widget_child_destroy_cb, NULL);
	}

	ewl_callback_call_with_event_data(w, EWL_CALLBACK_REPARENT, p);

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

	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED)) {
		ewl_object_state_remove(EWL_OBJECT(w), EWL_FLAGS_STATE_MASK);
		ewl_object_state_add(EWL_OBJECT(w), EWL_FLAG_STATE_NORMAL);
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

	if (!ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED)) {
		ewl_object_state_remove(EWL_OBJECT(w), EWL_FLAGS_STATE_MASK);
		ewl_object_state_add(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED);
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
void ewl_widget_layer_set(Ewl_Widget *w, int layer)
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
int ewl_widget_layer_get(Ewl_Widget *w)
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
int ewl_widget_layer_sum_get(Ewl_Widget *w)
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
 * This moves the widget @a w to the end of the tab order list in the embed
 * that holds it. 
 */
void ewl_widget_tab_order_append(Ewl_Widget *w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_widget_find(w);
	ewl_embed_tab_order_append(emb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be moved to the front of the focus list
 * @return Returns no value.
 * @brief Changes the order in the embed so @a w receives focus first on tab.
 *
 * This moves the widget @a w to the front of the tab order list in the embed
 * that holds it. 
 */
void ewl_widget_tab_order_prepend(Ewl_Widget *w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_widget_find(w);
	ewl_embed_tab_order_prepend(emb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be moved to the front of the focus list
 * @return Returns no value.
 * @brief Changes the order in the embed so @a w receives focus first on tab.
 *
 * This moves the widget @a w to the given index in the tab order list in the embed
 * that holds it. 
 */
void ewl_widget_tab_order_insert(Ewl_Widget *w, unsigned int idx)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_widget_find(w);
	ewl_embed_tab_order_insert(emb, w, idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to be inserted into the tab order
 * @param before: The widget we are to be inserted before
 * @return Returns no value.
 * @brief Inserts the widget into the tab order before the @a before widget
 */
void ewl_widget_tab_order_insert_before(Ewl_Widget *w, Ewl_Widget *before)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("before", before);

	emb = ewl_embed_widget_find(w);
	ewl_embed_tab_order_insert_before(emb, w, before);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to insert into the tab order
 * @param after: The widget to insert after
 * @return Returns no value.
 * @brief Insert the given widget into the tab order after the @a after
 * widget
 */
void ewl_widget_tab_order_insert_after(Ewl_Widget *w, Ewl_Widget *after)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("after", after);

	emb = ewl_embed_widget_find(w);
	ewl_embed_tab_order_insert_after(emb, w, after);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to display ancestry tree
 * @return Returns no value.
 * @brief Prints to stdout the tree of widgets that are parents of a widget.
 */
void ewl_widget_tree_print(Ewl_Widget *w)
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
			ewl_object_current_x_get(EWL_OBJECT(w)),
			ewl_object_current_y_get(EWL_OBJECT(w)),
			ewl_object_current_w_get(EWL_OBJECT(w)),
			ewl_object_current_h_get(EWL_OBJECT(w)),
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
void ewl_widget_internal_set(Ewl_Widget *w, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (val)
		ewl_object_flags_add(EWL_OBJECT(w), EWL_FLAG_PROPERTY_INTERNAL,
				EWL_FLAGS_PROPERTY_MASK);
	else
		ewl_object_flags_remove(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_INTERNAL,
				EWL_FLAGS_PROPERTY_MASK);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param widget: the widget to set the inheritance on
 * @param inherit: the string to append to the inheritance
 * @return Returns no value.
 * @brief Appends the given inheritance to this widgets inheritance string.
 */
void ewl_widget_inherit(Ewl_Widget *widget, char *inherit)
{
	int len;
	char *tmp = NULL;
	char *tmp2 = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);
	DCHECK_PARAM_PTR("inherit", inherit);

	len = strlen(inherit) +  3;
	tmp2 = widget->inheritance;
	if (tmp2)
		len += strlen(tmp2);
	else
		tmp2 = "";

	tmp = malloc(sizeof(char) * len);
	sprintf(tmp, "%s:%s:", tmp2, inherit);

	if (widget->inheritance)
		ecore_string_release(widget->inheritance);
	widget->inheritance = ecore_string_instance(tmp);

	FREE(tmp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param widget: the widget to determine if a type is inherited
 * @param type: the type to check for inheritance in the widget
 * @return Returns TRUE if @a w inherited the type @a t, otherwise FALSE.
 * @brief Determine if the widget @a w has inherited from the type @a t.
 */
unsigned int ewl_widget_type_is(Ewl_Widget *widget, char *type)
{
	int found = FALSE;
	char tmp[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, FALSE);
	DCHECK_PARAM_PTR_RET("type", type, FALSE);

	snprintf(tmp, PATH_MAX, ":%s:", type);
	if (widget->inheritance && strstr(widget->inheritance, tmp))
		found = TRUE;

	DRETURN_INT(found, DLEVEL_STABLE);
}

/**
 * @param w: the widget to query the state of the internal flag
 * @return Returns TRUE if the widget is marked internal, otherwise FALSE.
 * @brief Checks the widget for the internal flag.
 */
unsigned int ewl_widget_internal_is(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_object_flags_has(EWL_OBJECT(w), EWL_FLAG_PROPERTY_INTERNAL,
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
void ewl_widget_clipped_set(Ewl_Widget *w, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (val)
		ewl_object_flags_remove(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP,
				EWL_FLAGS_VISIBLE_MASK);
	else
		ewl_object_flags_add(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP,
				EWL_FLAGS_VISIBLE_MASK);

	if (!REALIZED(w) || (val && w->fx_clip_box) ||
			(!val && !w->fx_clip_box))
		DRETURN(DLEVEL_STABLE);

	if (val) {
		Ewl_Embed *emb;

		emb = ewl_embed_widget_find(w);
		if (!emb || !emb->evas)
			DRETURN(DLEVEL_STABLE);

		w->fx_clip_box = evas_object_rectangle_add(emb->evas);
		evas_object_pass_events_set(w->fx_clip_box, TRUE);
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
unsigned int ewl_widget_clipped_is(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_object_flags_has(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP,
			EWL_FLAGS_VISIBLE_MASK))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to receive keyboard focus
 * @return Returns no value.
 * @brief Changes the keyboard focus to the widget @a w.
 */
void ewl_widget_focus_send(Ewl_Widget *w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);
	ewl_embed_focused_widget_set(emb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the currnetly focused widget.
 * @brief Retrieve the currently focused widget.
 */
Ewl_Widget *ewl_widget_focused_get(void)
{
	Ewl_Embed *emb;
	Ewl_Widget *w = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);
	w = ewl_embed_focused_widget_get(emb);

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param w: The widget to set the color of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief sets the colour of the widget
 */
void
ewl_widget_color_set(Ewl_Widget *w, int r, int g, int b, int a)
{
	Ewl_Color_Set *color;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	color = NEW(Ewl_Color_Set, 1);
	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;
	ewl_attach_color_set(w, color);

	if (REALIZED(w))
		evas_object_color_set(w->fx_clip_box, r, g, b, a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to get the colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @return Returns no value
 * @brief Gets the colour settings of the widget
 */
void
ewl_widget_color_get(Ewl_Widget *w, int *r, int *g, int *b, int *a)
{
	Ewl_Color_Set *color;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	color = ewl_attach_color_get(w);
	if (!color) 
	{
		DRETURN(DLEVEL_STABLE);
	}

	if (r) *r = color->r;
	if (g) *g = color->g;
	if (b) *b = color->b;
	if (a) *a = color->a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the series of operations common to every widget when
 * they are destroyed. This should ALWAYS be the the last callback
 * in the chain.
 */
void ewl_widget_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *data __UNUSED__)
{
	int               i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * First remove the parents reference to this widget to avoid bad
	 * references.
	 */
	if (w->parent)
		ewl_container_child_remove(EWL_CONTAINER(w->parent), w);

	ewl_widget_unrealize(w);

	/*
	 * Free up appearance related information
	 */
	ewl_theme_widget_shutdown(w);
	IF_FREE(w->appearance);

	if (w->inheritance)
		ecore_string_release(w->inheritance);

	if (w->bit_state)
		ecore_string_release(w->bit_state);

	/* 
	 * cleanup the attachment lists 
	 */
	if (w->attach)
	{
		ewl_attach_list_del(w->attach, EWL_ATTACH_TYPE_TOOLTIP);
		ewl_attach_list_del(w->attach, EWL_ATTACH_TYPE_COLOR);
		ewl_attach_list_del(w->attach, EWL_ATTACH_TYPE_NAME);
	}

	/*
	 * Clear out the callbacks, this is a bit tricky because we don't want
	 * to continue using this widget after the callbacks have been
	 * deleted. Clear all callbacks except for the destroy callbacks.
	 * This preserves the list of the destroy type so we don't get a segfault.
	 */
	for (i = 0; i < EWL_CALLBACK_MAX; i++)
	{
		if (i == EWL_CALLBACK_DESTROY) continue;
		ewl_callback_del_type(w, i);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Every widget must show it's fx_clip_box to be seen
 */
void ewl_widget_show_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Container *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->fx_clip_box)
		evas_object_show(w->fx_clip_box);
	if (w->theme_object)
		evas_object_show(w->theme_object);

	pc = EWL_CONTAINER(w->parent);
	if (pc)
		ewl_container_child_show_call(pc, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Every widget must hide it's fx_clip_box in order to hide
 */
void ewl_widget_hide_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Container *pc;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Hide the visible evas objects.
	 */
	if (w->fx_clip_box)
		evas_object_hide(w->fx_clip_box);
	if (w->theme_object)
		evas_object_hide(w->theme_object);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED))
		ewl_realize_cancel_request(w);

	/*
	 * Notify parent of hidden state.
	 */
	pc = EWL_CONTAINER(w->parent);
	if (pc)
		ewl_container_child_hide_call(pc, w);

	emb = ewl_embed_widget_find(w);
	ewl_embed_info_widgets_cleanup(emb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for realizing a widget
 */
void ewl_widget_realize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	int             l = 0, r = 0, t = 0, b = 0;
	int             i_l = 0, i_r = 0, i_t = 0, i_b = 0;
	int             p_l = 0, p_r = 0, p_t = 0, p_b = 0;
	char           *i = NULL;
	char           *group = NULL;
	Evas_Coord      width, height;
	Ewl_Embed      *emb = NULL;
	Ewl_Container  *pc = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_widget_find(w);

	/*
	 * Create the fx clip box where special fx can be drawn to affect the
	 * entire contents of the widget
	 */
	if (!ewl_object_flags_get(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP)) {
		w->fx_clip_box = evas_object_rectangle_add(emb->evas);
		evas_object_pass_events_set(w->fx_clip_box, TRUE);
	}

	if (w->fx_clip_box) {
		int sum;
		int r, g, b, a;

		sum = ewl_widget_layer_sum_get(w);
		if (sum > ewl_embed_max_layer_get(emb))
			ewl_embed_max_layer_set(emb, sum);
		evas_object_layer_set(w->fx_clip_box, sum);

		r = g = b = a = 255;
		ewl_widget_color_get(w, &r, &g, &b, &a);
		evas_object_color_set(w->fx_clip_box, r, g, b, a);
	}

	pc = EWL_CONTAINER(w->parent);

	/*
	 * Clip the fx_clip_box to the parent clip_box.
	 */
	if (pc && pc->clip_box && w->fx_clip_box)
		evas_object_clip_set(w->fx_clip_box, pc->clip_box);

	/*
	 * Retrieve the path to the theme file that will be loaded
	 * return if no file to be loaded. Also get the group name in the
	 * theme file.
	 */
	i = ewl_theme_image_get(w, "file");
	group = ewl_theme_data_str_get(w, "group");

	if (group) {
		emb = ewl_embed_widget_find(w);
		if (!emb)
			DRETURN(DLEVEL_STABLE);

		/*
		 * Load the theme object
		 */
		w->theme_object = edje_object_add(emb->evas);
		evas_object_repeat_events_set(w->theme_object, 1);
		edje_object_file_set(w->theme_object, i, group);
		if (edje_object_load_error_get(w->theme_object)) {
			evas_object_del(w->theme_object);
			w->theme_object = NULL;
		}
		FREE(group);
	}

	IF_FREE(i);

	/*
	 * Set up the theme object on the widgets evas
	 */
	if (w->theme_object) {
		if (w->bit_state)
  			ewl_widget_state_set(w, w->bit_state);

		ewl_widget_theme_insets_get(w, &i_l, &i_r, &i_t, &i_b);
		ewl_widget_theme_padding_get(w, &p_l, &p_r, &p_t, &p_b);

		ewl_object_insets_get(EWL_OBJECT(w), &l, &r, &t, &b);

		/*
		 * Use previously set insets and padding if available.
		 */
		if (l)
			i_l = l;
		if (r)
			i_r = r;
		if (t)
			i_t = t;
		if (b)
			i_b = b;

		ewl_object_padding_get(EWL_OBJECT(w), &l, &r, &t, &b);

		if (l)
			p_l = l;
		if (r)
			p_r = r;
		if (t)
			p_t = t;
		if (b)
			p_b = b;

		/*
		 * Determine the evas layer for the objects and set clipping.
		 */
		evas_object_layer_set(w->theme_object,
				ewl_widget_layer_sum_get(w));
		if (w->fx_clip_box)
			evas_object_clip_set(w->theme_object, w->fx_clip_box);
		evas_object_show(w->theme_object);

		/*
		 * Assign the relevant insets and padding.
		 */
		ewl_object_insets_set(EWL_OBJECT(w), i_l, i_r, i_t, i_b);
		ewl_object_padding_set(EWL_OBJECT(w), p_l, p_r, p_t, p_b);

		i_l = CURRENT_X(w);
		i_t = CURRENT_Y(w);
		ewl_object_x_request(EWL_OBJECT(w), i_l);
		ewl_object_y_request(EWL_OBJECT(w), i_t);

		if (ewl_object_state_has(EWL_OBJECT(w),
					EWL_FLAG_STATE_DISABLED))
			ewl_widget_state_set(w, "disabled");

		/*
		 * Propagate minimum sizes from the bit theme to the widget.
		 */
		edje_object_size_min_get(w->theme_object, &width, &height);
		i_l = (int)(width);
		i_t = (int)(height);

		if (i_l > 0 && MINIMUM_W(w) == EWL_OBJECT_MIN_SIZE
				&& i_l > EWL_OBJECT_MIN_SIZE
				&& i_l <= EWL_OBJECT_MAX_SIZE)
			ewl_object_minimum_w_set(EWL_OBJECT(w), i_l);

		if (i_t > 0 && MINIMUM_H(w) == EWL_OBJECT_MIN_SIZE
				&& i_t > EWL_OBJECT_MIN_SIZE
				&& i_t <= EWL_OBJECT_MAX_SIZE)
			ewl_object_minimum_h_set(EWL_OBJECT(w), i_t);

		/*
		 * Propagate maximum sizes from the bit theme to the widget.
		 */
		edje_object_size_max_get(w->theme_object, &width, &height);
		i_l = (int)(width);
		i_t = (int)(height);

		if (i_l > 0 && MAXIMUM_W(w) == EWL_OBJECT_MAX_SIZE
				&& i_l >= EWL_OBJECT_MIN_SIZE
				&& i_l < EWL_OBJECT_MAX_SIZE)
			ewl_object_maximum_w_set(EWL_OBJECT(w), i_l);

		if (i_t > 0 && MAXIMUM_H(w) == EWL_OBJECT_MAX_SIZE
				&& i_t >= EWL_OBJECT_MIN_SIZE
				&& i_t < EWL_OBJECT_MAX_SIZE)
			ewl_object_maximum_h_set(EWL_OBJECT(w), i_t);
	}

	ewl_object_visible_add(EWL_OBJECT(w), EWL_FLAG_VISIBLE_REALIZED);
	ewl_widget_configure(w);

	DRETURN(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for unrealizing a widget
 */
void ewl_widget_unrealize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * First find it's parent embed so we can destroy the evas objects.
	 */
	emb = ewl_embed_widget_find(w);

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
void ewl_widget_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_widget_find(w);

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
void ewl_widget_reparent_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Container  *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	pc = EWL_CONTAINER(w->parent);
	if (REALIZED(w))
		ewl_widget_unrealize(w);

	if (pc && REALIZED(pc) && VISIBLE(w) && !REALIZED(w))
		ewl_realize_request(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void ewl_widget_enable_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_state_set(w, "default");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_widget_disable_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_state_set(w, "disabled");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_focus_in_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_PRESSED))
		ewl_widget_state_set(w, "mouse,down,0");
	else
		ewl_widget_state_set(w, "mouse,in");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_focus_out_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	ewl_widget_state_set(w, "mouse,out");
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_mouse_down_cb(Ewl_Widget *w, void *ev_data,
				void *user_data __UNUSED__)
{
	Ewl_Event_Mouse_Down *e = ev_data;
	char state[14];

	DENTER_FUNCTION(DLEVEL_STABLE);
	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	snprintf(state, 14, "mouse,down,%i", e->button);
	ewl_widget_state_set(w, state);
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_mouse_up_cb(Ewl_Widget *w, void *ev_data,
				void *user_data __UNUSED__)
{
	Ewl_Event_Mouse_Up *e = ev_data;
	char state[14];

	DENTER_FUNCTION(DLEVEL_STABLE);
	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	snprintf(state, 14, "mouse,up,%i", e->button);
	ewl_widget_state_set(w, state);

	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_HILITED)) {
		int x, y;

		ewl_widget_state_set(w, "mouse,in");
		x = e->x - (CURRENT_X(w) - INSET_LEFT(w));
		y = e->y - (CURRENT_Y(w) - INSET_TOP(w));
		if ((x > 0) && (x < CURRENT_W(w) + INSET_HORIZONTAL(w)) &&
		    (y > 0) && (y < CURRENT_H(w) + INSET_VERTICAL(w))) {
			ewl_callback_call_with_event_data(w,
					EWL_CALLBACK_CLICKED, ev_data);
		}
		else {
			ewl_embed_mouse_move_feed(ewl_embed_widget_find(w),
					e->x, e->y, e->modifiers);
		}
	} else
		ewl_widget_state_set(w, "mouse,out");
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_mouse_move_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

        ewl_widget_state_set(w, "mouse,move");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_widget_theme_padding_get(Ewl_Widget *w, int *l, int *r, int *t, int *b)
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
ewl_widget_theme_insets_get(Ewl_Widget *w, int *l, int *r, int *t, int *b)
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
ewl_widget_child_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->parent)
		ewl_container_child_remove(EWL_CONTAINER(w->parent), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
