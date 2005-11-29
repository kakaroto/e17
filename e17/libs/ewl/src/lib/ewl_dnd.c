#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

Ewl_Dnd_Types *
ewl_dnd_types_for_widget_get(Ewl_Widget *widget)
{
	Ewl_Widget *parent = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);
	DCHECK_TYPE_RET("widget", widget, "widget", NULL);
	
	/* We need to get the top-level window widget.  Note
	 * that we assume here that a widget is
	 * a) Parented, and
	 * b) It's top-level parent is a window */
	parent = widget->parent;
	while (parent && parent->parent)
		parent = parent->parent;

	/* Now check if this obj we found is a window */
	if (parent && ewl_widget_type_is(parent, "window")) 
		DRETURN_PTR(&(EWL_WINDOW(parent)->dnd_types), DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

