#ifndef __EWL_DIALOG_H__
#define __EWL_DIALOG_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
  EWL_RESPONSE_OPEN   = -5,
  EWL_RESPONSE_SAVE   = -6,
  EWL_RESPONSE_OK     = -10,
  EWL_RESPONSE_CANCEL = -11,
  EWL_RESPONSE_APPLY  = -12
}Ewl_Response_Type;

typedef struct Ewl_Dialog Ewl_Dialog;
  
#define EWL_DIALOG(dialog) ((Ewl_Dialog *) dialog)

struct Ewl_Dialog
{
  /* public */
  Ewl_Window window; /* Inherit from a window */
  
  Ewl_Widget *vbox;
  Ewl_Widget *action_area;
  
  /* private */
  Ewl_Widget *separator;

  Ewl_Position position;  /* position of the action_area */
};
  
Ewl_Widget *ewl_dialog_new (Ewl_Position pos);
int         ewl_dialog_init (Ewl_Dialog *dialog, Ewl_Position pos);

void        ewl_dialog_add_widget      (Ewl_Dialog *dialog, Ewl_Widget *w);
Ewl_Widget *ewl_dialog_add_button      (Ewl_Dialog *dialog, 
					char *button_text,
					int         response_id);
Ewl_Widget *ewl_dialog_add_button_left (Ewl_Dialog *dialog, 
					char *button_text,
					int         response_id);
Ewl_Widget *ewl_dialog_set_button      (char *button_text,
					int         response_id);

unsigned int ewl_dialog_get_has_separator (Ewl_Dialog *dialog);
void         ewl_dialog_set_has_separator (Ewl_Dialog *dialog,
					   unsigned int has_sep);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __EWL_DIALOG_H__ */
