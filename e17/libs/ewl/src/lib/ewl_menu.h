#ifndef EWL_MENU_H
#define EWL_MENU_H

/**
 * @addtogroup Ewl_Menu Ewl_Menu: A Simple Windowed Menu
 * Defines a menu used globally. The contents on the menu are drawn outside of
 * the Evas.
 *
 * @{
 */

/**
 * @themekey /menu/file
 * @themekey /menu/group
 */

/**
 * @def EWL_MENU_TYPE
 * The type name for the Ewl_Menu widget
 */
#define EWL_MENU_TYPE "menu"

/**
 * A simple internal menu, it is limited to drawing within the current evas.
 */
typedef struct Ewl_Menu Ewl_Menu;

/**
 * @def EWL_MENU(menu)
 * Typecasts a pointer to an Ewl_Menu pointer.
 */
#define EWL_MENU(menu) ((Ewl_Menu *) menu)

/**
 * Inherits from the Ewl_Menu_Base and does not extend the structure, but
 * provides policy for drawing on the current evas.
 */
struct Ewl_Menu
{
	Ewl_Menu_Base  base;		/**< Inherit from Ewl_Menu_Base */

	Ewl_Widget    *menubar_parent;	/**< Parent menu bar */
};

Ewl_Widget	*ewl_menu_new(void);
int		 ewl_menu_init(Ewl_Menu *menu);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_menu_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_cb_expand(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_cb_hide(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_menu_cb_popup_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_menu_cb_expand_mouse_move(Ewl_Widget *w, void *ev_data, 
							void *user_data);
void ewl_menu_cb_mouse_move(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
