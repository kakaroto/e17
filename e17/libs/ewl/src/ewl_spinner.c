
#include <Ewl.h>

static void __ewl_spinner_init(Ewl_Spinner * spinner);
static void __ewl_spinner_realize(Ewl_Widget * widget, void *event_data,
				  void *user_data);
static void __ewl_spinner_show(Ewl_Widget * widget, void *event_data,
			       void *user_data);
static void __ewl_spinner_hide(Ewl_Widget * widget, void *event_data,
			       void *user_data);
static void __ewl_spinner_destroy(Ewl_Widget * widget, void *event_data,
				  void *user_data);
static void __ewl_spinner_configure(Ewl_Widget * widget, void *event_data,
				    void *user_data);
static void __ewl_spinner_key_down(Ewl_Widget * widget, void *event_data,
				   void *user_data);

static void __ewl_spinner_set_value(Ewl_Widget * widget, double value);
static double __ewl_spinner_get_value(Ewl_Widget * widget);
static void __ewl_spinner_set_digits(Ewl_Widget * widget, int digits);
static void __ewl_spinner_set_min_val(Ewl_Widget * widget, double val);
static void __ewl_spinner_set_max_val(Ewl_Widget * widget, double val);
static void __ewl_spinner_increase_value(Ewl_Widget * widget,
					 void *event_data, void *user_data);
static void __ewl_spinner_decrease_value(Ewl_Widget * widget,
					 void *event_data, void *user_data);

Ewl_Widget *
ewl_spinner_new()
{
	Ewl_Spinner *spinner = NULL;

	spinner = NEW(Ewl_Spinner, 1);

	__ewl_spinner_init(spinner);

	return EWL_WIDGET(spinner);
}

void
ewl_spinner_set_value(Ewl_Widget * widget, double value)
{
	CHECK_PARAM_POINTER("widget", widget);

	__ewl_spinner_set_value(widget, value);
}

double
ewl_spinner_get_value(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, -1.00);

	return __ewl_spinner_get_value(widget);
}

void
ewl_spinner_set_digits(Ewl_Widget * widget, int digits)
{
	CHECK_PARAM_POINTER("widget", widget);

	__ewl_spinner_set_digits(widget, digits);
}

void
ewl_spinner_set_min_val(Ewl_Widget * widget, double val)
{
	CHECK_PARAM_POINTER("widget", widget);

	__ewl_spinner_set_min_val(widget, val);
}

void
ewl_spinner_set_max_val(Ewl_Widget * widget, double val)
{
	CHECK_PARAM_POINTER("widget", widget);

	__ewl_spinner_set_max_val(widget, val);
}








static void
__ewl_spinner_init(Ewl_Spinner * spinner)
{
	CHECK_PARAM_POINTER("spinner", spinner);

	memset(spinner, 0, sizeof(Ewl_Spinner));
	ewl_widget_init(EWL_WIDGET(spinner), EWL_WIDGET_SPINNER, 100, 15,
			400, 15);

	ewl_callback_append(EWL_WIDGET(spinner),
			    EWL_CALLBACK_REALIZE, __ewl_spinner_realize,
			    NULL);
	ewl_callback_append(EWL_WIDGET(spinner), EWL_CALLBACK_SHOW,
			    __ewl_spinner_show, NULL);
	ewl_callback_append(EWL_WIDGET(spinner), EWL_CALLBACK_HIDE,
			    __ewl_spinner_hide, NULL);
	ewl_callback_append(EWL_WIDGET(spinner), EWL_CALLBACK_DESTROY,
			    __ewl_spinner_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(spinner), EWL_CALLBACK_CONFIGURE,
			    __ewl_spinner_configure, NULL);

	spinner->min_val = -50.000;
	spinner->max_val = 50.000;
	spinner->value = 0.000;
	spinner->digits = 2;

}

