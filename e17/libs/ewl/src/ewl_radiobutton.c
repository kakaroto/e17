
#include <Ewl.h>


void            ewl_radiobutton_init(Ewl_RadioButton * cb, char *label);

void            __ewl_radiobutton_realize(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void            __ewl_checkbutton_clicked(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void            __ewl_radiobutton_mouse_down(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            __ewl_radiobutton_theme_update(Ewl_Widget * w, void *ev_data,
					       void *user_data);
void            __ewl_radiobutton_update_check(Ewl_Widget * w);

void            __ewl_checkbutton_mouse_down(Ewl_Widget * w, void *ev_data,
					     void *user_data);


void            ewl_checkbutton_init(Ewl_CheckButton * b, char *l);


/**
 * ewl_radiobutton_new - allocate and initialize a new radio button
 * @label: the label to associate with the radio button
 *
 * Returns a pointer to the new radio button on success, NULL on failure.
 */
Ewl_Widget     *
ewl_radiobutton_new(char *label)
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
void
ewl_radiobutton_set_chain(Ewl_Widget * w, Ewl_Widget * c)
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
 * ewl_radiobutton_set_checked - change the checked status of the radio button
 * @w: the radio button to change the status
 * @c: the new status of the radio button
 *
 * Returns no value. Changes the checked status of the radio button and
 * updates it's appearance to reflect the change. Also unchecks any previously
 * checked radio button in it's group.
 */
void
ewl_radiobutton_set_checked(Ewl_Widget * w, int c)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	if (c)
		cb->checked = 1;
	else
		cb->checked = 0;

	__ewl_radiobutton_mouse_down(w, NULL, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_radiobutton_is_checked - determine the check state of the radio button
 * @w: the radio button to examine for it's checked state
 *
 * Returns TRUE if the button is checked, FALSE if not.
 */
int
ewl_radiobutton_is_checked(Ewl_Widget * w)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);

	cb = EWL_CHECKBUTTON(w);

	DRETURN_INT(cb->checked, DLEVEL_STABLE);
}

void
__ewl_radiobutton_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	__ewl_radiobutton_update_check(w);

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
void
ewl_radiobutton_init(Ewl_RadioButton * rb, char *label)
{
	Ewl_CheckButton *cb;
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cb = EWL_CHECKBUTTON(rb);
	w = EWL_WIDGET(rb);

	ewl_checkbutton_init(cb, label);
	ewl_widget_set_appearance(w, "/appearance/button/radio");

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __ewl_checkbutton_clicked);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_radiobutton_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_radiobutton_realize, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_radiobutton_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *cb;
	Ewl_RadioButton *rb;
	int             oc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);
	rb = EWL_RADIOBUTTON(w);
	oc = cb->checked;

	if (rb->chain && !ewd_list_is_empty(rb->chain)) {
		Ewl_CheckButton *c;

		ewd_list_goto_first(rb->chain);

		while ((c = ewd_list_next(rb->chain)) != NULL) {
			c->checked = 0;

			__ewl_radiobutton_update_check(EWL_WIDGET(c));
		}
	}

	cb->checked = 1;

	__ewl_radiobutton_update_check(w);

	if (oc != cb->checked)
		ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_radiobutton_update_check(Ewl_Widget * w)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	if (w->ebits_object) {
		if (cb->checked)
			ebits_set_named_bit_state(w->ebits_object, "Check",
						  "clicked");
		else
			ebits_set_named_bit_state(w->ebits_object, "Check",
						  "normal");
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
