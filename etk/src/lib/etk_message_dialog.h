/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_message_dialog.h */
#ifndef _ETK_MESSAGE_DIALOG_H_
#define _ETK_MESSAGE_DIALOG_H_

#include "etk_dialog.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - Use printf-like format for the message
 * - Support primary and secondary texts?
 */

/**
 * @defgroup Etk_Message_Dialog Etk_Message_Dialog
 * @brief Etk_Message_Dialog is a convenient way to ask or to warn the user.
 * @{
 */

/** Gets the type of a message dialog */
#define ETK_MESSAGE_DIALOG_TYPE       (etk_message_dialog_type_get())
/** Casts the object to an Etk_Message_Dialog */
#define ETK_MESSAGE_DIALOG(obj)       (ETK_OBJECT_CAST((obj), ETK_MESSAGE_DIALOG_TYPE, Etk_Message_Dialog))
/** Checks if the object is an Etk_Message_Dialog */
#define ETK_IS_MESSAGE_DIALOG(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MESSAGE_DIALOG_TYPE))

#define ETK_MESSAGE_DIALOG_MAX_BUTTONS 2


/** @brief The type of the message of the dialog */
typedef enum
{
   ETK_MESSAGE_DIALOG_INFO,         /**< Informational message*/
   ETK_MESSAGE_DIALOG_WARNING,      /**< Warning message */
   ETK_MESSAGE_DIALOG_QUESTION,     /**< Question message */
   ETK_MESSAGE_DIALOG_ERROR         /**< Error message */
} Etk_Message_Dialog_Type;

/** @brief Some common sets of buttons for the message dialog. To use your own buttons, you can use
 * ETK_MESSAGE_DIALOG_NONE and add your buttons with etk_dialog_button_add() */
typedef enum
{
   ETK_MESSAGE_DIALOG_NONE,
   ETK_MESSAGE_DIALOG_OK,
   ETK_MESSAGE_DIALOG_CLOSE,
   ETK_MESSAGE_DIALOG_CANCEL,
   ETK_MESSAGE_DIALOG_YES_NO,
   ETK_MESSAGE_DIALOG_OK_CANCEL
} Etk_Message_Dialog_Buttons;


/**
 * @brief @widget The structure of a message dialog
 * @structinfo
 */
struct Etk_Message_Dialog
{
   /* private: */
   /* Inherit from Etk_Dialog */
   Etk_Dialog dialog;

   Etk_Message_Dialog_Type message_type;
   Etk_Message_Dialog_Buttons buttons_type;

   Etk_Widget *main_area_hbox;
   Etk_Widget *image;
   Etk_Widget *label;
   Etk_Widget *buttons[ETK_MESSAGE_DIALOG_MAX_BUTTONS];
};


Etk_Type   *etk_message_dialog_type_get(void);
Etk_Widget *etk_message_dialog_new(Etk_Message_Dialog_Type message_type, Etk_Message_Dialog_Buttons buttons, const char *text);

void        etk_message_dialog_text_set(Etk_Message_Dialog *dialog, const char *text);
const char *etk_message_dialog_text_get(Etk_Message_Dialog *dialog);

void        etk_message_dialog_message_type_set(Etk_Message_Dialog *dialog, Etk_Message_Dialog_Type type);
Etk_Message_Dialog_Type etk_message_dialog_message_type_get(Etk_Message_Dialog *dialog);

void        etk_message_dialog_buttons_set(Etk_Message_Dialog *dialog, Etk_Message_Dialog_Buttons buttons);
Etk_Message_Dialog_Buttons etk_message_dialog_buttons_get(Etk_Message_Dialog *dialog);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
