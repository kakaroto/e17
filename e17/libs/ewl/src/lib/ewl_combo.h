/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_COMBO_H
#define EWL_COMBO_H

#include "ewl_mvc.h"

/**
 * @addtogroup Ewl_Combo Ewl_Combo: A Simple Combo Box
 * Defines a combo box used internally. The contents on the box are not drawn
 * outside of the Evas.
 *
 * @remarks Inherits from Ewl_MVC.
 * @image html Ewl_Combo_inheritance.png
 *
 * @{
 */

/**
 * @themekey /combo/file
 * @themekey /combo/group
 */

/**
 * @def EWL_COMBO_TYPE
 * The type name for the Ewl_Combo widget
 */
#define EWL_COMBO_TYPE "combo"

/**
 * A simple internal combo box, it is limited to drawing within the current
 * evas.
 */
typedef struct Ewl_Combo Ewl_Combo;

/**
 * @def EWL_COMBO(combo)
 * Typecasts a pointer to an Ewl_Combo pointer.
 */
#define EWL_COMBO(combo) ((Ewl_Combo *) combo)

/**
 * Inherits from the Ewl_MVC widget 
 */
struct Ewl_Combo
{
	Ewl_MVC mvc; /**< Inherit from Ewl_MVC */

	Ewl_Widget *popup; 	/**< Use a popup to display with. */
	Ewl_Widget *button; 	/**< expand/contract button */
	Ewl_Widget *header; 	/**< The combo header widget */
	unsigned char editable:1; /**< Is the combo editable */
};

Ewl_Widget 	*ewl_combo_new(void);
int		 ewl_combo_init(Ewl_Combo *combo);

void		 ewl_combo_editable_set(Ewl_Combo *combo, 
						unsigned int editable);
unsigned int	 ewl_combo_editable_get(Ewl_Combo *combo);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_combo_cb_decrement_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_combo_cb_popup_mouse_down(Ewl_Widget *w, void *ev, void *data);
void ewl_combo_cb_item_clicked(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif
