
#include <Ewl.h>

Ewl_Widget *
ewl_widget_new()
{
	Ewl_Widget * widget = NULL;

	widget = malloc(sizeof(Ewl_Widget));

	if (!widget)
		return NULL;

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

	if (!EWL_OBJECT(widget)->realized) {
		ewl_callback_call(widget, Ewl_Callback_Realize);
		EWL_OBJECT(widget)->realized = TRUE;
	}
	
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

	if (!widget->container.clip_box)
		ewl_widget_clip_box_create(widget);

	if (widget->parent) {
		evas_set_clip(widget->evas, widget->container.clip_box, widget->parent->container.clip_box);
		ebits_set_clip(widget->ebits_object, widget->parent->container.clip_box);
	}
}

void
ewl_widget_clip_box_create(Ewl_Widget * widget)
{
	if (!widget->container.clip_box) {
		int l = 0, r = 0, t = 0, b = 0;

		if (widget->ebits_object)
			ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

        widget->container.clip_box = evas_add_rectangle(widget->evas);
		evas_set_color(widget->evas, widget->container.clip_box,
												255, 255, 255, 255);
		evas_move(widget->evas, widget->container.clip_box,
			EWL_OBJECT(widget)->current.x +l, EWL_OBJECT(widget)->current.y +t);
		evas_resize(widget->evas, widget->container.clip_box,
			EWL_OBJECT(widget)->current.w - (l+r),
			EWL_OBJECT(widget)->current.h - (t+b));
		evas_set_layer(widget->evas, widget->container.clip_box,
						EWL_OBJECT(widget)->layer);
		evas_show(widget->evas, widget->container.clip_box);
    }
}

void
ewl_widget_clip_box_resize(Ewl_Widget * widget)
{
	if (widget->container.clip_box) {
        int l = 0, r = 0, t = 0, b = 0;

		if (widget->ebits_object)
	        ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

        evas_move(widget->evas, widget->container.clip_box,           
            EWL_OBJECT(widget)->request.x +l, EWL_OBJECT(widget)->request.y +t);
        evas_resize(widget->evas, widget->container.clip_box,
            EWL_OBJECT(widget)->request.w - (l+r),
            EWL_OBJECT(widget)->request.h - (t+b));
	}
}
