
#ifndef __EWL_TABLE_H__
#define __EWL_TABLE_H__

typedef struct _ewl_table Ewl_Table;

#define EWL_TABLE(table) ((Ewl_Table *) table)

struct _ewl_table
{
	Ewl_Container container;
	Ebits_Object ebits_bg;
	unsigned int columns;
	unsigned int rows;
	unsigned int homogeneous;
	unsigned int col_spacing;
	unsigned int row_spacing;
	unsigned int *col_w;
	unsigned int *row_h;
};

typedef struct _ewl_table_child Ewl_Table_Child;

#define EWL_TABLE_CHILD(child) ((Ewl_Table_Child *) child)

struct _ewl_table_child
{
	Ewl_Widget *widget;
	unsigned int start_col;
	unsigned int end_col;
	unsigned int start_row;
	unsigned int end_row;
};

Ewl_Widget *ewl_table_new(unsigned int columns, unsigned int rows);

Ewl_Widget *ewl_table_new_all(unsigned int homogeneous,
			      unsigned int columns,
			      unsigned int rows,
			      unsigned int col_spacing,
			      unsigned int row_spacing);

void ewl_table_attach(Ewl_Widget * t,
		      Ewl_Widget * c,
		      unsigned int start_col,
		      unsigned int end_col,
		      unsigned int start_row, unsigned int end_row);

void ewl_table_detach(Ewl_Widget * t, unsigned int c, unsigned int r);

void ewl_table_resize(Ewl_Widget * t, unsigned int c, unsigned int r);

unsigned int ewl_table_get_columns(Ewl_Widget * t);

unsigned int ewl_table_get_rows(Ewl_Widget * t);

void ewl_table_set_homogeneous(Ewl_Widget * t, unsigned int h);

void ewl_table_set_col_spacing(Ewl_Widget * t, unsigned int cs);

void ewl_table_set_row_spacing(Ewl_Widget * t, unsigned int rs);

void ewl_table_column_set_width(Ewl_Widget * t,
				unsigned int c, unsigned int w);

void ewl_table_get_column_width(Ewl_Widget * t,
				unsigned int c, unsigned int *w);

Ewl_Widget *ewl_table_get_child(Ewl_Widget * t,
				unsigned int c, unsigned int r);

void ewl_table_get_row_geometry(Ewl_Widget * t,
				unsigned int r,
				int *x, int *y, int *w, int *h);

#endif /* __EWL_TABLE_H__ */
