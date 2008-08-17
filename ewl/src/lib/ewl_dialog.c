/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_dialog.h"
#include "ewl_box.h"
#include "ewl_cell.h"
#include "ewl_separator.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a pointer to a new dialog on success, NULL on failure.
 * @brief Create a new internal dialog
 */
Ewl_Widget *
ewl_dialog_new(void)
{
        Ewl_Dialog *d;

        DENTER_FUNCTION(DLEVEL_STABLE);

        d = NEW(Ewl_Dialog, 1);
        if (!d) {
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        if (!ewl_dialog_init(d)) {
                ewl_widget_destroy(EWL_WIDGET(d));
                d = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(d), DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog to initialize.
 * @return Return TRUE on success, FALSE otherwise.
 * @brief Initialize an internal dialog to starting values
 */
int
ewl_dialog_init(Ewl_Dialog *dialog)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dialog, FALSE);

        w = EWL_WIDGET(dialog);

        if (!ewl_window_init(EWL_WINDOW(dialog))) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        ewl_widget_appearance_set(w, EWL_DIALOG_TYPE);
        ewl_widget_inherit(w, EWL_DIALOG_TYPE);

        dialog->position = EWL_POSITION_BOTTOM;

        /*
         * Create a box for laying out the whole window
         */
        dialog->box = ewl_vbox_new();
        if (!dialog->box) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }
        ewl_container_child_append(EWL_CONTAINER(dialog), dialog->box);
        ewl_widget_internal_set(dialog->box, TRUE);
        ewl_object_fill_policy_set(EWL_OBJECT(dialog->box), EWL_FLAG_FILL_ALL);
        ewl_widget_show(dialog->box);

        /*
         * Setup a vertical box for the displayed window contents.
         */
        dialog->vbox = ewl_vbox_new();
        if (!dialog->vbox) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        ewl_container_child_append(EWL_CONTAINER(dialog->box), dialog->vbox);
        ewl_widget_internal_set(dialog->vbox, TRUE);
        ewl_box_homogeneous_set(EWL_BOX(dialog->vbox), FALSE);
        ewl_object_fill_policy_set(EWL_OBJECT(dialog->vbox), EWL_FLAG_FILL_ALL);
        ewl_widget_show(dialog->vbox);

        dialog->separator = ewl_hseparator_new();
        if (!dialog->separator) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }
        ewl_container_child_append(EWL_CONTAINER(dialog->box), dialog->separator);
        ewl_widget_internal_set(dialog->separator, TRUE);
        ewl_widget_show(dialog->separator);

        /*
         * Create an action area for buttons
         */
        dialog->action_area = ewl_cell_new();
        ewl_widget_appearance_set(dialog->action_area, "actionarea");
        ewl_object_fill_policy_set(EWL_OBJECT(dialog->action_area),
                           EWL_FLAG_FILL_HFILL);
        ewl_widget_internal_set(dialog->action_area, TRUE);
        ewl_container_child_append(EWL_CONTAINER(dialog->box), 
                        dialog->action_area);
        ewl_widget_show(dialog->action_area);

        dialog->action_box = ewl_hbox_new();
        if (!dialog->action_box) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }
        ewl_container_child_append(EWL_CONTAINER(dialog->action_area), 
                        dialog->action_box);
        ewl_box_homogeneous_set(EWL_BOX(dialog->action_box), FALSE);
        ewl_object_fill_policy_set(EWL_OBJECT(dialog->action_box),
                           EWL_FLAG_FILL_NONE);
        ewl_widget_internal_set(dialog->action_box, TRUE);
        ewl_widget_show(dialog->action_box);

        ewl_dialog_active_area_set(dialog, dialog->position);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param d: dialog to change action area position
 * @param pos: the new position for the new action area
 * @return Returns no value.
 * @brief Changes the action area position for a dialog.
 */
void
ewl_dialog_action_position_set(Ewl_Dialog *d, Ewl_Position pos)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(d);
        DCHECK_TYPE(d, EWL_DIALOG_TYPE);

        if (pos == d->position) {
                DRETURN(DLEVEL_STABLE);
        }

        d->position = pos;

        /*
         * First determine the orientation of the dialog area.
         */
        if (pos & (EWL_POSITION_LEFT | EWL_POSITION_RIGHT)) {
                ewl_box_orientation_set(EWL_BOX(d->box),
                                        EWL_ORIENTATION_HORIZONTAL);
                ewl_separator_orientation_set(EWL_SEPARATOR((d->separator)),
                                        EWL_ORIENTATION_VERTICAL);
                ewl_box_orientation_set(EWL_BOX(d->action_box),
                                        EWL_ORIENTATION_VERTICAL);
                ewl_object_fill_policy_set(EWL_OBJECT(d->action_area),
                           EWL_FLAG_FILL_VFILL);
        }
        else {
                ewl_box_orientation_set(EWL_BOX(d->box),
                                        EWL_ORIENTATION_VERTICAL);
                ewl_separator_orientation_set(EWL_SEPARATOR((d->separator)),
                                        EWL_ORIENTATION_HORIZONTAL);
                ewl_box_orientation_set(EWL_BOX(d->action_box),
                                        EWL_ORIENTATION_HORIZONTAL);
                ewl_object_fill_policy_set(EWL_OBJECT(d->action_area),
                           EWL_FLAG_FILL_HFILL);
        }

        ewl_container_child_remove(EWL_CONTAINER(d->box), d->separator);
        ewl_container_child_remove(EWL_CONTAINER(d->box), d->action_area);

        /*
         * Repack order of the widgets to match new position
         */
        if (pos & (EWL_POSITION_LEFT | EWL_POSITION_TOP)) {
                ewl_container_child_prepend(EWL_CONTAINER(d->box),
                                            d->separator);
                ewl_container_child_prepend(EWL_CONTAINER(d->box),
                                            d->action_area);
        }
        else {
                ewl_container_child_append(EWL_CONTAINER(d->box),
                                           d->separator);
                ewl_container_child_append(EWL_CONTAINER(d->box),
                                           d->action_area);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param d: dialog to check action area position
 * @return Returns the current action area position.
 * @brief Checks the action area position for a dialog.
 */
Ewl_Position
ewl_dialog_action_position_get(Ewl_Dialog *d)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(d, EWL_POSITION_BOTTOM);
        DCHECK_TYPE_RET(d, EWL_DIALOG_TYPE, EWL_POSITION_BOTTOM);

        DRETURN_INT(d->position, DLEVEL_STABLE);
}

