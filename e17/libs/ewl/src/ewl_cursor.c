
#include <Ewl.h>


/**
 * @return Returns a new cursor widget on success, NULL on failure.
 * @brief Create a cursor for positioning within an entry widget
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
 * @param c: the cursor to initialize
 * @return Returns no value.
 * @brief Initialize the cursor to default values and appearance
 *
 * Sets the default values and appearance for the cursor @a c.
 */
void ewl_cursor_init(Ewl_Cursor * c)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	w = EWL_WIDGET(c);

	ewl_widget_init(w, "cursor");

	c->position.start = 1;
	c->position.end = 1;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the cursor to change base position
 * @param pos: the new base position value
 * @return Returns no value.
 * @brief Set the initial position of the cursor
 *
 * Sets the initial position that will be used when determining layout when
 * start or end positions change.
 */
void ewl_cursor_set_base(Ewl_Cursor *c, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	c->position.start = c->position.end = c->position.base = pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the cursor widget to change position
 * @param start: the start index the cursor position
 * @param end: the end index the cursor position
 * @return Returns no value.
 * @brief Set the current position of the cursor
 *
 * Changes the position of the cursor so that the entry widget can update it
 * appropriately.
 */
void
ewl_cursor_set_position(Ewl_Cursor * c, unsigned int start, unsigned int end)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	if (start == 0)
		start = 1;
	c->position.start = start;

	if (end == 0)
		end = 1;
	if (end < start)
		end = start;
	c->position.end = end;

	ewl_callback_call(EWL_WIDGET(c), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the cursor to perform the selection
 * @param pos: the ending position of the selection
 * @return Returns no value.
 * @brief Selects the text between the cursor base and specified position
 *
 * Selects the characters between the previously specified base position and
 * the specified @a pos.
 */
void ewl_cursor_select_to(Ewl_Cursor *c, unsigned int pos)
{

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	if (pos < c->position.base) {
		c->position.start = pos;
		c->position.end = c->position.base;
	}
	else if (pos > c->position.base) {
		c->position.start = c->position.base;
		c->position.end = pos;
	}
	else
		c->position.start = c->position.end = pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: the cursor to retrieve base position
 * @return Returns the current base position.
 * @brief Retrieve the current base position.
 */
unsigned int ewl_cursor_get_base_position(Ewl_Cursor *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, 0);

	DRETURN_INT(c->position.base, DLEVEL_STABLE);
}

/**
 * @param c: the entry cursor to retrieve the current start position
 * @return Returns the current start position of the cursor @a c.
 * @brief Retrieve the start position of the cursor
 */
unsigned int ewl_cursor_get_start_position(Ewl_Cursor * c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, 0);

	DRETURN_INT(c->position.start, DLEVEL_STABLE);
}

/**
 * @param c: the entry cursor to retrieve the current end position
 * @return Returns the current end position of the cursor widget @a c.
 * @brief Retrieve the end position of the cursor
 */
unsigned int ewl_cursor_get_end_position(Ewl_Cursor * c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, 0);

	DRETURN_INT(c->position.end, DLEVEL_STABLE);
}
