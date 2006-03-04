/** @file etk_menu_window.h */
#ifndef _ETK_MENU_WINDOW_H_
#define _ETK_MENU_WINDOW_H_

#include "etk_window.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Menu_Window Etk_Menu_Window
 * @{
 */

/** @brief Gets the type of a menu window */
#define ETK_MENU_WINDOW_TYPE       (etk_menu_window_type_get())
/** @brief Casts the object to an Etk_Menu_Window */
#define ETK_MENU_WINDOW(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_WINDOW_TYPE, Etk_Menu_Window))
/** @brief Check if the object is an Etk_Menu_Window */
#define ETK_IS_MENU_WINDOW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_WINDOW_TYPE))

/* TODO: doc */
struct _Etk_Menu_Window
{
   /* private: */
   /* Inherit from Etk_Window */
   Etk_Window window;
};

Etk_Type *etk_menu_window_type_get();

void etk_menu_window_popup_at_xy(Etk_Menu_Window *menu_window, int x, int y);
void etk_menu_window_popup(Etk_Menu_Window *menu_window);
void etk_menu_window_popdown(Etk_Menu_Window *menu_window);
void etk_menu_window_popdown_all();

/** @} */

#endif
