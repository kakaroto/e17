
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

	EWL_OBJECT(widget)->realized = TRUE;
	ewl_callback_call(widget, Ewl_Callback_Realize);
}

void
ewl_widget_show(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_OBJECT(widget)->realized)
		ewl_widget_realize(widget);

	if (widget->parent)
		ewl_widget_configure(widget->parent);

	ewl_callback_call(widget, Ewl_Callback_Show);
	EWL_OBJECT(widget)->visible = TRUE;
}

void
ewl_widget_hide(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_callback_call(widget, Ewl_Callback_Hide);
	EWL_OBJECT(widget)->visible = FALSE;
}

void
ewl_widget_destroy(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_callback_call(widget, Ewl_Callback_Destroy);
	EWL_OBJECT(widget)->realized = FALSE;
}

void
ewl_widget_configure(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_callback_call(widget, Ewl_Callback_Configure);
}

void
ewl_widget_set_ebit(Ewl_Widget * widget, char * ebit)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("ebit", ebit);

    if (widget->ebits_object) {
        ebits_hide(widget->ebits_object);
        ebits_free(widget->ebits_object);
    }

	widget->ebits_object = ebits_load(ebit);
	ebits_add_to_evas(widget->ebits_object, widget->evas);
	ebits_set_layer(widget->ebits_object, EWL_OBJECT(widget)->layer);

	if (EWL_OBJECT(widget)->visible)
		ebits_show(widget->ebits_object);
	ebits_move(widget->ebits_object,
			EWL_OBJECT(widget)->current.x, EWL_OBJECT(widget)->current.y);
	ebits_resize(widget->ebits_object,
			EWL_OBJECT(widget)->current.w, EWL_OBJECT(widget)->current.h);

	if (widget->parent) {
		evas_set_clip(widget->evas, widget->container.clip_box, widget->parent->container.clip_box);
		ebits_set_clip(widget->ebits_object, widget->parent->container.clip_box);
	}
}
