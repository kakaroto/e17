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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	t->hide = FALSE;

	ewl_callback_append (parent, EWL_CALLBACK_FOCUS_IN, 
			ewl_tooltip_parent_focus_in, t);
	ewl_callback_append (parent, EWL_CALLBACK_FOCUS_OUT,
			ewl_tooltip_parent_focus_out, t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_tooltip_parent_focus_in(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	printf ("mouse in on parent\n");
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_tooltip_parent_focus_out(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	printf ("mouse out on parent\n");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
	
