#ifndef __EWL_MENU_H__
#define __EWL_MENU_H__

/**
 * @defgroup Ewl_Menu Menu: A Popup Menu
 * Provides a popup menu for displaying a list of options.
 *
 * @{
 */

/**
 * @themekey /menu/file
 * @themekey /menu/group
 */

/**
 * A simple popup menu widget, with the capability of being nested inside
 * another menu.
 */
typedef struct Ewl_Menu Ewl_Menu;

/**
 * @def EWL_MENU(menu)
 * Typecasts a pointer to an Ewl_Menu pointer.
 */
#define EWL_MENU(menu) ((Ewl_Menu *) menu)

/**
 * @struct Ewl_Menu
 * Inherits from Ewl_Menu_Base and adds on functionality for displaying a
 * pop-up menu that is in a window of it's own.
 */
struct Ewl_Menu
{
	Ewl_Menu_Base base; /**< Inherit from Ewl_Menu_Base */
};

Ewl_Widget *ewl_menu_new(char *image, char *title);
void        ewl_menu_init(Ewl_Menu * menu, char *image, char *title);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_menu_expand_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_menu_item_clicked_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_menu_realize_cb(Ewl_Widget * w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif				/* __EWL_MENU_H__ */
