
#include <Ewl.h>

static void __ewl_spinner_init(Ewl_Spinner * spinner);
static void __ewl_spinner_realize(Ewl_Widget * widget, void *ev_data,
				  void *user_data);
static void __ewl_spinner_configure(Ewl_Widget * widget, void *ev_data,
				    void *user_data);
static void __ewl_spinner_key_down(Ewl_Widget * widget, void *ev_data,
				   void *user_data);
void __ewl_spinner_deselect(Ewl_Widget * w, void *ev_data, void *user_data);

static void __ewl_spinner_set_value(Ewl_Widget * widget, double value);
static double __ewl_spinner_get_value(Ewl_Widget * widget);
static void __ewl_spinner_set_digits(Ewl_Widget * widget, int digits);
static void __ewl_spinner_set_min_val(Ewl_Widget * widget, double val);
static void __ewl_spinner_set_max_val(Ewl_Widget * widget, double val);
static void __ewl_spinner_increase_value(Ewl_Widget * widget,
					 void *ev_data, void *user_data);
static void __ewl_spinner_decrease_value(Ewl_Widget * widget,
					 void *ev_data, void *user_data);

void __ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_move_cursor_to_left(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_right(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_home(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_end(Ewl_Widget * w);
void __ewl_entry_insert_text(Ewl_Widget * w, char *s);
void __ewl_entry_delete_to_left(Ewl_Widget * w);
void __ewl_entry_delete_to_right(Ewl_Widget * w);


Ewl_Widget *
ewl_spinner_new()
{
	Ewl_Spinner *s = NULL;

	s = NEW(Ewl_Spinner, 1);

	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(s, 0, sizeof(Ewl_Spinner));

	s->entry = ewl_entry_new();
	s->button_increase = ewl_button_new(NULL);
	s->button_decrease = ewl_button_new(NULL);

	__ewl_spinner_init(s);

	ewl_container_append_child(EWL_CONTAINER(s), s->entry);
	ewl_container_append_child(EWL_CONTAINER(s), s->button_increase);
	ewl_container_append_child(EWL_CONTAINER(s), s->button_decrease);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

void
ewl_spinner_set_value(Ewl_Widget * w, double value)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	__ewl_spinner_set_value(w, value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

double
ewl_spinner_get_value(Ewl_Widget * widget)
{
	DCHECK_PARAM_PTR_RET("widget", widget, -1.00);

	return __ewl_spinner_get_value(widget);
}

void
ewl_spinner_set_digits(Ewl_Widget * widget, int digits)
{
	DCHECK_PARAM_PTR("widget", widget);

	__ewl_spinner_set_digits(widget, digits);
}

void
ewl_spinner_set_min_val(Ewl_Widget * widget, double val)
{
	DCHECK_PARAM_PTR("widget", widget);

	__ewl_spinner_set_min_val(widget, val);
}

void
ewl_spinner_set_max_val(Ewl_Widget * widget, double val)
{
	DCHECK_PARAM_PTR("widget", widget);

	__ewl_spinner_set_max_val(widget, val);
}

void
ewl_spinner_set_step(Ewl_Widget * w, double step)
{
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	s->step = step;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


static void
__ewl_spinner_init(Ewl_Spinner * s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_container_init(EWL_CONTAINER(w), "/appearance/spinner");
	ewl_object_set_minimum_size(EWL_OBJECT(w), 30, 20);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);

	w->recursive = TRUE;

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_spinner_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_spinner_configure, NULL);

	s->min_val = -50.0;
	s->max_val = 50.0;
	s->value = 0.0;
	s->step = 0.1;
	s->digits = 2;

	ewl_object_set_custom_size(EWL_OBJECT(s->button_increase), 10, 10);
	ewl_object_set_custom_size(EWL_OBJECT(s->button_decrease), 10, 10);

	ewl_callback_del(s->entry, EWL_CALLBACK_KEY_DOWN,
			 __ewl_entry_key_down);
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

static void
__ewl_spinner_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner *s;
	int ew = 0, eh = 0, biw = 0, bih = 0, bdw = 0, bdh = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	ewl_widget_show(s->entry);
	ewl_widget_show(s->button_increase);
	ewl_widget_show(s->button_decrease);

	__ewl_spinner_set_value(w, s->value);

	if (s->entry->ebits_object)
		ebits_get_max_size(s->entry->ebits_object, &ew, &eh);
	if (s->button_increase->ebits_object)
		ebits_get_max_size(s->button_increase->ebits_object, &biw,
				   &bih);
	if (s->button_decrease->ebits_object)
		ebits_get_max_size(s->button_decrease->ebits_object, &bdw,
				   &bdh);

	ewl_object_set_maximum_size(EWL_OBJECT(w), ew + biw + bdw, eh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w);

	REQUEST_X(s->entry) = CURRENT_X(w);
	REQUEST_Y(s->entry) = CURRENT_Y(w);
	REQUEST_W(s->entry) = CURRENT_W(w) - 12;

	REQUEST_X(s->button_increase) = REQUEST_X(s->entry) +
		REQUEST_W(s->entry) + 2;
	REQUEST_Y(s->button_increase) = REQUEST_Y(s->entry);
	REQUEST_W(s->button_increase) = 10;
	REQUEST_H(s->button_increase) = 10;

	REQUEST_X(s->button_decrease) = REQUEST_X(s->entry) +
		REQUEST_W(s->entry) + 2;
	REQUEST_Y(s->button_decrease) = REQUEST_Y(s->entry) + 10;
	REQUEST_W(s->button_decrease) = 10;
	REQUEST_H(s->button_decrease) = 10;

	ewl_widget_configure(s->entry);
	ewl_widget_configure(s->button_increase);
	ewl_widget_configure(s->button_decrease);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner *s;
	Ecore_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	ev = (Ecore_Event_Key_Down *) ev_data;

	if (!strcmp(ev->key, "Up"))
		__ewl_spinner_increase_value(w, NULL, NULL);
	else if (!strcmp(ev->key, "Down"))
		__ewl_spinner_decrease_value(w, NULL, NULL);
	else if (!strcmp(ev->key, "Left"))
		__ewl_entry_move_cursor_to_left(w);
	else if (!strcmp(ev->key, "Right"))
		__ewl_entry_move_cursor_to_right(w);
	else if (!strcmp(ev->key, "Home"))
		__ewl_entry_move_cursor_to_home(w);
	else if (!strcmp(ev->key, "End"))
		__ewl_entry_move_cursor_to_end(w);
	else if (!strcmp(ev->key, "BackSpace"))
		__ewl_entry_delete_to_left(w);
	else if (!strcmp(ev->key, "Delete"))
		__ewl_entry_delete_to_right(w);
	else if (ev->compose && (ev->compose[0] == '0' ||
				 ev->compose[0] == '1' ||
				 ev->compose[0] == '2' ||
				 ev->compose[0] == '3' ||
				 ev->compose[0] == '4' ||
				 ev->compose[0] == '5' ||
				 ev->compose[0] == '6' ||
				 ev->compose[0] == '7' ||
				 ev->compose[0] == '8' ||
				 ev->compose[0] == '9' ||
				 ev->compose[0] == '.' ||
				 ev->compose[0] == '-'))
		__ewl_entry_insert_text(s->entry, ev->compose);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_spinner_deselect(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner *s;
	char *str;
	float val;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	str = ewl_entry_get_text(s->entry);

	if (str && strlen(str))
	  {
		  val = atof(str);

		  __ewl_spinner_set_value(EWL_WIDGET(s), (double) (val));
	  }
	else if (str)
		FREE(str);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_spinner_set_value(Ewl_Widget * w, double value)
{
	Ewl_Spinner *s;
	char format[64];
	char str[64];
	double oval;

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

	ewl_entry_set_text(s->entry, str);

	if (oval != s->value)
		ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static double
__ewl_spinner_get_value(Ewl_Widget * w)
{
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1.0);

	s = EWL_SPINNER(w);

	return s->value;
}

static void
__ewl_spinner_set_digits(Ewl_Widget * w, int digits)
{
	Ewl_Spinner *s;

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
	Ewl_Spinner *s;

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
	Ewl_Spinner *s;

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
	Ewl_Spinner *s;

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
	Ewl_Spinner *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SPINNER(w->parent);

	__ewl_spinner_deselect(w, NULL, NULL);

	__ewl_spinner_set_value(w->parent, s->value - s->step);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
