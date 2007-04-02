/** @file etk_menu.h */
#ifndef _ETK_MENU_H_
#define _ETK_MENU_H_

#include "etk_menu_shell.h"
#include "etk_popup_window.h"
#include "etk_types.h"

/* TODO/FIXME list:
 * - Keyboard navigation!
 * - Sometimes an item is selected when the menu is popped up. Probably a bug in Etk_Popup_Window's feeding code
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
 * @brief @widget A menu shell implemented in a popup window
 * @structinfo
 */
struct Etk_Menu
{
   /* private: */
   /* Inherit from Etk_Menu_Shell */
   Etk_Menu_Shell menu_shell;
   
   Etk_Popup_Window *window;
   Etk_Menu_Item *parent_item;
};


Etk_Type      *etk_menu_type_get(void);
Etk_Widget    *etk_menu_new(void);
Etk_Menu_Item *etk_menu_parent_item_get(Etk_Menu *menu);

void           etk_menu_popup(Etk_Menu *menu);
void           etk_menu_popup_in_direction(Etk_Menu *menu, Etk_Popup_Direction direction);
void           etk_menu_popup_at_xy(Etk_Menu *menu, int x, int y);
void           etk_menu_popup_at_xy_in_direction(Etk_Menu *menu, int x, int y, Etk_Popup_Direction direction);
void           etk_menu_popdown(Etk_Menu *menu);

/** @} */

#endif
