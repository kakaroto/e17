#include <Ewl.h>

static void ewl_spinner_calc_value(Ewl_Spinner *s, double val);

/**
 * @return Returns a new spinner widget on success, NULL on failure.
 * @brief Allocate a new spinner widget
 */
Ewl_Widget     *ewl_spinner_new()
{
	Ewl_Spinner    *s = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	s = NEW(Ewl_Spinner, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_spinner_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the spinner to initialize
 * @return Returns no value.
 * @brief Initialize a spinner to default values and callbacks
 *
 * Sets the fields and callbacks of the spinner @a s their default values.
 */
int ewl_spinner_init(Ewl_Spinner * s)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, FALSE);

	w = EWL_WIDGET(s);

	if (!ewl_container_init(EWL_CONTAINER(w), "spinner"))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_container_show_notify(EWL_CONTAINER(w), ewl_spinner_child_show_cb);
	ewl_container_resize_notify(EWL_CONTAINER(w),
				    ewl_spinner_child_resize_cb);

	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_HFILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, ewl_spinner_realize_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_spinner_configure_cb,
			    NULL);

	s->entry = ewl_entry_new("0");
	ewl_container_append_child(EWL_CONTAINER(s), s->entry);
	ewl_widget_show(s->entry);

	s->button_increase = ewl_button_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(s), s->button_increase);
	ewl_widget_set_appearance(s->button_increase, "button_increment");
	ewl_object_set_fill_policy(EWL_OBJECT(s->button_increase),
			EWL_FLAG_FILL_NONE);
	ewl_widget_show(s->button_increase);

	s->button_decrease = ewl_button_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(s), s->button_decrease);
	ewl_widget_set_appearance(s->button_decrease, "button_decrement");
	ewl_object_set_fill_policy(EWL_OBJECT(s->button_decrease),
			EWL_FLAG_FILL_NONE);
	ewl_widget_show(s->button_decrease);

	s->min_val = -50.0;
	s->max_val = 50.0;
	s->value = 0.0;
	s->step = 0.1;
	s->digits = 2;

	ewl_callback_del(s->entry, EWL_CALLBACK_KEY_DOWN,
			 ewl_entry_key_down_cb);
	ewl_callback_append(s->entry, EWL_CALLBACK_KEY_DOWN,
			    ewl_spinner_key_down_cb, NULL);
	ewl_callback_append(s->entry, EWL_CALLBACK_DESELECT,
			    ewl_spinner_deselect_cb, NULL);
	ewl_callback_append(s->button_increase, EWL_CALLBACK_CLICKED,
			    ewl_spinner_increase_value_cb, w);
	ewl_callback_append(s->button_increase, EWL_CALLBACK_KEY_DOWN,
			    ewl_spinner_key_down_cb, NULL);
	ewl_callback_append(s->button_decrease, EWL_CALLBACK_CLICKED,
			    ewl_spinner_decrease_value_cb, w);
	ewl_callback_append(s->button_decrease, EWL_CALLBACK_KEY_DOWN,
			    ewl_spinner_key_down_cb, NULL);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param s: the spinner widget to set the current value
 * @param value: the value to set for the spinner @a s
 * @return Returns no value.
 * @brief Set the current value of a spinner widget
 *
 * Sets the current value of the spinner @a s to @a value.
 */
