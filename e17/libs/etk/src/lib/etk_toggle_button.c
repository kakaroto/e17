/** @file etk_toggle_button.c */
#include "etk_toggle_button.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Toggle_Button
 * @{
 */

enum Etk_Toggle_Button_Signal_Id
{
   ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL,
   ETK_TOGGLE_BUTTON_NUM_SIGNALS
};

enum Etk_Toggle_Button_Property_Id
{
   ETK_TOGGLE_BUTTON_ACTIVE_PROPERTY
};

static void _etk_toggle_button_constructor(Etk_Toggle_Button *toggle_button);
static void _etk_toggle_button_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toggle_button_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toggle_button_realized_cb(Etk_Object *object, void *data);
static void _etk_toggle_button_label_realized_cb(Etk_Object *object, void *data);
static void _etk_toggle_button_toggled_handler(Etk_Toggle_Button *toggle_button);
static void _etk_toggle_button_active_set_default(Etk_Toggle_Button *toggle_button, Etk_Bool active);

static Etk_Signal *_etk_toggle_button_signals[ETK_TOGGLE_BUTTON_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Toggle_Button
 * @return Returns the type of an Etk_Toggle_Button
 */
Etk_Type *etk_toggle_button_type_get(void)
{
   static Etk_Type *toggle_button_type = NULL;

   if (!toggle_button_type)
   {
      toggle_button_type = etk_type_new("Etk_Toggle_Button", ETK_BUTTON_TYPE, sizeof(Etk_Toggle_Button),
            ETK_CONSTRUCTOR(_etk_toggle_button_constructor), NULL);
      
      _etk_toggle_button_signals[ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL] = etk_signal_new("toggled",
            toggle_button_type, ETK_MEMBER_OFFSET(Etk_Toggle_Button, toggled_handler), etk_marshaller_VOID__VOID, NULL, NULL);
   
      etk_type_property_add(toggle_button_type, "active", ETK_TOGGLE_BUTTON_ACTIVE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      
      toggle_button_type->property_set = _etk_toggle_button_property_set;
      toggle_button_type->property_get = _etk_toggle_button_property_get;
   }

   return toggle_button_type;
}

/**
 * @brief Creates a new toggle button
 * @return Returns the new toggle-button widget
 */
Etk_Widget *etk_toggle_button_new(void)
{
   return etk_widget_new(ETK_TOGGLE_BUTTON_TYPE, "theme-group", "toggle_button","focusable", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new toggle button with a label
 * @param label the label
 * @return Returns the new toggle-button widget
 */
Etk_Widget *etk_toggle_button_new_with_label(const char *label)
{
   return etk_widget_new(ETK_TOGGLE_BUTTON_TYPE, "theme-group", "toggle_button",
         "label", label, "focusable", ETK_TRUE, NULL);
}

/**
 * @brief Toggles the toggle button
 * @param toggle_button a toggle button
 */
void etk_toggle_button_toggle(Etk_Toggle_Button *toggle_button)
{
   if (!toggle_button)
      return;
   etk_toggle_button_active_set(toggle_button, !toggle_button->active);
}

/**
 * @brief Sets whether the toggle button is active or not
 * @param toggle_button a toggle button
 * @param active if @a active is ETK_TRUE, the toggle button will be activated. Otherwise, it will be deactivated
 */
void etk_toggle_button_active_set(Etk_Toggle_Button *toggle_button, Etk_Bool active)
{
   if (toggle_button && toggle_button->active_set)
      toggle_button->active_set(toggle_button, active);
}

/**
 * @brief Gets the state of the toggle button
 * @param toggle_button a toggle button
 * @return Returns ETK_TRUE if the button is active, ETK_FALSE otherwise
 */
Etk_Bool etk_toggle_button_active_get(Etk_Toggle_Button *toggle_button)
{
   if (!toggle_button)
      return ETK_FALSE;
   return toggle_button->active;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the toggle button */
static void _etk_toggle_button_constructor(Etk_Toggle_Button *toggle_button)
{
   if (!toggle_button)
      return;

   toggle_button->active = 0;
   toggle_button->toggled_handler = _etk_toggle_button_toggled_handler;
   toggle_button->active_set = _etk_toggle_button_active_set_default;

   etk_signal_connect("realized", ETK_OBJECT(toggle_button),
         ETK_CALLBACK(_etk_toggle_button_realized_cb), NULL);
   etk_signal_connect("realized", ETK_OBJECT(ETK_BUTTON(toggle_button)->label),
         ETK_CALLBACK(_etk_toggle_button_label_realized_cb), toggle_button);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(toggle_button),
         ETK_CALLBACK(etk_toggle_button_toggle), toggle_button);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_toggle_button_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Toggle_Button *toggle_button;

   if (!(toggle_button = ETK_TOGGLE_BUTTON(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TOGGLE_BUTTON_ACTIVE_PROPERTY:
         etk_toggle_button_active_set(toggle_button, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_toggle_button_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Toggle_Button *toggle_button;

   if (!(toggle_button = ETK_TOGGLE_BUTTON(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TOGGLE_BUTTON_ACTIVE_PROPERTY:
         etk_property_value_bool_set(value, toggle_button->active);
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

/* Called when the toggle-button is realized */
static void _etk_toggle_button_realized_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle_button;

   if (!(toggle_button = ETK_TOGGLE_BUTTON(object)))
      return;
   
   etk_widget_theme_signal_emit(ETK_WIDGET(toggle_button),
         toggle_button->active ? "etk,state,on" : "etk,state,off", ETK_FALSE);
   etk_widget_theme_signal_emit(ETK_BUTTON(toggle_button)->label,
         toggle_button->active ? "etk,state,on" : "etk,state,off", ETK_FALSE);
}

/* Called when the toggle-button's label is realized */
static void _etk_toggle_button_label_realized_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle_button;

   if (!(toggle_button = ETK_TOGGLE_BUTTON(data)))
      return;
   
   etk_widget_theme_signal_emit(ETK_BUTTON(toggle_button)->label,
         toggle_button->active ? "etk,state,on" : "etk,state,off", ETK_FALSE);
}

/* Default handler for the "toggled" signal */
static void _etk_toggle_button_toggled_handler(Etk_Toggle_Button *toggle_button)
{
   if (!toggle_button)
      return;
   
   etk_widget_theme_signal_emit(ETK_WIDGET(toggle_button),
         toggle_button->active ? "etk,state,on" : "etk,state,off", ETK_FALSE);
   etk_widget_theme_signal_emit(ETK_BUTTON(toggle_button)->label,
         toggle_button->active ? "etk,state,on" : "etk,state,off", ETK_FALSE);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* This function is called when the toggle-button is turned on: here it just activates it */
static void _etk_toggle_button_active_set_default(Etk_Toggle_Button *toggle_button, Etk_Bool active)
{
   if (!toggle_button || toggle_button->active == active)
      return;
   
   toggle_button->active = active;
   etk_signal_emit(_etk_toggle_button_signals[ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL], ETK_OBJECT(toggle_button), NULL);
   etk_object_notify(ETK_OBJECT(toggle_button), "active");
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Toggle_Button
 *
 * @image html widgets/toggle_button.png
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Button
 *           - Etk_Toggle_Button
 *
 * \par Signals:
 * @signal_name "toggled": Emitted when the state of the toggle button is changed
 * @signal_cb void callback(Etk_Toggle_Button *button, void *data)
 * @signal_arg button: the toggle button that has been toggled
 * @signal_data
 *
 * \par Properties:
 * @prop_name "active": Whether or not the toggle button is active
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 */
