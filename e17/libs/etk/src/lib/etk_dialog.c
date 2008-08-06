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

/** @file etk_dialog.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_dialog.h"

#include <stdlib.h>
#include <string.h>

#include "etk_alignment.h"
#include "etk_button.h"
#include "etk_separator.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_theme.h"

/**
 * @addtogroup Etk_Dialog
 * @{
 */

int ETK_DIALOG_RESPONSE_SIGNAL;

enum Etk_Dialog_Property_Id
{
   ETK_DIALOG_HAS_SEPARATOR_PROPERTY,
   ETK_DIALOG_ACTION_AREA_HOMOGENEOUS_PROPERTY,
   ETK_DIALOG_ACTION_AREA_ALIGN_PROPERTY
};

static void _etk_dialog_constructor(Etk_Dialog *dialog);
static void _etk_dialog_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_dialog_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_dialog_button_clicked_cb(Etk_Object *object, void *data);


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Dialog
 * @return Returns the type of an Etk_Dialog
 */
Etk_Type *etk_dialog_type_get(void)
{
   static Etk_Type *dialog_type = NULL;

   if (!dialog_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_DIALOG_RESPONSE_SIGNAL,
            "response", etk_marshaller_INT),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      dialog_type = etk_type_new("Etk_Dialog", ETK_WINDOW_TYPE,
         sizeof(Etk_Dialog), ETK_CONSTRUCTOR(_etk_dialog_constructor),
         NULL, signals);

      etk_type_property_add(dialog_type, "has-separator", ETK_DIALOG_HAS_SEPARATOR_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(dialog_type, "action-area-homogeneous", ETK_DIALOG_ACTION_AREA_HOMOGENEOUS_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(dialog_type, "action-area-align", ETK_DIALOG_ACTION_AREA_ALIGN_PROPERTY,
            ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_float(0.5));

      dialog_type->property_set = _etk_dialog_property_set;
      dialog_type->property_get = _etk_dialog_property_get;
   }

   return dialog_type;
}

/**
 * @brief Creates a new dialog
 * @return Returns the new dialog widget
 */
Etk_Widget *etk_dialog_new(void)
{
   return etk_widget_new(ETK_DIALOG_TYPE, "theme-group", "dialog", NULL);
}

/**
 * @brief Packs a widget into the dialog's main-area (above the buttons and the separator).
 * The widget will be appended in the main-area's vbox (see etk_box_append())
 * @param dialog a dialog
 * @param widget the widget to pack
 * @param group the box-group where to pack the child (ETK_BOX_START or ETK_BOX_END)
 * @param fill_policy the fill-policy of the child, it indicates how it should fill its cell
 * @param padding the amount of free space on the two sides of the child, in pixels
 * @see etk_box_append()
 */
void etk_dialog_pack_in_main_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding)
{
   if (!dialog || !widget)
      return;
   etk_box_append(ETK_BOX(dialog->main_area_vbox), widget, group, fill_policy, padding);
}

/**
 * @brief Packs a widget into the dialog's action-area (at the bottom of the dialog).
 * The widget will be appended in the action-area's hbox (see etk_box_append())
 * @param dialog a dialog
 * @param widget the widget to pack
 * @param group the box-group where to pack the child (ETK_BOX_START or ETK_BOX_END)
 * @param fill_policy the fill-policy of the child, it indicates how it should fill its cell
 * @param padding the amount of free space on the two sides of the child, in pixels
 * @see etk_box_append()
 */
void etk_dialog_pack_widget_in_action_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding)
{
   if (!dialog || !widget)
      return;
   etk_box_append(ETK_BOX(dialog->action_area_hbox), widget, group, fill_policy, padding);
}

/**
 * @brief Packs a pre-created button into the dialog's action-area (at the bottom of the dialog).
 * The button will be appended in the action-area's hbox (see etk_box_append())
 * @param dialog a dialog
 * @param button the button to pack
 * @param response_id the response-id to associate to the button (see Etk_Dialog_Response_ID for common IDs).
 * The response-id will be passed to the "response" callbacks when the button is clicked
 * @param group the box-group where to pack the child (ETK_BOX_START or ETK_BOX_END)
 * @param fill_policy the fill-policy of the child, it indicates how it should fill its cell
 * @param padding the amount of free space on the two sides of the child, in pixels
 * @see etk_box_append()
 */
