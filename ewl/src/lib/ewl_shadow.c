/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_shadow.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a new shadow container on success, NULL on failure.
 * @brief Allocate and initialize a new shadow container
 */
Ewl_Widget *
ewl_shadow_new(void)
{
        Ewl_Shadow *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = NEW(Ewl_Shadow, 1);
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_shadow_init(s))
        {
                ewl_widget_destroy(EWL_WIDGET(s));
                s = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the shadow container to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a shadow container to default values
 */
int
ewl_shadow_init(Ewl_Shadow *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, FALSE);

        if (!ewl_box_init(EWL_BOX(s)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(s), EWL_ORIENTATION_VERTICAL);
        ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_SHRINKABLE);

        ewl_widget_appearance_set(EWL_WIDGET(s), EWL_SHADOW_TYPE);
        ewl_widget_inherit(EWL_WIDGET(s), EWL_SHADOW_TYPE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

