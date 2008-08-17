/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_MENU_H
#define EWL_MENU_H

#include "ewl_menu_item.h"
/**
 * @addtogroup Ewl_Menu Ewl_Menu: A Simple Windowed Menu
 * Defines a menu used globally. The contents on the menu are drawn outside of
 * the canvas.
 *
 * @remarks Inherits from Ewl_Menu_Item.
 * @if HAVE_IMAGES
 * @image html Ewl_Menu_inheritance.png
 * @endif
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
 * @def EWL_MENU_IS(w)
 * Returns TRUE if the widget is an Ewl_Menu, FALSE otherwise
 */
#define EWL_MENU_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_MENU_TYPE))

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
 * @brief Inherits from the Ewl_Menu_Base and extends to provide a simple
 * menu
 */
struct Ewl_Menu
{
        Ewl_Menu_Item  item;                /**< Inherit from Ewl_Menu_Base */
        Ewl_Widget *popup;              /**< The popup holding the menu items */

        Ewl_Widget *menubar_parent;        /**< Parent menu bar */
};

/**
 * A simple struct to contain menu information
 */
typedef struct Ewl_Menu_Info Ewl_Menu_Info;

/**
 * @brief Provides a simple structure to contain menu information
 */
struct Ewl_Menu_Info
{
        const char *name;                /**< The name of the menu item */
        const char *img;                /**< The menu item image */
        void (*cb)(Ewl_Widget *w, void *ev, void *data); /**< The menu item callback */
};

Ewl_Widget      *ewl_menu_new(void);
int              ewl_menu_init(Ewl_Menu *menu);

void             ewl_menu_from_info(Ewl_Menu *menu, Ewl_Menu_Info *info);

void             ewl_menu_collapse(Ewl_Menu *menu);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_menu_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_cb_expand(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_cb_hide(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_cb_realize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_menu_cb_popup_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_menu_cb_mouse_move(Ewl_Widget *w, void *ev_data, void *user_data);

int ewl_menu_mouse_feed(Ewl_Menu *menu, int x, int y);

/**
 * @}
 */

#endif
