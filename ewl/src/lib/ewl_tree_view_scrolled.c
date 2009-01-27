/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_tree_view_scrolled.h"
#include "ewl_scrollpane.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static Ewl_View *ewl_tree_view_scrolled_view = NULL;

static Ewl_Widget * ewl_tree_view_cb_constructor(unsigned int col,
                                                        void *pr_data);

/**
 * @return Returns the view for this widget
 * @brief Retrieves the view for this widget
 */
const Ewl_View *
ewl_tree_view_scrolled_get(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        if (!ewl_tree_view_scrolled_view)
        {
                ewl_tree_view_scrolled_view = ewl_view_new();
                ewl_view_widget_constructor_set(ewl_tree_view_scrolled_view,
                                        ewl_tree_view_cb_constructor);
        }

        DRETURN_PTR(ewl_tree_view_scrolled_view, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_tree_view_cb_constructor(unsigned int col __UNUSED__,
                                void *pr_data __UNUSED__)
{
        Ewl_Widget *tree;

        DENTER_FUNCTION(DLEVEL_STABLE);

        tree = ewl_tree_view_scrolled_new();
        ewl_widget_show(tree);

        DRETURN_PTR(tree, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Creates and initializes a new Ewl_Tree_View_Scrolled widget
 */
Ewl_Widget *
ewl_tree_view_scrolled_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Tree_View_Scrolled, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_tree_view_scrolled_init(EWL_TREE_VIEW_SCROLLED(w)))
        {
                ewl_widget_destroy(w);
                w = NULL;
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param tv: The view to work with
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialies a tree view to default values
 */
int
ewl_tree_view_scrolled_init(Ewl_Tree_View_Scrolled *tv)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tv, FALSE);

        if (!ewl_tree_view_init(EWL_TREE_VIEW(tv)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(tv), EWL_ORIENTATION_VERTICAL);
        ewl_widget_inherit(EWL_WIDGET(tv), EWL_TREE_VIEW_SCROLLED_TYPE);

        tv->scroll = ewl_scrollpane_new();
        ewl_container_child_append(EWL_CONTAINER(tv), tv->scroll);
        ewl_widget_show(tv->scroll);

        tv->scroll_header = FALSE;
        tv->scroll_visible = TRUE;

        ewl_container_redirect_set(EWL_CONTAINER(tv), EWL_CONTAINER(tv->scroll));

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param view: The view to work with
 * @param scroll: If the headers should be scrolled
 * @return Returns no value
 * @brief Sets if the headers should be scrolled with the tree
 */
void
ewl_tree_view_scrolled_scroll_headers_set(Ewl_Tree_View *view,
                                                unsigned int scroll)
{
        Ewl_Tree_View_Scrolled *vs;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(view);
        DCHECK_TYPE(view, EWL_TREE_VIEW_SCROLLED_TYPE);

        if (!view->parent)
        {
                DWARNING("Need parent tree before setting headers scrolled.");
                DRETURN(DLEVEL_STABLE);
        }

        vs = EWL_TREE_VIEW_SCROLLED(view);

        if (vs->scroll_header == scroll)
                DRETURN(DLEVEL_STABLE);

        vs->scroll_header = scroll;
        if (vs->scroll_header)
                ewl_container_child_prepend(EWL_CONTAINER(vs),
                                                view->parent->header);
        else
                ewl_container_child_prepend(EWL_CONTAINER(view->parent),
                                                        view->parent->header);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param view: The view to work with
 * @return Returns no value
 * @brief Retrieves if the tree headers will be scrolled or not
 */
unsigned int
ewl_tree_view_scrolled_scroll_headers_get(Ewl_Tree_View *view)
{
        Ewl_Tree_View_Scrolled *vs;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(view, FALSE);
        DCHECK_TYPE_RET(view, EWL_TREE_VIEW_SCROLLED_TYPE, FALSE);

        vs = EWL_TREE_VIEW_SCROLLED(view);

        DRETURN_INT(vs->scroll_header, DLEVEL_STABLE);
}

