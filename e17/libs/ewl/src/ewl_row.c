#include <Ewl.h>

static void __ewl_row_configure(Ewl_Widget * w, void *ev_data, void *user_data);

static void __ewl_row_add(Ewl_Container *c, Ewl_Widget *w);
static void __ewl_row_resize(Ewl_Container *c, Ewl_Widget *w, int size,
		Ewl_Orientation o);

/**
 * ewl_row_new - allocate and initialize a new row
 *
 * Returns a newly allocated row on success, NULL on failure.
 */
Ewl_Widget *ewl_row_new()
{
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);

	row = NEW(Ewl_Row, 1);
	if (!row)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(row, Ewl_Row, 1);
	if (!ewl_row_init(EWL_ROW(row))) {
		FREE(row);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(row, DLEVEL_STABLE);
}

/**
 * ewl_row_init - initialize the row fields of an inheriting object
 * @row: the row object to initialize
 *
 * Returns TRUE on success, FALSE on failure. The fields of the @row object
 * are initialized to their defaults.
 */
int ewl_row_init(Ewl_Row *row)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("row", row, FALSE);

	ewl_container_init(EWL_CONTAINER(row), "row", __ewl_row_add,
			__ewl_row_resize, NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(row), EWL_FILL_POLICY_HFILL |
			EWL_FILL_POLICY_HSHRINK);

	ewl_callback_append(EWL_WIDGET(row), EWL_CALLBACK_CONFIGURE,
			__ewl_row_configure, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_row_set_column_bounds - set the table of constraints on cell widths
 * @row: the row to change the column table
 * @base: base position for finding column widths, NULL causes no constraints
 * @bounds: the array of column width bounds, NULL causes no constraints
 *
 * Returns no value. The table of widths for @row is changed to @bounds, rows
 * calculate their actual width based on the position of @base. You must set
 * both of these variables to set one. It is strongly recommended that you add
 * a configure callback to the widget that contains this table and base to be
 * notified when the values change. If @base or @bounds is NULL, then each cell
 * is given it's preferred size.
 */
void
ewl_row_set_column_bounds(Ewl_Row *row, int n, unsigned int **base,
		unsigned int **bounds)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("row", row);

	if (!base || !bounds) {
		row->base = NULL;
		row->bounds = NULL;
	}
	else {
		row->base = base;
		row->bounds = bounds;
	}

	ewl_widget_configure(EWL_WIDGET(row));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_row_get_column - retrieve the widget at a specified column
 * @row: the row to retrieve a columns widget from
 * @n: the column containing the desired widget
 *
 * Returns the widget located in column @n in @row on success, NULL on
 * failure.
 */
Ewl_Widget *
ewl_row_get_column(Ewl_Row *row, short n)
{
	Ewl_Widget *found;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("row", row, NULL);

	found = ewd_list_goto_index(EWL_CONTAINER(row)->children, n + 1);

	DRETURN_PTR(found, DLEVEL_STABLE);
}

static void
__ewl_row_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Row *row;
	Ewl_Container *c;
	Ewl_Object *child;
	int x;

	DENTER_FUNCTION(DLEVEL_STABLE);

	row = EWL_ROW(w);
	c = EWL_CONTAINER(w);

	x = CURRENT_X(w);

	ewd_list_goto_first(c->children);

	/*
	 * This should be the common case, a row bounded by a set of fields,
	 * for forming a table.
	 */
	if (row->bounds) {
		int i = 0;
		while ((child = ewd_list_next(c->children))) {
			ewl_object_request_position(child, x, CURRENT_Y(w));
			ewl_object_request_w(child,
					*row->base[i] + *row->bounds[i] - x);
			ewl_object_request_h(child, CURRENT_H(w));
			x = ewl_object_get_current_x(child) +
				ewl_object_get_current_w(child);
			i++;
		}
	}
	/*
	 * In the uncommon case, we simply try to give out a fair amount of
	 * space.
	 */
	else {
		int remains, nodes;

		remains = CURRENT_W(w);
		nodes = ewd_list_nodes(c->children);
		while ((child = ewd_list_next(c->children))) {
			int portion;

			/*
			 * Attempt to divvy up remaining space equally among
			 * remaining children.
			 */
			portion =  remains / nodes;
			ewl_object_request_position(child, x, CURRENT_Y(w));
			ewl_object_request_w(child, portion);

			ewl_object_request_h(child, CURRENT_H(w));
			x = ewl_object_get_current_x(child) +
				ewl_object_get_current_w(child);

			remains = CURRENT_X(w) + CURRENT_W(w) - x;
			nodes--;
		}

		if ((child = ewd_list_goto_last(c->children)))
			ewl_object_request_w(child,
					ewl_object_get_current_w(child) +
					remains);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_row_add(Ewl_Container *c, Ewl_Widget *w)
{
	int size;
	Ewl_Row *row;

	row = EWL_ROW(c);

	/*
	 * Adjust the preferred height to the largest widget added.
	 */
	size = ewl_object_get_minimum_w(EWL_OBJECT(w));
	if (!row->max || ewl_object_get_minimum_h(row->max) > size) {
		row->max = EWL_OBJECT(w);
		ewl_object_set_preferred_h(EWL_OBJECT(c), size);
	}

	ewl_object_set_preferred_w(EWL_OBJECT(c), PREFERRED_W(c) +
			ewl_object_get_minimum_w(EWL_OBJECT(w)));
}

static void
__ewl_row_resize(Ewl_Container *c, Ewl_Widget *w, int size, Ewl_Orientation o)
{
	Ewl_Row *row;

	row = EWL_ROW(c);
	if (o == EWL_ORIENTATION_VERTICAL) {
		if (EWL_OBJECT(w) == row->max && size > 0)
			ewl_object_set_preferred_h(EWL_OBJECT(c),
					PREFERRED_H(c) + size);
		else {
			int h;
			int max_h = 0;
			Ewl_Object *child;

			/*
			 * Search out the tallest widget in the row
			 */
			ewd_list_goto_first(c->children);
			while ((child = ewd_list_next(c->children))) {
				h = ewl_object_get_minimum_h(child);
				if (h > max_h) {
					max_h = h;
					row->max = child;
				}
			}

			PREFERRED_H(c) = max_h;
		}
	}
	else {
		ewl_object_set_preferred_w(EWL_OBJECT(c),
				PREFERRED_W(c) + size);
	}
}
