
#include <Ewl.h>

static void     __ewl_spinner_child_add(Ewl_Container *c, Ewl_Widget *w);
static void     __ewl_spinner_child_resize(Ewl_Container *c, Ewl_Widget *w,
					   int size, Ewl_Orientation o);
static void     __ewl_spinner_realize(Ewl_Widget * widget, void *ev_data,
				      void *user_data);
static void     __ewl_spinner_configure(Ewl_Widget * widget, void *ev_data,
					void *user_data);
static void     __ewl_spinner_key_down(Ewl_Widget * widget, void *ev_data,
				       void *user_data);
static void     __ewl_spinner_deselect(Ewl_Widget * w, void *ev_data,
				       void *user_data);

static void     __ewl_spinner_set_value(Ewl_Widget * widget, double value);
static double   __ewl_spinner_get_value(Ewl_Widget * widget);
static void     __ewl_spinner_set_digits(Ewl_Widget * widget, int digits);
static void     __ewl_spinner_set_min_val(Ewl_Widget * widget, double val);
static void     __ewl_spinner_set_max_val(Ewl_Widget * widget, double val);
static void     __ewl_spinner_increase_value(Ewl_Widget * widget,
					     void *ev_data, void *user_data);
static void     __ewl_spinner_decrease_value(Ewl_Widget * widget,
					     void *ev_data, void *user_data);

static void     __ewl_entry_key_down(Ewl_Widget * w, void *ev_data,
				     void *user_data);
static void     __ewl_entry_move_cursor_to_left(Ewl_Widget * w);
static void     __ewl_entry_move_cursor_to_right(Ewl_Widget * w);
static void     __ewl_entry_move_cursor_to_home(Ewl_Widget * w);
static void     __ewl_entry_move_cursor_to_end(Ewl_Widget * w);
static void     __ewl_entry_insert_text(Ewl_Widget * w, char *s);
static void     __ewl_entry_delete_to_left(Ewl_Widget * w);
static void     __ewl_entry_delete_to_right(Ewl_Widget * w);


/**
 * @return Returns a new spinner widget on success, NULL on failure.
 * @brief Allocate a new spinner widget
 */
Ewl_Widget     *ewl_spinner_new()
{
	Ewl_Spinner    *s = NULL;

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
void ewl_spinner_init(Ewl_Spinner * s)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_container_init(EWL_CONTAINER(w), "spinner",
			__ewl_spinner_child_add, __ewl_spinner_child_resize,
			NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_HFILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_spinner_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_spinner_configure,
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

	ewl_callback_del(s->entry, EWL_CALLBACK_KEY_DOWN, __ewl_entry_key_down);
	ewl_callback_append(s->entry, EWL_CALLBACK_KEY_DOWN,
			    __ewl_spinner_key_down, NULL);
	ewl_callback_append(s->entry, EWL_CALLBACK_DESELECT,
			    __ewl_spinner_deselect, NULL);
	ewl_callback_append(s->button_increase, EWL_CALLBACK_CLICKED,
			    __ewl_spinner_increase_value, w);
	ewl_callback_append(s->button_increase, EWL_CALLBACK_KEY_DOWN,
			    __ewl_spinner_key_down, NULL);
	ewl_callback_append(s->button_decrease, EWL_CALLBACK_CLICKED,
			    __ewl_spinner_decrease_value, w);
	ewl_callback_append(s->button_decrease, EWL_CALLBACK_KEY_DOWN,
			    __ewl_spinner_key_down, NULL);
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

	__ewl_spinner_set_value(EWL_WIDGET(s), value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the spinner widget to retrieve the value
 * @return Returns the current value in @a s on success, -1.0 on failure.
 * @brief Get the current value of a spinner widget
 */
double ewl_spinner_get_value(Ewl_Spinner * s)
{
	DCHECK_PARAM_PTR_RET("s", s, -1.00);

	return __ewl_spinner_get_value(EWL_WIDGET(s));
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
	DCHECK_PARAM_PTR("s", s);

	__ewl_spinner_set_digits(EWL_WIDGET(s), digits);
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
	DCHECK_PARAM_PTR("s", s);

	__ewl_spinner_set_min_val(EWL_WIDGET(s), val);
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
	DCHECK_PARAM_PTR("s", s);

	__ewl_spinner_set_max_val(EWL_WIDGET(s), val);
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

static void
__ewl_spinner_child_add(Ewl_Container *c, Ewl_Widget *w)
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

static void
__ewl_spinner_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
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

static void
__ewl_spinner_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	__ewl_spinner_set_value(w, s->value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_configure(Ewl_Widget * w, void *ev_data, void *user_data)
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

static void
__ewl_spinner_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner    *s;
	Ecore_X_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	ev = ev_data;

	if (!strcmp(ev->keyname, "Up"))
		__ewl_spinner_increase_value(w, NULL, NULL);
	else if (!strcmp(ev->keyname, "Down"))
		__ewl_spinner_decrease_value(w, NULL, NULL);
	else if (!strcmp(ev->keyname, "Left"))
		__ewl_entry_move_cursor_to_left(w);
	else if (!strcmp(ev->keyname, "Right"))
		__ewl_entry_move_cursor_to_right(w);
	else if (!strcmp(ev->keyname, "Home"))
		__ewl_entry_move_cursor_to_home(w);
	else if (!strcmp(ev->keyname, "End"))
		__ewl_entry_move_cursor_to_end(w);
	else if (!strcmp(ev->keyname, "BackSpace"))
		__ewl_entry_delete_to_left(w);
	else if (!strcmp(ev->keyname, "Delete"))
		__ewl_entry_delete_to_right(w);
	else if (ev->key_compose && (ev->key_compose[0] == '0' ||
				 ev->key_compose[0] == '1' ||
				 ev->key_compose[0] == '2' ||
				 ev->key_compose[0] == '3' ||
				 ev->key_compose[0] == '4' ||
				 ev->key_compose[0] == '5' ||
				 ev->key_compose[0] == '6' ||
				 ev->key_compose[0] == '7' ||
				 ev->key_compose[0] == '8' ||
				 ev->key_compose[0] == '9' ||
				 ev->key_compose[0] == '.' ||
				 ev->key_compose[0] == '-'))
		__ewl_entry_insert_text(s->entry, ev->key_compose);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_deselect(Ewl_Widget * w, void *ev_data, void *user_data)
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

		__ewl_spinner_set_value(EWL_WIDGET(s), (double) (val));
	} else if (str)
		FREE(str);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_set_value(Ewl_Widget * w, double value)
{
	Ewl_Spinner    *s;
	char            format[64];
	char            str[64];
	double          oval;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

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

	if (oval != s->value)
		ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static double
__ewl_spinner_get_value(Ewl_Widget * w)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1.0);

	s = EWL_SPINNER(w);

	return s->value;
}

static void
__ewl_spinner_set_digits(Ewl_Widget * w, int digits)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	s->digits = digits;

	__ewl_spinner_set_value(w, s->value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_set_min_val(Ewl_Widget * w, double val)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	s->min_val = val;

	__ewl_spinner_set_value(w, s->value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_set_max_val(Ewl_Widget * w, double val)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	s->max_val = val;

	__ewl_spinner_set_value(w, s->value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_increase_value(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	__ewl_spinner_deselect(w, NULL, NULL);

	__ewl_spinner_set_value(w->parent, s->value + s->step);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_decrease_value(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner    *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	__ewl_spinner_deselect(w, NULL, NULL);

	__ewl_spinner_set_value(w->parent, s->value - s->step);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