void etk_dialog_pack_button_in_action_area(Etk_Dialog *dialog, Etk_Button *button, int response_id, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding)
{
   if (!dialog)
      return;

   etk_dialog_button_response_id_set(dialog, button, response_id);
   etk_box_append(ETK_BOX(dialog->action_area_hbox), ETK_WIDGET(button), group, fill_policy, padding);
}

/**
 * @brief Creates and packs a button to the dialog's action-area.
 * The button will be packed in the start-group of the action-area's hbox, and will use the ETK_BOX_FILL fill-policy
 * @param dialog a dialog
 * @param label the button's label
 * @param response_id the response-id to associate to the button (see Etk_Dialog_Response_ID for common IDs).
 * The response-id will be passed to the "response" callbacks when the button is clicked
 * @return Returns the newly added button
 * @note The new button will be automatically shown
 */
Etk_Widget *etk_dialog_button_add(Etk_Dialog *dialog, const char *label, int response_id)
{
   Etk_Widget *button;

   if (!dialog)
      return NULL;

   button = etk_button_new_with_label(label);
   etk_dialog_pack_button_in_action_area(dialog, ETK_BUTTON(button), response_id, ETK_BOX_START, ETK_BOX_FILL, 0);
   etk_widget_show(button);
   return button;
}

/**
 * @brief Creates and packs a button to the dialog's action-area. The button is created from the given stock-id,
 * will be packed in the start-group of the action-area's hbox, and will use the ETK_BOX_FILL fill-policy
 * @param dialog a dialog
 * @param stock_id the button's stock id
 * @param response_id the response-id to associate to the button (see Etk_Dialog_Response_ID for common IDs).
 * The response-id will be passed to the "response" callbacks when the button is clicked
 * @return Returns the newly added button
 * @note The new button will be automatically shown
 */
Etk_Widget *etk_dialog_button_add_from_stock(Etk_Dialog *dialog, int stock_id, int response_id)
{
   Etk_Widget *button;

   if (!dialog)
      return NULL;

   button = etk_button_new_from_stock(stock_id);
   etk_dialog_pack_button_in_action_area(dialog, ETK_BUTTON(button), response_id, ETK_BOX_START, ETK_BOX_FILL, 0);
   etk_widget_show(button);
   return button;
}

/**
 * @brief Sets whether or not the action-area's hbox is homogeneous, i.e whether or not all the widgets of the
 * action-area should have the same size
 * @param dialog a dialog
 * @param homogeneous ETK_TRUE to make the action-area's hbox homogeneous, ETK_FALSE otherwise
 */
void etk_dialog_action_area_homogeneous_set(Etk_Dialog *dialog, Etk_Bool homogeneous)
{
   if (!dialog)
      return;

   etk_box_homogeneous_set(ETK_BOX(dialog->action_area_hbox), homogeneous);
   etk_object_notify(ETK_OBJECT(dialog), "action-area-homogeneous");
}

/**
 * @brief Gets whether or not the action-area's hbox is homogeneous
 * @param dialog a dialog
 * @return Returns ETK_TRUE if the action-area's hbox is homogeneous, ETK_FALSE otherwise
 */
Etk_Bool etk_dialog_action_area_homogeneous_get(Etk_Dialog *dialog)
{
   if (!dialog)
      return ETK_FALSE;
   return etk_box_homogeneous_get(ETK_BOX(dialog->action_area_hbox));
}

/**
 * @brief Sets the horizontal alignment of the widget in the dialog's action-area
 * @param dialog a dialog
 * @param align the horizontal alignment (0.0 = left, 0.5 = center, 1.0 = right, ...)
 */
void etk_dialog_action_area_alignment_set(Etk_Dialog *dialog, float align)
{
   if (!dialog)
      return;

   etk_alignment_set(ETK_ALIGNMENT(dialog->action_area_alignment), align, 0.5, 0.0, 0.0);
   etk_object_notify(ETK_OBJECT(dialog), "action-area-align");
}

/**
 * @brief Gets the alignment of the widgets in the dialog's action-area
 * @param dialog a dialog
 * @return Returns the horizontal alignment of the action-area
 */
float etk_dialog_action_area_alignment_get(Etk_Dialog *dialog)
{
   float align;

   if (!dialog)
      return 0.0;

   etk_alignment_get(ETK_ALIGNMENT(dialog->action_area_alignment), &align, NULL, NULL, NULL);
   return align;
}

