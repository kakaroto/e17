/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_GRID_H
#define EWL_GRID_H

/**
 * @addtogroup Ewl_Grid Ewl_Grid: The ewl grid widget
 * @brief The Ewl Grid widget
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Grid_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /grid/file
 * @themekey /grid/group
 */

/**
 * @def EWL_GRID_TYPE
 * The type name for the Ewl_Grid widget
 */
#define EWL_GRID_TYPE "grid"

/**
 * @def EWL_GRID_IS(w)
 * Returns TRUE if the widget is an Ewl_Grid, FALSE otherwise
 */
#define EWL_GRID_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_GRID_TYPE))

/**
 * The Ewl_Grid_Info structure
 */
typedef struct Ewl_Grid_Info Ewl_Grid_Info;

/**
 * @brief Contains information about a row or column of Ewl_Grid
 */
struct Ewl_Grid_Info
{
        int current_size;                /**< the current size */
        int current_pos;                 /**< the current position */
        int preferred_size;                /**< The greatest preferred size of a widget inside */
        union {
                int size;                /**< The size set by the user */
                float rel_size;                /**< The relative size */
        } user;                                /**< The user set values for the grid */

        Ewl_Grid_Resize_Type resize_type; /**< Are there values set by the user */
};

/**
 * The Ewl_Grid widget
 */
typedef struct Ewl_Grid Ewl_Grid;

/**
 * @def EWL_GRID(grid)
 * Typecast a pointer to an Ewl_Grid pointer
 */
#define EWL_GRID(grid) ((Ewl_Grid *)grid)

/**
 * @brief Inherit from Ewl_Container and extend to privide a grid layout widget
 */
struct Ewl_Grid
{
        Ewl_Container container;                /**< Inherit from Ewl_Container */

        Ewl_Grid_Info *col_size;                /**< Horizontal/vertical size of the columns */
        Ewl_Grid_Info *row_size;                /**< Horizontal/vertical size of the rows */
        unsigned char data_dirty:1;                /**< flag if the size of the columns and rows must
                                                         be recalculated */
        unsigned char homogeneous_h:1;          /**< Horizontal homogeneous flag */
        unsigned char homogeneous_v:1;           /**< Vertical homogeneous flag */

        Ewl_Orientation orientation;                /**< The grid orientation */

        int rows;                                /**< Row count */
        int cols;                                /**< Column count */
        int space;                                /**< Space count */

        char *map;                                /**< Map of the child that have a postion */
};

/**
 * The Ewl_Grid_Child type
 */
typedef struct Ewl_Grid_Child Ewl_Grid_Child;

/**
 * @brief Contains information about a grid child
 */
struct Ewl_Grid_Child
{
        int start_col;        /**< The start column */
        int start_row;        /**< The start row */

        int end_col;        /**< The end column */
        int end_row;        /**< The end row */
};

Ewl_Widget      *ewl_grid_new(void);
int              ewl_grid_init(Ewl_Grid *g);

void             ewl_grid_child_position_set(Ewl_Grid *g, Ewl_Widget *child,
                                int start_col, int end_col, int start_row,
                                int end_row);
void             ewl_grid_child_position_get(Ewl_Grid *g, Ewl_Widget *child,
                                int *start_col, int *end_col, int *start_row,
                                int *end_row);

void             ewl_grid_dimensions_set(Ewl_Grid *g, int col, int row);
void             ewl_grid_dimensions_get(Ewl_Grid *g, int *col, int *row);

int              ewl_grid_column_current_w_get(Ewl_Grid *g, int col);
void             ewl_grid_column_fixed_w_set(Ewl_Grid *g, int col, int width);
int              ewl_grid_column_fixed_w_get(Ewl_Grid *g, int col);
void             ewl_grid_column_relative_w_set(Ewl_Grid *g, int col,
                                float relw);
float            ewl_grid_column_relative_w_get(Ewl_Grid *g, int col);
void             ewl_grid_column_preferred_w_use(Ewl_Grid *g, int col);
void             ewl_grid_column_w_remove(Ewl_Grid *g, int col);

int              ewl_grid_row_current_h_get(Ewl_Grid *g, int row);
void             ewl_grid_row_fixed_h_set(Ewl_Grid *g, int row, int height);
int              ewl_grid_row_fixed_h_get(Ewl_Grid *g, int row);
void             ewl_grid_row_relative_h_set(Ewl_Grid *g, int col, float relh);
float            ewl_grid_row_relative_h_get(Ewl_Grid *g, int col);
void             ewl_grid_row_preferred_h_use(Ewl_Grid *g, int col);
void             ewl_grid_row_h_remove(Ewl_Grid *g, int row);

void             ewl_grid_orientation_set(Ewl_Grid *g,
                                Ewl_Orientation orientation);
Ewl_Orientation  ewl_grid_orientation_get(Ewl_Grid *g);

void             ewl_grid_homogeneous_set(Ewl_Grid *g, unsigned int h);

void             ewl_grid_hhomogeneous_set(Ewl_Grid *g, unsigned int h);
unsigned int     ewl_grid_hhomogeneous_get(Ewl_Grid *g);

void             ewl_grid_vhomogeneous_set(Ewl_Grid *g, unsigned int h);
unsigned int     ewl_grid_vhomogeneous_get(Ewl_Grid *g);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_grid_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_grid_cb_destroy(Ewl_Widget *w, void *ev_data , void *user_data);
void ewl_grid_cb_child_add(Ewl_Container *p, Ewl_Widget *c);
void ewl_grid_cb_child_remove(Ewl_Container *p, Ewl_Widget *c, int idx);
void ewl_grid_cb_child_show(Ewl_Container *p, Ewl_Widget *child);
void ewl_grid_cb_child_resize(Ewl_Container *p, Ewl_Widget *child,
                                                int size, Ewl_Orientation o);

/**
 * @}
 */

#endif