void ewl_spinner_set_value(Ewl_Spinner * s, double value)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	ewl_spinner_calc_value(s, value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the spinner widget to retrieve the value
 * @return Returns the current value in @a s on success, 0.0 on failure.
 * @brief Get the current value of a spinner widget
 */
double ewl_spinner_get_value(Ewl_Spinner * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.00);

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
void ewl_spinner_set_digits(Ewl_Spinner * s, int digits)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->digits = digits;
	ewl_spinner_calc_value(s, s->value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the spinner to change the minimum possible value
 * @param val: the new minimum possible value for @a s
 * @return Returns no value.
 * @brief Set the minimum value possible for a spinner
 *
 * Sets the smallest value that @a s can obtain to @a val.
 */
void ewl_spinner_set_min_val(Ewl_Spinner * s, double val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->min_val = val;
	ewl_spinner_calc_value(s, s->value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the spinner to change the maximum possible value
 * @param val: the new maximum possible value for @a s
 * @return Returns no value.
 * @brief Set the maximum value possible for a spinner
 *
 * Sets the largest value that @a s can obtain to @a val.
 */
void ewl_spinner_set_max_val(Ewl_Spinner * s, double val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->max_val = val;
	ewl_spinner_calc_value(s, s->value);

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
void ewl_spinner_set_step(Ewl_Spinner * s, double step)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	s->step = step;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spinner_child_show_cb(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Spinner *s = EWL_SPINNER(c);

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (w == s->entry) {
		ewl_object_set_preferred_w(EWL_OBJECT(c), PREFERRED_W(c) +
				ewl_object_get_preferred_w(EWL_OBJECT(w)));
	}
	else if (s->button_increase && s->button_decrease) {
		ewl_object_set_preferred_w(EWL_OBJECT(c), PREFERRED_W(c) +
				MAX(ewl_object_get_preferred_w(EWL_OBJECT(
							s->button_increase)),
					ewl_object_get_preferred_h(EWL_OBJECT(
							s->button_decrease))));
	}

	if (s->entry && s->button_increase && s->button_decrease) {
		int size;

		size = ewl_object_get_preferred_w(EWL_OBJECT(s->button_increase))
			+ ewl_object_get_preferred_h(EWL_OBJECT(s->button_decrease));
		ewl_object_set_minimum_h(EWL_OBJECT(c),
			MAX(ewl_object_get_preferred_h(EWL_OBJECT(s->entry)),
			size));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spinner_child_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
			    Ewl_Orientation o)
{
	Ewl_Spinner *s = EWL_SPINNER(c);

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (o == EWL_ORIENTATION_HORIZONTAL) {
		if (w == s->entry)
			ewl_object_set_preferred_w(EWL_OBJECT(c),
					PREFERRED_W(c) + size);
		else if (s->button_increase && s->button_decrease) {
			ewl_object_set_preferred_w(EWL_OBJECT(c),
				MAX(ewl_object_get_preferred_w(EWL_OBJECT(
							s->button_increase)),
					ewl_object_get_preferred_w(EWL_OBJECT(
							s->button_decrease))));
		}
	}
	else if (s->entry && s->button_increase && s->button_decrease) {
		size = ewl_object_get_preferred_h(EWL_OBJECT(
					s->button_increase)) +
			ewl_object_get_preferred_h(EWL_OBJECT(
						s->button_decrease));
		ewl_object_set_preferred_h(EWL_OBJECT(c),
			MAX(ewl_object_get_preferred_h(EWL_OBJECT(s->entry)),
			size));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spinner_realize_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	ewl_spinner_calc_value(s, s->value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spinner_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             size;
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	/*
	 * Place the entry box, request it's height so we can use it later for
	 * determining available space for the buttons.
	 */
	ewl_object_request_position(EWL_OBJECT(s->entry), CURRENT_X(w),
			CURRENT_Y(w));
	ewl_object_request_h(EWL_OBJECT(s->entry), CURRENT_H(w));
	size = ewl_object_get_current_h(EWL_OBJECT(s->entry));

	/*
	 * Vertically position the increase button.
	 */
	ewl_object_request_y(EWL_OBJECT(s->button_increase), CURRENT_Y(w));
	ewl_object_request_h(EWL_OBJECT(s->button_increase), size / 2);

	/*
	 * Vertically position the decrease button.
	 */
	ewl_object_request_y(EWL_OBJECT(s->button_decrease), CURRENT_Y(w) +
		ewl_object_get_current_h(EWL_OBJECT(s->button_increase)) +
		(size % 2));
	ewl_object_request_h(EWL_OBJECT(s->button_decrease), size / 2);

	/*
	 * The buttons don't fill. So rely on the theme to set a maximum size.
	 */
	ewl_object_request_w(EWL_OBJECT(s->button_increase), CURRENT_W(w));
	ewl_object_request_w(EWL_OBJECT(s->button_decrease), CURRENT_W(w));

	/*
	 * Use the max of the two resulting button sizes to determine the area
	 * available for layout of each component.
	 */
	size = MAX(ewl_object_get_current_w(EWL_OBJECT(s->button_increase)),
		ewl_object_get_current_w(EWL_OBJECT(s->button_decrease)));

	ewl_object_request_w(EWL_OBJECT(s->entry), CURRENT_W(w) - size);

	ewl_object_request_x(EWL_OBJECT(s->button_increase),
			CURRENT_X(w) +
			ewl_object_get_current_w(EWL_OBJECT(s->entry)) +
			(size - ewl_object_get_current_w(EWL_OBJECT(s->button_increase)))
			/ 2);

	ewl_object_request_x(EWL_OBJECT(s->button_decrease),
			CURRENT_X(w) +
			ewl_object_get_current_w(EWL_OBJECT(s->entry)) +
			(size - ewl_object_get_current_w(EWL_OBJECT(s->button_decrease)))
			/ 2);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spinner_key_down_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry              *e;
	Ewl_Spinner            *s;
	Ewl_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	s = EWL_SPINNER(w->parent);

	ev = ev_data;

	if (!strcmp(ev->keyname, "Up"))
		ewl_spinner_increase_value_cb(w, NULL, NULL);
	else if (!strcmp(ev->keyname, "Down"))
		ewl_spinner_decrease_value_cb(w, NULL, NULL);
	else if (!strcmp(ev->keyname, "Left"))
		ewl_entry_move_cursor_to_left(e);
	else if (!strcmp(ev->keyname, "Right"))
		ewl_entry_move_cursor_to_right(e);
	else if (!strcmp(ev->keyname, "Home"))
		ewl_entry_move_cursor_to_home(e);
	else if (!strcmp(ev->keyname, "End"))
		ewl_entry_move_cursor_to_end(e);
	else if (!strcmp(ev->keyname, "BackSpace"))
		ewl_entry_delete_to_left(e);
	else if (!strcmp(ev->keyname, "Delete"))
		ewl_entry_delete_to_right(e);
	else if (ev->keyname && (isdigit(ev->keyname[0]) ||
				 ev->keyname[0] == '.' ||
				 ev->keyname[0] == '-'))
		ewl_entry_insert_text(e, ev->keyname);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spinner_deselect_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner    *s;
	char           *str;
	float           val;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	str = ewl_entry_get_text(EWL_ENTRY(s->entry));

	if (str && strlen(str)) {
		val = atof(str);

		ewl_spinner_calc_value(s, (double) (val));
	} else if (str)
		FREE(str);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_spinner_calc_value(Ewl_Spinner * s, double value)
{
	char            format[64];
	char            str[64];
	double          oval;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	oval = s->value;

	if (value < s->min_val)
		s->value = s->min_val;
	else if (value > s->max_val)
		s->value = s->max_val;
	else
		s->value = value;

	snprintf(format, 64, "%%.%df", s->digits);
	snprintf(str, 64, format, s->value);

	ewl_entry_set_text(EWL_ENTRY(s->entry), str);

	if (oval != s->value) {
		oval = s->value;
		ewl_callback_call_with_event_data(EWL_WIDGET(s),
				EWL_CALLBACK_VALUE_CHANGED, &oval);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spinner_increase_value_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	ewl_spinner_deselect_cb(w, NULL, NULL);

	ewl_spinner_calc_value(s, s->value + s->step);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_spinner_decrease_value_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	ewl_spinner_deselect_cb(w, NULL, NULL);

	ewl_spinner_calc_value(s, s->value - s->step);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
