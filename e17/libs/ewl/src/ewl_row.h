#ifndef _EWL_ROW_H
#define _EWL_ROW_H

typedef struct _ewl_row Ewl_Row;
#define EWL_ROW(t) ((Ewl_Row *)t)

/*
 * Tree rows layout cells in a left to right fashion.
 */
struct _ewl_row
{
	Ewl_Container container;
};

Ewl_Widget *ewl_row_new();
int ewl_row_init(Ewl_Row *row);

#endif
