/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_DIALOG_H
#define EWL_DIALOG_H

/**
 * @addtogroup Ewl_Dialog Ewl_Dialog: A dialog window
 * @brief Defines the Ewl_Dialog class which extends the Ewl_Window class.
 *
 * @remarks Inherits from Ewl_Window.
 * @if HAVE_IMAGES
 * @image html Ewl_Dialog_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /dialog/file
 * @themekey /dialog/group
 */

/**
 * @def EWL_DIALOG_TYPE
 * The type name for the Ewl_Dialog widget
 */
#define EWL_DIALOG_TYPE "dialog"

/**
 * @def EWL_DIALOG_IS(w)
 * Returns TRUE if the widget is an Ewl_Dialog, FALSE otherwise
 */
#define EWL_DIALOG_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_DIALOG_TYPE))

/**
 * The dialog structure is a window with two areas: a box (vbox) to hold
 * messages and another box (action_area) to put buttons
 * (usually). These boxes can be separated by a line.
 */
typedef struct Ewl_Dialog Ewl_Dialog;

/**
 * @def EWL_DIALOG(dialog)
 * Typecasts a pointer to an Ewl_Dialog pointer.
 */
#define EWL_DIALOG(dialog) ((Ewl_Dialog *) dialog)

/**
 * @brief Inherits from Ewl_Window and extends to provide a dialog widget.
 */
struct Ewl_Dialog
{
        Ewl_Window window;        /**< Inherit from a window */

        Ewl_Widget *box;        /**< The box for window layout */
        Ewl_Widget *vbox;        /**< The box where messages are displayed */
        Ewl_Widget *action_area; /**< The cell holding the action box */
        Ewl_Widget *action_box; /**< The box where the buttons are added */

        Ewl_Widget *separator;   /**< The separator between vbox and action_area */

        Ewl_Position position;        /**< Position of the action_area */
        Ewl_Position active_area; /**< Which section of the dialog is active */
};

Ewl_Widget      *ewl_dialog_new (void);
int              ewl_dialog_init (Ewl_Dialog *dialog);

void             ewl_dialog_action_position_set(Ewl_Dialog *d, Ewl_Position pos);
Ewl_Position     ewl_dialog_action_position_get(Ewl_Dialog *dialog);
void             ewl_dialog_action_fill_policy_set(Ewl_Dialog *d,
                                                unsigned int pol);
unsigned int     ewl_dialog_action_fill_policy_get(Ewl_Dialog *dialog);
void             ewl_dialog_action_alignment_set(Ewl_Dialog *d, unsigned int a);
unsigned int     ewl_dialog_action_alignment_get(Ewl_Dialog *dialog);

void             ewl_dialog_active_area_set(Ewl_Dialog *d, Ewl_Position pos);
Ewl_Position     ewl_dialog_active_area_get(Ewl_Dialog *d);

unsigned int     ewl_dialog_has_separator_get(Ewl_Dialog *dialog);
void             ewl_dialog_has_separator_set(Ewl_Dialog *dialog,
                                                unsigned int has_sep);

/**
 * @}
 */

#endif
