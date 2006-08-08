/** @file etk_entry.c */
#include "etk_entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_editable_text_object.h"
#include "etk_toplevel_widget.h"

/**
 * @addtogroup Etk_Entry
* @{
 */

enum _Etk_Entry_Signal_Id
{
   ETK_ENTRY_TEXT_CHANGED_SIGNAL,
   ETK_ENTRY_NUM_SIGNALS
};

enum _Etk_Entry_Propery_Id
{
   ETK_ENTRY_PASSWORD_PROPERTY
};

static void _etk_entry_constructor(Etk_Entry *entry);
static void _etk_entry_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_entry_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_entry_realize_cb(Etk_Object *object, void *data);
static void _etk_entry_unrealize_cb(Etk_Object *object, void *data);
static void _etk_entry_key_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_entry_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_entry_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_entry_focus_cb(Etk_Object *object, void *data);
static void _etk_entry_unfocus_cb(Etk_Object *object, void *data);
static void _etk_entry_text_changed_cb(Etk_Object *object, void *data);
  
static Etk_Signal *_etk_entry_signals[ETK_ENTRY_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Entry
 * @return Returns the type on an Etk_Entry
 */
Etk_Type *etk_entry_type_get()
{
   static Etk_Type *entry_type = NULL;

   if (!entry_type)
   {
      entry_type = etk_type_new("Etk_Entry", ETK_WIDGET_TYPE, sizeof(Etk_Entry), ETK_CONSTRUCTOR(_etk_entry_constructor), NULL);

      _etk_entry_signals[ETK_ENTRY_TEXT_CHANGED_SIGNAL] = etk_signal_new("text_changed", entry_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      
      etk_type_property_add(entry_type, "password", ETK_ENTRY_PASSWORD_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_FALSE));
            
      entry_type->property_set = _etk_entry_property_set;
      entry_type->property_get = _etk_entry_property_get;      
   }

   return entry_type;
}

/**
 * @brief Creates a new entry
 * @return Returns the new entry widget
 */
Etk_Widget *etk_entry_new()
{
   return etk_widget_new(ETK_ENTRY_TYPE, "theme_group", "entry", "focusable", ETK_TRUE, "focus_on_click", ETK_TRUE, NULL);
}

/**
 * @brief Sets the text of an entry
 * @param entry an entry
 * @param text the text to set
 */
void etk_entry_text_set(Etk_Entry *entry, const char *text)
{
   if (!entry)
      return;
   if(!entry->editable_object)
     entry->text = strdup(text);
   else
   {
      etk_editable_text_object_text_set(entry->editable_object, text);
      etk_signal_emit(_etk_entry_signals[ETK_ENTRY_TEXT_CHANGED_SIGNAL], ETK_OBJECT(entry), NULL);
   }
}

/**
 * @brief Gets the text of an entry
 * @param entry an entry
 * @return Returns the text of the entry
 */
const char *etk_entry_text_get(Etk_Entry *entry)
{
   if (!entry || !entry->editable_object)
      return NULL;
   if(entry->is_password)
     return entry->password_text;
   else
     return etk_editable_text_object_text_get(entry->editable_object);
}

/**
 * @brief Turns an entry into a password entry or disables it
 * @param entry an entry
 * @param on wether we want to to be a password entry or not
 */
void etk_entry_password_set(Etk_Entry *entry, Etk_Bool on)
{
   if (!entry)
      return;

   if(entry->is_password == on)
     return;
   
   if(on)
   {
      char *text;
      int   i;
      
      entry->is_password = ETK_TRUE;
      etk_signal_connect("text_changed", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_text_changed_cb), NULL);

      if(!entry->editable_object)
	return;
      
      entry->password_text = strdup(etk_editable_text_object_text_get(entry->editable_object));
      text = calloc(strlen(entry->password_text) + 1, sizeof(char));
      for(i = 0; i < strlen(entry->password_text); i++)
	strncat(text, "*", strlen(entry->password_text));
      etk_editable_text_object_text_set(entry->editable_object, text);
   }
   else
   {
      entry->is_password = ETK_FALSE;
      etk_signal_disconnect("text_changed", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_text_changed_cb));
      if(entry->password_text && entry->editable_object)
      {
	 etk_editable_text_object_text_set(entry->editable_object, entry->password_text);
	 free(entry->password_text);
      }
      
   }
}

/**
 * @brief Gets wether an entry is a password entry
 * @param entry an entry
 * @return Returns true if its a password entry, false otherwise
 */
