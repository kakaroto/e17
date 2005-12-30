#ifndef _EWL_TREE2_H
#define _EWL_TREE2_H

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
 * Data:
 * Provides a wrapper to the data which allows for observer registration and a
 * reference to a model for data access.
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

typedef void *(*Ewl_Model_Fetch)(void *data, int row, int column);

#define EWL_MODEL_DATA_GET(f) ((Ewl_Model_Fetch *)f)

typedef int (*Ewl_Model_Sort)(void *data, int column);

#define EWL_MODEL_DATA_SORT(f) ((Ewl_Model_Sort *)f)

typedef int (*Ewl_Model_Count)(void *data);

#define EWL_MODEL_DATA_COUNT(f) ((Ewl_Model_Count *)f)

/**
 * @def EWL_MODEL(model)
 * Typecasts a pointer to an Ewl_Model pointer.
 */
#define EWL_MODEL(model) ((Ewl_Model *)model)

typedef struct Ewl_Model Ewl_Model;

struct Ewl_Model
{
	Ewl_Model_Fetch  fetch;    /**< Retrieve data for a cell */
	Ewl_Model_Fetch  subfetch; /**< Check for subdata */
	Ewl_Model_Sort   sort;     /**< Trigger sort on column */
	Ewl_Model_Count  count;    /**< Count of data items */
};

typedef int (*Ewl_View_Constructor)(void *data);

#define EWL_VIEW_CONSTRUCTOR(f) ((Ewl_View_Constructor *)f)

typedef int (*Ewl_View_Assign)(Ewl_Widget *w, void *data);

#define EWL_VIEW_ASSIGN(f) ((Ewl_View_Assign *)f)

/**
 * @def EWL_VIEW(view)
 * Typecasts a pointer to an Ewl_View pointer.
 */
#define EWL_VIEW(view) ((Ewl_View *)view)

typedef struct Ewl_View Ewl_View;

struct Ewl_View
{
	Ewl_View_Constructor construct;     /**< Create a widget for display */
	Ewl_View_Assign assign;             /**< Assign data to a widget */
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
	Ewl_Container container; /**< Inherit from container. */

	Ewl_Model *models;       /**< Data models for the tree columns. */

	void *data;              /**< Data provided to the tree */
	int *rowcache;           /**< Cache of row sizes */
	int fixed;               /**< Rows are fixed height */
};

/*
 * Tree view/controller manipulation
 */
Ewl_Widget 	*ewl_tree2_new(void);
int 		 ewl_tree2_init(Ewl_Tree2 *tree);

void             ewl_tree2_data_set(Ewl_Tree2 *m, void *data);
void            *ewl_tree2_data_get(Ewl_Tree2 *m);

void             ewl_tree2_column_append(Ewl_Tree2 *t, Ewl_Model *m, Ewl_View *v);
void             ewl_tree2_column_prepend(Ewl_Tree2 *t, Ewl_Model *m, Ewl_View *v);
void             ewl_tree2_column_remove(Ewl_Tree2 *t, Ewl_Model *m, Ewl_View *v);

void		 ewl_tree2_headers_visible_set(Ewl_Tree2 *tree,
					       unsigned char visible);
unsigned int	 ewl_tree2_headers_visible_get(Ewl_Tree2 *tree);

Ecore_List 	*ewl_tree2_selected_get(Ewl_Tree2 *tree);
void 		 ewl_tree2_selected_clear(Ewl_Tree2 *tree);

Ewl_Tree_Mode 	 ewl_tree2_mode_get(Ewl_Tree2 *tree);
void 		 ewl_tree2_mode_set(Ewl_Tree2 *tree, Ewl_Tree_Mode mode);

void             ewl_tree2_fixed_rows_set(Ewl_Tree2 *tree, int fixed);
int              ewl_tree2_fixed_rows_get(Ewl_Tree2 *tree);

/*
 * View manipulation
 */
Ewl_View            *ewl_view_new(void);
int                  ewl_view_init(Ewl_View *view);

void                 ewl_view_constructor_set(Ewl_View *view, Ewl_View_Constructor construct);
Ewl_View_Constructor ewl_view_constructor_get(Ewl_View *view);

void                 ewl_view_assign_set(Ewl_View *view, Ewl_View_Assign assign);
Ewl_View_Assign      ewl_view_assign_get(Ewl_View *view);

/*
 * Model manipulation.
 */
Ewl_Model 	*ewl_model_new(void);
int       	 ewl_model_init(Ewl_Model *model);

void             ewl_model_fetch_set(Ewl_Model *m, Ewl_Model_Fetch get);
Ewl_Model_Fetch  ewl_model_fetch_get(Ewl_Model *m);

void             ewl_model_subfetch_set(Ewl_Model *m, Ewl_Model_Fetch get);
Ewl_Model_Fetch  ewl_model_subfetch_get(Ewl_Model *m);

void             ewl_model_sort_set(Ewl_Model *m, Ewl_Model_Sort sort);
Ewl_Model_Sort   ewl_model_sort_get(Ewl_Model *m);

void             ewl_model_count_set(Ewl_Model *m, Ewl_Model_Count count);
Ewl_Model_Count  ewl_model_count_get(Ewl_Model *m);

/**
 * @}
 */

#endif
