
#include <Ewl.h>

/*\
|*|
|*| The button widget is really a merge of many diffrent button types.
|*| Normal - You put whatever widget you want in here
|*| Label - You specify label for button..
|*|
\*/

void __ewl_button_realize (Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_button_remove_label (Ewl_Button * b);
void __ewl_button_update_label (Ewl_Button * b);

void __ewl_button_focus_in (Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_button_focus_out (Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_button_mouse_down (Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_button_mouse_up (Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_button_theme_update (Ewl_Widget * w, void *ev_data,
				void *user_data);


Ewl_Widget *
ewl_button_new (char *label)
{
	Ewl_Button *b;

	DENTER_FUNCTION;

	b = NEW (Ewl_Button, 1);
	if (!b)
		return NULL;

	memset (b, 0, sizeof (Ewl_Button));

	ewl_button_init (b, label);

	DRETURN_PTR (EWL_WIDGET (b));
}

void
ewl_button_init (Ewl_Button * b, char *label)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;

	w = EWL_WIDGET (b);

	ewl_box_init (EWL_BOX (b), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_set_appearance (w, "/appearance/button/default");

	/*
	 * Override the default recursive setting on containers. This prevents
	 * the coordinate->object mapping from searching below the button
	 * class.
	 */
	w->recursive = FALSE;

	/*
	 * Attach necessary callback mechanisms
	 */
	ewl_callback_append (w, EWL_CALLBACK_REALIZE,
			     __ewl_button_realize, NULL);
	ewl_callback_append (w, EWL_CALLBACK_MOUSE_DOWN,
			     __ewl_button_mouse_down, NULL);
	ewl_callback_append (w, EWL_CALLBACK_MOUSE_UP,
			     __ewl_button_mouse_up, NULL);
	ewl_callback_append (w, EWL_CALLBACK_FOCUS_IN,
			     __ewl_button_focus_in, NULL);
	ewl_callback_append (w, EWL_CALLBACK_FOCUS_OUT,
			     __ewl_button_focus_out, NULL);
	ewl_callback_append (w, EWL_CALLBACK_THEME_UPDATE,
			     __ewl_button_theme_update, NULL);

	if (label)
	  {
		  b->label = strdup (label);
		  b->label_object = ewl_text_new ();
		  ewl_text_set_text (b->label_object, label);

		  ewl_object_set_alignment (EWL_OBJECT (b->label_object),
					    EWL_ALIGNMENT_CENTER);
		  ewl_container_append_child (EWL_CONTAINER (b),
					      b->label_object);
	  }

	DLEAVE_FUNCTION;
}

/*
 * Change the label of the specified button
 */
void
ewl_button_set_label (Ewl_Button * b, char *l)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("b", b);

	if (!l)
	  {
		  __ewl_button_remove_label (b);
		  DRETURN;
	  }

	IF_FREE (b->label);
	b->label = strdup (l);
	__ewl_button_update_label (b);

	ewl_widget_configure (EWL_WIDGET (b));
	DLEAVE_FUNCTION;
}

void
__ewl_button_realize (Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Button *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	b = EWL_BUTTON (w);

	if (b->label_object)
	  {
		  void *tmp;

		  tmp = ewl_theme_data_get (w,
					    "/appearance/button/default/text/font");
		  if (tmp)
			  ewl_text_set_font (b->label_object, tmp);

		  tmp = ewl_theme_data_get (w,
					    "/appearance/button/default/text/style");
		  if (tmp)
			  ewl_text_set_style (b->label_object, tmp);

		  tmp = ewl_theme_data_get (b->label_object,
					    "/appearance/button/default/text/font_size");


		  if (tmp)
			  ewl_text_set_font_size (b->label_object,
						  (int) (tmp));

		  ewl_widget_show (b->label_object);
	  }

	DLEAVE_FUNCTION;
}


void
__ewl_button_focus_in (Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	if (w->state & EWL_STATE_PRESSED)
		ewl_widget_update_appearance (w, "clicked");
	else
		ewl_widget_update_appearance (w, "hilited");

	DLEAVE_FUNCTION;
}

void
__ewl_button_focus_out (Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	ewl_widget_update_appearance (w, "normal");

	DLEAVE_FUNCTION;
}

void
__ewl_button_mouse_down (Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	ewl_widget_update_appearance (w, "clicked");

	DLEAVE_FUNCTION;
}

void
__ewl_button_mouse_up (Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);


	if (w->state & EWL_STATE_HILITED)
	  {
		  ewl_widget_update_appearance (w, "hilited");
		  ewl_callback_call (w, EWL_CALLBACK_CLICKED);
	  }
	else
		ewl_widget_update_appearance (w, "normal");

	DLEAVE_FUNCTION;
}

void
__ewl_button_theme_update (Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	/*
	 * Don't want to update anything if the widget isn't realized. 
	 */
	if (!REALIZED (w))
		DRETURN;

	__ewl_button_update_label (EWL_BUTTON (w));

	DLEAVE_FUNCTION;
}

void
__ewl_button_update_label (Ewl_Button * b)
{
	void *tmp;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("b", b);

	if (!b->label)
		DRETURN;

	if (VISIBLE (b))
		ewl_widget_show (b->label_object);
	else if (REALIZED (b))
		ewl_widget_realize (b->label_object);

	/*
	 * Retrieve theme information
	 */
	tmp = ewl_theme_data_get (EWL_WIDGET (b), "text/font");

	if (tmp)
	  {
		  ewl_text_set_font (b->label_object, tmp);
		  FREE (tmp);
	  }
	FREE (tmp);

	tmp = ewl_theme_data_get (EWL_WIDGET (b), "text/font_size");

	/*
	 * Apply theme info to the text
	 */
	if (tmp)
		ewl_text_set_font_size (b->label_object, (int) (tmp));

	ewl_text_set_text (b->label_object, b->label);
	ewl_widget_show (b->label_object);

	DLEAVE_FUNCTION;
}

void
__ewl_button_remove_label (Ewl_Button * b)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("b", b);

	if (!b->label)
		DRETURN;

	if (!b->label_object)
		DRETURN;

	ewd_list_goto_first (EWL_CONTAINER (b)->children);

	if (ewd_list_goto (EWL_CONTAINER (b)->children, b->label_object))
		ewd_list_remove (EWL_CONTAINER (b)->children);

	ewl_widget_destroy (b->label_object);

	DLEAVE_FUNCTION;
}
