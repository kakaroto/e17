#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_grid_resize(Ewl_Grid *g);

/**
 * @param cols: number of columns
 * @param rows: number of rows
 * @return Returns a pointer to a newly allocated grid on success, NULL on failure.
 * @brief Create a new Ewl_Grid widget
 */
Ewl_Widget *
ewl_grid_new(int cols, int rows)
{
	Ewl_Grid *g;

	DENTER_FUNCTION(DLEVEL_STABLE);

	g = NEW(Ewl_Grid, 1);
	if (!g)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_grid_init(g, cols, rows)) {
		ewl_widget_destroy(EWL_WIDGET(g));
		g = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(g), DLEVEL_STABLE);
}


/**
 * @param g: the grid
 * @param cols: number of columns
 * @param rows: number of rows
 * @return Returns no value. Responsible for setting up default values and
 * callbacks within a grid structure
 * @brief Initializes an Ewl_Grid widget to default values
 */
int
ewl_grid_init(Ewl_Grid *g, int cols, int rows)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("g", g, FALSE);

	/*
	 * Initialize the grids inherited fields
	 */
	if (!ewl_container_init(EWL_CONTAINER(g)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(g), EWL_GRID_TYPE);
	ewl_widget_inherit(EWL_WIDGET(g), EWL_GRID_TYPE);

	ewl_container_show_notify_set(EWL_CONTAINER(g), ewl_grid_child_show_cb);
	ewl_container_resize_notify_set(EWL_CONTAINER(g), ewl_grid_child_resize_cb);

	/*
	 * Initialize the lists that keep track of the
	 * horisontal and vertical size of cols/rows
	 */
	g->col_size = NEW(Ewl_Grid_Info, cols);
	if (!g->col_size)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	g->row_size = NEW(Ewl_Grid_Info, rows);
	if (!g->row_size) {
		FREE(g->col_size);
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	/*
	 * Store the cols/rows in the grid
	 */
	g->cols = cols;
	g->rows = rows;

	g->homogeneous_h = FALSE;
	g->homogeneous_v = FALSE;

	/*
	 * Append callbacks
	 */
	ewl_callback_append(EWL_WIDGET(g), EWL_CALLBACK_REALIZE,
			    ewl_grid_realize_cb, NULL);
	ewl_callback_append(EWL_WIDGET(g), EWL_CALLBACK_CONFIGURE,
			    ewl_grid_configure_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(g), EWL_CALLBACK_DESTROY, 
                             ewl_grid_destroy_cb, NULL);

	ewl_widget_focusable_set(EWL_WIDGET(g), FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param g: the grid
 * @param cols: the new number of columns
 * @param rows: the new number of rows
 * @return Returns no value
 * @brief Clears the grid and sets new geometry
 */
void
ewl_grid_reset(Ewl_Grid *g, int cols, int rows)
{
	Ewl_Widget *w;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

	w = EWL_WIDGET(g);

	ewl_container_reset(EWL_CONTAINER(w));

	IF_FREE(g->col_size);
	IF_FREE(g->row_size);

	g->col_size = NEW(Ewl_Grid_Info, cols);
	if (!g->col_size)
		DRETURN(DLEVEL_STABLE);

	g->row_size = NEW(Ewl_Grid_Info, rows);
	if (!g->row_size) {
		FREE(g->col_size);
		DRETURN(DLEVEL_STABLE);
	}

	g->cols = cols;
	g->rows = rows;

	/* store the total size of the grid widget */
	g->grid_w = CURRENT_W(EWL_OBJECT(w));
	g->grid_h = CURRENT_H(EWL_OBJECT(w));

	/* initialize the column width to default values */
	for (i = 0; i < g->cols; i++) {
		if (g->homogeneous_h)
			g->col_size[i].size = CURRENT_W(g) / g->cols;
		else
			g->col_size[i].size = 1;
	}

	/* initialize the row height to default values */
	for (i = 0; i < g->rows; i++) {
		if (g->homogeneous_v)
			g->row_size[i].size = CURRENT_H(g) / g->rows;
		else
			g->row_size[i].size = 1;
	}

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: the grid to change homogeneous layout 
 * @param h: the boolean value to change the layout mode to
 * @return Returns no value.
 * @brief Change the homogeneous layout of the grid
 *
 * Grids use non-homogeneous layout by default, this can be used
 * to change that. 
 */
void
ewl_grid_homogeneous_set(Ewl_Grid *g, unsigned int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

 	if (g->homogeneous_h != h)
		ewl_grid_hhomogeneous_set(g, h);
	if (g->homogeneous_v != h)
		ewl_grid_vhomogeneous_set(g, h);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: the grid to change horizontal homogeneous layout 
 * @param h: the boolean value to change the horizontal layout mode to
 * @return Returns no value.
 * @brief Change the horizontal homogeneous layout of the box
 *
 * Grids use non-homogeneous layout by default, this can be used
 * to change that for horizontal orientation, i.e. all columns can
 * have the same width. 
 */
void
ewl_grid_hhomogeneous_set(Ewl_Grid *g, unsigned int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

 	if (g->homogeneous_h != h)
		g->homogeneous_h = h; 
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: the grid to change vertical homogeneous layout 
 * @param h: the boolean value to change the vertical layout mode to
 * @return Returns no value.
 * @brief Change the vertical homogeneous layout of the box
 *
 * Grids use non-homogeneous layout by default, this can be used
 * to change that for vertical orientation, i.e. all rows can have 
 * the same height. 
 */
void
ewl_grid_vhomogeneous_set(Ewl_Grid *g, unsigned int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

 	if (g->homogeneous_v != h)
		g->homogeneous_v = h; 
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: the grid to get the homogeneous layout 
 * @return The horizontal homogeneous flag 
 * @brief Retrieves the horizontal homogeneous flag  
 */
unsigned int 
ewl_grid_hhomogeneous_get(Ewl_Grid *g)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("g", g, 0);
	DCHECK_TYPE_RET("g", g, EWL_GRID_TYPE, 0);

	DRETURN_INT(g->homogeneous_h, DLEVEL_STABLE); 
}

/**
 * @param g: the grid to get the vertical layout 
 * @return The vertical homogeneous flag 
 * @brief Retrieves the vertical homogeneous flag  
 */
unsigned int
ewl_grid_vhomogeneous_get(Ewl_Grid *g)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("g", g, 0);
	DCHECK_TYPE_RET("g", g, EWL_GRID_TYPE, 0);

	DRETURN_INT(g->homogeneous_v, DLEVEL_STABLE); 
}

/**
 * @param g: the grid
 * @param w: the child widget
 * @param start_col: the start column
 * @param end_col: the end column
 * @param start_row: the start row
 * @param end_row: the end row
 * @return Returns no value
 * @brief Add a child widget to the grid
 */
void
ewl_grid_add(Ewl_Grid *g, Ewl_Widget *w,
	     int start_col, int end_col, int start_row, int end_row)
{
	Ewl_Grid_Child *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/*
	 * check bounds
	 */
	if (start_col < 1) {
		printf("start_col out of bounds. min is 1\n");
		DLEAVE_FUNCTION(DLEVEL_STABLE);
	}
	if (end_col > g->cols) {
		printf("end_col out of bounds. max is %d\n", g->cols);
		DLEAVE_FUNCTION(DLEVEL_STABLE);
	}
	if (start_row < 1) {
		printf("start_row out of bounds. min is 1\n");
		DLEAVE_FUNCTION(DLEVEL_STABLE);
	}
	if (end_row > g->rows) {
		printf("end_row out of bounds. max is %d\n", g->rows);
		DLEAVE_FUNCTION(DLEVEL_STABLE);
	}

	/* create a new child */
	child = NEW(Ewl_Grid_Child, 1);
	if (!child)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	child->start_col = start_col;
	child->end_col = end_col;
	child->start_row = start_row;
	child->end_row = end_row;

	/* store the child info in the child widget */
	ewl_widget_data_set(w, (void *) g, child);
	ewl_container_child_append(EWL_CONTAINER(g), w);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_DESTROY, 
                             ewl_grid_child_destroy_cb, g);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: the grid
 * @param col: the column
 * @param width: the new width
 * @return Returns no value.
 * @brief Set the widget of a column
 */
void
ewl_grid_col_w_set(Ewl_Grid *g, int col, int width)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

	/*
	 * check bounds
	 */
	if (col < 1 || col > g->cols) {
		printf("parameter 'col' is out of bounds\n");
		DLEAVE_FUNCTION(DLEVEL_STABLE);
	}

	g->col_size[col - 1].override = 1;
	ewl_object_size_request(EWL_OBJECT(g),
				ewl_object_current_w_get(EWL_OBJECT(g)) +
				(width - g->col_size[col - 1].size),
				ewl_object_current_h_get(EWL_OBJECT(g)));

	g->col_size[col - 1].size = width -	/* this will be reverted in resize */
	    ((ewl_object_current_w_get(EWL_OBJECT(g)) - g->grid_w) / g->cols);

	ewl_widget_configure(EWL_WIDGET(g));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: the grid
 * @param col: the column
 * @param width: integer pointer to store the width in
 * @return Returns no value.
 * @brief Get the width of a column
 */
void
ewl_grid_col_w_get(Ewl_Grid *g, int col, int *width)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_PARAM_PTR("width", width);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

	*width = g->col_size[col].size;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: the grid
 * @param row: the row
 * @param height: the new height
 * @return Returns no value.
 * @brief Set the height of a row
 */
void
ewl_grid_row_h_set(Ewl_Grid *g, int row, int height)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

	/* check bounds */
	if ((row < 1) || (row > g->rows)) {
		DLEAVE_FUNCTION(DLEVEL_STABLE);
	}

	g->row_size[row - 1].override = 1;
	ewl_object_size_request(EWL_OBJECT(g),
				ewl_object_current_w_get(EWL_OBJECT(g)),
				ewl_object_current_h_get(EWL_OBJECT(g)) +
				(height - g->row_size[row - 1].size));

	g->row_size[row - 1].size = height -	/* this will be reverted in resize */
	    ((ewl_object_current_h_get(EWL_OBJECT(g)) - g->grid_h) / g->rows);

	ewl_widget_configure(EWL_WIDGET(g));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: the grid
 * @param row: the row
 * @param height: integer pointer to store the height in
 * @return Returns no value.
 * @brief Get the height of a row
 */
void
ewl_grid_row_h_get(Ewl_Grid *g, int row, int *height)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_PARAM_PTR("height", height);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

	*height = g->row_size[row].size;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_grid_realize_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Grid *g;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	g = EWL_GRID(w);

	/* store the total size of the grid widget */
	g->grid_w = CURRENT_W(EWL_OBJECT(w));
	g->grid_h = CURRENT_H(EWL_OBJECT(w));

	/* initialize the column width to default values */
	for (i = 0; i < g->cols; i++)
		g->col_size[i].size = g->grid_w / g->cols;

	/* initialize the row height to default values */
	for (i = 0; i < g->rows; i++)
		g->row_size[i].size = g->grid_h / g->rows;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_grid_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Grid *g;
	Ewl_Grid_Child *c;
	Ewl_Widget *child;
	int c_w = 0, c_h = 0;	/* child width/height */
	int c_x = 0, c_y = 0;	/* child x/y coordinate */
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	g = EWL_GRID(w);

	ewl_grid_resize(g);

	c_x = CURRENT_X(EWL_OBJECT(w));
	c_y = CURRENT_Y(EWL_OBJECT(w));

	ecore_dlist_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ecore_dlist_next(EWL_CONTAINER(w)->children)) != NULL) {
		c = (Ewl_Grid_Child *) ewl_widget_data_get(child, (void *) g);

		/* calculate child widgets width */
		for (i = c->start_col - 1; i < c->end_col; i++)
			c_w += g->col_size[i].size;

		/* calculate child widgets height */
		for (i = c->start_row - 1; i < c->end_row; i++)
			c_h += g->row_size[i].size;

		/* calculate child widgets x coordinate */
		for (i = 0; i < (c->start_col - 1); i++)
			c_x += g->col_size[i].size;

		/* calculate child widgets y coordinate */
		for (i = 0; i < (c->start_row - 1); i++)
			c_y += g->row_size[i].size;


		ewl_object_place(EWL_OBJECT(child), c_x, c_y, c_w, c_h);
		ewl_widget_configure(child);

		/* reset geometry values for the next child */
		c_x = CURRENT_X(EWL_OBJECT(w));
		c_y = CURRENT_Y(EWL_OBJECT(w));
		c_w = 0;
		c_h = 0;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_grid_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
		void *user_data __UNUSED__)
{
	Ewl_Grid *g;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	g = EWL_GRID(w);

	IF_FREE(g->col_size)
	IF_FREE(g->row_size)
	g->col_size = NULL;
	g->row_size = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_grid_child_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
		void *user_data)
{
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	child = (Ewl_Widget *)ewl_widget_data_get(w, user_data);

	IF_FREE(child)

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_grid_resize(Ewl_Grid *g)
{
	int             w_flag = 0, h_flag = 0;
	int             i, new_w = 0, new_h = 0;
	int             left_over, left_over2;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_GRID_TYPE);

	/* store the total size of the grid widget */
	if (ewl_object_current_w_get(EWL_OBJECT(g)) != g->grid_w) {
		new_w = ewl_object_current_w_get(EWL_OBJECT(g));
		w_flag = 1;
	}

	if (ewl_object_current_h_get(EWL_OBJECT(g)) != g->grid_h) {
		new_h = ewl_object_current_h_get(EWL_OBJECT(g));
		h_flag = 1;
	}

	/* 
	 * if grid with has changed we need to store the new column
	 * width
	 */
	if (w_flag) {
		for (i = 0; i < g->cols; i++) {
			g->col_size[i].size += ((new_w - g->grid_w) / g->cols);
		}
		g->grid_w = new_w;
	}

	/*
	 * if grid height has changed we need to store the new row
	 * height
	 */
	if (h_flag) {
		for (i = 0; i < g->rows; i++) {
			g->row_size[i].size += ((new_h - g->grid_h) / g->rows);
		}
		g->grid_h = new_h;
	}

	/*
	 * since the above set values may be doubles rounded down there
	 * might be some more space to fill at the right and bottom.
	 * this claims the left over space
	 */
	left_over = g->grid_w;
	for (i = 0; i < g->cols; i++)
		left_over -= g->col_size[i].size;
	if (g->cols == 0)
		g->cols = 1;
	while (left_over != 0) {
		if (left_over > 0) {
			g->col_size[left_over % g->cols].size += 1;
			left_over--;
		} else if (left_over < 0) {
			left_over2 = 0 - left_over;
			g->col_size[left_over2 % g->cols].size -= 1;
			left_over++;
		}
	}

	left_over = g->grid_h;
	for (i = 0; i < g->rows; i++)
		left_over -= g->row_size[i].size;
	if (g->rows == 0)
		g->rows = 1;
	while (left_over != 0) {
		if (left_over > 0) {
			g->row_size[left_over % g->rows].size += 1;
			left_over--;
		} else if (left_over < 0) {
			left_over2 = 0 - left_over;
			g->row_size[left_over2 % g->rows].size -= 1;
			left_over++;
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Notify the grid that a child has been added.
 */
void
ewl_grid_child_show_cb(Ewl_Container *p, Ewl_Widget *c)
{
	int i;
	int temp;
	int max_w = 0, max_h = 0; 
	int g_w = 0, g_h = 0; 
	Ewl_Grid *g;
	Ewl_Grid_Child *cdata;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("p", p, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("c", c, EWL_WIDGET_TYPE);

	g = EWL_GRID(p);
	cdata = ewl_widget_data_get(c, (void *) g);

	/*
	 * If no data exists, we should add some defaults.
	 */
	if (!cdata) {
		cdata = NEW(Ewl_Grid_Child, 1);
		if (!cdata)
			DRETURN(DLEVEL_STABLE);
		cdata->start_col = cdata->end_col = 1;
		cdata->start_row = cdata->end_row = 1;

		ewl_widget_data_set(c, g, cdata);
	}

	/*
	 * Add the widget to columns that it intersects.
	 */
	for (i = cdata->start_col; i <= cdata->end_col; i++) {
		if (i < cdata->end_col) {
			if (!g->col_size[i].cross)
				g->col_size[i].cross = ecore_list_new();

			ecore_list_append(g->col_size[i].cross, c);
		}

		/*
	 	* Calculate the amount of space the widget would
	 	* need in this column.
	 	*/
		temp = ewl_object_preferred_w_get(EWL_OBJECT(c)) /
	  	  	(cdata->end_col - cdata->start_col + 1);

		/*
	 	 * Give the column a new preferred size based on 
	 	 * the added widget.
		 */
		if (g->col_size[i-1].size < temp) {
			if (!g->col_size[i-1].override)
				g->col_size[i-1].size = temp;

			/*
			 * Save a pointer to the largest child.
			 */
			g->col_size[i-1].max = c;
		}
		max_w = MAX(max_w, temp) ; 
		max_w = MAX(max_w, g->col_size[i-1].size);
	}

	/*
	 * Add the widget to rows that it intersects.
	 */
	for (i = cdata->start_row; i <= cdata->end_row; i++) {
		if (i < cdata->end_row) {
			if (!g->row_size[i].cross)
				g->row_size[i].cross = ecore_list_new();

			ecore_list_append(g->row_size[i].cross, c);
		}

		/*
		 * Calculate the amount of space the widget would
		 * need in this row.
		 */
		temp = ewl_object_preferred_h_get(EWL_OBJECT(c)) /
		    	(cdata->end_row - cdata->start_row + 1);

		/*
		 * Give the row a new preferred size based on
		 * the added  widget.
		 */
		if (g->row_size[i-1].size < temp) {
			if (!g->row_size[i-1].override)
				g->row_size[i-1].size = temp;

			/*
			 * Save a pointer to the largest child.
			 */
			g->row_size[i-1].max = c;
		}
		max_h = MAX(max_h, temp) ; 
		max_h = MAX(max_h, g->row_size[i-1].size);
	}
	
	for (i = 0; i < g->cols; i++) {
		if (g->homogeneous_h) {
			g->col_size[i].size = max_w; 
		}
		g_w += g->col_size[i].size;
	}
	for (i = 0; i < g->rows; i++) {
		if (g->homogeneous_v) {
			g->row_size[i].size = max_h; 
		}
		g_h += g->row_size[i].size; 
	}
	g->grid_w = g_w; 
	g->grid_h = g_h;
	ewl_object_preferred_inner_size_set(EWL_OBJECT(g), g_w, g_h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Catch notification of child resizes.
 */
void
ewl_grid_child_resize_cb(Ewl_Container *p, Ewl_Widget *child, int size,
		        Ewl_Orientation o)
{
	int give;
	Ewl_Grid *g;
	int used = 0;
	int start_off, end_off;
	Ewl_Grid_Info *info;
	int i, num_spread = 1;
	Ewl_Grid_Child *cdata;
	int (*widget_size) (Ewl_Object * o);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("p", p, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("child", child, EWL_WIDGET_TYPE);

	g = EWL_GRID(p);
	cdata = ewl_widget_data_get(child, (void *) g);

	/*
	 * Setup a couple orientation specific variables.
	 */
	if (o == EWL_ORIENTATION_HORIZONTAL) {
		info = g->col_size;
		start_off = cdata->start_col;
		end_off = cdata->end_col;
		widget_size = ewl_object_preferred_w_get;
	} else {
		info = g->row_size;
		start_off = cdata->start_row;
		end_off = cdata->end_row;
		widget_size = ewl_object_preferred_h_get;
	}

	/*
	 * Count the number of resizable columns to give the size change to.
	 */
	for (i = start_off; i < end_off; i++) {
		if (!info[i].override)
			num_spread++;
	}

	/*
	 * No need to continue if none of the grid spaces will accept space.
	 */
	if (!num_spread)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Give out space to each of the grid spaces that will accept it.
	 */
	give = size / num_spread;
	for (i = start_off; i < end_off && num_spread; i++) {
		if (!info[i].override) {

			/*
			 * This case is simple, just add in the amount
			 * specified.
			 */
			if (child == info[i].max && give > 0) {
				info[i].size += give;
				used += give;
				num_spread--;
			} else {
				int             max = 0;
				Ewl_Widget     *temp;

				/*
				 * Otherwise we need to search for the largest
				 * widget in this space in the grid.
				 */
				ecore_list_goto_first(info[i].cross);
				while ((temp = ecore_list_next(info[i].cross))) {
					if (widget_size(EWL_OBJECT(temp)) > max) {
						max = widget_size(EWL_OBJECT
								  (temp));
						info[i].max = temp;
					}
				}
			}
		}
	}

	/*
	 * Hand out any remaining space available.
	 */
	info[i - 1].size += size % num_spread;
	used += size % num_spread;

	/*
	 * Now resize the grid appropriately.
	 */
	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_object_w_request(EWL_OBJECT(g), CURRENT_W(g) -
				     (INSET_LEFT(g) + INSET_RIGHT(g)) + used);
	else
		ewl_object_h_request(EWL_OBJECT(g), CURRENT_H(g) -
				     (INSET_TOP(g) + INSET_BOTTOM(g)) + used);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

