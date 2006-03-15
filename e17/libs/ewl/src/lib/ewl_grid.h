#ifndef EWL_GRID_H
#define EWL_GRID_H

/**
 * @addtogroup Ewl_Grid Ewl_Grid The ewl grid widget
 * @brief The Ewl Grid widget
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

typedef struct Ewl_Grid_Info Ewl_Grid_Info;

struct Ewl_Grid_Info
{
	int		 override;	/**< Override the size of the column */
	int		 size;		/**< The size */
	Ewl_Widget	*max;		/**< The largets child */
	Ecore_List	*cross;		/**< The row of widgets */
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
 * Inherit from Ewl_Container and extend to privide a grid layout widget
 */
struct Ewl_Grid 
{
	Ewl_Container   container;		/**< Inherit from Ewl_Container */

	Ewl_Grid_Info  *col_size;		/**< Horizontal/vertical size of the columns */
	Ewl_Grid_Info  *row_size;		/**< Horizontal/vertical size of the rows */

	int rows;				/**< Row count */
	int cols;				/**< Column count */

	unsigned int homogeneous_h;   		/**< Horizontal homogeneous flag */
	unsigned int homogeneous_v;   		/**< Vertical homogeneous flag */ 

	int grid_h;				/**< Total height of the grid */
	int grid_w;				/**< Total width of the grid */

	Ecore_List *rchildren;			/**< List of old children after a reset call */
};


typedef struct Ewl_Grid_Child Ewl_Grid_Child;

struct Ewl_Grid_Child
{
	int start_col;	/**< The start column */
	int start_row;	/**< The start row */

	int end_col;	/**< The end column */
	int end_row;	/**< The end row */
};

Ewl_Widget     *ewl_grid_new(int cols, int rows);
int             ewl_grid_init(Ewl_Grid *g, int cols, int rows);

void            ewl_grid_add(Ewl_Grid *g, Ewl_Widget *w,
			     int start_col, int end_col, int start_row,
			     int end_row);

void            ewl_grid_col_w_set(Ewl_Grid *g, int col, int width);
void            ewl_grid_row_h_set(Ewl_Grid *g, int row, int height);

void            ewl_grid_col_w_get(Ewl_Grid *g, int col, int *width);
void            ewl_grid_row_h_get(Ewl_Grid *g, int row, int *height);

void            ewl_grid_reset(Ewl_Grid *g, int rows, int cols);

void		ewl_grid_homogeneous_set(Ewl_Grid *g, unsigned int h);
void		ewl_grid_hhomogeneous_set(Ewl_Grid *g, unsigned int h);
void		ewl_grid_vhomogeneous_set(Ewl_Grid *g, unsigned int h);
unsigned int    ewl_grid_hhomogeneous_get(Ewl_Grid *g);
unsigned int 	ewl_grid_vhomogeneous_get(Ewl_Grid *g);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_grid_realize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_grid_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_grid_destroy_cb(Ewl_Widget *w, void *ev_data , void *user_data);
void ewl_grid_child_show_cb(Ewl_Container *p, Ewl_Widget *c);
void ewl_grid_child_resize_cb(Ewl_Container *p, Ewl_Widget *child,
			     int size, Ewl_Orientation o);
void ewl_grid_child_destroy_cb(Ewl_Widget *w, void *ev_data,
                           void *user_data);

/**
 * @}
 */

#endif

