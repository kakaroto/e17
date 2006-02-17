#ifndef EWL_TREE2_H
#define EWL_TREE2_H

/**
 * @defgroup Ewl_Tree2 Ewl_Tree2: A Widget for List or Tree Layout
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

#define EWL_TREE2_TYPE "tree2"

/**
 * @def EWL_TREE2_COLUMN
 * Typecasts a pointer to an Ewl_Tree2_Column pointer.
 */
#define EWL_TREE2_COLUMN(c) ((Ewl_Tree2_Column *)c)

typedef struct Ewl_Tree2_Column Ewl_Tree2_Column;

/**
 * Holdes the model and view to use for this column in the tree
 */
struct Ewl_Tree2_Column
{
	Ewl_Model *model;	/**< The model for the column */
	Ewl_View *view;		/**< The view for the column */
};

/**
 * @def EWL_TREE2(t)
 * Typecasts a pointer to an Ewl_Tree pointer.
 */
#define EWL_TREE2(t) ((Ewl_Tree2 *)t)

typedef struct Ewl_Tree2 Ewl_Tree2;

/**
 * The tree is a columnar listing, where items in the list may be nested
 * below other items.
 */
struct Ewl_Tree2
{
	Ewl_Container container; /**< Inherit from container. */

	Ewl_Widget *header;	 /**< The tree header */
	Ewl_Widget *rows;	 /**< Holds the rows of the tree */

	Ecore_List *columns;     /**< The tree columns. */
	void *data;              /**< Data provided to the tree */

	int *rowcache;           /**< Cache of row sizes */
	Ecore_List *selected; 	 /**< The list of selected cells */

	Ewl_Tree_Mode mode;	 /**< The mode of the tree */
	unsigned int fixed:1;   /**< Rows are fixed height */
	unsigned int headers_visible:1; /**< Are the headers visible? */
};

/*
 * Tree view/controller manipulation
 */
Ewl_Widget 	*ewl_tree2_new(void);
int 		 ewl_tree2_init(Ewl_Tree2 *tree);

void		 ewl_tree2_data_set(Ewl_Tree2 *m, void *data);
void		*ewl_tree2_data_get(Ewl_Tree2 *m);

void		 ewl_tree2_column_append(Ewl_Tree2 *t, Ewl_Model *m, 
							Ewl_View *v);
void		 ewl_tree2_column_prepend(Ewl_Tree2 *t, Ewl_Model *m, 
							Ewl_View *v);
void		 ewl_tree2_column_insert(Ewl_Tree2 *t, Ewl_Model *m, 
							Ewl_View *v, 
							unsigned int idx);
void		 ewl_tree2_column_remove(Ewl_Tree2 *t, unsigned int idx);

void		 ewl_tree2_headers_visible_set(Ewl_Tree2 *tree,
					       unsigned char visible);
unsigned int	 ewl_tree2_headers_visible_get(Ewl_Tree2 *tree);

Ecore_List 	*ewl_tree2_selected_cells_get(Ewl_Tree2 *tree);
void 		 ewl_tree2_selected_cells_clear(Ewl_Tree2 *tree);

Ewl_Tree_Mode 	 ewl_tree2_mode_get(Ewl_Tree2 *tree);
void 		 ewl_tree2_mode_set(Ewl_Tree2 *tree, Ewl_Tree_Mode mode);

void		 ewl_tree2_fixed_rows_set(Ewl_Tree2 *tree, unsigned int fixed);
unsigned int	 ewl_tree2_fixed_rows_get(Ewl_Tree2 *tree);

/*
 * Internal stuff.
 */
void ewl_tree2_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_tree2_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_tree2_cb_child_resize(Ewl_Container *c, Ewl_Widget *w,
				     int size, Ewl_Orientation o);

/*
 * Ewl_Tree2_Column stuff
 */
Ewl_Tree2_Column	*ewl_tree2_column_new(void);
void			 ewl_tree2_column_destroy(Ewl_Tree2_Column *c);

void			 ewl_tree2_column_model_set(Ewl_Tree2_Column *c, Ewl_Model *m);
Ewl_Model 		*ewl_tree2_column_model_get(Ewl_Tree2_Column *c);

void			 ewl_tree2_column_view_set(Ewl_Tree2_Column *c, Ewl_View *v);
Ewl_View		*ewl_tree2_column_view_get(Ewl_Tree2_Column *c);

/**
 * @}
 */

#endif

