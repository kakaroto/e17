#include <Ewl.h>

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
	ZERO(s, Ewl_Separator, 1);

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

	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_widget_init(w, "hseparator");
	else
		ewl_widget_init(w, "vseparator");

	ewl_object_set_alignment(EWL_OBJECT(s), EWL_ALIGNMENT_LEFT);
	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FILL_POLICY_FILL);

	s->orientation = o;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
