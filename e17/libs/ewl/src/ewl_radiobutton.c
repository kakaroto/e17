
#include <Ewl.h>


void            ewl_radiobutton_init(Ewl_RadioButton * cb, char *label);

void            __ewl_radiobutton_clicked(Ewl_Widget * w, void *ev_data,
					  void *user_data);

void            __ewl_checkbutton_clicked(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void            __ewl_checkbutton_mouse_down(Ewl_Widget * w, void *ev_data,
					     void *user_data);


/**
 * ewl_radiobutton_new - allocate and initialize a new radio button
 * @label: the label to associate with the radio button
 *
 * Returns a pointer to the new radio button on success, NULL on failure.
 */
Ewl_Widget     *ewl_radiobutton_new(char *label)
{
	Ewl_RadioButton *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_RadioButton, 1);
	if (!b)
		return NULL;

	ZERO(b, Ewl_RadioButton, 1);
	ewl_radiobutton_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * ewl_radiobutton_set_chain - attach the button to a chain of radio buttons
 * @w: the radio button to be added to a chain of radio buttons
 * @c: a radio button already in the chain of radio buttons
 *
 * Returns no value. Associates @w with the same chain as @c, in order to
 * ensure that only one radio button of that group is checked at any time.
 */
void ewl_radiobutton_set_chain(Ewl_Widget * w, Ewl_Widget * c)
{
	Ewl_RadioButton *rb, *crb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);

	rb = EWL_RADIOBUTTON(w);
	crb = EWL_RADIOBUTTON(c);

	/*
	 * If a chain doesnt exist, create one 
	 */
	if (!crb->chain) {
		crb->chain = ewd_list_new();

		rb->chain = crb->chain;

		ewd_list_append(crb->chain, w);
		ewd_list_append(crb->chain, c);
	} else {
		rb->chain = crb->chain;

		if (!ewd_list_goto(crb->chain, w))
			ewd_list_append(crb->chain, w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_radiobutton_init - initialize the radio button fields and callbacks
 * @rb: the radio button to initialize
 * @label: the label for the initialized radio button
 *
 * Returns no value. Sets internal fields of the radio button to default
 * values and sets the label to the specified @label.
 */
void ewl_radiobutton_init(Ewl_RadioButton * rb, char *label)
{
	Ewl_CheckButton *cb;
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cb = EWL_CHECKBUTTON(rb);
	w = EWL_WIDGET(rb);

	ewl_checkbutton_init(cb, label);
	ewl_widget_set_appearance(w, "/button/radio");
	ewl_widget_set_appearance(cb->check, "/radio");

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __ewl_checkbutton_clicked);
	ewl_callback_append(w, EWL_CALLBACK_CLICKED,
			    __ewl_radiobutton_clicked, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_radiobutton_clicked(Ewl_Widget * w, void *ev_data, void *user_data)
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
