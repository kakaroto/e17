/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_tree_view.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @param v: The view to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Tree_View to default values
 */
int
ewl_tree_view_init(Ewl_Tree_View *v)
{
        if (!ewl_box_init(EWL_BOX(v)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(EWL_WIDGET(v), EWL_TREE_VIEW_TYPE);
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
ewl_tree_view_tree_set(Ewl_Tree_View *v, Ewl_Tree *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(v);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(v, EWL_TREE_VIEW_TYPE);
        DCHECK_TYPE(t, EWL_TREE_TYPE);

        v->parent = t;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param v: The view to work with
 * @return Returns the Ewl_Tree parent
 * @brief Retrieves the tree parent set into the view
 */
Ewl_Tree *
ewl_tree_view_tree_get(Ewl_Tree_View *v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(v, NULL);
        DCHECK_TYPE_RET(v, EWL_TREE_VIEW_TYPE, NULL);

        DRETURN_PTR(v->parent, DLEVEL_STABLE);
}

