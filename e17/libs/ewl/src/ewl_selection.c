
#include <Ewl.h>


void            ewl_selection_init(Ewl_Selection * s);


Ewl_Widget     *ewl_selection_new(void)
{
	Ewl_Selection  *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Selection, 1);

	ZERO(s, Ewl_Selection, 1);

	ewl_selection_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

void ewl_selection_set_covered(Ewl_Widget * w, int s, int e)
{
	Ewl_Selection  *se;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	se = EWL_SELECTION(w);

	se->start_pos = s;
	se->end_pos = e;

	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_selection_get_covered(Ewl_Widget * w, int *s, int *e)
{
	Ewl_Selection  *se;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	se = EWL_SELECTION(w);

	if (s)
		*s = se->start_pos;
	if (e)
		*e = se->end_pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_selection_expand(Ewl_Widget * w, int p)
{
	Ewl_Selection  *se;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	se = EWL_SELECTION(w);

	if (p < se->start_pos)
		se->start_pos = p;
	else if (p > se->end_pos)
		se->end_pos = p;
	else
		DRETURN(DLEVEL_STABLE);

	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_selection_init(Ewl_Selection * s)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_widget_init(w, "selection");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
