#include <Ewl.h>

static void __ewl_table_destroy(Ewl_Widget * w, void *ev_data,
				void *user_data);
static void __ewl_table_configure(Ewl_Widget * w, void *ev_data,
				  void *user_data);

/* make the configure callback smaller by spliting up big stuff into
 * small functions... */
Ewd_List *__ewl_table_fill_normal(Ewl_Table * w, int *rem_w, int *rem_h);
void __ewl_table_fill_fillers(Ewl_Table * t, int rem_w, int rem_h,
			      Ewd_List * l);
static void __ewl_table_layout_children(Ewl_Table * w);

/**
 * ewl_table_new - allocate a new table with default column spacing, row
 * spacing and homogenous settings.
 *
 * @columns: the number of columns in the table
 * @rows: the number of rows in the table
 *
 * Returns NULL on failure, the newly allocated table on success.
 */
Ewl_Widget *
ewl_table_new(unsigned int columns, unsigned int rows)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DRETURN_PTR(ewl_table_new_all(columns, rows, FALSE, 2, 2),
		    DLEVEL_STABLE);
}

/**
 * ewl_table_new_all - allocate a new table with specified settings
 *
 * @homogenous: set whether the table is homogenous
 * @columns: the number of columns in the table
 * @rows: the number of rows in the table
 * @col_spacing: the space between each column
 * @row_spacing: the space between each row
 *
 * Returns NULL on failure, the newly allocated table on success.
 */
Ewl_Widget *
ewl_table_new_all(unsigned int homogeneous,
		  unsigned int columns, unsigned int rows,
		  unsigned int col_spacing, unsigned int row_spacing)
{
	Ewl_Table *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = NEW(Ewl_Table, 1);
	if (!t)
		DRETURN_PTR(NULL, DLEVEL_TESTING);

	memset(t, 0, sizeof(Ewl_Table));
	ewl_table_init(t, homogeneous, columns, rows, col_spacing,
		       row_spacing);

	DRETURN_PTR(EWL_WIDGET(t), DLEVEL_STABLE);
}

/**
 * ewl_table_attach - attach a widget to the table in the specified locations
 *
 * @t: the table to attach the child
 * @c: the child to attach to the table
 * @start_col: the starting column of the child in the table
 * @end_col: the ending column of the child in the table
 * @start_row: the starting row of the child in the table
 * @end_row: the ending row of the child in the table
 *
 * Returns no value.
 */
