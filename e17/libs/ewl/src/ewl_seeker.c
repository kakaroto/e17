
#include <Ewl.h>


static Ewl_Widget * ewl_seeker_new(Ewl_Orientation orientation);
static void __ewl_seeker_init(Ewl_Seeker * Seeker, Ewl_Orientation orientation);
static void __ewl_seeker_realize(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_show(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_hide(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_destroy(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_configure(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_set_value(Ewl_Widget * widget, int value);

static void __ewl_seeker_key_down(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_key_up(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_mouse_down(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_mouse_up(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_mouse_move(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_focus_in(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_focus_out(Ewl_Widget * widget, void * func_data);
static void __ewl_seeker_get_button_geometry(Ewl_Widget * widget,
											 int * x,
											 int * y,
											 int * w,
											 int * h);


static Ewl_Widget *
ewl_seeker_new(Ewl_Orientation orientation)
{
	Ewl_Seeker * seeker = NULL;

	seeker = NEW(Ewl_Seeker, 1);

	__ewl_seeker_init(seeker, orientation);

	return EWL_WIDGET(seeker);
}

Ewl_Widget *
ewl_hseeker_new(void)
{
	return ewl_seeker_new(EWL_ORIENTATION_HORISONTAL);
}

Ewl_Widget *
ewl_vseeker_new(void)
{
	return ewl_seeker_new(EWL_ORIENTATION_VERTICAL);
}

Ewl_Widget *
ewl_seeker_new_all(Ewl_Orientation orientation, int range)
{
	Ewl_Widget * widget;

	widget = ewl_seeker_new(orientation);
	EWL_SEEKER(widget)->range = range;

	return widget;
}

void
ewl_seeker_set_value(Ewl_Widget * widget, int value)
{
	CHECK_PARAM_POINTER("widget", widget);

	__ewl_seeker_set_value(widget, value);
}

int
ewl_seeker_get_value(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, 0);

	return EWL_SEEKER(widget)->value;
}

void
ewl_seeker_set_range(Ewl_Widget * widget, int range)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_SEEKER(widget)->range = range;
	__ewl_seeker_set_value(widget, EWL_SEEKER(widget)->value);
}

int
ewl_seeker_get_range(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, 0);

	return EWL_SEEKER(widget)->range;
}

void
ewl_seeker_set_orientation(Ewl_Widget * widget, Ewl_Orientation orientation)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_SEEKER(widget)->orientation = orientation;
    if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_HORISONTAL)
      {
        EWL_OBJECT(widget)->current.w = 150;
        EWL_OBJECT(widget)->current.h = 15;
        EWL_OBJECT(widget)->maximum.w = 150;
        EWL_OBJECT(widget)->maximum.h = 15;
        EWL_OBJECT(widget)->minimum.w = 150;
        EWL_OBJECT(widget)->minimum.h = 15;
        EWL_OBJECT(widget)->request.w = 150;
        EWL_OBJECT(widget)->request.h = 15;
      }
    else if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_VERTICAL)
      {
        EWL_OBJECT(widget)->current.w = 15;
        EWL_OBJECT(widget)->current.h = 150;
        EWL_OBJECT(widget)->maximum.w = 15;
        EWL_OBJECT(widget)->maximum.h = 150;
        EWL_OBJECT(widget)->minimum.w = 15;
        EWL_OBJECT(widget)->minimum.h = 150;
        EWL_OBJECT(widget)->request.w = 15;
        EWL_OBJECT(widget)->request.h = 150;
      }
	ewl_widget_configure(widget);
	ewl_widget_configure(widget->parent);
}
										   

static void
__ewl_seeker_init(Ewl_Seeker * seeker, Ewl_Orientation orientation)
{
	CHECK_PARAM_POINTER("seeker", seeker);

	memset(seeker, 0, sizeof(Ewl_Seeker));

    ewl_callback_append(EWL_WIDGET(seeker),
        EWL_CALLBACK_REALIZE, __ewl_seeker_realize, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
        EWL_CALLBACK_SHOW, __ewl_seeker_show, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
        EWL_CALLBACK_HIDE, __ewl_seeker_hide, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
        EWL_CALLBACK_DESTROY, __ewl_seeker_destroy, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
        EWL_CALLBACK_CONFIGURE, __ewl_seeker_configure, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
		EWL_CALLBACK_KEY_DOWN, __ewl_seeker_key_down, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
		EWL_CALLBACK_KEY_UP, __ewl_seeker_key_up, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
		EWL_CALLBACK_MOUSE_DOWN, __ewl_seeker_mouse_down, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
		EWL_CALLBACK_MOUSE_UP, __ewl_seeker_mouse_up, NULL);
	ewl_callback_append(EWL_WIDGET(seeker),
		EWL_CALLBACK_MOUSE_MOVE, __ewl_seeker_mouse_move, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
		EWL_CALLBACK_FOCUS_IN, __ewl_seeker_focus_in, NULL);
    ewl_callback_append(EWL_WIDGET(seeker),
		EWL_CALLBACK_FOCUS_OUT, __ewl_seeker_focus_out, NULL);

	EWL_WIDGET(seeker)->container.recursive = FALSE;

	seeker->orientation = orientation;
	seeker->value = 0;
	seeker->range = 100;

	if (seeker->orientation == EWL_ORIENTATION_HORISONTAL)
	  {
		EWL_OBJECT(seeker)->current.w = 150;
		EWL_OBJECT(seeker)->current.h = 15;
		EWL_OBJECT(seeker)->maximum.w = 150;
		EWL_OBJECT(seeker)->maximum.h = 15;
		EWL_OBJECT(seeker)->minimum.w = 150;
		EWL_OBJECT(seeker)->minimum.h = 15;
		EWL_OBJECT(seeker)->request.w = 150;
		EWL_OBJECT(seeker)->request.h = 15;
	  }
	else if (seeker->orientation == EWL_ORIENTATION_VERTICAL)
	  {
		EWL_OBJECT(seeker)->current.w = 15;    
		EWL_OBJECT(seeker)->current.h = 150;    
		EWL_OBJECT(seeker)->maximum.w = 15;    
		EWL_OBJECT(seeker)->maximum.h = 150;    
		EWL_OBJECT(seeker)->minimum.w = 15;    
		EWL_OBJECT(seeker)->minimum.h = 150;    
		EWL_OBJECT(seeker)->request.w = 15;    
		EWL_OBJECT(seeker)->request.h = 150;
	  }
}

static void
__ewl_seeker_realize(Ewl_Widget * widget, void * func_data)
{
	char * image = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	image = ewl_theme_ebit_get("seeker", "default", "base");
	EWL_SEEKER(widget)->ebits_bg = ebits_load(image);
	ebits_add_to_evas(EWL_SEEKER(widget)->ebits_bg, widget->evas);
	ebits_set_layer(EWL_SEEKER(widget)->ebits_bg, widget->object.layer);
	IF_FREE(image);

	image = ewl_theme_ebit_get("seeker", "default", "dragbar");
	EWL_SEEKER(widget)->ebits_dragbar = ebits_load(image);
	ebits_add_to_evas(EWL_SEEKER(widget)->ebits_dragbar, widget->evas);
	ebits_set_layer(EWL_SEEKER(widget)->ebits_dragbar, widget->object.layer+1);
	IF_FREE(image);
}

static void
__ewl_seeker_show(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_fx_clip_box_create(widget);

	ebits_show(EWL_SEEKER(widget)->ebits_bg);
	ebits_show(EWL_SEEKER(widget)->ebits_dragbar);

	if (widget->parent && widget->parent->container.clip_box)
		evas_set_clip(widget->evas, widget->fx_clip_box,
						widget->parent->container.clip_box);


	ebits_set_clip(EWL_SEEKER(widget)->ebits_bg, widget->fx_clip_box);
	ebits_set_clip(EWL_SEEKER(widget)->ebits_dragbar, widget->fx_clip_box);

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);
}

static void
__ewl_seeker_hide(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(EWL_SEEKER(widget)->ebits_bg);
	ebits_hide(EWL_SEEKER(widget)->ebits_dragbar);
}

static void
__ewl_seeker_destroy(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_free(EWL_SEEKER(widget)->ebits_bg);
	ebits_free(EWL_SEEKER(widget)->ebits_dragbar);

	FREE(EWL_SEEKER(widget));
}

static void
__ewl_seeker_configure(Ewl_Widget * widget, void * func_data)
{
	int l = 0, r = 0, t = 0, b = 0;

	CHECK_PARAM_POINTER("widget", widget);

	ebits_get_insets(EWL_SEEKER(widget)->ebits_bg, &l, &r, &t, &b);

	ewl_fx_clip_box_resize(widget);

	ebits_move(EWL_SEEKER(widget)->ebits_bg,
				EWL_OBJECT(widget)->request.x,
				EWL_OBJECT(widget)->request.y);
	ebits_resize(EWL_SEEKER(widget)->ebits_bg,
				 EWL_OBJECT(widget)->request.w,
				 EWL_OBJECT(widget)->request.h);
				 

	ebits_move(EWL_SEEKER(widget)->ebits_dragbar,
				EWL_OBJECT(widget)->request.x + l,
				EWL_OBJECT(widget)->request.y + t);

	if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_HORISONTAL)
		ebits_resize(EWL_SEEKER(widget)->ebits_dragbar, 25,
						EWL_OBJECT(widget)->request.h - (t+b));
	else if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_VERTICAL)
		ebits_resize(EWL_SEEKER(widget)->ebits_dragbar,
					EWL_OBJECT(widget)->request.w - (r+l),
					25);

    EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
    EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
    EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
    EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;

	__ewl_seeker_set_value(widget, EWL_SEEKER(widget)->value);
}

static void
__ewl_seeker_set_value(Ewl_Widget * widget, int value)
{
	int l = 0, r = 0, t = 0, b = 0;
	int x = 0, y = 0;
	float play = 0;

	CHECK_PARAM_POINTER("widget", widget);


	if (widget->object.realized)
	  {
	if (value > EWL_SEEKER(widget)->range)
		return;

	ebits_get_insets(EWL_SEEKER(widget)->ebits_bg, &l, &r, &t, &b);

	x = EWL_OBJECT(widget)->current.x + l;
	y = EWL_OBJECT(widget)->current.y + t;

	play = ((float) value / (float) EWL_SEEKER(widget)->range);

	if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_HORISONTAL)
		x += ((EWL_OBJECT(widget)->current.w - (l + r) - 25) * play);
	else if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_VERTICAL)
		y += ((EWL_OBJECT(widget)->current.h - (t + b) - 25) * (1.000 - play));

	ebits_move(EWL_SEEKER(widget)->ebits_dragbar, x, y);
	  }
	EWL_SEEKER(widget)->value = value;
}

