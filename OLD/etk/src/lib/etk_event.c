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

/** @file etk_event.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_event.h"

#include <stdlib.h>

#include "etk_engine.h"
#include "etk_widget.h"

/**
 * @addtogroup Etk_Event
 * @{
 */

typedef struct Etk_Event_Callback
{
   void (*callback)(Etk_Event_Global event, void *data);
   void *data;
} Etk_Event_Callback;

static void _etk_event_callback_cb(Etk_Event_Type event, Etk_Event_Global event_info);
static void _etk_event_modifiers_wrap(Evas_Modifier *evas_modifiers, Etk_Modifiers *etk_modifiers);
static void _etk_event_locks_wrap(Evas_Lock *evas_locks, Etk_Locks *etk_locks);
static void _etk_event_mouse_flags_wrap(Evas_Button_Flags evas_flags, Etk_Mouse_Flags *etk_flags);

static Eina_List *_etk_event_callbacks[ETK_EVENT_NUM_EVENTS];
static char *_etk_event_empty = "";

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Initializes the event subsystem
 */
void etk_event_init(void)
{
   int i;

   for (i = 0; i < ETK_EVENT_NUM_EVENTS; i++)
      _etk_event_callbacks[i] = NULL;
   etk_engine_event_callback_set(_etk_event_callback_cb);
}

/**
 * @internal
 * @brief Shutdowns the event subsystem
 */
void etk_event_shutdown(void)
{
   int i;

   for (i = 0; i < ETK_EVENT_NUM_EVENTS; i++)
   {
      while (_etk_event_callbacks[i])
      {
         free(_etk_event_callbacks[i]->data);
         _etk_event_callbacks[i] = eina_list_remove_list(_etk_event_callbacks[i], _etk_event_callbacks[i]);
      }
   }

   etk_engine_event_callback_set(NULL);
}

/**
 * @brief Converts a "mouse_in" event from Evas to the corresponding event of Etk
 * @param widget the widget which has received the Evas event
 * @param evas_event the received Evas event
 * @param etk_event the location where to store the converted Etk event
 */
