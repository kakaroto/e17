#ifndef EWL_IMENU_H
#define EWL_IMENU_H

/**
 * @addtogroup Ewl_Imenu Ewl_Imenu: A Simple Internal Menu
 * Defines a menu used internally. The contents on the menu are not drawn
 * outside of the Evas.
 *
 * @{
 */

/**
 * @themekey /imenu/file
 * @themekey /imenu/group
 */

/**
 * @def EWL_IMENU_TYPE
 * The type name for the Ewl_IMenu widget
 */
#define EWL_IMENU_TYPE "imenu"

/**
 * A simple internal menu, it is limited to drawing within the current evas.
 */
typedef struct Ewl_Imenu Ewl_Imenu;

/**
 * @def EWL_IMENU(menu)
 * Typecasts a pointer to an Ewl_Imenu pointer.
 */
#define EWL_IMENU(menu) ((Ewl_Imenu *) menu)

/**
 * Inherits from the Ewl_Menu_Base and does not extend the structure, but
 * provides policy for drawing on the current evas.
 */
struct Ewl_Imenu
{
	Ewl_Menu_Base base;		/**< Inherit from Ewl_Menu_base */
};

Ewl_Widget	*ewl_imenu_new(void);
int		 ewl_imenu_init(Ewl_Imenu *menu);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_imenu_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_imenu_expand_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_imenu_popup_destroy_cb(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif
