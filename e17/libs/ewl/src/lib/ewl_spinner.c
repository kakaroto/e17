#include "ewl_private.h"
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

static void ewl_spinner_calc_value(Ewl_Spinner *s, double val, unsigned int call);
static int ewl_spinner_timer(void *data);

/**
 * @return Returns a new spinner widget on success, NULL on failure.
 * @brief Allocate a new spinner widget
 */
Ewl_Widget *
ewl_spinner_new(void)
{
	Ewl_Spinner *s = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Spinner, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_spinner_init(s)) {
		ewl_widget_destroy(EWL_WIDGET(s));
		s = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the spinner to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialize a spinner to default values and callbacks
 *
 * Sets the fields and callbacks of the spinner @a s their default values.
 */
int
ewl_spinner_init(Ewl_Spinner *s)
{
	Ewl_Widget *w, *vbox;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, FALSE);

	w = EWL_WIDGET(s);

	if (!ewl_box_init(EWL_BOX(w)))
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_box_orientation_set(EWL_BOX(w), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_appearance_set(w, EWL_SPINNER_TYPE);
	ewl_widget_inherit(w, EWL_SPINNER_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_HSHRINK |
				   EWL_FLAG_FILL_HFILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, 
				ewl_spinner_realize_cb, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, 
				ewl_spinner_destroy_cb, NULL);

	s->entry = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(s->entry), "0");
	ewl_container_child_append(EWL_CONTAINER(s), s->entry);
	ewl_object_fill_policy_set(EWL_OBJECT(s->entry), EWL_FLAG_FILL_HFILL |
				   EWL_FLAG_FILL_HSHRINK);
	ewl_object_alignment_set(EWL_OBJECT(s->entry), EWL_FLAG_ALIGN_LEFT);

	ewl_widget_internal_set(EWL_WIDGET(s->entry), TRUE);
	ewl_callback_del(s->entry, EWL_CALLBACK_KEY_DOWN,
			    ewl_entry_cb_key_down);
	ewl_callback_append(s->entry, EWL_CALLBACK_KEY_DOWN,
			    ewl_spinner_key_down_cb, s);
	ewl_callback_append(s->entry, EWL_CALLBACK_FOCUS_OUT,
			    ewl_spinner_focus_out_cb, s);
	ewl_callback_append(s->entry, EWL_CALLBACK_MOUSE_WHEEL,
			    ewl_spinner_wheel_cb, s);

	ewl_widget_show(s->entry);

	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(s), vbox);
	ewl_widget_appearance_set(vbox, "controls");
	ewl_widget_internal_set(vbox, TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_NONE);
	ewl_widget_show(vbox);

	s->increment = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), s->increment);
	ewl_object_alignment_set(EWL_OBJECT(s->increment),
					EWL_FLAG_ALIGN_CENTER);
	ewl_widget_appearance_set(s->increment, "increment");
	ewl_object_fill_policy_set(EWL_OBJECT(s->increment),
				   EWL_FLAG_FILL_NONE);
	ewl_widget_internal_set(s->increment, TRUE);
	ewl_callback_append(s->increment, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_spinner_increase_value_cb, w);
	ewl_callback_append(s->increment, EWL_CALLBACK_MOUSE_UP,
			    ewl_spinner_value_stop_cb, w);
	ewl_callback_append(s->increment, EWL_CALLBACK_KEY_DOWN,
			    ewl_spinner_key_down_cb, s);
	ewl_widget_show(s->increment);

	s->decrement = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), s->decrement);
	ewl_object_alignment_set(EWL_OBJECT(s->decrement),
					EWL_FLAG_ALIGN_CENTER);
	ewl_widget_appearance_set(s->decrement, "decrement");
	ewl_object_fill_policy_set(EWL_OBJECT(s->decrement),
				   EWL_FLAG_FILL_NONE);
	ewl_widget_internal_set(s->decrement, TRUE);
	ewl_callback_append(s->decrement, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_spinner_decrease_value_cb, w);
	ewl_callback_append(s->decrement, EWL_CALLBACK_MOUSE_UP,
			    ewl_spinner_value_stop_cb, w);
	ewl_callback_append(s->decrement, EWL_CALLBACK_KEY_DOWN,
			    ewl_spinner_key_down_cb, s);
	ewl_widget_show(s->decrement);

	s->min_val = INT_MIN;
	s->max_val = INT_MAX;
	s->value = 0.0;
	s->step = 0.1;
	s->digits = 2;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: the spinner widget to set the current value
 * @param value: the value to set for the spinner @a s
 * @return Returns no value.
 * @brief Set the current value of a spinner widget
 *
 * Sets the current value of the spinner @a s to @a value.
 */
