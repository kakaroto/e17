/** @file etk_dialog.c */
#include "etk_dialog.h"
#include <stdlib.h>
#include <string.h>
#include "etk_box.h"
#include "etk_separator.h"
#include "etk_button.h"
#include "etk_theme.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Dialog
 * @{
 */

#define ETK_DIALOG_FILL_POLICY(fill, expand) \
   (((fill) ? ETK_BOX_FILL : ETK_BOX_NONE) | (((expand) ? ETK_BOX_EXPAND : ETK_BOX_NONE)))

enum Etk_Dialog_Signal_Id
{
   ETK_DIALOG_RESPONSE_SIGNAL,
   ETK_DIALOG_NUM_SIGNALS
};

enum Etk_Dialog_Property_Id
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
 * @internal
 * @brief Gets the type of an Etk_Dialog
 * @return Returns the type of an Etk_Dialog
 */
Etk_Type *etk_dialog_type_get()
{
   static Etk_Type *dialog_type = NULL;

   if (!dialog_type)
   {
      dialog_type = etk_type_new("Etk_Dialog", ETK_WINDOW_TYPE, sizeof(Etk_Dialog),
         ETK_CONSTRUCTOR(_etk_dialog_constructor), NULL);

      _etk_dialog_signals[ETK_DIALOG_RESPONSE_SIGNAL] = etk_signal_new("response",
         dialog_type, -1, etk_marshaller_VOID__INT, NULL, NULL);

      etk_type_property_add(dialog_type, "has_separator", ETK_DIALOG_HAS_SEPARATOR_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));

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
 * @brief Packs a widget into the dialog's main area (above the buttons and the separator).
 * The widget will be appended in the main area's vbox (see etk_box_append())
 * @param dialog a dialog
 * @param widget the widget to pack
 * @param expand ETK_TRUE to make the widget expand as much as possible
 * @param fill ETK_TRUE to make the widget fill all the size allocated for it
 * @param padding the amount of space in pixels to put between the widget and its neighbors
 * @param pack_at_end if ETK_TRUE, the widget will be packed in the end-group of the vbox (at the bottom). Otherwise,
 * it will be packed at the start (at the top)
 * @see etk_box_append()
 */
void etk_dialog_pack_in_main_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
{
   Etk_Box_Fill_Policy fill_policy;
   Etk_Box_Group group;
   
   if (!dialog || !widget)
      return;
   
   fill_policy = ETK_DIALOG_FILL_POLICY(fill, expand);
   group = pack_at_end ? ETK_BOX_END : ETK_BOX_START;
   etk_box_append(ETK_BOX(dialog->main_area_vbox), widget, group, fill_policy, padding);
}

/**
 * @brief Packs a widget into the dialog's action area (at the bottom of the dialog).
 * The widget will be appended in the action area's hbox (see etk_box_append())
 * @param dialog a dialog
 * @param widget the widget to pack
 * @param expand ETK_TRUE to make the widget expand as much as possible
 * @param fill ETK_TRUE to make the widget fill all the size allocated for it
 * @param padding the amount of space in pixels to put between the widget and its neighbors
 * @param pack_at_end if ETK_TRUE, the widget will be packed in the end-group of the hbox (on the right). Otherwise,
 * it will be packed at the start (on the left)
 * @see etk_box_append()
 */
void etk_dialog_pack_widget_in_action_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
{
   Etk_Box_Fill_Policy fill_policy;
   Etk_Box_Group group;
   
   if (!dialog || !widget)
      return;
   
   fill_policy = ETK_DIALOG_FILL_POLICY(fill, expand);
   group = pack_at_end ? ETK_BOX_END : ETK_BOX_START;
   etk_box_append(ETK_BOX(dialog->action_area_hbox), widget, group, fill_policy, padding);
}

/**
 * @brief Packs a pre-created button into the dialog's action area (at the bottom of the dialog).
 * The button will be appended in the action area's hbox (see etk_box_append())
 * @param dialog a dialog
 * @param button the button to pack
 * @param response_id the response id to associate to the button (see Etk_Dialog_Response_ID for common IDs)
 * @param expand ETK_TRUE to make the button expand as much as possible
 * @param fill ETK_TRUE to make the button fill all the size allocated for it
 * @param padding the amount of space in pixels to put between the button and its neighbors
 * @param pack_at_end if ETK_TRUE, the button will be packed in the end-group of the hbox (on the right). Otherwise,
 * it will be packed at the start (on the left)
 * @see etk_box_append()
 */
void etk_dialog_pack_button_in_action_area(Etk_Dialog *dialog, Etk_Button *button, int response_id, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
{
   int *id;   

   if (!dialog)
      return;
   
   id = malloc(sizeof(int));
   *id = response_id;
   etk_object_data_set_full(ETK_OBJECT(button), "_Etk_Dialog::response_id", id, free);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_dialog_button_clicked_cb), dialog);
   
   etk_dialog_pack_widget_in_action_area(dialog, ETK_WIDGET(button), expand, fill, padding, pack_at_end);
}

/**
 * @brief Adds a button to the dialog's action area.
 * The button will be packed at the end of the hbox of the action area
 * @param dialog a dialog
 * @param label the button's label
 * @param response_id the response id to associate to the button (see Etk_Dialog_Response_ID for common IDs)
 * @return Returns the newly added button
 * @note The new button will be automatically shown
 */
Etk_Widget *etk_dialog_button_add(Etk_Dialog *dialog, const char *label, int response_id)
{
   Etk_Widget *button;

   if (!dialog)
      return NULL;
   
   button = etk_button_new_with_label(label);
   etk_dialog_pack_button_in_action_area(dialog, ETK_BUTTON(button), response_id, ETK_FALSE, ETK_FALSE, 0, ETK_TRUE);
   etk_widget_show(button);
   return button;
}

/**
 * @brief Adds a button created from a stock id to the dialog's action area.
 * The button will be packed at the end of the hbox of the action area
 * @param dialog a dialog
 * @param stock_id the button's stock id
 * @param response_id the response id to associate to the button (see Etk_Dialog_Response_ID for common IDs)
 * @return Returns the newly added button
 * @note The new button will be automatically shown
 */
Etk_Widget *etk_dialog_button_add_from_stock(Etk_Dialog *dialog, int stock_id, int response_id)
{
   Etk_Widget *button;

   if (!dialog)
      return NULL;
   
   button = etk_button_new_from_stock(stock_id);
   etk_dialog_pack_button_in_action_area(dialog, ETK_BUTTON(button), response_id, ETK_FALSE, ETK_FALSE, 0, ETK_TRUE);
   etk_widget_show(button);
   return button;
}

/**
 * @brief Sets whether the dialog has a horizontal separator between its main area and its action area
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
   etk_box_append(ETK_BOX(dialog->dialog_vbox), dialog->main_area_vbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_widget_internal_set(dialog->main_area_vbox, ETK_TRUE);
   etk_widget_show(dialog->main_area_vbox);
   
   dialog->separator = etk_hseparator_new();
   etk_box_append(ETK_BOX(dialog->dialog_vbox), dialog->separator, ETK_BOX_START, ETK_BOX_FILL, 6);
   etk_widget_theme_parent_set(dialog->separator, ETK_WIDGET(dialog));
   etk_widget_internal_set(dialog->separator, ETK_TRUE);
   etk_widget_show(dialog->separator);

   dialog->action_area_hbox = etk_hbox_new(ETK_FALSE, 6);
   etk_box_append(ETK_BOX(dialog->dialog_vbox), dialog->action_area_hbox, ETK_BOX_END, ETK_BOX_FILL, 0);
   etk_widget_internal_set(dialog->action_area_hbox, ETK_TRUE);
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

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Dialog
 *
 * @image html widgets/dialog.png
 * The dialog's window is split vertically in two area: the top area, called "main area", is an Etk_VBox where you can
 * pack any type of widgets; the bottom area, called "action area", is an Etk_HBox where you can pack any type of
 * widgets, but mostly buttons. Those buttons can be associated to a response id, which will be passed when the button
 * is clicked through the @b "response" signal of the dialog. @n @n
 * You can pack widgets in the main area with etk_dialog_pack_in_main_area(). @n
 * You can pack widgets in the action area with etk_dialog_pack_widget_in_action_area(),
 * etk_dialog_pack_button_in_action_area(), etk_dialog_button_add() and etk_dialog_button_add_from_stock(). @n @n
 * A horizontal separator can also separate the action area from the main area (visible by default)
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
 * @signal_name "response": Emitted when a button of the action area of the dialog is clicked.
 * @signal_cb void callback(Etk_Dialog *dialog, int response_id, void *data)
 * @signal_arg dialog: the dialog connected to the callback
 * @signal_arg response_id: the response id of the button that has been clicked
 * @signal_data
 *
 * \par Properties:
 * @prop_name "has_separator": Whether or not the horizontal separator is visible
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 */
