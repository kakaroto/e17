
#include <Ewl.h>


extern Ewl_Widget *last_selected;
extern Ewl_Widget *last_key;
extern Ewl_Widget *last_focused;
extern Ewl_Widget *dnd_widget;


void
ewl_widget_init(Ewl_Widget * w, int req_w, int req_h, Ewl_Fill_Policy fill,
		Ewl_Alignment align)
{
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Set up the necessary theme structures 
	 */
	ewl_theme_init_widget(EWL_WIDGET(w));

	/*
	 * Set size fields on the object 
	 */
	ewl_object_init(EWL_OBJECT(w), req_w, req_h, fill, align);

	DLEAVE_FUNCTION;
}

void
ewl_widget_realize(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (ewl_object_is_realized(EWL_OBJECT(w)))
		DRETURN;

	ewl_object_set_realized(EWL_OBJECT(w), TRUE);

	ewl_callback_call(w, EWL_CALLBACK_REALIZE);

	DLEAVE_FUNCTION;
}

void
ewl_widget_show(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (ewl_object_is_visible(EWL_OBJECT(w)))
		DRETURN;

	if (!ewl_object_is_realized(EWL_OBJECT(w)))
		ewl_widget_realize(w);

	ewl_object_set_visible(EWL_OBJECT(w), TRUE);

	ewl_callback_call(w, EWL_CALLBACK_SHOW);

	DLEAVE_FUNCTION;
}

void
ewl_widget_hide(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_call(w, EWL_CALLBACK_HIDE);

	ewl_object_set_visible(EWL_OBJECT(w), FALSE);

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
ewl_widget_set_data(Ewl_Widget * w, char *k, void *v)
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
ewl_widget_del_data(Ewl_Widget * w, char *k)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (!w->data)
		DRETURN;

	DLEAVE_FUNCTION;
}

void *
ewl_widget_get_data(Ewl_Widget * w, char *k)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	if (!w->data)
		DRETURN_PTR(NULL);

	DRETURN_PTR(ewd_hash_get(w->data, k));
}
