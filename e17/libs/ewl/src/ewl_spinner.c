
#include <Ewl.h>

static void __ewl_spinner_init (Ewl_Spinner * spinner);
static void __ewl_spinner_realize (Ewl_Widget * widget, void *ev_data,
				   void *user_data);
static void __ewl_spinner_show (Ewl_Widget * widget, void *ev_data,
				void *user_data);
static void __ewl_spinner_hide (Ewl_Widget * widget, void *ev_data,
				void *user_data);
static void __ewl_spinner_configure (Ewl_Widget * widget, void *ev_data,
				     void *user_data);
static void __ewl_spinner_key_down (Ewl_Widget * widget, void *ev_data,
				    void *user_data);

static void __ewl_spinner_set_value (Ewl_Widget * widget, double value);
static double __ewl_spinner_get_value (Ewl_Widget * widget);
static void __ewl_spinner_set_digits (Ewl_Widget * widget, int digits);
static void __ewl_spinner_set_min_val (Ewl_Widget * widget, double val);
static void __ewl_spinner_set_max_val (Ewl_Widget * widget, double val);
static void __ewl_spinner_increase_value (Ewl_Widget * widget,
					  void *ev_data, void *user_data);
static void __ewl_spinner_decrease_value (Ewl_Widget * widget,
					  void *ev_data, void *user_data);

Ewl_Widget *
ewl_spinner_new ()
{
	Ewl_Spinner *spinner = NULL;

	spinner = NEW (Ewl_Spinner, 1);
	if (!spinner)
		DRETURN_PTR (NULL);

	memset (spinner, 0, sizeof (Ewl_Spinner));
	__ewl_spinner_init (spinner);

	return EWL_WIDGET (spinner);
}

void
ewl_spinner_set_value (Ewl_Widget * widget, double value)
{
	DCHECK_PARAM_PTR ("widget", widget);

	__ewl_spinner_set_value (widget, value);
}

double
ewl_spinner_get_value (Ewl_Widget * widget)
{
	DCHECK_PARAM_INT_RET ("widget", widget, -1.00);

	return __ewl_spinner_get_value (widget);
}

void
ewl_spinner_set_digits (Ewl_Widget * widget, int digits)
{
	DCHECK_PARAM_PTR ("widget", widget);

	__ewl_spinner_set_digits (widget, digits);
}

void
ewl_spinner_set_min_val (Ewl_Widget * widget, double val)
{
	DCHECK_PARAM_PTR ("widget", widget);

	__ewl_spinner_set_min_val (widget, val);
}

void
ewl_spinner_set_max_val (Ewl_Widget * widget, double val)
{
	DCHECK_PARAM_PTR ("widget", widget);

	__ewl_spinner_set_max_val (widget, val);
}








static void
__ewl_spinner_init (Ewl_Spinner * spinner)
{
	DCHECK_PARAM_PTR ("spinner", spinner);

	ewl_widget_init (EWL_WIDGET (spinner), "/appearance/spinner");
	ewl_object_set_minimum_size (EWL_OBJECT (spinner), 100, 15);

	EWL_WIDGET (spinner)->recursive = TRUE;

	ewl_callback_append (EWL_WIDGET (spinner), EWL_CALLBACK_REALIZE,
			     __ewl_spinner_realize, NULL);
	ewl_callback_append (EWL_WIDGET (spinner), EWL_CALLBACK_SHOW,
			     __ewl_spinner_show, NULL);
	ewl_callback_append (EWL_WIDGET (spinner), EWL_CALLBACK_HIDE,
			     __ewl_spinner_hide, NULL);
	ewl_callback_append (EWL_WIDGET (spinner), EWL_CALLBACK_CONFIGURE,
			     __ewl_spinner_configure, NULL);

	spinner->min_val = -50.0;
	spinner->max_val = 50.0;
	spinner->value = 0.0;
	spinner->digits = 2;

}

