
#include <Ewl.h>


void ewl_radiobutton_init (Ewl_RadioButton * cb, char *label);

void __ewl_radiobutton_mouse_down (Ewl_Widget * w, void *ev_data,
				   void *user_data);
void __ewl_radiobutton_theme_update (Ewl_Widget * w, void *ev_data,
				     void *user_data);
void __ewl_radiobutton_update_check (Ewl_Widget * w);

void __ewl_checkbutton_mouse_down (Ewl_Widget * w, void *ev_data,
				   void *user_data);


void ewl_checkbutton_init (Ewl_CheckButton * b, char *l);

Ewl_Widget *
ewl_radiobutton_new (char *label)
{
	Ewl_RadioButton *b;

	DENTER_FUNCTION;

	b = NEW (Ewl_RadioButton, 1);
	if (!b)
		return NULL;

	memset (b, 0, sizeof (Ewl_RadioButton));
	ewl_radiobutton_init (b, label);

	DRETURN_PTR (EWL_WIDGET (b));
}

void
ewl_radiobutton_set_chain (Ewl_Widget * w, Ewl_Widget * c)
{
	Ewl_RadioButton *rb, *crb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);
	DCHECK_PARAM_PTR ("c", c);

	rb = EWL_RADIOBUTTON (w);
	crb = EWL_RADIOBUTTON (c);

	/* If a chain doesnt exist, create one */
	if (!crb->chain)
	  {
		  crb->chain = ewd_list_new ();

		  rb->chain = crb->chain;

		  ewd_list_append (crb->chain, w);
		  ewd_list_append (crb->chain, c);
	  }
	else
	  {
		  rb->chain = crb->chain;

		  if (!ewd_list_goto (crb->chain, w))
			  ewd_list_append (crb->chain, w);
	  }

	DLEAVE_FUNCTION;
}

void
ewl_radiobutton_init (Ewl_RadioButton * rb, char *label)
{
	Ewl_CheckButton *cb;
	Ewl_Widget *w;

	DENTER_FUNCTION;

	cb = EWL_CHECKBUTTON (rb);
	w = EWL_WIDGET (rb);

	ewl_checkbutton_init (cb, label);
	ewl_widget_set_appearance (w, "/appearance/button/radio");

	ewl_callback_del (w, EWL_CALLBACK_MOUSE_DOWN,
			  __ewl_checkbutton_mouse_down);
	ewl_callback_append (w, EWL_CALLBACK_MOUSE_DOWN,
			     __ewl_radiobutton_mouse_down, NULL);

	DLEAVE_FUNCTION;
}

void
__ewl_radiobutton_mouse_down (Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_CheckButton *cb;
	Ewl_RadioButton *rb;
	int oc;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	cb = EWL_CHECKBUTTON (w);
	rb = EWL_RADIOBUTTON (w);
	oc = cb->checked;

	if (rb->chain && !ewd_list_is_empty (rb->chain))
	  {
		  Ewl_CheckButton *c;

		  ewd_list_goto_first (rb->chain);

		  while ((c = ewd_list_next (rb->chain)) != NULL)
		    {
			    c->checked = 0;

			    __ewl_radiobutton_update_check (EWL_WIDGET (c));
		    }
	  }

	cb->checked = 1;

	__ewl_radiobutton_update_check (w);

	if (oc != cb->checked)
		ewl_callback_call (w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION;
}

void
__ewl_radiobutton_update_check (Ewl_Widget * w)
{
	Ewl_CheckButton *cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	cb = EWL_CHECKBUTTON (w);

	if (w->ebits_object)
	  {
		  if (cb->checked)
			  ebits_set_named_bit_state (w->ebits_object, "Check",
						     "clicked");
		  else
			  ebits_set_named_bit_state (w->ebits_object, "Check",
						     "normal");
	  }

	DLEAVE_FUNCTION;
}
