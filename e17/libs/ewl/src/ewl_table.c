
#include <Ewl.h>

void __ewl_table_configure (Ewl_Widget *w, void *ev_data, void *user_data);
void __ewl_table_show (Ewl_Widget *w, void *ev_data, void *user_data);
void __ewl_table_child_configure (Ewl_Widget *w, void *ev_data, void *user_data);
void __ewl_table_child_show (Ewl_Widget *w, void *ev_data, void *user_data);
void __ewl_table_child_select (Ewl_Widget *w, void *ev_data, void *user_data);

Ewl_Table_Child *__ewl_table_create_child (Ewl_Widget *w);


/**
 * ewl_table_new - create a new table
 * @cols: the number of columns
 * @rows: the number of rows
 * @col_headers: the column titles
 *
 * Returns a pointer to a newly allocated grid on success, NULL on
 * failure.
 */
Ewl_Widget *ewl_table_new(int cols, int rows, char **col_headers)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * the regular new function
	 *---------------------------------*/
	
	Ewl_Table *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = NEW(Ewl_Table, 1);
	if (!t)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(t, 0, sizeof(Ewl_Table));

	ewl_table_init (t, cols, rows, col_headers);

	
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
void ewl_table_init (Ewl_Table *t, int cols, int rows, char **col_headers)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * the regular init function
	 *---------------------------------*/

	
	Ewl_Widget *button;
	Ewl_Table_Child *child;
	int i;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("col_headers", col_headers);


	/*
	 * Iniitialize the tables inherited fields
	 */
	ewl_container_init(EWL_CONTAINER(t), 
			"/appearance/box/vertical", NULL, NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(t), EWL_FILL_POLICY_FILL);
	ewl_theme_data_set_str(EWL_WIDGET(t),
			"/appearance/box/vertical/base/visible",
			"no");


	/*
	 * Create a new grid
	 */
	t->grid = (Ewl_Grid *)ewl_grid_new(cols, rows);
	ewl_container_append_child(EWL_CONTAINER(t), EWL_WIDGET(t->grid));
	ewl_theme_data_set_str(EWL_WIDGET(t->grid),
			"/appearance/box/vertical/base/visible",
			"no");


	/*
	 * Add the column headers to the grid
	 */
	if (col_headers != NULL) {
		
		for (i = 1; i <= cols; i++) {
			button = ewl_button_new(col_headers[i-1]);
			ewl_object_set_fill_policy(EWL_OBJECT(button), 
					EWL_FILL_POLICY_VSHRINK);
			ewl_widget_disable(button);
			child = __ewl_table_create_child(button);
			ewl_grid_add(t->grid, EWL_WIDGET(child), i, i, 1, 1);
			ewl_widget_show(EWL_WIDGET(child));
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
 * @t: the table
 * @text: the text to add
 * @dm_click: callback function to be called on double mouse click
 * @start_col: the start column
 * @end_col: the end columnt
 * @start_row: the start row
 * @end_row: the end row
 *
 * Returns no value.
 */
void
ewl_table_add(Ewl_Table *table, char *text,
		int start_col, int end_col, int start_row, int end_row)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * Need to create a new child widget (container containing the
	 * widget passed in here). Then add the child to the grid.
	 *---------------------------------*/

	Ewl_Table_Child *child;
	Ewl_Widget *txt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	txt = ewl_button_new(text);
	child = __ewl_table_create_child(txt);
	ewl_grid_add(table->grid, EWL_WIDGET(child), 
			start_col, end_col, start_row, end_row);

	ewl_callback_prepend(child->widget, EWL_CALLBACK_MOUSE_UP,
			__ewl_table_child_select, table);

	ewl_widget_show(EWL_WIDGET(child));

	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



/**
 * ewl_table_add_return - add a child widget to the table
 * @t: the table
 * @text: the text to add
 * @start_col: the start column
 * @end_col: the end columnt
 * @start_row: the start row
 * @end_row: the end row
 *
 * Returns the child widget created so people can add callbacks and do
 * other fun stuff to it.
 */
Ewl_Widget *
ewl_table_add_return(Ewl_Table *table, char *text,
		int start_col, int end_col, int start_row, int end_row)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * Need to create a new child widget (container containing the
	 * widget passed in here). Then add the child to the grid.
	 *---------------------------------*/

	Ewl_Table_Child *child;
	Ewl_Widget *txt;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if(!table) {
		printf("no table\n");
		return NULL;
	}

	txt = ewl_button_new(text);
	ewl_theme_data_set_str(txt,
			"/appearance/button/default/base/visible", "no");
	child = __ewl_table_create_child(txt);
	ewl_grid_add(table->grid, EWL_WIDGET(child), 
			start_col, end_col, start_row, end_row);

	ewl_callback_prepend(child->widget, EWL_CALLBACK_MOUSE_UP,
			__ewl_table_child_select, table);

	
	DRETURN_PTR(EWL_WIDGET(child), DLEVEL_STABLE);
}



/**
 * ewl_table_get_col_row - get the column and row of a widget
 * @t: the table
 * @w: the widget to find
 * @start_col: integer pointer to store the start column
 * @end_col: integer pointer to store the end column in
 * @start_row: integer pointer to store the start row in
 * @end_row: integer pointer to store the end row in
 *
 * Returns nothing
 */
void 
ewl_table_get_col_row(Ewl_Table *t, char *text, /* Ewl_Widget *w */
		int *start_col, int *end_col, int *start_row, int *end_row)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * This method is supposed to search the grid for the specified 
	 * widget and store it's column/row specs in the col/row 
	 * parameters and I think that is in fact what it does now. It
	 * is not tested.
	 *---------------------------------

	Ewl_Widget *child;
	Ewl_Grid_Child *g_child;
	Ewd_List *children;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("w", w);

	children = EWL_CONTAINER(t->grid)->children;

	ewd_list_goto_first(children);
	while ((child = ewd_list_next(children)) != NULL)
	{
		if (child == w) {
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

	*/
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
Ewd_List *
ewl_table_find(Ewl_Table *t, int start_col, int end_col, 
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
	Ewl_Widget *child;
	Ewd_List *children;
	Ewd_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);

	list = ewd_list_new();
	children = EWL_CONTAINER(t->grid)->children;
	
	ewd_list_goto_first(children);
	while ((child = ewd_list_next(children)) != NULL)
	{
		gc = (Ewl_Grid_Child *) ewl_widget_get_data(child,
					(void *) t->grid);

		if (start_col >= gc->start_col && end_col <= gc->end_col &&
			start_row >= gc->start_row && end_row <= gc->end_row)
		{
			ewd_list_append(list, EWL_TABLE_CHILD(child)->widget);
		}
	}

	
	DRETURN_PTR(list, DLEVEL_STABLE);
}


/**
 * ewl_table_set_col_w - set the width of a table column
 * @t: the table
 * @col: the column
 * @width: the new width
 *
 * Returns no value.
 */
void
ewl_table_set_col_w (Ewl_Table *table, int col, int width)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * set column width of the grid
	 *---------------------------------*/

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_set_col_w (table->grid, col, width);
	ewl_widget_configure(EWL_WIDGET(table));
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_table_get_col_w - get the width of a table column
 * @t: the table
 * @col: the column
 * @width: integer pointer to store the width in
 *
 * Returns no value.
 */
void
ewl_table_get_col_w (Ewl_Table *table, int col, int *width)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * get column width of the grid. 
	 *---------------------------------*/

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_get_col_w (table->grid, col, width);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_table_set_row_h - set the height of a table row
 * @t: the table
 * @row: the row
 * @height: the new height
 *
 * Returns no value.
 */
void
ewl_table_set_row_h (Ewl_Table *table, int row, int height)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * set row height of the grid
	 *---------------------------------*/

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_set_row_h (table->grid, row, height);
	ewl_widget_configure(EWL_WIDGET(table));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_table_get_row_h - get the height of a table row
 * @t: the table
 * @row: the row
 * @height: integer pointer to store the height in
 *
 * Returns no value.
 */
void
ewl_table_get_row_h (Ewl_Table *table, int row, int *height)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * get row height of the grid
	 *---------------------------------*/

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("table", table);

	ewl_grid_get_row_h (table->grid, row, height);

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
void
ewl_table_reset (Ewl_Table *t, int cols, int rows, char **col_headers)
{
	Ewl_Widget *button;
	Ewl_Table_Child *child;
	int i;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);


	ewl_grid_reset(EWL_GRID(t->grid), cols, rows);

	if (col_headers != NULL) {

		for (i = 1; i <= cols; i++) {
			button = ewl_button_new(col_headers[i-1]);
			ewl_object_set_fill_policy(EWL_OBJECT(button), 
					EWL_FILL_POLICY_VSHRINK);
			ewl_widget_disable(button);
			child = __ewl_table_create_child(button);
			ewl_grid_add(t->grid, EWL_WIDGET(child), i, i, 1, 1);
			ewl_widget_show(EWL_WIDGET(child));
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
char *
ewl_table_get_selected (Ewl_Table *t)
{
	Ewl_Text *tw;
	Ewl_Widget *child;
	Ewd_List *children;
	Ewl_Grid_Child *gc;

	DENTER_FUNCTION(DLEVEL_STABLE);

	children = EWL_CONTAINER(t->grid)->children;

	ewd_list_goto_first(children);
	while ((child = ewd_list_next(children)) != NULL)
	{
		gc = (Ewl_Grid_Child *) ewl_widget_get_data(child,
					(void *) t->grid);
	
		if (t->selected.start_c == gc->start_col && 
			t->selected.start_r == gc->start_row &&
			t->selected.end_c == gc->end_col &&
			t->selected.end_r == gc->end_row)
		{
			tw = EWL_TEXT(EWL_BUTTON(
				EWL_TABLE_CHILD(child)->widget)->label_object);
			
			DRETURN_PTR(ewl_text_get_text(tw), DLEVEL_STABLE);

			break;
		}
	}

	DRETURN_PTR("", DLEVEL_STABLE);
}



Ewl_Table_Child *
__ewl_table_create_child(Ewl_Widget *w)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * This function is supposed to create a new container, put the
	 * widget in this container. Then this container will be the
	 * actual child widget we add to the grid.
	 *---------------------------------*/

	Ewl_Table_Child *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	child = NEW(Ewl_Table_Child, 1);
	if (!child)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	memset(child, 0, sizeof(Ewl_Table_Child));

	/*
	 * Initialize the inherited fields
	 */
	ewl_container_init(EWL_CONTAINER(child), 
			"/appearance/box/vertical", NULL, NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(child), EWL_FILL_POLICY_FILL);

	
	/*
	 * Add the widget
	 */
	ewl_container_append_child (EWL_CONTAINER(child), w);
	child->widget = w;

	/*
	 * Add callbacks
	 */
	ewl_callback_append(EWL_WIDGET(child), EWL_CALLBACK_CONFIGURE,
			__ewl_table_child_configure, NULL);
	ewl_callback_append(EWL_WIDGET(child), EWL_CALLBACK_SHOW,
			__ewl_table_child_show, NULL);
	

	
	DRETURN_PTR(EWL_TABLE_CHILD(child), DLEVEL_STABLE);
}



void
__ewl_table_child_show (Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Table_Child *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	
	child = EWL_TABLE_CHILD(w);

	ewl_widget_show(child->widget);
	ewl_widget_configure(child->widget);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__ewl_table_child_configure(Ewl_Widget *w, void *ev_data, void *user_data)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * The purpose of this method is to set the size of the actual
	 * widget inside a grid child container. These widgets will be
	 * resized according to their fill policies and
	 * current/preferred size accordiningly. 
	 *
	 *
	 * Right now I just resize the widgets to take up all available
	 * space but this will be changed as soon as preferred sizing
	 * works according to new design scheme. I also resize the
	 * clipping are of the container to the correct values.
	 *---------------------------------*/

	Ewl_Window *window;
	Ewl_Object *o;
	Ewl_Grid *g;
	Ewl_Table_Child *c;
	Ewl_Grid_Child *gc;
	int width, height, i;
	
	DENTER_FUNCTION(DLEVEL_STABLE);


	o = EWL_OBJECT(w);
	c = EWL_TABLE_CHILD(w);
	g = EWL_GRID(w->parent);


	ewl_object_request_geometry(EWL_OBJECT(c->widget),
			CURRENT_X(o), CURRENT_Y(o),
			CURRENT_W(o) - INSET_LEFT(o) + INSET_RIGHT(o),
			CURRENT_H(o) - INSET_TOP(o) + INSET_BOTTOM(o));
	

	
	gc = (Ewl_Grid_Child *) ewl_widget_get_data(w, (void *) g);
	
	width = height = 0;
	for (i = gc->start_col-1; i < gc->end_col; i++) {
		width += g->col_size[i].size;
		height += g->row_size[i].size;
	}

	window = ewl_window_find_window_by_widget(w);
	evas_resize(window->evas, w->fx_clip_box, width, height);
	

	ewl_widget_configure(EWL_WIDGET(c->widget));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_table_child_select(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Table *t;
	Ewl_Grid_Child *gc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);


	t = EWL_TABLE(user_data);

	gc = (Ewl_Grid_Child *) ewl_widget_get_data(w->parent, (void *)t->grid);

	t->selected.start_r = gc->start_row;
	t->selected.start_c = gc->start_col;
	t->selected.end_r = gc->end_row;
	t->selected.end_c = gc->end_col;


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


	
void
__ewl_table_show(Ewl_Widget *w, void *ev_data, void *user_data)
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

void
__ewl_table_configure(Ewl_Widget *w, void *ev_data, void *user_data)
{
	/*---------------------------------
	 * DEVELOPER NOTE:
	 * Right now all this method does is making sure the grid take
	 * up the space it can. (i.e. the same space as the table
	 * itself). That's it's only purpose I can think of now.
	 *---------------------------------*/

	Ewl_Table *table;
	Ewl_Object *o;
	Ewl_Widget *child;
	
	DENTER_FUNCTION(DLEVEL_STABLE);

	
	table = EWL_TABLE(w);
	o = EWL_OBJECT(w);

	
	ewl_object_request_geometry(EWL_OBJECT(table->grid), 
			CURRENT_X(o), CURRENT_Y(o),
			CURRENT_W(o) - INSET_LEFT(o) + INSET_RIGHT(o),
			CURRENT_H(o) - INSET_TOP(o) + INSET_BOTTOM(o));


	
	ewl_widget_configure(EWL_WIDGET(table->grid));


	ewd_list_goto_first(EWL_CONTAINER(table->grid)->children);
	while((child = ewd_list_next(
			EWL_CONTAINER(table->grid)->children)) != NULL) {

		ewl_widget_configure(child);
	}



	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

