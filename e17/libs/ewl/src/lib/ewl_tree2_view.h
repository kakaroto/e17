/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_TREE2_VIEW_H
#define EWL_TREE2_VIEW_H

#include "ewl_box.h"
#include "ewl_tree2.h"

/**
 * @addtogroup Ewl_Tree2_View Ewl_Tree2_View: A tree view
 * @brief The tree view class
 *
 * @remarks Inherits from Ewl_Box.
 * @image html Ewl_Tree2_View_inheritance.png
 *
 * @{
 */

/**
 * @def EWL_TREE2_VIEW_TYPE
 * The type name for Ewl_Tree2_View
 */
#define EWL_TREE2_VIEW_TYPE "tree2_view"

/**
 * @def EWL_TREE2_VIEW_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree2_View, FALSE otherwise
 */
#define EWL_TREE2_VIEW_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE2_VIEW_TYPE))

/**
 * @def EWL_TREE2_VIEW(v)
 * Typecasts a pointer to an Ewl_Tree2_View pointer 
 */
#define EWL_TREE2_VIEW(v) ((Ewl_Tree2_View *)v)

/**
 * A View on the tree
 */
typedef struct Ewl_Tree2_View Ewl_Tree2_View;

/**
 * @brief Inherits from Ewl_Box and provides the base information for a tree
 * view
 */
struct Ewl_Tree2_View
{
	Ewl_Box box;		/**< Inherit from Ewl_Box */
	Ewl_Tree2 *parent;	/**< The tree parent */
};

int ewl_tree2_view_init(Ewl_Tree2_View *v);

void ewl_tree2_view_tree2_set(Ewl_Tree2_View *v, Ewl_Tree2 *t);
Ewl_Tree2 *ewl_tree2_view_tree2_get(Ewl_Tree2_View *v);

/**
 * @}
 */

#endif

