
#include <Ewl.h>


/**
 * ewl_cursor_new - create a cursor for positioning within an entry widget
 *
 * Returns a newly allocated cursor widget on success, NULL on failure.
 */
Ewl_Widget     *ewl_cursor_new(void)
{
	Ewl_Cursor     *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = NEW(Ewl_Cursor, 1);

	ZERO(c, Ewl_Cursor, 1);
	ewl_cursor_init(c);

	DRETURN_PTR(EWL_WIDGET(c), DLEVEL_STABLE);
}

/**
 * ewl_cursor_set_position - set the current position of the cursor
 * @w: the cursor widget to change position
 * @p: the index of the cursor position within the entry widget's text
 *
 * Returns no value. Changes the position of the cursor so that the entry
 * widget can update it appropriately.
 */
void ewl_cursor_set_position(Ewl_Widget * w, int p)
{
	Ewl_Cursor     *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CURSOR(w);

	c->position = p;

	ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_cursor_get_position - retrieve the position of the cursor
 * @w: the entry cursor to retrieve the current position
 *
 * Returns the current position of the entry widget @w.
 */
int ewl_cursor_get_position(Ewl_Widget * w)
{
	Ewl_Cursor     *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);

	c = EWL_CURSOR(w);

	DRETURN_INT(c->position, DLEVEL_STABLE);
}

/**
 * ewl_cursor_init - initialize the cursor to default values and appearance
 * @c: the cursor to initialize
 *
 * Returns no value. Sets the default values and appearance for the cursor @c.
 */
void ewl_cursor_init(Ewl_Cursor * c)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	w = EWL_WIDGET(c);

	ewl_widget_init(w, "/appearance/cursor/default");

	c->position = 1;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
