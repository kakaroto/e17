#ifndef __EWL_MENU_H__
#define __EWL_MENU_H__

/**
 * @file ewl_menu.h
 * @defgroup Ewl_Menu Menu: A Simple Windowed Menu
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
 * A simple internal menu, it is limited to drawing within the current evas.
 */
typedef struct Ewl_Menu Ewl_Menu;

/**
 * @def EWL_MENU(menu)
 * Typecasts a pointer to an Ewl_Menu pointer.
 */
#define EWL_MENU(menu) ((Ewl_Menu *) menu)

/**
 * @struct Ewl_Menu
 * Inherits from the Ewl_Menu_Base and does not extend the structure, but
 * provides policy for drawing on the current evas.
 */
struct Ewl_Menu
{
	Ewl_Menu_Base  base;
        int            popup_x;
        int            popup_y;
};

Ewl_Widget     *ewl_menu_new(void);

void            ewl_menu_init(Ewl_Menu *menu);

/*
 * Internally used callbacks, override at your own risk.
 */
void            ewl_menu_realize_cb  (Ewl_Widget *w,
				      void       *ev_data,
				      void       *user_data);
void            ewl_menu_configure_cb  (Ewl_Widget *w,
					void       *ev_data,
					void       *user_data);

void            ewl_menu_expand_cb     (Ewl_Widget *w,
					void       *ev_data,
					void       *user_data);

void            ewl_menu_popup_move_cb (Ewl_Widget *w,
					void       *ev_data,
					void       *user_data);

/**
 * @}
 */

#endif				/* __EWL_MENU_H__ */
