#ifndef __EWL_MENU_BASE_H__
#define __EWL_MENU_BASE_H__

/**
 * @defgroup Ewl_Menu_Base Menu_Base: The Basic Menu Functionality
 * @brief Defines the basic menu classes that are extended to an actual menu
 * implementation by inheriting classes such as Ewl_Menu and Ewl_IMenu.
 *
 * @{
 */

/**
 * This serves as a basis for all menu related entries. It provides the most
 * basic layout facilities for items in a menu.
 */
typedef struct Ewl_Menu_Item Ewl_Menu_Item;

/**
 * @def EWL_MENU_ITEM(mi)
 * Typecasts a pointer to an Ewl_Menu_Item pointer.
 */
#define EWL_MENU_ITEM(mi) ((Ewl_Menu_Item *)mi)

/**
 * @struct Ewl_Menu_Item
 * Inherits from Ewl_Box to gain it's layout abilities, places policy on top
 * of the box framework to provide a simple menu layout of icon and label.
 */
struct Ewl_Menu_Item
{
	
	Ewl_Box         box; /**< Inherit from Ewl_Box for layout  */
	Ewl_Widget     *icon; /**< The image in this menu item.  */
	Ewl_Widget     *text; /**< The text label for this menu item.  */
	int             submenu; /**< Indicates if this is inside a menu.  */
};

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
#define EWL_MENU_SEPARATOR(s) ((Ewl_Menu_Separator *)s)

/**
 * @struct Ewl_Menu_Separator
 * Inherits from Ewl_Menu_Item and limits it's functionality to simply provide
 * a separator between items in a menu.
 */
struct Ewl_Menu_Separator
{
	Ewl_Menu_Item item; /**< Inherit from Ewl_Menu_Item */
};

/**
 * Provides the basic functionality common to the various menu classes.
 */
typedef struct Ewl_Menu_Base Ewl_Menu_Base;

/**
 * @def EWL_MENU_BASE(menu)
 * Typecasts a pointer to an Ewl_Menu_Base pointer.
 */
#define EWL_MENU_BASE(menu) ((Ewl_Menu_Base *) menu)

struct Ewl_Menu_Base
{
	Ewl_Menu_Item   item; /**< Inherit from Ewl_Menu_Item */
	Ewl_Widget     *popup; /**< The popup portion of the menu */
	Ewl_Widget     *popbox; /**< Box for layout in popup */
};

Ewl_Widget     *ewl_menu_item_new(char *image, char *title);
void            ewl_menu_item_init(Ewl_Menu_Item * menu, char *image,
				   char *title);

Ewl_Menu_Separator *ewl_menu_separator_new();
void            ewl_menu_separator_init(Ewl_Menu_Separator *sep);

void            ewl_menu_base_init(Ewl_Menu_Base * menu, char *image,
				   char *title);

/**
 * @}
 */

#endif
