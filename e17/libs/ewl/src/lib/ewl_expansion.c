/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_check.h"
#include "ewl_expansion.h"
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns the newly allocated expansion on success, NULL on failure.
 * @brief Allocate and initialize a new expansion
 */
Ewl_Widget *
ewl_expansion_new(void)
{
	Ewl_Expansion *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Expansion, 1);
	if (!b)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_expansion_init(EWL_EXPANSION(b))) {
		ewl_widget_destroy(EWL_WIDGET(b));
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param cb: the expansion to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the members and callbacks of a expansion
 *
 * The internal structures and callbacks of the expansion are initialized to
 * default values.
 */
int
ewl_expansion_init(Ewl_Expansion *cb)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cb", cb, FALSE);

	w = EWL_WIDGET(cb);

	if (!ewl_check_init(w))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_EXPANSION_TYPE);
	ewl_widget_inherit(w, EWL_EXPANSION_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
	ewl_object_preferred_inner_size_set(EWL_OBJECT(w), 20, 20);

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_OUT,
				ewl_expansion_cb_update_expandable, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REVEAL,
				ewl_expansion_cb_reveal, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * @param cb: the expansion to change the status
 * @param c: the new status of the expansion
 * @return Returns no value.
 * @brief Change the expanded status of the expansion
 *
 * Changes the expanded status of the expansion and updates it's appearance
 * to reflect the change.
 */
void
ewl_expansion_expanded_set(Ewl_Expansion *cb, int c)
{
	ewl_check_checked_set(EWL_CHECK(cb), c);
}

/**
 * @param cb: the expansion to examine for it's expanded state
 * @return Returns TRUE if the expansion is expanded, FALSE if not.
 * @brief Determine the expanded state of the expansion
 */
int
ewl_expansion_is_expanded(Ewl_Expansion *cb)
{
	return ewl_check_is_checked(EWL_CHECK(cb));
}

/**
 * @param cb: the expansion to change the status
 * @param c: the new status of the expansion
 * @return Returns no value.
 * @brief Change the expandable status of the expansion
 *
 * Changes the expandable status of the expansion and updates it's appearance
 * to reflect the change.
 */
void
ewl_expansion_expandable_set(Ewl_Expansion *cb, int c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cb", cb);
	DCHECK_TYPE("cb", cb, EWL_EXPANSION_TYPE);

	cb->expandable = !!c;
	ewl_expansion_cb_update_expandable(EWL_WIDGET(cb), NULL, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cb: the expansion to examine for it's expandable state
 * @return Returns TRUE if the expansion is expandable, FALSE if not.
 * @brief Determine the expandable state of the expansion
 */
int
ewl_expansion_is_expandable(Ewl_Expansion *cb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cb", cb, 0);
	DCHECK_TYPE_RET("cb", cb, EWL_EXPANSION_TYPE, 0);

	DRETURN_INT(cb->expandable, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback to update the expansion
 */
void
ewl_expansion_cb_update_expandable(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Expansion *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cb = EWL_EXPANSION(w);
	if (cb->expandable)
		ewl_widget_state_set(w, "expandable", EWL_STATE_TRANSIENT);
	else
		ewl_widget_state_set(w, "nonexpandable", EWL_STATE_TRANSIENT);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback to update the state on reveal
 */
void
ewl_expansion_cb_reveal(Ewl_Widget *w, void *ev_data __UNUSED__,
	void *user_data __UNUSED__)
{
	Ewl_Expansion *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cb = EWL_EXPANSION(w);

	ewl_expansion_cb_update_expandable(w, NULL, NULL);
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
