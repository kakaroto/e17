#include <Ewl.h>

/**
 * @return Returns a newly allocated cell on success, NULL on failure.
 * @brief Allocate and initialize a new cell
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
 * @param cell: the cell object to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the cell fields of an inheriting object
 *
 * The fields of the @a cell object are initialized to their defaults.
 */
int ewl_cell_init(Ewl_Cell *cell)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("cell", cell, FALSE);

	if (!ewl_container_init(EWL_CONTAINER(cell), "cell", ewl_cell_add_cb,
			ewl_cell_child_resize_cb, NULL))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_callback_append(EWL_WIDGET(cell), EWL_CALLBACK_CONFIGURE,
			ewl_cell_configure_cb, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_cell_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Cell *cell;
	Ewl_Container *c;
	Ewl_Object *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cell = EWL_CELL(w);
	c = EWL_CONTAINER(w);

	child = ewd_list_goto_first(c->children);

	if (child)
		ewl_object_place(child, CURRENT_X(w), CURRENT_Y(w),
				CURRENT_W(w), CURRENT_H(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_cell_add_cb(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Cell's only allow one child, so remove the rest, this may cause a
	 * leak, but they should know better.
	 */
	ewd_list_goto_first(c->children);
	while ((child = ewd_list_next(c->children))) {
		if (child != w)
			ewl_container_remove_child(c, child);
	}

	ewl_object_set_preferred_size(EWL_OBJECT(c),
			ewl_object_get_preferred_w(EWL_OBJECT(w)),
			ewl_object_get_preferred_h(EWL_OBJECT(w)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_cell_child_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
		Ewl_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_object_set_preferred_size(EWL_OBJECT(c),
			ewl_object_get_preferred_w(EWL_OBJECT(w)),
			ewl_object_get_preferred_h(EWL_OBJECT(w)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
