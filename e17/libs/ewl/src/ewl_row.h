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
	Ewl_Object *max;

	Ewl_Row *header;
};

Ewl_Widget *ewl_row_new();
int ewl_row_init(Ewl_Row *row);

void ewl_row_set_header(Ewl_Row *row, Ewl_Row *header);
Ewl_Widget *ewl_row_get_column(Ewl_Row *row, short n);

#endif
