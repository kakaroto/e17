
#include <Ewl.h>

static void ewl_button_init(Ewl_Widget * widget, Ewl_Button_Type type);
static void ewl_button_realize(Ewl_Widget * widget, void * func_data);
static void ewl_button_show(Ewl_Widget * widget, void * func_data);
static void ewl_button_hide(Ewl_Widget * widget, void * func_data);
static void ewl_button_destroy(Ewl_Widget * widget, void * func_data);
static void ewl_button_configure(Ewl_Widget * widget, void * func_data);
static void ewl_button_key_down(Ewl_Widget * widget, void * func_data);
static void ewl_button_key_up(Ewl_Widget * widget, void * func_data);
static void ewl_button_mouse_down(Ewl_Widget * widget, void * func_data);
static void ewl_button_mouse_up(Ewl_Widget * widget, void * func_data);
static void ewl_button_focus_in(Ewl_Widget * widget, void * func_data);
static void ewl_button_focus_out(Ewl_Widget * widget, void * func_data);


static void ewl_button_set_state(Ewl_Widget * widget, void * func_data);


Ewl_Widget *
ewl_button_new(Ewl_Button_Type type)
{
	Ewl_Button * button = NULL;

	button = NEW(Ewl_Button, 1);

	ewl_button_init(EWL_WIDGET(button), type);

	return EWL_WIDGET(button);
}

static void
ewl_button_init(Ewl_Widget * widget, Ewl_Button_Type type)
{
	CHECK_PARAM_POINTER("widget", widget);

	/* Zero out bogus values before adding anything */
	memset(EWL_BUTTON(widget), 0, sizeof(Ewl_Button));

	/* Add necessery callback's */
	ewl_callback_append(widget, Ewl_Callback_Realize,
							ewl_button_realize, NULL);
	ewl_callback_append(widget, Ewl_Callback_Show,
							ewl_button_show, NULL);
	ewl_callback_append(widget, Ewl_Callback_Hide,
							ewl_button_hide, NULL);
	ewl_callback_append(widget, Ewl_Callback_Destroy,
							ewl_button_destroy, NULL);
	ewl_callback_append(widget, Ewl_Callback_Configure,
							ewl_button_configure, NULL);
	ewl_callback_append(widget, Ewl_Callback_Key_Down,
							ewl_button_key_down, NULL);
	ewl_callback_append(widget, Ewl_Callback_Key_Up,
							ewl_button_key_up, NULL);
	ewl_callback_append(widget, Ewl_Callback_Mouse_Down,
							ewl_button_mouse_down, NULL);
	ewl_callback_append(widget, Ewl_Callback_Mouse_Up,
							ewl_button_mouse_up, NULL);
	ewl_callback_append(widget, Ewl_Callback_Focus_In,
							ewl_button_focus_in, NULL);
	ewl_callback_append(widget, Ewl_Callback_Focus_Out,
							ewl_button_focus_out, NULL);

	/* Do this so the button's child wont get any events */
	widget->container.recursive = FALSE;

	EWL_BUTTON(widget)->type = type;

	EWL_OBJECT(widget)->current.w = 85;
	EWL_OBJECT(widget)->current.h = 35;
	EWL_OBJECT(widget)->minimum.w = 85;
	EWL_OBJECT(widget)->minimum.h = 35;
	EWL_OBJECT(widget)->maximum.w = 256;
	EWL_OBJECT(widget)->maximum.h = 256;
	EWL_OBJECT(widget)->request.w = 85;
	EWL_OBJECT(widget)->request.h = 35;
}

static void
ewl_button_realize(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	/* Lets do only this for now */
	EWL_BUTTON(widget)->state = Ewl_Button_State_Normal;

	ewl_widget_set_ebit(widget,ewl_theme_ebit_get("button", "default", "base"));

	ewl_container_new(widget);
}

static void
ewl_button_show(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	/* Prehaps show it's parent automatically ?
	 * But that sounds more like the ewl_widget_show's responsibility */
	ebits_show(widget->ebits_object);

	ewl_container_set_clip(widget);
}

static void
ewl_button_hide(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	/* Maybe hide children to ? */
	ebits_hide(widget->ebits_object);
}

static void
ewl_button_destroy(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_free(widget->ebits_object);

	FREE(EWL_BUTTON(widget));
}

