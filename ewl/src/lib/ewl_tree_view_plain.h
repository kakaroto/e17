/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TREE_VIEW_PLAIN_H
#define EWL_TREE_VIEW_PLAIN_H

#include "ewl_tree_view.h"

/**
 * @addtogroup Ewl_Tree_View_Plain Ewl_Tree_View_Plain: The plain tree view
 * @brief Defines a plain view into the tree just listing rows
 *
 * @remarks Inherits from Ewl_Tree_View.
 * @if HAVE_IMAGES
 * @image html Ewl_Tree_View_Plain_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_TREE_VIEW_PLAIN_TYPE
 * The Ewl_Tree_View_Plain type
 */
#define EWL_TREE_VIEW_PLAIN_TYPE "tree_view_plain"

/**
 * @def EWL_TREE_VIEW_PLAIN_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree_View_Plain, FALSE otherwise
 */
#define EWL_TREE_VIEW_PLAIN_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE_VIEW_PLAIN_TYPE))

/**
 * Ewl_Tree_View_Plain
 */
typedef struct Ewl_Tree_View_Plain Ewl_Tree_View_Plain;

/**
 * @def EWL_TREE_VIEW_PLAIN(v)
 * Typecasts a pointer to an Ewl_Tree_View_Plain pointer
 */
#define EWL_TREE_VIEW_PLAIN(tv) ((Ewl_Tree_View_Plain*)tv)

/**
 * @brief Inherits from Ewl_Tree_View and provides a plain tree layout
 */
struct Ewl_Tree_View_Plain
{
        Ewl_Tree_View view;        /**< Inherit from Ewl_Tree_View */
};

const Ewl_View          *ewl_tree_view_plain_get(void);

Ewl_Widget              *ewl_tree_view_plain_new(void);
int                      ewl_tree_view_plain_init(Ewl_Tree_View_Plain *tv);

/**
 * @}
 */

#endif

