#include <Ewl.h>


/**
 * @param label: the label to associate with the radio button
 * @return Returns a pointer to new radio button on success, NULL on failure.
 * @brief Allocate and initialize a new radio button
 */
Ewl_Widget     *ewl_radiobutton_new(char *label)
{
	Ewl_RadioButton *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_RadioButton, 1);
	if (!b)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_radiobutton_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param rb: the radio button to be added to a chain of radio buttons
 * @param crb: a radio button already in the chain of radio buttons
 * @return Returns no value.
 * @brief Attach the button to a chain of radio buttons
 *
 * Associates @a w with the same chain as @a c, in order to
 * ensure that only one radio button of that group is checked at any time.
 */
void ewl_radiobutton_set_chain(Ewl_RadioButton *rb, Ewl_RadioButton *crb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("rb", rb);
	DCHECK_PARAM_PTR("crb", crb);

	/*
	 * If a chain doesnt exist, create one 
	 */
	if (!crb->chain) {
		crb->chain = ewd_list_new();

		ewd_list_append(crb->chain, rb);
		ewd_list_append(crb->chain, crb);
	} else {

		if (!ewd_list_goto(crb->chain, rb))
			ewd_list_append(crb->chain, rb);
	}

	rb->chain = crb->chain;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param rb: the radio button to initialize
 * @param label: the label for the initialized radio button
 * @return Returns no value.
 * @brief Initialize the radio button fields and callbacks
 *
 * Sets internal fields of the radio button to default
 * values and sets the label to the specified @a label.
 */
void ewl_radiobutton_init(Ewl_RadioButton * rb, char *label)
{
	Ewl_CheckButton *cb;
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cb = EWL_CHECKBUTTON(rb);
	w = EWL_WIDGET(rb);

	ewl_checkbutton_init(cb, label);
	ewl_widget_set_appearance(w, "radiobutton");
	ewl_widget_set_appearance(cb->check, "radio");
	ewl_callback_append(w, EWL_CALLBACK_CLICKED, ewl_radiobutton_clicked_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, ewl_radiobutton_destroy_cb,
			    NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_radiobutton_clicked_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *cb;
	Ewl_RadioButton *rb;
	int             oc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);
	rb = EWL_RADIOBUTTON(w);
	oc = ewl_checkbutton_is_checked(cb);

	if (rb->chain && !ewd_list_is_empty(rb->chain)) {
		Ewl_CheckButton *c;

		ewd_list_goto_first(rb->chain);

		while ((c = ewd_list_next(rb->chain)) != NULL) {
			ewl_checkbutton_set_checked(c, 0);
		}
	}

	ewl_checkbutton_set_checked(cb, 1);

	if (oc != ewl_checkbutton_is_checked(cb))
		ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_radiobutton_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_RadioButton *rb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	rb = EWL_RADIOBUTTON(w);

	if (!rb->chain)
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto(rb->chain, w);
	ewd_list_remove(rb->chain);

	if (ewd_list_is_empty(rb->chain)) {
		ewd_list_destroy(rb->chain);
		rb->chain = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
