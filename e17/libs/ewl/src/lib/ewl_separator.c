#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @param o: the orientation of the newly allocated separator widget
 * @return Returns pointer to new separator widget on success, NULL on failure.
 * @brief Allocate a new separator widget
 */
Ewl_Widget     *ewl_separator_new(Ewl_Orientation o)
{
	Ewl_Separator  *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Separator, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_separator_init(s, o);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the separator to initialize
 * @param o: the orientation of the separator to be initialized
 * @return Returns no value.
 * @brief Initialize the separator and inherited fields
 *
 * Clears the contents of the separator and stores the
 * default values along with the orientation specified by @a o.
 */
void ewl_separator_init(Ewl_Separator * s, Ewl_Orientation o)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = EWL_WIDGET(s);
	ewl_widget_init(w);

	if (o == EWL_ORIENTATION_HORIZONTAL) {
		ewl_widget_appearance_set(EWL_WIDGET(s), "hseparator");
		ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_HFILL);
	}
	else {
		ewl_widget_appearance_set(w, "vseparator");
		ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_VFILL);
	}
	ewl_widget_inherit(w, "separator");

	ewl_object_alignment_set(EWL_OBJECT(s), EWL_FLAG_ALIGN_LEFT);

	s->orientation = o;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