static void
__ewl_spinner_realize(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ewl_Widget *entry, *button_increase, *button_decrease;

	CHECK_PARAM_POINTER("widget", widget);

	ewl_fx_clip_box_create(widget);

	EWL_CONTAINER(widget)->clip_box = widget->fx_clip_box;
	evas_set_clip(widget->evas, widget->fx_clip_box,
		      EWL_CONTAINER(widget->parent)->clip_box);

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);

	EWL_CONTAINER(widget)->children = ewd_list_new();

	entry = ewl_entry_new();
	ewl_container_append_child(EWL_CONTAINER(widget), entry);
	ewl_callback_append(entry, EWL_CALLBACK_KEY_DOWN,
			    __ewl_spinner_key_down, NULL);
	ewl_widget_realize(entry);

	button_increase = ewl_button_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(widget), button_increase);
	EWL_OBJECT(button_increase)->minimum.w = 10;
	EWL_OBJECT(button_increase)->minimum.h = 10;
	ewl_callback_append(button_increase, EWL_CALLBACK_CLICKED,
			    __ewl_spinner_increase_value, widget);
	ewl_callback_append(button_increase, EWL_CALLBACK_KEY_DOWN,
			    __ewl_spinner_key_down, NULL);
	ewl_widget_realize(button_increase);

	button_decrease = ewl_button_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(widget), button_decrease);
	EWL_OBJECT(button_decrease)->minimum.w = 10;
	EWL_OBJECT(button_decrease)->minimum.h = 10;
	ewl_callback_append(button_decrease, EWL_CALLBACK_CLICKED,
			    __ewl_spinner_decrease_value, widget);
	ewl_callback_append(button_decrease, EWL_CALLBACK_KEY_DOWN,
			    __ewl_spinner_key_down, NULL);
	ewl_widget_realize(button_decrease);
}

static void
__ewl_spinner_show(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ewl_Widget *child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	while ((child =
		ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
		ewl_widget_show(child);

	evas_show(widget->evas, widget->fx_clip_box);

	__ewl_spinner_set_value(widget, EWL_SPINNER(widget)->value);
}

static void
__ewl_spinner_hide(Ewl_Widget * widget, void *event_data, void *user_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_hide(widget->evas, widget->fx_clip_box);
}

static void
__ewl_spinner_destroy(Ewl_Widget * widget, void *event_data, void *user_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_widget_destroy(EWL_SPINNER(widget)->entry);
	ewl_widget_destroy(EWL_SPINNER(widget)->button_increase);
	ewl_widget_destroy(EWL_SPINNER(widget)->button_decrease);

	evas_hide(widget->evas, widget->fx_clip_box);
	evas_unset_clip(widget->evas, widget->fx_clip_box);
	evas_del_object(widget->evas, widget->fx_clip_box);

	ewl_callback_del_all(widget);

	ewl_theme_deinit_widget(widget);

	FREE(widget);
}

static void
__ewl_spinner_configure(Ewl_Widget * widget, void *event_data,
			void *user_data)
{
	Ewl_Widget *child;
	int x = 0, y = 0, w = 50;

	CHECK_PARAM_POINTER("widget", widget);

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	while ((child =
		ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
	  {
		  if (child->type == EWL_WIDGET_ENTRY)
		    {
			    EWL_OBJECT(child)->request.x =
				    EWL_OBJECT(widget)->request.x;
			    EWL_OBJECT(child)->request.y =
				    EWL_OBJECT(widget)->request.y;
			    EWL_OBJECT(child)->request.w =
				    EWL_OBJECT(widget)->request.w - 12;
			    ewl_widget_configure(child);
			    x = EWL_OBJECT(widget)->request.x +
				    EWL_OBJECT(child)->current.w + 2;
			    y = EWL_OBJECT(widget)->request.y;
			    w = EWL_OBJECT(child)->current.w;
		    }
		  else if (child->type == EWL_WIDGET_BUTTON)
		    {
			    EWL_OBJECT(child)->request.x = x;
			    EWL_OBJECT(child)->request.y = y;
			    EWL_OBJECT(child)->custom.w = 10;
			    EWL_OBJECT(child)->custom.h = 10;
			    if (y == EWL_OBJECT(widget)->request.y)
				    y += 10;

			    ewl_widget_configure(child);
		    }
	  }


	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
	EWL_OBJECT(widget)->current.w = w + 12;
	EWL_OBJECT(widget)->current.h = 30;
	EWL_OBJECT(widget)->request.w = EWL_OBJECT(widget)->current.w;
	EWL_OBJECT(widget)->request.h = EWL_OBJECT(widget)->current.h;

	ewl_fx_clip_box_resize(widget);
}

static void
__ewl_spinner_key_down(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ev_Key_Down *ev;
	CHECK_PARAM_POINTER("widget", widget);

	ev = (Ev_Key_Down *) event_data;

	if (!strcmp(ev->key, "Up"))
		__ewl_spinner_increase_value(widget, NULL, NULL);
	else if (!strcmp(ev->key, "Down"))
		__ewl_spinner_decrease_value(widget, NULL, NULL);
}


static void
__ewl_spinner_set_value(Ewl_Widget * widget, double value)
{
	Ewl_Widget *entry;
	char *format;
	char *str;

	CHECK_PARAM_POINTER("widget", widget);

	if (value < EWL_SPINNER(widget)->min_val)
		EWL_SPINNER(widget)->value = EWL_SPINNER(widget)->min_val;
	else if (value > EWL_SPINNER(widget)->max_val)
		EWL_SPINNER(widget)->value = EWL_SPINNER(widget)->max_val;
	else
		EWL_SPINNER(widget)->value = value;

	format = malloc(64);
	snprintf(format, 64, "%%.%df", EWL_SPINNER(widget)->digits);
	str = malloc(64);
	snprintf(str, 64, format, EWL_SPINNER(widget)->value);

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);
	entry = ewd_list_current(EWL_CONTAINER(widget)->children);

	ewl_entry_set_text(entry, str);

	FREE(format);
	FREE(str);
}

static double
__ewl_spinner_get_value(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, -1.000);

	return EWL_SPINNER(widget)->value;
}

static void
__ewl_spinner_set_digits(Ewl_Widget * widget, int digits)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_SPINNER(widget)->digits = digits;

	__ewl_spinner_set_value(widget, EWL_SPINNER(widget)->value);
}

static void
__ewl_spinner_set_min_val(Ewl_Widget * widget, double val)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_SPINNER(widget)->min_val = val;

	__ewl_spinner_set_value(widget, EWL_SPINNER(widget)->value);
}