static void
__ewl_spinner_realize (Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner *s;

	DCHECK_PARAM_PTR ("w", w);

	s = EWL_SPINNER (w);

	{
		Evas_Object *clip_box;

		clip_box = evas_add_rectangle (w->evas);
		evas_set_color (w->evas, clip_box, 255, 255, 255, 255);
		evas_set_layer (w->evas, clip_box, LAYER (w) - 1);
		if (w->parent && EWL_CONTAINER (w->parent)->clip_box)
			evas_set_clip (w->evas, clip_box,
				       EWL_CONTAINER (w->parent)->clip_box);
		w->fx_clip_box = clip_box;
		EWL_CONTAINER (w)->clip_box = clip_box;

	}

	EWL_CONTAINER (w)->children = ewd_list_new ();

	s->entry = ewl_entry_new ();
	ewl_container_append_child (EWL_CONTAINER (w), s->entry);
	ewl_callback_append (s->entry, EWL_CALLBACK_KEY_DOWN,
			     __ewl_spinner_key_down, NULL);
	ewl_widget_realize (s->entry);

	s->button_increase = ewl_button_new (NULL);
	ewl_container_append_child (EWL_CONTAINER (w), s->button_increase);
	MIN_W (s->button_increase) = 10;
	MIN_H (s->button_increase) = 10;
	ewl_callback_append (s->button_increase, EWL_CALLBACK_CLICKED,
			     __ewl_spinner_increase_value, w);
	ewl_callback_append (s->button_increase, EWL_CALLBACK_KEY_DOWN,
			     __ewl_spinner_key_down, NULL);
	ewl_widget_realize (s->button_increase);

	s->button_decrease = ewl_button_new (NULL);
	ewl_container_append_child (EWL_CONTAINER (w), s->button_decrease);
	MIN_W (s->button_decrease) = 10;
	MIN_H (s->button_decrease) = 10;
	ewl_callback_append (s->button_decrease, EWL_CALLBACK_CLICKED,
			     __ewl_spinner_decrease_value, w);
	ewl_callback_append (s->button_decrease, EWL_CALLBACK_KEY_DOWN,
			     __ewl_spinner_key_down, NULL);
	ewl_widget_realize (s->button_decrease);
}

static void
__ewl_spinner_show (Ewl_Widget * widget, void *ev_data, void *user_data)
{
	Ewl_Widget *child = NULL;

	DCHECK_PARAM_PTR ("widget", widget);

	ewd_list_goto_first (EWL_CONTAINER (widget)->children);

	while ((child =
		ewd_list_next (EWL_CONTAINER (widget)->children)) != NULL)
		ewl_widget_show (child);

	evas_show (widget->evas, widget->fx_clip_box);
}

static void
__ewl_spinner_hide (Ewl_Widget * widget, void *ev_data, void *user_data)
{
	DCHECK_PARAM_PTR ("widget", widget);

	evas_hide (widget->evas, widget->fx_clip_box);
}

static void
__ewl_spinner_configure (Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Spinner *s;

	DCHECK_PARAM_PTR ("w", w);

	s = EWL_SPINNER (w);

	ewd_list_goto_first (EWL_CONTAINER (w)->children);

	REQUEST_X (s->entry) = REQUEST_X (w);
	REQUEST_Y (s->entry) = REQUEST_Y (w);
	REQUEST_W (s->entry) = REQUEST_W (s->entry) - 12;
	REQUEST_H (s->entry) = 20;

	REQUEST_X (s->button_increase) = REQUEST_X (s->entry) +
		REQUEST_W (s->entry) + 2;
	REQUEST_Y (s->button_increase) = REQUEST_Y (s->entry);
	REQUEST_W (s->button_increase) = 10;
	REQUEST_H (s->button_increase) = 10;

	REQUEST_X (s->button_decrease) = REQUEST_X (s->entry) +
		REQUEST_W (s->entry) + 2;
	REQUEST_Y (s->button_decrease) = REQUEST_Y (s->entry) + 10;
	REQUEST_W (s->button_decrease) = 10;
	REQUEST_H (s->button_decrease) = 10;

	ewl_widget_configure (s->entry);
	ewl_widget_configure (s->button_increase);
	ewl_widget_configure (s->button_decrease);

	ewl_object_set_current_geometry (EWL_OBJECT (w),
					 REQUEST_X (w), REQUEST_Y (w),
					 REQUEST_W (s->entry) + 12, 30);
}

static void
__ewl_spinner_key_down (Ewl_Widget * widget, void *ev_data, void *user_data)
{
	Ev_Key_Down *ev;

	DCHECK_PARAM_PTR ("widget", widget);

	ev = (Ev_Key_Down *) ev_data;

	if (!strcmp (ev->key, "Up"))
		__ewl_spinner_increase_value (widget, NULL, NULL);
	else if (!strcmp (ev->key, "Down"))
		__ewl_spinner_decrease_value (widget, NULL, NULL);
}


