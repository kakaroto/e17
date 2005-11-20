/** @file etk_menu_bar.h */
#ifndef _ETK_MENU_BAR_H_
#define _ETK_MENU_BAR_H_

#include "etk_menu_shell.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Menu_Bar Etk_Menu_Bar
 * @{
 */

/** @brief Gets the type of a menu bar */
#define ETK_MENU_BAR_TYPE       (etk_menu_bar_type_get())
/** @brief Casts the object to an Etk_Menu_Bar */
#define ETK_MENU_BAR(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_BAR_TYPE, Etk_Menu_Bar))
/** @brief Check if the object is an Etk_Menu_Bar */
#define ETK_IS_MENU_BAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_BAR_TYPE))

/**
 * @struct Etk_Menu_Bar
 * @param A menu bar is an horizontal bar that can contain several menu items
*/
struct _Etk_Menu_Bar
{
   /* private: */
   /* Inherit from Etk_Menu_Shell */
   Etk_Menu_Shell menu_shell;
   
   Etk_Bool item_selected;
};

Etk_Type *etk_menu_bar_type_get();
Etk_Widget *etk_menu_bar_new();

/** @} */

#endif
