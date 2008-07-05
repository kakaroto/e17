/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_MENU_ITEM_H
#define EWL_MENU_ITEM_H

#include "ewl_button.h"

/**
 * @addtogroup Ewl_Menu_Item Ewl_Menu_Item: The basic menu item
 * The basic menu item
 *
 * @remarks Inherits from Ewl_Button.
 * @if HAVE_IMAGES
 * @image html Ewl_Menu_Item_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /menu_item/file
 * @themekey /menu_item/group
 */

/**
 * @def EWL_MENU_ITEM_TYPE
 * The type name for the Ewl_Menu_Item widget
 */
#define EWL_MENU_ITEM_TYPE "menu_item"

/**
 * @def EWL_MENU_ITEM_IS(w)
 * Returns TRUE if the widget is an Ewl_Menu_Item, FALSE otherwise
 */
#define EWL_MENU_ITEM_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_MENU_ITEM_TYPE))

/**
 * This serves as a basis for all menu related entries. It provides the most
 * basic layout facilities for items in a menu.
 */
typedef struct Ewl_Menu_Item Ewl_Menu_Item;

/**
 * @def EWL_MENU_ITEM(mi)
 * Typecasts a pointer to an Ewl_Menu_Item pointer.
 */
#define EWL_MENU_ITEM(mi) ((Ewl_Menu_Item *) mi)

/**
 * @brief Inherits from Ewl_Button and provides a basic menu item widget
 */
struct Ewl_Menu_Item
{
        Ewl_Button button;         /**< Inherit from Ewl_Button */
        Ewl_Widget *inmenu;         /**< Set if inside a menu */
};

Ewl_Widget      *ewl_menu_item_new(void);
int              ewl_menu_item_init(Ewl_Menu_Item *menu);

/**
 * @}
 */
#endif