static void
__ewl_spinner_set_value (Ewl_Widget * widget, double value)
{
	Ewl_Spinner *s;
	char *format;
	char *str;

	DCHECK_PARAM_PTR ("widget", widget);

	s = EWL_SPINNER (widget);

	if (value < EWL_SPINNER (widget)->min_val)
		EWL_SPINNER (widget)->value = EWL_SPINNER (widget)->min_val;
	else if (value > EWL_SPINNER (widget)->max_val)
		EWL_SPINNER (widget)->value = EWL_SPINNER (widget)->max_val;
	else
		EWL_SPINNER (widget)->value = value;

	format = malloc (64);
	snprintf (format, 64, "%%.%df", EWL_SPINNER (widget)->digits);
	str = malloc (64);
	snprintf (str, 64, format, EWL_SPINNER (widget)->value);

	ewl_entry_set_text (EWL_WIDGET (s->entry), str);

	FREE (format);
	FREE (str);
}

static double
__ewl_spinner_get_value (Ewl_Widget * widget)
{
	DCHECK_PARAM_INT_RET ("widget", widget, -1.000);

	return EWL_SPINNER (widget)->value;
}

static void
__ewl_spinner_set_digits (Ewl_Widget * widget, int digits)
{
	DCHECK_PARAM_PTR ("widget", widget);

	EWL_SPINNER (widget)->digits = digits;

	__ewl_spinner_set_value (widget, EWL_SPINNER (widget)->value);
}

static void
__ewl_spinner_set_min_val (Ewl_Widget * widget, double val)
{
	DCHECK_PARAM_PTR ("widget", widget);

	EWL_SPINNER (widget)->min_val = val;

	__ewl_spinner_set_value (widget, EWL_SPINNER (widget)->value);
}

static void
__ewl_spinner_set_max_val (Ewl_Widget * widget, double val)
{
	DCHECK_PARAM_PTR ("widget", widget);

	EWL_SPINNER (widget)->max_val = val;

	__ewl_spinner_set_value (widget, EWL_SPINNER (widget)->value);
}

static void
__ewl_spinner_increase_value (Ewl_Widget * widget, void *ev_data,
			      void *user_data)
{
	double val;

	DCHECK_PARAM_PTR ("widget", widget);

	if (!EWL_SPINNER (widget->parent)->digits)
		val = 1.0;
	else if (EWL_SPINNER (widget->parent)->digits == 1)
		val = 0.1;
	else if (EWL_SPINNER (widget->parent)->digits == 2)
		val = 0.01;
	else if (EWL_SPINNER (widget->parent)->digits == 3)
		val = 0.001;
	else if (EWL_SPINNER (widget->parent)->digits == 4)
		val = 0.0001;
	else if (EWL_SPINNER (widget->parent)->digits == 5)
		val = 0.00001;
	else if (EWL_SPINNER (widget->parent)->digits == 6)
		val = 0.000001;
	else
		val = 0.0000001;

	__ewl_spinner_set_value (widget->parent,
				 EWL_SPINNER (widget->parent)->value + val);
}

static void
__ewl_spinner_decrease_value (Ewl_Widget * widget, void *ev_data,
			      void *user_data)
{
	double val;

	DCHECK_PARAM_PTR ("widget", widget);

	if (!EWL_SPINNER (widget->parent)->digits)
		val = 1.0;
	else if (EWL_SPINNER (widget->parent)->digits == 1)
		val = 0.1;
	else if (EWL_SPINNER (widget->parent)->digits == 2)
		val = 0.01;
	else if (EWL_SPINNER (widget->parent)->digits == 3)
		val = 0.001;
	else if (EWL_SPINNER (widget->parent)->digits == 4)
		val = 0.0001;
	else if (EWL_SPINNER (widget->parent)->digits == 5)
		val = 0.00001;
	else if (EWL_SPINNER (widget->parent)->digits == 6)
		val = 0.000001;
	else
		val = 0.0000001;

	__ewl_spinner_set_value (widget->parent,
				 EWL_SPINNER (widget->parent)->value - val);
}
