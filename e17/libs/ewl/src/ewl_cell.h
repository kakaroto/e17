#ifndef _EWL_CELL_H
#define _EWL_CELL_H

/**
 * @defgroup Ewl_Cell The Cell Container, Layout for a Single Widget
 * Defines a container to layout a single child with all of it's available
 * space.
 *
 * @{
 */

typedef struct _ewl_cell Ewl_Cell;
#define EWL_CELL(t) ((Ewl_Cell *)t)

/**
 * @struct Ewl_Cell
 * The cell inherits from the container for packing widgets inside the
 * cell.
 */
struct Ewl_Cell
{
	Ewl_Container container;
	int col, row;
};

Ewl_Widget *ewl_cell_new();
int ewl_cell_init(Ewl_Cell *cell);

/**
 * @}
 */

#endif