void
ewl_table_attach(Ewl_Table * t, Ewl_Widget * c,
		 unsigned int start_col, unsigned int end_col,
		 unsigned int start_row, unsigned int end_row)
{
	Ewl_Table_Child *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("c", c);

	child = NEW(Ewl_Table_Child, 1);
	memset(child, 0, sizeof(Ewl_Table_Child));

	child->start_col = start_col - 1;
	child->end_col = end_col - 1;
	child->start_row = start_row - 1;
	child->end_row = end_row - 1;

	ewl_widget_set_data(c, (void *) t, child);

	ewl_container_append_child(EWL_CONTAINER(t), c);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_table_detach - remove the child in the specified column and row
 *
 * @t: the table to detach the child
 * @c: the column of the child to detach
 * @r: the row of the child to detach
 *
 * Returns no value.
 */
void
ewl_table_detach(Ewl_Table * t, unsigned int c, unsigned int r)
{
	Ewl_Widget *w;
	Ewl_Table_Child *child;

	DENTER_FUNCTION(DLEVEL_TESTING);
	DCHECK_PARAM_PTR("t", t);

	if (!EWL_CONTAINER(t)->children ||
	    ewd_list_is_empty(EWL_CONTAINER(t)->children))
		return;

	w = ewl_table_get_child(t, c, r);
	ewl_container_remove_child(EWL_CONTAINER(t), w);

	child = ewl_widget_get_data(w, (void *) t);

	if (child) {
		ewl_widget_del_data(w, (void *) t);
		FREE(child);
	}

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/**
 * ewl_table_resize - resize to the table to the specified dimensions
 *
 * @t: the table to resize
 * @c: the number of columns for the table
 * @r: the number of rows for the table
 *
 * Returns no value.
 */
void
ewl_table_resize(Ewl_Table * t, unsigned int c, unsigned int r)
{
	DENTER_FUNCTION(DLEVEL_TESTING);
	DCHECK_PARAM_PTR("t", t);

	EWL_TABLE(t)->columns = c;
	EWL_TABLE(t)->rows = r;

	REALLOC(EWL_TABLE(t)->col_w, int, c);
	REALLOC(EWL_TABLE(t)->row_h, int, r);

	REALLOC(EWL_TABLE(t)->x_offsets, int, c);
	REALLOC(EWL_TABLE(t)->y_offsets, int, r);

	DLEAVE_FUNCTION(DLEVEL_TESTING);
}

/**
 * ewl_table_get_columns - retrieve the number of columns in the table
 *
 * @t: the table to retrieve the number of columns
 *
 * Returns the number of columns in the table
 */
unsigned int
ewl_table_get_columns(Ewl_Table * t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

	DRETURN_INT(EWL_TABLE(t)->columns, DLEVEL_STABLE);
}

/**
 * ewl_table_get_rows - retrieve the number of rows in the table
 *
 * @t: the table to retrieve the number of rows
 *
 * Returns the number of rows in the table
 */
unsigned int
ewl_table_get_rows(Ewl_Table * t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

	DRETURN_INT(EWL_TABLE(t)->rows, DLEVEL_STABLE);
}

/**
 * ewl_table_set_homogeneous - set the homogenous setting of the table
 *
 * @t: the table to set the homogenous setting
 * @h: the new value for the homogenous setting
 *
 * Returns no value.
 */
void
ewl_table_set_homogeneous(Ewl_Table * t, unsigned int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	EWL_TABLE(t)->homogeneous = h;

	ewl_widget_configure(EWL_WIDGET(t));
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_table_set_col_spacing - set the column spacing setting of the table
 *
 * @t: the table to set the column spacing setting
 * @h: the new spacing between the columns in the table
 *
 * Returns no value.
 */
void
ewl_table_set_col_spacing(Ewl_Table * t, unsigned int cs)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	EWL_TABLE(t)->col_spacing = cs;

	ewl_widget_configure(EWL_WIDGET(t));
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_table_set_row_spacing - set the row spacing setting of the table
 *
 * @t: the table to set the row spacing setting
 * @h: the new spacing between the rows in the table
 *
 * Returns no value.
 */
void
ewl_table_set_row_spacing(Ewl_Table * t, unsigned int rs)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	t->row_spacing = rs;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_table_column_set_width - set the width of the specified column
 *
 * @t: the table to change the specified columns width
 * @c: the column to change the width
 * @w: the width to change the column
 *
 * Returns no value.
 */
void
ewl_table_column_set_width(Ewl_Table * t, unsigned int c, unsigned int w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	t->col_w[c - 1] = w;

	ewl_widget_configure(EWL_WIDGET(t));
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * ewl_table_get_column_width - get the width of the specified column
 *
 * @t: the table to retrieve the column width
 * @c: the column to retrieve the width
 *
 * Returns the width of the specified column.
 */
unsigned int
ewl_table_get_column_width(Ewl_Table * t, unsigned int c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0.0);

	DRETURN_INT(EWL_TABLE(t)->col_w[c - 1], DLEVEL_STABLE);
}

/*
 * ewl_table_get_child - retrieve the child at the specified row and column
 *
 * @t: the table to retrieve the child
 * @c: the column of the child to retrieve
 * @r: the row of the child to retrieve
 *
 * Returns NULL on failure, the child at the specified row and column on
 * success.
 */
Ewl_Widget *
ewl_table_get_child(Ewl_Table * t, unsigned int c, unsigned int r)
{
	Ewl_Widget *w;
	Ewl_Table_Child *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);

	ewd_list_goto_first(EWL_CONTAINER(t)->children);

	while ((w = ewd_list_next(EWL_CONTAINER(t)->children)) != NULL) {
		child = ewl_widget_get_data(w, (void *) t);

		if (child && child->start_col <= c &&
		    child->end_col >= c && child->start_row <= r &&
		    child->end_row >= r)
			DRETURN_PTR(w, DLEVEL_STABLE);
	}

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/*
 * ewl_table_get_row_geometry - get the specified row's position and shape
 *
 * @t: the table to retrieve the geometry
 * @r: the row to retrieve the geometry
 * @x: the integer to store the x position of the row
 * @y: the integer to store the y position of the row
 * @w: the integer to store the width of the row
 * @h: the integer to store the height of the row
 *
 * Returns no value.
 */
void
ewl_table_get_row_geometry(Ewl_Table * t, unsigned int r, int *x, int *y,
			   int *w, int *h)
{
	int i, width = 0;
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	if (r >= t->rows)
		DRETURN(DLEVEL_STABLE);

	for (i = 0; i < t->columns; i++)
		width += t->col_w[i];

	if (x)
		*x = t->x_offsets[0];

	if (y)
		*y = t->y_offsets[0];

	if (w)
		*w = width;

	if (h)
		*h = t->row_h[r];

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * ewl_table_init - initialize the tables fields
 *
 * @t: the table to initialize
 * @homogeneous: the homogenous setting of the table
 * @column: the number of columns in the table
 * @rows: the number of rows in the table
 * @col_spacing: the space between columns in the table
 * @row_spacing: the space between rows in the table
 *
 * Returns no value.
 */
void
ewl_table_init(Ewl_Table * t, unsigned int homogeneous,
	       unsigned int columns, unsigned int rows,
	       unsigned int col_spacing, unsigned int row_spacing)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/*
	 * Initialize the tables inherited fields
	 */
	ewl_container_init(EWL_CONTAINER(t), "/appearance/table");

	/*
	 * Setup the starting values of the tables settings
	 */
	t->columns = columns;
	t->rows = rows;
	t->homogeneous = homogeneous;
	t->col_spacing = col_spacing;
	t->row_spacing = row_spacing;

	/*
	 * Allocate and zero out the tables that keep track of child positions
	 * and dimensions
	 */
	t->col_w = NEW(int, columns);
	memset(t->col_w, 0, columns * sizeof(int));

	t->row_h = NEW(int, rows);
	memset(t->row_h, 0, rows * sizeof(int));

	t->x_offsets = NEW(int, columns);
	memset(t->x_offsets, 0, columns * sizeof(int));

	t->y_offsets = NEW(int, rows);
	memset(t->y_offsets, 0, rows * sizeof(int));

	/*
	 * Setup the default alignment of the table
	 */
	ewl_object_set_alignment(EWL_OBJECT(t), EWL_ALIGNMENT_CENTER);

	/*
	 * Now attach the necessary callbacks to the table
	 */
	ewl_callback_prepend(EWL_WIDGET(t), EWL_CALLBACK_DESTROY,
			     __ewl_table_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_CONFIGURE,
			    __ewl_table_configure, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Destroy the table, but don't destroy child widgets
 */
static void
__ewl_table_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Free the allocated tables that track child locations and dimensions
	 */
	IF_FREE(EWL_TABLE(w)->col_w);
	IF_FREE(EWL_TABLE(w)->row_h);

	IF_FREE(EWL_TABLE(w)->x_offsets);
	IF_FREE(EWL_TABLE(w)->y_offsets);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Reassign sizes and positions to each of the child widgets
 */
static void
__ewl_table_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int rem_w, rem_h;
	Ewd_List *fillers;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Layout the normal children first, that returns a list of the filler
	 * children, which are then laid out.
	 */
	fillers = __ewl_table_fill_normal(EWL_TABLE(w), &rem_w, &rem_h);
	if (fillers)
		__ewl_table_fill_fillers(EWL_TABLE(w), rem_w, rem_h,
					 fillers);

	__ewl_table_layout_children(EWL_TABLE(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Place the child into the table and modify the remaining widths and heights
 * to indicate space used
 */
void
__ewl_table_normal_span(Ewl_Table * w, Ewl_Widget * c, int *rem_w,
			int *rem_h)
{
	int i;
	int col_span = 1;
	int row_span = 1;
	int req_w, req_h;
	int used_w = 0, used_h = 0;
	int x, y;
	Ewl_Table_Child *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	ewl_object_requested_geometry(EWL_OBJECT(c), &x, &y, &req_w,
				      &req_h);
	used_w += req_w;
	used_h += req_h;

	child = ewl_widget_get_data(c, (void *) w);
	if (!child)
		return;

	col_span += child->start_col - child->end_col;

	/*
	 * Split up the size of the child between the col's
	 * it spans 
	 */
	for (i = child->start_col - 1; i < child->end_col; i++) {
		/*
		 * Only assign the column this width if another child doesn't
		 * need more.
		 */
		if (req_w / col_span > w->col_w[i]) {
			w->col_w[i] = req_w / col_span;
			*rem_w -= (req_w / col_span) + w->col_spacing;
		}
	}

	/*
	 * The child may have been shorted some of the width it needs, if so
	 * tack it on the end
	 */
	if ((req_w % col_span) + (req_w / col_span) > w->col_w[i - 1]) {
		w->col_w[i - 1] += req_w % col_span;
		*rem_w -= req_w % col_span;
	}

	row_span += child->start_row - child->end_row;

	/*
	 * Split up the size of the child between the row's it
	 * * spans 
	 */
	for (i = child->start_row - 1; i < child->end_row; i++) {
		/*
		 * Only assign the row this height if another child doesn't
		 * need more.
		 */
		if (req_h > w->row_h[i]) {
			w->row_h[i] = req_h / row_span;
			*rem_h -= (req_h / row_span) + w->row_spacing;
		}
	}

	/*
	 * The child has been shorted some of the height it needs, so tack it
	 * on the end.
	 */
	if ((req_w % row_span) + (req_h / row_span) > w->row_h[i - 1]) {
		w->row_h[i - 1] += req_h % row_span;
		*rem_h -= req_h % row_span;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Fill in the widgets that request their sizes normally and are not resized
 */
Ewd_List *
__ewl_table_fill_normal(Ewl_Table * w, int *rem_w, int *rem_h)
{
	int x, y;
	Ewd_List *fillers = NULL;
	Ewl_Widget *c = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);

	/*
	 * Grab the size so we know how much room we have to split the
	 * * children up into 
	 */
	ewl_object_requested_geometry(EWL_OBJECT(w), &x, &y, rem_w, rem_h);
	*rem_w -= EWL_TABLE(w)->columns * EWL_TABLE(w)->col_spacing;
	*rem_h -= EWL_TABLE(w)->rows * EWL_TABLE(w)->row_spacing;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	/*
	 * Loop through and allocate the space for each normal child,
	 * * add any filler children to the list of fillers that will be
	 * * returned 
	 */
	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL) {

		/*
		 * Normal children are given their requested space, otherwise
		 * we add them to the list of children to assign the remaining
		 * space.
		 */
		if (EWL_OBJECT(c)->fill == EWL_FILL_POLICY_NORMAL) {
			__ewl_table_normal_span(EWL_TABLE(w), c, rem_w,
						rem_h);
		} else {
			if (!fillers)
				fillers = ewd_list_new();
			ewd_list_append(fillers, c);
		}
	}

	DRETURN_PTR(fillers, DLEVEL_STABLE);
}

/*
 * Fill in widgets that are stretched to fill the column they occupy
 */
void
__ewl_table_fill_fillers(Ewl_Table * t, int rem_w, int rem_h, Ewd_List * l)
{
	int num;
	Ewl_Widget *c;
	Ewl_Table_Child *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("l", l);

	/*
	 * Determine the number of rows and columns remaining 
	 */
	num = ewd_list_nodes(l);
	ewd_list_goto_first(l);
	for (c = ewd_list_next(l); c; c = ewd_list_next(l)) {
		int i, x = 0, y = 0, cols, rows, req_w, req_h;

		child = ewl_widget_get_data(c, (void *) t);
		if (!child)
			continue;

		/*
		 * Determine the number of rows and columns this item uses 
		 */
		cols = child->end_col - child->start_col + 1;
		rows = child->end_row - child->start_row + 1;
		ewl_object_requested_geometry(EWL_OBJECT(c),
					      &x, &y, &req_w, &req_h);

		/*
		 * Sum the width values from the column width table
		 */
		for (i = child->start_col - 1; i < child->end_col; i++)
			req_w += t->col_w[i] + t->col_spacing;

		/*
		 * If the minimum width of the widget is greater than the
		 * assigned width, then increase the size of all the columns
		 * it crosses.
		 */
		if (MINIMUM_W(child) < req_w) {
			double increment = (double) req_w;

			increment /= (double) (child->end_col -
					       child->start_col + 1);

			/*
			 * Add the correction factor onto all columns that
			 * the child crosses
			 */
			for (i = child->start_col - 1; i < child->end_col;
			     i++)
				t->col_w[i] +=
				    (int) floor(increment + 0.5);
		}

		/*
		 * Sum the height values from the row height table
		 */
		for (i = child->start_row - 1; i < child->end_row; i++)
			req_h += t->row_h[i] + t->row_spacing;

		/*
		 * If the minimum width of the widget is greater than the
		 * assigned width, then increase the size of all the columns
		 * it crosses.
		 */
		if (MINIMUM_H(child) < req_h) {
			double increment = (double) req_h;

			increment /= (double) (child->end_row -
					       child->start_row + 1);

			/*
			 * Add the correction factor onto all rows this
			 * child crosses
			 */
			for (i = child->start_row - 1; i < child->end_row;
			     i++)
				t->row_h[i] +=
				    (int) floor(increment + 0.5);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_table_layout_children(Ewl_Table * w)
{
	Ewl_Widget *child;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Zero out the data in the location tables
	 */
	memset(w->x_offsets, 0, w->rows * w->columns * sizeof(int));
	memset(w->y_offsets, 0, w->rows * w->columns * sizeof(int));

	/*
	 * Now run through the table offsets and build up starting x and y
	 * positions for each column.
	 */
	for (i = 1, w->x_offsets[0] = REQUEST_X(w); i < w->rows; i++)
		w->x_offsets[i] = w->x_offsets[i - 1] + w->col_w[i - 1];

	for (i = 1, w->y_offsets[0] = REQUEST_Y(w); i < w->rows; i++)
		w->y_offsets[i] = w->y_offsets[i - 1] + w->row_h[i - 1];

	/*
	 * Loop through the children and determine their starting offsets
	 */
	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ewd_list_next(EWL_CONTAINER(w)->children))) {
		Ewl_Table_Child *c =
		    ewl_widget_get_data(child, (void *) w);

		/*
		 * Ok, now that we have offset tables put each child at its
		 * required offset. Fillers get resized to fill the whole
		 * area available.
		 */
		if (ewl_object_get_fill_policy(EWL_OBJECT(w)) ==
		    EWL_FILL_POLICY_NORMAL)
			ewl_object_request_geometry(EWL_OBJECT(child),
						    w->x_offsets[c->
								 start_col
								 - 1],
						    w->y_offsets[c->
								 start_row
								 - 1],
						    CURRENT_W(child),
						    CURRENT_H(child));
		else {
			int i, wide = 0, high = 0;

			for (i = c->start_col - 1; i < c->end_col; i++)
				wide += w->col_w[i];

			for (i = c->start_row - 1; i < c->end_row; i++)
				high += w->row_h[i];

			ewl_object_request_geometry(EWL_OBJECT(child),
						    w->x_offsets[c->
								 start_col
								 - 1],
						    w->y_offsets[c->
								 start_row
								 - 1],
						    wide, high);
		}
		printf("Placed child %p at (%d, %d) dim %dx%d\n", child,
		       REQUEST_X(child), REQUEST_Y(child),
		       REQUEST_W(child), REQUEST_H(child));

		ewl_widget_configure(child);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
