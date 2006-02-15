#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a pointer to new radio button on success, NULL on failure.
 * @brief Allocate and initialize a new radio button
 */
Ewl_Widget *
ewl_radiobutton_new(void)
{
	Ewl_Radiobutton *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Radiobutton, 1);
	if (!b)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_radiobutton_init(b)) {
		ewl_widget_destroy(EWL_WIDGET(b));
		b = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
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
int
ewl_radiobutton_init(Ewl_Radiobutton *rb)
{
	Ewl_Checkbutton *cb;
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("rb", rb, FALSE);

	cb = EWL_CHECKBUTTON(rb);
	w = EWL_WIDGET(rb);

	if (!ewl_checkbutton_init(cb))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_RADIOBUTTON_TYPE);
	ewl_widget_inherit(w, EWL_RADIOBUTTON_TYPE);
	ewl_widget_appearance_set(cb->check, "radio");
	ewl_callback_append(w, EWL_CALLBACK_CLICKED, ewl_radiobutton_clicked_cb,
			    NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_radiobutton_destroy_cb,
			    NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
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
void
ewl_radiobutton_chain_set(Ewl_Radiobutton *rb, Ewl_Radiobutton *crb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("rb", rb);
	DCHECK_PARAM_PTR("crb", crb);
	DCHECK_TYPE("rb", rb, EWL_RADIOBUTTON_TYPE);
	DCHECK_TYPE("crb", crb, EWL_RADIOBUTTON_TYPE);

	/*
	 * If a chain doesnt exist, create one 
	 */
	if (!crb->chain) {
		crb->chain = ecore_list_new();

		ecore_list_append(crb->chain, rb);
		ecore_list_append(crb->chain, crb);
	} else {

		if (!ecore_list_goto(crb->chain, rb))
			ecore_list_append(crb->chain, rb);
	}

	rb->chain = crb->chain;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_radiobutton_clicked_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Checkbutton *cb;
	Ewl_Radiobutton *rb;
	int oc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cb = EWL_CHECKBUTTON(w);
	rb = EWL_RADIOBUTTON(w);
	oc = ewl_checkbutton_is_checked(cb);

	if (rb->chain && !ecore_list_is_empty(rb->chain)) {
		Ewl_Checkbutton *c;

		ecore_list_goto_first(rb->chain);
		while ((c = ecore_list_next(rb->chain)) != NULL) {
			ewl_checkbutton_checked_set(c, 0);
		}
	}
	ewl_checkbutton_checked_set(cb, 1);

	if (oc != ewl_checkbutton_is_checked(cb))
		ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_radiobutton_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Radiobutton *rb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	rb = EWL_RADIOBUTTON(w);

	if (!rb->chain)
		DRETURN(DLEVEL_STABLE);

	ecore_list_goto(rb->chain, w);
	ecore_list_remove(rb->chain);

	if (ecore_list_is_empty(rb->chain)) {
		ecore_list_destroy(rb->chain);
		rb->chain = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

