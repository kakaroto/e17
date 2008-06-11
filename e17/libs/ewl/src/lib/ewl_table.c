/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_table.h"
#include "ewl_button.h"
#include "ewl_text.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @param cols: the number of columns
 * @param rows: the number of rows
 * @param col_headers: the column titles
 * @return Returns a a newl table on success, NULL on failure.
 * @brief Create a new table
 */
Ewl_Widget *
ewl_table_new(int cols, int rows, char **col_headers)
{
        Ewl_Table *t;

        DENTER_FUNCTION(DLEVEL_STABLE);

        t = NEW(Ewl_Table, 1);
        if (!t)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_table_init(t, cols, rows, col_headers)) {
                ewl_widget_destroy(EWL_WIDGET(t));
                t = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(t), DLEVEL_STABLE);
}

/**
 * @param t: the table
 * @param cols: the number of columns
 * @param rows: the number of rows
 * @param col_headers: the column titles
 * @return Returns TRUE on success or FALSE on failure.
 * @brief Initialize table to starting values
 *
 * Responsible for setting up default values and callbacks withing the table
 * structure
 */
int
ewl_table_init(Ewl_Table *t, int cols, int rows, char **col_headers)
{
        Ewl_Widget *button;
        Ewl_Cell *cell;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, FALSE);

        /*
         * Iniitialize the tables inherited fields
         */
        if (!ewl_container_init(EWL_CONTAINER(t))) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }
        ewl_widget_appearance_set(EWL_WIDGET(t), EWL_TABLE_TYPE);
        ewl_widget_inherit(EWL_WIDGET(t), EWL_TABLE_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_FILL);

        ewl_container_show_notify_set(EWL_CONTAINER(t), ewl_table_cb_child_show);

        /*
         * Create a new grid
         */
        t->grid = (Ewl_Grid *)ewl_grid_new();
        if (col_headers)
                ewl_grid_dimensions_set(t->grid, cols, rows + 1);
        else
                ewl_grid_dimensions_set(t->grid, cols, rows);
        ewl_container_child_append(EWL_CONTAINER(t), EWL_WIDGET(t->grid));
        ewl_widget_show(EWL_WIDGET(t->grid));

        /*
         * Add the column headers to the grid
         */
        if (col_headers != NULL) {

                for (i = 1; i <= cols; i++) {
                        cell = (Ewl_Cell *) ewl_cell_new();
                        button = ewl_button_new();
                        ewl_button_label_set(EWL_BUTTON(button), col_headers[i - 1]);
                        ewl_widget_disable(button);
                        ewl_container_child_append(EWL_CONTAINER(cell), button);
                        ewl_object_fill_policy_set(EWL_OBJECT(cell),
                                                EWL_FLAG_FILL_VSHRINKABLE | EWL_FLAG_FILL_HFILL);
                        ewl_container_child_append(EWL_CONTAINER(t->grid),
                                                        EWL_WIDGET(cell));
                        ewl_grid_child_position_set(t->grid, EWL_WIDGET(cell),
                                                                i, i, 1, 1);
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
        t->homogeneous_h = FALSE;
        t->homogeneous_v = FALSE;

        /*
         * Append callbacks
         */
        ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_CONFIGURE,
                                        ewl_table_cb_configure, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param table: the table
 * @param w: the cell to add
 * @param start_col: the start column
 * @param end_col: the end column
 * @param start_row: the start row
 * @param end_row: the end row
 * @return Returns no value.
 * @brief Add a child widget to the table
 */
void
ewl_table_add(Ewl_Table *table, Ewl_Widget *w,
                int start_col, int end_col, int start_row, int end_row)
{
        Ewl_Cell *cell;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(table);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(table, EWL_TABLE_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        cell = (Ewl_Cell *)ewl_cell_new ();
        ewl_container_child_append(EWL_CONTAINER (cell), w);

        ewl_container_child_append(EWL_CONTAINER(table->grid),
                                                        EWL_WIDGET(cell));
        if (table->col_headers)
                ewl_grid_child_position_set(table->grid, EWL_WIDGET(cell),
                                start_col, end_col, start_row + 1, end_row + 1);
        else
                ewl_grid_child_position_set(table->grid, EWL_WIDGET(cell),
                                start_col, end_col, start_row, end_row);

        ewl_callback_prepend(EWL_WIDGET(cell), EWL_CALLBACK_MOUSE_UP,
                                ewl_table_cb_child_select, table);

        ewl_widget_show(EWL_WIDGET(cell));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: the table
 * @param cell: the cell to add
 * @param start_col: integer pointer to store the start column
 * @param end_col: integer pointer to store the end column in
 * @param start_row: integer pointer to store the start row in
 * @param end_row: integer pointer to store the end row in
 * @return Returns nothing
 * @brief Get the column and row of a widget
 */
void
ewl_table_col_row_get(Ewl_Table *t, Ewl_Cell *cell,
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
        Ecore_List *children;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_PARAM_PTR(cell);
        DCHECK_TYPE(t, EWL_TABLE_TYPE);
        DCHECK_TYPE(cell, EWL_CELL_TYPE);

        children = EWL_CONTAINER(t->grid)->children;

        ecore_dlist_first_goto(children);
        while ((child = ecore_dlist_next(children)) != NULL)
        {
                if (child == EWL_WIDGET(cell)) {
                        g_child = (Ewl_Grid_Child *) ewl_widget_data_get(child,
                                        (void *) t->grid);

                        if (start_col) *start_col = g_child->start_col;
                        if (end_col) *end_col = g_child->end_col;
                        if (start_row) *start_row = g_child->start_row;
                        if (end_row) *end_row = g_child->end_row;

                        break;
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: the table
 * @param start_col: the start column
 * @param end_col: the end column
 * @param start_row: the start row
 * @param end_row: the end row
 * @return Returns Ecore_List of widgets found in the specified col/row area.
 * @brief Get a list of the widgets in the specified col/row
 */
Ecore_List *
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
        Ecore_List *children;
        Ecore_List *list;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TABLE_TYPE, NULL);

        list = ecore_list_new();
        children = EWL_CONTAINER(t->grid)->children;

        ecore_dlist_first_goto(children);
        while ((child = ecore_dlist_next(children)) != NULL) {
                gc = (Ewl_Grid_Child *) ewl_widget_data_get(child,
                                                (void *) t->grid);

                if (start_col >= gc->start_col && end_col <= gc->end_col &&
                        start_row >= gc->start_row && end_row <= gc->end_row) {
                        ecore_list_append(list, child);
                }
        }

        DRETURN_PTR(list, DLEVEL_STABLE);
}

/**
 * @param table: the table
 * @param col: the column
 * @param width: the new width
 * @return Returns no value.
 * @brief Set the width of a table column
 */
void
ewl_table_column_w_set(Ewl_Table *table, int col, int width)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(table);
        DCHECK_TYPE(table, EWL_TABLE_TYPE);

        ewl_grid_column_fixed_w_set(table->grid, col, width);
        ewl_widget_configure(EWL_WIDGET(table));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param table: the table
 * @param col: the column
 * @param width: integer pointer to store the width in
 * @return Returns no value.
 * @brief Get the width of a table column
 */
void
ewl_table_column_w_get(Ewl_Table *table, int col, int *width)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(table);
        DCHECK_TYPE(table, EWL_TABLE_TYPE);

        if (width) *width = ewl_grid_column_fixed_w_get(table->grid, col);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param table: the table
 * @param row: the row
 * @param height: the new height
 * @return Returns no value.
 * @brief Set the height of a table row
 */
void
ewl_table_row_h_set(Ewl_Table *table, int row, int height)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(table);
        DCHECK_TYPE(table, EWL_TABLE_TYPE);

        ewl_grid_row_fixed_h_set(table->grid, row, height);

        ewl_widget_configure(EWL_WIDGET(table));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param table: the table
 * @param row: the row
 * @param height: integer pointer to store the height in
 * @return Returns no value.
 * @brief Get the height of a table row
 */
void
ewl_table_row_h_get(Ewl_Table *table, int row, int *height)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(table);
        DCHECK_TYPE(table, EWL_TABLE_TYPE);

        if (height) *height = ewl_grid_row_fixed_h_get(table->grid, row);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: the table
 * @param cols: the new number of columns
 * @param rows: the new number of columns
 * @param col_headers: the new column headers
 * @return Returns no value
 * @brief Clear the table and set new geometry
 */
void
ewl_table_reset(Ewl_Table *t, int cols, int rows, char **col_headers)
{
        Ewl_Widget *button;
        Ewl_Cell *cell;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TABLE_TYPE);

        ewl_container_reset(EWL_CONTAINER(t->grid));
        if (col_headers != NULL)
                ewl_grid_dimensions_set(EWL_GRID(t->grid), cols, rows+1);
        else
                ewl_grid_dimensions_set(EWL_GRID(t->grid), cols, rows);

        if (col_headers != NULL) {

                for (i = 1; i <= cols; i++) {
                        cell = (Ewl_Cell *) ewl_cell_new();
                        button = ewl_button_new();
                        ewl_button_label_set(EWL_BUTTON(button), col_headers[i - 1]);
                        ewl_widget_disable(button);
                        ewl_container_child_append(EWL_CONTAINER(cell), button);
                        ewl_container_child_append(EWL_CONTAINER(t->grid),
                                                        EWL_WIDGET(cell));
                        ewl_grid_child_position_set(t->grid, EWL_WIDGET(cell),
                                                                i, i, 1, 1);
                        ewl_widget_show(button);
                        ewl_widget_show(EWL_WIDGET(cell));
                }

                t->col_headers = col_headers;
        }

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: the table
 * @return Returns the text in the currently selected widget in the table
 * @brief Get the text in the current selected box
 */
char *
ewl_table_selected_get(Ewl_Table *t)
{
        Ewl_Text *tw;
        Ewl_Widget *child;
        Ecore_List *children;
        Ewl_Grid_Child *gc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, strdup(""));
        DCHECK_TYPE_RET(t, EWL_TABLE_TYPE, strdup(""));

        children = EWL_CONTAINER(t->grid)->children;
        ecore_dlist_first_goto(children);
        while ((child = ecore_dlist_next(children)) != NULL) {
                gc = (Ewl_Grid_Child *) ewl_widget_data_get(child,
                                                (void *) t->grid);

                if (t->selected.start_c == gc->start_col &&
                                t->selected.start_r == gc->start_row &&
                                t->selected.end_c == gc->end_col &&
                                t->selected.end_r == gc->end_row) {

                        tw = (Ewl_Text *) ecore_dlist_first_goto(
                                        EWL_CONTAINER(child)->children);

                        DRETURN_PTR(ewl_text_text_get(tw), DLEVEL_STABLE);
                }
        }

        DRETURN_PTR(strdup(""), DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: The Ewl_Table
 * @return Returns no value
 * @brief The child select callback
 */
void
ewl_table_cb_child_select(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                        void *user_data)
{
        Ewl_Table *t;
        Ewl_Grid_Child *gc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(user_data, EWL_TABLE_TYPE);

        t = EWL_TABLE(user_data);
        gc = (Ewl_Grid_Child *) ewl_widget_data_get(w, (void *) t->grid);

        t->selected.start_r = gc->start_row;
        t->selected.start_c = gc->start_col;
        t->selected.end_r = gc->end_row;
        t->selected.end_c = gc->end_col;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_table_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        /*---------------------------------
         * DEVELOPER NOTE:
         * Right now all this method does is making sure the grid take
         * up the space it can. (i.e. the same space as the table
         * itself). That's it's only purpose I can think of now.
         *---------------------------------*/

        Ewl_Table *table;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TABLE_TYPE);

        table = EWL_TABLE(w);

        ewl_object_geometry_request(EWL_OBJECT(table->grid),
                                        CURRENT_X(w), CURRENT_Y(w),
                                        CURRENT_W(w) - INSET_LEFT(w) +
                                        INSET_RIGHT(w),
                                        CURRENT_H(w) - INSET_TOP(w) +
                                        INSET_BOTTOM(w));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param p: The container to work with
 * @param c: UNUSED
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_table_cb_child_show(Ewl_Container *p, Ewl_Widget *c __UNUSED__)
{
        Ewl_Table *table;
        int width_g, height_g;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_TYPE(p, EWL_CONTAINER_TYPE);

        table = EWL_TABLE (p);
        ewl_object_preferred_inner_size_get (EWL_OBJECT (table->grid),
                                                        &width_g, &height_g);
        ewl_object_preferred_inner_size_set (EWL_OBJECT (table), width_g, height_g);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param table: the table to change homogeneous layout
 * @param h: the boolean value to change the layout mode to
 * @return Returns no value.
 * @brief Change the homogeneous layout of the box
 *
 * Grids use non-homogeneous layout by default, this can be used
 * to change that.
 */
void
ewl_table_homogeneous_set(Ewl_Table *table, unsigned int h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(table);
        DCHECK_TYPE(table, EWL_TABLE_TYPE);

         if (table->homogeneous_h != h)
                ewl_table_hhomogeneous_set(table, h);
        if (table->homogeneous_v != h)
                ewl_table_vhomogeneous_set(table, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param table: the table to change horizontal homogeneous layout
 * @param h: the boolean value to change the horizontal layout mode to
 * @return Returns no value.
 * @brief Change the horizontal homogeneous layout of the box
 *
 * Grids use non-homogeneous layout by default, this can be used
 * to change that for horizontal orientation, i.e. all columns can
 * have the same width.
 */
void
ewl_table_hhomogeneous_set(Ewl_Table *table, unsigned int h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(table);
        DCHECK_TYPE(table, EWL_TABLE_TYPE);

         if (table->homogeneous_h != h)
        {
                table->homogeneous_h = h;
                ewl_grid_hhomogeneous_set (EWL_GRID (table->grid), h );
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param table: the table to change vertical homogeneous layout
 * @param h: the boolean value to change the vertical layout mode to
 * @return Returns no value.
 * @brief Change the vertical homogeneous layout of the box
 *
 * Grids use non-homogeneous layout by default, this can be used
 * to change that for vertical orientation, i.e. all rows can have
 * the same height.
 */
void
ewl_table_vhomogeneous_set(Ewl_Table *table, unsigned int h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(table);
        DCHECK_TYPE(table, EWL_TABLE_TYPE);

         if (table->homogeneous_v != h)
        {
                table->homogeneous_v = h;
                ewl_grid_vhomogeneous_set (EWL_GRID (table->grid), h );
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param table: the table to get the homogeneous layout
 * @return The horizontal homogeneous flag
 * @brief Retrieves the horizontal homogeneous flag
 */
unsigned int
ewl_table_hhomogeneous_get(Ewl_Table *table)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(table, 0);
        DCHECK_TYPE_RET(table, EWL_TABLE_TYPE, 0);

        DRETURN_INT(table->homogeneous_h, DLEVEL_STABLE);
}

/**
 * @param table: the table to get the vertical layout
 * @return The vertical homogeneous flag
 * @brief Retrieves the vertical homogeneous flag
 */
unsigned int
ewl_table_vhomogeneous_get(Ewl_Table *table)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(table, 0);
        DCHECK_TYPE_RET(table, EWL_TABLE_TYPE, 0);

        DRETURN_INT(table->homogeneous_v, DLEVEL_STABLE);
}

