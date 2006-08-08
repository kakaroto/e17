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

enum Etk_Menu_Shell_Signal_Id
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
 * @return Returns the type of an Etk_Menu_Shell
 */
Etk_Type *etk_menu_shell_type_get()
{
   static Etk_Type *menu_shell_type = NULL;

   if (!menu_shell_type)
   {
      menu_shell_type = etk_type_new("Etk_Menu_Shell", ETK_WIDGET_TYPE, sizeof(Etk_Menu_Shell),
         ETK_CONSTRUCTOR(_etk_menu_shell_constructor), ETK_DESTRUCTOR(_etk_menu_shell_destructor));
   
      _etk_menu_shell_signals[ETK_MENU_SHELL_ITEM_ADDED_SIGNAL] = etk_signal_new("item_added",
         menu_shell_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_menu_shell_signals[ETK_MENU_SHELL_ITEM_REMOVED_SIGNAL] = etk_signal_new("item_removed",
         menu_shell_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
   }

   return menu_shell_type;
}

/** 
 * @brief Adds a menu item at the start of the menu shell
 * @param menu_shell a menu shell
 * @param item the menu item to prepend to the menu shell
 */
void etk_menu_shell_prepend(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item)
{
   if (!menu_shell || !item)
      return;
   
   menu_shell->items = evas_list_prepend(menu_shell->items, item);
   _etk_menu_shell_item_add(menu_shell, item);
}

/** 
 * @brief Adds a menu item at the end of the menu shell
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
 * @brief Adds a menu item before an existing item of the menu shell
 * @param menu_shell a menu shell
 * @param item the menu item to add
 * @param relative the item before which @a item will be added.
 * If @a relative is not in the menu shell, @a item will be added at the start of the menu shell
 */
void etk_menu_shell_prepend_relative(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, Etk_Menu_Item *relative)
{
   if (!menu_shell || !item)
      return;
   
   menu_shell->items = evas_list_prepend_relative(menu_shell->items, item, relative);
   _etk_menu_shell_item_add(menu_shell, item);
}

/**
 * @brief Adds a menu item after an existing item of the menu shell
 * @param menu_shell a menu shell
 * @param item the menu item to add
 * @param relative the item after which @a item will be added.
 * If @a relative is not in the menu shell, @a item will be added at the end of the menu shell
 */
void etk_menu_shell_append_relative(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, Etk_Menu_Item *relative)
{
   if (!menu_shell || !item)
      return;
   
   menu_shell->items = evas_list_append_relative(menu_shell->items, item, relative);
   _etk_menu_shell_item_add(menu_shell, item);
}

/**
 * @brief Inserts a menu item to the menu shell at a specific position
 * @param menu_shell a menu shell
 * @param item the menu item to insert
 * @param position the position where to insert the item (0 being the first). @n
 * If @a position <= 0, the item will be prepended. @n
 * If @a position >= the number of items of the menu shell, the item will be appended
 */
void etk_menu_shell_insert(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, int position)
{
   if (!menu_shell || !item)
      return;
   
   if (position <= 0)
      menu_shell->items = evas_list_prepend(menu_shell->items, item);
   else if (position >= evas_list_count(menu_shell->items))
      menu_shell->items = evas_list_append(menu_shell->items, item);
   else
   {
      void *rel;
      
      rel = evas_list_nth(menu_shell->items, position);
      menu_shell->items = evas_list_prepend_relative(menu_shell->items, item, rel);
   }
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
      menu_shell->items = evas_list_remove_list(menu_shell->items, l);
      etk_widget_size_recalc_queue(ETK_WIDGET(menu_shell));
      etk_signal_emit(_etk_menu_shell_signals[ETK_MENU_SHELL_ITEM_REMOVED_SIGNAL], ETK_OBJECT(menu_shell), NULL, item);
   }
}

/**
 * @brief Gets the list of the items of the menu shell
 * @param menu_shell a menu shell
 * @return Returns the list of the items of the menu shell.
 * The returned list will have to be freed with evas_list_free()
 */
Evas_List *etk_menu_shell_items_get(Etk_Menu_Shell *menu_shell)
{
   Evas_List *list, *l;
   
   if (!menu_shell)
      return NULL;
   
   list = NULL;
   for (l = menu_shell->items; l; l = l->next)
      list = evas_list_append(list, l->data);
   
   return list;
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
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Menu_Shell
 *
 * New menu items can be added to the menu shell with etk_menu_shell_append(), etk_menu_shell_insert()... @n
 * Menu items can be removed from the menu shell with etk_menu_shell_remove(). @n
 * You can also get the list of the menu items of the menu shell with etk_menu_shell_items_get().
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Menu_Shell
 */
