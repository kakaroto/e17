#include <Ewl.h>


Ewl_Widget *ewl_tooltip_new (Ewl_Widget *parent)
{
	Ewl_Tooltip *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = NEW(Ewl_Tooltip, 1);
	if (!t)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_tooltip_init (t, parent);

	DRETURN_PTR(EWL_WIDGET(t), DLEVEL_STABLE);
}


void ewl_tooltip_init (Ewl_Tooltip *t, Ewl_Widget *parent)
{
	Ewl_Widget *w;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET (t);

	ewl_floater_init (EWL_FLOATER(w), parent);
	ewl_object_set_fill_policy (EWL_OBJECT(w), EWL_FLAG_FILL_SHRINK |
			EWL_FLAG_FILL_FILL);

	ewl_widget_set_appearance (EWL_WIDGET (w), "tooltip");

	t->text = ewl_text_new ("test text");
	ewl_object_set_alignment (EWL_OBJECT(t->text),
			EWL_FLAG_ALIGN_CENTER);
	ewl_container_append_child (EWL_CONTAINER(w), t->text);
	ewl_widget_show (t->text);

	t->delay = 3.5;
	t->hide = FALSE;
	
	ewl_callback_append (parent, EWL_CALLBACK_FOCUS_IN, 
			ewl_tooltip_parent_focus_in, t);
	ewl_callback_append (parent, EWL_CALLBACK_FOCUS_OUT,
			ewl_tooltip_parent_focus_out, t);

	/* 
	 * If the parent is clicked we don't want to display 
	 * the tooltip.
	 */
	ewl_callback_append (parent, EWL_CALLBACK_MOUSE_DOWN,
			ewl_tooltip_parent_mouse_down, t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_tooltip_set_text (Ewl_Tooltip *t, char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);


	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_tooltip_set_delay (Ewl_Tooltip *t, double delay)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	if (delay > 0) 
		t->delay = delay;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int ewl_tooltip_focus_timer (void *data)
{
	Ewl_Tooltip *t = data;

	if (t->hide)
		return FALSE;

	printf ("Opening tooltip after %lf secs\n", t->delay);
	
	/*
	 * TODO
	 * Show the tooltip here
	 */

	t->timer = NULL;

	return FALSE;
}

void ewl_tooltip_parent_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tooltip *t = user_data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (t->hide)
		return;

	if (t->timer) {
		ecore_timer_del (t->timer);
		t->timer = NULL;
	}

	printf ("Mouse clicked the parent, don't display tooltip\n");
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_tooltip_parent_focus_in(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tooltip *t = user_data;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (t->hide)
		return;

	if (!t->timer) {
		t->timer = ecore_timer_add (t->delay, ewl_tooltip_focus_timer, t);
	}

	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_tooltip_parent_focus_out(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tooltip *t = user_data;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (t->hide)
		return;

	if (t->timer) {
		ecore_timer_del (t->timer);
		t->timer = NULL;
	}

	/*
	 * TODO
	 * hide the tooltip here
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
	
