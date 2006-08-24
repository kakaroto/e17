/** @file etk_event.h */
#ifndef _ETK_EVENT_H_
#define _ETK_EVENT_H_

#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Event Etk_Event
 * @brief The event system of Etk (TODOC)
 * @{
 */

/** @brief TODOC */
typedef enum Etk_Modifiers
{
   ETK_MODIFIER_NONE = 0,
   ETK_MODIFIER_CTRL = 1 << 0,
   ETK_MODIFIER_ALT = 1 << 1,
   ETK_MODIFIER_SHIFT = 1 << 2,
   ETK_MODIFIER_WIN = 1 << 3,
} Etk_Modifiers;

/** @brief TODOC */
typedef enum Etk_Locks
{
   ETK_LOCK_NONE = 0,
   ETK_LOCK_NUM = 1 << 0,
   ETK_LOCK_CAPS = 1 << 1,
   ETK_LOCK_SCROLL = 1 << 2,
} Etk_Locks;

/** @brief TODOC */
typedef enum Etk_Mouse_Flags
{
   ETK_MOUSE_NONE = 0,
   ETK_MOUSE_DOUBLE_CLICK = 1 << 0,
   ETK_MOUSE_TRIPLE_CLICK = 1 << 1,
} Etk_Mouse_Flags;


/**
 * @brief The event structure sent when the mouse enters a widget
 * @structinfo
 */
/* TODOC: members */
struct Etk_Event_Mouse_In
{
   int buttons;
   struct {
      int x, y;
   } canvas, widget;
   Etk_Modifiers modifiers;
   Etk_Locks locks;
   unsigned int timestamp;
};

/**
 * @brief The event structure sent when the mouse leaves a widget
 * @structinfo
 */
/* TODOC: members */
struct Etk_Event_Mouse_Out
{
   int buttons;
   struct {
      int x, y;
   } canvas, widget;
   Etk_Modifiers modifiers;
   Etk_Locks locks;
   unsigned int timestamp;
};

/**
 * @brief The event structure sent when the mouse moves over a widget
 * @structinfo
 */
/* TODOC: members */
struct Etk_Event_Mouse_Move
{
   int buttons;
   struct {
      struct {
         int x, y;
      } canvas, widget;
   } cur, prev;
   Etk_Modifiers modifiers;
   Etk_Locks locks;
   unsigned int timestamp;
};

/**
 * @brief The event structure sent when the user presses a widget with the mouse
 * @structinfo
 */
/* TODOC: members */
struct Etk_Event_Mouse_Down
{
   int button;
   struct {
      int x, y;
   } canvas, widget;
   Etk_Modifiers modifiers;
   Etk_Locks locks;
   Etk_Mouse_Flags flags;
   unsigned int timestamp;
};

/**
 * @brief The event structure sent when the user releases a widget with the mouse
 * @structinfo
 */
/* TODOC: members */
struct Etk_Event_Mouse_Up
{
   int button;
   struct {
      int x, y;
   } canvas, widget;
   Etk_Modifiers modifiers;
   Etk_Locks locks;
   Etk_Mouse_Flags flags;
   unsigned int timestamp;
};

/**
 * @brief The event structure sent when the mouse wheel is used over a widget
 * @structinfo
 */
/* TODOC: members */
struct Etk_Event_Mouse_Wheel
{
   int direction;
   int z;
   struct {
      int x, y;
   } canvas, widget;
   Etk_Modifiers modifiers;
   Etk_Locks locks;
   unsigned int timestamp;
};

/**
 * @brief The event structure sent to the focused widget when a key is pressed
 * @structinfo
 */
/* TODOC: members */
struct Etk_Event_Key_Down
{
   char *keyname;
   Etk_Modifiers modifiers;
   Etk_Locks locks;

   const char *key;
   const char *string;
   const char *compose;
   unsigned int timestamp;
};

/**
 * @brief The event structure sent to the focused widget when a key is released
 * @structinfo
 */
/* TODOC: members */
struct Etk_Event_Key_Up
{
   char *keyname;
   Etk_Modifiers modifiers;
   Etk_Locks locks;

   const char *key;
   const char *string;
   const char *compose;
   unsigned int timestamp;
};


void etk_event_mouse_in_wrap(Etk_Widget *widget, Evas_Event_Mouse_In *evas_event, Etk_Event_Mouse_In *etk_event);
void etk_event_mouse_out_wrap(Etk_Widget *widget, Evas_Event_Mouse_Out *evas_event, Etk_Event_Mouse_Out *etk_event);
void etk_event_mouse_move_wrap(Etk_Widget *widget, Evas_Event_Mouse_Move *evas_event, Etk_Event_Mouse_Move *etk_event);
void etk_event_mouse_down_wrap(Etk_Widget *widget, Evas_Event_Mouse_Down *evas_event, Etk_Event_Mouse_Down *etk_event);
void etk_event_mouse_up_wrap(Etk_Widget *widget, Evas_Event_Mouse_Up *evas_event, Etk_Event_Mouse_Up *etk_event);
void etk_event_mouse_wheel_wrap(Etk_Widget *widget, Evas_Event_Mouse_Wheel *evas_event, Etk_Event_Mouse_Wheel *etk_event);
void etk_event_key_down_wrap(Etk_Widget *widget, Evas_Event_Key_Down *evas_event, Etk_Event_Key_Down *etk_event);
void etk_event_key_up_wrap(Etk_Widget *widget, Evas_Event_Key_Up *evas_event, Etk_Event_Key_Up *etk_event);

/** @} */

#endif
