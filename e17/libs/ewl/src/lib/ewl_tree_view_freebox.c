/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_tree_view_freebox.h"
#include "ewl_freebox.h"
#include "ewl_scrollpane.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static Ewl_View *ewl_tree_view_freebox_view = NULL;
static Ewl_Widget *ewl_tree_view_freebox_cb_widget_fetch(void *data,
        						unsigned int col,
        						unsigned int row);

/**
 * @return Returns the view for this widget
 * @brief Retrieves the view for this widget
 */
const Ewl_View *
ewl_tree_view_freebox_get(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        if (!ewl_tree_view_freebox_view)
        {
        	ewl_tree_view_freebox_view = ewl_view_new();
        	ewl_view_widget_fetch_set(ewl_tree_view_freebox_view,
        			ewl_tree_view_freebox_cb_widget_fetch);
        }

        DRETURN_PTR(ewl_tree_view_freebox_view, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_tree_view_freebox_cb_widget_fetch(void *data __UNUSED__,
        				unsigned int col __UNUSED__,
        				unsigned int row __UNUSED__)
{
        Ewl_Widget *tree;

        DENTER_FUNCTION(DLEVEL_STABLE);

        tree = ewl_tree_view_freebox_new();
        ewl_widget_show(tree);

        DRETURN_PTR(tree, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Creates and initializes a new Ewl_Tree_View_Freebox widget
 */
Ewl_Widget *
ewl_tree_view_freebox_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Tree_View_Freebox, 1);
        if (!w)
        	DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_tree_view_freebox_init(EWL_TREE_VIEW_FREEBOX(w)))
        {
        	ewl_widget_destroy(w);
        	w = NULL;
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param tv: The view to work with
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes a tree view to default values
 */
int
ewl_tree_view_freebox_init(Ewl_Tree_View_Freebox *tv)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tv, FALSE);

        if (!ewl_tree_view_init(EWL_TREE_VIEW(tv)))
        	DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(tv), EWL_ORIENTATION_VERTICAL);
        ewl_widget_inherit(EWL_WIDGET(tv), EWL_TREE_VIEW_FREEBOX_TYPE);


        tv->scroll = ewl_scrollpane_new();
        ewl_container_child_append(EWL_CONTAINER(tv), tv->scroll);
        ewl_widget_inherit(EWL_WIDGET(tv), EWL_TREE_VIEW_FREEBOX_TYPE);
        ewl_widget_show(tv->scroll);

        tv->fbox = ewl_vfreebox_new();
        ewl_freebox_layout_type_set(EWL_FREEBOX(tv->fbox),
        			EWL_FREEBOX_LAYOUT_AUTO);
        ewl_container_child_append(EWL_CONTAINER(tv->scroll), tv->fbox);
        ewl_widget_show(tv->fbox);

        ewl_container_redirect_set(EWL_CONTAINER(tv), 
        			EWL_CONTAINER(tv->fbox));

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}



