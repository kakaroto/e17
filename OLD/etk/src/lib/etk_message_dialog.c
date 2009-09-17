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

/** @file etk_message_dialog.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_message_dialog.h"

#include <stdlib.h>

#include "etk_box.h"
#include "etk_image.h"
#include "etk_label.h"
#include "etk_stock.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Message_Dialog
 * @{
 */

enum Etk_Message_Dialog_Property_Id
{
   ETK_MESSAGE_DIALOG_TYPE_PROPERTY,
   ETK_MESSAGE_DIALOG_BUTTONS_PROPERTY,
   ETK_MESSAGE_DIALOG_TEXT_PROPERTY
};

static void _etk_message_dialog_constructor(Etk_Message_Dialog *dialog);
static void _etk_message_dialog_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_message_dialog_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Message_Dialog
 * @return Returns the type of an Etk_Message_Dialog
 */
Etk_Type *etk_message_dialog_type_get(void)
{
   static Etk_Type *message_dialog_type = NULL;

   if (!message_dialog_type)
   {
      message_dialog_type = etk_type_new("Etk_Message_Dialog", ETK_DIALOG_TYPE,
         sizeof(Etk_Message_Dialog),
         ETK_CONSTRUCTOR(_etk_message_dialog_constructor), NULL, NULL);

      etk_type_property_add(message_dialog_type, "message-type", ETK_MESSAGE_DIALOG_TYPE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_MESSAGE_DIALOG_INFO));
      etk_type_property_add(message_dialog_type, "buttons", ETK_MESSAGE_DIALOG_BUTTONS_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_MESSAGE_DIALOG_NONE));
      etk_type_property_add(message_dialog_type, "text", ETK_MESSAGE_DIALOG_TEXT_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));

      message_dialog_type->property_set = _etk_message_dialog_property_set;
      message_dialog_type->property_get = _etk_message_dialog_property_get;
   }

   return message_dialog_type;
}

/**
 * @brief Creates a new message dialog
 * @return Returns the new message dialog widget
 */
Etk_Widget *etk_message_dialog_new(Etk_Message_Dialog_Type message_type, Etk_Message_Dialog_Buttons buttons, const char *text)
{
   return etk_widget_new(ETK_MESSAGE_DIALOG_TYPE, "theme-group", "dialog", "message-type", message_type,
      "buttons", buttons, "text", text, NULL);
}

/**
 * @brief Sets the type of the dialog's message. It will change the title and the icon of the message dialog.
 * @param dialog a meesage dialog
 * @param type the type to set
 */
void etk_message_dialog_message_type_set(Etk_Message_Dialog *dialog, Etk_Message_Dialog_Type type)
{
   if (!dialog || dialog->message_type == type)
      return;

   switch (type)
   {
      case ETK_MESSAGE_DIALOG_WARNING:
         etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_WARNING, ETK_STOCK_BIG);
         etk_window_title_set(ETK_WINDOW(dialog), _("Warning"));
         break;
      case ETK_MESSAGE_DIALOG_QUESTION:
         etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_QUESTION, ETK_STOCK_BIG);
         etk_window_title_set(ETK_WINDOW(dialog), _("Question"));
         break;
      case ETK_MESSAGE_DIALOG_ERROR:
         etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_ERROR, ETK_STOCK_BIG);
         etk_window_title_set(ETK_WINDOW(dialog), _("Error"));
         break;
      case ETK_MESSAGE_DIALOG_INFO:
      default:
         etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_INFORMATION, ETK_STOCK_BIG);
         etk_window_title_set(ETK_WINDOW(dialog), _("Information"));
         break;
   }

   dialog->message_type = type;
   etk_object_notify(ETK_OBJECT(dialog), "message-type");
}

/**
 * @brief Gets the type of the dialog's message
 * @param dialog a dialog
 * @return Returns the type of the dialog
 */
Etk_Message_Dialog_Type etk_message_dialog_message_type_get(Etk_Message_Dialog *dialog)
{
   if (!dialog)
      return ETK_STOCK_DIALOG_INFORMATION;
   return dialog->message_type;
}

/**
 * @brief Sets the message dialog's buttons. To add your own buttons, use ETK_MESSAGE_DIALOG_NONE
 * and add them with etk_dialog_button_add()
 * @param dialog a dialog
 * @param buttons the type of the buttons
 */
void etk_message_dialog_buttons_set(Etk_Message_Dialog *dialog, Etk_Message_Dialog_Buttons buttons)
{
   int i;

   if (!dialog)
      return;
   if (dialog->buttons_type == buttons)
      return;

   for (i = 0; i < ETK_MESSAGE_DIALOG_MAX_BUTTONS; i++)
   {
      if (dialog->buttons[i])
      {
         etk_object_destroy(ETK_OBJECT(dialog->buttons[i]));
         dialog->buttons[i] = NULL;
      }
   }

   switch (buttons)
   {
      case ETK_MESSAGE_DIALOG_OK:
         dialog->buttons[0] = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog),ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);
         break;
      case ETK_MESSAGE_DIALOG_CLOSE:
         dialog->buttons[0] = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CLOSE, ETK_RESPONSE_CLOSE);
         break;
      case ETK_MESSAGE_DIALOG_CANCEL:
         dialog->buttons[0] = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL);
         break;
      case ETK_MESSAGE_DIALOG_YES_NO:
         dialog->buttons[0] = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_YES, ETK_RESPONSE_YES);
         dialog->buttons[1] = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_NO, ETK_RESPONSE_NO);
         break;
      case ETK_MESSAGE_DIALOG_OK_CANCEL:
         dialog->buttons[0] = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_YES, ETK_RESPONSE_OK);
         dialog->buttons[1] = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_NO, ETK_RESPONSE_CANCEL);
         break;
      case ETK_MESSAGE_DIALOG_NONE:
      default:
         break;
   }

   dialog->buttons_type = buttons;
   etk_object_notify(ETK_OBJECT(dialog), "buttons");
}

