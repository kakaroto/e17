
#include <Ewl.h>


extern Ewl_Widget *last_selected;
extern Ewl_Widget *last_key;
extern Ewl_Widget *last_focused;
extern Ewl_Widget *dnd_widget;

static void __ewl_widget_show(Ewl_Widget *w, void *event_data, void *user_data);
static void __ewl_widget_hide(Ewl_Widget *w, void *event_data, void *user_data);
static void __ewl_widget_realize(Ewl_Widget *w, void *event_data,
		void *user_data);
static void __ewl_widget_destroy(Ewl_Widget *w, void *event_data,
		void *user_data);
static void __ewl_widget_reparent(Ewl_Widget *w, void *event_data,
		void *user_data);

void
ewl_widget_init(Ewl_Widget * w, char *appearance)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Set up the necessary theme structures 
	 */
	ewl_theme_init_widget(EWL_WIDGET(w));

	/*
	 * The base appearance is used for updating the appearance of the
	 * widget
	 */
	w->appearance = strdup(appearance);

	/*
	 * Add the common callbacks that all widgets must perform
	 */
	ewl_callback_append(w, EWL_CALLBACK_SHOW, __ewl_widget_show, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE, __ewl_widget_hide, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_widget_realize,NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_widget_configure,
			NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			__ewl_widget_theme_update, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, __ewl_widget_destroy,NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT, __ewl_widget_reparent,
			NULL);

	/*
	 * Set size fields on the object 
	 */
	ewl_object_init(EWL_OBJECT(w));

	DLEAVE_FUNCTION;
}

void
ewl_widget_realize(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (REALIZED(w))
		DRETURN;

	w->visible |= EWL_VISIBILITY_REALIZED;

	ewl_callback_call(w, EWL_CALLBACK_REALIZE);

	DLEAVE_FUNCTION;
}

void
ewl_widget_show(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (w->visible)
		DRETURN;

	if (!REALIZED(w))
		ewl_widget_realize(w);

	w->visible |= EWL_VISIBILITY_SHOWN;

	ewl_callback_call(w, EWL_CALLBACK_SHOW);

	DLEAVE_FUNCTION;
}

void
ewl_widget_hide(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	w = EWL_VISIBILITY_HIDDEN;

	ewl_callback_call(w, EWL_CALLBACK_HIDE);

	DLEAVE_FUNCTION;
}

void
ewl_widget_destroy(Ewl_Widget * w)
{
	DENTER_FUNCTION;
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

	DLEAVE_FUNCTION;
}

void
ewl_widget_destroy_recursive(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_call(w, EWL_CALLBACK_DESTROY_RECURSIVE);
	ewl_widget_destroy(w);

	DLEAVE_FUNCTION;
}

void
ewl_widget_configure(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

	DLEAVE_FUNCTION;
}

void
ewl_widget_theme_update(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_call(w, EWL_CALLBACK_THEME_UPDATE);

	DLEAVE_FUNCTION;
}

void
ewl_widget_set_data(Ewl_Widget * w, void *k, void *v)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);
	DCHECK_PARAM_PTR("v", v);

	if (!w->data)
		w->data = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	ewd_hash_set(w->data, k, v);

	DLEAVE_FUNCTION;
}

void
ewl_widget_del_data(Ewl_Widget * w, void *k)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (!w->data)
		DRETURN;

	DLEAVE_FUNCTION;
}

void *
ewl_widget_get_data(Ewl_Widget * w, void *k)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	if (!w->data)
		DRETURN_PTR(NULL);

	DRETURN_PTR(ewd_hash_get(w->data, k));
}

void
ewl_widget_set_appearance(Ewl_Widget *w, char *appearance)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	IF_FREE(w->appearance);
	w->appearance = (appearance ? strdup(appearance) : NULL);

	DLEAVE_FUNCTION;
}

char *
ewl_widget_get_appearance(Ewl_Widget *w)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR_RET("w", w, NULL);

	return (w->appearance ? strdup(w->appearance) : NULL);
}

/*
 * This simplifies updating the appearance of the widget
 */
void
ewl_widget_update_appearance(Ewl_Widget *w, char *state)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("state", state);

	/*
	 * Set up the ebits object on the widgets evas
	 */
	if (!w->ebits_object)
		ewl_widget_theme_update(w);

	if (!w->ebits_object)
		DRETURN;

	ebits_set_named_bit_state(w->ebits_object, "Base", state);

	DRETURN;
}

void
ewl_widget_set_parent(Ewl_Widget * w, Ewl_Widget * p)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("p", p);

	w->parent = p;

	ewl_callback_call(w, EWL_CALLBACK_REPARENT);

	DLEAVE_FUNCTION;
}

/*
 * Perform the series of operations common to every widget when
 * they are destroyed
 */
