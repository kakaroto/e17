/** @file etk_popup_window.h */
#ifndef _ETK_POPUP_WINDOW_H_
#define _ETK_POPUP_WINDOW_H_

#include "etk_window.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Popup_Window Etk_Popup_Window
 * @brief The Etk_Popup_Window widget is a borderless window that can pops up/down. It is used by menus and comboboxes
 * @{
 */

/** Gets the type of a popup window */
#define ETK_POPUP_WINDOW_TYPE       (etk_popup_window_type_get())
/** Casts the object to an Etk_Popup_Window */
#define ETK_POPUP_WINDOW(obj)       (ETK_OBJECT_CAST((obj), ETK_POPUP_WINDOW_TYPE, Etk_Popup_Window))
/** Check if the object is an Etk_Popup_Window */
#define ETK_IS_POPUP_WINDOW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_POPUP_WINDOW_TYPE))

/**
 * @brief @widget A borderless window that can be popped up/down
 * @structinfo
 */
struct Etk_Popup_Window
{
   /* private: */
   /* Inherit from Etk_Window */
   Etk_Window window;
   
   Etk_Bool popped_up;
   Etk_Popup_Window *parent;
   Evas_List *children;
   Etk_Popup_Window *popped_child;
};

Etk_Type *etk_popup_window_type_get();

void              etk_popup_window_parent_set(Etk_Popup_Window *popup_window, Etk_Popup_Window *parent);
Etk_Popup_Window *etk_popup_window_parent_get(Etk_Popup_Window *popup_window);
void              etk_popup_window_focused_window_set(Etk_Popup_Window *popup_window);
Etk_Popup_Window *etk_popup_window_focused_window_get();

void     etk_popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y);
void     etk_popup_window_popup(Etk_Popup_Window *popup_window);
void     etk_popup_window_popdown(Etk_Popup_Window *popup_window);
void     etk_popup_window_popdown_all();
Etk_Bool etk_popup_window_is_popped_up(Etk_Popup_Window *popup_window);

/** @} */

#endif
