
#include <Ewl.h>


void ewl_checkbutton_init(Ewl_CheckButton * cb, char *label);

void __ewl_checkbutton_configure(Ewl_Widget * w, void *ev_data,
				 void *user_data);
void __ewl_checkbutton_clicked(Ewl_Widget * w, void *ev_data,
			       void *user_data);
void __ewl_checkbutton_update_check(Ewl_Widget * w);

void __ewl_box_configure(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_checkbutton_theme_update(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void ewl_button_init(Ewl_Button * b, char *l);

/**
 * ewl_checkbutton_new - allocate and initialize a new check button
 * @label: the label to display with the checkbutton, NULL for no label
 *
 * Returns the newly allocated checkbutton on success, NULL on failure.
 */
Ewl_Widget *
ewl_checkbutton_new(char *label)
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
 * ewl_checkbutton_set_checked - change the checked status of the check button
 * @w: the check button to change the status
 * @c: the new status of the check button
 *
 * Returns no value. Changes the checked status of the check button and
 * updates it's appearance to reflect the change.
 */
void
ewl_checkbutton_set_checked(Ewl_Widget * w, int c)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	if (c)
		cb->checked = 1;
	else
		cb->checked = 0;

	__ewl_checkbutton_update_check(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_checkbutton_is_checked - determine the check state of the check button
 * @w: the check button to examine for it's checked state
 *
 * Returns TRUE if the button is checked, FALSE if not.
 */
int
ewl_checkbutton_is_checked(Ewl_Widget * w)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);

	cb = EWL_CHECKBUTTON(w);

	DRETURN_INT(cb->checked, DLEVEL_STABLE);
}

/**
 * ewl_checkbutton_set_label_position - set the check buttons label position
 * @w: the widget to change the label positioning
 * @p: the new position of the label
 *
 * Returns no value. Changes the position of the label associated with the
 * check button.
 */
void
ewl_checkbutton_set_label_position(Ewl_Widget * w, Ewl_Position p)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	cb->label_position = p;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_checkbutton_init - initialize the members and callbacks of a check button
 * @cb: the check button to initialize
 * @label: the label to give the initialized check button
 *
 * Returns no vlalue.The internal structures and callbacks of the checkbutton
 * are initialized ot default values.
 */
void
ewl_checkbutton_init(Ewl_CheckButton * cb, char *label)
{
	Ewl_Button *b;
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = EWL_BUTTON(cb);
	w = EWL_WIDGET(cb);

	ewl_button_init(b, label);
	ewl_widget_set_appearance(w, "/appearance/button/check");

	ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, __ewl_box_configure);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_checkbutton_theme_update, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_checkbutton_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CLICKED,
			    __ewl_checkbutton_clicked, NULL);

	cb->label_position = EWL_POSITION_RIGHT;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_checkbutton_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Button *b;
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);
	cb = EWL_CHECKBUTTON(w);

	if (b->label_object)
	  {
		  if (MINIMUM_H(b->label_object) > CURRENT_H(w))
		    {
			    REQUEST_Y(b->label_object) = CURRENT_Y(w) + 17;
			    MINIMUM_H(w) = MINIMUM_H(b->label_object);
			    MAXIMUM_H(w) = MINIMUM_H(b->label_object);
		    }
		  else
		    {
			    REQUEST_Y(b->label_object) = CURRENT_Y(w);
			    REQUEST_Y(b->label_object) +=
				    (CURRENT_H(w) / 2) -
				    (CURRENT_H(b->label_object) / 2);
			    MINIMUM_H(w) = 17;
			    MAXIMUM_H(w) = 17;
		    }

		  MINIMUM_W(w) = 17 + CURRENT_W(b->label_object);
		  MAXIMUM_W(w) = 17 + CURRENT_W(b->label_object);

		  if (cb->label_position == EWL_POSITION_LEFT)
			  REQUEST_X(b->label_object) = REQUEST_X(w);
		  else
			  REQUEST_X(b->label_object) = CURRENT_X(w) + 17;

		  ewl_widget_configure(b->label_object);

		  if (w->ebits_object)
		    {
			    if (cb->label_position == EWL_POSITION_LEFT)
				    ebits_move(w->ebits_object,
					       REQUEST_X(w) +
					       CURRENT_W(b->label_object),
					       REQUEST_Y(w));
			    else
				    ebits_move(w->ebits_object, REQUEST_X(w),
					       REQUEST_Y(w));

		    }
	  }
	else
	  {
		  MAXIMUM_W(w) = 17;
		  MAXIMUM_H(w) = 17;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_checkbutton_clicked(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *cb;
	int oc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);
	oc = cb->checked;

	cb->checked ^= 1;

	__ewl_checkbutton_update_check(w);

	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_checkbutton_update_check(Ewl_Widget * w)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	if (w->ebits_object)
	  {
		  if (cb->checked)
			  ebits_set_named_bit_state(w->ebits_object, "Check",
						    "clicked");
		  else
			  ebits_set_named_bit_state(w->ebits_object, "Check",
						    "normal");
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_checkbutton_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	__ewl_checkbutton_update_check(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