void
ewl_spinner_value_set(Ewl_Spinner *s, double value)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SPINNER_TYPE);

	ewl_spinner_calc_value(s, value, FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the spinner widget to retrieve the value
 * @return Returns the current value in @a s on success, 0.0 on failure.
 * @brief Get the current value of a spinner widget
 */
double
ewl_spinner_value_get(Ewl_Spinner *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.00);
	DCHECK_TYPE_RET("s", s, EWL_SPINNER_TYPE, 0.00);

	DRETURN_FLOAT(s->value, DLEVEL_STABLE);
}

/**
 * @param s: the widget to change the number of digits displayed
 * @param digits: the number of digits to display for the spinner @a s
 * @return Returns no value.
 * @brief Set the number of digits displayed by a spinner
 *
 * Changes the digits displayed by @a s to @a digits.
 */
void
ewl_spinner_digits_set(Ewl_Spinner *s, unsigned char digits)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SPINNER_TYPE);

	s->digits = digits;
	ewl_spinner_calc_value(s, s->value, FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the spinner to retrieve minimum value
 * @brief Retrieves the minimum value for the spinner.
 * @return Returns the currently set minimum value for the specified spinner.
 */
double
ewl_spinner_min_val_get(Ewl_Spinner *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.0);
	DCHECK_TYPE_RET("s", s, EWL_SPINNER_TYPE, 0.0);

	DRETURN_FLOAT(s->min_val, DLEVEL_STABLE);
}

/**
 * @param s: the spinner to change the minimum possible value
 * @param val: the new minimum possible value for @a s
 * @return Returns no value.
 * @brief Set the minimum value possible for a spinner
 *
 * Sets the smallest value that @a s can obtain to @a val.
 */
void
ewl_spinner_min_val_set(Ewl_Spinner *s, double val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SPINNER_TYPE);

	s->min_val = val;
	ewl_spinner_calc_value(s, s->value, FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the spinner to retrieve maximum value
 * @brief Retrieves the maximum value for the spinner.
 * @return Returns the currently set maximum value for the specified spinner.
 */
double
ewl_spinner_max_val_get(Ewl_Spinner *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 100.0);
	DCHECK_TYPE_RET("s", s, EWL_SPINNER_TYPE, 100.0);

	DRETURN_FLOAT(s->max_val, DLEVEL_STABLE);
}

/**
 * @param s: the spinner to change the maximum possible value
 * @param val: the new maximum possible value for @a s
 * @return Returns no value.
 * @brief Set the maximum value possible for a spinner
 *
 * Sets the largest value that @a s can obtain to @a val.
 */
void
ewl_spinner_max_val_set(Ewl_Spinner *s, double val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SPINNER_TYPE);

	s->max_val = val;
	ewl_spinner_calc_value(s, s->value, FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the spinner to change increment step
 * @param step: the new increment between clicks of the spinner @a s
 * @brief Returns no value.
 * @brief Set the increment between each click of the spinner
 *
 * Changes the increment that @a s changes by with each click of it's spinner
 * buttons to @a step.
 */
void
ewl_spinner_step_set(Ewl_Spinner *s, double step)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SPINNER_TYPE);

	s->step = step;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The realize callback
 */
