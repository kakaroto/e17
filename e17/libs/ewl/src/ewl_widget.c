
#include <Ewl.h>


extern Ewl_Widget *last_selected;
extern Ewl_Widget *last_key;
extern Ewl_Widget *last_focused;
extern Ewl_Widget *dnd_widget;


void
ewl_widget_init(Ewl_Widget * w, int type, int req_w, int req_h,
		int max_w, int max_h)
{
	CHECK_PARAM_POINTER("w", w);

	/*
	 * Set up the necessary theme structures 
	 */
	ewl_theme_init_widget(EWL_WIDGET(w));
	w->type = type;

	/*
	 * Set size fields on the object 
	 */
	MAX_W(w) = max_w;
	MAX_H(w) = max_h;

	MIN_W(w) = CURRENT_W(w) = REQUEST_W(w) = req_w;
	MIN_H(w) = CURRENT_H(w) = REQUEST_H(w) = req_h;
}

void
ewl_widget_realize(Ewl_Widget * w)
{
	int realized = 0;

	CHECK_PARAM_POINTER("w", w);

	ewl_object_get_realized(EWL_OBJECT(w), &realized);

	if (realized)
		return;

	ewl_object_set_realized(EWL_OBJECT(w), TRUE);

	ewl_callback_call(w, EWL_CALLBACK_REALIZE);
}

void
ewl_widget_show(Ewl_Widget * w)
{
	int visible = 0;
	int realized = 0;

	CHECK_PARAM_POINTER("w", w);

	ewl_object_get_visible(EWL_OBJECT(w), &visible);
	ewl_object_get_realized(EWL_OBJECT(w), &realized);

	if (visible)
		return;

	if (!realized)
		ewl_widget_realize(w);

	ewl_object_set_visible(EWL_OBJECT(w), TRUE);

	ewl_callback_call(w, EWL_CALLBACK_SHOW);
}

void
ewl_widget_hide(Ewl_Widget * w)
{
	CHECK_PARAM_POINTER("w", w);

	ewl_callback_call(w, EWL_CALLBACK_HIDE);

	ewl_object_set_visible(EWL_OBJECT(w), FALSE);
}

void
ewl_widget_destroy(Ewl_Widget * w)
{
	CHECK_PARAM_POINTER("w", w);

	if (last_selected == w)
		last_selected = NULL;

	if (last_key == w)
		last_key = NULL;

	if (last_focused == w)
		last_focused = NULL;

	if (dnd_widget == w)
		dnd_widget = NULL;

	ewl_callback_call(w, EWL_CALLBACK_DESTROY);
}

void
ewl_widget_destroy_recursive(Ewl_Widget * w)
{
	CHECK_PARAM_POINTER("w", w);

	ewl_callback_call(w, EWL_CALLBACK_DESTROY_RECURSIVE);
	ewl_widget_destroy(w);
}

void
ewl_widget_configure(Ewl_Widget * w)
{
	CHECK_PARAM_POINTER("w", w);

	ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);
}

void
ewl_widget_theme_update(Ewl_Widget * w)
{
	CHECK_PARAM_POINTER("w", w);

	ewl_callback_call(w, EWL_CALLBACK_THEME_UPDATE);
}
