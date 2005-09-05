#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @param pos: the position of the action area.
 * @return Returns a pointer to a new dialog on success, NULL on failure.
 * @brief Create a new internal dialog
 */
Ewl_Widget *ewl_dialog_new(Ewl_Position pos)
{
	Ewl_Dialog *d;

	DENTER_FUNCTION(DLEVEL_STABLE);

	d = NEW(Ewl_Dialog, 1);
	if (!d)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_dialog_init(d, pos);

	DRETURN_PTR(EWL_WIDGET(d), DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog to initialize.
 * @param pos: the position of the action area.
 * @return Return TRUE on success, FALSE otherwise.
 * @brief Initialize an internal dialog to starting values
 */
int ewl_dialog_init(Ewl_Dialog * dialog, Ewl_Position pos)
{
	Ewl_Widget *w;
	Ewl_Widget *box;
	Ewl_Widget *spacer;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("d", dialog, 0);

	w = EWL_WIDGET(dialog);

	if (!ewl_window_init(EWL_WINDOW(dialog)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, "window");
	ewl_widget_inherit(w, "dialog");

	dialog->position = pos;

	switch (pos) {
	case EWL_POSITION_LEFT:
		box = ewl_hbox_new();
		break;
	case EWL_POSITION_RIGHT:
		box = ewl_hbox_new();
		break;
	case EWL_POSITION_TOP:
		box = ewl_vbox_new();
		break;
	default:
		box = ewl_vbox_new();
		break;
	}

	if (box) {
		ewl_container_child_append(EWL_CONTAINER(dialog), box);
		ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_ALL);
		ewl_widget_show(box);
	}

	dialog->vbox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(dialog->vbox), EWL_FLAG_FILL_ALL);
	if (dialog->vbox) {
		ewl_container_child_append(EWL_CONTAINER(box),
					   dialog->vbox);
		ewl_box_homogeneous_set(EWL_BOX(dialog->vbox), FALSE);
		switch (pos) {
		case EWL_POSITION_LEFT:
			{
				dialog->action_area = ewl_vbox_new();
				dialog->separator = ewl_vseparator_new();
				break;
			}
		case EWL_POSITION_RIGHT:
			{
				dialog->action_area = ewl_vbox_new();
				dialog->separator = ewl_vseparator_new();
				break;
			}
		case EWL_POSITION_TOP:
			{
				dialog->action_area = ewl_hbox_new();
				dialog->separator = ewl_hseparator_new();
				break;
			}
		default:
			{
				dialog->action_area = ewl_hbox_new();
				dialog->separator = ewl_hseparator_new();
				break;
			}
		}
		ewl_widget_show(dialog->vbox);
	}

	if (dialog->separator) {
		switch (pos) {
		case EWL_POSITION_LEFT:
			{
				ewl_container_child_prepend(EWL_CONTAINER
							    (box),
							    dialog->
							    separator);
				break;
			}
		case EWL_POSITION_TOP:
			{
				ewl_container_child_prepend(EWL_CONTAINER
							    (box),
							    dialog->
							    separator);
				break;
			}
		case EWL_POSITION_RIGHT:
			{
				ewl_container_child_append(EWL_CONTAINER
							   (box),
							   dialog->
							   separator);
				break;
			}
		default:
			{
				ewl_container_child_append(EWL_CONTAINER
							   (box),
							   dialog->
							   separator);
				break;
			}
		}
		ewl_object_fill_policy_set(EWL_OBJECT(dialog->separator),
					   EWL_FLAG_FILL_SHRINK);
		ewl_widget_show(dialog->separator);
	}

	if (dialog->action_area) {
		switch (pos) {
		case EWL_POSITION_LEFT:
			{
				ewl_container_child_prepend(EWL_CONTAINER
							    (box),
							    dialog->
							    action_area);
				ewl_object_fill_policy_set(EWL_OBJECT
							   (dialog->
							    action_area),
							   EWL_FLAG_FILL_VFILL);
				break;
			}
		case EWL_POSITION_TOP:
			{
				ewl_container_child_prepend(EWL_CONTAINER
							    (box),
							    dialog->
							    action_area);
				ewl_object_fill_policy_set(EWL_OBJECT
							   (dialog->
							    action_area),
							   EWL_FLAG_FILL_HFILL);
				break;
			}
		case EWL_POSITION_RIGHT:
			{
				ewl_container_child_append(EWL_CONTAINER
							   (box),
							   dialog->
							   action_area);
				ewl_object_fill_policy_set(EWL_OBJECT
							   (dialog->
							    action_area),
							   EWL_FLAG_FILL_VFILL);
				break;
			}
		default:
			{
				ewl_container_child_append(EWL_CONTAINER
							   (box),
							   dialog->
							   action_area);
				ewl_object_fill_policy_set(EWL_OBJECT
							   (dialog->
							    action_area),
							   EWL_FLAG_FILL_HFILL);
				break;
			}
		}

		ewl_box_homogeneous_set(EWL_BOX(dialog->action_area),
					FALSE);
		ewl_widget_show(dialog->action_area);

		spacer = ewl_spacer_new();
		ewl_container_child_append(EWL_CONTAINER
					   (dialog->action_area), spacer);
		ewl_object_fill_policy_set(EWL_OBJECT(spacer),
					   EWL_FLAG_FILL_FILL);
		ewl_widget_show(spacer);

		ewl_container_redirect_set(EWL_CONTAINER(dialog),
					   EWL_CONTAINER(dialog->
							 action_area));
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog to add the widget in.
 * @param w: the widget to add in the vbox.
 * @return Returns no value.
 * @brief Convenient function to add widgets in the vbox.
 */
void ewl_dialog_widget_add(Ewl_Dialog * dialog, Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("d", dialog);

	if (!dialog)
		return;

	switch (dialog->position) {
	case EWL_POSITION_LEFT:
		{
			ewl_container_child_append(EWL_CONTAINER(dialog->vbox),
						   w);
			break;
		}
	case EWL_POSITION_TOP:
		{
			ewl_container_child_append(EWL_CONTAINER(dialog->vbox),
						   w);
			break;
		}
	case EWL_POSITION_RIGHT:
		{
			ewl_container_child_prepend(EWL_CONTAINER(dialog->vbox),
						    w);
			break;
		}
	default:
		{
			ewl_container_child_prepend(EWL_CONTAINER(dialog->vbox),
						    w);
			break;
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog to add the button in.
 * @param button_text: the text of the button or a stock Id.
 * @param response_id: The Id that will be retured when clicking on the button.
 * @return Returns a button, or NULL on failure.
 * @brief Add a (stock) button on the right of the action_area of @a dialog.
 */
Ewl_Widget *ewl_dialog_button_add(Ewl_Dialog * dialog, char *button_text,
				  int response_id)
{
	Ewl_Widget *button;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("d", dialog, 0);

	if (!dialog)
		return NULL;

	button = ewl_button_stock_with_id_new(button_text, response_id);
	ewl_object_padding_set(EWL_OBJECT(button), 0, 3, 3, 3);
	ewl_container_child_append(EWL_CONTAINER(dialog->action_area),
				   button);
	ewl_object_fill_policy_set(EWL_OBJECT(button),
				   EWL_FLAG_FILL_VFILL
				   || EWL_FLAG_FILL_SHRINK);

	DRETURN_PTR(button, DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog to add the button in.
 * @param button_text: the text of the button or a stock Id.
 * @param response_id: The Id that will be retured when clicking on the button.
 * @return Returns a button, or NULL on failure.
 * @brief Same as ewl_dialog_add_button(), but add the button on the left.
 */
Ewl_Widget *ewl_dialog_button_left_add(Ewl_Dialog * dialog,
				       char *button_text, int response_id)
{
	Ewl_Widget *button;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("d", dialog, 0);

	if (!dialog)
		return NULL;

	button = ewl_button_stock_with_id_new(button_text, response_id);
	ewl_object_padding_set(EWL_OBJECT(button), 0, 3, 3, 3);
	ewl_container_child_prepend(EWL_CONTAINER(dialog->action_area),
				    button);
	ewl_object_fill_policy_set(EWL_OBJECT(button),
				   EWL_FLAG_FILL_VFILL
				   || EWL_FLAG_FILL_SHRINK);

	DRETURN_PTR(button, DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog.
 * @return Returns TRUE if @a dialog has a separator.
 * @brief Checks if @a dialog has a separator or not.
 */
unsigned int ewl_dialog_has_separator_get(Ewl_Dialog * dialog)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("d", dialog, 0);

	if (!dialog)
		return FALSE;

	DRETURN_INT(dialog->separator != NULL, DLEVEL_STABLE);
}

/**
 * @param dialog: the dialog.
 * @param has_sep: TRUE to draw the separator, FALSE to hide it.
 * @return Returns no value.
 * @brief Sets the separator of @a dialog.
 */
void
ewl_dialog_has_separator_set(Ewl_Dialog * dialog, unsigned int has_sep)
{
	Ewl_Widget *child;
	int n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("d", dialog);

	if (!dialog)
		DLEAVE_FUNCTION(DLEVEL_STABLE);

	if (has_sep && (dialog->separator == NULL)) {
		ewl_container_child_iterate_begin(EWL_CONTAINER(EWL_DIALOG
								(dialog)->
								vbox));
		n = 0;
		child = ewl_container_child_next(EWL_CONTAINER(EWL_DIALOG
							       (dialog)->
							       vbox));
		while (child) {
			n++;
			child =
			    ewl_container_child_next(EWL_CONTAINER
						     (EWL_DIALOG(dialog)->
						      vbox));
		}
		dialog->separator = ewl_hseparator_new();
		ewl_container_child_insert(EWL_CONTAINER(dialog->vbox),
					   dialog->separator, n);
		ewl_object_fill_policy_set(EWL_OBJECT(dialog->separator),
					   EWL_FLAG_FILL_SHRINK);
		ewl_widget_show(dialog->separator);

	} else if (!has_sep && (dialog->separator != NULL)) {
		ewl_widget_destroy(dialog->separator);
		dialog->separator = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