/**
 * @brief Gets the set of buttons used by the message dialogs
 * @param dialog a dialog
 * @return Returns the set of the buttons used
 */
Etk_Message_Dialog_Buttons etk_message_dialog_buttons_get(Etk_Message_Dialog *dialog)
{
   if (!dialog)
      return ETK_MESSAGE_DIALOG_NONE;
   return dialog->buttons_type;
}

/**
 * @brief Sets the message dialog's text
 * @param dialog a dialog
 * @param text the text to set
 */
void etk_message_dialog_text_set(Etk_Message_Dialog *dialog, const char *text)
{
   if (!dialog || !dialog->label)
      return;
   etk_label_set(ETK_LABEL(dialog->label), text);
}

/**
 * @brief Gets the message dialog's text
 * @param dialog a dialog
 * @return Returns the text of the message dialog
 */
const char *etk_message_dialog_text_get(Etk_Message_Dialog *dialog)
{
   if (!dialog || !dialog->label)
      return NULL;
   return etk_label_get(ETK_LABEL(dialog->label));
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the message dialog */
static void _etk_message_dialog_constructor(Etk_Message_Dialog *dialog)
{
   int i;

   if (!dialog)
      return;

   dialog->main_area_hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), dialog->main_area_hbox, ETK_BOX_START, ETK_BOX_NONE, 4);
   etk_widget_internal_set(dialog->main_area_hbox, ETK_TRUE);
   etk_widget_show(dialog->main_area_hbox);

   dialog->image = etk_image_new_from_stock(ETK_STOCK_DIALOG_INFORMATION, ETK_STOCK_BIG);
   etk_box_append(ETK_BOX(dialog->main_area_hbox), dialog->image, ETK_BOX_START, ETK_BOX_NONE, 3);
   etk_widget_internal_set(dialog->image, ETK_TRUE);
   etk_widget_show(dialog->image);

   dialog->label = etk_label_new(NULL);
   etk_box_append(ETK_BOX(dialog->main_area_hbox), dialog->label, ETK_BOX_START, ETK_BOX_NONE, 2);
   etk_widget_internal_set(dialog->label, ETK_TRUE);
   etk_widget_show(dialog->label);

   dialog->buttons_type = ETK_MESSAGE_DIALOG_NONE;
   dialog->message_type = ETK_MESSAGE_DIALOG_INFO;
   for(i = 0; i < ETK_MESSAGE_DIALOG_MAX_BUTTONS; i++)
      dialog->buttons[i] = NULL;

   etk_window_title_set(ETK_WINDOW(dialog), _("Information"));
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_message_dialog_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Message_Dialog *dialog;

   if (!(dialog = ETK_MESSAGE_DIALOG(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MESSAGE_DIALOG_TYPE_PROPERTY:
         etk_message_dialog_message_type_set(dialog, etk_property_value_int_get(value));
         break;
      case ETK_MESSAGE_DIALOG_BUTTONS_PROPERTY:
         etk_message_dialog_buttons_set(dialog, etk_property_value_int_get(value));
         break;
      case ETK_MESSAGE_DIALOG_TEXT_PROPERTY:
         etk_message_dialog_text_set(dialog, etk_property_value_string_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_message_dialog_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Message_Dialog *dialog;

   if (!(dialog = ETK_MESSAGE_DIALOG(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MESSAGE_DIALOG_TYPE_PROPERTY:
         etk_property_value_int_set(value, dialog->message_type);
         break;
      case ETK_MESSAGE_DIALOG_BUTTONS_PROPERTY:
         etk_property_value_int_set(value, etk_message_dialog_buttons_get(dialog));
         break;
      case ETK_MESSAGE_DIALOG_TEXT_PROPERTY:
         etk_property_value_string_set(value, etk_message_dialog_text_get(dialog));
         break;
      default:
         break;
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Message_Dialog
 *
 * @image html widgets/message_dialog.png
 * A message dialog can have several type (see Etk_Message_Dialog_Type), each type corresponding
 * to a title and to an icon. @n
 * Several common predefined sets of buttons can be used by the message dialog (see Etk_Message_Dialog_Buttons),
 * but you can also use ETK_MESSAGE_DIALOG_NONE and add your own buttons with etk_dialog_button_add().
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Toplevel
 *           - Etk_Window
 *             - Etk_Dialog
 *               - Etk_Message_Dialog
 *
 * \par Properties:
 * @prop_name "message-type": The type of the dialog's message which defined the title and the icon
 * @prop_type Integer (Etk_Message_Dialog_Type)
 * @prop_rw
 * @prop_val ETK_MESSAGE_DIALOG_INFO
 * \par
 * @prop_name "buttons": The set of buttons used by the message dialog
 * @prop_type Integer (Etk_Message_Dialog_Buttons)
 * @prop_rw
 * @prop_val ETK_MESSAGE_DIALOG_NONE
 * \par
 * @prop_name "text": The text of the message dialog
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 */
