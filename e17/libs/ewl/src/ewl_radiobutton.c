
#include <Ewl.h>


void ewl_radiobutton_init(Ewl_RadioButton * cb, char *label);

void __ewl_radiobutton_mouse_down(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void __ewl_radiobutton_theme_update(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void __ewl_radiobutton_update_check(Ewl_Widget * w);

void __ewl_checkbutton_mouse_down(Ewl_Widget * w,void *ev_data,void *user_data);
				    

Ewl_Widget *
ewl_radiobutton_new(char *label)
{
	Ewl_RadioButton *b;

	DENTER_FUNCTION;

	b = NEW(Ewl_RadioButton, 1);
	if (!b)
		return NULL;

	memset(b, 0, sizeof(Ewl_RadioButton));
	ewl_radiobutton_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b));
}

void
ewl_radiobutton_init(Ewl_RadioButton * rb, char *label)
{
	Ewl_CheckButton * cb;
	Ewl_Widget * w;

	DENTER_FUNCTION;

	cb = EWL_CHECKBUTTON(rb);
	w = EWL_WIDGET(rb);

	ewl_checkbutton_init(cb, label);
	ewl_widget_set_appearance(w, "/appearance/button/radio");

	ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN, __ewl_checkbutton_mouse_down);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			__ewl_radiobutton_mouse_down, NULL);

	DLEAVE_FUNCTION;
}

void
__ewl_radiobutton_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton * cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	cb->checked = 1;

	__ewl_radiobutton_update_check(w);

	DLEAVE_FUNCTION;
}

void
__ewl_radiobutton_update_check(Ewl_Widget * w)
{
	Ewl_CheckButton * cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	if (w->ebits_object)
	  {
		if (cb->checked)
			ebits_set_bit_state(w->ebits_object, "Check","clicked");
		else
			ebits_set_bit_state(w->ebits_object, "Check", "normal");
	  }

	DLEAVE_FUNCTION;
}
