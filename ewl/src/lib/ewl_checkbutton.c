/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_checkbutton.h"
#include "ewl_button.h"
#include "ewl_check.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns the newly allocated checkbutton on success, NULL on failure.
 * @brief Allocate and initialize a new check button
 */
Ewl_Widget *
ewl_checkbutton_new(void)
{
        Ewl_Checkbutton *b;

        DENTER_FUNCTION(DLEVEL_STABLE);

        b = NEW(Ewl_Checkbutton, 1);
        if (!b)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_checkbutton_init(b)) {
                ewl_widget_destroy(EWL_WIDGET(b));
                b = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param cb: the check button to initialize
 * @return Returns no value.
 * @brief Initialize the members and callbacks of a check button
 *
 * The internal structures and callbacks of the checkbutton are initialized to
 * default values.
 */
int
ewl_checkbutton_init(Ewl_Checkbutton *cb)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cb, FALSE);

        w = EWL_WIDGET(cb);

        if (!ewl_togglebutton_init(EWL_TOGGLEBUTTON(cb)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_CHECKBUTTON_TYPE);
        ewl_widget_inherit(w, EWL_CHECKBUTTON_TYPE);

        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_HFILL);
        ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_LEFT);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

