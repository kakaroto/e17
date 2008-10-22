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

/** @file etk_statusbar.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_statusbar.h"

#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_event.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_toplevel.h"
#include "etk_utils.h"
#include "etk_window.h"

/**
 * @addtogroup Etk_Statusbar
 * @{
 */

typedef struct Etk_Statusbar_Msg
{
   char *msg;
   int context_id;
   int message_id;
} Etk_Statusbar_Msg;

enum Etk_Statusbar_Property_Id
{
   ETK_STATUSBAR_CURRENT_MESSAGE_PROPERTY,
   ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY
};

static void _etk_statusbar_constructor(Etk_Statusbar *statusbar);
static void _etk_statusbar_destructor(Etk_Statusbar *statusbar);
static void _etk_statusbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_statusbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_statusbar_realized_cb(Etk_Object *object, void *data);
static void _etk_statusbar_resize_grip_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static Etk_Bool _etk_statusbar_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data);
static void _etk_statusbar_update(Etk_Statusbar *statusbar);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Statusbar
 * @return Returns the type of an Etk_Statusbar
 */
Etk_Type *etk_statusbar_type_get(void)
{
   static Etk_Type *statusbar_type = NULL;

   if (!statusbar_type)
   {
      statusbar_type = etk_type_new("Etk_Statusbar", ETK_WIDGET_TYPE, sizeof(Etk_Statusbar),
         ETK_CONSTRUCTOR(_etk_statusbar_constructor), ETK_DESTRUCTOR(_etk_statusbar_destructor), NULL);

      etk_type_property_add(statusbar_type, "current-message", ETK_STATUSBAR_CURRENT_MESSAGE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE, NULL);
      etk_type_property_add(statusbar_type, "has-resize-grip", ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));

      statusbar_type->property_set = _etk_statusbar_property_set;
      statusbar_type->property_get = _etk_statusbar_property_get;
   }

   return statusbar_type;
}

/**
 * @brief Creates a new statusbar
 * @return Returns the new statusbar widget
 */
Etk_Widget *etk_statusbar_new(void)
{
   return etk_widget_new(ETK_STATUSBAR_TYPE, "theme-group", "statusbar", NULL);
}

/**
 * @brief Gets a context-id corresponding to the context description
 * @param statusbar a statusbar
 * @param context the description of the context
 * @return Returns the context-id corresponding to the context description, or -1 on failure
 */
int etk_statusbar_context_id_get(Etk_Statusbar *statusbar, const char *context)
{
   char *key;
   int *context_id;

   if (!statusbar || !context)
      return -1;

   key = malloc(strlen("_Etk_Statusbar::") + strlen(context) + 1);
   sprintf(key, "_Etk_Statusbar::%s", context);

   if (!(context_id = etk_object_data_get(ETK_OBJECT(statusbar), key)))
   {
      context_id = malloc(sizeof(int));
      *context_id = statusbar->next_context_id++;
      etk_object_data_set_full(ETK_OBJECT(statusbar), key, context_id, free);
   }

   free(key);
   return *context_id;
}

/**
 * @brief Pushs a new message on the statusbar's message-stack
 * @param statusbar a statusbar
 * @param message the message to push
 * @param context_id the context-id to associate to the message.
 * You can generate a context-id with @a etk_statusbar_context_id_get()
 * @return Returns the message-id of the message, or -1 on failure
 */
int etk_statusbar_message_push(Etk_Statusbar *statusbar, const char *message, int context_id)
{
   Etk_Statusbar_Msg *new_msg;

   if (!statusbar || !message)
      return -1;

   new_msg = malloc(sizeof(Etk_Statusbar_Msg));
   new_msg->msg = strdup(message);
   new_msg->context_id = context_id;
   new_msg->message_id = statusbar->next_message_id++;

   statusbar->msg_stack = eina_list_prepend(statusbar->msg_stack, new_msg);
   _etk_statusbar_update(statusbar);

   etk_object_notify(ETK_OBJECT(statusbar), "current-message");
   return new_msg->message_id;
}

/**
 * @brief Pops from the statusbar's message-stack the first message whose context-id matchs
 * @param statusbar a statusbar
 * @param context_id the context-id of the message to pop
 */
void etk_statusbar_message_pop(Etk_Statusbar *statusbar, int context_id)
{
   Eina_List *l;
   Etk_Statusbar_Msg *m;

   if (!statusbar)
      return;

   for (l = statusbar->msg_stack; l; l = l->next)
   {
      m = l->data;
      if (m->context_id == context_id)
      {
         free(m->msg);
         free(m);
         statusbar->msg_stack = eina_list_remove_list(statusbar->msg_stack, l);
         _etk_statusbar_update(statusbar);
         break;
      }
   }

   etk_object_notify(ETK_OBJECT(statusbar), "current-message");
}

/**
 * @brief Removes the message corresponding to the message-id from the statusbar
 * @param statusbar a statusbar
 * @param message_id the message-id of the message to remove. It has been returned
 * when you pushed the message with @a etk_statusbar_push()
 */
void etk_statusbar_message_remove(Etk_Statusbar *statusbar, int message_id)
{
   Eina_List *l;
   Etk_Statusbar_Msg *m;

   if (!statusbar)
      return;

   for (l = statusbar->msg_stack; l; l = l->next)
   {
      m = l->data;
      if (m->message_id == message_id)
      {
         free(m->msg);
         free(m);
         statusbar->msg_stack = eina_list_remove_list(statusbar->msg_stack, l);
         _etk_statusbar_update(statusbar);
         etk_object_notify(ETK_OBJECT(statusbar), "current-message");
         break;
      }
   }
}

/**
 * @brief Gets the message currently displayed by the statusbar
 * @param statusbar a statusbar
 * @param message the location where to store the message
 * @param message_id the location where to store the id of the message
 * @param context_id the location where to store the context-id of the message
 */
void etk_statusbar_message_get(Etk_Statusbar *statusbar, const char **message, int *message_id, int *context_id)
{
   Etk_Statusbar_Msg *m;

   if (message)     *message = NULL;
   if (message_id)  *message_id = 0;
   if (context_id)  *context_id = 0;

   if (!statusbar)
      return;

   if (statusbar->msg_stack && (m = statusbar->msg_stack->data))
   {
      if (message)     *message = m->msg;
      if (message_id)  *message_id = m->message_id;
      if (context_id)  *context_id = m->context_id;
   }
}

/**
 * @brief Sets whether the statusbar has a resize-grip. The resize-grip is a small grip at the right of the statusbar
 * that the user can use to resize the window
 * @param statusbar a statusbar
 * @param has_resize_grip if @a has_resize_grip is ETK_TRUE to make the statusbar have a resize-grip
 */
void etk_statusbar_has_resize_grip_set(Etk_Statusbar *statusbar, Etk_Bool has_resize_grip)
{
   if (!statusbar || statusbar->has_resize_grip == has_resize_grip)
      return;

   statusbar->has_resize_grip = has_resize_grip;
   if (statusbar->has_resize_grip)
      etk_widget_theme_signal_emit(ETK_WIDGET(statusbar), "etk,action,show,resize_grip", ETK_TRUE);
   else
      etk_widget_theme_signal_emit(ETK_WIDGET(statusbar), "etk,action,hide,resize_grip", ETK_TRUE);
   etk_object_notify(ETK_OBJECT(statusbar), "has-resize-grip");
}

/**
 * @brief Gets whether the statusbar has a resize-grip
 * @param statusbar a statusbar
 * @return Returns ETK_TRUE if the statusbar has a resize-grip, ETK_FALSE otherwise
 */
Etk_Bool etk_statusbar_has_resize_grip_get(Etk_Statusbar *statusbar)
{
   if (!statusbar)
      return ETK_FALSE;
   return statusbar->has_resize_grip;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_statusbar_constructor(Etk_Statusbar *statusbar)
{
   if (!statusbar)
      return;

   statusbar->has_resize_grip = ETK_TRUE;
   statusbar->msg_stack = NULL;
   statusbar->next_message_id = 0;
   statusbar->next_context_id = 0;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(statusbar), ETK_CALLBACK(_etk_statusbar_realized_cb), NULL);
}

