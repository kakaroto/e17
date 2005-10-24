#ifndef EWL_MENU_SEPARATOR_H
#define EWL_MENU_SEPARATOR_H

/**
 * @file ewl_menu_separator.h
 * @defgroup Ewl_Menu_Separator Menu_Separator: A menu separator
 *
 * @{
 */

/**
 * A simple separator widget for putting lines between items in the menu.
 * Special widget so enclosing menus can assume it can be treated as an
 * Ewl_Menu_Item.
 */
typedef struct Ewl_Menu_Separator Ewl_Menu_Separator;

/**
 * @def EWL_MENU_SEPARATOR(s)
 * Typecasts a pointer to an Ewl_Menu_Separator pointer.
 */
#define EWL_MENU_SEPARATOR(s) ((Ewl_Menu_Separator *) s)

/**
 * @struct Ewl_Menu_Separator
 * Inherits from Ewl_Menu_Item and limits it's functionality to simply provide
 * a separator between items in a menu.
 */
struct Ewl_Menu_Separator
{
        Ewl_Menu_Item item; /**< Inherit from Ewl_Menu_Item */
};

Ewl_Widget	*ewl_menu_separator_new(void);
int 		 ewl_menu_separator_init(Ewl_Menu_Separator *sep);

/**
 * @}
 */

#endif