void etk_event_mouse_in_wrap(Etk_Widget *widget, Evas_Event_Mouse_In *evas_event, Etk_Event_Mouse_In *etk_event)
{
   if (!widget || !evas_event || !etk_event)
      return;

   etk_event->buttons = evas_event->buttons;
   etk_event->canvas.x = evas_event->canvas.x;
   etk_event->canvas.y = evas_event->canvas.y;
   etk_event->widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   etk_event->widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   if (evas_event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     etk_event->modifiers |= ETK_MODIFIER_ON_HOLD;
}

/**
 * @brief Converts a "mouse_out" event from Evas to the corresponding event of Etk
 * @param widget the widget which has received the Evas event
 * @param evas_event the received Evas event
 * @param etk_event the location where to store the converted Etk event
 */
void etk_event_mouse_out_wrap(Etk_Widget *widget, Evas_Event_Mouse_Out *evas_event, Etk_Event_Mouse_Out *etk_event)
{
   if (!widget || !evas_event || !etk_event)
      return;

   etk_event->buttons = evas_event->buttons;
   etk_event->canvas.x = evas_event->canvas.x;
   etk_event->canvas.y = evas_event->canvas.y;
   etk_event->widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   etk_event->widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   if (evas_event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     etk_event->modifiers |= ETK_MODIFIER_ON_HOLD;
}

/**
 * @brief Converts a "mouse_move" event from Evas to the corresponding event of Etk
 * @param widget the widget which has received the Evas event
 * @param evas_event the received Evas event
 * @param etk_event the location where to store the converted Etk event
 */
void etk_event_mouse_move_wrap(Etk_Widget *widget, Evas_Event_Mouse_Move *evas_event, Etk_Event_Mouse_Move *etk_event)
{
   if (!widget || !evas_event || !etk_event)
      return;

   etk_event->buttons = evas_event->buttons;
   etk_event->cur.canvas.x = evas_event->cur.canvas.x;
   etk_event->cur.canvas.y = evas_event->cur.canvas.y;
   etk_event->cur.widget.x = evas_event->cur.canvas.x - widget->inner_geometry.x;
   etk_event->cur.widget.y = evas_event->cur.canvas.y - widget->inner_geometry.y;
   etk_event->prev.canvas.x = evas_event->prev.canvas.x;
   etk_event->prev.canvas.y = evas_event->prev.canvas.y;
   etk_event->prev.widget.x = evas_event->prev.canvas.x - widget->inner_geometry.x;
   etk_event->prev.widget.y = evas_event->prev.canvas.y - widget->inner_geometry.y;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   if (evas_event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     etk_event->modifiers |= ETK_MODIFIER_ON_HOLD;
}

/**
 * @brief Converts a "mouse_down" event from Evas to the corresponding event of Etk
 * @param widget the widget which has received the Evas event
 * @param evas_event the received Evas event
 * @param etk_event the location where to store the converted Etk event
 */
void etk_event_mouse_down_wrap(Etk_Widget *widget, Evas_Event_Mouse_Down *evas_event, Etk_Event_Mouse_Down *etk_event)
{
   if (!widget || !evas_event || !etk_event)
      return;

   etk_event->button = evas_event->button;
   etk_event->canvas.x = evas_event->canvas.x;
   etk_event->canvas.y = evas_event->canvas.y;
   etk_event->widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   etk_event->widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   _etk_event_mouse_flags_wrap(evas_event->flags, &etk_event->flags);
   if (evas_event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     etk_event->modifiers |= ETK_MODIFIER_ON_HOLD;
}

/**
 * @brief Converts a "mouse_up" event from Evas to the corresponding event of Etk
 * @param widget the widget which has received the Evas event
 * @param evas_event the received Evas event
 * @param etk_event the location where to store the converted Etk event
 */
void etk_event_mouse_up_wrap(Etk_Widget *widget, Evas_Event_Mouse_Up *evas_event, Etk_Event_Mouse_Up *etk_event)
{
   if (!widget || !evas_event || !etk_event)
      return;

   etk_event->button = evas_event->button;
   etk_event->canvas.x = evas_event->canvas.x;
   etk_event->canvas.y = evas_event->canvas.y;
   etk_event->widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   etk_event->widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   _etk_event_mouse_flags_wrap(evas_event->flags, &etk_event->flags);
   if (evas_event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     etk_event->modifiers |= ETK_MODIFIER_ON_HOLD;
}

/**
 * @brief Converts a "mouse_wheel" event from Evas to the corresponding event of Etk
 * @param widget the widget which has received the Evas event
 * @param evas_event the received Evas event
 * @param etk_event the location where to store the converted Etk event
 */
void etk_event_mouse_wheel_wrap(Etk_Widget *widget, Evas_Event_Mouse_Wheel *evas_event, Etk_Event_Mouse_Wheel *etk_event)
{
   if (!widget || !evas_event || !etk_event)
      return;

   etk_event->direction = (evas_event->direction == 0) ? ETK_WHEEL_VERTICAL : ETK_WHEEL_HORIZONTAL;
   etk_event->z = evas_event->z;
   etk_event->canvas.x = evas_event->canvas.x;
   etk_event->canvas.y = evas_event->canvas.y;
   etk_event->widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   etk_event->widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   etk_event->timestamp = evas_event->timestamp;
   if (evas_event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     etk_event->modifiers |= ETK_MODIFIER_ON_HOLD;
}

/**
 * @brief Converts a "key_down" event from Evas to the corresponding event of Etk
 * @param widget the widget which has received the Evas event
 * @param evas_event the received Evas event
 * @param etk_event the location where to store the converted Etk event
 */
void etk_event_key_down_wrap(Etk_Widget *widget, Evas_Event_Key_Down *evas_event, Etk_Event_Key_Down *etk_event)
{
   etk_event->keyname = evas_event->keyname ? evas_event->keyname : _etk_event_empty;
   etk_event->key = evas_event->key ? evas_event->key : _etk_event_empty;
   etk_event->string = evas_event->string ? evas_event->string : _etk_event_empty;
   etk_event->compose = evas_event->compose ? evas_event->compose : _etk_event_empty;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   if (evas_event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     etk_event->modifiers |= ETK_MODIFIER_ON_HOLD;
}

/**
 * @brief Converts a "key_up" event from Evas to the corresponding event of Etk
 * @param widget the widget which has received the Evas event
 * @param evas_event the received Evas event
 * @param etk_event the location where to store the converted Etk event
 */
void etk_event_key_up_wrap(Etk_Widget *widget, Evas_Event_Key_Up *evas_event, Etk_Event_Key_Up *etk_event)
{
   etk_event->keyname = evas_event->keyname ? evas_event->keyname : _etk_event_empty;
   etk_event->key = evas_event->key ? evas_event->key : _etk_event_empty;
   etk_event->string = evas_event->string ? evas_event->string : _etk_event_empty;
   etk_event->compose = evas_event->compose ? evas_event->compose : _etk_event_empty;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   if (evas_event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     etk_event->modifiers |= ETK_MODIFIER_ON_HOLD;
}

/**
 * @brief Adds a function to call when the given input event is emitted. It receives all the events,
 * even those that are not sent to an Etk window
 * @param event the type of event to monitor
 * @param callback the callback to call when a corresponding event is emitted
 * @param data the data to pass to the callback
 */
void etk_event_global_callback_add(Etk_Event_Type event, void (*callback)(Etk_Event_Global event, void *data), void *data)
{
   Etk_Event_Callback *cb;

   if (!callback)
      return;

   cb = malloc(sizeof(Etk_Event_Callback));
   cb->callback = callback;
   cb->data = data;
   _etk_event_callbacks[event] = eina_list_append(_etk_event_callbacks[event], cb);
}

/**
 * @brief Removes a callback connected to the given given input event
 * @param event the type of event to remove
 * @param callback the callback to remove
 */
void etk_event_global_callback_del(Etk_Event_Type event, void (*callback)(Etk_Event_Global event, void *data))
{
   Etk_Event_Callback *cb;
   Eina_List *l;

   if (!callback)
      return;

   for (l = _etk_event_callbacks[event]; l; l = l->next)
   {
      cb = l->data;
      if (cb->callback == callback)
      {
         _etk_event_callbacks[event] = eina_list_remove_list(_etk_event_callbacks[event], l);
         free(cb);
         return;
      }
   }
}

/**
 * @brief Gets the position of the mouse pointer, relative to the screen
 * @param x the location where to store the x position of the mouse
 * @param y the location where to store the y position of the mouse
 */
void etk_event_mouse_position_get(int *x, int *y)
{
   etk_engine_mouse_position_get(x, y);
}

/**************************
 *
 * Callbacks
 *
 **************************/

/* Called when an input event is emitted */
static void _etk_event_callback_cb(Etk_Event_Type event, Etk_Event_Global event_info)
{
   Eina_List *callbacks, *l;
   Etk_Event_Callback *callback;

   /* We make a copy of the callbacks list to avoid potential
    * problems if a callback removes itself from the list */
   callbacks = NULL;
   for (l = _etk_event_callbacks[event]; l; l = l->next)
      callbacks = eina_list_append(callbacks, l->data);

   for (l = callbacks; l; l = l->next)
   {
      callback = l->data;
      callback->callback(event_info, callback->data);
   }

   eina_list_free(callbacks);
}

/**************************
 *
 * Private function
 *
 **************************/

/* Converts the Evas modifiers to Etk modifiers */
static void _etk_event_modifiers_wrap(Evas_Modifier *evas_modifiers, Etk_Modifiers *etk_modifiers)
{
   if (!evas_modifiers || !etk_modifiers)
      return;

   *etk_modifiers = ETK_MODIFIER_NONE;
   if (evas_key_modifier_is_set(evas_modifiers, "Control"))
      *etk_modifiers |= ETK_MODIFIER_CTRL;
   if (evas_key_modifier_is_set(evas_modifiers, "Alt"))
      *etk_modifiers |= ETK_MODIFIER_ALT;
   if (evas_key_modifier_is_set(evas_modifiers, "Shift"))
      *etk_modifiers |= ETK_MODIFIER_SHIFT;
   if (evas_key_modifier_is_set(evas_modifiers, "Super") || evas_key_modifier_is_set(evas_modifiers, "Hyper"))
      *etk_modifiers |= ETK_MODIFIER_WIN;
}

/* Converts the Evas locks to Etk locks */
static void _etk_event_locks_wrap(Evas_Lock *evas_locks, Etk_Locks *etk_locks)
{
   if (!evas_locks || !etk_locks)
      return;

   *etk_locks = ETK_LOCK_NONE;
   if (evas_key_lock_is_set(evas_locks, "Num_Lock"))
      *etk_locks |= ETK_LOCK_NUM;
   if (evas_key_lock_is_set(evas_locks, "Caps_Lock"))
      *etk_locks |= ETK_LOCK_CAPS;
   if (evas_key_lock_is_set(evas_locks, "Scroll_Lock"))
      *etk_locks |= ETK_LOCK_SCROLL;
}

/* Converts the Evas mouse flags to Etk mouse flags */
static void _etk_event_mouse_flags_wrap(Evas_Button_Flags evas_flags, Etk_Mouse_Flags *etk_flags)
{
   if (!etk_flags)
      return;

   *etk_flags = ETK_MOUSE_NONE;
   if (evas_flags & EVAS_BUTTON_DOUBLE_CLICK)
      *etk_flags |= ETK_MOUSE_DOUBLE_CLICK;
   if (evas_flags & EVAS_BUTTON_TRIPLE_CLICK)
      *etk_flags |= ETK_MOUSE_TRIPLE_CLICK;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Event
 *
 * There are two kinds of events in Etk:
 *   - The widget-events that are received by every widget: for example, when a button is clicked, when the mouse wheel
 * is used over a widget... @n
 * You can connect a callback to one of these events by calling etk_signal_connect() on
 * the widget with the given signal. For example:
 * @code
 * //_image_clicked_cb() will be called when the image is clicked
 * etk_signal_connect(ETK_OBJECT(image), "mouse-clicked", _image_clicked_cb, NULL);
 * @endcode
 *   - The global-events that are emitted each time an input event occurs, even if the event was not for a widget: for
 * example, when the user clicks somewhere on the screen, when he moves the mouse... You will rarely have to use these
 * events. @n
 * You can connect a callback to this kind of events with the function etk_event_global_callback_add(). For example:
 * @code
 * //_mouse_down_cb() will be called each time the user presses the mouse
 * etk_event_global_callback_add(ETK_EVENT_MOUSE_DOWN, _mouse_down_cb, NULL);
 * @endcode
 */
