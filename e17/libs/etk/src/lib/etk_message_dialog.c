/** @file etk_message_dialog.c */
#include "etk_message_dialog.h"
#include <stdlib.h>
#include <string.h>
#include "etk_hbox.h"
#include "etk_separator.h"
#include "etk_button.h"
#include "etk_label.h"
#include "etk_signal.h"
#include "etk_image.h"
#include "etk_signal_callback.h"
#include "etk_stock.h"

/**
 * @addtogroup Etk_Message_Dialog
* @{
 */

enum _Etk_Message_Dialog_Property_Id
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
 * @brief Gets the type of an Etk_Message_Dialog
 * @return Returns the type on an Etk_Message_Dialog
 */
Etk_Type *etk_message_dialog_type_get()
{
   static Etk_Type *message_dialog_type = NULL;

   if (!message_dialog_type)
   {
      message_dialog_type = etk_type_new("Etk_Message_Dialog", ETK_DIALOG_TYPE, sizeof(Etk_Message_Dialog), ETK_CONSTRUCTOR(_etk_message_dialog_constructor), NULL);

      etk_type_property_add(message_dialog_type, "dialog_type", ETK_MESSAGE_DIALOG_TYPE_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(-1));
      etk_type_property_add(message_dialog_type, "buttons", ETK_MESSAGE_DIALOG_BUTTONS_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(-1));
      etk_type_property_add(message_dialog_type, "text", ETK_MESSAGE_DIALOG_TEXT_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));

      message_dialog_type->property_set = _etk_message_dialog_property_set;
      message_dialog_type->property_get = _etk_message_dialog_property_get;
   }

   return message_dialog_type;
}

/**
 * @brief Creates a new dialog
 * @return Returns the new dialog widget
 */
Etk_Widget *etk_message_dialog_new(int dialog_type, int buttons, const char *text)
{
   return etk_widget_new(ETK_MESSAGE_DIALOG_TYPE, "theme_group", "dialog", "dialog_type", dialog_type, "buttons", buttons, "text", text, NULL);
}

/**
 * @brief Sets the message dialog's type
 * @param dialog a dialog
 * @param dialog_type the type to set
 */
void etk_message_dialog_icon_set(Etk_Message_Dialog *dialog, int dialog_type)
{
   if (!dialog)
      return;
   
   switch(dialog_type)
   {
      case ETK_MESSAGE_DIALOG_INFO:      
      etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_INFORMATION, ETK_STOCK_BIG);
      break;
      
      case ETK_MESSAGE_DIALOG_WARNING:
      etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_WARNING, ETK_STOCK_BIG);
      break;
      
      case ETK_MESSAGE_DIALOG_QUESTION:      
      etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_QUESTION, ETK_STOCK_BIG);
      break;
      
      case ETK_MESSAGE_DIALOG_ERROR:      
      etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_ERROR, ETK_STOCK_BIG);
      break;
      
      default:
      etk_image_set_from_stock(ETK_IMAGE(dialog->image), ETK_STOCK_DIALOG_INFORMATION, ETK_STOCK_BIG);
      break; 
   }
}

/* @brief Gets the message dialog's type
 * @param dialog a dialog
 * @return Returns the type of the dialog
 */
int etk_message_dialog_icon_get(Etk_Message_Dialog *dialog)
{
   if(!dialog)
     return 0;
   
   return dialog->dialog_type;
}

/**
 * @brief Sets the message dialog's buttons
 * @param dialog a dialog
 * @param buttons the type of the buttons
 */
void etk_message_dialog_buttons_set(Etk_Message_Dialog *dialog, int buttons)
{
   int i;
   
   if (!dialog)
      return;

   if(dialog->buttons_type == buttons)
      return;
   
   for(i = 0; i < ETK_MESSAGE_DIALOG_MAX_BUTTONS; i++)
      if(dialog->buttons[i])
      {
	 etk_object_destroy(ETK_OBJECT(dialog->buttons[i]));
	 dialog->buttons[i] = NULL;
      }
   
   switch(buttons)
   {
      case ETK_MESSAGE_DIALOG_OK:
      etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);
      break;
      
      case ETK_MESSAGE_DIALOG_CLOSE:
      etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CLOSE, ETK_RESPONSE_CLOSE);
      break;
      
      case ETK_MESSAGE_DIALOG_CANCEL:
      etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL);
      break;
      
      case ETK_MESSAGE_DIALOG_YES_NO:
      etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_YES, ETK_RESPONSE_YES);
      etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_NO, ETK_RESPONSE_NO);
      break;
      
      case ETK_MESSAGE_DIALOG_OK_CANCEL:
      etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_YES, ETK_RESPONSE_OK);
      etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_NO, ETK_RESPONSE_CANCEL);
      break;      
    
      case ETK_MESSAGE_DIALOG_NONE:
      default:
      break; 
   }
}

/* @brief Gets the message dialogs buttons
 * @param dialog a dialog
 * @return Returns the type of the buttons used
 */
int etk_message_dialog_buttons_get(Etk_Message_Dialog *dialog)
{
   if(!dialog)
     return 0;
   
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
 * @return The text for the message dialog.
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

/* Initializes the members */
static void _etk_message_dialog_constructor(Etk_Message_Dialog *dialog)
{
   int i;
   
   if (!dialog)
      return;

   dialog->main_area_hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_widget_visibility_locked_set(dialog->main_area_hbox, ETK_TRUE);
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), dialog->main_area_hbox, ETK_FALSE, ETK_FALSE, 4, ETK_FALSE);
   etk_widget_show(dialog->main_area_hbox);
   
   dialog->image = etk_image_new_from_stock(ETK_STOCK_DIALOG_INFORMATION, ETK_STOCK_BIG);
   etk_widget_visibility_locked_set(dialog->image, ETK_TRUE);
   etk_box_pack_start(ETK_BOX(dialog->main_area_hbox), dialog->image, ETK_FALSE, ETK_FALSE, 3);
   etk_widget_show(dialog->image);

   dialog->label = etk_label_new(" ");
   etk_widget_visibility_locked_set(dialog->label, ETK_TRUE);
   etk_box_pack_start(ETK_BOX(dialog->main_area_hbox), dialog->label, ETK_FALSE, ETK_FALSE, 2);
   etk_widget_show(dialog->label);

   dialog->buttons_type = ETK_MESSAGE_DIALOG_NONE;
   dialog->dialog_type = ETK_MESSAGE_DIALOG_INFO;
   
   for(i = 0; i < ETK_MESSAGE_DIALOG_MAX_BUTTONS; i++)
     dialog->buttons[i] = NULL;
       
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
         etk_message_dialog_icon_set(dialog, etk_property_value_int_get(value));
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
         etk_property_value_int_set(value, etk_message_dialog_icon_get(dialog));
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

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/** @} */
