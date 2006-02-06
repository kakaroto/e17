#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a new Ewl_View object on success or NULL on failure
 * @brief Creates a new Ewl_View object
 */
Ewl_View *
ewl_view_new(void)
{
	Ewl_View *view;

	DENTER_FUNCTION(DLEVEL_STABLE);

	view = NEW(Ewl_View, 1);
	if (!ewl_view_init(view))
	{
		FREE(view);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(view, DLEVEL_STABLE);
}

/**
 * @param view: The Ewl_View to initialize
 * @return Returns TRUEE on success or FALSE on failure
 * @brief Initializes an Ewl_View object to default values
 */
int
ewl_view_init(Ewl_View *view)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("view", view, FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to set the constructor into
 * @param construct: The Ewl_View_Constructor to set into the view
 * @return Returns no value.
 * @brief This will set the given constructor into the view
 */
void
ewl_view_constructor_set(Ewl_View *v, Ewl_View_Constructor construct)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("v", v);

	v->construct = construct;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to get the constructor from
 * @return Returns the Ewl_View_Constructor set into the view or NULL if
 * none set.
 */
Ewl_View_Constructor
ewl_view_constructor_get(Ewl_View *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("v", v, NULL);

	DRETURN_PTR(v->construct, DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to set the assignment function into
 * @param assign: The Ewl_View_Assign assignment function to set
 * @return Returns no value.
 */
void
ewl_view_assign_set(Ewl_View *v, Ewl_View_Assign assign)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("v", v);

	v->assign = assign;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to get the assignment function from
 * @return Returns the Ewl_View_Assign set into the Ewl_View or NULL if none
 * set.
 */
Ewl_View_Assign
ewl_view_assign_get(Ewl_View *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("v", v, NULL);

	DRETURN_PTR(v->assign, DLEVEL_STABLE);
}

