
#include <Ewl.h>

static void __ewl_spinner_init(Ewl_Spinner * spinner);
static void __ewl_spinner_realize(Ewl_Widget * widget, void * func_data);
static void __ewl_spinner_show(Ewl_Widget * widget, void * func_data);
static void __ewl_spinner_hide(Ewl_Widget * widget, void * func_data);
static void __ewl_spinner_destroy(Ewl_Widget * widget, void * func_data);
static void __ewl_spinner_configure(Ewl_Widget * widget, void * func_data);

static void		__ewl_spinner_set_value(Ewl_Widget * widget, double value);
static double	__ewl_spinner_get_value(Ewl_Widget * widget);
static void		__ewl_spinner_set_digits(Ewl_Widget * widget, int digits);
static void		__ewl_spinner_set_min_val(Ewl_Widget * widget, double val);
static void		__ewl_spinner_set_max_val(Ewl_Widget * widget, double val);


Ewl_Widget *
ewl_spinner_new()
{
	Ewl_Spinner * spinner = NULL;

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

	EWL_WIDGET(spinner)->container.recursive = TRUE;
	EWL_WIDGET(spinner)->type = EWL_WIDGET_SPINNER;

    ewl_callback_append(EWL_WIDGET(spinner),
        EWL_CALLBACK_REALIZE, __ewl_spinner_realize, NULL);
    ewl_callback_append(EWL_WIDGET(spinner),
        EWL_CALLBACK_SHOW, __ewl_spinner_show, NULL);
    ewl_callback_append(EWL_WIDGET(spinner),
        EWL_CALLBACK_HIDE, __ewl_spinner_hide, NULL);
    ewl_callback_append(EWL_WIDGET(spinner),
        EWL_CALLBACK_DESTROY, __ewl_spinner_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(spinner),
		EWL_CALLBACK_CONFIGURE, __ewl_spinner_configure, NULL);

	spinner->min_val = -50.000;
	spinner->max_val =  50.000;
	spinner->value   =   0.000;
	spinner->digits  =   2;

	EWL_OBJECT(spinner)->current.w = 150;
	EWL_OBJECT(spinner)->current.h = 15;
	EWL_OBJECT(spinner)->maximum.w = 400;
	EWL_OBJECT(spinner)->maximum.h = 15;
	EWL_OBJECT(spinner)->minimum.w = 100;
	EWL_OBJECT(spinner)->minimum.h = 15;
	EWL_OBJECT(spinner)->request.w = 150;
	EWL_OBJECT(spinner)->request.h = 15;
}

static void
__ewl_spinner_realize(Ewl_Widget * widget, void * func_data)
{
	Ewl_Widget * entry, * button_increase, * button_decrease;

	CHECK_PARAM_POINTER("widget", widget);

	widget->container.children = ewd_list_new();

	entry = ewl_entry_new();
	ewl_container_append_child(widget, entry);
	ewl_widget_realize(entry);

	button_increase = ewl_button_new();
	ewl_container_append_child(widget, button_increase);
	EWL_OBJECT(button_increase)->minimum.w = 10;
	EWL_OBJECT(button_increase)->minimum.h = 10;
	ewl_widget_realize(button_increase);

	button_decrease = ewl_button_new();
	ewl_container_append_child(widget, button_decrease);
	EWL_OBJECT(button_decrease)->minimum.w = 10;
	EWL_OBJECT(button_decrease)->minimum.h = 10;
	ewl_widget_realize(button_decrease);
}

static void
__ewl_spinner_show(Ewl_Widget * widget, void * func_data)
{
	Ewl_Widget * child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ewl_fx_clip_box_create(widget);

	widget->container.clip_box = widget->fx_clip_box;
	evas_set_clip(widget->evas, widget->fx_clip_box,
				widget->parent->container.clip_box);

	ewd_list_goto_first(widget->container.children);

	while ((child = ewd_list_next(widget->container.children)) != NULL)
		ewl_widget_show(child);

	__ewl_spinner_set_value(widget, EWL_SPINNER(widget)->value);

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);
}

static void
__ewl_spinner_hide(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
__ewl_spinner_destroy(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
__ewl_spinner_configure(Ewl_Widget * widget, void * func_data)
{
	Ewl_Widget * child;
	int x = 0, y = 0, w = 50;

	CHECK_PARAM_POINTER("widget", widget);

	ewd_list_goto_first(widget->container.children);

	while ((child = ewd_list_next(widget->container.children)) != NULL)
	  {
		if (child->type == EWL_WIDGET_ENTRY)
		  {
			EWL_OBJECT(child)->request.x = EWL_OBJECT(widget)->request.x;
			EWL_OBJECT(child)->request.y = EWL_OBJECT(widget)->request.y;
			EWL_OBJECT(child)->request.w = EWL_OBJECT(widget)->request.w - 12;
			ewl_widget_configure(child);
			x = EWL_OBJECT(widget)->request.x + EWL_OBJECT(child)->current.w +2;
			y = EWL_OBJECT(widget)->request.y;
			w = EWL_OBJECT(child)->current.w;
		  }
		else if (child->type == EWL_WIDGET_BUTTON)
		  {
			EWL_OBJECT(child)->request.x = x;
			EWL_OBJECT(child)->request.y = y;
			EWL_OBJECT(child)->request.w = 10;
			EWL_OBJECT(child)->request.h = 10;
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
__ewl_spinner_set_value(Ewl_Widget * widget, double value)
{
	Ewl_Widget * entry;
	char * format;
	char * str;

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

	entry = widget->container.children->first->data;

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
