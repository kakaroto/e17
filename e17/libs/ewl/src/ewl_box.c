
#include <Ewl.h>

static void ewl_box_init(Ewl_Widget * widget, Ewl_Box_Type type);
static void ewl_box_realize(Ewl_Widget * widget, void * func_data);
static void ewl_box_show(Ewl_Widget * widget, void * func_data);
static void ewl_box_hide(Ewl_Widget * widget, void * func_data);
static void ewl_box_destroy(Ewl_Widget * widget, void * func_data);
static void ewl_box_configure(Ewl_Widget * widget, void * func_data);


Ewl_Widget *
ewl_box_new(Ewl_Box_Type type)
{
	Ewl_Box * box = NULL;

	box = malloc(sizeof(Ewl_Box));

	if (!box)
		return NULL;

	ewl_box_init(EWL_WIDGET(box), type);

	return EWL_WIDGET(box);
}

static void
ewl_box_init(Ewl_Widget * widget, Ewl_Box_Type type)
{
	CHECK_PARAM_POINTER("widget", widget);

	memset(EWL_BOX(widget), 0, sizeof(Ewl_Box));

	EWL_BOX(widget)->type = type;

	EWL_BOX(widget)->spacing = 5;

	widget->container.recursive = TRUE;

	ewl_callback_append(widget, Ewl_Callback_Realize,
							ewl_box_realize, NULL);
	ewl_callback_append(widget, Ewl_Callback_Show,
							ewl_box_show, NULL);
	ewl_callback_append(widget, Ewl_Callback_Hide,
							ewl_box_hide, NULL);
	ewl_callback_append(widget, Ewl_Callback_Destroy,
							ewl_box_destroy, NULL);
	ewl_callback_append(widget, Ewl_Callback_Configure,
							ewl_box_configure, NULL);

	EWL_OBJECT(widget)->current.w = 10;
	EWL_OBJECT(widget)->current.h = 10;
	EWL_OBJECT(widget)->maximum.w = 2024;
	EWL_OBJECT(widget)->maximum.h = 2024;
	EWL_OBJECT(widget)->minimum.w = 10;
	EWL_OBJECT(widget)->minimum.h = 10;
	EWL_OBJECT(widget)->request.w = 10;
	EWL_OBJECT(widget)->request.h = 10;
}

static void
ewl_box_realize(Ewl_Widget * widget, void * func_data)
{
	if (EWL_BOX(widget)->type == Ewl_Box_Type_Horisontal)
		ewl_widget_set_ebit(widget,
				ewl_theme_ebit_get("box", "horisontal", "base"));
	else
		ewl_widget_set_ebit(widget,
				ewl_theme_ebit_get("box", "vertical", "base"));

	return;
	func_data = NULL;
}

static void
ewl_box_show(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_show(widget->ebits_object);
}

static void
ewl_box_hide(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(widget->ebits_object);
}

static void
ewl_box_destroy(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(widget->ebits_object);
	ebits_free(widget->ebits_object);	
}

