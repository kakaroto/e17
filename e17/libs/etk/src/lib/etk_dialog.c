/** @file etk_dialog.c */
#include "etk_dialog.h"
#include <stdlib.h>
#include <string.h>
#include "etk_theme.h"
#include "etk_vbox.h"
#include "etk_hbox.h"
#include "etk_separator.h"
#include "etk_button.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Dialog
 * @{
 */

enum _Etk_Dialog_Signal_Id
{
   ETK_DIALOG_CLOSE_SIGNAL,
   ETK_DIALOG_RESPONSE_SIGNAL,
   ETK_DIALOG_NUM_SIGNALS
};

enum _Etk_Dialog_Property_Id
{
   ETK_DIALOG_HAS_SEPARATOR_PROPERTY
};

static void _etk_dialog_constructor(Etk_Dialog *dialog);
static void _etk_dialog_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_dialog_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_dialog_button_clicked_cb(Etk_Object *object, void *data);
  
static Etk_Signal *_etk_dialog_signals[ETK_DIALOG_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Dialog
 * @return Returns the type on an Etk_Dialog
 */
Etk_Type *etk_dialog_type_get()
{
   static Etk_Type *dialog_type = NULL;

   if (!dialog_type)
   {
      dialog_type = etk_type_new("Etk_Dialog", ETK_WINDOW_TYPE, sizeof(Etk_Dialog), ETK_CONSTRUCTOR(_etk_dialog_constructor), NULL);

      _etk_dialog_signals[ETK_DIALOG_CLOSE_SIGNAL] = etk_signal_new("close", dialog_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_dialog_signals[ETK_DIALOG_RESPONSE_SIGNAL] = etk_signal_new("response", dialog_type, -1, etk_marshaller_VOID__INT, NULL, NULL);

      etk_type_property_add(dialog_type, "has_separator", ETK_DIALOG_HAS_SEPARATOR_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));

      dialog_type->property_set = _etk_dialog_property_set;
      dialog_type->property_get = _etk_dialog_property_get;
   }

   return dialog_type;
}

/**
 * @brief Creates a new dialog
 * @return Returns the new dialog widget
 */
Etk_Widget *etk_dialog_new()
{
   return etk_widget_new(ETK_DIALOG_TYPE, "theme_group", "dialog", NULL);
}

/**
 * @brief Packs a widget in the main area of the dialog (above the buttons and the separator)
 * @param dialog a dialog
 * @param widget the widget to pack
 * @param expand ETK_TRUE if the widget should ask for as much room as possible
 * @param fill ETK_TRUE if the widget should fill all the size allocated for it
 * @param pack_at_end if ETK_TRUE, the widget will be packed at the end of the vbox of the main area.
 * @see etk_box_pack_start
 * @see etk_box_pack_end
 */
void etk_dialog_pack_in_main_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
{
   if (!dialog || !widget)
      return;
   
   if (pack_at_end)
      etk_box_pack_end(ETK_BOX(dialog->main_area_vbox), widget, expand, fill, padding);
   else
      etk_box_pack_start(ETK_BOX(dialog->main_area_vbox), widget, expand, fill, padding);
}

/**
 * @brief Packs a widget in the action area of the dialog (at the bottom of the dialog)
 * @param dialog a dialog
 * @param widget the widget to pack
 * @param expand ETK_TRUE if the widget should ask for as much room as possible
 * @param fill ETK_TRUE if the widget should fill all the size allocated for it
 * @param pack_at_end if ETK_TRUE, the widget will be packed at the end of the hbox of the action area.
 * @see etk_box_pack_start
 * @see etk_box_pack_end
 */
void etk_dialog_pack_in_action_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
{
   if (!dialog || !widget)
      return;
   
   if (pack_at_end)
      etk_box_pack_end(ETK_BOX(dialog->action_area_hbox), widget, expand, fill, padding);
   else
      etk_box_pack_start(ETK_BOX(dialog->action_area_hbox), widget, expand, fill, padding);
}

/**
 * @brief Sets whether the dialog has a separator that separate the action area and the upper area
 * @param dialog a dialog
 * @param has_separator ETK_TRUE to show the separator
 */
void etk_dialog_has_separator_set(Etk_Dialog *dialog, Etk_Bool has_separator)
{
   if (!dialog || dialog->has_separator == has_separator)
      return;

   if (has_separator)
      etk_widget_show(dialog->separator);
   else
      etk_widget_hide(dialog->separator);
   dialog->has_separator = has_separator;
   etk_object_notify(ETK_OBJECT(dialog), "has_separator");
}

/**
 * @brief Gets whether the dialog has a separator that separate the action area and the upper area
 * @param dialog a dialog
 * @return Returns whether the dialog has a separator
 */
Etk_Bool etk_dialog_has_separator_get(Etk_Dialog *dialog)
{
   if (!dialog)
      return ETK_FALSE;
   return dialog->has_separator;
}

/**
 * @brief Adds a button to the dialog's action area
 * @param label the button's label
 * @param response_id the button's response id (Etk_Dialog_Response_ID)
 * @return Returns the newly added button.
 */
Etk_Widget *etk_dialog_button_add(Etk_Dialog *dialog, const char *label, int response_id)
{
   Etk_Widget *button;

   if (!dialog)
      return NULL;
   
   button = etk_button_new_with_label(label);
   etk_dialog_pack_button_in_action_area(dialog, ETK_BUTTON(button), response_id, ETK_FALSE, ETK_FALSE, 0, ETK_TRUE);
   return button;
}

/**
 * @brief Adds a button created from a stock id to the dialog's action area
 * @param stock_id the button's stock id
 * @param response_id the button's response id (Etk_Dialog_Response_ID)
 * @return Returns the newly added button.
 */
Etk_Widget *etk_dialog_button_add_from_stock(Etk_Dialog *dialog, int stock_id, int response_id)
{
   Etk_Widget *button;

   if (!dialog)
      return NULL;
   
   button = etk_button_new_from_stock(stock_id);
   etk_dialog_pack_button_in_action_area(dialog, ETK_BUTTON(button), response_id, ETK_FALSE, ETK_FALSE, 0, ETK_TRUE);
   return button;
}

/**
 * @brief Pack a pre-created button into the dialog's action area
 * @param dialog the dialog we want to pack into
 * @param button the button we want to pacl
 * @param response_id the response id of the button (Etk_Dialog_Response_ID)
 * @param expand expand the button
 * @param fill make the button fill the available space
 * @param padding how much padding the button will have in pixels
 * @param pack_at_end if true, the button will be packed at the end of button box
 */
void etk_dialog_pack_button_in_action_area(Etk_Dialog *dialog, Etk_Button *button, int response_id, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
{
   int *id;   

   if (!dialog)
      return;
   
   etk_widget_visibility_locked_set(ETK_WIDGET(button), ETK_TRUE);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_dialog_button_clicked_cb), dialog);
   
   id = malloc(sizeof(int));
   *id = response_id;
   etk_object_data_set_full(ETK_OBJECT(button), "_Etk_Dialog::response_id", id, free);
   
   if (pack_at_end)
      etk_box_pack_end(ETK_BOX(dialog->action_area_hbox), ETK_WIDGET(button), fill, expand, padding);
   else
      etk_box_pack_start(ETK_BOX(dialog->action_area_hbox), ETK_WIDGET(button), fill, expand, padding);
   etk_widget_show(ETK_WIDGET(button));
}


/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_dialog_constructor(Etk_Dialog *dialog)
{
   if (!dialog)
      return;

   dialog->dialog_vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(dialog), dialog->dialog_vbox);
   etk_widget_show(dialog->dialog_vbox);
   
   dialog->main_area_vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_widget_visibility_locked_set(dialog->main_area_vbox, ETK_TRUE);
   etk_box_pack_start(ETK_BOX(dialog->dialog_vbox), dialog->main_area_vbox, ETK_TRUE, ETK_TRUE, 0);
   etk_widget_show(dialog->main_area_vbox);
   
   dialog->separator = etk_hseparator_new();
   etk_widget_visibility_locked_set(dialog->separator, ETK_TRUE);
   etk_box_pack_start(ETK_BOX(dialog->dialog_vbox), dialog->separator, ETK_FALSE, ETK_TRUE, 6);
   etk_widget_show(dialog->separator);

   dialog->action_area_hbox = etk_hbox_new(ETK_FALSE, 6);
   etk_widget_visibility_locked_set(dialog->action_area_hbox, ETK_TRUE);
   etk_box_pack_end(ETK_BOX(dialog->dialog_vbox), dialog->action_area_hbox, ETK_FALSE, ETK_TRUE, 0);
   etk_widget_show(dialog->action_area_hbox);

   dialog->has_separator = ETK_TRUE;
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_dialog_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Dialog *dialog;

   if (!(dialog = ETK_DIALOG(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_DIALOG_HAS_SEPARATOR_PROPERTY:
         etk_dialog_has_separator_set(dialog, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_dialog_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Dialog *dialog;

   if (!(dialog = ETK_DIALOG(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_DIALOG_HAS_SEPARATOR_PROPERTY:
         etk_property_value_bool_set(value, etk_dialog_has_separator_get(dialog));
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

/* Called when a button of the action area of the dialog is clicked */
static void _etk_dialog_button_clicked_cb(Etk_Object *object, void *data)
{
   int *response_id;
   
   if (!(response_id = (int *)etk_object_data_get(object, "_Etk_Dialog::response_id")))
      return;

   etk_signal_emit(_etk_dialog_signals[ETK_DIALOG_RESPONSE_SIGNAL], ETK_OBJECT(data), NULL, *response_id);
}

/** @} */