static void
__ewl_seeker_key_down(Ewl_Widget * widget, void * func_data)
{
	Ev_Key_Down * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ev = func_data;

	if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_HORISONTAL)
	  {
		if (!strcmp("Left", ev->key))
			__ewl_seeker_set_value(widget, EWL_SEEKER(widget)->value-1);
		else if (!strcmp("Right", ev->key))
			__ewl_seeker_set_value(widget, EWL_SEEKER(widget)->value+1);
		else if (!strcmp("Home", ev->key))
			__ewl_seeker_set_value(widget, 0);
		else if (!strcmp("End", ev->key))
			__ewl_seeker_set_value(widget, EWL_SEEKER(widget)->range);
	  }
	else if  (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_VERTICAL)
	  {
        if (!strcmp("Up", ev->key))
            __ewl_seeker_set_value(widget, EWL_SEEKER(widget)->value+1);
        else if (!strcmp("Down", ev->key))
            __ewl_seeker_set_value(widget, EWL_SEEKER(widget)->value-1);
        else if (!strcmp("Home", ev->key))
            __ewl_seeker_set_value(widget, 0);
        else if (!strcmp("End", ev->key))
            __ewl_seeker_set_value(widget, EWL_SEEKER(widget)->range);
	  }
}

static void
__ewl_seeker_key_up(Ewl_Widget * widget, void * func_data)
{

}

