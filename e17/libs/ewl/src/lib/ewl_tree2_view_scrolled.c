#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static Ewl_View *ewl_tree2_view_scrolled_view = NULL;

Ewl_View *
ewl_tree2_view_scrolled_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_tree2_view_scrolled_view)
	{
		ewl_tree2_view_scrolled_view = ewl_view_new();
		ewl_view_constructor_set(ewl_tree2_view_scrolled_view,
					ewl_tree2_view_scrolled_new);
	}

	DRETURN_PTR(ewl_tree2_view_scrolled_view, DLEVEL_STABLE);
}

Ewl_Widget *
ewl_tree2_view_scrolled_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Tree2_View_Scrolled, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_tree2_view_scrolled_init(EWL_TREE2_VIEW_SCROLLED(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

int
ewl_tree2_view_scrolled_init(Ewl_Tree2_View_Scrolled *tv)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tv", tv, FALSE);

	if (!ewl_tree2_view_init(EWL_TREE2_VIEW(tv)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(tv), EWL_ORIENTATION_VERTICAL);
	ewl_widget_inherit(EWL_WIDGET(tv), EWL_TREE2_VIEW_SCROLLED_TYPE);

	tv->scroll = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(tv), tv->scroll);
	ewl_widget_show(tv->scroll);

	tv->scroll_header = FALSE;
	tv->scroll_visible = TRUE;

	ewl_container_redirect_set(EWL_CONTAINER(tv), EWL_CONTAINER(tv->scroll));

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param scroll: If the headers should be scrolled
 * @return Returns no value
 * @brief Sets if the headers should be scrolled with the tree
 */
void
ewl_tree2_view_scrolled_scroll_headers_set(Ewl_Tree2_View *view, 
						unsigned int scroll)
{
	Ewl_Tree2_View_Scrolled *vs;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("view", view, EWL_TREE2_VIEW_SCROLLED_TYPE);

	if (!view->parent)
	{
		DWARNING("Need parent tree before setting headers scrolled\n");
		DRETURN(DLEVEL_STABLE);
	}

	vs = EWL_TREE2_VIEW_SCROLLED(view);

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
ewl_tree2_view_scrolled_scroll_headers_get(Ewl_Tree2_View *view)
{
	Ewl_Tree2_View_Scrolled *vs;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("view", view, FALSE);
	DCHECK_TYPE_RET("view", view, EWL_TREE2_VIEW_SCROLLED_TYPE, FALSE);

	vs = EWL_TREE2_VIEW_SCROLLED(view);

	DRETURN_INT(vs->scroll_header, DLEVEL_STABLE);
}

