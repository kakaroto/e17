
#include <Ewl.h>

static Ewl_Widget * ewl_box_new(Ewl_Orientation orientation);
static void ewl_box_init(Ewl_Box * box, Ewl_Orientation orientation);
static void ewl_box_realize(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_box_show(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_box_hide(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_box_destroy(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_box_configure(Ewl_Widget * widget, Ewl_Callback * cb);

static Ewl_Widget *
ewl_box_new(Ewl_Orientation orientation)
{
	Ewl_Box * box = NULL;

	box = NEW(Ewl_Box, 1);

	ewl_box_init(box, orientation);

	return EWL_WIDGET(box);
}

Ewl_Widget *
ewl_hbox_new(void)
{
	return ewl_box_new(EWL_ORIENTATION_HORISONTAL);
}

Ewl_Widget *
ewl_vbox_new(void)
{
	return ewl_box_new(EWL_ORIENTATION_VERTICAL);
}

void
ewl_box_set_orientation(Ewl_Widget * widget, Ewl_Orientation orientation)
{
    CHECK_PARAM_POINTER("widget", widget);

    EWL_BOX(widget)->orientation = orientation;

    ewl_widget_configure(widget);
}

void
ewl_box_set_spacing(Ewl_Widget * widget, unsigned int spacing)
{
    CHECK_PARAM_POINTER("widget", widget);

    EWL_BOX(widget)->spacing = spacing;

    ewl_widget_configure(widget);
}

void
ewl_box_set_homogeneous(Ewl_Widget * widget, unsigned int homogeneous)
{
    CHECK_PARAM_POINTER("widget", widget);

    EWL_BOX(widget)->homogeneous = homogeneous;

    ewl_widget_configure(widget);
}


static void
ewl_box_init(Ewl_Box * box, Ewl_Orientation orientation)
{
	CHECK_PARAM_POINTER("box", box);

	memset(box, 0, sizeof(Ewl_Box));

	ewl_callback_append(EWL_WIDGET(box),
		EWL_CALLBACK_REALIZE, ewl_box_realize, NULL);
	ewl_callback_append(EWL_WIDGET(box),
		EWL_CALLBACK_SHOW, ewl_box_show, NULL);
	ewl_callback_append(EWL_WIDGET(box),
		EWL_CALLBACK_HIDE, ewl_box_hide, NULL);
	ewl_callback_append(EWL_WIDGET(box),
		EWL_CALLBACK_DESTROY, ewl_box_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(box),
		EWL_CALLBACK_CONFIGURE, ewl_box_configure, NULL);

	EWL_WIDGET(box)->container.recursive = TRUE;

	box->orientation = orientation;
	box->spacing = 5;
	box->homogeneous = 0;

	EWL_OBJECT(box)->current.w = 10;
	EWL_OBJECT(box)->current.h = 10;
	EWL_OBJECT(box)->maximum.w = 2024;
	EWL_OBJECT(box)->maximum.h = 2024;
	EWL_OBJECT(box)->minimum.w = 10;
	EWL_OBJECT(box)->minimum.h = 10;
	EWL_OBJECT(box)->request.w = 10;
	EWL_OBJECT(box)->request.h = 10;
}

static void
ewl_box_realize(Ewl_Widget * widget, Ewl_Callback * cb)
{
	char * image = NULL;

	if (EWL_BOX(widget)->orientation == EWL_ORIENTATION_HORISONTAL)
		image = ewl_theme_ebit_get("box", "horisontal", "base");
	else if (EWL_BOX(widget)->orientation == EWL_ORIENTATION_VERTICAL)
		image = ewl_theme_ebit_get("box", "vertical", "base");

	EWL_BOX(widget)->ebits_object = ebits_load(image);
	IF_FREE(image);
	ebits_add_to_evas(EWL_BOX(widget)->ebits_object, widget->evas);
	ebits_set_layer(EWL_BOX(widget)->ebits_object, widget->object.layer);

	ewl_fx_clip_box_create(widget);

	ebits_show(EWL_BOX(widget)->ebits_object);

	if (widget->parent && widget->parent->container.clip_box)
	  {
        evas_set_clip(widget->evas, widget->fx_clip_box,
                    widget->parent->container.clip_box);
        ebits_set_clip(EWL_BOX(widget)->ebits_object,
                    widget->fx_clip_box);
        evas_set_clip(widget->evas, widget->container.clip_box,
                    widget->fx_clip_box);
      }

    evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);
}

static void
ewl_box_show(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_show(widget->evas, widget->fx_clip_box);
}

static void
ewl_box_hide(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_hide(widget->evas, widget->fx_clip_box);
}

static void
ewl_box_destroy(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(EWL_BOX(widget)->ebits_object);
	ebits_unset_clip(EWL_BOX(widget)->ebits_object);
	ebits_free(EWL_BOX(widget)->ebits_object);

	evas_hide(widget->evas, widget->fx_clip_box);
	evas_unset_clip(widget->evas, widget->fx_clip_box);
	evas_del_object(widget->evas, widget->fx_clip_box);

	evas_hide(widget->evas, widget->container.clip_box);
	evas_unset_clip(widget->evas, widget->container.clip_box);
	evas_del_object(widget->evas, widget->container.clip_box);

	FREE(widget);
}

static void
ewl_box_configure(Ewl_Widget * widget, Ewl_Callback * cb)
{
	int l = 0, r = 0, t = 0, b = 0;

	CHECK_PARAM_POINTER("widget", widget);

    EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
    EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
    EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
    EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;

	ebits_get_insets(EWL_BOX(widget)->ebits_object, &l, &r, &t, &b);

	if (EWL_OBJECT(widget)->realized)
	  {
		ebits_move(EWL_BOX(widget)->ebits_object,
			EWL_OBJECT(widget)->request.x,
			EWL_OBJECT(widget)->request.y);
		ebits_resize(EWL_BOX(widget)->ebits_object,
			EWL_OBJECT(widget)->request.w,
			EWL_OBJECT(widget)->request.h);
		ewl_fx_clip_box_resize(widget);
		if (widget->container.clip_box)
		  {
			evas_move(widget->evas, widget->container.clip_box,
					EWL_OBJECT(widget)->current.x + l,
					EWL_OBJECT(widget)->current.y + t);
			evas_resize(widget->evas, widget->container.clip_box,
					EWL_OBJECT(widget)->current.w - (l+r),
					EWL_OBJECT(widget)->current.h - (t+b));
		  }
	  }

	if (!widget->container.children || !widget->container.children->nodes)
		return;

	if (EWL_BOX(widget)->orientation == EWL_ORIENTATION_HORISONTAL)
	  {
		Ewl_Widget * child;
		int w = 0, x = 0;

		ewd_list_goto_first(widget->container.children);

		w = EWL_OBJECT(widget)->current.w / widget->container.children->nodes;
		x = EWL_OBJECT(widget)->current.x + l;

		while ((child = ewd_list_next(widget->container.children)) != NULL)
		  {
		EWL_OBJECT(child)->request.x = x;
		EWL_OBJECT(child)->request.y = EWL_OBJECT(widget)->current.y + t;
		EWL_OBJECT(child)->request.w = w - (l + r);
		EWL_OBJECT(child)->request.h = EWL_OBJECT(widget)->current.h - (t + b);

        if (EWL_OBJECT(child)->request.w < EWL_OBJECT(child)->minimum.w)
		  {
			EWL_OBJECT(child)->request.x +=
								(EWL_OBJECT(child)->request.w / 2) -
								(EWL_OBJECT(child)->minimum.w / 2);
            EWL_OBJECT(child)->request.w = EWL_OBJECT(child)->minimum.w;
		  }

        if (EWL_OBJECT(child)->request.h < EWL_OBJECT(child)->minimum.h)
		  {
			EWL_OBJECT(child)->request.y += (EWL_OBJECT(child)->request.h / 2) -
											(EWL_OBJECT(child)->minimum.h / 2);
            EWL_OBJECT(child)->request.h = EWL_OBJECT(child)->minimum.h;
		  }

        if (EWL_OBJECT(child)->request.w > EWL_OBJECT(child)->maximum.w)
		  {
			EWL_OBJECT(child)->request.x += (EWL_OBJECT(child)->request.w / 2) -
											(EWL_OBJECT(child)->maximum.w / 2);
			EWL_OBJECT(child)->request.w = EWL_OBJECT(child)->maximum.w;
		  }

        if (EWL_OBJECT(child)->request.h > EWL_OBJECT(child)->maximum.h)
		  {
			EWL_OBJECT(child)->request.y += (EWL_OBJECT(child)->request.h / 2) -
											(EWL_OBJECT(child)->maximum.h / 2);
            EWL_OBJECT(child)->request.h = EWL_OBJECT(child)->maximum.h;
		  }

		if (EWL_OBJECT(child)->request.y < EWL_OBJECT(widget)->current.y + t)
			EWL_OBJECT(child)->request.y = EWL_OBJECT(widget)->current.y + t;

		x += w - (l + r) + EWL_BOX(widget)->spacing;

		ewl_widget_configure(child);

		  }
	  }
	else if (EWL_BOX(widget)->orientation == EWL_ORIENTATION_VERTICAL)
	  {
		Ewl_Widget * child;
		int h = 0;
		int y = 0;

		ewd_list_goto_first(widget->container.children);

		h = EWL_OBJECT(widget)->current.h / widget->container.children->nodes;
		y = EWL_OBJECT(widget)->current.y + t;

		while ((child = ewd_list_next(widget->container.children)) != NULL)
		  {
		EWL_OBJECT(child)->request.x = EWL_OBJECT(widget)->current.x + l;
		EWL_OBJECT(child)->request.y = y;
		EWL_OBJECT(child)->request.w = EWL_OBJECT(widget)->current.w - (l + r);
		EWL_OBJECT(child)->request.h = h - (t + b);

		if (EWL_OBJECT(child)->request.w < EWL_OBJECT(child)->minimum.w)
		  {
			EWL_OBJECT(child)->request.x += (EWL_OBJECT(child)->request.w / 2) -
											(EWL_OBJECT(child)->minimum.w / 2);
			EWL_OBJECT(child)->request.w = EWL_OBJECT(child)->minimum.w;
		  }

		if (EWL_OBJECT(child)->request.h < EWL_OBJECT(child)->minimum.h)
		  {
			EWL_OBJECT(child)->request.y += (EWL_OBJECT(child)->request.h / 2) -
											(EWL_OBJECT(child)->minimum.h / 2);
			EWL_OBJECT(child)->request.h = EWL_OBJECT(child)->minimum.h;
		  }

		if (EWL_OBJECT(child)->request.w > EWL_OBJECT(child)->maximum.w)
		  {
			EWL_OBJECT(child)->request.x += (EWL_OBJECT(child)->request.w / 2) -
											(EWL_OBJECT(child)->maximum.w / 2);
			EWL_OBJECT(child)->request.w = EWL_OBJECT(child)->maximum.w;
		  }

		if (EWL_OBJECT(child)->request.h > EWL_OBJECT(child)->maximum.h)
		  {
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