/**
 * @brief Gets the vbox of the dialog's main-area. It might be useful if you want more control
 * on the way the widgets are packed
 * @param dialog a dialog
 * @return Returns the vbox of the dialog's main-area
 */
Etk_Widget *etk_dialog_main_area_vbox_get(Etk_Dialog *dialog)
{
   if (!dialog)
      return NULL;
   return dialog->main_area_vbox;
}

/**
 * @brief Gets the hbox of the dialog's action-area. It might be useful if you want more control
 * on the way the widgets are packed
 * @param dialog a dialog
 * @return Returns the hbox of the dialog's action-area
 */
Etk_Widget *etk_dialog_action_area_hbox_get(Etk_Dialog *dialog)
{
   if (!dialog)
      return NULL;
   return dialog->action_area_hbox;
}

/**
 * @brief Associates a response-id to the button. This way, when the button will be clicked, the "response" signal
 * of the dialog will be emitted with this response-id. This function might be useful if you pack the button yourself,
 * without using etk_dialog_pack_button_in_action_area() or etk_dialog_button_add()
 * @param dialog a dialog
 * @param button the button to associate to the dialog and to the response-id
 * @param response_id the response-id to associate to the button
 * @note The button doesn't have to be packed in the dialog
 */
void etk_dialog_button_response_id_set(Etk_Dialog *dialog, Etk_Button *button, int response_id)
{
   int *id;

   if (!dialog || !button)
      return;

   id = malloc(sizeof(int));
   *id = response_id;
   etk_object_data_set_full(ETK_OBJECT(button), "_Etk_Dialog::Response_Id", id, free);

   etk_signal_disconnect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_dialog_button_clicked_cb), dialog);
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_dialog_button_clicked_cb), dialog);
}

/**
 * @brief Gets the response-id associated to the button
 * @param button a button
 * @return Returns the response-id associated to the button, or ETK_RESPONSE_NONE if the button is not packed in a
 * dialog
 */
int etk_dialog_button_response_id_get(Etk_Button *button)
{
   int *id;

   if (!button || !(id = (int *)etk_object_data_get(ETK_OBJECT(button), "_Etk_Dialog::Response_Id")))
      return ETK_RESPONSE_NONE;
   return *id;
}

/**
 * @brief Sets whether or not there is a horizontal separator between the main-area and the action-area of the dialog
 * @param dialog a dialog
 * @param has_separator ETK_TRUE to make the separator visible, ETK_FALSE to hide it
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
   etk_object_notify(ETK_OBJECT(dialog), "has-separator");
}

/**
 * @brief Gets whether or not the horizontal separator of the dialog is visible
 * @param dialog a dialog
 * @return Returns ETK_TRUE if the horizontal separator of the dialog is visible, ETK_FALSE otherwise
 */
