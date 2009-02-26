/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_togglebutton.h"
#include "ewl_button.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns the newly allocated togglebutton on success, NULL on failure.
 * @brief Allocate and initialize a new check button
 */
Ewl_Widget *
ewl_togglebutton_new(void)
{
        Ewl_Togglebutton *b;

        DENTER_FUNCTION(DLEVEL_STABLE);

        b = NEW(Ewl_Togglebutton, 1);
        if (!b)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_togglebutton_init(b)) {
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
 * The internal structures and callbacks of the togglebutton are initialized to
 * default values.
 */
int
ewl_togglebutton_init(Ewl_Togglebutton *cb)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cb, FALSE);

        w = EWL_WIDGET(cb);

        if (!ewl_button_init(EWL_BUTTON(cb)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_TOGGLEBUTTON_TYPE);
        ewl_widget_inherit(w, EWL_TOGGLEBUTTON_TYPE);
        ewl_callback_append(w, EWL_CALLBACK_CLICKED,
                                ewl_togglebutton_cb_clicked, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param cb: The togglebutton to set the checked value of
 * @param c: The checked value to set
 * @return Returns no value.
 * @brief Update the checked state of the check button.
 */
void
ewl_togglebutton_checked_set(Ewl_Togglebutton *cb, unsigned int c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cb);
        DCHECK_TYPE(cb, EWL_TOGGLEBUTTON_TYPE);

        if (cb->checked == !!c)
                DRETURN(DLEVEL_STABLE);

        cb->checked = !!c;

        if (cb->checked)
                ewl_widget_state_set(EWL_WIDGET(cb), "checked",
                                EWL_STATE_PERSISTENT);
        else
                ewl_widget_state_set(EWL_WIDGET(cb), "default",
                                EWL_STATE_PERSISTENT);

        ewl_callback_call(EWL_WIDGET(cb), EWL_CALLBACK_VALUE_CHANGED);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cb: The togglebutton to work with
 * @return Returns TRUE if the togglebutton is set, FALSE otherwise
 * @brief Retrieve the checked state of a check button.
 */
unsigned int
ewl_togglebutton_checked_get(Ewl_Togglebutton *cb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cb, FALSE);
        DCHECK_TYPE_RET(cb, EWL_TOGGLEBUTTON_TYPE, FALSE);

        DRETURN_INT(cb->checked, DLEVEL_STABLE);
}

/**
 * @param cb: The togglebutton to toggle the state
 * @return Returns no value.
 * @brief Toggle the checked state of the toggle button.
 */
void
ewl_togglebutton_toggle(Ewl_Togglebutton *cb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cb);
        DCHECK_TYPE(cb, EWL_TOGGLEBUTTON_TYPE);

        ewl_togglebutton_checked_set(cb, !cb->checked);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The clicked callback for the togglebutton
 */
void
ewl_togglebutton_cb_clicked(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TOGGLEBUTTON_TYPE);

        ewl_togglebutton_toggle(EWL_TOGGLEBUTTON(w));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