static void
ewl_box_configure(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_OBJECT(widget)->realized) {
		ebits_move(widget->ebits_object, EWL_OBJECT(widget)->request.x,
										 EWL_OBJECT(widget)->request.y);
		ebits_resize(widget->ebits_object, EWL_OBJECT(widget)->request.w,
										   EWL_OBJECT(widget)->request.h);
		ewl_widget_clip_box_resize(widget);
	}

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;

	if (!widget->container.children || !widget->container.children->nodes)
		return;

	if (EWL_BOX(widget)->type == Ewl_Box_Type_Horisontal) {
		Ewl_Widget * child;
		int w, x, l, r, t, b;

		ewd_list_goto_first(widget->container.children);

		if (EWL_OBJECT(widget)->realized) {
			ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);
		} else {
			l = 0;
			r = 0;
			t = 0;
			b = 0;
		}

		w = EWL_OBJECT(widget)->current.w / widget->container.children->nodes;
		x = EWL_OBJECT(widget)->current.x + l;

	while ((child = ewd_list_next(widget->container.children)) != NULL) {
		EWL_OBJECT(child)->request.x = x;
		EWL_OBJECT(child)->request.y = EWL_OBJECT(widget)->current.y + t;
		EWL_OBJECT(child)->request.w = w - (l + r);
		EWL_OBJECT(child)->request.h = EWL_OBJECT(widget)->current.h - (t + b);

        if (EWL_OBJECT(child)->request.w < EWL_OBJECT(child)->minimum.w) {
			EWL_OBJECT(child)->request.x += (EWL_OBJECT(child)->request.w / 2) -
											(EWL_OBJECT(child)->minimum.w / 2);
            EWL_OBJECT(child)->request.w = EWL_OBJECT(child)->minimum.w;
		}

        if (EWL_OBJECT(child)->request.h < EWL_OBJECT(child)->minimum.h) {
			EWL_OBJECT(child)->request.y += (EWL_OBJECT(child)->request.h / 2) -
											(EWL_OBJECT(child)->minimum.h / 2);
            EWL_OBJECT(child)->request.h = EWL_OBJECT(child)->minimum.h;
		}

        if (EWL_OBJECT(child)->request.w > EWL_OBJECT(child)->maximum.w) {
			EWL_OBJECT(child)->request.x += (EWL_OBJECT(child)->request.w / 2) -
											(EWL_OBJECT(child)->maximum.w / 2);
			EWL_OBJECT(child)->request.w = EWL_OBJECT(child)->maximum.w;
		}

        if (EWL_OBJECT(child)->request.h > EWL_OBJECT(child)->maximum.h) {
			EWL_OBJECT(child)->request.y += (EWL_OBJECT(child)->request.h / 2) -
											(EWL_OBJECT(child)->maximum.h / 2);
            EWL_OBJECT(child)->request.h = EWL_OBJECT(child)->maximum.h;
		}

		if (EWL_OBJECT(child)->request.y < EWL_OBJECT(widget)->current.y + t)
			EWL_OBJECT(child)->request.y = EWL_OBJECT(widget)->current.y + t;

		x += w - (l + r) + EWL_BOX(widget)->spacing;

		ewl_widget_configure(child);

	}
	} else if (EWL_BOX(widget)->type == Ewl_Box_Type_Vertical) {
		Ewl_Widget * child;
		int h, y, l, r, t, b;

		ewd_list_goto_first(widget->container.children);

		if (EWL_OBJECT(widget)->realized) {
			ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);
		} else {
			l = 0;
			r = 0;
			t = 0;
			b = 0;
		}

		h = EWL_OBJECT(widget)->current.h / widget->container.children->nodes;
		y = EWL_OBJECT(widget)->current.y + t;

	while ((child = ewd_list_next(widget->container.children)) != NULL) {
		EWL_OBJECT(child)->request.x = EWL_OBJECT(widget)->current.x + l;
		EWL_OBJECT(child)->request.y = y;
		EWL_OBJECT(child)->request.w = EWL_OBJECT(widget)->current.w - (l + r);
		EWL_OBJECT(child)->request.h = h - (t + b);

		if (EWL_OBJECT(child)->request.w < EWL_OBJECT(child)->minimum.w) {
			EWL_OBJECT(child)->request.x += (EWL_OBJECT(child)->request.w / 2) -
											(EWL_OBJECT(child)->minimum.w / 2);
			EWL_OBJECT(child)->request.w = EWL_OBJECT(child)->minimum.w;
		}

		if (EWL_OBJECT(child)->request.h < EWL_OBJECT(child)->minimum.h) {
			EWL_OBJECT(child)->request.y += (EWL_OBJECT(child)->request.h / 2) -
											(EWL_OBJECT(child)->minimum.h / 2);
			EWL_OBJECT(child)->request.h = EWL_OBJECT(child)->minimum.h;
		}

		if (EWL_OBJECT(child)->request.w > EWL_OBJECT(child)->maximum.w) {
			EWL_OBJECT(child)->request.x += (EWL_OBJECT(child)->request.w / 2) -
											(EWL_OBJECT(child)->maximum.w / 2);
			EWL_OBJECT(child)->request.w = EWL_OBJECT(child)->maximum.w;
		}

		if (EWL_OBJECT(child)->request.h > EWL_OBJECT(child)->maximum.h) {
			EWL_OBJECT(child)->request.y += (EWL_OBJECT(child)->request.h / 2) -
											(EWL_OBJECT(child)->maximum.h / 2);
			EWL_OBJECT(child)->request.h = EWL_OBJECT(child)->maximum.h;
		}

		if (EWL_OBJECT(child)->request.x < EWL_OBJECT(widget)->current.x + l)
			EWL_OBJECT(child)->request.x = EWL_OBJECT(widget)->current.x + l;

		y += h - (t + b) + EWL_BOX(widget)->spacing;

		ewl_widget_configure(child);
	}
	}
}

void
ewl_box_set_type(Ewl_Widget * widget, Ewl_Box_Type type)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_BOX(widget)->type = type;
	ewl_callback_call(widget, Ewl_Callback_Configure);
}

void
ewl_box_set_spacing(Ewl_Widget * widget, unsigned int spacing)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_BOX(widget)->spacing = spacing;
	ewl_callback_call(widget, Ewl_Callback_Configure);
}

void
ewl_box_set_homogeneous(Ewl_Widget * widget, unsigned int homogeneous)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_BOX(widget)->homogeneous = homogeneous;
	ewl_callback_call(widget, Ewl_Callback_Configure);
}