Etk_Bool etk_entry_password_get(Etk_Entry *entry)
{
   if (!entry || !entry->editable_object)
     return ETK_FALSE;
   
   return entry->is_password;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the entry */
static void _etk_entry_constructor(Etk_Entry *entry)
{
   if (!entry)
      return;

   entry->editable_object = NULL;
   entry->is_password = ETK_FALSE;
   entry->password_text = NULL;
   entry->text = NULL;   
   
   etk_signal_connect("realize", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_unrealize_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_key_down_cb), NULL);
   etk_signal_connect("mouse_in", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_mouse_in_cb), NULL);
   etk_signal_connect("mouse_out", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_mouse_out_cb), NULL);
   etk_signal_connect("focus", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_focus_cb), NULL);
   etk_signal_connect("unfocus", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_unfocus_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_entry_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Entry *entry;
   
   if (!(entry = ETK_ENTRY(object)) || !value)
     return;
   
   switch (property_id)
     {
      case ETK_ENTRY_PASSWORD_PROPERTY:
	etk_entry_password_set(entry, etk_property_value_bool_get(value));
	break;
      default:
	break;
     }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_entry_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Entry *entry;
   
   if (!(entry = ETK_ENTRY(object)) || !value)
     return;
   
   switch (property_id)
     {
      case ETK_ENTRY_PASSWORD_PROPERTY:
	etk_property_value_bool_set(value, etk_entry_password_get(entry));
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

/* Called when the entry is realized */
static void _etk_entry_realize_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;
   Etk_Widget *entry_widget;
   Evas *evas;

   if (!(entry_widget = ETK_WIDGET(object)) || !(evas = etk_widget_toplevel_evas_get(entry_widget)))
      return;

   entry = ETK_ENTRY(entry_widget);
   entry->editable_object = etk_editable_text_object_add(evas);
   if(entry->text != NULL)
   {
      etk_editable_text_object_text_set(entry->editable_object, entry->text);
      free(entry->text);
      entry->text = NULL;
   }
   evas_object_show(entry->editable_object);
   etk_widget_swallow_object(entry_widget, "text_area", entry->editable_object);
}

/* Called when the entry is unrealized */
static void _etk_entry_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)))
      return;
   entry->editable_object = NULL;
}

/* Called when the user presses a key */
static void _etk_entry_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *key_event = event;
   Etk_Entry *entry;
   Etk_Bool text_changed = ETK_FALSE;

   if (!(entry = ETK_ENTRY(object)) || !entry->editable_object)
      return;

   if (strcmp(key_event->key, "BackSpace") == 0)
      text_changed = etk_editable_text_object_delete_char_before(entry->editable_object);
   else if (strcmp(key_event->key, "Delete") == 0)
      text_changed = etk_editable_text_object_delete_char_after(entry->editable_object);
   else if (strcmp(key_event->key, "Left") == 0)
      etk_editable_text_object_cursor_move_left(entry->editable_object);
   else if (strcmp(key_event->key, "Right") == 0)
      etk_editable_text_object_cursor_move_right(entry->editable_object);
   else if (strcmp(key_event->key, "Home") == 0)
      etk_editable_text_object_cursor_move_at_start(entry->editable_object);
   else if (strcmp(key_event->key, "End") == 0)
      etk_editable_text_object_cursor_move_at_end(entry->editable_object);
   else
      text_changed = etk_editable_text_object_insert(entry->editable_object, key_event->string);

   if (text_changed)
      etk_signal_emit(_etk_entry_signals[ETK_ENTRY_TEXT_CHANGED_SIGNAL], object, NULL);
}

/* Called when the mouse enters the entry */
static void _etk_entry_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data)
{
   Etk_Widget *entry_widget;

   if (!(entry_widget = ETK_WIDGET(object)))
      return;
   etk_toplevel_widget_pointer_push(etk_widget_toplevel_parent_get(entry_widget), ETK_POINTER_TEXT_EDIT);
}

/* Called when the mouse leaves the entry */
static void _etk_entry_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data)
{
   Etk_Widget *entry_widget;

   if (!(entry_widget = ETK_WIDGET(object)))
      return;
   etk_toplevel_widget_pointer_pop(entry_widget->toplevel_parent, ETK_POINTER_TEXT_EDIT);
}

/* Called when the entry is focused */
static void _etk_entry_focus_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)) || !entry->editable_object)
      return;
   etk_editable_text_object_cursor_show(entry->editable_object);
}

/* Called when the entry is unfocused */
static void _etk_entry_unfocus_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)) || !entry->editable_object)
      return;
   etk_editable_text_object_cursor_hide(entry->editable_object);
   etk_editable_text_object_cursor_move_at_start(entry->editable_object);
}

/* Called when the entry's text is changed and its a password */
static void _etk_entry_text_changed_cb(Etk_Object *object, void *data)
{
   Etk_Entry       *entry;
   const char      *text;
   char            *text2;
   int              i;
   
   if (!(entry = ETK_ENTRY(object)))
     return;      
   
   if(entry->password_text)
   {
      int size;
      
      text = etk_editable_text_object_text_get(entry->editable_object);
      if(!text) 
	return;
      
      while(*text == '*')
	++text;
      
      if(*text == '\0')
      {
	 int size;
	 
	 /* set the visible text, the *'s */
	 text = etk_editable_text_object_text_get(entry->editable_object);
	 size = strlen(text) + 1;
	 text2 = calloc(size, sizeof(char));
	 snprintf(text2, size, "%s", text);
	 etk_editable_text_object_text_set(entry->editable_object, text2);
	 free(text2);
	 
	 /* save the real text */
	 size = strlen(entry->password_text) + 1;
	 text2 = calloc(size, sizeof(char));
	 snprintf(text2, size - 1, "%s", entry->password_text);
	 free(entry->password_text);	 
	 entry->password_text = text2;
	 
	 return;
      }
      else
      {      
	 size = strlen(text) + strlen(entry->password_text) + 1;
	 text2 = calloc(size, sizeof(char));
	 snprintf(text2, size, "%s%s", entry->password_text, text);
	 free(entry->password_text);
	 entry->password_text = text2;
      }
   }
   else
     entry->password_text = strdup(etk_editable_text_object_text_get(entry->editable_object));

   text = etk_entry_text_get(entry);
   text2 = calloc(strlen(text) + 2, sizeof(char));
   for(i = 0; i < strlen(text); i++)
     strncat(text2, "*", strlen(text));
   etk_editable_text_object_text_set(entry->editable_object, text2);
   
   free(text2);
}

/** @} */