static void
__ewl_widget_destroy(Ewl_Widget *w, void *ev_data, void *data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Destroy the ebits object that gives the widget it's
	 * appearance
	 */
	if (w->ebits_object) {
		ebits_hide(w->ebits_object);
		ebits_unset_clip(w->ebits_object);
		ebits_free(w->ebits_object);
	}

	/*
	 * Destroy the fx_clip_box of the widget
	 */
	if (w->fx_clip_box) {
		evas_hide(w->evas, w->fx_clip_box);
		evas_unset_clip(w->evas, w->fx_clip_box);
		evas_del_object(w->evas, w->fx_clip_box);
	}

	/*
	 * Free up appearance related information
	 */
	ewl_theme_deinit_widget(w);
	FREE(w->appearance);

	/*
	 * Clear out the callbacks, this is a bit tricky because we don't want
	 * to continue using this widget after the callbacks have been
	 * deleted. So we remove the callbacks of type destroy and then clear
	 * the remaining callbacks. This preserves the list of the destroy
	 * type so we don't get a segfault.
	 */
	ewd_list_clear(w->callbacks[EWL_CALLBACK_DESTROY]);
	w->callbacks[EWL_CALLBACK_DESTROY] = NULL;

	ewl_callback_clear(w);

	FREE(w);

	DLEAVE_FUNCTION;
}

/*
 * Every widget must show it's fx_clip_box to be seen
 */
static void
__ewl_widget_show(Ewl_Widget *w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

/*
 * Every widget must hide it's fx_clip_box in order to hide
 */
static void
__ewl_widget_hide(Ewl_Widget *w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

/*
 * Perform the basic operations necessary for realizing a widget
 */
static void
__ewl_widget_realize(Ewl_Widget *w, void *event_data, void *user_data)
{
	Ewl_Container * pc;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Create the fx clip box where special fx can be drawn to affect the
	 * entire contents of the widget
	 */
	w->fx_clip_box = evas_add_rectangle(w->evas);
	evas_set_color(w->evas, w->fx_clip_box, 255, 255, 255, 255);
	evas_set_layer(w->evas, w->fx_clip_box, LAYER(w) - 1);

	pc = EWL_CONTAINER(w->parent);

	/*
	 * clip the fx clip box to the parent clip box
	 */
	if (pc && pc->clip_box)
		evas_set_clip(w->evas, w->fx_clip_box, pc->clip_box);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

/*
 * Perform the basic operations necessary for configuring a widget
 */
void
__ewl_widget_configure(Ewl_Widget *w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_object_apply_requested(w);

	/*
	 * Move the base ebits object to the correct size and position
	 */
	if (w->ebits_object)
	  {
		ebits_move(w->ebits_object, CURRENT_X(w), CURRENT_Y(w));
		ebits_resize(w->ebits_object, CURRENT_W(w), CURRENT_H(w));
	  }

	/*
	 * Now move the clipbox to the correct size and position
	 */
	if (w->fx_clip_box)
	  {
		evas_move(w->evas, w->fx_clip_box, CURRENT_X(w), CURRENT_Y(w));
		evas_resize(w->evas, w->fx_clip_box, CURRENT_W(w),
				CURRENT_H(w));
	  }

	DLEAVE_FUNCTION;
}


/*
 * This simplifies updating the appearance of the widget
 */
void
__ewl_widget_theme_update(Ewl_Widget *w, void *event_data, void *user_data)
{
	int len;
	char *i;
	char *key;
	char *visible;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!w->appearance)
		DRETURN;

        /* Destroy old image (if any) */
        if (w->ebits_object) {
                ebits_hide(w->ebits_object);
                ebits_unset_clip(w->ebits_object);
                ebits_free(w->ebits_object);
		w->ebits_object = NULL;
        }

	/*
	 * Calculate the length of the base key string, then allocate the
	 * memory for it plus room for placing /visible at the end.
	 */
	len = strlen(w->appearance) + 14;
	key = NEW(char, len);
	snprintf(key, len, "%s/base/visible", w->appearance);

	/*
	 * Determine the widgets visibility, return if not visible
	 */
	visible = ewl_theme_data_get(w, key);

	if (!visible || !strncasecmp(visible, "no", 2)) {
		FREE(key);
		DRETURN;
	}

	/*
	 * Retrieve the path to the ebits file that will be loaded
	 * return if no file to be loaded.
	 */
	snprintf(key, len, "%s/base", w->appearance);

	i = ewl_theme_image_get(w, key);

	if (!i) {
		FREE(key);
		DRETURN;
	}

	/*
	 * Load the ebits object
	 */
	w->ebits_object = ebits_load(i);

	/*
	 * Set up the ebits object on the widgets evas
	 */
	if (w->ebits_object) {
		ebits_add_to_evas(w->ebits_object, w->evas);
		ebits_set_layer(w->ebits_object, LAYER(w));
		if (w->fx_clip_box)
			ebits_set_clip(w->ebits_object, w->fx_clip_box);
		ebits_show(w->ebits_object);
	}

	FREE(key);

	DRETURN;
}

/*
 * Perform the basic operations necessary for reparenting a widget
 */
static void
__ewl_widget_reparent(Ewl_Widget *w, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!w->parent) {
		DLEAVE_FUNCTION;
		DRETURN;
	}

	/*
	 * Grab the evas settings from the parent and set up the clip box
	 * again if necessary
	 */
        w->evas = EWL_WIDGET(w->parent)->evas;
        w->evas_window = EWL_WIDGET(w->parent)->evas_window;
	if (EWL_CONTAINER(w->parent)->clip_box && w->fx_clip_box)
		evas_set_clip(w->evas, w->fx_clip_box,
				EWL_CONTAINER(w->parent)->clip_box);

        LAYER(w) = LAYER(w->parent) + 1;

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}
