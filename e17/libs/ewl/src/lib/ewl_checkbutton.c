#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @param label: the label to display with the checkbutton, NULL for no label
 * @return Returns the newly allocated checkbutton on success, NULL on failure.
 * @brief Allocate and initialize a new check button
 */
Ewl_Widget     *ewl_checkbutton_new(char *label)
{
	Ewl_CheckButton *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_CheckButton, 1);
	if (!b)
		return NULL;

	ewl_checkbutton_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param cb: the check button to initialize
 * @param label: the label to give the initialized check button
 * @return Returns no value.
 * @brief Initialize the members and callbacks of a check button
 *
 * The internal structures and callbacks of the checkbutton are initialized to
 * default values.
 */
void ewl_checkbutton_init(Ewl_CheckButton * cb, char *label)
{
	Ewl_Button     *b;
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = EWL_BUTTON(cb);
	w = EWL_WIDGET(cb);

	ewl_button_init(b, label);
	ewl_widget_appearance_set(w, "checkbutton");
	ewl_widget_inherit(w, "checkbutton");

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
	ewl_callback_append(w, EWL_CALLBACK_CLICKED,
				ewl_checkbutton_clicked_cb, NULL);

	cb->label_position = EWL_POSITION_RIGHT;

	/*
	 * Add the check box first.
	 */
	cb->check = ewl_check_new();
	ewl_widget_internal_set(cb->check, TRUE);
	ewl_callback_del(cb->check, EWL_CALLBACK_CLICKED, ewl_check_clicked_cb);
	ewl_container_child_prepend(EWL_CONTAINER(cb), cb->check);
	ewl_widget_show(cb->check);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cb: the widget to change the label positioning
 * @param p: the new position of the label
 * @return Returns no value.
 * @brief Set the check buttons label position
 *
 * Changes the position of the label associated with the check button.
 */
void ewl_checkbutton_label_position_set(Ewl_CheckButton * cb, Ewl_Position p)
{
	Ewl_Button     *b;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cb", cb);

	b = EWL_BUTTON(cb);

	if (cb->label_position == p)
		DRETURN(DLEVEL_STABLE);

	cb->label_position = p;
	ewl_container_child_remove(EWL_CONTAINER(cb),
			EWL_WIDGET(b->label_object));
	ewl_container_child_remove(EWL_CONTAINER(cb), cb->check);

	/*
	 * Add the label and check back into the checkbutton with the correct
	 * order.
	 */
	if (p == EWL_POSITION_RIGHT) {
		ewl_container_child_append(EWL_CONTAINER(cb), cb->check);
		ewl_container_child_append(EWL_CONTAINER(cb),
					   EWL_WIDGET(b->label_object));
	} else {
		ewl_container_child_append(EWL_CONTAINER(cb),
					   EWL_WIDGET(b->label_object));
		ewl_container_child_append(EWL_CONTAINER(cb), cb->check);
	}

	ewl_widget_configure(EWL_WIDGET(cb));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_checkbutton_clicked_cb(Ewl_Widget * w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	if (ewl_check_is_checked(EWL_CHECK(cb->check)))
		ewl_check_checked_set(EWL_CHECK(cb->check), FALSE);
	else
		ewl_check_checked_set(EWL_CHECK(cb->check), TRUE);
	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


