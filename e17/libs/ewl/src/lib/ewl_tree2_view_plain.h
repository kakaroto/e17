/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_TREE2_VIEW_PLAIN_H
#define EWL_TREE2_VIEW_PLAIN_H

#include "ewl_tree2_view.h"

/**
 * @addtogroup Ewl_Tree2_View_Plain Ewl_Tree2_View_Plain: The plain tree view
 * @brief Defines a plain view into the tree just listing rows
 * 
 * @remarks Inherits from Ewl_Tree2_View.
 * @image html Ewl_Tree2_View_Plain_inheritance.png
 *
 * @{
 */

/**
 * @def EWL_TREE2_VIEW_PLAIN_TYPE
 * The Ewl_Tree2_View_Plain type
 */
#define EWL_TREE2_VIEW_PLAIN_TYPE "tree2_view_plain"

/**
 * @def EWL_TREE2_VIEW_PLAIN_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree2_View_Plain, FALSE otherwise
 */
#define EWL_TREE2_VIEW_PLAIN_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE2_VIEW_PLAIN_TYPE))

/**
 * Ewl_Tree2_View_Plain
 */
typedef struct Ewl_Tree2_View_Plain Ewl_Tree2_View_Plain;

/**
 * @def EWL_TREE2_VIEW_PLAIN(v)
 * Typecasts a pointer to an Ewl_Tree2_View_Plain pointer
 */
#define EWL_TREE2_VIEW_PLAIN(tv) ((Ewl_Tree2_View_Plain*)tv)

/**
 * Provides a plain view for the tree
 */
struct Ewl_Tree2_View_Plain
{
	Ewl_Tree2_View view;	/**< Inherit from Ewl_Tree2_View */
};

Ewl_View	*ewl_tree2_view_plain_get(void);

Ewl_Widget	*ewl_tree2_view_plain_new(void);
int		 ewl_tree2_view_plain_init(Ewl_Tree2_View_Plain *tv);

/**
 * @}
 */

#endif