/* Destroys the statusbar */
static void _etk_statusbar_destructor(Etk_Statusbar *statusbar)
{
   Etk_Statusbar_Msg *msg;

   if (!statusbar)
      return;

   while (statusbar->msg_stack)
   {
      msg = statusbar->msg_stack->data;
      free(msg->msg);
      free(msg);
      statusbar->msg_stack = eina_list_remove_list(statusbar->msg_stack, statusbar->msg_stack);
   }
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_statusbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Statusbar *statusbar;

   if (!(statusbar = ETK_STATUSBAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY:
         etk_statusbar_has_resize_grip_set(statusbar, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_statusbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Statusbar *statusbar;

   if (!(statusbar = ETK_STATUSBAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_STATUSBAR_CURRENT_MESSAGE_PROPERTY:
      {
         const char *message;

         etk_statusbar_message_get(statusbar, &message, NULL, NULL);
         etk_property_value_string_set(value, message);
         break;
      }
      case ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY:
         etk_property_value_bool_set(value, statusbar->has_resize_grip);
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

/* Called when the statusbar is realized */
static Etk_Bool _etk_statusbar_realized_cb(Etk_Object *object, void *data)
{
   Etk_Statusbar *statusbar;
   Etk_Widget *statusbar_widget;

   if (!(statusbar = ETK_STATUSBAR(object)))
      return ETK_TRUE;
   statusbar_widget = ETK_WIDGET(statusbar);

   if (statusbar->has_resize_grip)
      etk_widget_theme_signal_emit(statusbar_widget, "etk,action,show,resize_grip", ETK_TRUE);
   else
      etk_widget_theme_signal_emit(statusbar_widget, "etk,action,hide,resize_grip", ETK_TRUE);
   _etk_statusbar_update(statusbar);

   if (statusbar_widget->theme_object)
   {
      edje_object_signal_callback_add(statusbar_widget->theme_object, "*", "etk.event.resize",
         _etk_statusbar_resize_grip_cb, object);
   }

   return ETK_TRUE;
}

/* Called when an event occurs on the resize grip of the statusbar */
static void _etk_statusbar_resize_grip_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Statusbar *statusbar;

   if (!(statusbar = ETK_STATUSBAR(data)))
      return;

   if (strcmp(emission, "mouse,in") == 0)
      etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(ETK_WIDGET(statusbar)), ETK_POINTER_RESIZE_BR);
   else if (strcmp(emission, "mouse,out") == 0)
      etk_toplevel_pointer_pop(etk_widget_toplevel_parent_get(ETK_WIDGET(statusbar)), ETK_POINTER_RESIZE_BR);
   else if (strcmp(emission, "mouse,down,1") == 0)
   {
      Etk_Toplevel *window;

      if (!(window = etk_widget_toplevel_parent_get(ETK_WIDGET(statusbar))) || !ETK_IS_WINDOW(window))
         return;

      etk_window_geometry_get(ETK_WINDOW(window), NULL, NULL, &statusbar->window_width, &statusbar->window_height);
      etk_signal_connect_by_code(ETK_WIDGET_MOUSE_MOVE_SIGNAL, ETK_OBJECT(statusbar), ETK_CALLBACK(_etk_statusbar_mouse_move_cb), NULL);
   }
   else if (strcmp(emission, "mouse,up,1") == 0)
      etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_MOVE_SIGNAL, ETK_OBJECT(statusbar), ETK_CALLBACK(_etk_statusbar_mouse_move_cb), NULL);
}

/* Called when mouse presses the resize grip and when the mouse is moved */
static Etk_Bool _etk_statusbar_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data)
{
   Etk_Statusbar *statusbar;
   Etk_Toplevel *window;

   if (!(statusbar = ETK_STATUSBAR(object)))
      return ETK_TRUE;
   if (!(window = etk_widget_toplevel_parent_get(ETK_WIDGET(statusbar))) || !ETK_IS_WINDOW(window))
      return ETK_TRUE;

   statusbar->window_width += event->cur.widget.x - event->prev.widget.x;
   statusbar->window_height += event->cur.widget.y - event->prev.widget.y;
   etk_window_resize(ETK_WINDOW(window), statusbar->window_width, statusbar->window_height);
   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Updates the messages displayed in the statusbar */
static void _etk_statusbar_update(Etk_Statusbar *statusbar)
{
   Etk_Statusbar_Msg *msg;

   if (!statusbar)
      return;

   if (statusbar->msg_stack && (msg = statusbar->msg_stack->data) && msg->msg)
      etk_widget_theme_part_text_set(ETK_WIDGET(statusbar), "etk.text.message", msg->msg);
   else
      etk_widget_theme_part_text_set(ETK_WIDGET(statusbar), "etk.text.message", "");
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Statusbar
 *
 * @image html widgets/statusbar.png
 * The statusbar is usually placed at the bottom of a window, and can also have a resize-grip (see
 * etk_statusbar_has_resize_grip_set()) which the user can use to resize the window. @n
 *
 * The statusbar has an internal stack of messages, and always displays the message on the top of the stack. @n
 * To display a message, you need to push it on the stack with etk_statusbar_message_push(),
 * and to hide it, you need to pop it from the stack with etk_statusbar_message_pop(). @n
 *
 * Each message is also associated to a context-id corresponding to the source of the message. A context-id can be
 * generated from a description of the context, with etk_statusbar_context_id_get(). @n
 *
 * You can also remove a specific message from its message-id (returned by etk_statusbar_message_push()) using
 * etk_statusbar_message_remove(). @n
 *
 * Finally, you can get the message currently displayed with etk_statusbar_message_get(). @n
 *
 * Here is a simple example showing how to use a statusbar:
 * @code
 * //Push a message when a menu item is selected. For example:
 * etk_statusbar_message_push(statusbar, "Paste", etk_statusbar_context_id_get("Menu"));
 *
 * //Push a message to inform the user of the progress of the current operation
 * etk_statusbar_message_push(statusbar, "Looking for the files...", etk_statusbar_context_id_get("Progress"));
 *
 * //Pop the message from the menu, when the menu item is unselected
 * //Note: it won't change the current message of the statusbar since "Paste" is not on the top of the message-stack
 * etk_statusbar_message_pop(statusbar, etk_statusbar_context_id_get("Menu"));
 * @endcode @n @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Statusbar
 *
 * \par Properties:
 * @prop_name "current-message": The message currently displayed in the statusbar
 * @prop_type String (char *)
 * @prop_ro
 * @prop_val NULL
 * \par
 * @prop_name "has-resize-grip": Whether or not the resize-grip of the statusbar is visible
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE;
 */