/**
 * @param d: dialog to change fill policy of the action area position
 * @param pol: The new fill policy to set
 * @return Returns no value.
 * @brief Changes the action area's fill policy for the dialog.
 */
void
ewl_dialog_action_fill_policy_set(Ewl_Dialog *d, unsigned int pol)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(d);
        DCHECK_TYPE(d, EWL_DIALOG_TYPE);

        ewl_object_fill_policy_set(EWL_OBJECT(d->action_box), pol);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param d: dialog to check the fill policy of the action area
 * @return Returns the current fill policy of the action area.
 * @brief Checks the fill policy of the action area.
 */
unsigned int
ewl_dialog_action_fill_policy_get(Ewl_Dialog *d)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(d, EWL_POSITION_BOTTOM);
        DCHECK_TYPE_RET(d, EWL_DIALOG_TYPE, EWL_FLAG_FILL_NONE);

        DRETURN_INT(ewl_object_fill_policy_get(EWL_OBJECT(d->action_box)), 
                        DLEVEL_STABLE);
}

/**
 * @param d: dialog to change the alignment of the action area position
 * @param align: The new alignment to set
 * @return Returns no value.
 * @brief Changes the action area's alignment for the dialog.
 */
void
ewl_dialog_action_alignment_set(Ewl_Dialog *d, unsigned int align)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(d);
        DCHECK_TYPE(d, EWL_DIALOG_TYPE);

        ewl_object_alignment_set(EWL_OBJECT(d->action_box), align);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param d: dialog to check the alignment of the action area
 * @return Returns the current alignment of the action area.
 * @brief Checks the alignment of the action area.
 */
unsigned int
ewl_dialog_action_alignment_get(Ewl_Dialog *d)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(d, EWL_POSITION_BOTTOM);
        DCHECK_TYPE_RET(d, EWL_DIALOG_TYPE, EWL_FLAG_FILL_NONE);

        DRETURN_INT(ewl_object_alignment_get(EWL_OBJECT(d->action_box)), 
                        DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog.
 * @return Returns TRUE if @a dialog has a separator.
 * @brief Checks if @a dialog has a separator or not.
 */
unsigned int
ewl_dialog_has_separator_get(Ewl_Dialog *dialog)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dialog, FALSE);
        DCHECK_TYPE_RET(dialog, EWL_DIALOG_TYPE, FALSE);

        if (!dialog)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        DRETURN_INT(dialog->separator != NULL, DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog.
 * @param has_sep: TRUE to draw the separator, FALSE to hide it.
 * @return Returns no value.
 * @brief Sets the separator of @a dialog.
 */
void
ewl_dialog_has_separator_set(Ewl_Dialog *dialog, unsigned int has_sep)
{
        Ewl_Widget *child;
        int n;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(dialog);
        DCHECK_TYPE(dialog, EWL_DIALOG_TYPE);

        if (has_sep && (dialog->separator == NULL)) {
                ewl_container_child_iterate_begin(EWL_CONTAINER(EWL_DIALOG(dialog)->vbox));
                n = 0;
                child = ewl_container_child_next(EWL_CONTAINER(EWL_DIALOG(dialog)->vbox));
                while (child) {
                        n++;
                        child = ewl_container_child_next(EWL_CONTAINER(EWL_DIALOG(dialog)->vbox));
                }
                dialog->separator = ewl_hseparator_new();
                ewl_container_child_insert(EWL_CONTAINER(dialog->vbox),
                                           dialog->separator, n);
                ewl_object_fill_policy_set(EWL_OBJECT(dialog->separator),
                                           EWL_FLAG_FILL_SHRINKABLE);
                ewl_widget_show(dialog->separator);

        } else if (!has_sep && (dialog->separator != NULL)) {
                ewl_widget_destroy(dialog->separator);
                dialog->separator = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param d: The Ewl_Dialog to work with
 * @param pos: The area of the dialog to set active
 * @return Returns no value
 * @brief Sets the section of the dialog that will have widgets packed into
 * it
 */
void
ewl_dialog_active_area_set(Ewl_Dialog *d, Ewl_Position pos)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(d);
        DCHECK_TYPE(d, EWL_DIALOG_TYPE);

        d->active_area = pos;

        if (pos == d->position)
                ewl_container_redirect_set(EWL_CONTAINER(d),
                                           EWL_CONTAINER(d->action_box));
        else
                ewl_container_redirect_set(EWL_CONTAINER(d),
                                           EWL_CONTAINER(d->vbox));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param d: The Ewl_Dialog to get the active area from
 * @return Returns the active portion of the dialog
 * @brief Retrieve the location of the dialog widgets will be packed into
 */
Ewl_Position
ewl_dialog_active_area_get(Ewl_Dialog *d)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(d, EWL_POSITION_TOP);
        DCHECK_TYPE_RET(d, EWL_DIALOG_TYPE, EWL_POSITION_TOP);

        DRETURN_INT(d->active_area, DLEVEL_STABLE);
}


