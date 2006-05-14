/** @file etk_menu_shell.h */
#ifndef _ETK_MENU_SHELL_H_
#define _ETK_MENU_SHELL_H_

#include "etk_widget.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Menu_Shell Etk_Menu_Shell
 * @brief Etk_Menu_Shell is the base class used to derive Etk_Menu and Etk_Menu_Bar
 * @{
 */

/** Gets the type of a menu shell */
#define ETK_MENU_SHELL_TYPE       (etk_menu_shell_type_get())
/** Casts the object to an Etk_Menu_Shell */
#define ETK_MENU_SHELL(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_SHELL_TYPE, Etk_Menu_Shell))
/** Check if the object is an Etk_Menu_Shell */
#define ETK_IS_MENU_SHELL(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_SHELL_TYPE))

/**
 * @brief @widget The structure of a menu shell
 * @structinfo
 */
struct Etk_Menu_Shell
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Etk_Menu_Item *parent;
   Evas_List *items;
};

Etk_Type *etk_menu_shell_type_get();

void etk_menu_shell_prepend(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item);
void etk_menu_shell_append(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item);
void etk_menu_shell_prepend_relative(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, Etk_Menu_Item *relative);
void etk_menu_shell_append_relative(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, Etk_Menu_Item *relative);
void etk_menu_shell_insert(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, int position);
void etk_menu_shell_remove(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item);
Evas_List *etk_menu_shell_items_get(Etk_Menu_Shell *menu_shell);

/** @} */

#endif
