
#include <Ewl.h>

Ewl_Widget *
ewl_widget_new()
{
	Ewl_Widget * widget = NULL;

	widget = NEW(Ewl_Widget, 1);

	ewl_widget_init(widget);

	return widget;
}

void
ewl_widget_init(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	memset(widget, 0, sizeof(Ewl_Widget));
}

void
ewl_widget_reparent(Ewl_Widget * widget, Ewl_Widget * parent)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("parent", parent);

	widget->evas = parent->evas;
	widget->evas_window = parent->evas_window;
	widget->parent = parent;
}

void
ewl_widget_realize(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_OBJECT(widget)->realized)
		return;

	if (!widget->evas && widget->parent && widget->parent->evas)
	  {
		EWL_OBJECT(widget)->layer = widget->parent->object.layer + 1;
		widget->evas = widget->parent->evas;
	  }

	EWL_OBJECT(widget)->realized = TRUE;
	ewl_callback_call(widget, EWL_CALLBACK_REALIZE);
}

void
ewl_widget_show(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_OBJECT(widget)->visible)
		return;

	if (!EWL_OBJECT(widget)->realized)
		ewl_widget_realize(widget);

	EWL_OBJECT(widget)->visible = TRUE;
	ewl_callback_call(widget, EWL_CALLBACK_SHOW);
}

void
ewl_widget_hide(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_callback_call(widget, EWL_CALLBACK_HIDE);
	EWL_OBJECT(widget)->visible = FALSE;
}

void
ewl_widget_destroy(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_callback_call(widget, EWL_CALLBACK_DESTROY);
	EWL_OBJECT(widget)->realized = FALSE;
}

void
ewl_widget_configure(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_callback_call(widget, EWL_CALLBACK_CONFIGURE);
}
