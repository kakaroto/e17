/** @file etk_message_dialog.h */
#ifndef _ETK_MESSAGE_DIALOG_H_
#define _ETK_MESSAGE_DIALOG_H_

#include "etk_dialog.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Message_Dialog Etk_Message_Dialog
 * @{
 */

/** @brief Gets the type of a message dialog */
#define ETK_MESSAGE_DIALOG_TYPE       (etk_message_dialog_type_get())
/** @brief Casts the object to an Etk_Message_Dialog */
#define ETK_MESSAGE_DIALOG(obj)       (ETK_OBJECT_CAST((obj), ETK_MESSAGE_DIALOG_TYPE, Etk_Message_Dialog))
/** @brief Checks if the object is an Etk_Message_Dialog */
#define ETK_IS_MESSAGE_DIALOG(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MESSAGE_DIALOG_TYPE))

#define ETK_MESSAGE_DIALOG_MAX_BUTTONS 2

/**
 * @enum Etk_Message_Dialog_Type
 * @brief Message types for the message dialog
 */
typedef enum _Etk_Message_Dialog_Type
{
   ETK_MESSAGE_DIALOG_INFO,
   ETK_MESSAGE_DIALOG_WARNING,
   ETK_MESSAGE_DIALOG_QUESTION,
   ETK_MESSAGE_DIALOG_ERROR
} Etk_Message_Dialog_Type;

/**
 * @enum Etk_Message_Dialog_Buttons
 * @brief What buttons the message dialog will have
 */
typedef enum _Etk_Message_Dialog_Buttons
{
   ETK_MESSAGE_DIALOG_NONE = -1,
   ETK_MESSAGE_DIALOG_OK = -2,
   ETK_MESSAGE_DIALOG_CLOSE = -3,
   ETK_MESSAGE_DIALOG_CANCEL = -4,
   ETK_MESSAGE_DIALOG_YES_NO = -5,
   ETK_MESSAGE_DIALOG_OK_CANCEL = -6     
} Etk_Message_Dialog_Buttons;

/**
 * @struct Etk_Message_Dialog
 * @brief An Etk_Message_Dialog is a convenient way to ask or to warn the user
 */
struct _Etk_Message_Dialog
{
   /* private: */
   /* Inherit from Etk_Dialog */
   Etk_Dialog dialog;
   
   Etk_Message_Dialog_Type dialog_type;
   Etk_Message_Dialog_Buttons buttons_type;
   
   Etk_Widget *main_area_hbox;
   Etk_Widget *image;
   Etk_Widget *label;
   Etk_Widget *buttons[ETK_MESSAGE_DIALOG_MAX_BUTTONS];
};

Etk_Type *etk_message_dialog_type_get();
Etk_Widget *etk_message_dialog_new(Etk_Message_Dialog_Type dialog_type, Etk_Message_Dialog_Buttons buttons, const char *text);

void etk_message_dialog_text_set(Etk_Message_Dialog *dialog, const char *text);
const char *etk_message_dialog_text_get(Etk_Message_Dialog *dialog);

void etk_message_dialog_icon_set(Etk_Message_Dialog *dialog, Etk_Message_Dialog_Type dialog_type);
Etk_Message_Dialog_Type etk_message_dialog_icon_get(Etk_Message_Dialog *dialog);

void etk_message_dialog_buttons_set(Etk_Message_Dialog *dialog, Etk_Message_Dialog_Buttons buttons);
Etk_Message_Dialog_Buttons etk_message_dialog_buttons_get(Etk_Message_Dialog *dialog);

/** @} */

#endif
