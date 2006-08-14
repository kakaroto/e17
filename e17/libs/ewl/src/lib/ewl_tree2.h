#ifndef EWL_TREE2_H
#define EWL_TREE2_H

/* 
 * Make ewl_tree2 an ewl_box. it will pack in the paned header and a
 * ewl_tree_display widget for the body. This will be a standard vbox. 
 *
 * subclassing widgets (ewl_tree_scrolled) will inherit tree and can then
 * pull the _display widget out of the main box, pack in their scrollpane
 * and then pack the _display widget back into the box. 
 *
 * This means that all the standard tree calls will work on the subclassing
 * stuff, as the tree will do the work, and the subclassers will just move
 * the shit in the tree around as needed.
 *
 * Ewl_Widget *rows needs to be a custom widget that can lay the widgets out
 * in their tree view.
 *
 * Cons
 * ----
 * Needs an extra widget to hold the 'display' of the tree data
 *
 *
 * The _display will actually be called ewl_tree2_view. This will be
 * abstracted out like the filelist does for the filedialog. This way we can
 * have different types of 'views' on the tree columns. So, there would be
 * the default view which is just as now (ewl_tree2_view_tree) and if
 * someone wanted they could write ewl_tree2_view_spreadsheet to get a more
 * spreadsheet type view with numbers down the side and letters over the
 * top.
 *
 */


/**
 * @addtogroup Ewl_Tree2 Ewl_Tree2: A Widget for List or Tree Layout
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

/**
 * @def EWL_TREE2_TYPE
 * The type name for the Ewl_ widget
 */
#define EWL_TREE2_TYPE "tree2"

/**
 * @def EWL_TREE2_COLUMN
 * Typecasts a pointer to an Ewl_Tree2_Column pointer.
 */
#define EWL_TREE2_COLUMN(c) ((Ewl_Tree2_Column *)c)

/**
 * The Ewl_Tree2_Column type
 */
typedef struct Ewl_Tree2_Column Ewl_Tree2_Column;

/**
 * Holdes the model and view to use for this column in the tree
 */
struct Ewl_Tree2_Column
{
	Ewl_Model *model;	/**< The model for the column */
	Ewl_View  *view;	/**< The view for the column */
};

/**
 * @def EWL_TREE2(t)
 * Typecasts a pointer to an Ewl_Tree pointer.
 */
#define EWL_TREE2(t) ((Ewl_Tree2 *)t)

/**
 * The Ewl_Tree2 widget
 */
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
	unsigned char fixed:1;    /**< Rows are fixed height */
	unsigned char headers_visible:1; /**< Are the headers visible? */
	unsigned char dirty:1;    /**< Has the data changed? */
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

void		 ewl_tree2_dirty_set(Ewl_Tree2 *tree2, unsigned int dirty);
unsigned int	 ewl_tree2_dirty_get(Ewl_Tree2 *tree2);

/*
 * Internal stuff.
 */
void ewl_tree2_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_tree2_cb_configure(Ewl_Widget *w, void *ev, void *data);

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

