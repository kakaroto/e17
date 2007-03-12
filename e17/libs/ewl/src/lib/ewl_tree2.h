/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_TREE2_H
#define EWL_TREE2_H

#include "ewl_mvc.h"

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
 * @remarks Inherits from Ewl_MVC.
 * @image html Ewl_Tree2_inheritance.png
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
 * @def EWL_TREE2_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree2, FALSE otherwise
 */
#define EWL_TREE2_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE2_TYPE))

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
 * @brief Inherits from Ewl_MVC and extends to provide a tree widget.
 */
struct Ewl_Tree2
{
	Ewl_MVC mvc; 		/**< Inherit from ewl_mvc. */

	Ewl_Widget *header; 	/**< The tree header */
	Ewl_Widget *rows; 	/**< The rows of the tree */

	Ecore_List *columns;	/**< The tree columns. */
	int *rowcache; 		/**< Cache of row sizes */

	Ewl_Tree_Selection_Type type;	 /**< The selection type of the tree */

	Ecore_Hash *expansions;	/**< Hash of expanded rows */

	unsigned char fixed:1;    /**< Rows are fixed height */
	unsigned char headers_visible:1; /**< Are the headers visible? */
};

/*
 * Tree view/controller manipulation
 */
Ewl_Widget 	*ewl_tree2_new(void);
int 		 ewl_tree2_init(Ewl_Tree2 *tree);

void		 ewl_tree2_column_append(Ewl_Tree2 *t, Ewl_View *v, 
						unsigned int sortable);
void		 ewl_tree2_column_prepend(Ewl_Tree2 *t, Ewl_View *v,
						unsigned int sortable);
void		 ewl_tree2_column_insert(Ewl_Tree2 *t, Ewl_View *v, 
							unsigned int idx,
							unsigned int sortable);
void		 ewl_tree2_column_remove(Ewl_Tree2 *t, unsigned int idx);

void		 ewl_tree2_headers_visible_set(Ewl_Tree2 *tree,
					       unsigned char visible);
unsigned int	 ewl_tree2_headers_visible_get(Ewl_Tree2 *tree);

Ewl_Tree_Selection_Type ewl_tree2_selection_type_get(Ewl_Tree2 *tree);
void 		 ewl_tree2_selection_type_set(Ewl_Tree2 *tree, 
						Ewl_Tree_Selection_Type type);

void		 ewl_tree2_fixed_rows_set(Ewl_Tree2 *tree, unsigned int fixed);
unsigned int	 ewl_tree2_fixed_rows_get(Ewl_Tree2 *tree);

Ewl_Widget	*ewl_tree2_view_widget_get(Ewl_Tree2 *tree);

void		 ewl_tree2_row_expand(Ewl_Tree2 *tree, void *data, 
						unsigned int row);
void		 ewl_tree2_row_collapse(Ewl_Tree2 *tree, void *data, 
						unsigned int row);

unsigned int	 ewl_tree2_row_expanded_is(Ewl_Tree2 *tree, void *data,
						unsigned int row);

/*
 * Internal stuff.
 */
void ewl_tree2_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_tree2_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_tree2_cb_column_sort(Ewl_Widget *w, void *ev, void *data);

/*
 * Ewl_Tree2_Column stuff
 */

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
 * @brief Holds the model and view to use for this column in the tree
 */
struct Ewl_Tree2_Column
{
	Ewl_View *view;		 /**< The view for the column */
	
	Ewl_MVC *parent;	 /**< The mvc this column is for */
	Ewl_Sort_Direction sort; /**< direction the column is sorted in */

	unsigned char sortable:1; /**< Is this column sortable */
};

Ewl_Tree2_Column	*ewl_tree2_column_new(void);
void			 ewl_tree2_column_destroy(Ewl_Tree2_Column *c);

void			 ewl_tree2_column_view_set(Ewl_Tree2_Column *c, Ewl_View *v);
Ewl_View		*ewl_tree2_column_view_get(Ewl_Tree2_Column *c);

void			 ewl_tree2_column_mvc_set(Ewl_Tree2_Column *c, Ewl_MVC *mvc);
Ewl_MVC 		*ewl_tree2_column_mvc_get(Ewl_Tree2_Column *c);

void			 ewl_tree2_column_sortable_set(Ewl_Tree2_Column *c, 
							unsigned int sortable);
unsigned int 		 ewl_tree2_column_sortable_get(Ewl_Tree2_Column *c);

void			 ewl_tree2_column_sort_direction_set(Ewl_Tree2_Column *c, 
								Ewl_Sort_Direction sort);
Ewl_Sort_Direction	 ewl_tree2_column_sort_direction_get(Ewl_Tree2_Column *c);

/*
 * Ewl_Tree2_Node stuff
 */
#define EWL_TREE2_NODE_TYPE "node"

/**
 * @def EWL_TREE2_NODE_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree2_Node, FALSE otherwise
 */
#define EWL_TREE2_NODE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE2_NODE_TYPE))

#define EWL_TREE2_NODE(n) ((Ewl_Tree2_Node *)n)

/**
 * Ewl_Tree2_Node 
 */
typedef struct Ewl_Tree2_Node Ewl_Tree2_Node;

/** 
 * @brief Inherits from Ewl_Container and extends to hold information on a
 * row of the tree
 */
struct Ewl_Tree2_Node
{
	Ewl_MVC mvc;

	Ewl_Widget *tree;		/**< The parent tree */
	Ewl_Widget *row;		/**< The row this node is for */
	Ewl_Widget *handle;		/**< the expansion handle */

	unsigned int row_num;		/**< The row number of this row */
	Ewl_Tree_Node_Flags expanded;
};

Ewl_Widget	*ewl_tree2_node_new(void);
int		 ewl_tree2_node_init(Ewl_Tree2_Node *node);

void		 ewl_tree2_node_expandable_set(Ewl_Tree2_Node *node, void *data);
unsigned int	 ewl_tree2_node_expandable_get(Ewl_Tree2_Node *node);

void		 ewl_tree2_node_expand(Ewl_Tree2_Node *node);
void		 ewl_tree2_node_collapse(Ewl_Tree2_Node *node);

unsigned int	 ewl_tree2_node_expanded_is(Ewl_Tree2_Node *node);

void ewl_tree2_cb_node_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree2_cb_node_realize(Ewl_Widget *w, void *ev, void *data);
void ewl_tree2_cb_node_toggle(Ewl_Widget *w, void *ev_data, void *user_data);

void ewl_tree2_cb_node_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree2_cb_node_child_hide(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree2_cb_node_resize(Ewl_Container *c, Ewl_Widget *w, int size,
                                                     Ewl_Orientation o);
void ewl_tree2_cb_node_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree2_cb_node_child_del(Ewl_Container *c, Ewl_Widget *w, int idx);

/**
 * @}
 */

#endif

