/** @file etk_spin_button.c */
#include "etk_spin_button.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include <Edje.h>
#include <stdlib.h>
#include <string.h>

#define is_digit(ch) (((ch)>=(unsigned)'0'&&(ch)<=(unsigned)'9')?1:0)

/**
 * @addtogroup Etk_Spin_Button
* @{
 */

static void _etk_spin_button_constructor(Etk_Spin_Button *spin_button);

static void _etk_spin_button_realize_cb(Etk_Object *object, void *data);
static void _etk_spin_button_text_changed_cb(Etk_Object *object, void *data);

static void _etk_spin_button_scroll_start_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_spin_button_scroll_stop_cb(void *data, Evas_Object *obj, const char *emission, const char *source);

static void _etk_spin_button_value_changed_handler(Etk_Range *range, double value);
static void _etk_spin_button_page_size_changed_cb(Etk_Object *object, const char *property_name, void *data);
static void _etk_spin_button_range_changed_cb(Etk_Object *object, const char *property_name, void *data);
static void _etk_spin_button_mouse_wheel(Etk_Object *object, void *event, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Spin_Button
 * @return Returns the type on an Etk_Spin_Button
 */
Etk_Type *etk_spin_button_type_get()
{
   static Etk_Type *spin_button_type = NULL;

   if (!spin_button_type)
   {
      spin_button_type = etk_type_new("Etk_Spin_Button", ETK_ENTRY_TYPE, sizeof(Etk_Spin_Button), ETK_CONSTRUCTOR(_etk_spin_button_constructor), NULL);
   }

   return spin_button_type;
}

/**
 * @brief Creates a new spin button
 * @return Returns the new spin button widget
 */
Etk_Widget *etk_spin_button_new(double min, double max, double step)
{
   Etk_Widget *new_widget;
   Etk_Table *new_spin_button;
   
   new_widget = etk_widget_new(ETK_SPIN_BUTTON_TYPE, "theme_group", "spin_button", "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
   
//   new_spin_button = ETK_TABLE(new_widget);
//   new_spin_button->range = etk_widget_new(ETK_RANGE_TYPE, NULL);
   
//   etk_range_range_set(new_spin_button->range, min, max);
//   etk_range_increments_set(new_spin_button->range, step, step);   
   
   return new_widget;      
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the spin button */
static void _etk_spin_button_constructor(Etk_Spin_Button *spin_button)
{
   if (!spin_button)
     return;
   
   etk_signal_connect("realize", ETK_OBJECT(spin_button), ETK_CALLBACK(_etk_spin_button_realize_cb), NULL);
   etk_signal_connect("text_changed", ETK_OBJECT(spin_button), ETK_CALLBACK(_etk_spin_button_text_changed_cb), NULL);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(spin_button), ETK_CALLBACK(_etk_spin_button_mouse_wheel), NULL);
   
//   etk_object_notification_callback_add(ETK_OBJECT(spin_button), "page_size", _etk_spin_button_page_size_changed_cb, NULL);
//   etk_object_notification_callback_add(ETK_OBJECT(spin_button), "lower", _etk_spin_button_range_changed_cb, NULL);
//   etk_object_notification_callback_add(ETK_OBJECT(spin_button), "upper", _etk_spin_button_range_changed_cb, NULL);
}

/* Called when the spin button is realized */
static void _etk_spin_button_realize_cb(Etk_Object *object, void *data)
{
   Evas_Object *theme_object;
   
   if (!object || !(theme_object = ETK_WIDGET(object)->theme_object))
     return;
   
   edje_object_signal_callback_add(theme_object, "scroll_*_start", "", _etk_spin_button_scroll_start_cb, object);
   edje_object_signal_callback_add(theme_object, "scroll_stop", "", _etk_spin_button_scroll_stop_cb, object);
}

/* FIXME: etk_entry should support an "empty" string so we dont have to set it to " " */
static char *
_etk_spin_button_str_to_num(const char *str, Etk_Bool *changed)
{
   int i, j = 0;
   int found_dot = 0;
   char *str2;
   char *ret;
   
   if(!str)
     {
	*changed = ETK_TRUE;
	return strdup(" ");
     }   
   
   if(!strcmp(str, " "))
     {
	*changed = ETK_FALSE;
	return strdup(str);
     }
      
   str2 = calloc(strlen(str) + 1, sizeof(char));
         
   for(i = 0; i < strlen(str); i++)
     {
	if(str[i] == '.')
	  {
	     if(found_dot)
	       continue;
	     found_dot = 1;
	  }
	else if(!is_digit(str[i]))
	  {
	     *changed = ETK_TRUE;
	     continue;
	  }
	str2[j] = str[i];
	++j;
     }
   
   str2[j] = '\0';
   
   if(!str2[0])
     {
	free(str2);
	*changed = ETK_TRUE;
	return strdup(" ");
     }
   
   if(!*changed)
     {
	free(str2);
	return strdup(str);
     }
   
   ret = calloc(strlen(str2) + 1, sizeof(char));
   snprintf(ret, strlen(str2) + 1, "%s", str2);
   free(str2);
   
   return ret;
}

/* Called when the spin button's text is changed */
static void _etk_spin_button_text_changed_cb(Etk_Object *object, void *data)
{
   Etk_Spin_Button *spin_button;
   const char      *text;
   char            *text2;
   Etk_Bool         changed;
   
   if (!(spin_button = ETK_SPIN_BUTTON(object)))
     return;
   
   text = etk_entry_text_get(ETK_ENTRY(spin_button));
   
   text2 = _etk_spin_button_str_to_num(text, &changed);     
   
   if(changed)     
     etk_entry_text_set(ETK_ENTRY(spin_button), text2);
     
   free(text2);
}

/* Called when the user wants to change values with the mouse wheel */
static void _etk_spin_button_mouse_wheel(Etk_Object *object, void *event, void *data)
{
   Etk_Spin_Button *spin_button;
   Etk_Event_Mouse_Wheel *wheel_event;
   
   if (!(spin_button = ETK_SPIN_BUTTON(object)) || !(wheel_event = event))
     return;
   
//   etk_range_value_set(spin_button->range, spin_button->range->value + wheel_event->z * spin_button->range->step_increment);
//   etk_widget_event_propagation_stop();
}

static void _etk_spin_button_scroll_start_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   printf("start!\n");
}

static void _etk_spin_button_scroll_stop_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   printf("stop!\n");
}

/** @} */
