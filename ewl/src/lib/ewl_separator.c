/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_separator.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns pointer to new separator widget on success, NULL on failure.
 * @brief Allocate a new separator widget with default (horizontal) orientation
 */
Ewl_Widget *
ewl_separator_new(void)
{
        Ewl_Separator  *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = NEW(Ewl_Separator, 1);
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_separator_init(s)) {
                ewl_widget_destroy(EWL_WIDGET(s));
                s = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @return Returns pointer to new separator widget on success, NULL on failure.
 * @brief Allocate a new separator widget with horizontal orientation
 */
Ewl_Widget *
ewl_hseparator_new(void)
{
        Ewl_Widget *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = ewl_separator_new();
        if (s)
                ewl_separator_orientation_set(EWL_SEPARATOR(s),
                                EWL_ORIENTATION_HORIZONTAL);

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @return Returns pointer to new separator widget on success, NULL on failure.
 * @brief Allocate a new separator widget with vertical orientation
 */
Ewl_Widget *
ewl_vseparator_new(void)
{
        Ewl_Widget *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = ewl_separator_new();
        if (s)
                ewl_separator_orientation_set(EWL_SEPARATOR(s),
                                EWL_ORIENTATION_VERTICAL);

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the separator to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the separator and inherited fields
 *
 * Clears the contents of the separator and stores the
 * default values.
 */
int
ewl_separator_init(Ewl_Separator *s)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, FALSE);

        w = EWL_WIDGET(s);
        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(w, EWL_SEPARATOR_TYPE);
        ewl_widget_appearance_set(EWL_WIDGET(s), "hseparator");
        ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_HFILL |
                                                  EWL_FLAG_FILL_SHRINKABLE);

        ewl_object_alignment_set(EWL_OBJECT(s), EWL_FLAG_ALIGN_LEFT);

        s->orientation = EWL_ORIENTATION_HORIZONTAL;

        ewl_widget_focusable_set(w, FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: The separator to set the orientation on
 * @param o: The orientation to set on the separator
 * @return Returns no value
 * @brief Set the orientation of the separator
 */
void
ewl_separator_orientation_set(Ewl_Separator *s, Ewl_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SEPARATOR_TYPE);

        if (s->orientation == o)
                DRETURN(DLEVEL_STABLE);

        s->orientation = o;

        if (o == EWL_ORIENTATION_HORIZONTAL) {
                ewl_widget_appearance_set(EWL_WIDGET(s), "hseparator");
                ewl_object_fill_policy_set(EWL_OBJECT(s),
                                           EWL_FLAG_FILL_HFILL |
                                           EWL_FLAG_FILL_HSHRINKABLE);
        }
        else {
                ewl_widget_appearance_set(EWL_WIDGET(s), "vseparator");
                ewl_object_fill_policy_set(EWL_OBJECT(s),
                                           EWL_FLAG_FILL_VFILL |
                                           EWL_FLAG_FILL_VSHRINKABLE);
        }

        ewl_widget_configure(EWL_WIDGET(s));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The separator to get the orientation from
 * @return Returns the orientation currently set on the separator
 * @brief This retrieves the current orientation of the separator
 */
Ewl_Orientation
ewl_separator_orientation_get(Ewl_Separator *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, EWL_ORIENTATION_HORIZONTAL);
        DCHECK_TYPE_RET(s, EWL_SEPARATOR_TYPE, EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(s->orientation, DLEVEL_STABLE);
}