Etk_Bool etk_dialog_has_separator_get(Etk_Dialog *dialog)
{
   if (!dialog)
      return ETK_FALSE;
   return dialog->has_separator;
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
   etk_widget_internal_set(dialog->main_area_vbox, ETK_TRUE);
   etk_box_append(ETK_BOX(dialog->dialog_vbox), dialog->main_area_vbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_widget_show(dialog->main_area_vbox);

   dialog->separator = etk_hseparator_new();
   etk_widget_internal_set(dialog->separator, ETK_TRUE);
   etk_widget_theme_parent_set(dialog->separator, ETK_WIDGET(dialog));
   etk_box_append(ETK_BOX(dialog->dialog_vbox), dialog->separator, ETK_BOX_START, ETK_BOX_FILL, 6);
   etk_widget_show(dialog->separator);

   dialog->action_area_alignment = etk_alignment_new(0.5, 0.5, 0.0, 0.0);
   etk_widget_internal_set(dialog->action_area_alignment, ETK_TRUE);
   etk_box_append(ETK_BOX(dialog->dialog_vbox), dialog->action_area_alignment, ETK_BOX_END, ETK_BOX_FILL, 0);
   etk_widget_show(dialog->action_area_alignment);

   dialog->action_area_hbox = etk_hbox_new(ETK_TRUE, 4);
   etk_widget_internal_set(dialog->action_area_hbox, ETK_TRUE);
   etk_container_add(ETK_CONTAINER(dialog->action_area_alignment), dialog->action_area_hbox);
   etk_widget_show(dialog->action_area_hbox);

   etk_widget_lower(dialog->separator);
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
      case ETK_DIALOG_ACTION_AREA_HOMOGENEOUS_PROPERTY:
         etk_dialog_action_area_homogeneous_set(dialog, etk_property_value_bool_get(value));
         break;
      case ETK_DIALOG_ACTION_AREA_ALIGN_PROPERTY:
         etk_dialog_action_area_alignment_set(dialog, etk_property_value_float_get(value));
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
      case ETK_DIALOG_ACTION_AREA_HOMOGENEOUS_PROPERTY:
         etk_property_value_bool_set(value, etk_dialog_action_area_homogeneous_get(dialog));
         break;
      case ETK_DIALOG_ACTION_AREA_ALIGN_PROPERTY:
         etk_property_value_float_set(value, etk_dialog_action_area_alignment_get(dialog));
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

/* Called when a button of the action-area of the dialog is clicked */
static Etk_Bool _etk_dialog_button_clicked_cb(Etk_Object *object, void *data)
{
   int response_id;

   if ((response_id = etk_dialog_button_response_id_get(ETK_BUTTON(object))) == ETK_RESPONSE_NONE)
      return ETK_TRUE;
   etk_signal_emit(ETK_DIALOG_RESPONSE_SIGNAL, ETK_OBJECT(data), response_id);

   return ETK_TRUE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Dialog
 *
 * @image html widgets/dialog.png
 * The dialog's window is split vertically in two areas: the top-area, called "main-area", is made of an Etk_VBox where
 * you can pack any type of widgets; and the bottom-area, called "action-area", which is an Etk_HBox where you can pack
 * any type of widgets, but mostly buttons. Those buttons can be associated to a response id, which will be passed when
 * the button is clicked through the @b "response" signal of the dialog. @n @n
 * You can pack widgets in the main-area with etk_dialog_pack_in_main_area(). @n
 * You can pack widgets in the action-area with etk_dialog_pack_widget_in_action_area(),
 * etk_dialog_pack_button_in_action_area(), etk_dialog_button_add() and etk_dialog_button_add_from_stock(). @n @n
 * Here is a small code that shows how to treat the events when a button of the dialog is clicked:
 * @code
 * //Called when one of the buttons of the dialog is clicked
 * void dialog_response_cb(Etk_Dialog *dialog, int response_id, void *data)
 * {
 *    switch (response_id)
 *    {
 *       case ETK_RESPONSE_OK:
 *          printf("Ok has been clicked\n");
 *          break;
 *       case ETK_RESPONSE_APPLY:
 *          printf("Apply has been clicked\n");
 *          break;
 *       case ETK_RESPONSE_CLOSE:
 *          printf("Close has been clicked\n");
 *          break;
 *       default:
 *          break;
 *    }
 *    etk_widget_hide(ETK_WIDGET(dialog));
 * }
 *
 * //Creation of the dialog
 * Etk_Dialog *dialog;
 *
 * dialog = ETK_DIALOG(etk_dialog_new());
 * etk_dialog_pack_in_main_area(dialog, main_widget, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
 * etk_dialog_button_add_from_stock(dialog, ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);
 * etk_dialog_button_add_from_stock(dialog, ETK_STOCK_DIALOG_APPLY, ETK_RESPONSE_APPLY);
 * etk_dialog_button_add_from_stock(dialog, ETK_STOCK_DIALOG_CLOSE, ETK_RESPONSE_CLOSE);
 * etk_signal_connect("response", ETK_OBJECT(dialog), ETK_CALLBACK(dialog_response_cb), NULL);
 * @endcode @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Toplevel
 *           - Etk_Window
 *             - Etk_Dialog
 *
 * \par Signals:
 * @signal_name "response": Emitted when a button of the action-area of the dialog is clicked.
 * @signal_cb Etk_Bool callback(Etk_Dialog *dialog, int response_id, void *data)
 * @signal_arg dialog: the dialog connected to the callback
 * @signal_arg response_id: the response id of the button that has been clicked
 * @signal_data
 *
 * \par Properties:
 * @prop_name "has-separator": Whether or not the horizontal separator is visible
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "action-area-homogeneous": Whether or not the widgets of the action-area have all the same size
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "action-area-align": The horizontal alignment of the widgets in the action-area,
 * from 0.0 (left) to 1.0 (right)
 * @prop_type Float
 * @prop_rw
 * @prop_val 0.5
 */
