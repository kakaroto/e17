#include "ewl_private.h"
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

static Ecore_Hash *ewl_widget_name_table = NULL;
static int ewl_widget_dnd_drag_move_count = 0;
static Ewl_Widget *ewl_widget_drag_widget= NULL;

static void ewl_widget_theme_padding_get(Ewl_Widget *w, int *l, int *r,
						int *t, int *b);
static void ewl_widget_theme_insets_get(Ewl_Widget *w, int *l, int *r,
						int *t, int *b);
static void ewl_widget_appearance_part_text_apply(Ewl_Widget * w,
						  const char *part, char *text);

static void ewl_widget_drag_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);
static void ewl_widget_drag_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
static void ewl_widget_drag_down_cb (Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @return Returns a newly allocated widget on success, NULL on failure.
 * @brief Allocate a new widget.
 *
 * Do not use this function unless you know what you are doing! It is only
 * intended to easily create custom widgets that are not containers.
 */
Ewl_Widget *
ewl_widget_new(void)
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
int
ewl_widget_init(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	ewl_widget_inherit(w, EWL_WIDGET_TYPE);

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
	ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_widget_reveal_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_OBSCURE, ewl_widget_obscure_cb,
				NULL);
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
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_IN, ewl_widget_mouse_in_cb,
				NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_OUT, ewl_widget_mouse_out_cb,
				NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
				ewl_widget_mouse_down_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, ewl_widget_mouse_up_cb,
				NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_widget_mouse_move_cb, NULL);

	/* widgets can take focus by default */
	ewl_widget_focusable_set(w, TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to name
 * @param name: the new name for the widget
 * @return Returns no value.
 * @brief Name the specified widget.
 */
void
ewl_widget_name_set(Ewl_Widget * w, const char *name)
{
	char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
const char *
ewl_widget_name_get(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	DRETURN_PTR(ewl_attach_name_get(w), DLEVEL_STABLE);
}

/**
 * @param name: the name of the widget to retrieve
 * @return Returns an pointer a matched widget on success.
 * @brief Find a widget identified by a name.
 */
Ewl_Widget *
ewl_widget_name_find(const char * name)
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
void
ewl_widget_realize(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED)
			&& !ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RPROCESS))
		ewl_realize_cancel_request(w);

	/*
	 * The parent's realize function will get us here again.
	 */
	if (w->parent && !REALIZED(w->parent))
		ewl_widget_realize(w->parent);
	else if (w->parent || ewl_object_toplevel_get(EWL_OBJECT(w))) {
		ewl_object_queued_add(EWL_OBJECT(w), EWL_FLAG_QUEUED_RPROCESS);
		ewl_callback_call(w, EWL_CALLBACK_REALIZE);
		ewl_object_queued_remove(EWL_OBJECT(w),
					 EWL_FLAG_QUEUED_RPROCESS);
		ewl_object_visible_add(EWL_OBJECT(w),
					EWL_FLAG_VISIBLE_REALIZED);
		ewl_widget_obscure(w);
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
void
ewl_widget_unrealize(Ewl_Widget * w)
{
	Ewl_Container *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED))
		ewl_realize_cancel_request(w);

	if (!REALIZED(w))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Notify parent of hidden state.
	 */
	pc = EWL_CONTAINER(w->parent);
	if (pc)
		ewl_container_child_hide_call(pc, w);

	ewl_callback_call(w, EWL_CALLBACK_UNREALIZE);
	ewl_object_visible_remove(EWL_OBJECT(w), EWL_FLAG_VISIBLE_REALIZED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to mark as revealed
 * @return Returns no value.
 * @brief Indicate a widget is revealed.
 */
void
ewl_widget_reveal(Ewl_Widget *w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!OBSCURED(w))
		DRETURN(DLEVEL_STABLE);

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

	if (OBSCURED(w))
		DRETURN(DLEVEL_STABLE);

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
void
ewl_widget_show(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_hide(Ewl_Widget * w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_RSCHEDULED))
		ewl_realize_cancel_request(w);

	/*
	 * Already hidden? Then no work to be done.
	 */
	if (HIDDEN(w))
		DRETURN(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);
	if (emb)
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
void
ewl_widget_destroy(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if ((ewl_object_queued_has(EWL_OBJECT(w), EWL_FLAG_QUEUED_DSCHEDULED))
			|| (ewl_object_queued_has(EWL_OBJECT(w), 
					EWL_FLAG_QUEUED_DPROCESS)))
		DRETURN(DLEVEL_STABLE);

	if (w == ewl_widget_drag_candidate_get())
		ewl_widget_dnd_reset();

	/*
	 * Request prior to hiding so we can skip resizing for child widgets
	 * being hidden.
	 */
	ewl_destroy_request(w);

	if (w->parent)
		ewl_container_child_remove(EWL_CONTAINER(w->parent), w);

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
void
ewl_widget_configure(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if ((!VISIBLE(w)) || (w->parent && OBSCURED(w->parent)))
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
void
ewl_widget_theme_update(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_reparent(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_data_set(Ewl_Widget * w, void *k, void *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void *
ewl_widget_data_del(Ewl_Widget * w, void *k)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void *
ewl_widget_data_get(Ewl_Widget * w, void *k)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_appearance_set(Ewl_Widget * w, char *appearance)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("appearance", appearance);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* make sure we have something to do */
	if (w->appearance && !strcmp(appearance, w->appearance))
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	/*
	 * The base appearance is used for determining the theme key of the
	 * widget. Intentionally lose a reference to the ecore string to keep a
	 * reference cached for later re-use.
	 */
	w->appearance = ecore_string_instance(appearance);
	if (!w->appearance)
		DRETURN(DLEVEL_STABLE);

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
char *
ewl_widget_appearance_get(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	DRETURN_PTR((w->appearance ? strdup(w->appearance) : NULL), DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve the full appearance key
 * @param size: pointer to an int indicating the string length
 * @return Returns a pointer to the full appearance path string on success, NULL on
 * failure.
 * @brief Retrieve the appearance path key of the widget
 */
static char *
ewl_widget_appearance_path_size_get(Ewl_Widget *w, int *size)
{
	char *ret = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/*
	 * Allocate enough for the appearance plus a leading "/"
	 */
	*size += (w->appearance ? strlen(w->appearance) : 0) + 1;
	if (w->parent) {
		ret = ewl_widget_appearance_path_size_get(w->parent, size);
	}
	else {
		ret = NEW(char, *size + 1);
	}

	strcat(ret, "/");
	if (w->appearance)
		strcat(ret, w->appearance);

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve the full appearance key
 * @return Returns a pointer to the full appearance path string on success, NULL on
 * failure.
 * @brief Retrieve the appearance path key of the widget
 */
char *
ewl_widget_appearance_path_get(Ewl_Widget * w)
{
	char *ret = NULL;
	int len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ret = ewl_widget_appearance_path_size_get(w, &len);

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
void
ewl_widget_state_set(Ewl_Widget *w, char *state)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("state", state);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/*
	 * Intentionally lose a reference to the ecore string to keep a
	 * reference cached for later re-use.
	 */
	w->bit_state = ecore_string_instance(state);

	if (w->theme_object) {
		if (ewl_config.theme.print_signals)
			printf("Emitting: %s\n", state);
		edje_object_signal_emit(w->theme_object, state, "EWL");
	} 

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
void
ewl_widget_parent_set(Ewl_Widget * w, Ewl_Widget * p)
{
	Ewl_Widget *tmp;
	Ewl_Container *op;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	op = EWL_CONTAINER(w->parent);
	if (op == EWL_CONTAINER(p))
		DRETURN(DLEVEL_STABLE);
	

	if (!p)
		ewl_widget_obscure(w);

	emb = ewl_embed_widget_find(w);
	if (emb)
		ewl_embed_info_widgets_cleanup(emb, w);

	/*
	 * Verify this will not result in recursively nested widgets.
	 */
	tmp = p;
	while (tmp) {
		if (tmp == w) {
			DWARNING("ERROR: Recursivly nested widgets.\n");
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
		/*
		if (!p)
			ewl_callback_del(w, EWL_CALLBACK_DESTROY,
					ewl_widget_child_destroy_cb);
					*/
	}
	/*
	 * A widget that has not had a previous parent needs the parent
	 * destruction callback added.
	 */
	else if (p) {
		/*
		ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
					ewl_widget_child_destroy_cb, NULL);
					*/
	}

	ewl_callback_call_with_event_data(w, EWL_CALLBACK_REPARENT, p);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**     
 * @param w: the widget whose text to change
 * @param part: the theme part name whose text to change
 * @param text: the new text to change to
 * @return Returns no value.
 * @brief Change the text of the given theme part of a widget
 *
 * Changes the text of a given Edje-define TEXT part.
 */
static void
ewl_widget_appearance_part_text_apply(Ewl_Widget * w, const char *part, char *text)
{
	Evas_Coord nw, nh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("part", part);
	DCHECK_PARAM_PTR("text", text);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!w->theme_object)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	edje_object_part_text_set(w->theme_object, part, text);
	edje_object_size_min_calc(w->theme_object, &nw, &nh);

	ewl_object_preferred_inner_size_set(EWL_OBJECT(w), (int)nw, (int)nh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to change
 * @param part: the theme part name whose text to change
 * @param text: the new text to change to
 * @return Returns no value.
 * @brief Change the text of the given theme part of a widget
 *
 * Changes the text of a given Edje-define TEXT part.  This is for
 * widgets whose Edje appearance defines TEXT parts, and enables
 * each of those text parts to be changed independently.
 * The text value is recorded in a hash and reapplied if the theme
 * is reloaded for this widget.
 */
void
ewl_widget_appearance_part_text_set(Ewl_Widget * w, char *part, char *text)
{
	int i;
	Ewl_Pair *match = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("part", part);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/*
	 * Check for an existing instance of the part key.
	 */
	if (w->theme_text.list) {
		if (w->theme_text.direct) {
			match = EWL_PAIR(w->theme_text.list);
			if (strcmp(part, match->key))
				match = NULL;
		}
		else {
			for (i = 0; i < w->theme_text.len; i++) {
				Ewl_Pair *current = w->theme_text.list[i];
				if (!strcmp(current->key, part)) {
					match = current;
					break;
				}
			}
		}
	}

	/*
	 * Part key exists and the value is the same as the current value.
	 */
	if (match) {
	       	if (!strcmp(text, match->value))
			DRETURN(DLEVEL_STABLE);

		IF_FREE(match->value);
	}
	else {
		match = NEW(Ewl_Pair, 1);
		if (!match)
			DRETURN(DLEVEL_STABLE);
		match->key = ecore_string_instance(part);
		w->theme_text.len++;

		if (!w->theme_text.list) {
			w->theme_text.direct = 1;
			w->theme_text.list = (Ewl_Pair **)match;
		}
		else {
			if (w->theme_text.direct) {
				Ewl_Pair *old = EWL_PAIR(w->theme_text.list);
				w->theme_text.list = NEW(Ewl_Pair *, 2);
				w->theme_text.list[0] = old;
			}
			else {
				w->theme_text.list = realloc(w->theme_text.list,
						sizeof(Ewl_Pair) *
						w->theme_text.len);
			}
			w->theme_text.list[w->theme_text.len - 1] = match;
		}
	}

	/*
	 * What should be the default if you enter NULL? A blank string?
	 * Revert to the text specified in the Edje? Use blank for now.
	 */
	match->value = strdup( text ? text : "" );

	ewl_widget_appearance_part_text_apply(w, match->key, match->value);
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to retrieve
 * @param part: the theme part name whose text to retrieve
 * @return Returns NULL on failure, a copy of the current text on success.
 * @brief Retrieve a copy of a parts current text.
 *
 * Get the text of a given Edje-define TEXT part.  This is for
 * widgets whose Edje appearance defines TEXT parts, and enables
 * each of those text parts to be retrieved independently.
 */
char *
ewl_widget_appearance_part_text_get(Ewl_Widget * w, char *part)
{
	int i;
	Ewl_Pair *match = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("part", part, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	/*
	 * Check for an existing instance of the part key.
	 */
	if (w->theme_text.list) {
		if (w->theme_text.direct) {
			match = EWL_PAIR(w->theme_text.list);
			if (strcmp(part, match->key))
				match = NULL;
		}
		else {
			for (i = 0; i < w->theme_text.len; i++) {
				Ewl_Pair *current = w->theme_text.list[i];
				if (!strcmp(current->key, part)) {
					match = current;
					break;
				}
			}
		}
	}

	DRETURN_PTR((match ? strdup(match->value) : NULL), DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to change
 * @param text: the new text to change to
 * @return Returns no value.
 * @brief Change the text of the given theme part of a widget
 *
 * Changes the text of an Edje-define TEXT part.  This is for
 * widgets whose Edje appearance defines a TEXT part, and identifies
 * it with with a data item called "/WIDGET/textpart".
 * The text value is recorded in a hash and reapplied if the theme
 * is reloaded for this widget.
 */
void
ewl_widget_appearance_text_set(Ewl_Widget * w, char *text)
{
	char *part;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	part = ewl_theme_data_str_get(w, "textpart");
	if (part) {
		ewl_widget_appearance_part_text_set(w, part, text);
		FREE(part);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to retrieve
 * @return Returns the current text on success, NULL on failure.
 * @brief Retrieve the text of the given theme part of a widget
 */
char *
ewl_widget_appearance_text_get(Ewl_Widget * w)
{
	char *part;
	char *match = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	part = ewl_theme_data_str_get(w, "textpart");
	if (part) {
		match = ewl_widget_appearance_part_text_get(w, part);
		FREE(part);
	}

	DRETURN_PTR(match, DLEVEL_STABLE);
}

/**
 * @param w: the widget to re-enable
 * @return Returns no value.
 * @brief Re-enable a disabled widget
 *
 * Re-enables a previously disabled widget.
 */
void
ewl_widget_enable(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_disable(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_layer_set(Ewl_Widget *w, int layer)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
int
ewl_widget_layer_get(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	DRETURN_INT(LAYER(w), DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve sum of layers
 * @return Returns the absolute layer the widget is placed on.
 * @brief Sums the layers of a widgets parents to determine it's absolute layer.
 */
int
ewl_widget_layer_sum_get(Ewl_Widget *w)
{
	int sum = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, 0);

	while (w) {
		sum += LAYER(w);
		w = w->parent;
	}

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
void
ewl_widget_tab_order_append(Ewl_Widget *w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);
	if (emb) ewl_embed_tab_order_append(emb, w);

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
void
ewl_widget_tab_order_prepend(Ewl_Widget *w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);
	if (emb) ewl_embed_tab_order_prepend(emb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be moved to the front of the focus list
 * @param idx: The index to insert the tab into
 * @return Returns no value.
 * @brief Changes the order in the embed so @a w receives focus first on tab.
 *
 * This moves the widget @a w to the given index in the tab order list in the embed
 * that holds it. 
 */
void
ewl_widget_tab_order_insert(Ewl_Widget *w, unsigned int idx)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);
	if (emb) ewl_embed_tab_order_insert(emb, w, idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to be inserted into the tab order
 * @param before: The widget we are to be inserted before
 * @return Returns no value.
 * @brief Inserts the widget into the tab order before the @a before widget
 */
void
ewl_widget_tab_order_insert_before(Ewl_Widget *w, Ewl_Widget *before)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("before", before);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("before", before, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);
	if (emb) ewl_embed_tab_order_insert_before(emb, w, before);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to insert into the tab order
 * @param after: The widget to insert after
 * @return Returns no value.
 * @brief Insert the given widget into the tab order after the @a after
 * widget
 */
void
ewl_widget_tab_order_insert_after(Ewl_Widget *w, Ewl_Widget *after)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("after", after);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("after", after, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);
	if (emb) ewl_embed_tab_order_insert_after(emb, w, after);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to remove from the tab order
 * @return Returns no value.
 * @brief Remove the widget from the tab order
 */
void
ewl_widget_tab_order_remove(Ewl_Widget *w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);
	if (emb) ewl_embed_tab_order_remove(emb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to set if it accepts or blocks focus changes
 * @param val: TRUE or FALSE on if this widget blocks tabbing off
 * @return Returns no value.
 * @brief Set if the widget should ignore focus changes
 */
void
ewl_widget_ignore_focus_change_set(Ewl_Widget *w, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (val)
		ewl_object_flags_add(EWL_OBJECT(w), 
				EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS,
				EWL_FLAGS_PROPERTY_MASK);
	else
		ewl_object_flags_remove(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS,
				EWL_FLAGS_PROPERTY_MASK);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to check if it blocks tab focus
 * @return Returns TRUE if the widget blocks tab focus, FALSE otherwise.
 * @brief Get if the widget is ignoring focus changes
 */
unsigned int
ewl_widget_ignore_focus_change_get(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_flags_has(EWL_OBJECT(w), 
					EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS,
					EWL_FLAGS_PROPERTY_MASK))
		DRETURN_INT(TRUE, DLEVEL_STABLE);
	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param w: The widget to set the focusable values
 * @param val: The focusable value to set
 * @return Returns no value
 * @brief Set if the given widget is focusable or not
 */
void
ewl_widget_focusable_set(Ewl_Widget *w, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (val)
		ewl_object_flags_add(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_FOCUSABLE,
				EWL_FLAGS_PROPERTY_MASK);
	else
		ewl_object_flags_remove(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_FOCUSABLE,
				EWL_FLAGS_PROPERTY_MASK);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to get the focusable state from
 * @return Returns TRUE if the widget is focusable, FALSE otherwise
 * @brief Checks the focusable state of the widget
 */
unsigned int
ewl_widget_focusable_get(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_flags_has(EWL_OBJECT(w), 
					EWL_FLAG_PROPERTY_FOCUSABLE,
					EWL_FLAGS_PROPERTY_MASK))
		DRETURN_INT(TRUE, DLEVEL_STABLE);
	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to display ancestry tree
 * @return Returns no value.
 * @brief Prints to stdout the tree of widgets that are parents of a widget.
 */
void
ewl_widget_tree_print(Ewl_Widget *w)
{
	int i = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	while (w) {
		int j;
		for (j = 0; j < i; j++)
			printf("\t");

		ewl_widget_print(w);

		w = w->parent;
		i++;
	}
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to print info
 * @return Returns no value.
 * @brief Prints info for debugging a widget's state information.
 */
void
ewl_widget_print(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
 * @param w: the widget to print verbose info
 * @return Returns no value.
 * @brief Prints verbose info for debugging a widget's state information.
 */
void
ewl_widget_print_verbose(Ewl_Widget *w)
{
	unsigned int flags;
	unsigned int matched = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	flags = ewl_object_fill_policy_get(EWL_OBJECT(w));

	ewl_widget_print(w);
	printf("\tPreferred size: %dx%d\n",
			ewl_object_preferred_w_get(EWL_OBJECT(w)),
			ewl_object_preferred_h_get(EWL_OBJECT(w)));
	printf("\tMinimum size: %dx%d\n",
			ewl_object_minimum_w_get(EWL_OBJECT(w)),
			ewl_object_minimum_h_get(EWL_OBJECT(w)));
	printf("\tFill policy:\n");

	if (flags & EWL_FLAG_FILL_HSHRINK) {
		printf("\t\tHSHRINK\n");
		matched = 1;
	}

	if (flags & EWL_FLAG_FILL_HFILL) {
		printf("\t\tHFILL\n");
		matched = 1;
	}

	if (flags & EWL_FLAG_FILL_VSHRINK) {
		printf("\t\tVSHRINK\n");
		matched = 1;
	}

	if (flags & EWL_FLAG_FILL_VFILL) {
		printf("\t\tVFILL\n");
		matched = 1;
	}

	if (!matched) {
		printf("\t\tNONE\n");
	}

	matched = 0;

	flags = ewl_object_alignment_get(EWL_OBJECT(w));

	if (flags & EWL_FLAG_ALIGN_LEFT) {
		printf("\t\tLEFT\n");
		matched = 1;
	}

	if (flags & EWL_FLAG_ALIGN_RIGHT) {
		printf("\t\tRIGHT\n");
		matched = 1;
	}

	if (flags & EWL_FLAG_ALIGN_TOP) {
		printf("\t\tTOP\n");
		matched = 1;
	}

	if (flags & EWL_FLAG_ALIGN_BOTTOM) {
		printf("\t\tTOP\n");
		matched = 1;
	}

	if (!matched) {
		printf("\t\tCENTER\n");
	}


	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
void
ewl_widget_internal_set(Ewl_Widget *w, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_inherit(Ewl_Widget *widget, char *inherit)
{
	int len;
	char *tmp = NULL;
	const char *tmp2 = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);
	DCHECK_PARAM_PTR("inherit", inherit);
	/* can't check the inheritence here as we don't know if the widget
	 * has any inheritence yet (ewl_widget_init() calls this function to
	 * set the inheritence */

	len = strlen(inherit) +  3;
	tmp2 = widget->inheritance;
	if (tmp2)
		len += strlen(tmp2);
	else
		tmp2 = "";

	tmp = malloc(sizeof(char) * len);
	sprintf(tmp, "%s:%s:", tmp2, inherit);

	/*
	 * Intentionally lose a reference to the ecore string to keep a
	 * reference cached for later re-use.
	 */
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
unsigned int
ewl_widget_type_is(Ewl_Widget *widget, const char *type)
{
	int found = FALSE;
	char tmp[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, FALSE);
	DCHECK_PARAM_PTR_RET("type", type, FALSE);
	/* we don't check the type of the widget in here or we'll get into
	 * an infinate loop ... */

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
unsigned int
ewl_widget_internal_is(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, FALSE);

	if (ewl_object_flags_has(EWL_OBJECT(w), EWL_FLAG_PROPERTY_INTERNAL,
				EWL_FLAGS_PROPERTY_MASK))
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param w: The widget to check
 * @return Returns TRUE if the widget is onscreen
 * @brief Checks if the given widget is currently on screen
 */
unsigned int
ewl_widget_onscreen_is(Ewl_Widget *w)
{
	int onscreen = FALSE;
	Ewl_Embed *emb;

	DCHECK_PARAM_PTR_RET("w", w, 0);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/*
	 * Until an embed is present, the widget is off screen by default.
	 */
	emb = ewl_embed_widget_find(w);
	if (emb) {
		onscreen = TRUE;
	}

	/*
	 * We don't need to configure if it's outside the viewable space in
	 * it's parent widget. This does not recurse upwards to determine
	 * ancestor clipped areas, but does obscure items that are outside the
	 * top level container.
	 */
	if (w->parent) {
		int x, y;
		int width, height;
		Ewl_Widget *p = w->parent;

		ewl_object_current_geometry_get(EWL_OBJECT(w), &x, &y, &width,
				&height);

		if ((x + width) < CURRENT_X(p))
			onscreen = FALSE;

		if (x > (CURRENT_X(p) + CURRENT_W(p)))
			onscreen = FALSE;

		if ((y + height) < CURRENT_Y(p))
			onscreen = FALSE;

		if (y > (CURRENT_Y(p) + CURRENT_H(p)))
			onscreen = FALSE;

		if ((x + width) < CURRENT_X(emb))
			onscreen = FALSE;

		if (x > (CURRENT_X(emb) + CURRENT_W(emb)))
			onscreen = FALSE;

		if ((y + height) < CURRENT_Y(emb))
			onscreen = FALSE;

		if (y > (CURRENT_Y(emb) + CURRENT_H(emb)))
			onscreen = FALSE;
	}

	if (onscreen == TRUE && w->parent) {
		if ( (ewl_widget_onscreen_is(w->parent) == FALSE)) onscreen = FALSE;
	}

	DRETURN_INT(onscreen, DLEVEL_STABLE);
}

/**
 * @param w: the widget to mark as unclipped
 * @param val: the state of the clipping flag
 * @return Returns no value.
 * @brief Marks whether the widget should be clipped at it's boundaries.
 */
void
ewl_widget_clipped_set(Ewl_Widget *w, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
unsigned int
ewl_widget_clipped_is(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, FALSE);

	if (ewl_object_flags_has(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP,
					EWL_FLAGS_VISIBLE_MASK))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param c: the widget to compare against
 * @param w: the widget to check parentage
 * @return Returns TRUE if @a c is a parent of @a w, otherwise returns FALSE.
 * @brief Determine if a widget is a parent of another widget
 */
int
ewl_widget_parent_of(Ewl_Widget *c, Ewl_Widget *w)
{
	Ewl_Widget *parent;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, FALSE);
	DCHECK_TYPE("c", c, EWL_WIDGET_TYPE);

	if (!w) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	parent = w;
	while ((parent = parent->parent)) {
		if (parent == c)
			DRETURN_INT(TRUE, DLEVEL_STABLE);
	}

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to receive keyboard focus
 * @return Returns no value.
 * @brief Changes the keyboard focus to the widget @a w.
 */
void
ewl_widget_focus_send(Ewl_Widget *w)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);
	ewl_embed_active_set(emb, TRUE);
	ewl_embed_focused_widget_set(emb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the currnetly focused widget.
 * @brief Retrieve the currently focused widget.
 */
Ewl_Widget *
ewl_widget_focused_get(void)
{
	Ewl_Embed *emb;
	Ewl_Widget *w = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = ewl_embed_active_embed_get();
	if (emb) w = ewl_embed_focused_widget_get(emb);

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
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	color = NEW(Ewl_Color_Set, 1);
	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;
	ewl_attach_color_set(w, color);

	if (w->fx_clip_box)
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
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			void *data __UNUSED__)
{
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN, ewl_widget_drag_down_cb);
	ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, ewl_widget_drag_move_cb);
	ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, ewl_widget_drag_up_cb);

	/*
	 * First remove the parents reference to this widget to avoid bad
	 * references.
	 */
	if (w->parent)
		ewl_container_child_remove(EWL_CONTAINER(w->parent), w);

	ewl_widget_unrealize(w);

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

	/*
	 * Free up appearance related information
	 */
	ewl_theme_widget_shutdown(w);

	if (w->appearance) {
		ecore_string_release(w->appearance);
		w->appearance = NULL;
	}

	if (w->inheritance) {
		ecore_string_release(w->inheritance);
		w->inheritance = NULL;
	}

	if (w->bit_state) {
		ecore_string_release(w->bit_state);
		w->inheritance = NULL;
		w->bit_state = NULL;
	}

	if (w->theme_text.list) {
		if (w->theme_text.direct) {
			ecore_string_release(EWL_PAIR(w->theme_text.list)->key);
			FREE(EWL_PAIR(w->theme_text.list)->value);
		}
		else {
			int i;
			for (i = 0; i < w->theme_text.len; i++) {
				ecore_string_release(w->theme_text.list[i]->key);
				FREE(w->theme_text.list[i]->value);
				FREE(w->theme_text.list[i]);
			}
		}

		FREE(w->theme_text.list);
		w->theme_text.len = 0;
	}

	if (w->data) {
		ecore_hash_destroy(w->data);
		w->data = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Every widget must show it's fx_clip_box to be seen
 */
void
ewl_widget_show_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			void *user_data __UNUSED__)
{
	Ewl_Container *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (w->fx_clip_box)
		evas_object_show(w->fx_clip_box);
	if (w->theme_object)
		evas_object_show(w->theme_object);

	pc = EWL_CONTAINER(w->parent);
	if (pc)
		ewl_container_child_show_call(pc, w);

	ewl_widget_tab_order_prepend(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Every widget must hide it's fx_clip_box in order to hide
 */
void
ewl_widget_hide_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			void *user_data __UNUSED__)
{
	Ewl_Container *pc;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_widget_obscure(w);

	/*
	 * Notify parent of hidden state.
	 */
	pc = EWL_CONTAINER(w->parent);
	if (pc)
		ewl_container_child_hide_call(pc, w);

	emb = ewl_embed_widget_find(w);
	if (emb)
		ewl_embed_info_widgets_cleanup(emb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Request a new set of evas objects now that we're back on screen
 */
void ewl_widget_reveal_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
                                          void *user_data __UNUSED__)
{
	int sum;
	Ewl_Embed *emb;

	/* printf("Revealing %s\n", w->appearance); */

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_widget_find(w);
	if (!emb || !emb->evas)
		DRETURN(DLEVEL_STABLE);

	/*
	 * No object allocated yet for this widget
	 */
	if (!w->theme_object && w->bit_path && w->bit_group) {
		/*
		 * Attempt to load a cached object first, fallback to adding a
		 * new one.
                 */
		w->theme_object = ewl_embed_object_request(emb, "edje");
		if (!w->theme_object) {
			w->theme_object = edje_object_add(emb->evas);
			evas_object_repeat_events_set(w->theme_object, 1);
			/*
			printf("Created edje %d: %s\n", ++edjes,
					w->bit_group);
					*/
		}

		/*
		 * Attempt to load the theme object
		 */
		evas_object_repeat_events_set(w->theme_object, 1);
		edje_object_file_set(w->theme_object, w->bit_path,
				w->bit_group);
		/*
		 * If the file failed to load, destroy the unnecessary evas
		 * object.
		 */
		if (edje_object_load_error_get(w->theme_object)) {
			evas_object_del(w->theme_object);
			w->theme_object = NULL;
		}

		/*
		 * Set the state of the theme object
		 */
		if (w->bit_state)
			ewl_widget_state_set(w, (char *)w->bit_state);

		if (ewl_object_state_has(EWL_OBJECT(w),
					EWL_FLAG_STATE_DISABLED))
			ewl_widget_state_set(w, "disabled");

		/*
		 * Apply any text overrides
		 */
		if (w->theme_object && w->theme_text.list) {
			const char *key;
		       	char *value;

			if (w->theme_text.direct) {
				key = EWL_PAIR(w->theme_text.list)->key;
				value = EWL_PAIR(w->theme_text.list)->value;
				ewl_widget_appearance_part_text_apply(w,
						key, value);
					
			}
			else {
				int i;
				for (i = 0; i < w->theme_text.len; i++) {
					key = w->theme_text.list[i]->key;
					value = w->theme_text.list[i]->value;
					ewl_widget_appearance_part_text_apply(w,
							key, value);
				}
			}
		}
	}

	/*
	 * Create clip box if necessary
	 */
	if (!w->fx_clip_box && !ewl_object_flags_get(EWL_OBJECT(w), EWL_FLAG_VISIBLE_NOCLIP)) {
		w->fx_clip_box = ewl_embed_object_request(emb, "rectangle");
		if (!w->fx_clip_box) {
			w->fx_clip_box = evas_object_rectangle_add(emb->evas);
			/* printf("Created rectangle %d\n", ++rects); */
		}
		evas_object_pass_events_set(w->fx_clip_box, TRUE);
	}

	if (w->theme_object && w->fx_clip_box)
		evas_object_clip_set(w->theme_object, w->fx_clip_box);

	/*
	 * Setup the appropriate clippings.
	 */
	if (w->parent && EWL_CONTAINER(w->parent)->clip_box && w->fx_clip_box) {
		evas_object_clip_set(w->fx_clip_box,
				EWL_CONTAINER(w->parent)->clip_box);
		evas_object_show(EWL_CONTAINER(w->parent)->clip_box);
	}

	/*
	 * Set the layer of the clip box and theme object
	 */
	sum = ewl_widget_layer_sum_get(w);
	if (sum > ewl_embed_max_layer_get(emb))
		ewl_embed_max_layer_set(emb, sum);
	if (w->theme_object) {
		evas_object_layer_set(w->theme_object, sum);
		/* printf("Setting layer %d on %s\n", sum, w->appearance); */
	}

	if (w->fx_clip_box) {
		Ewl_Color_Set *color;

		evas_object_layer_set(w->fx_clip_box, sum);

		color = ewl_attach_color_get(w);
		if (color)
			evas_object_color_set(w->fx_clip_box, color->r,
						color->g, color->b, color->a);
	}

	/*
	 * Show the theme and clip box if widget is visible
	 */
	if (VISIBLE(w)) {
		evas_object_show(w->fx_clip_box);
		evas_object_show(w->theme_object);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Give up unnecessary objects when we're offscreen
 */
void ewl_widget_obscure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
                                           void *user_data __UNUSED__)
{
	Ewl_Embed *emb;
	Ewl_Container *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/* printf("Obscuring %s\n", w->appearance); */

	emb = ewl_embed_widget_find(w);
	if (!emb)
		DRETURN(DLEVEL_STABLE);

	pc = EWL_CONTAINER(w->parent);

	/*
	 * Remove all properties on the edje and hand it back to the embed for
	 * caching.
	 */
	if (w->theme_object) {
		/* edje_object_file_set(w->theme_object, "", ""); */
		ewl_embed_object_cache(emb, w->theme_object);
		w->theme_object = NULL;
	}

	/*
	 * Repeat the process for the clip rect, but also hide the parent clip
	 * box if there are no visible children. If we don't hide it, there
	 * will be a white rectangle displayed.
	 */
	if (w->fx_clip_box) {
		if (pc && pc->clip_box) {
			if (!evas_object_clipees_get(pc->clip_box))
				evas_object_hide(pc->clip_box);
		}
		evas_object_hide(w->fx_clip_box);
		evas_object_clip_unset(w->theme_object);
		ewl_embed_object_cache(emb, w->fx_clip_box);
		w->fx_clip_box = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for realizing a widget
 */
void
ewl_widget_realize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			void *user_data __UNUSED__)
{
	int l = 0, r = 0, t = 0, b = 0;
	int i_l = 0, i_r = 0, i_t = 0, i_b = 0;
	int p_l = 0, p_r = 0, p_t = 0, p_b = 0;
	char *i = NULL;
	char *group = NULL;
	Evas_Coord width, height;
	Ewl_Embed *emb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);

	/*
	 * Retrieve the path to the theme file that will be loaded
	 * return if no file to be loaded. Also get the group name in the
	 * theme file.
	 */
	i = ewl_theme_image_get(w, "file");
	group = ewl_theme_data_str_get(w, "group");

	if (i)
		w->bit_path = ecore_string_instance(i);

	if (group)
		w->bit_group = ecore_string_instance(group);

	IF_FREE(i);
	IF_FREE(group);

	/*
	 * Reveal is done in this part of the callback to avoid duplicate code
	 * for creating the evas objects. Must be done in the callback so that
	 * prepended callbacks in the embed can create the evas, windows,
	 * etc.
	 */
	ewl_object_visible_add(EWL_OBJECT(w), EWL_FLAG_VISIBLE_OBSCURED);
	ewl_widget_reveal(w);

	/*
	 * Set up the theme object on the widgets evas
	 */
	if (w->theme_object) {

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
		 * Assign the relevant insets and padding.
		 */
		ewl_object_insets_set(EWL_OBJECT(w), i_l, i_r, i_t, i_b);
		ewl_object_padding_set(EWL_OBJECT(w), p_l, p_r, p_t, p_b);

		i_l = CURRENT_X(w);
		i_t = CURRENT_Y(w);
		ewl_object_x_request(EWL_OBJECT(w), i_l);
		ewl_object_y_request(EWL_OBJECT(w), i_t);

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

	DRETURN(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for unrealizing a widget
 */
void
ewl_widget_unrealize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (w->theme_object) {
		int i_l, i_r, i_t, i_b;
		int p_l, p_r, p_t, p_b;
		int l, r, t, b;

		ewl_widget_theme_insets_get(w, &l, &r, &t, &b);

		ewl_object_insets_get(EWL_OBJECT(w), &i_l, &i_r, &i_t, &i_b);
		ewl_object_padding_get(EWL_OBJECT(w), &p_l, &p_r, &p_t, &p_b);

		/*
		 * Use previously set insets and padding if available.
		 */
		if (l == i_l)
			i_l = 0;
		if (r == i_r)
			i_r = 0;
		if (t == i_t)
			i_t = 0;
		if (b == i_b)
			i_b = 0;

		ewl_widget_theme_padding_get(w, &l, &r, &t, &b);

		if (l == p_l)
			p_l = 0;
		if (r == p_r)
			p_r = 0;
		if (t == p_t)
			p_t = 0;
		if (b == p_b)
			p_b = 0;


		/*
		 * Assign the relevant insets and padding.
		 */
		ewl_object_insets_set(EWL_OBJECT(w), i_l, i_r, i_t, i_b);
		ewl_object_padding_set(EWL_OBJECT(w), p_l, p_r, p_t, p_b);
	}

	

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Perform the basic operations necessary for configuring a widget
 */
void
ewl_widget_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			void *user_data __UNUSED__)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
void
ewl_widget_reparent_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
			void *user_data __UNUSED__)
{
	Ewl_Container *pc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	pc = EWL_CONTAINER(w->parent);
	if (REALIZED(w))
		ewl_widget_unrealize(w);

	if (pc && REALIZED(pc) && VISIBLE(w) && !REALIZED(w))
		ewl_realize_request(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_widget_enable_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_widget_state_set(w, "default");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_disable_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_widget_state_set(w, "disabled");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_focus_in_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	ewl_widget_state_set(w, "focus,in");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_focus_out_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	ewl_widget_state_set(w, "focus,out");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_mouse_in_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	ewl_widget_state_set(w, "mouse,in");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_mouse_out_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_DISABLED))
		DRETURN(DLEVEL_STABLE);

	snprintf(state, 14, "mouse,up,%i", e->button);
	ewl_widget_state_set(w, state);

	if (ewl_object_state_has(EWL_OBJECT(w), EWL_FLAG_STATE_MOUSE_IN)) {
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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_widget_state_set(w, "mouse,move");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_widget_theme_padding_get(Ewl_Widget *w, int *l, int *r, int *t, int *b)
{
	const char *key;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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

/*
void
ewl_widget_child_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (w->parent)
		ewl_container_child_remove(EWL_CONTAINER(w->parent), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
*/


void
ewl_widget_drag_down_cb(Ewl_Widget *w, void *ev_data ,
				void *user_data __UNUSED__)
{
	Ewl_Event_Mouse_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ev_data", ev_data);

	ev = ev_data;
	if (!ewl_dnd_status_get()) 
		DRETURN(DLEVEL_STABLE);

	if (ev->button == 1 && !ewl_object_flags_has(EWL_OBJECT(w), 
						EWL_FLAG_STATE_DND_WAIT, 
						EWL_FLAGS_STATE_MASK)) 
	{
		ewl_object_flags_add(EWL_OBJECT(w), EWL_FLAG_STATE_DND_WAIT, 
							EWL_FLAGS_STATE_MASK);
		ewl_widget_dnd_drag_move_count=0;		
		ewl_widget_drag_widget = w;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_drag_move_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
				void *user_data __UNUSED__) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_dnd_status_get())
		DRETURN(DLEVEL_STABLE);
	
	if (ewl_widget_drag_widget 
			&& ewl_object_flags_has(EWL_OBJECT(ewl_widget_drag_widget), 
						EWL_FLAG_STATE_DND_WAIT, 
						EWL_FLAGS_STATE_MASK)) 
	{
		ewl_widget_dnd_drag_move_count++;
		
		if (ewl_widget_dnd_drag_move_count > 2) 
		{
			ewl_object_flags_remove(EWL_OBJECT(ewl_widget_drag_widget), 
						EWL_FLAG_STATE_DND_WAIT, 
						EWL_FLAGS_STATE_MASK);
			ewl_object_flags_add(EWL_OBJECT(ewl_widget_drag_widget), 
						EWL_FLAG_STATE_DND,
						EWL_FLAGS_STATE_MASK);
		
			ewl_drag_start(ewl_widget_drag_widget);	
		}
	}
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_widget_drag_up_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
					void *user_data __UNUSED__) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_object_flags_has(EWL_OBJECT(ewl_widget_drag_widget), 
					EWL_FLAG_STATE_DND_WAIT, 
					EWL_FLAGS_STATE_MASK)) 
		ewl_object_flags_remove(EWL_OBJECT(ewl_widget_drag_widget), 
					EWL_FLAG_STATE_DND_WAIT, 
					EWL_FLAGS_STATE_MASK);

	ewl_widget_dnd_drag_move_count = 0;
	ewl_widget_drag_widget = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to set draggable state
 * @param val: the true/false state of draggable
 * @param cb: The drag callback to set
 * @return Returns no value.
 * @brief Set the draggable state, and setup any callbacks
 */
void
ewl_widget_draggable_set(Ewl_Widget *w, unsigned int val, Ewl_Widget_Drag cb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (val) { 
		if (!ewl_object_flags_has(EWL_OBJECT(w), 
					EWL_FLAG_PROPERTY_DRAGGABLE, 
					EWL_FLAGS_PROPERTY_MASK)) 
		{
			ewl_object_flags_add(EWL_OBJECT(w), 
						EWL_FLAG_PROPERTY_DRAGGABLE, 
						EWL_FLAGS_PROPERTY_MASK );

			ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, 
						ewl_widget_drag_down_cb, NULL);
			ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE, 
						ewl_widget_drag_move_cb, NULL);
			ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, 
						ewl_widget_drag_up_cb, NULL);

			if (cb) {
				ewl_widget_data_set(w, "DROP_CB", (void *)cb);
			}
		}
	} 
	else 
	{
		if (ewl_object_flags_has(EWL_OBJECT(w), 
						EWL_FLAG_PROPERTY_DRAGGABLE, 
						EWL_FLAGS_PROPERTY_MASK)) 
		{
			ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN, 
						ewl_widget_drag_down_cb);
			ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, 
						ewl_widget_drag_move_cb);
			ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, 
						ewl_widget_drag_up_cb);

			ewl_object_flags_remove(EWL_OBJECT(w), EWL_FLAG_PROPERTY_DRAGGABLE,  EWL_FLAGS_PROPERTY_MASK);

		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the current widget that has been clicked or moved (i.e. drag_wait)
 * @brief Accessor function for the current drag candidate widget
 *
 */
Ewl_Widget * 
ewl_widget_drag_candidate_get(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ewl_widget_drag_widget, DLEVEL_STABLE);
}

/**
 * @return none
 * @brief Cancel any active dnd_wait state widgets
 *
 */
void
ewl_widget_dnd_reset(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_widget_drag_widget) {
		Ewl_Widget* temp = ewl_widget_drag_widget;

		while (temp) {
			ewl_object_state_remove(EWL_OBJECT(temp), EWL_FLAG_STATE_PRESSED);
			temp = temp->parent;
		}

		ewl_embed_active_set(ewl_embed_widget_find(ewl_widget_drag_widget), 0);
	}

	ewl_widget_dnd_drag_move_count = 0;
	ewl_widget_drag_widget = NULL;
//	printf("Drag reset..\n");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

