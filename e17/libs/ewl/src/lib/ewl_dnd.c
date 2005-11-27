#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

Ewl_Dnd_Types* ewl_dnd_types_for_widget_get(Ewl_Widget* widget)
{
	Ewl_Widget* parent = NULL;
	
	/*We need to get the top-level window widget.  Note
	 * that we assume here that a widget is
	 * a) Parented, and
	 * b) It's top-level parent is a window*/

	parent = widget->parent;
	do {
		if (parent && parent->parent) 
			parent = parent->parent;
		else 
			break;
	} while (TRUE);

	/*Now check if this obj we found is a window*/
	if (parent && ewl_widget_type_is(parent, "window")) {
		return &(EWL_WINDOW(parent)->dnd_types);
	}

	return NULL;
	
}
