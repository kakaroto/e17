
#include <Ewl.h>

void ewl_entry_init(Ewl_Entry * e);
void __ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_focus_in(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_focus_out(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_key_up(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_theme_update(Ewl_Widget * w, void * ev_data, void * user_data);

Ewl_Widget *
ewl_entry_new(void)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;

	e = NEW(Ewl_Entry, 1);
	if (!e)
		return NULL;

	memset(e, 0, sizeof(Ewl_Entry));
	ewl_entry_init(e);

	e->text = ewl_text_new();

	DRETURN_PTR(EWL_WIDGET(e));
}

void
ewl_entry_set_text(Ewl_Widget * w, char *t)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_text_set_text(e->text, t);

	DLEAVE_FUNCTION;
}

char *
ewl_entry_get_text(Ewl_Widget * w)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	e = EWL_ENTRY(w);

	DRETURN_PTR(ewl_text_get_text(e->text));
}

void
ewl_entry_init(Ewl_Entry * e)
{
	Ewl_Widget * w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("e", e);

	w = EWL_WIDGET(e);

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_set_appearance(w, "/appearance/entry/default");
	ewl_object_set_minimum_size(EWL_OBJECT(w), 17, 17);
	ewl_object_set_maximum_size(EWL_OBJECT(w), 1 << 30, 17);

	w->recursive = FALSE;

	/* Attach necessary callback mechanisms */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_entry_realize, NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
                            __ewl_entry_focus_in, NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
                            __ewl_entry_focus_out, NULL);
        ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN,
                            __ewl_entry_key_down, NULL);
        ewl_callback_append(w, EWL_CALLBACK_KEY_UP,
                            __ewl_entry_key_up, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_entry_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
			    __ewl_entry_mouse_up, NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_entry_theme_update, NULL);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_container_append_child(EWL_CONTAINER(w), e->text);
	ewl_widget_show(e->text);

	DLEAVE_FUNCTION;
}


void
__ewl_entry_focus_in(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_focus_out(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_key_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_theme_update(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Entry * e;
	char * font, * style;
	int size;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	font = ewl_theme_data_get(w,"/appearance/entry/default/text/font");
	size = ewl_theme_data_get(w,"/appearance/entry/default/text/font_size");
	style = ewl_theme_data_get(w,"/appearance/entry/default/text/style");

	if (font) ewl_text_set_font(e->text, font);
	if (size) ewl_text_set_font_size(e->text, size);
	if (style) ewl_text_set_style(e->text, style);

	DLEAVE_FUNCTION;
}
