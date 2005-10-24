#ifndef __EWL_MENU_BASE_H__
#define __EWL_MENU_BASE_H__

/**
 * @file ewl_menu_base.h
 * @defgroup Ewl_Menu_Base Menu_Base: The Basic Menu Functionality
 * @brief Defines the basic menu classes that are extended to an actual menu
 * implementation by inheriting classes such as Ewl_Menu and Ewl_Imenu.
 *
 * @{
 */

/**
 * @themekey /menu_base/file
 * @themekey /menu_base/group
 */

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
	Ewl_Menu_Item   item;   /**< Inherit from Ewl_Menu_Item */
	Ewl_Widget     *popup;  /**< The popup portion of the menu */
	Ewl_Widget     *popbox; /**< Box for layout in popup */
	int             hold;   /**< Indicates not to hide this on a deselect */
};

int ewl_menu_base_init(Ewl_Menu_Base *menu);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_menu_base_expand_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_base_collapse_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_menu_base_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);

void ewl_menu_base_popup_show_cb(Ewl_Widget *w, void *ev_data, 
						void *user_data);
void ewl_menu_base_popup_hide_cb(Ewl_Widget *w, void *ev_data, 
						void *user_data);
void ewl_menu_base_popbox_key_down_cb(Ewl_Widget *w, void *ev_data, 
						void *user_data);

/**
 * @}
 */

#endif
