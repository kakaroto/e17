
#include <Ewl.h>

void            __ewl_table_configure(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void            __ewl_table_show(Ewl_Widget * w, void *ev_data,
				 void *user_data);
void            __ewl_table_child_configure(Ewl_Widget * w, void *ev_data,
					    void *user_data);
void            __ewl_table_child_show(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_table_child_select(Ewl_Widget * w, void *ev_data,
					 void *user_data);


/**
 * ewl_table_new - create a new table
 * @cols: the number of columns
 * @rows: the number of rows
 * @col_headers: the column titles
 *
 * Returns a pointer to a newly allocated table on success, NULL on
 * failure.
 */
Ewl_Widget     *ewl_table_new(int cols, int rows, char **col_headers)
{
	Ewl_Table      *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = NEW(Ewl_Table, 1);
	if (!t)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(t, 0, sizeof(Ewl_Table));
	ewl_table_init(t, cols, rows, col_headers);

	DRETURN_PTR(EWL_WIDGET(t), DLEVEL_STABLE);
}

/**
 * ewl_table_init - initialize table to starting values
 * @t: the table
 * @cols: the number of columns
 * @rows: the number of rows
 * @col_headers: the column titles
 *
 * Returns no value. Responsible for setting up default values and
 * callbacks withing the table structure
 */
void ewl_table_init(Ewl_Table * t, int cols, int rows, char **col_headers)
{
	Ewl_Widget	*button;
	Ewl_Cell	*cell;
	int             i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("col_headers", col_headers);

	/*
	 * Iniitialize the tables inherited fields
	 */
	ewl_container_init(EWL_CONTAINER(t), "table", NULL, NULL, NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(t), EWL_FLAG_FILL_FILL);

	/*
	 * Create a new grid
	 */
	t->grid = (Ewl_Grid *) ewl_grid_new(cols, rows);
	ewl_container_append_child(EWL_CONTAINER(t), EWL_WIDGET(t->grid));

	/*
	 * Add the column headers to the grid
	 */
	if (col_headers != NULL) {

		for (i = 1; i <= cols; i++) {
			cell = (Ewl_Cell *) ewl_cell_new();
			button = ewl_button_new(col_headers[i - 1]);
			ewl_widget_disable(button);
			ewl_container_append_child(EWL_CONTAINER(cell), button);
			ewl_grid_add(t->grid, EWL_WIDGET(cell), i, i, 1, 1);
			ewl_widget_show(EWL_WIDGET(button));
			ewl_widget_show(EWL_WIDGET(cell));
		}

		t->col_headers = col_headers;
	}

	t->row_select = 0;

	t->selected.start_r = -1;
	t->selected.start_c = -1;
	t->selected.end_r = -1;
	t->selected.end_c = -1;

	/*
	 * Append callbacks
	 */
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_CONFIGURE,
			    __ewl_table_configure, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_REALIZE,
			    __ewl_table_show, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_table_add - add a child widget to the table
 * @table: the table
 * @cell: the cell to add
 * @dm_click: callback function to be called on double mouse click
 * @start_col: the start column
 * @end_col: the end columnt
 * @start_row: the start row
 * @end_row: the end row
 *
 * Returns no value.
 */
void
ewl_table_add(Ewl_Table * table, Ewl_Cell * cell,
	      int start_col, int end_col, int start_row, int end_row)
{


	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_add(table->grid, EWL_WIDGET(cell),
		     start_col, end_col, start_row, end_row);

	ewl_callback_prepend(EWL_WIDGET(cell), EWL_CALLBACK_MOUSE_UP,
			     __ewl_table_child_select, table);

	ewl_widget_show(EWL_WIDGET(cell));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}




/**
 * ewl_table_get_col_row - get the column and row of a widget
 * @t: the table
 * @cell: the cell to add
 * @w: the widget to find
 * @start_col: integer pointer to store the start column
 * @end_col: integer pointer to store the end column in
 * @start_row: integer pointer to store the start row in
 * @end_row: integer pointer to store the end row in
 *
 * Returns nothing
 */
void ewl_table_get_col_row(Ewl_Table * t, Ewl_Cell * cell,
			   int *start_col, int *end_col, int *start_row,
			   int *end_row)
{

	/*---------------------------------
	 * DEVELOPER NOTE:
	 * This method is supposed to search the grid for the specified 
	 * widget and store it's column/row specs in the col/row 
	 * parameters and I think that is in fact what it does now. It
	 * is not tested.
	 *---------------------------------*/

	Ewl_Widget *child;
	Ewl_Grid_Child *g_child;
	Ewd_List *children;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	children = EWL_CONTAINER(t->grid)->children;

	ewd_list_goto_first(children);
	while ((child = ewd_list_next(children)) != NULL)
	{
		if (child == EWL_WIDGET(cell)) {
			g_child = (Ewl_Grid_Child *) ewl_widget_get_data(child,
					(void *) t->grid);

			if (start_col)
				*start_col = g_child->start_col;
			if (end_col)
				*end_col = g_child->end_col;
			if (start_row)
				*start_row = g_child->start_row;
			if (end_row)
				*end_row = g_child->end_row;

			break;
		}
	}

	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_table_find - get a list of the widgets in the specified col/row
 * @t: the table
 * @start_col: the start column
 * @end_col: the end column
 * @start_row: the start row
 * @end_row: the end row
 *
 * Returns Ewd_List of widgets found in the specified col/row area
 */
Ewd_List       *ewl_table_find(Ewl_Table * t, int start_col, int end_col,
			       int start_row, int end_row)
{

	/*---------------------------------
	 * DEVELOPER NOTE:
	 * This is supposed to search for any widgets that can be found
	 * inside an area bounded by the col/row parameters and return a
	 * list of all those widgets
	 *
	 * And now it does so (UNTESTED)
	 *---------------------------------*/

	Ewl_Grid_Child *gc;
	Ewl_Widget     *child;
	Ewd_List       *children;
	Ewd_List       *list;

	DENTER_FUNCTION(DLEVEL_STABLE);

	list = ewd_list_new();
	children = EWL_CONTAINER(t->grid)->children;

	ewd_list_goto_first(children);
	while ((child = ewd_list_next(children)) != NULL) {
		gc = (Ewl_Grid_Child *) ewl_widget_get_data(child,
							    (void *) t->grid);

		if (start_col >= gc->start_col && end_col <= gc->end_col &&
		    start_row >= gc->start_row && end_row <= gc->end_row) {
			ewd_list_append(list, child);
		}
	}


	DRETURN_PTR(list, DLEVEL_STABLE);
}


/**
 * ewl_table_set_col_w - set the width of a table column
 * @table: the table
 * @col: the column
 * @width: the new width
 *
 * Returns no value.
 */
void ewl_table_set_col_w(Ewl_Table * table, int col, int width)
{

	/*---------------------------------
	 * DEVELOPER NOTE:
	 * set column width of the grid
	 *---------------------------------*/

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_set_col_w(table->grid, col, width);
	ewl_widget_configure(EWL_WIDGET(table));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_table_get_col_w - get the width of a table column
 * @table: the table
 * @col: the column
 * @width: integer pointer to store the width in
 *
 * Returns no value.
 */
void ewl_table_get_col_w(Ewl_Table * table, int col, int *width)
{

	/*---------------------------------
	 * DEVELOPER NOTE:
	 * get column width of the grid. 
	 *---------------------------------*/

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_get_col_w(table->grid, col, width);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_table_set_row_h - set the height of a table row
 * @table: the table
 * @row: the row
 * @height: the new height
 *
 * Returns no value.
 */
void ewl_table_set_row_h(Ewl_Table * table, int row, int height)
{

	/*---------------------------------
	 * DEVELOPER NOTE:
	 * set row height of the grid
	 *---------------------------------*/

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_set_row_h(table->grid, row, height);
	ewl_widget_configure(EWL_WIDGET(table));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_table_get_row_h - get the height of a table row
 * @table: the table
 * @row: the row
 * @height: integer pointer to store the height in
 *
 * Returns no value.
 */
void ewl_table_get_row_h(Ewl_Table * table, int row, int *height)
{

	/*---------------------------------
	 * DEVELOPER NOTE:
	 * get row height of the grid
	 *---------------------------------*/

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_get_row_h(table->grid, row, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_table_reset - clear the table and set new geometry
 * @t: the table
 * @cols: the new number of columns
 * @rows: the new number of columns
 * @col_headers: the new column headers
 *
 * Returns no value
 */
void ewl_table_reset(Ewl_Table * t, int cols, int rows, char **col_headers)
{
	Ewl_Widget	*button;
	Ewl_Cell	*cell;
	int             i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);


	ewl_grid_reset(EWL_GRID(t->grid), cols, rows);

	if (col_headers != NULL) {

		for (i = 1; i <= cols; i++) {
			cell = (Ewl_Cell *) ewl_cell_new();
			button = ewl_button_new(col_headers[i - 1]);
			ewl_widget_disable(button);
			ewl_container_append_child(EWL_CONTAINER(cell), button);
			ewl_grid_add(t->grid, EWL_WIDGET(cell), i, i, 1, 1);
			ewl_widget_show(EWL_WIDGET(cell));
		}

		t->col_headers = col_headers;
	}

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_table_get_selected - get the text in the current selected box
 * @t: the table
 *
 * Returns the text in the currently selected widget in the table
 */
char           *ewl_table_get_selected(Ewl_Table * t)
{
	Ewl_Text       *tw;
	Ewl_Widget     *child;
	Ewd_List       *children;
	Ewl_Grid_Child *gc;

	DENTER_FUNCTION(DLEVEL_STABLE);

	children = EWL_CONTAINER(t->grid)->children;

	ewd_list_goto_first(children);
	while ((child = ewd_list_next(children)) != NULL) {
		gc = (Ewl_Grid_Child *) ewl_widget_get_data(child,
							    (void *) t->grid);

		if (t->selected.start_c == gc->start_col &&
		    t->selected.start_r == gc->start_row &&
		    t->selected.end_c == gc->end_col &&
		    t->selected.end_r == gc->end_row) {
			
			tw = (Ewl_Text *) ewd_list_goto_first(
					EWL_CONTAINER(child)->children);

			DRETURN_PTR(ewl_text_get_text(tw), DLEVEL_STABLE);

			break;
		}
	}

	DRETURN_PTR("", DLEVEL_STABLE);
}


void __ewl_table_child_select(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Table      *t;
	Ewl_Grid_Child *gc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);


	t = EWL_TABLE(user_data);

	gc = (Ewl_Grid_Child *) ewl_widget_get_data(w, (void *) t->grid);

	t->selected.start_r = gc->start_row;
	t->selected.start_c = gc->start_col;
	t->selected.end_r = gc->end_row;
	t->selected.end_c = gc->end_col;


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



void __ewl_table_show(Ewl_Widget * w, void *ev_data, void *user_data)
{

	/*---------------------------------
	 * DEVELOPER NOTE:
	 * This thing does all it's supposed to do.
	 *
	 * Maybe it's not needed even ??
	 *---------------------------------*/


	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_widget_show(EWL_WIDGET(EWL_TABLE(w)->grid));
	ewl_widget_configure(EWL_WIDGET(EWL_TABLE(w)->grid));
	ewl_widget_configure(w);

	DENTER_FUNCTION(DLEVEL_STABLE);
}

void __ewl_table_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{

	/*---------------------------------
	 * DEVELOPER NOTE:
	 * Right now all this method does is making sure the grid take
	 * up the space it can. (i.e. the same space as the table
	 * itself). That's it's only purpose I can think of now.
	 *---------------------------------*/

	Ewl_Table      *table;
	Ewl_Object     *o;
	Ewl_Widget     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);


	table = EWL_TABLE(w);
	o = EWL_OBJECT(w);


	ewl_object_request_geometry(EWL_OBJECT(table->grid),
				    CURRENT_X(o), CURRENT_Y(o),
				    CURRENT_W(o) - INSET_LEFT(o) +
				    INSET_RIGHT(o),
				    CURRENT_H(o) - INSET_TOP(o) +
				    INSET_BOTTOM(o));


	ewd_list_goto_first(EWL_CONTAINER(table->grid)->children);
	while ((child =
		ewd_list_next(EWL_CONTAINER(table->grid)->children)) != NULL) {

		ewl_widget_configure(child);
	}


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
