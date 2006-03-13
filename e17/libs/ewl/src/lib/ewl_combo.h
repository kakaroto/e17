#ifndef EWL_COMBO_H
#define EWL_COMBO_H

/**
 * @defgroup Ewl_Combo Ewl_Combo: A Simple Combo Box
 * Defines a combo box used internally. The contents on the box are not drawn
 * outside of the Evas.
 *
 * @{
 */

/**
 * @themekey /combo/file
 * @themekey /combo/group
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
 * Inherits from the Ewl_Menu_Base and does not extend the structure, but
 * provides policy for drawing on the current evas.
 */
struct Ewl_Combo
{
	Ewl_Box box; /**< Inherit from box */
	Ewl_Menu_Base *popup; /**< Use a menu to display with. */

	Ewl_Widget *button; /**< expand/contract button */
	Ewl_Widget *selected; /**< Selected widget */
	int selected_idx; /**< The selected row */

	Ewl_Model *model; /**< The model for the combo */
	Ewl_View *view;   /**< The view for the combo */

	void *data;	  /**< The data for the combo */
	unsigned char dirty:1; /**< The combo's data is dirty */
	unsigned char editable:1; /**< Is the combo editable */
};

Ewl_Widget 	*ewl_combo_new(void);
int		 ewl_combo_init(Ewl_Combo *combo);

int		 ewl_combo_selected_get(Ewl_Combo *combo);
void		 ewl_combo_selected_set(Ewl_Combo *combo, int);

void		 ewl_combo_model_set(Ewl_Combo *combo, Ewl_Model *model);
Ewl_Model	*ewl_combo_model_get(Ewl_Combo *combo);

void		 ewl_combo_view_set(Ewl_Combo *combo, Ewl_View *view);
Ewl_View	*ewl_combo_view_get(Ewl_Combo *combo);

void		 ewl_combo_data_set(Ewl_Combo *combo, void *data);
void		*ewl_combo_data_get(Ewl_Combo *combo);

void		 ewl_combo_dirty_set(Ewl_Combo *combo, unsigned int dirty);
unsigned int	 ewl_combo_dirty_get(Ewl_Combo *combo);

void		 ewl_combo_editable_set(Ewl_Combo *combo, 
						unsigned int editable);
unsigned int	 ewl_combo_editable_get(Ewl_Combo *combo);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_combo_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_combo_cb_decrement_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_combo_cb_increment_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_combo_cb_item_clicked(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif
