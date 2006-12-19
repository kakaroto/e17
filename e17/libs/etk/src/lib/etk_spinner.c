/** @file etk_spinner.c */
#include "etk_spinner.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_editable.h"
#include "etk_toplevel.h"
#include "etk_selection.h"
#include "etk_event.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Spinner
 * @{
 */

#define FIRST_DELAY 0.4
#define REPEAT_DELAY 0.05
#define MAX_TEXT_LEN 256

enum Etk_Spinner_Propery_Id
{
   ETK_SPINNER_DIGITS_PROPERTY
};

static void _etk_spinner_constructor(Etk_Spinner *spinner);
static void _etk_spinner_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_spinner_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_spinner_realize_cb(Etk_Object *object, void *data);
static void _etk_spinner_unrealize_cb(Etk_Object *object, void *data);
static void _etk_spinner_focus_cb(Etk_Object *object, void *data);
static void _etk_spinner_unfocus_cb(Etk_Object *object, void *data);

static void _etk_spinner_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static void _etk_spinner_editable_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_spinner_editable_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_spinner_editable_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_spinner_editable_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_spinner_editable_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);

static void _etk_spinner_selection_received_cb(Etk_Object *object, void *event, void *data);
static void _etk_spinner_value_changed_handler(Etk_Range *range, double value);

static void _etk_spinner_step_start_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_spinner_step_stop_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static int _etk_spinner_step_decrement_timer_cb(void *data);
static int _etk_spinner_step_increment_timer_cb(void *data);

static void _etk_spinner_update_text_from_value(Etk_Spinner *spinner);
static void _etk_spinner_update_value_from_text(Etk_Spinner *spinner);
static void _etk_spinner_selection_copy(Etk_Spinner *spinner, Etk_Selection_Type selection, Etk_Bool cut);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Spinner
 * @return Returns the type of an Etk_Spinner
 */
Etk_Type *etk_spinner_type_get()
{
   static Etk_Type *spinner_type = NULL;

   if (!spinner_type)
   {
      spinner_type = etk_type_new("Etk_Spinner", ETK_RANGE_TYPE, sizeof(Etk_Spinner),
         ETK_CONSTRUCTOR(_etk_spinner_constructor), NULL);
      
      etk_type_property_add(spinner_type, "digits", ETK_SPINNER_DIGITS_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      
      spinner_type->property_set = _etk_spinner_property_set;
      spinner_type->property_get = _etk_spinner_property_get;
   }

   return spinner_type;
}

/**
 * @brief Creates a new spinner
 * @return Returns the new spinner widget
 */
Etk_Widget *etk_spinner_new(double lower, double upper, double value, double step_increment, double page_increment)
{
   return etk_widget_new(ETK_SPINNER_TYPE, "theme_group", "spinner",
      "focusable", ETK_TRUE, "focus_on_click", ETK_TRUE,
      "lower", lower, "upper", upper, "value", value, "step_increment",
      step_increment, "page_increment", page_increment, NULL);
}

/**
 * @brief Sets the number of digits displayed by the spinner
 * @param spinner a spinner
 * @param digits the number of digits to display
 */
void etk_spinner_digits_set(Etk_Spinner *spinner, int digits)
{
   if (!spinner || spinner->digits == digits)
      return;
   
   spinner->digits = ETK_MAX(0, digits);
   snprintf(spinner->value_format, 16, "%%.%df", spinner->digits);
   
   _etk_spinner_update_text_from_value(spinner);
   etk_object_notify(ETK_OBJECT(spinner), "digits");
}

/**
 * @brief Gets the number of digits displayed by the spinner
 * @param spinner a spinner
 * @return Returns the number of digits displayed by the spinner
 */
int etk_spinner_digits_get(Etk_Spinner *spinner)
{
   if (!spinner)
      return 0;
   return spinner->digits;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the spinner */
static void _etk_spinner_constructor(Etk_Spinner *spinner)
{
   if (!spinner)
      return;
   
   spinner->digits = 0;
   strcpy(spinner->value_format, "%.0f");
   spinner->step_timer = NULL;
   spinner->successive_steps = 0;

   spinner->editable_object = NULL;
   spinner->selection_dragging = ETK_FALSE;
   
   ETK_RANGE(spinner)->value_changed = _etk_spinner_value_changed_handler;
   etk_signal_connect("realize", ETK_OBJECT(spinner), ETK_CALLBACK(_etk_spinner_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(spinner), ETK_CALLBACK(_etk_spinner_unrealize_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(spinner), ETK_CALLBACK(_etk_spinner_key_down_cb), NULL);
   etk_signal_connect("focus", ETK_OBJECT(spinner), ETK_CALLBACK(_etk_spinner_focus_cb), NULL);
   etk_signal_connect("unfocus", ETK_OBJECT(spinner), ETK_CALLBACK(_etk_spinner_unfocus_cb), NULL);
   etk_signal_connect("selection_received", ETK_OBJECT(spinner),
      ETK_CALLBACK(_etk_spinner_selection_received_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_spinner_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Spinner *spinner;
   
   if (!(spinner = ETK_SPINNER(object)) || !value)
      return;
   
   switch (property_id)
   {
      case ETK_SPINNER_DIGITS_PROPERTY:
	 etk_spinner_digits_set(spinner, etk_property_value_int_get(value));
	 break;
      default:
	 break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_spinner_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Spinner *spinner;
   
   if (!(spinner = ETK_SPINNER(object)) || !value)
      return;
   
   switch (property_id)
   {
      case ETK_SPINNER_DIGITS_PROPERTY:
         etk_property_value_int_set(value, spinner->digits);
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

/* Called when the spinner is realized */
static void _etk_spinner_realize_cb(Etk_Object *object, void *data)
{
   Etk_Spinner *spinner;
   Evas_Object *theme_object;
   Evas *evas;

   if (!(spinner = ETK_SPINNER(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(spinner))))
      return;
   if (!(theme_object = ETK_WIDGET(spinner)->theme_object))
      return;

   /* Create the editable object */
   spinner->editable_object = etk_editable_add(evas);
   etk_editable_theme_set(spinner->editable_object, etk_widget_theme_file_get(ETK_WIDGET(spinner)),
      etk_widget_theme_group_get(ETK_WIDGET(spinner)));
   etk_editable_align_set(spinner->editable_object, 1.0);
   
   if (!etk_widget_is_focused(ETK_WIDGET(spinner)))
   {
      etk_editable_cursor_hide(spinner->editable_object);
      etk_editable_selection_hide(spinner->editable_object);
   }
   evas_object_show(spinner->editable_object);
   etk_widget_swallow_object(ETK_WIDGET(spinner), "etk.swallow.text", spinner->editable_object);
   
   evas_object_event_callback_add(spinner->editable_object, EVAS_CALLBACK_MOUSE_IN,
      _etk_spinner_editable_mouse_in_cb, spinner);
   evas_object_event_callback_add(spinner->editable_object, EVAS_CALLBACK_MOUSE_OUT,
      _etk_spinner_editable_mouse_out_cb, spinner);
   evas_object_event_callback_add(spinner->editable_object, EVAS_CALLBACK_MOUSE_DOWN,
      _etk_spinner_editable_mouse_down_cb, spinner);
   evas_object_event_callback_add(spinner->editable_object, EVAS_CALLBACK_MOUSE_UP,
      _etk_spinner_editable_mouse_up_cb, spinner);
   evas_object_event_callback_add(spinner->editable_object, EVAS_CALLBACK_MOUSE_MOVE,
      _etk_spinner_editable_mouse_move_cb, spinner);
   
   /* Connect the "decrement"/"increment" edje signals to callbacks */
   edje_object_signal_callback_add(theme_object, "etk,action,decrement,start", "", _etk_spinner_step_start_cb, spinner);
   edje_object_signal_callback_add(theme_object, "etk,action,increment,start", "", _etk_spinner_step_start_cb, spinner);
   edje_object_signal_callback_add(theme_object, "etk,action,decrement,stop", "", _etk_spinner_step_stop_cb, spinner);
   edje_object_signal_callback_add(theme_object, "etk,action,increment,stop", "", _etk_spinner_step_stop_cb, spinner);
   
   /* Update the text of the editable object */
   _etk_spinner_update_text_from_value(spinner);
}

/* Called when the spinner is unrealized */
static void _etk_spinner_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Spinner *spinner;

   if (!(spinner = ETK_SPINNER(object)))
      return;
   
   evas_object_del(spinner->editable_object);
   spinner->editable_object = NULL;
}

/* Called when the spinner is focused */
static void _etk_spinner_focus_cb(Etk_Object *object, void *data)
{
   Etk_Spinner *spinner;

   if (!(spinner = ETK_SPINNER(object)) || !spinner->editable_object)
      return;
   
   etk_editable_cursor_show(spinner->editable_object);
   etk_editable_selection_show(spinner->editable_object);
}

/* Called when the spinner is unfocused */
static void _etk_spinner_unfocus_cb(Etk_Object *object, void *data)
{
   Etk_Spinner *spinner;

   if (!(spinner = ETK_SPINNER(object)) || !spinner->editable_object)
      return;
   
   etk_editable_cursor_move_to_end(spinner->editable_object);
   etk_editable_selection_move_to_end(spinner->editable_object);
   etk_editable_cursor_hide(spinner->editable_object);
   etk_editable_selection_hide(spinner->editable_object);
   
   _etk_spinner_update_value_from_text(spinner);
}

/* Called when the user presses a key */
static void _etk_spinner_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Spinner *spinner;
   Etk_Range *range;
   Evas_Object *editable;
   int cursor_pos, selection_pos;
   int start_pos, end_pos;
   Etk_Bool selecting;
   Etk_Bool changed = ETK_FALSE;
   Etk_Bool selection_changed = ETK_FALSE;
   Etk_Bool stop_signal = ETK_TRUE;
   
   if (!(spinner = ETK_SPINNER(object)))
     return;

   range = ETK_RANGE(spinner);
   editable = spinner->editable_object;
   cursor_pos = etk_editable_cursor_pos_get(editable);
   selection_pos = etk_editable_selection_pos_get(editable);
   start_pos = ETK_MIN(cursor_pos, selection_pos);
   end_pos = ETK_MAX(cursor_pos, selection_pos);
   selecting = (start_pos != end_pos);
   
   /* Move the cursor/selection to the left */
   if (strcmp(event->keyname, "Left") == 0)
   {
      if (event->modifiers & ETK_MODIFIER_SHIFT)
      {
         etk_editable_cursor_move_left(editable);
         selection_changed = ETK_TRUE;
      }
      else if (selecting)
      {
         if (cursor_pos < selection_pos)
            etk_editable_selection_pos_set(editable, cursor_pos);
         else
            etk_editable_cursor_pos_set(editable, selection_pos);
      }
      else
      {
         etk_editable_cursor_move_left(editable);
         etk_editable_selection_pos_set(editable, etk_editable_cursor_pos_get(editable));
      }
   }
   /* Move the cursor/selection to the right */
   else if (strcmp(event->keyname, "Right") == 0)
   {
      if (event->modifiers & ETK_MODIFIER_SHIFT)
      {
         etk_editable_cursor_move_right(editable);
         selection_changed = ETK_TRUE;
      }
      else if (selecting)
      {
         if (cursor_pos > selection_pos)
            etk_editable_selection_pos_set(editable, cursor_pos);
         else
            etk_editable_cursor_pos_set(editable, selection_pos);
      }
      else
      {
         etk_editable_cursor_move_right(editable);
         etk_editable_selection_pos_set(editable, etk_editable_cursor_pos_get(editable));
      }
   }
   /* Move the cursor/selection to the start of the spinner */
   else if (strcmp(event->keyname, "Home") == 0)
   {
      etk_editable_cursor_move_to_start(editable);
      if (!(event->modifiers & ETK_MODIFIER_SHIFT))
         etk_editable_selection_pos_set(editable, etk_editable_cursor_pos_get(editable));
      else
         selection_changed = ETK_TRUE;
   }
   /* Move the cursor/selection to the end of the spinner */
   else if (strcmp(event->keyname, "End") == 0)
   {
      etk_editable_cursor_move_to_end(editable);
      if (!(event->modifiers & ETK_MODIFIER_SHIFT))
         etk_editable_selection_pos_set(editable, etk_editable_cursor_pos_get(editable));
      else
         selection_changed = ETK_TRUE;
   }
   /* Delete the previous character */
   else if (strcmp(event->keyname, "BackSpace") == 0)
   {
      if (selecting)
         changed = etk_editable_delete(editable, start_pos, end_pos);
      else
         changed = etk_editable_delete(editable, cursor_pos - 1, cursor_pos);
   }
   /* Delete the next character */
   else if (strcmp(event->keyname, "Delete") == 0)
   {
      if (selecting)
         changed = etk_editable_delete(editable, start_pos, end_pos);
      else
         changed = etk_editable_delete(editable, cursor_pos, cursor_pos + 1);
   }
   /* Increment the value */
   else if (strcmp(event->keyname, "Up") == 0)
   {
      /* TODO: increment faster if the key has been pressed for a long time... */
      etk_range_value_set(range, range->value + range->step_increment);
   }
   /* Decrement the value */
   else if (strcmp(event->keyname, "Down") == 0)
   {
      /* TODO: decrement faster if the key has been pressed for a long time... */
      etk_range_value_set(range, range->value - range->step_increment);
   }
   /* Validate the value entered in the spinner */
   else if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "KP_Enter") == 0)
      _etk_spinner_update_value_from_text(spinner);
   /* Ctrl + A,C,X,V */
   else if (event->modifiers & ETK_MODIFIER_CTRL)
   {
      if (strcmp(event->keyname, "a") == 0)
      {
         etk_editable_select_all(editable);
         selection_changed = ETK_TRUE;
      }
      else if (strcmp(event->keyname, "x") == 0 || strcmp(event->keyname, "c") == 0)
         _etk_spinner_selection_copy(spinner, ETK_SELECTION_CLIPBOARD, (strcmp(event->keyname, "x") == 0));
      else if (strcmp(event->keyname, "v") == 0)
         etk_selection_text_request(ETK_SELECTION_CLIPBOARD, ETK_WIDGET(spinner));
      else
         stop_signal = ETK_FALSE;
   }
   /* Otherwise, we insert the corresponding character */
   else if (event->string && *event->string && (strlen(event->string) != 1 || event->string[0] >= 0x20))
   {
      if (selecting)
         changed |= etk_editable_delete(editable, start_pos, end_pos);
      changed |= etk_editable_insert(editable, start_pos, event->string);
   }
   else
      stop_signal = ETK_FALSE;
   
   if (selection_changed)
      _etk_spinner_selection_copy(spinner, ETK_SELECTION_PRIMARY, ETK_FALSE);
   if (stop_signal)
      etk_signal_stop();
}

/* Called when the mouse enters the spinner's editable-object */
static void _etk_spinner_editable_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *spinner_widget;

   if (!(spinner_widget = ETK_WIDGET(data)))
      return;
   etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(spinner_widget), ETK_POINTER_TEXT_EDIT);
}

/* Called when the mouse leaves the spinner's editable-object */
static void _etk_spinner_editable_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *spinner_widget;

   if (!(spinner_widget = ETK_WIDGET(data)))
      return;
   etk_toplevel_pointer_pop(spinner_widget->toplevel_parent, ETK_POINTER_TEXT_EDIT);
}

/* Called when the spinner's editable-object is pressed by the mouse */
static void _etk_spinner_editable_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Spinner *spinner;
   Etk_Event_Mouse_Down event;
   Evas_Coord ox, oy;
   int pos;
   
   if (!(spinner = ETK_SPINNER(data)))
      return;
   
   etk_event_mouse_down_wrap(ETK_WIDGET(spinner), event_info, &event);
   evas_object_geometry_get(spinner->editable_object, &ox, &oy, NULL, NULL);
   pos = etk_editable_pos_get_from_coords(spinner->editable_object, event.canvas.x - ox, event.canvas.y - oy);
   if (event.button == 1)
   {
      if (event.flags & ETK_MOUSE_DOUBLE_CLICK)
         etk_editable_select_all(spinner->editable_object);
      else
      {
         etk_editable_cursor_pos_set(spinner->editable_object, pos);
         if (!(event.modifiers & ETK_MODIFIER_SHIFT))
            etk_editable_selection_pos_set(spinner->editable_object, pos);
         
         spinner->selection_dragging = ETK_TRUE;
      }
   }
   else if (event.button == 2)
   {
      etk_editable_cursor_pos_set(spinner->editable_object, pos);
      etk_editable_selection_pos_set(spinner->editable_object, pos);
      
      etk_selection_text_request(ETK_SELECTION_PRIMARY, ETK_WIDGET(spinner));
   }
}

/* Called when the spinner's editable-object is released by the mouse */
static void _etk_spinner_editable_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Spinner *spinner;
   Etk_Event_Mouse_Up event;
   
   if (!(spinner = ETK_SPINNER(data)))
      return;
   
   etk_event_mouse_up_wrap(ETK_WIDGET(spinner), event_info, &event);
   if (event.button == 1)
   {
      spinner->selection_dragging = ETK_FALSE;
      _etk_spinner_selection_copy(spinner, ETK_SELECTION_PRIMARY, ETK_FALSE);
   }
}

/* Called when the mouse moves over the spinner's editable-object */
static void _etk_spinner_editable_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Spinner *spinner;
   Etk_Event_Mouse_Move event; 
   Evas_Coord ox, oy;
   int pos;
   
   if (!(spinner = ETK_SPINNER(data)))
      return;
   
   if (spinner->selection_dragging)
   {
      etk_event_mouse_move_wrap(ETK_WIDGET(spinner), event_info, &event);
      evas_object_geometry_get(spinner->editable_object, &ox, &oy, NULL, NULL);
      pos = etk_editable_pos_get_from_coords(spinner->editable_object, event.cur.canvas.x - ox, event.cur.canvas.y - oy);
      if (pos >= 0)
         etk_editable_cursor_pos_set(spinner->editable_object, pos);
   }
}

/* Called when the selection/clipboard content is received */
static void _etk_spinner_selection_received_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Spinner *spinner;
   Evas_Object *editable;
   Etk_Selection_Event *ev = event;
   const char *text;
   
   if (!(spinner = ETK_SPINNER(object)) || !(editable = spinner->editable_object))
      return;
   
   if (ev->type == ETK_SELECTION_TEXT && (text = ev->data.text) && *text && (strlen(text) != 1 || text[0] >= 0x20))
   {
      int cursor_pos, selection_pos;
      int start_pos, end_pos;
      Etk_Bool selecting;
      
      cursor_pos = etk_editable_cursor_pos_get(editable);
      selection_pos = etk_editable_selection_pos_get(editable);
      start_pos = ETK_MIN(cursor_pos, selection_pos);
      end_pos = ETK_MAX(cursor_pos, selection_pos);
      selecting = (start_pos != end_pos);
      
      if (selecting)
         etk_editable_delete(editable, start_pos, end_pos);
      etk_editable_insert(editable, start_pos, text);
   }
}

/* Default handler for the "value_changed" signal of a spinner */
static void _etk_spinner_value_changed_handler(Etk_Range *range, double value)
{
   Etk_Spinner *spinner;
   
   if (!(spinner = ETK_SPINNER(range)))
      return;
   _etk_spinner_update_text_from_value(spinner);
}

/* TODOC */
static void _etk_spinner_step_start_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Spinner *spinner;
   
   if (!(spinner = ETK_SPINNER(data)))
      return;
   
   if (spinner->step_timer)
   {
      ecore_timer_del(spinner->step_timer);
      spinner->step_timer = NULL;
   }
   
   spinner->successive_steps = 0;
   if (strcmp(emission, "etk,action,decrement,start") == 0)
      spinner->step_timer = ecore_timer_add(0.0, _etk_spinner_step_decrement_timer_cb, spinner);
   else if (strcmp(emission, "etk,action,increment,start") == 0)
      spinner->step_timer = ecore_timer_add(0.0, _etk_spinner_step_increment_timer_cb, spinner);
}

/* TODOC */
static void _etk_spinner_step_stop_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Spinner *spinner;
   
   if (!(spinner = ETK_SPINNER(data)))
      return;
   
   if (spinner->step_timer)
   {
      ecore_timer_del(spinner->step_timer);
      spinner->step_timer = NULL;
   }
}

/* TODOC */
static int _etk_spinner_step_decrement_timer_cb(void *data)
{
   Etk_Spinner *spinner;
   Etk_Range *range;
   
   if (!(spinner = ETK_SPINNER(data)))
      return 1;

   _etk_spinner_update_value_from_text(spinner);
   
   range = ETK_RANGE(spinner);
   etk_range_value_set(range, range->value - range->step_increment);
   
   ecore_timer_interval_set(spinner->step_timer, (spinner->successive_steps == 0) ? FIRST_DELAY : REPEAT_DELAY);
   spinner->successive_steps++;
   
   return 1;
}

/* TODOC */
static int _etk_spinner_step_increment_timer_cb(void *data)
{
   Etk_Spinner *spinner;
   Etk_Range *range;
   
   if (!(spinner = ETK_SPINNER(data)))
      return 1;

   _etk_spinner_update_value_from_text(spinner);
   
   range = ETK_RANGE(spinner);
   etk_range_value_set(range, range->value + range->step_increment);
   
   ecore_timer_interval_set(spinner->step_timer, (spinner->successive_steps == 0) ? FIRST_DELAY : REPEAT_DELAY);
   spinner->successive_steps++;
   
   return 1;
}

/**************************
 *
 * Private function
 *
 **************************/

/* Updates the text of the spinner from its value */
static void _etk_spinner_update_text_from_value(Etk_Spinner *spinner)
{
   char text[MAX_TEXT_LEN];
   
   if (!spinner || !spinner->editable_object)
      return;
   
   snprintf(text, MAX_TEXT_LEN, spinner->value_format, etk_range_value_get(ETK_RANGE(spinner)));
   etk_editable_text_set(spinner->editable_object, text);
}

/* Updates the value of the spinner from its text */
static void _etk_spinner_update_value_from_text(Etk_Spinner *spinner)
{
   const char *text;
   float value;
   
   if (!spinner || !spinner->editable_object)
      return;
   
   text = etk_editable_text_get(spinner->editable_object);
   if (sscanf(text, "%f", &value) != 1)
      value = 0.0;
   etk_range_value_set(ETK_RANGE(spinner), value);
}

/* Copies the selected text of the spinner to the given selection */
static void _etk_spinner_selection_copy(Etk_Spinner *spinner, Etk_Selection_Type selection, Etk_Bool cut)
{
   Evas_Object *editable;
   int cursor_pos, selection_pos;
   int start_pos, end_pos;
   Etk_Bool selecting;
   char *range;
   
   if (!spinner)
     return;

   editable = spinner->editable_object;
   cursor_pos = etk_editable_cursor_pos_get(editable);
   selection_pos = etk_editable_selection_pos_get(editable);
   start_pos = ETK_MIN(cursor_pos, selection_pos);
   end_pos = ETK_MAX(cursor_pos, selection_pos);
   selecting = (start_pos != end_pos);
   
   if (!selecting)
      return;
   
   range = etk_editable_text_range_get(editable, start_pos, end_pos);
   if (range)
   {
      etk_selection_text_set(selection, range);
      free(range);
      if (cut)
         etk_editable_delete(editable, start_pos, end_pos);
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Spinner
 *
 * TODOC
 */
