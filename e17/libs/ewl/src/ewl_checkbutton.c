
#include <Ewl.h>


void ewl_checkbutton_init(Ewl_CheckButton * cb, char *label);

void __ewl_checkbutton_configure(Ewl_Widget * w, void * ev_data,
				void * user_data);
void __ewl_checkbutton_mouse_down(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void __ewl_checkbutton_update_check(Ewl_Widget * w);

void __ewl_box_configure(Ewl_Widget * w, void *ev_data, void *user_data);


Ewl_Widget *
ewl_checkbutton_new(char *label)
{
	Ewl_CheckButton *b;

	DENTER_FUNCTION;

	b = NEW(Ewl_CheckButton, 1);
	if (!b)
		return NULL;

	memset(b, 0, sizeof(Ewl_CheckButton));
	ewl_checkbutton_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b));
}

void
ewl_checkbutton_init(Ewl_CheckButton * cb, char *label)
{
	Ewl_Button * b;
	Ewl_Widget * w;

	DENTER_FUNCTION;

	b = EWL_BUTTON(cb);
	w = EWL_WIDGET(cb);

	ewl_button_init(b, label);
	ewl_widget_set_appearance(w, "/appearance/button/check");

	ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, __ewl_box_configure);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			__ewl_checkbutton_configure, NULL);

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			__ewl_checkbutton_mouse_down, NULL);

	DLEAVE_FUNCTION;
}


void
__ewl_checkbutton_configure(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Button * b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BUTTON(w);

	if (b->label_object)
	  {
		REQUEST_X(b->label_object) = CURRENT_X(w) + 17;

		if (MINIMUM_H(b->label_object) > CURRENT_H(w))
		  {
			REQUEST_Y(b->label_object) = CURRENT_Y(w) + 17;
			MAXIMUM_H(w) = MINIMUM_H(b->label_object);
		  }
		else
		  {
			REQUEST_Y(b->label_object) = CURRENT_Y(w);
			REQUEST_Y(b->label_object) += (CURRENT_H(w) / 2) -
					(CURRENT_H(b->label_object) / 2);
			MAXIMUM_H(w) = 17;
		  }

		MAXIMUM_W(w) = 17 + CURRENT_W(b->label_object);

		ewl_widget_configure(b->label_object);
	  }
	else
	  {
		MAXIMUM_W(w) = 17;
		MAXIMUM_H(w) = 17;
	  }

	DLEAVE_FUNCTION;
}

void
__ewl_checkbutton_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton * cb;
	int oc;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);
	oc = cb->checked;

	cb->checked ^= 1;

	__ewl_checkbutton_update_check(w);

	if (oc != cb->checked)
		ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION;
}

void
__ewl_checkbutton_update_check(Ewl_Widget * w)
{
	Ewl_CheckButton * cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	cb = EWL_CHECKBUTTON(w);

	if (w->ebits_object)
	  {
		if (cb->checked)
			ebits_set_named_bit_state(w->ebits_object, "Check","clicked");
		else
			ebits_set_named_bit_state(w->ebits_object, "Check", "normal");
	  }

	DLEAVE_FUNCTION;
}
