
#include <Ewl.h>

/**
 * ewl_spacer_new - allocate and initialize a new spacer
 *
 * Returns NULL on failure, a pointer to a new spacer on success
 */
Ewl_Widget     *ewl_spacer_new()
{
	Ewl_Spacer     *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Spacer, 1);
	if (!s)
		return NULL;

	ZERO(s, Ewl_Spacer, 1);
	ewl_spacer_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * ewl_spacer_init - initialize a spacer to starting values
 * @b: the spacer to initialize
 * @label: set the label of the spacer @b to @label
 *
 * Returns no value. Initializes a spacer to default values and callbacks.
 */
void ewl_spacer_init(Ewl_Spacer * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_widget_init(EWL_WIDGET(s), "/spacer/default");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
