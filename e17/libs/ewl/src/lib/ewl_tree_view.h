/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TREE_VIEW_H
#define EWL_TREE_VIEW_H

#include "ewl_box.h"
#include "ewl_tree.h"

/**
 * @addtogroup Ewl_Tree_View Ewl_Tree_View: A tree view
 * @brief The tree view class
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Tree_View_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_TREE_VIEW_TYPE
 * The type name for Ewl_Tree_View
 */
#define EWL_TREE_VIEW_TYPE "tree_view"

/**
 * @def EWL_TREE_VIEW_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree_View, FALSE otherwise
 */
#define EWL_TREE_VIEW_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE_VIEW_TYPE))

/**
 * @def EWL_TREE_VIEW(v)
 * Typecasts a pointer to an Ewl_Tree_View pointer
 */
#define EWL_TREE_VIEW(v) ((Ewl_Tree_View *)v)

/**
 * A View on the tree
 */
typedef struct Ewl_Tree_View Ewl_Tree_View;

/**
 * @brief Inherits from Ewl_Box and provides the base information for a tree
 * view
 */
struct Ewl_Tree_View
{
        Ewl_Box box;                /**< Inherit from Ewl_Box */
        Ewl_Tree *parent;        /**< The tree parent */
};

int              ewl_tree_view_init(Ewl_Tree_View *v);

void             ewl_tree_view_tree_set(Ewl_Tree_View *v, Ewl_Tree *t);
Ewl_Tree        *ewl_tree_view_tree_get(Ewl_Tree_View *v);

/**
 * @}
 */

#endif

