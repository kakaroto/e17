#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/**
 * @param label: the string to use as a label for the button
 * @return Returns NULL on failure, a pointer to a new button on success
 * @brief Allocate and initialize a new button
 */
Ewl_Widget     *ewl_button_new(char *label)
{
	Ewl_Button     *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Button, 1);
	if (!b)
		return NULL;

	ewl_button_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param b: the button to initialize
 * @param label: set the label of the button @b to @a label
 * @return Returns no value.
 * @brief Initialize a button to starting values
 *
 * Initializes a button to default values and callbacks.
 */
int ewl_button_init(Ewl_Button * b, char *label)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, 0);

	w = EWL_WIDGET(b);

	if (!ewl_box_init(EWL_BOX(b), EWL_ORIENTATION_HORIZONTAL))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, "button");
	ewl_widget_inherit(w, "button");

	/*
	 * Create and setup the label for the button if it's desired.
	 */
	if (label)
		b->label_object = ewl_label_new(label);

	if (b->label_object) {
		ewl_object_alignment_set(EWL_OBJECT(b->label_object),
					 EWL_FLAG_ALIGN_CENTER);
		ewl_container_child_append(EWL_CONTAINER(b), b->label_object);
		ewl_widget_show(b->label_object);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param b: the buttons whose label will be changed
 * @param l: the new label for the button
 * @return Returns no value.
 * @brief Change the label of the specified button
 */
void ewl_button_label_set(Ewl_Button * b, char *l)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	if (!l) {
		ewl_widget_destroy(b->label_object);
		b->label_object = NULL;
	}
	else if (!b->label_object) {
		b->label_object = ewl_label_new(l);
		ewl_widget_show(b->label_object);
		ewl_container_child_append(EWL_CONTAINER(b), b->label_object);
	}
	else
		ewl_label_text_set(EWL_LABEL(b->label_object), l);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param b: the buttons whose label will be returned
 * @return A newly allocated copy of the label on the button.
 * @brief Retrieve the label of the specified button
 */
char *ewl_button_label_get(Ewl_Button *b)
{
	char *val = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (b->label_object)
		val = ewl_label_text_get(EWL_LABEL(b->label_object));

	DRETURN_PTR(val, DLEVEL_STABLE);
}
