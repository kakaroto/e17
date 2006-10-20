#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static Ewl_View *ewl_tree2_view_plain_view = NULL;

/**
 * @return Returns a default Ewl_Tree2_View_Plain struct
 * @brief Retrieves a shared Ewl_Tree2_View_Plain widget
 */
Ewl_View *
ewl_tree2_view_plain_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_tree2_view_plain_view)
	{
		ewl_tree2_view_plain_view = ewl_view_new();
		ewl_view_constructor_set(ewl_tree2_view_plain_view,
					ewl_tree2_view_plain_new);
	}

	DRETURN_PTR(ewl_tree2_view_plain_view, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Creates a new Ewl_Tree2_View_Plain_View
 */
Ewl_Widget *
ewl_tree2_view_plain_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Tree2_View_Plain, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_tree2_view_plain_init(EWL_TREE2_VIEW_PLAIN(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param tv: The tree view to work with
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Tree2_View_Plain to default values
 */
int
ewl_tree2_view_plain_init(Ewl_Tree2_View_Plain *tv)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tv", tv, FALSE);

	if (!ewl_tree2_view_init(EWL_TREE2_VIEW(tv)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(tv), EWL_TREE2_VIEW_PLAIN_TYPE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

