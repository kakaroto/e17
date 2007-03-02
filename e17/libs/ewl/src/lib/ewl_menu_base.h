/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_MENU_BASE_H
#define EWL_MENU_BASE_H

#include "ewl_menu_item.h"

/**
 * @addtogroup Ewl_Menu_Base Ewl_Menu_Base: The Basic Menu Functionality
 * @brief Defines the basic menu classes that are extended to an actual menu
 * implementation by inheriting classes such as Ewl_Menu and Ewl_Imenu.
 *
 * @remarks Inherits from Ewl_Menu_Item.
 * @image html Ewl_Menu_Base_inheritance.png
 *
 * @{
 */

/**
 * @themekey /menu_base/file
 * @themekey /menu_base/group
 */

/**
 * @def EWL_MENU_BASE_TYPE
 * The type name for the Ewl_Menu_Base widget
 */
#define EWL_MENU_BASE_TYPE "menu_base"

/**
 * @def EWL_MENU_BASE_IS(w)
 * Returns TRUE if the widget is an Ewl_Menu_Base, FALSE otherwise
 */
#define EWL_MENU_BASE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_MENU_BASE_TYPE))

/**
 * Provides the basic functionality common to the various menu classes.
 */
typedef struct Ewl_Menu_Base Ewl_Menu_Base;

/**
 * @def EWL_MENU_BASE(menu)
 * Typecasts a pointer to an Ewl_Menu_Base pointer.
 */
#define EWL_MENU_BASE(menu) ((Ewl_Menu_Base *) menu)

/**
 * Inherits from Ewl_Menu_Item and extends to create the base menu 
 */
struct Ewl_Menu_Base
{
	Ewl_Menu_Item item;   	/**< Inherit from Ewl_Menu_Item */
	Ewl_Widget *popup;  	/**< The popup portion of the menu */
	Ewl_Widget *popbox; 	/**< Box for layout in popup */
	int hold; 		/**< Indicates not to hide this on a deselect */
};

int ewl_menu_base_init(Ewl_Menu_Base *menu);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_menu_base_cb_expand(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_base_cb_collapse(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_base_cb_destroy(Ewl_Widget *w, void *ev_data, void *user_data);

void ewl_menu_base_cb_popup_show(Ewl_Widget *w, void *ev_data, 
						void *user_data);
void ewl_menu_base_cb_popup_hide(Ewl_Widget *w, void *ev_data, 
						void *user_data);
void ewl_menu_base_cb_popbox_key_down(Ewl_Widget *w, void *ev_data, 
						void *user_data);

/**
 * @}
 */

#endif
