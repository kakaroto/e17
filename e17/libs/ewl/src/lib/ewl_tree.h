/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_TREE_H
#define EWL_TREE_H

#include "ewl_row.h"

/**
 * @addtogroup Ewl_Tree Ewl_Tree: A Widget for List or Tree Layout
 * @brief Defines a widget for laying out other widgets in a tree or list like
 * manner.
 *
 * @remarks Inherits from Ewl_Container.
 * @image html Ewl_Tree_inheritance.png
 *
 * @{
 */

/**
 * @themekey /tree/file
 * @themekey /tree/group
 */

/**
 * @def EWL_TREE_TYPE
 * The type name for the Ewl_Tree widget
 */
#define EWL_TREE_TYPE "tree"

/**
 * @def EWL_TREE_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree, FALSE otherwise
 */
#define EWL_TREE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE_TYPE))

/**
 * @def EWL_TREE_NODE_TYPE
 * The type name for the Ewl_Tree_Node widget
 */
#define EWL_TREE_NODE_TYPE "node"

/**
 * @def EWL_TREE_NODE_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree_Node, FALSE otherwise
 */
#define EWL_TREE_NODE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE_NODE_TYPE))

/**
 * The Ewl_Tree widget
 */
typedef struct Ewl_Tree Ewl_Tree;

/**
 * @def EWL_TREE(t)
 * Typecasts a pointer to an Ewl_Tree pointer.
 */
#define EWL_TREE(t) ((Ewl_Tree *)t)

/**
 * The tree is a columnar listing, where items in the list may be nested below
 * other items.
 */
struct Ewl_Tree
{
	Ewl_Container container;  /**< Inherit from Ewl_Container */

	Ewl_Widget *header;     /**< Array of widgets in the header */
	Ewl_Widget *scrollarea; /**< Scrollable area of rows */
	Ecore_List *selected;   /**< The currently selected rows */
	Ewl_Selection_Mode mode;       /**< Mode for selecting rows */
	unsigned short ncols;      /**< Number of columns in tree */
	unsigned short headers_visible; /**< Are the headers visible */
	unsigned short expands_visible; /**< Are the rows expandable */
};

/**
 * A node in the tree
 */
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
	Ewl_Container container; /**< Inherit from Ewl_Container */
	Ewl_Tree *tree; /**< Pointer to the tree this is inside */
	Ewl_Widget *row; /**< The child row */
	Ewl_Widget *handle; /**< The child row */
	Ewl_Tree_Node_Flags expanded; /**< Indicator of expansion state */
};

Ewl_Widget 	*ewl_tree_node_new(void);
int 		 ewl_tree_node_init(Ewl_Tree_Node *tree_node);

void 		 ewl_tree_node_expandable_set(Ewl_Tree_Node *node, int expand);
int 		 ewl_tree_node_expandable_get(Ewl_Tree_Node *node);

void 		 ewl_tree_node_collapse(Ewl_Tree_Node *tree);
void 		 ewl_tree_node_expand(Ewl_Tree_Node *tree);

Ewl_Widget 	*ewl_tree_new(unsigned short columns);
int 		 ewl_tree_init(Ewl_Tree *tree, unsigned short columns);

void 		 ewl_tree_headers_set(Ewl_Tree *tree, char **headers);
void  		 ewl_tree_columns_set(Ewl_Tree *tree, unsigned short columns);

void		ewl_tree_headers_visible_set(Ewl_Tree *tree, unsigned int visible);
unsigned int	ewl_tree_headers_visible_get(Ewl_Tree *tree);

void		ewl_tree_expandable_rows_set(Ewl_Tree *tree, unsigned int visible);
unsigned int	ewl_tree_expandable_rows_get(Ewl_Tree *tree);


Ecore_List 	*ewl_tree_selected_get(Ewl_Tree *tree);
void 		 ewl_tree_selected_clear(Ewl_Tree *tree);
Ewl_Widget	*ewl_tree_row_column_get(Ewl_Row *row, int i);

Ewl_Selection_Mode  	 ewl_tree_mode_get(Ewl_Tree *tree);
void 		 ewl_tree_mode_set(Ewl_Tree *tree, Ewl_Selection_Mode mode);

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
void ewl_tree_cb_node_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_node_destroy(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_node_toggle(Ewl_Widget *w, void *ev_data, void *user_data);

void ewl_tree_cb_node_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree_cb_node_child_hide(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree_cb_node_resize(Ewl_Container *c, Ewl_Widget *w, int size,
						     Ewl_Orientation o);

void ewl_tree_cb_header_change(Ewl_Widget *w, void *ev, void *data);
void ewl_tree_cb_row_select(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_row_hide(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_header_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_destroy(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_hscroll(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_child_resize(Ewl_Container *c);
void ewl_tree_cb_node_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree_cb_node_child_del(Ewl_Container *c, Ewl_Widget *w, int idx);

/**
 * @}
 */

#endif