static void
ewl_button_configure(Ewl_Widget * widget, void * func_data)
{
	Ewl_Widget * child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;

	if (!widget->container.children || !widget->container.children->nodes) {
		EWL_OBJECT(widget)->request.w = 85;
		EWL_OBJECT(widget)->request.h = 35;
	} else {
		int w = 6, h = 6, x, y;
		int l, r, t, b;
		ewd_list_goto_first(widget->container.children);

		ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

		x = EWL_OBJECT(widget)->current.x + l + 2;
		y = EWL_OBJECT(widget)->current.y + t + 2;

		while ((child = ewd_list_next(widget->container.children)) != NULL) {
			EWL_OBJECT(child)->request.x = x;
			EWL_OBJECT(child)->request.y = y;
			ewl_widget_configure(child);
			x += EWL_OBJECT(child)->current.w + 3;

			w += EWL_OBJECT(child)->current.w;
			h += EWL_OBJECT(child)->current.h;
		}
		EWL_OBJECT(widget)->request.w = w + 3;
		EWL_OBJECT(widget)->request.h = h;
	}

	ebits_move(widget->ebits_object,
		EWL_OBJECT(widget)->request.x, EWL_OBJECT(widget)->request.y);

	ebits_resize(widget->ebits_object,
		EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);

	ewl_container_clip_box_resize(widget);

	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;
	EWL_OBJECT(widget)->minimum.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->minimum.h = EWL_OBJECT(widget)->request.h;
	EWL_OBJECT(widget)->maximum.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->maximum.h = EWL_OBJECT(widget)->request.h;
}

static void
ewl_button_key_down(Ewl_Widget * widget, void * func_data)
{
	Ev_Key_Down * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ev = func_data;

	if (!strcmp(ev->key, "Return")) {
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Clicked);
	} else if (!strcmp(ev->key, "XP_Enter")) {
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Clicked);
	} else if (!strcmp(ev->key, "Space")) {
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Clicked);
	}
}

static void
ewl_button_key_up(Ewl_Widget * widget, void * func_data)
{
	Ev_Key_Up * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ev = func_data;

	if (!strcmp(ev->key, "Return")) {
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Normal);
	} else if (!strcmp(ev->key, "XP_Enter")) {
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Normal);
	} else if (!strcmp(ev->key, "Space")) {
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Normal);
	}
}

static void
ewl_button_mouse_down(Ewl_Widget * widget, void * func_data)
{
	Ev_Mouse_Down * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ev = func_data;

	if (ev->button == 1) {
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Clicked);
	} else if (ev->button == 2) {
	} else if (ev->button == 3) {
	}
}

static void
ewl_button_mouse_up(Ewl_Widget * widget, void * func_data)
{
	Ev_Mouse_Up * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ev = func_data;

    if (ev->button == 1) {
		if (ev->x >= EWL_OBJECT(widget)->current.x &&
			ev->y >= EWL_OBJECT(widget)->current.y &&
			EWL_OBJECT(widget)->current.x +
							EWL_OBJECT(widget)->current.w >= ev->x &&
			EWL_OBJECT(widget)->current.y +
							EWL_OBJECT(widget)->current.h >= ev->y)
			ewl_button_set_state(widget, (void *) Ewl_Button_State_Hilited);
		else
	        ewl_button_set_state(widget, (void *) Ewl_Button_State_Normal);
    } else if (ev->button == 2) {
    } else if (ev->button == 3) {
    }
}

static void
ewl_button_set_state(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_BUTTON(widget)->type == Ewl_Button_Type_Normal) {
		if ((Ewl_Button_State) func_data == Ewl_Button_State_Normal) {
			EWL_BUTTON(widget)->state = Ewl_Button_State_Normal;
			ewl_widget_set_ebit(widget,
					ewl_theme_ebit_get("button", "default", "base"));
			ewl_callback_call(widget, Ewl_Callback_Released);
		} else if ((Ewl_Button_State) func_data == Ewl_Button_State_Hilited) {
			EWL_BUTTON(widget)->state = Ewl_Button_State_Hilited;
			ewl_widget_set_ebit(widget,
					ewl_theme_ebit_get("button", "default", "hilited"));
			ewl_callback_call(widget, Ewl_Callback_Hilited);
		} else if ((Ewl_Button_State) func_data == Ewl_Button_State_Clicked) {
			EWL_BUTTON(widget)->state = Ewl_Button_State_Clicked;
			ewl_widget_set_ebit(widget,
					ewl_theme_ebit_get("button", "default", "clicked"));
			ewl_callback_call(widget, Ewl_Callback_Clicked);
		}
	}
}

static void
ewl_button_focus_in(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_BUTTON(widget)->state != Ewl_Button_State_Clicked)
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Hilited);
}

static void
ewl_button_focus_out(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_BUTTON(widget)->state != Ewl_Button_State_Clicked)
		ewl_button_set_state(widget, (void *) Ewl_Button_State_Normal);
}
