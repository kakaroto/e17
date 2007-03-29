/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_tree2_view.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @param v: The view to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Tree2_View to default values
 */
int
ewl_tree2_view_init(Ewl_Tree2_View *v)
{
	if (!ewl_box_init(EWL_BOX(v)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(v), EWL_TREE2_VIEW_TYPE);
	ewl_box_orientation_set(EWL_BOX(v), EWL_ORIENTATION_VERTICAL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param v: The view to work with
 * @param t: The tree to set
 * @return Returns no value
 * @brief Sets the @a t tree into the @a v view
 */
void
ewl_tree2_view_tree2_set(Ewl_Tree2_View *v, Ewl_Tree2 *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("v", v);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("v", v, EWL_TREE2_VIEW_TYPE);
	DCHECK_TYPE("t", t, EWL_TREE2_TYPE);

	v->parent = t;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param v: The view to work with
 * @return Returns the Ewl_Tree2 parent
 * @brief Retrieves the tree parent set into the view
 */
Ewl_Tree2 *
ewl_tree2_view_tree2_get(Ewl_Tree2_View *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("v", v, NULL);
	DCHECK_TYPE_RET("v", v, EWL_TREE2_VIEW_TYPE, NULL);

	DRETURN_PTR(v->parent, DLEVEL_STABLE);
}

