#include <Ewl.h>

static void __ewl_cell_configure(Ewl_Widget * w, void *ev_data, void *user_data);

/**
 * ewl_cell_new - allocate and initialize a new cell
 *
 * Returns a newly allocated cell on success, NULL on failure.
 */
Ewl_Widget *ewl_cell_new()
{
	Ewl_Widget *cell;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cell = NEW(Ewl_Cell, 1);
	if (!cell)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_cell_init(EWL_CELL(cell))) {
		FREE(cell);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(cell, DLEVEL_STABLE);
}

/**
 * ewl_cell_init - initialize the cell fields of an inheriting object
 * @cell: the cell object to initialize
 *
 * Returns TRUE on success, FALSE on failure. The fields of the @cell object
 * are initialized to their defaults.
 */
int ewl_cell_init(Ewl_Cell *cell)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("cell", cell, FALSE);

	ewl_container_init(EWL_CONTAINER(cell), "cell", NULL, NULL);

	ewl_callback_append(EWL_WIDGET(cell), EWL_CALLBACK_CONFIGURE,
			__ewl_cell_configure, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
__ewl_cell_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Cell *cell;
	Ewl_Container *c;
	Ewl_Object *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cell = EWL_CELL(w);
	c = EWL_CONTAINER(w);

	child = ewd_list_goto_first(c->children);

	ewl_object_request_geometry(child, CURRENT_X(w), CURRENT_Y(w),
			CURRENT_W(w), CURRENT_H(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
