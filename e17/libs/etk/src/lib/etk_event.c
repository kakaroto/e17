/** @file etk_event->c */
#include "etk_event.h"
#include <stdlib.h>
#include "etk_widget.h"

/**
 * @addtogroup Etk_Event
 * @{
 */

static void _etk_event_modifiers_wrap(Evas_Modifier *evas_modifiers, Etk_Modifiers *etk_modifiers);
static void _etk_event_locks_wrap(Evas_Lock *evas_locks, Etk_Locks *etk_locks);
static void _etk_event_mouse_flags_wrap(Evas_Button_Flags evas_flags, Etk_Mouse_Flags *etk_flags);

static char *_etk_event_empty = "";

/**************************
 *
 * Implementation
 *
 **************************/

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
   
   etk_event->direction = evas_event->direction;
   etk_event->z = evas_event->z;
   etk_event->canvas.x = evas_event->canvas.x;
   etk_event->canvas.y = evas_event->canvas.y;
   etk_event->widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   etk_event->widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   etk_event->timestamp = evas_event->timestamp;
   _etk_event_modifiers_wrap(evas_event->modifiers, &etk_event->modifiers);
   _etk_event_locks_wrap(evas_event->locks, &etk_event->locks);
   etk_event->timestamp = evas_event->timestamp;
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
