#include "ewl_private.h"
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

extern Ecore_List *ewl_embed_list;
static unsigned int key_modifiers = 0;

/**
 * @internal
 * @return Returns true or false to indicate success in initializing events.
 * @brief Initialize the event handlers for dispatching to proper widgets
 */
int
ewl_ev_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns the current mask of modifier keys.
 * @brief Retrieve the current mask of modifiers that are set.
 */
unsigned int
ewl_ev_modifiers_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT(key_modifiers, DLEVEL_STABLE);
}

/**
 * @param modifiers: the key modifiers to set
 * @return Returns no value.
 * @brief Sets the key modifiers to the given value
 */
void
ewl_ev_modifiers_set(unsigned int modifiers)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	key_modifiers = modifiers;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

