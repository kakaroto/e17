/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_spacer.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns NULL on failure, a pointer to a new spacer on success
 * @brief Allocate and initialize a new spacer
 */
Ewl_Widget *
ewl_spacer_new(void)
{
        Ewl_Spacer *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = NEW(Ewl_Spacer, 1);
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_spacer_init(s)) {
                ewl_widget_destroy(EWL_WIDGET(s));
                s = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the spacer to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialize a spacer to starting values
 *
 * Initializes a spacer to default values and callbacks.
 */
int
ewl_spacer_init(Ewl_Spacer *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, FALSE);

        ewl_widget_init(EWL_WIDGET(s));
        ewl_widget_appearance_set(EWL_WIDGET(s), EWL_SPACER_TYPE);
        ewl_widget_inherit(EWL_WIDGET(s), EWL_SPACER_TYPE);
        ewl_widget_focusable_set(EWL_WIDGET(s), FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

