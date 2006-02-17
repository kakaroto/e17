#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns the newly allocated checkbutton on success, NULL on failure.
 * @brief Allocate and initialize a new check button
 */
Ewl_Widget *
ewl_checkbutton_new(void)
{
	Ewl_Checkbutton *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Checkbutton, 1);
	if (!b)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_checkbutton_init(b)) {
		ewl_widget_destroy(EWL_WIDGET(b));
		b = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param cb: the check button to initialize
 * @return Returns no value.
 * @brief Initialize the members and callbacks of a check button
 *
 * The internal structures and callbacks of the checkbutton are initialized to
 * default values.
 */
int
ewl_checkbutton_init(Ewl_Checkbutton *cb)
{
	Ewl_Button *b;
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cb", cb, FALSE);

	b = EWL_BUTTON(cb);
	w = EWL_WIDGET(cb);

	if (!ewl_button_init(b))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_CHECKBUTTON_TYPE);
	ewl_widget_inherit(w, EWL_CHECKBUTTON_TYPE);

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

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param cb: The checkbutton to set the checked value of
 * @param c: The checked value to set
 * @return Returns no value.
 * @brief Update the checked state of the check button.
 */
void ewl_checkbutton_checked_set(Ewl_Checkbutton *cb, int c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cb", cb);
	DCHECK_TYPE("cb", cb, EWL_CHECKBUTTON_TYPE);

	ewl_check_checked_set(EWL_CHECK(EWL_CHECKBUTTON(cb)->check), c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief Retrieve the checked state of a check button.
 */
int ewl_checkbutton_is_checked(Ewl_Checkbutton *cb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cb", cb, FALSE);
	DCHECK_TYPE_RET("cb", cb, EWL_CHECKBUTTON_TYPE, FALSE);

	DRETURN_INT(ewl_check_is_checked(EWL_CHECK(EWL_CHECKBUTTON(cb)->check)),
			DLEVEL_STABLE);
}

/**
 * @param cb: the widget to change the label positioning
 * @param p: the new position of the label
 * @return Returns no value.
 * @brief Set the check buttons label position
 *
 * Changes the position of the label associated with the check button.
 */
void
ewl_checkbutton_label_position_set(Ewl_Checkbutton *cb, Ewl_Position p)
{
	Ewl_Button *b;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cb", cb);
	DCHECK_TYPE("cb", cb, EWL_CHECKBUTTON_TYPE);

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

void
ewl_checkbutton_clicked_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	Ewl_Checkbutton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cb = EWL_CHECKBUTTON(w);

	if (ewl_check_is_checked(EWL_CHECK(cb->check)))
		ewl_check_checked_set(EWL_CHECK(cb->check), FALSE);
	else
		ewl_check_checked_set(EWL_CHECK(cb->check), TRUE);
	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


