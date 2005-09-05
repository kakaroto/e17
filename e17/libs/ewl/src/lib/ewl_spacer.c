#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns NULL on failure, a pointer to a new spacer on success
 * @brief Allocate and initialize a new spacer
 */
Ewl_Widget     *ewl_spacer_new()
{
	Ewl_Spacer     *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Spacer, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_spacer_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the spacer to initialize
 * @return Returns no value.
 * @brief Initialize a spacer to starting values
 *
 * Initializes a spacer to default values and callbacks.
 */
void ewl_spacer_init(Ewl_Spacer * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_widget_init(EWL_WIDGET(s), "spacer");
	ewl_widget_inherit(EWL_WIDGET(s), "spacer");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
