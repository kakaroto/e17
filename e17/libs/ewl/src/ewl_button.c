
#include <Ewl.h>

/**
 * ewl_button_new - allocate and initialize a new button
 * @label: the string to use as a label for the button
 *
 * Returns NULL on failure, a pointer to a new button on success
 */
Ewl_Widget     *ewl_button_new(char *label)
{
	Ewl_Button     *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Button, 1);
	if (!b)
		return NULL;

	ZERO(b, Ewl_Button, 1);
	ewl_button_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * ewl_button_init - initialize a button to starting values
 * @b: the button to initialize
 * @label: set the label of the button @b to @label
 *
 * Returns no value. Initializes a button to default values and callbacks.
 */
void ewl_button_init(Ewl_Button * b, char *label)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = EWL_WIDGET(b);

	ewl_box_init(EWL_BOX(b), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_set_appearance(w, "button");

	/*
	 * Override the default recursive setting on containers. This prevents
	 * the coordinate->object mapping from searching below the button
	 * class.
	 */
	w->recursive = FALSE;

	/*
	 * Create and setup the label for the button if it's desired.
	 */
	if (label) {
		b->label_object = ewl_text_new(label);
		ewl_object_set_alignment(EWL_OBJECT(b->label_object),
					 EWL_ALIGNMENT_CENTER);
		ewl_container_append_child(EWL_CONTAINER(b), b->label_object);
		ewl_widget_show(b->label_object);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_button_set_label - change the label of the specified button
 * @b: the buttons whose label will be changed
 * @l: the new label for the button
 *
 * Returns no value.
 */
void ewl_button_set_label(Ewl_Button * b, char *l)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	if (!l) {
		ewl_widget_destroy(b->label_object);
		b->label_object = NULL;
	} else
		ewl_text_set_text(EWL_TEXT(b->label_object), l);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
