
#include <Ewl.h>


void ewl_selection_init(Ewl_Selection * s);


Ewl_Widget *
ewl_selection_new(void)
{
	Ewl_Selection *s;

	DENTER_FUNCTION;

	s = NEW(Ewl_Selection, 1);

	memset(s, 0, sizeof(Ewl_Selection));
	ewl_selection_init(s);

	DRETURN_PTR(EWL_WIDGET(s));
}

void
ewl_selection_set_covered(Ewl_Widget * w, int s, int e)
{
	Ewl_Selection *se;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	se = EWL_SELECTION(w);

	se->start_pos = s;
	se->end_pos = e;

	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION;
}

void
ewl_selection_get_position(Ewl_Widget * w, int * s, int * e)
{
	Ewl_Selection *se;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	se = EWL_SELECTION(w);

	if (*s)
		*s = se->start_pos;
	if (*e)
		*e = se->end_pos;

	DLEAVE_FUNCTION;
}

void
ewl_selection_init(Ewl_Selection * s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_widget_init(w, "/appearance/selection/default");

	DLEAVE_FUNCTION;
}
