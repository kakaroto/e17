#ifndef __EWL_DIALOG_H__
#define __EWL_DIALOG_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup Ewl_Dialog Window: A Window to prompt the user for
 * Displaying Messages, asking
 * questions or warning him
 * @brief Defines the Ewl_Dialog class which extends the Ewl_Window class.
 *
 * @{
 */

/**
 * @themekey /dialog/file
 * @themekey /dialog/group
 */

typedef enum _Ewl_Response_Type Ewl_Response_Type;

/**
 * @enum Ewl_Response_Type
 */
enum _Ewl_Response_Type
{
  EWL_RESPONSE_OPEN   = -5,
  EWL_RESPONSE_SAVE   = -6,
  EWL_RESPONSE_OK     = -10,
  EWL_RESPONSE_CANCEL = -11,
  EWL_RESPONSE_APPLY  = -12
};

/**
 * The dialog structure is a window with two area: a box (vbox) to hold
 * messages and another box (action_area to put buttons
 * (usually). These boxes can be separated by a line.
 */
typedef struct _Ewl_Dialog Ewl_Dialog;

/**
 * @def EWL_DIALOG(dialog)
 * Typecasts a pointer to an Ewl_Dialog pointer.
 */
#define EWL_DIALOG(dialog) ((Ewl_Dialog *) dialog)

/**
 * @struct _Ewl_Dialog
 * Extends the Ewl_Window class. Add two boxes to hold massages (vbox)
 * and buttons (action_area). The action_area could be on top, bottom,
 * right or left of the window. The boxes could be separated by a line.
 */
struct _Ewl_Dialog
{
  /* public */
  Ewl_Window window; /* Inherit from a window */
  
  Ewl_Widget *vbox;        /* the box where messages are displayed */
  Ewl_Widget *action_area; /* The box where the buttons are added */
  
  /* private */
  Ewl_Widget *separator;   /* The separator between vbox and action_area */

  Ewl_Position position;  /* position of the action_area */
};
  
Ewl_Widget *ewl_dialog_new (Ewl_Position pos);
int         ewl_dialog_init (Ewl_Dialog *dialog, Ewl_Position pos);

void        ewl_dialog_add_widget      (Ewl_Dialog *dialog, Ewl_Widget *w);
Ewl_Widget *ewl_dialog_add_button      (Ewl_Dialog *dialog, 
					char       *button_text,
					int         response_id);
Ewl_Widget *ewl_dialog_add_button_left (Ewl_Dialog *dialog, 
					char       *button_text,
					int         response_id);
Ewl_Widget *ewl_dialog_set_button      (char       *button_text,
					int         response_id);

unsigned int ewl_dialog_get_has_separator (Ewl_Dialog *dialog);
void         ewl_dialog_set_has_separator (Ewl_Dialog *dialog,
					   unsigned int has_sep);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __EWL_DIALOG_H__ */
