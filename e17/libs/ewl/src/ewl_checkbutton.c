
#include <Ewl.h>


void            __ewl_checkbutton_clicked(Ewl_Widget * w, void *ev_data,
					  void *user_data);

/**
 * ewl_checkbutton_new - allocate and initialize a new check button
 * @label: the label to display with the checkbutton, NULL for no label
 *
 * Returns the newly allocated checkbutton on success, NULL on failure.
 */
Ewl_Widget     *ewl_checkbutton_new(char *label)
{
	Ewl_CheckButton *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_CheckButton, 1);
	if (!b)
		return NULL;

	ZERO(b, Ewl_CheckButton, 1);
	ewl_checkbutton_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * ewl_checkbutton_init - initialize the members and callbacks of a check button
 * @cb: the check button to initialize
 * @label: the label to give the initialized check button
 *
 * Returns no vlalue.The internal structures and callbacks of the checkbutton
 * are initialized ot default values.
 */
void ewl_checkbutton_init(Ewl_CheckButton * cb, char *label)
{
	Ewl_Button     *b;
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = EWL_BUTTON(cb);
	w = EWL_WIDGET(cb);

	ewl_button_init(b, label);
	ewl_widget_set_appearance(w, "/button/check");
	RECURSIVE(b) = FALSE;

	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_NONE);

	ewl_callback_append(w, EWL_CALLBACK_CLICKED,
			    __ewl_checkbutton_clicked, NULL);

	cb->label_position = EWL_POSITION_RIGHT;

	/*
	 * Add the check box first.
	 */
	cb->check = ewl_check_new();
	ewl_container_prepend_child(EWL_CONTAINER(cb), cb->check);
	ewl_widget_show(cb->check);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_checkbutton_set_label_position - set the check buttons label position
 * @w: the widget to change the label positioning
 * @p: the new position of the label
 *
 * Returns no value. Changes the position of the label associated with the
 * check button.
 */
void ewl_checkbutton_set_label_position(Ewl_Widget * w, Ewl_Position p)
{
	Ewl_Button     *b;
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);
	cb = EWL_CHECKBUTTON(w);

	if (cb->label_position == p)
		DRETURN(DLEVEL_STABLE);

	cb->label_position = p;
	ewl_container_remove_child(EWL_CONTAINER(cb),
			EWL_WIDGET(b->label_object));
	ewl_container_remove_child(EWL_CONTAINER(cb), cb->check);

	/*
	 * Add the label and check back into the checkbutton with the correct
	 * order.
	 */
	if (p == EWL_POSITION_RIGHT) {
		ewl_container_append_child(EWL_CONTAINER(cb), cb->check);
		ewl_container_append_child(EWL_CONTAINER(cb),
					   EWL_WIDGET(b->label_object));
	} else {
		ewl_container_append_child(EWL_CONTAINER(cb),
					   EWL_WIDGET(b->label_object));
		ewl_container_append_child(EWL_CONTAINER(cb), cb->check);
	}

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_checkbutton_clicked(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	ewl_callback_call_with_event_data(cb->check, EWL_CALLBACK_CLICKED,
					  ev_data);
	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
