/** @file etk_menu_shell.c */
#include "etk_menu_shell.h"
#include <stdlib.h>
#include "etk_menu_item.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Menu_Shell
 * @{
 */

enum _Etk_Menu_Shell_Signal_Id
{
   ETK_MENU_SHELL_ITEM_ADDED_SIGNAL,
   ETK_MENU_SHELL_ITEM_REMOVED_SIGNAL,
   ETK_MENU_SHELL_NUM_SIGNALS
};

static void _etk_menu_shell_constructor(Etk_Menu_Shell *menu_shell);
static void _etk_menu_shell_destructor(Etk_Menu_Shell *menu_shell);
static void _etk_menu_shell_item_add(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item);

static Etk_Signal *_etk_menu_shell_signals[ETK_MENU_SHELL_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Menu_Shell
 * @return Returns the type on an Etk_Menu_Shell
 */
Etk_Type *etk_menu_shell_type_get()
{
   static Etk_Type *menu_shell_type = NULL;

   if (!menu_shell_type)
   {
      menu_shell_type = etk_type_new("Etk_Menu_Shell", ETK_WIDGET_TYPE, sizeof(Etk_Menu_Shell), ETK_CONSTRUCTOR(_etk_menu_shell_constructor), ETK_DESTRUCTOR(_etk_menu_shell_destructor));
   
      _etk_menu_shell_signals[ETK_MENU_SHELL_ITEM_ADDED_SIGNAL] = etk_signal_new("item_added", menu_shell_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_menu_shell_signals[ETK_MENU_SHELL_ITEM_REMOVED_SIGNAL] = etk_signal_new("item_removed", menu_shell_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
   }

   return menu_shell_type;
}

/**
 * @brief Creates a new menu_shell
 * @return Returns the new menu_shell widget
 */
Etk_Widget *etk_menu_shell_new()
{
   return etk_widget_new(ETK_MENU_SHELL_TYPE, "theme_group", "menu_shell", NULL);
}

/** 
 * @brief Adds a menu_shell item at the start of the menu shell
 * @param menu_shell a menu shell
 * @param item the menu item to prepend to the menu shell
 */
void etk_menu_shell_prepend(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item)
{
   if (!menu_shell || !item)
      return;
   
   menu_shell->items = evas_list_prepend(menu_shell->items, item);
   etk_widget_parent_set(ETK_WIDGET(item), ETK_WIDGET(menu_shell));
   item->parent = menu_shell;
   etk_signal_emit(_etk_menu_shell_signals[ETK_MENU_SHELL_ITEM_ADDED_SIGNAL], ETK_OBJECT(menu_shell), NULL, item);
}

/** 
 * @brief Adds a menu_shell item at the end of the menu shell
 * @param menu_shell a menu shell
 * @param item the menu item to append to the menu shell
 */
void etk_menu_shell_append(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item)
{
   if (!menu_shell || !item)
      return;
   
   menu_shell->items = evas_list_append(menu_shell->items, item);
   _etk_menu_shell_item_add(menu_shell, item);
}

/**
 * @brief Removes an item from the menu shell
 * @param menu_shell a menu shell
 * @param item the item to remove
 */
void etk_menu_shell_remove(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item)
{
   Evas_List *l;
   
   if (!menu_shell || !item)
      return;
   
   if ((l = evas_list_find_list(menu_shell->items, item)))
   {
      etk_widget_parent_set(ETK_WIDGET(item), NULL);
      item->parent = NULL;
      etk_signal_emit(_etk_menu_shell_signals[ETK_MENU_SHELL_ITEM_REMOVED_SIGNAL], ETK_OBJECT(menu_shell), NULL, item);
      menu_shell->items = evas_list_remove_list(menu_shell->items, l);
      etk_widget_size_recalc_queue(ETK_WIDGET(menu_shell));
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_menu_shell_constructor(Etk_Menu_Shell *menu_shell)
{
   if (!menu_shell)
      return;
   menu_shell->items = NULL;
   menu_shell->parent = NULL;
}

/* Destroys the menu_shell */
static void _etk_menu_shell_destructor(Etk_Menu_Shell *menu_shell)
{
   if (!menu_shell)
      return;
   evas_list_free(menu_shell->items);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Adds a menu item to the menu shell */
static void _etk_menu_shell_item_add(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item)
{
   etk_widget_parent_set(ETK_WIDGET(item), ETK_WIDGET(menu_shell));
   item->parent = menu_shell;
   etk_signal_emit(_etk_menu_shell_signals[ETK_MENU_SHELL_ITEM_ADDED_SIGNAL], ETK_OBJECT(menu_shell), NULL, item);
   etk_widget_size_recalc_queue(ETK_WIDGET(menu_shell));
}

/** @} */
