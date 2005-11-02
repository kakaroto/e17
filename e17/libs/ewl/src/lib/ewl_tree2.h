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
 * data, indicate expansion points, notify views and controllers of changes.
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

typedef struct Ewl_Tree_Column_Provider Ewl_Tree_Column_Provider;

/**
 * @def EWL_TREE_COLUMN_PROVIDER(t)
 * Typecasts a pointer to an Ewl_Tree_Column_Provider pointer.
 */
#define EWL_TREE_COLUMN_PROVIDER(p) ((Ewl_Tree_Column_Provider *)p)

/**
 * @struct Ewl_Tree_Column_Provider
 * The tree column provider is used to map data from an arbitrary data structure
 * into a widget and corresponding information for display.
 */
struct Ewl_Tree_Column_Provider
{
	Ewl_Widget *(*display_get)(void);                    /**< Get widget for data display */
	void (*display_set)(Ewl_Widget *w, void *data);      /**< Set widget data */

	int (*row_count_get)(void *data);                    /**< Total rows in column */
	void *(*data_get)(void *data, int row, int col);     /**< Get cell data */
	int (*expandable_get)(void *data, int row, int col); /**< Mark column expandable */
	int (*persistent_get)(void *data, int row, int col); /**< Mark column display data persistent */
};

typedef struct Ewl_Tree Ewl_Tree;

/**
 * @def EWL_TREE(t)
 * Typecasts a pointer to an Ewl_Tree pointer.
 */
#define EWL_TREE(t) ((Ewl_Tree *)t)

/**
 * @struct Ewl_Tree
 * The tree is a columnar listing, where items in the list may be nested
 * below other items.
 */
struct Ewl_Tree
{
	Ewl_Container  container;               /**< Inherit from container */
	Ewl_Tree_Column_Provider *providers; /**< Array of providers */
};

typedef struct Ewl_Tree_Node Ewl_Tree_Node;

/**
 * @def EWL_TREE_NODE(t)
 * Typecasts a pointer to an Ewl_Tree_Node pointer.
 */
#define EWL_TREE_NODE(t) ((Ewl_Tree_Node *)t)

/*
 * The tree_node exists for each row, at this level the tree_node contains rows
 * that contain cells which display the data.
 */
struct Ewl_Tree_Node
{
	Ewl_Container container;      /**< Inherit from Ewl_Container */
	Ewl_Tree *tree;               /**< Pointer to the tree this is inside */
	Ewl_Widget *row;              /**< The child row */
	Ewl_Widget *handle;           /**< The child row */
	Ewl_Tree_Node_Flags expanded; /**< Indicator of expansion state */
};

/*
 * Node creation/initialization functions.
 */
Ewl_Widget 	*ewl_tree_node_new(void);
int 		 ewl_tree_node_init(Ewl_Tree_Node *tree_node);

/*
 * Node state modifying functions
 */
void 		 ewl_tree_node_collapse(Ewl_Tree_Node *tree);
void 		 ewl_tree_node_expand(Ewl_Tree_Node *tree);

/*
 * Tree creation/initialization functions.
 */
Ewl_Widget 	*ewl_tree_new(unsigned short columns);
int 		 ewl_tree_init(Ewl_Tree *tree, unsigned short columns);

void 		 ewl_tree_headers_set(Ewl_Tree *tree, char **headers);
void  		 ewl_tree_columns_set(Ewl_Tree *tree, unsigned short columns);

void		 ewl_tree_headers_visible_set(Ewl_Tree *tree, unsigned int visible);
unsigned int	 ewl_tree_headers_visible_get(Ewl_Tree *tree);


Ecore_List 	*ewl_tree_selected_get(Ewl_Tree *tree);
void 		 ewl_tree_selected_clear(Ewl_Tree *tree);
Ewl_Widget	*ewl_tree_row_column_get(Ewl_Row *row, int i);

Ewl_Tree_Mode  	 ewl_tree_mode_get(Ewl_Tree *tree);
void 		 ewl_tree_mode_set(Ewl_Tree *tree, Ewl_Tree_Mode mode);

Ewl_Widget 	*ewl_tree_row_add(Ewl_Tree *tree, Ewl_Row *prow,
					     Ewl_Widget **children);
Ewl_Widget 	*ewl_tree_text_row_add(Ewl_Tree *tree, Ewl_Row *prow, char **text);
Ewl_Widget 	*ewl_tree_entry_row_add(Ewl_Tree *tree, Ewl_Row *prow, char **text);

void 		 ewl_tree_row_destroy(Ewl_Tree *tree, Ewl_Row *row);
void 		 ewl_tree_row_expand_set(Ewl_Row *row, Ewl_Tree_Node_Flags expanded);
Ewl_Widget 	*ewl_tree_row_find(Ewl_Tree *tree, int row);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_tree_node_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_node_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_node_toggle_cb(Ewl_Widget *w, void *ev_data, void *user_data);

void ewl_tree_node_child_show_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree_node_child_hide_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree_node_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
						     Ewl_Orientation o);

void ewl_tree_row_select_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_row_hide_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_hscroll_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_child_resize_cb(Ewl_Container *c);
void ewl_tree_node_child_add_cb(Ewl_Container *c, Ewl_Widget *w);

/**
 * @}
 */

#endif
