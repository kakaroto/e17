#ifndef _EWL_TREE_H
#define _EWL_TREE_H

/**
 * @file ewl_tree.h
 * @defgroup Ewl_Tree Tree: A Widget for List or Tree Layout
 * @brief Defines a widget for laying out other widgets in a tree or list
 * like manner. This also fulfills the functionality often seen in a table
 * widget.
 *
 * Model:
 * Defines communication callbacks for views and controllers. Query row/column
 * data, indicate expansion points, notify views and controllers of changes,
 * trigger sorting on a row/column combination.
 *
 * View:
 * Defines the callbacks for setting up the widgets based on the data returned
 * from the Model. Create widgets, set data on widgets, calculate sizing,
 * minimize number of widgets.
 *
 * Controller:
 * The tree defines the entire controller and registers with the model.
 * Provides controls to sort columns, expand/collapse branching points, reacts
 * to changes from data model.
 *
 * Simple case:
 * Widgets packed using standard container functions. This should result in a
 * simple list of packed widgets.
 *
 * Features:
 * * Default data provider, so standard container functions are supported.
 * * MVC pattern of data access for more powerful layouts.
 * * Packing of arbitrary widget types in cells.
 * * Theming based on repeating row counts.
 * * Number of columns dependant on number of providers, not a fixed number.
 * * When using data providers, can reap widgets to save memory and decrease
 *   layout time.
 *
 * Issues:
 * * How do we support click callbacks on full rows? Special notifier
 *   necessary? Proposal: Tree value change callback on row selection. Passes
 *   event structure with array of selected row numbers. Simple case handled
 *   by callback on packed widgets.
 * * Column or row layout? Column allows for redirecting to a container to
 *   handle the case of container functions used to add widgets and to use a
 *   fixed number of rows in display region. It would also allow for skipping
 *   configure of an entire off-screen column, rows would lay out the cells
 *   off-screen. How do we implement column layout? Proposal: Row height
 *   cache, easy for fixed height rows, more complex for variable height.
 *
 * @{
 */

/**
 * @themekey /tree/file
 * @themekey /tree/group
 */

typedef void *(*data_get)(void *data, int row, int column) Ewl_Tree2_Cell_Data;
typedef void *(*child_data_get)(void *data, int row) Ewl_Tree2_Child_Data;
typedef int (*column_sort)(void *data, int column) Ewl_Tree2_Column_Sort;
typedef int (*row_count)(void *data) Ewl_Tree2_Row_Count;

typedef struct Ewl_Tree2_Model Ewl_Tree2_Model;

struct Ewl_Tree2_Model
{
	void *data;                          /**< Data provided to the tree */

	Ewl_Tree2_Cell_Data cell_data_get;   /**< Retrieve data for a cell */
	Ewl_Tree2_Child_Data child_data_get; /**< Check if a row expands */
	Ewl_Tree2_Column_Sort column_sort;   /**< Trigger sort on column */
	Ewl_Tree2_Row_Count row_count;       /**< Count of rows in data */
};

typedef struct Ewl_Tree2 Ewl_Tree2;

/**
 * @def EWL_TREE2(t)
 * Typecasts a pointer to an Ewl_Tree pointer.
 */
#define EWL_TREE2(t) ((Ewl_Tree2 *)t)

/**
 * @struct Ewl_Tree
 * The tree is a columnar listing, where items in the list may be nested
 * below other items.
 */
struct Ewl_Tree2
{
	Ewl_Container    container; /**< Inherit from container. */

	Ewl_Tree2_Model *model;     /**< Current data model for the tree. */

	int *rowcache;              /**< Cache of row sizes */
	int fixed;                  /**< Rows are fixed height */
};

/*
 * Tree view/controller manipulation
 */
Ewl_Widget 	*ewl_tree2_new();
int 		 ewl_tree2_init(Ewl_Tree *tree, unsigned short columns);

void		 ewl_tree2_headers_visible_set(Ewl_Tree *tree,
					       unsigned char visible);
unsigned int	 ewl_tree2_headers_visible_get(Ewl_Tree *tree);

Ecore_List 	*ewl_tree2_selected_get(Ewl_Tree *tree);
void 		 ewl_tree2_selected_clear(Ewl_Tree *tree);

Ewl_Tree2_Mode 	 ewl_tree2_mode_get(Ewl_Tree *tree);
void 		 ewl_tree2_mode_set(Ewl_Tree *tree, Ewl_Tree2_Mode mode);

void             ewl_tree2_fixed_rows_set(Ewl_Tree2 *tree, int fixed);
int              ewl_tree2_fixed_rows_get(Ewl_Tree2 *tree);

/*
 * Tree model manipulation.
 */
Ewl_Tree2_Model *ewl_tree2_model_new();

void             ewl_tree2_model_data_set(Ewl_Tree2_Model *m, void *data);
void            *ewl_tree2_model_data_get(Ewl_Tree2_Model *m);

void             ewl_tree2_model_cell_data_get_set(Ewl_Tree2_Model *m, Ewl_Tree2_Cell_Data get);
Ewl_Tree2_Cell_Data ewl_tree2_model_cell_data_get_get(Ewl_Tree2_Model *m);

void             ewl_tree2_model_column_sort_set(Ewl_Tree2_Model *m, Ewl_Tree2_Column_Sort sort);
Ewl_Tree2_Column_Sort ewl_tree2_model_column_sort_get(Ewl_Tree2_Model *m);

/**
 * @}
 */

#endif
