/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

extern Ecore_List *ewl_embed_list;
static unsigned int key_modifiers = 0;

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

