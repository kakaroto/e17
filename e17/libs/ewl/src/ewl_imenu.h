#ifndef __EWL_MENU_H__
#define __EWL_MENU_H__

typedef struct _ewl_imenu_item Ewl_IMenu_Item;

#define EWL_IMENU_ITEM(mi) ((Ewl_IMenu_Item *)mi)

struct _ewl_imenu_item {
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

typedef struct _ewl_imenu Ewl_IMenu;

#define EWL_IMENU(menu) ((Ewl_IMenu *) menu)

struct _ewl_imenu {
	/*
	 * Inherit the item so that we can place menus inside other menus
	 */
	Ewl_IMenu_Item  item;

	/*
	 * The popup portion of the menu
	 *
	 * FIXME: Should this become a separate pop-up menu class?
	 */
	Ewl_Widget     *popup;

	/*
	 * boolean: is title collapsed or expanded
	 */
	int             t_expand;

	/* animation variables */
	int             width;
	int             height;
};

Ewl_Widget     *ewl_imenu_new(char *image, char *title);
void            ewl_imenu_init(Ewl_IMenu * menu, char *image, char *title);

Ewl_Widget     *ewl_imenu_item_new(char *image, char *title);
void            ewl_imenu_item_init(Ewl_IMenu_Item * menu, char *image,
				    char *title);

void            ewl_imenu_set_title_expandable(Ewl_IMenu * m);

#endif /* __EWL_MENU_H__ */