void
ewl_spinner_realize_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
				void *user_data __UNUSED__)
{
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	s = EWL_SPINNER(w);
	ewl_spinner_calc_value(s, s->value, FALSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Key_Down data
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The key down callback
 */
void
ewl_spinner_key_down_cb(Ewl_Widget *w, void *ev_data,
				void *user_data)
{
	Ewl_Entry *e;
	Ewl_Spinner *s;
	Ewl_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	e = EWL_ENTRY(w);
	s = EWL_SPINNER(user_data);

	ev = ev_data;

	if (!strcmp(ev->keyname, "Up"))
		ewl_spinner_increase_value_cb(w, NULL, s);

	else if (!strcmp(ev->keyname, "Down"))
		ewl_spinner_decrease_value_cb(w, NULL, s);

	else if (!strcmp(ev->keyname, "Left"))
		ewl_entry_cursor_move_left(e);

	else if (!strcmp(ev->keyname, "Right"))
		ewl_entry_cursor_move_right(e);
/*
	else if (!strcmp(ev->keyname, "Home"))
		ewl_entry_cursor_home_move(e);

	else if (!strcmp(ev->keyname, "End"))
		ewl_entry_cursor_end_move(e);
*/
	else if (!strcmp(ev->keyname, "BackSpace"))
		ewl_entry_delete_left(e);

	else if (!strcmp(ev->keyname, "Delete"))
		ewl_entry_delete_right(e);

	else if (ev->keyname && (isdigit(ev->keyname[0]) ||
				 ev->keyname[0] == '.' ||
				 ev->keyname[0] == '-'))
	{
		ewl_entry_selection_clear(e);
		ewl_text_text_insert(EWL_TEXT(e), ev->keyname, 
				ewl_text_cursor_position_get(EWL_TEXT(e)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The focus out callback
 */
void
ewl_spinner_focus_out_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
				void *user_data)
{
	Ewl_Spinner *s;
	char *str;
	float val;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	s = EWL_SPINNER(user_data);
	str = ewl_text_text_get(EWL_TEXT(s->entry));

	if (str && strlen(str)) {
		val = atof(str);

		ewl_spinner_calc_value(s, (double) (val), TRUE);
	} else if (str)
		FREE(str);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Wheel data
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The mouse wheel callback
 */
void
ewl_spinner_wheel_cb(Ewl_Widget *w __UNUSED__, void *ev_data,
			void *user_data)
{
	Ewl_Spinner *s;
	Ewl_Event_Mouse_Wheel *wheel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	s = EWL_SPINNER(user_data);
	wheel = (Ewl_Event_Mouse_Wheel *)ev_data;

	ewl_spinner_calc_value(s, s->value - (wheel->z * s->step), TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_spinner_calc_value(Ewl_Spinner *s, double value, unsigned int call)
{
	char format[64];
	char str[64];
	double oval;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SPINNER_TYPE);

	oval = s->value;

	if (value < s->min_val)
		s->value = s->min_val;
	else if (value > s->max_val)
		s->value = s->max_val;
	else
		s->value = value;

	snprintf(format, 64, "%%.%df", s->digits);
	snprintf(str, 64, format, s->value);

	ewl_text_text_set(EWL_TEXT(s->entry), str);

	if ((call == TRUE) && (oval != s->value)) {
		oval = s->value;
		ewl_callback_call_with_event_data(EWL_WIDGET(s),
				EWL_CALLBACK_VALUE_CHANGED, &oval);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The event data
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The increase value callback
 */
void
ewl_spinner_increase_value_cb(Ewl_Widget *w, void *ev_data,
					void *user_data)
{
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	s = EWL_SPINNER(user_data);
	ewl_spinner_focus_out_cb(w, NULL, s);

	ewl_spinner_calc_value(s, s->value + s->step, TRUE);

	if (ev_data) {
		s->direction = 1;
		s->start_time = ecore_time_get();
		s->timer = ecore_timer_add(0.02, ewl_spinner_timer, s);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The value stop callback
 */
void
ewl_spinner_value_stop_cb(Ewl_Widget *w __UNUSED__,
			void *ev_data __UNUSED__, void *user_data)
{
	double oval;
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	s = user_data;
	if (s->timer) {
		ecore_timer_del(s->timer);

		s->timer = NULL;
		s->direction = 0;
		s->start_time = 0;
	}

	oval = s->value;
	ewl_callback_call_with_event_data(EWL_WIDGET(s),
					  EWL_CALLBACK_VALUE_CHANGED, &oval);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The event
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The decrease value callback
 */
void
ewl_spinner_decrease_value_cb(Ewl_Widget *w, void *ev_data,
					void *user_data)
{
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	s = EWL_SPINNER(user_data);
	ewl_spinner_focus_out_cb(w, NULL, s);

	ewl_spinner_calc_value(s, s->value - s->step, TRUE);
	if (ev_data) {
		s->direction = -1;
		s->start_time = ecore_time_get();
		s->timer = ecore_timer_add(0.02, ewl_spinner_timer, s);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_spinner_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	s = EWL_SPINNER(w);
	if (s->timer) {
		ecore_timer_del(s->timer);
		s->timer = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_spinner_timer(void *data)
{
	Ewl_Spinner *s;
	double dt;
	double value, range, tmpt;
	int velocity, delay;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, FALSE);

	s = EWL_SPINNER(data);

	dt = ecore_time_get() - s->start_time;
	value = ewl_spinner_value_get(s);
	range = s->max_val - s->min_val;

	/*
	 * Check the theme for a velocity setting and bring it within normal
	 * useable bounds.
	 */
	velocity = ewl_theme_data_int_get(EWL_WIDGET(s), "velocity");
	if (velocity < 1)
		velocity = 1;
	else if (velocity > 10)
		velocity = 10;

	/*
	 * Check the theme for a delay setting and bring it within normal
	 * useable bounds.
	 */
	delay = ewl_theme_data_int_get(EWL_WIDGET(s), "delay");
	if (delay < 1)
		delay = 1;
	else if (delay > 10)
		delay = 10;

	/*
	 * Move the value of the spinner based on the direction of it's motion
	 * and the velocity setting.
	 */
	tmpt = (dt > (double)delay ? dt - (double)delay : 0.0);
	tmpt = ((1 - exp(-tmpt)) * ((double)(velocity) / 100.0)) * range;
	value += (double)(s->direction) * ((1 - exp(-dt)) + tmpt);

	ewl_spinner_value_set(s, value);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

