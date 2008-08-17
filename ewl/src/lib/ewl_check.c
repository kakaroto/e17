/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_check.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns the newly allocated check on success, NULL on failure.
 * @brief Allocate and initialize a new check
 */
Ewl_Widget *
ewl_check_new(void)
{
        Ewl_Check *b;

        DENTER_FUNCTION(DLEVEL_STABLE);

        b = NEW(Ewl_Check, 1);
        if (!b)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_check_init(EWL_CHECK(b))) {
                ewl_widget_destroy(EWL_WIDGET(b));
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param cb: the check to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the members and callbacks of a check
 *
 * The internal structures and callbacks of the check are initialized ot
 * default values.
 */
int
ewl_check_init(Ewl_Check *cb)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cb, FALSE);

        w = EWL_WIDGET(cb);

        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_CHECK_TYPE);
        ewl_widget_inherit(w, EWL_CHECK_TYPE);

        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
        ewl_object_preferred_inner_size_set(EWL_OBJECT(w), 20, 20);

        ewl_callback_append(w, EWL_CALLBACK_CLICKED,
                                ewl_check_cb_clicked, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_OUT,
                                ewl_check_cb_update_check, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param cb: the check to change the status
 * @param c: the new status of the check
 * @return Returns no value.
 * @brief Change the checked status of the check
 *
 * Changes the checked status of the check and updates it's appearance to
 * reflect the change.
 */
void
ewl_check_checked_set(Ewl_Check *cb, int c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cb);
        DCHECK_TYPE(cb, EWL_CHECK_TYPE);

        if (cb->checked == !!c)
                DRETURN(DLEVEL_STABLE);

        cb->checked = !!c;
        ewl_check_cb_update_check(EWL_WIDGET(cb), NULL, NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cb: the check to examine for it's checked state
 * @return Returns TRUE if the check is checked, FALSE if not.
 * @brief Determine the check state of the check
 */
int
ewl_check_is_checked(Ewl_Check *cb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cb, 0);
        DCHECK_TYPE_RET(cb, EWL_CHECK_TYPE, 0);

        DRETURN_INT(cb->checked, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The clicked callback for the check widget
 */
void
ewl_check_cb_clicked(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Check *cb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CHECK_TYPE);

        cb = EWL_CHECK(w);
        cb->checked ^= 1;
        ewl_check_cb_update_check(w, NULL, NULL);
        ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback to update the checkmark
 */
void
ewl_check_cb_update_check(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Check *cb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CHECK_TYPE);

        cb = EWL_CHECK(w);
        if (cb->checked)
                ewl_widget_state_set(w, "checked", EWL_STATE_PERSISTENT);
        else
                ewl_widget_state_set(w, "default", EWL_STATE_PERSISTENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

