/** @file etk_radio_button.c */
#include "etk_radio_button.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Radio_Button
 * @{
 */

enum Etk_Button_Property_Id
{
   ETK_RADIO_BUTTON_GROUP_PROPERTY
};

static void _etk_radio_button_constructor(Etk_Radio_Button *radio_button);
static void _etk_radio_button_destructor(Etk_Radio_Button *radio_button);
static void _etk_radio_button_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_radio_button_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_radio_button_active_set(Etk_Toggle_Button *toggle_button, Etk_Bool active);

/**
 * @brief Gets the type of an Etk_Radio_Button
 * @return Returns the type of an Etk_Radio_Button
 */
Etk_Type *etk_radio_button_type_get()
{
   static Etk_Type *radio_button_type = NULL;

   if (!radio_button_type)
   {
      radio_button_type = etk_type_new("Etk_Radio_Button", ETK_CHECK_BUTTON_TYPE, sizeof(Etk_Radio_Button),
         ETK_CONSTRUCTOR(_etk_radio_button_constructor), ETK_DESTRUCTOR(_etk_radio_button_destructor));

      etk_type_property_add(radio_button_type, "group", ETK_RADIO_BUTTON_GROUP_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_pointer(NULL));
   
      radio_button_type->property_set = _etk_radio_button_property_set;
      radio_button_type->property_get = _etk_radio_button_property_get;
   }

   return radio_button_type;
}

/**
 * @brief Creates a new radio button
 * @param group the group to which the radio button will be added (NULL if the radio button should create its own group)
 * @return Returns the new radio button widget
 */
Etk_Widget *etk_radio_button_new(Evas_List **group)
{
   return etk_widget_new(ETK_RADIO_BUTTON_TYPE, "theme_group", "radio_button", "group", group,
      "focusable", ETK_TRUE, "xalign", 0.0, NULL);
}

/**
 * @brief Creates a new radio button. It will use the group of another radio button
 * @param radio_button the radio button whose group will be used by the new radio button
 * @return Returns the new radio button widget
 */
Etk_Widget *etk_radio_button_new_from_widget(Etk_Radio_Button *radio_button)
{
   return etk_widget_new(ETK_RADIO_BUTTON_TYPE, "theme_group", "radio_button", "focusable", ETK_TRUE,
      "group", etk_radio_button_group_get(radio_button), "xalign", 0.0, NULL);
}

/**
 * @brief Creates a new radio button with a label
 * @param label the label
 * @param group the group to which the radio button will be added (NULL if the radio button should create its own group)
 * @return Returns the new radio button widget
 */
Etk_Widget *etk_radio_button_new_with_label(const char *label, Evas_List **group)
{
   return etk_widget_new(ETK_RADIO_BUTTON_TYPE, "theme_group", "radio_button", "label", label,
      "focusable", ETK_TRUE, "group", group, "xalign", 0.0, NULL);
}

/**
 * @brief Creates a new radio button with a label. It will use the group of another radio button
 * @param label the label
 * @param radio_button the radio button whose group will be used by the new radio button
 * @return Returns the new radio button widget
 */
Etk_Widget *etk_radio_button_new_with_label_from_widget(const char *label, Etk_Radio_Button *radio_button)
{
   return etk_widget_new(ETK_RADIO_BUTTON_TYPE, "theme_group", "radio_button", "label", label, "focusable", ETK_TRUE,
      "group", etk_radio_button_group_get(radio_button), "xalign", 0.0, NULL);
}

/**
 * @brief Sets the group of the radio button
 * @param radio_button a radio button
 * @param group the group to use
 */
void etk_radio_button_group_set(Etk_Radio_Button *radio_button, Evas_List **group)
{
   Etk_Toggle_Button *toggle_button;
   Etk_Bool active;

   if (!(toggle_button = ETK_TOGGLE_BUTTON(radio_button)) || (group && (radio_button->group == group)))
      return;

   if (radio_button->group)
   {
      *radio_button->group = evas_list_remove(*radio_button->group, radio_button);
      if (!(*radio_button->group))
      {
         free(radio_button->group);
         radio_button->group = NULL;
      }
   }

   if (!group)
   {
      group = malloc(sizeof(Evas_List *));
      *group = NULL;
      active = ETK_TRUE;
   }
   else
      active = ETK_FALSE;

   *group = evas_list_append(*group, radio_button);
   radio_button->group = group;
   etk_object_notify(ETK_OBJECT(radio_button), "group");

   radio_button->can_uncheck = ETK_TRUE;
   etk_toggle_button_active_set(toggle_button, active);
}

/**
 * @brief Gets the group of the radio button
 * @param radio_button a radio button
 * @return Returns the group used by the radio button
 */
Evas_List **etk_radio_button_group_get(Etk_Radio_Button *radio_button)
{
   if (!radio_button)
      return NULL;
   return radio_button->group;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the radio button */
static void _etk_radio_button_constructor(Etk_Radio_Button *radio_button)
{
   if (!radio_button)
      return;

   radio_button->group = NULL;
   radio_button->can_uncheck = ETK_FALSE;
   ETK_TOGGLE_BUTTON(radio_button)->active_set = _etk_radio_button_active_set;
}

/* Destroys the radio button */
static void _etk_radio_button_destructor(Etk_Radio_Button *radio_button)
{
   if (!radio_button || !radio_button->group)
      return;

   *radio_button->group = evas_list_remove(*radio_button->group, radio_button);
   if (!(*radio_button->group))
      free(radio_button->group);
   else if (ETK_TOGGLE_BUTTON(radio_button)->active)
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON((*radio_button->group)->data), ETK_TRUE);
}


/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_radio_button_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Radio_Button *radio_button;

   if (!(radio_button = ETK_RADIO_BUTTON(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_RADIO_BUTTON_GROUP_PROPERTY:
         etk_radio_button_group_set(radio_button, etk_property_value_pointer_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_radio_button_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Radio_Button *radio_button;

   if (!(radio_button = ETK_RADIO_BUTTON(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_RADIO_BUTTON_GROUP_PROPERTY:
         etk_property_value_pointer_set(value, radio_button->group);
         break;
      default:
         break;
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Deactivates all the other radio buttons of the group when a radio button is activated */
static void _etk_radio_button_active_set(Etk_Toggle_Button *toggle_button, Etk_Bool active)
{
   Etk_Radio_Button *radio_button;
   Etk_Toggle_Button *tb;
   Evas_List *l;

   if (!(radio_button = ETK_RADIO_BUTTON(toggle_button)) || toggle_button->active == active)
      return;

   if (!toggle_button->active || (toggle_button->active && radio_button->can_uncheck))
   {
      toggle_button->active = active;
      etk_signal_emit_by_name("toggled", ETK_OBJECT(toggle_button), NULL);
      etk_object_notify(ETK_OBJECT(toggle_button), "active");
   
      if (toggle_button->active)
      {
         /* Uncheck the previously checked button of the group */
         for (l = *radio_button->group; l; l = l->next)
         {
            tb = ETK_TOGGLE_BUTTON(l->data);
            if (tb != toggle_button && tb->active)
            {
               ETK_RADIO_BUTTON(tb)->can_uncheck = ETK_TRUE;
               etk_toggle_button_active_set(tb, ETK_FALSE);
            }
         }
      }
      radio_button->can_uncheck = ETK_FALSE;
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Radio_Button
 *
 * @image html widgets/radio_button.png
 * When a radio button is activated, the other radio buttons of the same group are deactivated. This way, in a group,
 * only one radio button can be active. @n
 * Radio buttons are used when the user has to make a choice between several options. @n
 *
 * To create several radio buttons belonging to the same group, you can first call etk_radio_button_new_with_label() to
 * create the first radio button, and then use etk_radio_button_new_with_label_from_widget() to create the other radio
 * buttons of the group. For example:
 * @code
 * Etk_Widget *radio_buttons[3];
 *
 * //Creates 3 radio buttons belonging to the same group
 * radio_buttons[0] = etk_radio_button_new_with_label("Option 1");
 * radio_buttons[1] = etk_radio_button_new_with_label_from_widget("Option 2", ETK_RADIO_BUTTON(radio_buttons[0]));
 * radio_buttons[2] = etk_radio_button_new_with_label_from_widget("Option 3", ETK_RADIO_BUTTON(radio_buttons[0]));
 * @endcode
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Button
 *           - Etk_Toggle_Button
 *             - Etk_Check_Button
 *               - Etk_Radio_Button
 *
 * \par Properties:
 * @prop_name "group": The group of the radio button
 * @prop_type Pointer (Evas_List **)
 * @prop_rw
 * @prop_val NULL
 */

