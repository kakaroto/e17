#ifndef _EWL_CELL_H
#define _EWL_CELL_H

typedef struct _ewl_cell Ewl_Cell;
#define EWL_CELL(t) ((Ewl_Cell *)t)

/*
 * The cell inherits from the container for packing widgets inside the
 * cell.
 */
struct _ewl_cell
{
	Ewl_Container container;
	int col, row;
};

Ewl_Widget *ewl_cell_new();
int ewl_cell_init(Ewl_Cell *cell);

#endif