static void
__ewl_seeker_mouse_down(Ewl_Widget * widget, void * func_data)
{
	Ev_Mouse_Down * ev;
	int x, y, w, h;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ev = func_data;

	__ewl_seeker_get_button_geometry(widget, &x, &y, &w, &h);

	if (ev->x >= x &&
		ev->y >= y &&
		x + w >= ev->x &&
		y + h >= ev->y)
		widget->state = widget->state | EWL_STATE_DND;
}

static void
__ewl_seeker_mouse_up(Ewl_Widget * widget, void * func_data)
{
	Ev_Mouse_Up * ev;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ev = func_data;

	if (widget->state & EWL_STATE_DND)
	  {
		widget->state = widget->state & !EWL_STATE_DND;
	  }
}

static void
__ewl_seeker_mouse_move(Ewl_Widget * widget, void * func_data)
{
	float play;
	int value;
	Ev_Mouse_Move * ev;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ev = func_data;

	if (widget->state & EWL_STATE_DND)
	  {
		if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_HORISONTAL)
		  {
			play = ((float) ev->x - EWL_OBJECT(widget)->current.x) /
						EWL_OBJECT(widget)->current.w;
			value = EWL_SEEKER(widget)->range * play;
			ewl_seeker_set_value(widget, value);
		  }
		else if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_VERTICAL)
		  {
		  }
	  }
}

static void
__ewl_seeker_focus_in(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
__ewl_seeker_focus_out(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
__ewl_seeker_get_button_geometry(Ewl_Widget * widget,
								 int * x,
								 int * y,
								 int * w,
								 int * h)
{
	int l = 0, r = 0, t = 0, b = 0;
	float play = 0;

	CHECK_PARAM_POINTER("widget", widget);

	ebits_get_insets(EWL_SEEKER(widget)->ebits_bg, &l, &r, &t, &b);

    *x = EWL_OBJECT(widget)->current.x + l;
    *y = EWL_OBJECT(widget)->current.y + t;

	play = ((float) EWL_SEEKER(widget)->value /
				(float) EWL_SEEKER(widget)->range);

	if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_HORISONTAL)
	  {
		*w = 25;
		*h = EWL_OBJECT(widget)->current.h - (t+b);
		*x += ((EWL_OBJECT(widget)->current.w - (l + r) - 25) * play);
	  }
	else if (EWL_SEEKER(widget)->orientation == EWL_ORIENTATION_VERTICAL)
	  {
		*w = EWL_OBJECT(widget)->current.w - (l+r);
		*h = 25;
		*y += ((EWL_OBJECT(widget)->current.h - (t + b) - 25) * (1.000 - play));
	  }
}
