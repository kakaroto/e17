/** @file etk_menu.h */
#ifndef _ETK_MENU_H_
#define _ETK_MENU_H_

#include "etk_menu_shell.h"
#include "etk_types.h"

/* TODO/FIXME list:
 * - Sometimes, when the menu is popped down, the window stay visible, but its content is hidden
 * - The width of the menu window is not correctly calculated. Probably a bug of edje_object_size_min_calc().
 * For now, 15 pixels are manually added to the calculated width
 * - Keyboard navigation!
 * - better theme grouping?
 * - Sometimes the menu is not popped up at the right place (maybe a bug ot Etk_Popup_Window)
 */

/**
 * @defgroup Etk_Menu Etk_Menu
 * @brief The Etk_Menu widget is a menu shell implemented in a popup window
 * @{
 */

/** Gets the type of a menu */
#define ETK_MENU_TYPE       (etk_menu_type_get())
/** Casts the object to an Etk_Menu */
#define ETK_MENU(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_TYPE, Etk_Menu))
/** Check if the object is an Etk_Menu */
#define ETK_IS_MENU(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_TYPE))

/**
 * @brief @widget The structure of a menu
 * @structinfo
 */
struct Etk_Menu
{
   /* private: */
   /* Inherit from Etk_Menu_Shell */
   Etk_Menu_Shell menu_shell;
   
   Etk_Popup_Window *window;
};

Etk_Type *etk_menu_type_get();
Etk_Widget *etk_menu_new();

void etk_menu_popup_at_xy(Etk_Menu *menu, int x, int y);
void etk_menu_popup(Etk_Menu *menu);
void etk_menu_popdown(Etk_Menu *menu);

/** @} */

#endif
