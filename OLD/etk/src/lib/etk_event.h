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

/** @file etk_event.h */
#ifndef _ETK_EVENT_H_
#define _ETK_EVENT_H_

#include <Evas.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Event The event system of Etk
 * @brief The event system of Etk
 * @{
 */


/** @brief The different types of input events */
typedef enum
{
   ETK_EVENT_MOUSE_MOVE,         /**< Emitted when the mouse is moved */
   ETK_EVENT_MOUSE_DOWN,         /**< Emitted when a mouse button is pressed */
   ETK_EVENT_MOUSE_UP,           /**< Emitted when a mouse button is released */
   ETK_EVENT_MOUSE_WHEEL,        /**< Emitted when the mouse wheel used */
   ETK_EVENT_KEY_DOWN,           /**< Emitted when a key of the keyboard is pressed */
   ETK_EVENT_KEY_UP,             /**< Emitted when a key of the keyboard is released */
   ETK_EVENT_NUM_EVENTS
} Etk_Event_Type;

/** @brief The keyboard modifiers active when the event has been emitted */
typedef enum
{
   ETK_MODIFIER_NONE = 0,               /**< No active modifiers */
   ETK_MODIFIER_CTRL = 1 << 0,          /**< "Control" is pressed */
   ETK_MODIFIER_ALT = 1 << 1,           /**< "Alt" is pressed */
   ETK_MODIFIER_SHIFT = 1 << 2,         /**< "Shift" is pressed */
   ETK_MODIFIER_WIN = 1 << 3,           /**< "Win" (between "Ctrl" and "Alt") is pressed */
   ETK_MODIFIER_ON_HOLD = 1 << 31       /**< When the event is to be "on hold" and is informative only */
} Etk_Modifiers;

/** @brief The keyboard locks active when the event has been emitted */
typedef enum
{
   ETK_LOCK_NONE = 0,                   /**< No locks are active */
   ETK_LOCK_NUM = 1 << 0,               /**< "Num" lock is active */
   ETK_LOCK_CAPS = 1 << 1,              /**< "Caps" lock is active */
   ETK_LOCK_SCROLL = 1 << 2             /**< "Scroll" lock is active */
} Etk_Locks;

/** @brief A flag describing whether the click was a single, double or triple click */
typedef enum
{
   ETK_MOUSE_NONE = 0,                  /**< A single click */
   ETK_MOUSE_DOUBLE_CLICK = 1 << 0,     /**< A double click */
   ETK_MOUSE_TRIPLE_CLICK = 1 << 1      /**< A triple click */
} Etk_Mouse_Flags;

/** @brief The scroll direction corresponding to the wheel event */
typedef enum
{
   ETK_WHEEL_VERTICAL,                  /**< Vertical scrolling */
   ETK_WHEEL_HORIZONTAL                 /**< Horizontal scrolling */
} Etk_Wheel_Direction;


/**
 * @brief The event structure sent to "global" callback (see etk_event_global_callback_add())
 * @structinfo
 */
union Etk_Event_Global
{
   struct
   {
      Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
      Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
      Etk_Position pos;            /**< The position of the mouse pointer */
      unsigned int timestamp;      /**< The timestamp when the event occured */
   } mouse_move;                   /**< The structure of a "mouse move" global event */

   struct
   {
      int button;                  /**< The button which has been pressed/released */
      Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
      Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
      Etk_Mouse_Flags flags;       /**< The flags corresponding the mouse click (single, double or triple click) */
      Etk_Position pos;            /**< The position of the mouse pointer */
      unsigned int timestamp;      /**< The timestamp when the event occured */
   } mouse_down,                   /**< The structure of a "mouse down" global event */
     mouse_up;                     /**< The structure of a "mouse up" global event */

   struct
   {
      Etk_Wheel_Direction direction;   /**< The direction of the mouse wheel event */
      int z;                       /**< ...,-2,-1 = down; 1,2,... = up */
      Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
      Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
      Etk_Position pos;            /**< The position of the mouse pointer */
      unsigned int timestamp;      /**< The timestamp when the event occured */
   } mouse_wheel;                  /**< The structure of a "mouse wheel" global event */

   struct
   {
      char *keyname;               /**< A name corresponding to the pressed/released key */
      Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
      Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */

      const char *key;             /**< A name corresponding to the pressed/released key (unlike @a keyname, it
                                    * takes the active modifiers into account */
      const char *string;          /**< The UTF-8 string corresponding to the pressed/released key
                                    * (string to insert in an entry for example) */
      unsigned int timestamp;      /**< The timestamp when the event occured */
   } key_down,                     /**< The structure of a "key down" global event */
     key_up;                       /**< The structure of a "key up" global event */
};

/**
 * @brief The event structure sent when the mouse enters a widget
 * @structinfo
 */
struct Etk_Event_Mouse_In
{
   int buttons;                 /**< Bits describing the buttons pressed when the event has been emitted
                                 * (0x0000001 for the first button, 0x00000010 for the second button, ...) */
   Etk_Position canvas;         /**< The position of the mouse pointer, relative to the Evas canvas */
   Etk_Position widget;         /**< The position of the mouse pointer, relative to the widget */
   Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
   Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
   unsigned int timestamp;      /**< The timestamp when the event occured */
};

/**
 * @brief The event structure sent when the mouse leaves a widget
 * @structinfo
 */
struct Etk_Event_Mouse_Out
{
   int buttons;                 /**< Bits describing the buttons pressed when the event has been emitted
                                 * (0x0000001 for the first button, 0x00000010 for the second button, ...) */
   Etk_Position canvas;         /**< The position of the mouse pointer, relative to the Evas canvas */
   Etk_Position widget;         /**< The position of the mouse pointer, relative to the widget */
   Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
   Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
   unsigned int timestamp;      /**< The timestamp when the event occured */
};

/**
 * @brief The event structure sent when the mouse moves over a widget
 * @structinfo
 */
struct Etk_Event_Mouse_Move
{
   int buttons;                 /**< Bits describing the buttons pressed when the event has been emitted
                                 * (0x0000001 for the first button, 0x00000010 for the second button, ...) */
   struct
   {
      Etk_Position canvas;      /**< The position of the mouse pointer, relative to the Evas canvas */
      Etk_Position widget;      /**< The position of the mouse pointer, relative to the widget */
   } cur,                       /**< The current position of the mouse pointer */
     prev;                      /**< The previous position of the mouse pointer */
   Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
   Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
   unsigned int timestamp;      /**< The timestamp when the event occured */
};

/**
 * @brief The event structure sent when the user presses a widget with the mouse
 * @structinfo
 */
struct Etk_Event_Mouse_Down
{
   int button;                  /**< The button which has been pressed */
   Etk_Position canvas;         /**< The position of the mouse pointer, relative to the Evas canvas */
   Etk_Position widget;         /**< The position of the mouse pointer, relative to the widget */
   Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
   Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
   Etk_Mouse_Flags flags;       /**< The flags corresponding the mouse click (single, double or triple click) */
   unsigned int timestamp;      /**< The timestamp when the event occured */
};

/**
 * @brief The event structure sent when the user releases a widget with the mouse
 * @structinfo
 */
struct Etk_Event_Mouse_Up
{
   int button;                  /**< The button which has been pressed */
   Etk_Position canvas;         /**< The position of the mouse pointer, relative to the Evas canvas */
   Etk_Position widget;         /**< The position of the mouse pointer, relative to the widget */
   Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
   Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
   Etk_Mouse_Flags flags;       /**< The flags corresponding the mouse click (single, double or triple click) */
   unsigned int timestamp;      /**< The timestamp when the event occured */
};

/**
 * @brief The event structure sent when the mouse wheel is used over a widget
 * @structinfo
 */
struct Etk_Event_Mouse_Wheel
{
   Etk_Wheel_Direction direction;   /**< The direction of the mouse wheel event */
   int z;                       /**< ...,-2,-1 = down; 1,2,... = up */
   Etk_Position canvas;         /**< The position of the mouse pointer, relative to the Evas canvas */
   Etk_Position widget;         /**< The position of the mouse pointer, relative to the widget */
   Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
   Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */
   unsigned int timestamp;      /**< The timestamp when the event occured */
};

/**
 * @brief The event structure sent to the focused widget when a key is pressed
 * @structinfo
 */
struct Etk_Event_Key_Down
{
   char *keyname;               /**< A name corresponding to the pressed key */
   Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
   Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */

   const char *key;             /**< A name corresponding to the pressed key (unlike @a keyname, it takes the
                                 * active modifiers into account */
   const char *string;          /**< The UTF-8 string corresponding to the pressed key
                                 * (string to insert in an entry for example) */
   const char *compose;         /**< Unused */
   unsigned int timestamp;      /**< The timestamp when the event occured */
};

/**
 * @brief The event structure sent to the focused widget when a key is released
 * @structinfo
 */
struct Etk_Event_Key_Up
{
   char *keyname;               /**< A name corresponding to the released key */
   Etk_Modifiers modifiers;     /**< The keyboard modifiers active when the event has been emitted */
   Etk_Locks locks;             /**< The keyboard locks active when the event has been emitted */

   const char *key;             /**< A name corresponding to the released key (unlike @a keyname, it takes the
                                 * active modifiers into account */
   const char *string;          /**< The UTF-8 string corresponding to the released key
                                 * (string to insert in an entry for example) */
   const char *compose;         /**< Unused */
   unsigned int timestamp;      /**< The timestamp when the event occured */
};


void etk_event_init(void);
void etk_event_shutdown(void);

void etk_event_mouse_in_wrap(Etk_Widget *widget, Evas_Event_Mouse_In *evas_event, Etk_Event_Mouse_In *etk_event);
void etk_event_mouse_out_wrap(Etk_Widget *widget, Evas_Event_Mouse_Out *evas_event, Etk_Event_Mouse_Out *etk_event);
void etk_event_mouse_move_wrap(Etk_Widget *widget, Evas_Event_Mouse_Move *evas_event, Etk_Event_Mouse_Move *etk_event);
void etk_event_mouse_down_wrap(Etk_Widget *widget, Evas_Event_Mouse_Down *evas_event, Etk_Event_Mouse_Down *etk_event);
void etk_event_mouse_up_wrap(Etk_Widget *widget, Evas_Event_Mouse_Up *evas_event, Etk_Event_Mouse_Up *etk_event);
void etk_event_mouse_wheel_wrap(Etk_Widget *widget, Evas_Event_Mouse_Wheel *evas_event, Etk_Event_Mouse_Wheel *etk_event);
void etk_event_key_down_wrap(Etk_Widget *widget, Evas_Event_Key_Down *evas_event, Etk_Event_Key_Down *etk_event);
void etk_event_key_up_wrap(Etk_Widget *widget, Evas_Event_Key_Up *evas_event, Etk_Event_Key_Up *etk_event);

void etk_event_global_callback_add(Etk_Event_Type event, void (*callback)(Etk_Event_Global event, void *data), void *data);
void etk_event_global_callback_del(Etk_Event_Type event, void (*callback)(Etk_Event_Global event, void *data));

void etk_event_mouse_position_get(int *x, int *y);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
