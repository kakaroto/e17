
#ifndef __EWL_TABLE_H
#define __EWL_TABLE_H

struct _ewl_table {
	Ewl_Widget widget;
	Ebits_Object ebits_object;
	unsigned int columns;		/* Number of columns */
	unsigned int rows;			/* Number of rows */
	unsigned int homogeneous;
	unsigned int col_spacing;
	unsigned int row_spacing;
};

typedef struct _ewl_table Ewl_Table;

struct _ewl_table_child {
    Ewl_Widget * child;
    unsigned int start_col;
    unsigned int end_col;
    unsigned int start_row;
    unsigned int end_row;
};

typedef struct _ewl_table_child Ewl_Table_Child;

#define EWL_TABLE(table) ((Ewl_Table *) table)

/* Create a new table wich you do not know the size of yet nor row spacing */
Ewl_Widget * ewl_table_new();

/* Create a table setting everything */
Ewl_Widget * ewl_table_new_all(unsigned int homogeneous,
							   unsigned int columns,
							   unsigned int rows,
							   unsigned int col_spacing,
							   unsigned int row_spacing);

/* Resize the table */
void ewl_table_resize(Ewl_Widget * widget,
					  unsigned int rows,
					  unsigned int columns);


/* Add a child to the table */
void ewl_table_attach(Ewl_Widget * table,
					  Ewl_Widget * child,
					  unsigned int start_col,
					  unsigned int end_col,
					  unsigned int start_row,
					  unsigned int end_row);

void ewl_table_detach(Ewl_Widget * table,
					  unsigned int start_row,
					  unsigned int start_col);

/* Set wether or not the table will set all columns to the widest column */
void ewl_table_set_homogeneous(Ewl_Widget * widget,
							   unsigned int homogeneous);

/* Space between each column */
void ewl_table_set_col_spacing(Ewl_Widget * widget,
							   unsigned int spacing);

/* Space between each row */
void ewl_table_set_row_spacing(Ewl_Widget * widget,
							   unsigned int spacing);

#endif
