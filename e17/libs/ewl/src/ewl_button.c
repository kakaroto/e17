
#include <Ewl.h>

static void ewl_button_init(Ewl_Widget * widget, Ewl_Button_Type type);
static void ewl_button_realize(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_show(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_hide(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_destroy(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_configure(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_key_down(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_key_up(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_mouse_down(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_mouse_up(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_focus_in(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_focus_out(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_select(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_button_unselect(Ewl_Widget * widget, Ewl_Callback * cb);

static void ewl_button_set_state(Ewl_Widget * widget, void * func_data);


Ewl_Widget *
ewl_button_new()
{
	Ewl_Button * button = NULL;

	button = NEW(Ewl_Button, 1);

	ewl_button_init(EWL_WIDGET(button), EWL_BUTTON_TYPE_NORMAL);

	return EWL_WIDGET(button);
}

Ewl_Widget *
ewl_button_new_with_label(const char * label)
{
	Ewl_Widget * button;
	Ewl_Widget * text;

	button = ewl_button_new();
	EWL_BUTTON(button)->type = EWL_BUTTON_TYPE_LABEL;

	text = ewl_text_new();
	ewl_text_set_text(text, label);
	ewl_text_set_font_size(text, 8);
	ewl_container_append_child(button, text);

	return button;
}

static void
ewl_button_init(Ewl_Widget * widget, Ewl_Button_Type type)
{
	CHECK_PARAM_POINTER("widget", widget);

	memset(EWL_BUTTON(widget), 0, sizeof(Ewl_Button));

	EWL_WIDGET(widget)->type = EWL_WIDGET_BUTTON;

	ewl_callback_append(widget, EWL_CALLBACK_REALIZE,
							ewl_button_realize, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SHOW,
							ewl_button_show, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_HIDE,
							ewl_button_hide, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_DESTROY,
							ewl_button_destroy, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_CONFIGURE,
							ewl_button_configure, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_KEY_DOWN,
							ewl_button_key_down, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_KEY_UP,
							ewl_button_key_up, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_MOUSE_DOWN,
							ewl_button_mouse_down, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_MOUSE_UP,
							ewl_button_mouse_up, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_FOCUS_IN,
							ewl_button_focus_in, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_FOCUS_OUT,
							ewl_button_focus_out, NULL);

	widget->container.recursive = FALSE;

	EWL_BUTTON(widget)->type = type;

	EWL_OBJECT(widget)->current.w = 80;
	EWL_OBJECT(widget)->current.h = 35;
	EWL_OBJECT(widget)->minimum.w = 80;
	EWL_OBJECT(widget)->minimum.h = 35;
	EWL_OBJECT(widget)->maximum.w = 80;
	EWL_OBJECT(widget)->maximum.h = 35;
	EWL_OBJECT(widget)->request.w = 85;
	EWL_OBJECT(widget)->request.h = 35;
}

static void
ewl_button_realize(Ewl_Widget * widget, Ewl_Callback * cb)
{
	char * image = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_BUTTON(widget)->type == EWL_BUTTON_TYPE_CHECK)
		image = ewl_theme_ebit_get("button", "check", "base");
	else if (EWL_BUTTON(widget)->type == EWL_BUTTON_TYPE_NORMAL ||
			EWL_BUTTON(widget)->type == EWL_BUTTON_TYPE_LABEL)
		image = ewl_theme_ebit_get("button", "default", "base");
	else if (EWL_BUTTON(widget)->type == EWL_BUTTON_TYPE_RADIO)
		image = ewl_theme_ebit_get("button", "radio", "base");

	EWL_BUTTON(widget)->ebits_object = ebits_load(image);
	FREE(image);
	ebits_add_to_evas(EWL_BUTTON(widget)->ebits_object, widget->evas);
	ebits_set_layer(EWL_BUTTON(widget)->ebits_object, widget->object.layer);

	ewl_fx_clip_box_create(widget);

	if (widget->container.children)
	  {
		Ewl_Widget * child;

		ewd_list_goto_first(widget->container.children);

		while ((child = ewd_list_next(widget->container.children)) != NULL)
			ewl_widget_realize(child);
	  }

	ebits_show(EWL_BUTTON(widget)->ebits_object);

    if (widget->parent && widget->parent->container.clip_box)
      {
        evas_set_clip(widget->evas, widget->fx_clip_box,
                    widget->parent->container.clip_box);
        ebits_set_clip(EWL_BUTTON(widget)->ebits_object,
                    widget->fx_clip_box);
        evas_set_clip(widget->evas, widget->container.clip_box,
                    widget->fx_clip_box);
      }

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);
}

static void
ewl_button_show(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (widget->container.children)
	  {
		Ewl_Widget * child;

		ewd_list_goto_first(widget->container.children);

		while ((child = ewd_list_next(widget->container.children)) != NULL)
		  {
			ewl_widget_show(child);
			EWL_OBJECT(widget)->minimum.w = EWL_OBJECT(child)->current.w + 10;
			EWL_OBJECT(widget)->minimum.h = EWL_OBJECT(child)->current.h + 10;
			EWL_OBJECT(widget)->maximum.w = EWL_OBJECT(child)->current.w + 10;
			EWL_OBJECT(widget)->maximum.h = EWL_OBJECT(child)->current.h + 10;
		  }
	  }

	evas_show(widget->evas, widget->fx_clip_box);
}

static void
ewl_button_hide(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_hide(widget->evas, widget->fx_clip_box);
}

static void
ewl_button_destroy(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(EWL_BUTTON(widget)->ebits_object);
	ebits_unset_clip(EWL_BUTTON(widget)->ebits_object);
	ebits_free(EWL_BUTTON(widget)->ebits_object);

	evas_hide(widget->evas, widget->fx_clip_box);
	evas_unset_clip(widget->evas, widget->fx_clip_box);
	evas_del_object(widget->evas, widget->fx_clip_box);

	evas_hide(widget->evas, widget->fx_clip_box);
	evas_unset_clip(widget->evas, widget->container.clip_box);
	evas_del_object(widget->evas, widget->container.clip_box);

	FREE(EWL_BUTTON(widget));
}

static void
ewl_button_configure(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ewl_Widget * child = NULL;
	int l = 0, r = 0, t = 0, b = 0;

	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;

	if (EWL_BUTTON(widget)->ebits_object)
		ebits_get_insets(EWL_BUTTON(widget)->ebits_object, &l, &r, &t, &b);

	if (!widget->container.children || !widget->container.children->nodes) {
		EWL_OBJECT(widget)->minimum.w = 80;
		EWL_OBJECT(widget)->minimum.h = 35;
		EWL_OBJECT(widget)->maximum.w = 80;
		EWL_OBJECT(widget)->maximum.h = 35;
		EWL_OBJECT(widget)->request.w = EWL_OBJECT(widget)->minimum.w;
		EWL_OBJECT(widget)->request.h = EWL_OBJECT(widget)->minimum.h;
	} else {
		int w = 6, h = 6, x, y;

		ewd_list_goto_first(widget->container.children);

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


        if (EWL_BUTTON(widget)->type == EWL_BUTTON_TYPE_LABEL)
          {
			child = widget->container.children->first->data;
            EWL_OBJECT(widget)->minimum.w = 130;
            EWL_OBJECT(widget)->maximum.w = 100;
			EWL_OBJECT(child)->request.x =
				EWL_OBJECT(widget)->current.x;
			EWL_OBJECT(child)->request.x +=
				(130 / 2)-(EWL_OBJECT(child)->current.w /2);
			ewl_widget_configure(child);
          }

	if (EWL_OBJECT(widget)->custom.w)
	  {
		EWL_OBJECT(widget)->minimum.w = EWL_OBJECT(widget)->custom.w;
		EWL_OBJECT(widget)->maximum.w = EWL_OBJECT(widget)->custom.w;
		EWL_OBJECT(widget)->request.w = EWL_OBJECT(widget)->custom.w;

		if (EWL_BUTTON(widget)->type == EWL_BUTTON_TYPE_LABEL)
		  {
			EWL_OBJECT(child)->request.x = EWL_OBJECT(widget)->current.x + l +2;
			ewl_widget_configure(child);
		  }
			
	  }

	if (EWL_OBJECT(widget)->custom.h)
	  {
        EWL_OBJECT(widget)->minimum.h = EWL_OBJECT(widget)->custom.h;
        EWL_OBJECT(widget)->maximum.h = EWL_OBJECT(widget)->custom.h;
        EWL_OBJECT(widget)->request.h = EWL_OBJECT(widget)->custom.h;
	  }

	if (EWL_BUTTON(widget)->ebits_object)
	  {
		if (EWL_OBJECT(widget)->request.w > EWL_OBJECT(widget)->maximum.w)
			EWL_OBJECT(widget)->request.w = EWL_OBJECT(widget)->maximum.w;

		if (EWL_OBJECT(widget)->request.w < EWL_OBJECT(widget)->minimum.w)
			EWL_OBJECT(widget)->request.w = EWL_OBJECT(widget)->minimum.w;
		ebits_move(EWL_BUTTON(widget)->ebits_object,
			EWL_OBJECT(widget)->request.x, EWL_OBJECT(widget)->request.y);

		ebits_resize(EWL_BUTTON(widget)->ebits_object,
			EWL_OBJECT(widget)->request.w, EWL_OBJECT(widget)->request.h);

		ewl_fx_clip_box_resize(widget);
	  }

	if (widget->container.clip_box)
	  {
		evas_move(widget->evas, widget->container.clip_box,
					EWL_OBJECT(widget)->current.x + l,
					EWL_OBJECT(widget)->current.y + t);
		evas_resize(widget->evas, widget->container.clip_box,
					EWL_OBJECT(widget)->current.w - (l+r),
					EWL_OBJECT(widget)->current.h - (t+b));
	  }


	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;
	EWL_OBJECT(widget)->minimum.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->minimum.h = EWL_OBJECT(widget)->request.h;
	EWL_OBJECT(widget)->maximum.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->maximum.h = EWL_OBJECT(widget)->request.h;
}

static void
ewl_button_key_down(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ev_Key_Down * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ev = cb->func_data;

	if (!strcmp(ev->key, "Return")) {
		ewl_button_set_state(widget, (void *) EWL_STATE_PRESSED);
	} else if (!strcmp(ev->key, "XP_Enter")) {
		ewl_button_set_state(widget, (void *) EWL_STATE_PRESSED);
	} else if (!strcmp(ev->key, "Space")) {
		ewl_button_set_state(widget, (void *) EWL_STATE_PRESSED);
	}
}

static void
ewl_button_key_up(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ev_Key_Up * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ev = cb->func_data;

	if (!strcmp(ev->key, "Return")) {
		ewl_button_set_state(widget, (void *) EWL_STATE_NORMAL);
	} else if (!strcmp(ev->key, "XP_Enter")) {
		ewl_button_set_state(widget, (void *) EWL_STATE_NORMAL);
	} else if (!strcmp(ev->key, "Space")) {
		ewl_button_set_state(widget, (void *) EWL_STATE_NORMAL);
	}
}

static void
ewl_button_mouse_down(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ev_Mouse_Down * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ev = cb->func_data;

	if (ev->button == 1) {
		ewl_button_set_state(widget, (void *) EWL_STATE_PRESSED);
	} else if (ev->button == 2) {
	} else if (ev->button == 3) {
	}
}

static void
ewl_button_mouse_up(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ev_Mouse_Up * ev = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	ev = cb->func_data;

    if (ev->button == 1) {
		if (ev->x >= EWL_OBJECT(widget)->current.x &&
			ev->y >= EWL_OBJECT(widget)->current.y &&
			EWL_OBJECT(widget)->current.x +
							EWL_OBJECT(widget)->current.w >= ev->x &&
			EWL_OBJECT(widget)->current.y +
							EWL_OBJECT(widget)->current.h >= ev->y)
			ewl_button_set_state(widget, (void *) EWL_STATE_HILITED);
		else
	        ewl_button_set_state(widget, (void *) EWL_STATE_NORMAL);
    } else if (ev->button == 2) {
    } else if (ev->button == 3) {
    }
}

static void
ewl_button_set_state(Ewl_Widget * widget, void * func_data)
{
	char * image = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_BUTTON(widget)->type == EWL_BUTTON_TYPE_NORMAL ||
		EWL_BUTTON(widget)->type == EWL_BUTTON_TYPE_LABEL)
	  {
		if ((Ewl_State) func_data == EWL_STATE_NORMAL)
		  {
			image = ewl_theme_ebit_get("button", "default", "base");
		  }
		else if ((Ewl_State) func_data == EWL_STATE_HILITED)
		  {
			image = ewl_theme_ebit_get("button", "default", "hilited");
		  }
		else if ((Ewl_State) func_data == EWL_STATE_PRESSED)
		  {
			image = ewl_theme_ebit_get("button", "default", "clicked");
		  }
		ebits_hide(EWL_BUTTON(widget)->ebits_object);
		ebits_free(EWL_BUTTON(widget)->ebits_object);
		EWL_BUTTON(widget)->ebits_object = ebits_load(image);
		ebits_add_to_evas(EWL_BUTTON(widget)->ebits_object, widget->evas);
		ebits_set_layer(EWL_BUTTON(widget)->ebits_object,
							EWL_OBJECT(widget)->layer);
		if (EWL_OBJECT(widget)->visible)
			ebits_show(EWL_BUTTON(widget)->ebits_object);
		ebits_set_clip(EWL_BUTTON(widget)->ebits_object,
							widget->fx_clip_box);
		ewl_widget_configure(widget);
		IF_FREE(image);

		if ((Ewl_State) func_data == EWL_STATE_NORMAL)
		  {
			if (EWL_BUTTON(widget)->state == EWL_STATE_PRESSED)
				ewl_callback_call(widget, EWL_CALLBACK_CLICKED);
			EWL_BUTTON(widget)->state = EWL_STATE_NORMAL;
		  }
		else if ((Ewl_State) func_data == EWL_STATE_HILITED)
		  {
			if (EWL_BUTTON(widget)->state == EWL_STATE_PRESSED)
				ewl_callback_call(widget, EWL_CALLBACK_CLICKED);
			EWL_BUTTON(widget)->state = EWL_STATE_HILITED;
		  }
		else if ((Ewl_State) func_data == EWL_STATE_PRESSED)
			EWL_BUTTON(widget)->state = EWL_STATE_PRESSED;
	  }
}

static void
ewl_button_focus_in(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_BUTTON(widget)->state != EWL_STATE_PRESSED)
		ewl_button_set_state(widget, (void *) EWL_STATE_HILITED);
}

static void
ewl_button_focus_out(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_BUTTON(widget)->state != EWL_STATE_PRESSED)
		ewl_button_set_state(widget, (void *) EWL_STATE_NORMAL);
}

static void
ewl_button_select(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	
}

static void
ewl_button_unselect(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
}
