#ifndef EWL_CELL_H
#define EWL_CELL_H

/**
 * @addtogroup Ewl_Cell Ewl_Cell: The Cell Container, Layout for a Single Widget
 * Defines a container to layout a single child with all of it's available space.
 *
 * @{
 */

/**
 * @themekey /cell/file
 * @themekey /cell/group
 */

/**
 * @def EWL_CELL_TYPE
 * The type name for the Ewl_Cell widget
 */
#define EWL_CELL_TYPE "cell"

typedef struct Ewl_Cell Ewl_Cell;

/**
* @def EWL_CELL(t)
* Typecast a pointer to an Ewl_Cell pointer.
*/
#define EWL_CELL(t) ((Ewl_Cell *)t)

/**
 * The cell inherits from the container for packing widgets inside the
 * cell.
 */
struct Ewl_Cell
{
	Ewl_Container container; /**< Inherit from the Ewl_Container */
};

Ewl_Widget 	*ewl_cell_new(void);
int 		 ewl_cell_init(Ewl_Cell *cell);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_cell_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_cell_child_show_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_cell_child_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
							Ewl_Orientation o);

/**
 * @}
 */

#endif
