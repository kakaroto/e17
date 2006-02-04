#ifndef __EWL_TABLE_H__
#define __EWL_TABLE_H__

/**
 * @file ewl_table.h
 * @defgroup Ewl_Table Table: The Table Layout Container.
 * @brief Defines the Ewl_Box class used for laying out Ewl_Widget's in a
 * horizontal or vertical line.
 *
 * @{
 */

/**
 * @themekey /table/file
 * @themekey /table/group
 */

#define EWL_TABLE_TYPE "table"

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

struct Ewl_Table
{
	Ewl_Container   container;

	Ewl_Grid       *grid;

	char          **col_headers;

	int             row_select;	/* boolean: select entire rows */

        /*
         * Flag indicating space assignment 
         */
        unsigned int    homogeneous_h;   /** Horizontal homogeneous flag */
        unsigned int    homogeneous_v;   /** Vertical homogeneous flag */ 

	struct {
		int start_r;
		int start_c;
		int end_r;
		int end_c;
	} selected;

};

Ewl_Widget     *ewl_table_new(int cols, int rows, char **col_headers);
int             ewl_table_init(Ewl_Table *t, int cols, int rows,
			       char **col_headers);
void            ewl_table_add(Ewl_Table *table, Ewl_Widget *w, int start_col,
			      int end_col, int start_row, int end_row);
void            ewl_table_reset(Ewl_Table *t, int cols, int rows,
				char **c_headers);
void            ewl_table_row_select(Ewl_Table *t, int boolean);

void            ewl_table_col_w_set(Ewl_Table *table, int col, int width);
void            ewl_table_row_h_set(Ewl_Table *table, int row, int height);

void            ewl_table_col_w_get(Ewl_Table *table, int col, int *width);
void            ewl_table_row_h_get(Ewl_Table *table, int row, int *height);

void            ewl_table_col_row_get(Ewl_Table *table, Ewl_Cell *cell,
				      int *start_col, int *end_col,
				      int *start_row, int *end_row);

Ecore_List       *ewl_table_find(Ewl_Table *table,
			       int start_col,
			       int end_col, int start_row, int emd_row);

char           *ewl_table_selected_get(Ewl_Table *table);
void            ewl_table_homogeneous_set(Ewl_Table *table, unsigned int h);
void            ewl_table_hhomogeneous_set(Ewl_Table *table, unsigned int h);
void            ewl_table_vhomogeneous_set(Ewl_Table *table, unsigned int h);
unsigned int    ewl_table_hhomogeneous_get(Ewl_Table *table);
unsigned int    ewl_table_vhomogeneous_get(Ewl_Table *table);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_table_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_table_show_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_table_child_configure_cb(Ewl_Widget * w, void *ev_data,
							void *user_data);
//void ewl_table_child_show_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_table_child_select_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_table_child_show_cb(Ewl_Container *p, Ewl_Widget *c); 

/**
 * @}
 */

#endif				/* __EWL_TABLE_H__ */
