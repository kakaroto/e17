#ifndef EWL_DIALOG_H
#define EWL_DIALOG_H

/**
 * @defgroup Ewl_Dialog Ewl_Dialog: A dialog window
 * @brief Defines the Ewl_Dialog class which extends the Ewl_Window class.
 *
 * @{
 */

/**
 * @themekey /dialog/file
 * @themekey /dialog/group
 */

#define EWL_DIALOG_TYPE "dialog"

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
 * Extends the Ewl_Window class. Add two boxes to hold massages (vbox)
 * and buttons (action_area). The action_area could be on top, bottom,
 * right or left of the window. The boxes could be separated by a line.
 */
struct Ewl_Dialog
{
	Ewl_Window   window; /* Inherit from a window */

        Ewl_Widget  *box;         /* The box for window layout */
	Ewl_Widget  *vbox;        /* The box where messages are displayed */
	Ewl_Widget  *action_area; /* The box where the buttons are added */

	Ewl_Widget  *separator;   /* The separator between vbox and action_area */

	Ewl_Position position;    /* Position of the action_area */
	Ewl_Position active_area; /* Which section of the dialog is active */
};
  
Ewl_Widget  *ewl_dialog_new (void);
int          ewl_dialog_init (Ewl_Dialog *dialog);

void         ewl_dialog_action_position_set(Ewl_Dialog *d, Ewl_Position pos);
Ewl_Position ewl_dialog_action_position_get(Ewl_Dialog *dialog);

void         ewl_dialog_active_area_set(Ewl_Dialog *d, Ewl_Position pos);
Ewl_Position ewl_dialog_active_area_get(Ewl_Dialog *d);

unsigned int ewl_dialog_has_separator_get (Ewl_Dialog *dialog);
void         ewl_dialog_has_separator_set (Ewl_Dialog *dialog,
					   unsigned int has_sep);

/**
 * @}
 */

#endif
