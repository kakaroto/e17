#ifndef __EWL_MENU_BASE_H__
#define __EWL_MENU_BASE_H__

typedef struct _ewl_menu_item Ewl_Menu_Item;

#define EWL_MENU_ITEM(mi) ((Ewl_Menu_Item *)mi)

struct _ewl_menu_item
{
	/*
	 * The grid allows for easy layout of the icon and text.
	 */
	Ewl_Box         box;

	/*
	 * The image in this menu item.
	 */
	Ewl_Widget     *icon;

	/*
	 * The text for this menu item.
	 */
	Ewl_Widget     *text;

	/*
	 * Indicates if this is inside a menu. This is actually only used by
	 * the menus, but placed here for simplicity.
	 */
	int             submenu;
};

typedef struct _ewl_menu_base Ewl_Menu_Base;

#define EWL_MENU_BASE(menu) ((Ewl_Menu_Base *) menu)

struct _ewl_menu_base
{
	/*
	 * Inherit the item so that we can place menus inside other menus
	 */
	Ewl_Menu_Item   item;

	/*
	 * The popup portion of the menu
	 *
	 * FIXME: Should this become a separate pop-up menu class?
	 */
	Ewl_Widget     *popup;
	Ewl_Widget     *popbox;

	/*
	 * boolean: is title collapsed or expanded
	 */
	int             t_expand;

	/* animation variables */
	int             width;
	int             height;
};

Ewl_Widget     *ewl_menu_item_new(char *image, char *title);
void            ewl_menu_item_init(Ewl_Menu_Item * menu, char *image,
				   char *title);

void            ewl_menu_base_init(Ewl_Menu_Base * menu, char *image,
				   char *title);

#endif
