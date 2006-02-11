/** @file etk_entry.c */
#include "etk_entry.h"
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

static void _etk_entry_constructor(Etk_Entry *entry);
static void _etk_entry_realize_cb(Etk_Object *object, void *data);
static void _etk_entry_unrealize_cb(Etk_Object *object, void *data);
static void _etk_entry_key_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_entry_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_entry_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_entry_focus_cb(Etk_Object *object, void *data);
static void _etk_entry_unfocus_cb(Etk_Object *object, void *data);

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
   }

   return entry_type;
}

/**
 * @brief Creates a new entry
 * @return Returns the new entry widget
 */
Etk_Widget *etk_entry_new()
{
   return etk_widget_new(ETK_ENTRY_TYPE, "theme_group", "entry", "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
}

/**
 * @brief Sets the text of an entry
 * @param entry an entry
 * @param text the text to set
 */
void etk_entry_text_set(Etk_Entry *entry, const char *text)
{
   if (!entry || !entry->editable_object)
      return;
   etk_editable_text_object_text_set(entry->editable_object, text);
   etk_signal_emit(_etk_entry_signals[ETK_ENTRY_TEXT_CHANGED_SIGNAL], ETK_OBJECT(entry), NULL);
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
   return etk_editable_text_object_text_get(entry->editable_object);
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

   etk_signal_connect("realize", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_unrealize_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_key_down_cb), NULL);
   etk_signal_connect("mouse_in", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_mouse_in_cb), NULL);
   etk_signal_connect("mouse_out", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_mouse_out_cb), NULL);
   etk_signal_connect("focus", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_focus_cb), NULL);
   etk_signal_connect("unfocus", ETK_OBJECT(entry), ETK_CALLBACK(_etk_entry_unfocus_cb), NULL);
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
   evas_object_show(entry->editable_object);
   etk_widget_theme_object_swallow(entry_widget, "text_area", entry->editable_object);
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

/** @} */