static void
__ewl_spinner_set_max_val(Ewl_Widget * widget, double val)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_SPINNER(widget)->max_val = val;

	__ewl_spinner_set_value(widget, EWL_SPINNER(widget)->value);
}

static void
__ewl_spinner_increase_value(Ewl_Widget * widget, void *event_data,
			     void *user_data)
{
	double val;

	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_SPINNER(widget->parent)->digits)
		val = 1.0;
	else if (EWL_SPINNER(widget->parent)->digits == 1)
		val = 0.1;
	else if (EWL_SPINNER(widget->parent)->digits == 2)
		val = 0.01;
	else if (EWL_SPINNER(widget->parent)->digits == 3)
		val = 0.001;
	else if (EWL_SPINNER(widget->parent)->digits == 4)
		val = 0.0001;
	else if (EWL_SPINNER(widget->parent)->digits == 5)
		val = 0.00001;
	else if (EWL_SPINNER(widget->parent)->digits == 6)
		val = 0.000001;
	else
		val = 0.0000001;

	__ewl_spinner_set_value(widget->parent,
				EWL_SPINNER(widget->parent)->value + val);
}

static void
__ewl_spinner_decrease_value(Ewl_Widget * widget, void *event_data,
			     void *user_data)
{
	double val;

	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_SPINNER(widget->parent)->digits)
		val = 1.0;
	else if (EWL_SPINNER(widget->parent)->digits == 1)
		val = 0.1;
	else if (EWL_SPINNER(widget->parent)->digits == 2)
		val = 0.01;
	else if (EWL_SPINNER(widget->parent)->digits == 3)
		val = 0.001;
	else if (EWL_SPINNER(widget->parent)->digits == 4)
		val = 0.0001;
	else if (EWL_SPINNER(widget->parent)->digits == 5)
		val = 0.00001;
	else if (EWL_SPINNER(widget->parent)->digits == 6)
		val = 0.000001;
	else
		val = 0.0000001;

	__ewl_spinner_set_value(widget->parent,
				EWL_SPINNER(widget->parent)->value - val);
}
