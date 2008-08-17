/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TABLE_H
#define EWL_TABLE_H

#include "ewl_cell.h"
#include "ewl_grid.h"

/**
 * @addtogroup Ewl_Table Ewl_Table: The Table Layout Container.
 * Defines the Ewl_Box class used for laying out Ewl_Widget's in a
 * horizontal or vertical line.
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Table_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /table/file
 * @themekey /table/group
 */

/**
 * @def EWL_TABLE_TYPE
 * The type name for the Ewl_Table widget
 */
#define EWL_TABLE_TYPE "table"

/**
 * @def EWL_TABLE_IS(w)
 * Returns TRUE if the widget is an Ewl_Table, FALSE otherwise
 */
#define EWL_TABLE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TABLE_TYPE))

/**
 * The table widget is an Ewl_Container used to lay out widgets in a grid like
 * pattern with headers and alignment.
 */
typedef struct Ewl_Table Ewl_Table;

/**
 * @def EWL_TABLE(table)
 * Typecase a pointer to an Ewl_Table pointer.
 */
#define EWL_TABLE(table) ((Ewl_Table *)table)

/**
 * @brief Inherits from Ewl_Container and extends to provide a table layout
 */
struct Ewl_Table
{
        Ewl_Container container;                /**< Inherit from Ewl_Container */
        Ewl_Grid *grid;                        /**< Table uses a grid based layout */

        char **col_headers;                /**< The column headers */
        int row_select;                /**< boolean: select entire rows */

        unsigned int homogeneous_h;           /**< Horizontal homogeneous flag */
        unsigned int homogeneous_v;           /**< Vertical homogeneous flag */

        struct {
                int start_r;                        /**< Selection start row */
                int start_c;                        /**< Selection start column */
                int end_r;                        /**< Selection end row */
                int end_c;                        /**< Selection end column */
        } selected;                                /**< Currently selected rows */

};

Ewl_Widget      *ewl_table_new(int cols, int rows, char **col_headers);
int              ewl_table_init(Ewl_Table *t, int cols, int rows,
                                char **col_headers);

void             ewl_table_add(Ewl_Table *table, Ewl_Widget *w, int start_col,
                                int end_col, int start_row, int end_row);
void             ewl_table_reset(Ewl_Table *t, int cols, int rows,
                                char **c_headers);

void             ewl_table_column_w_set(Ewl_Table *table, int col, int width);
void             ewl_table_row_h_set(Ewl_Table *table, int row, int height);

void             ewl_table_column_w_get(Ewl_Table *table, int col, int *width);
void             ewl_table_row_h_get(Ewl_Table *table, int row, int *height);

void             ewl_table_col_row_get(Ewl_Table *table, Ewl_Cell *cell,
                                int *start_col, int *end_col,
                                int *start_row, int *end_row);

Ecore_List      *ewl_table_find(Ewl_Table *table, int start_col,
                               int end_col, int start_row, int emd_row);

char            *ewl_table_selected_get(Ewl_Table *table);
void             ewl_table_homogeneous_set(Ewl_Table *table, unsigned int h);
void             ewl_table_hhomogeneous_set(Ewl_Table *table, unsigned int h);
void             ewl_table_vhomogeneous_set(Ewl_Table *table, unsigned int h);
unsigned int     ewl_table_hhomogeneous_get(Ewl_Table *table);
unsigned int     ewl_table_vhomogeneous_get(Ewl_Table *table);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_table_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_table_cb_child_select(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_table_cb_child_show(Ewl_Container *p, Ewl_Widget *c);

/**
 * @}
 */

#endif
