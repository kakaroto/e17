
#include <Ewl.h>


void ewl_cursor_init(Ewl_Cursor * c);


Ewl_Widget *
ewl_cursor_new(void)
{
	Ewl_Cursor *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = NEW(Ewl_Cursor, 1);

	memset(c, 0, sizeof(Ewl_Cursor));
	ewl_cursor_init(c);

	DRETURN_PTR(EWL_WIDGET(c), DLEVEL_STABLE);
}

void
ewl_cursor_set_position(Ewl_Widget * w, int p)
{
	Ewl_Cursor *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CURSOR(w);

	c->position = p;

	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewl_cursor_get_position(Ewl_Widget * w)
{
	Ewl_Cursor *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);

	c = EWL_CURSOR(w);

	DRETURN_INT(c->position, DLEVEL_STABLE);
}

void
ewl_cursor_init(Ewl_Cursor * c)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	w = EWL_WIDGET(c);

	ewl_widget_init(w, "/appearance/cursor/default");

	c->position = 1;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
