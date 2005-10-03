#ifndef __EWL_RADIOBUTTON_H__
#define __EWL_RADIOBUTTON_H__

/**
 * @file ewl_radiobutton.h
 * @defgroup Ewl_RadioButton RadioButton: A Radio Button Widget and Grouping System
 * Provides for a simple radiobutton with label, and to group radio buttons
 * for selecting a single option.
 *
 * @{
 */

/**
 * @themekey /radiobutton/file
 * @themekey /radiobutton/group
 */

/**
 * The radio button provides a means for selecting a single item from a group
 * of options.
 */
typedef struct Ewl_RadioButton Ewl_RadioButton;

/**
 * @def EWL_RADIOBUTTON(button)
 * Typecasts a pointer to an Ewl_RadioButton pointer.
 */
#define EWL_RADIOBUTTON(button) ((Ewl_RadioButton *) button)

/**
 * Inherits from  Ewl_CheckButton and extends it to provide grouping buttons
 * to limit to a single selection in a group at a given time.
 */
struct Ewl_RadioButton
{
	Ewl_CheckButton button; /**< Inherit from Ewl_CheckButton */
	Ecore_List       *chain; /**< List of members of the group */
};

Ewl_Widget     *ewl_radiobutton_new(void);
int             ewl_radiobutton_init(Ewl_RadioButton * cb);

/**
 * @def ewl_radiobutton_checked_set(r, c)
 * Shortcut for setting the checked status on the inherited Ewl_CheckButton
 * fields.
 */
#define ewl_radiobutton_checked_set(r, c) \
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(r), c)

/**
 * @def ewl_radiobutton_is_checked(r)
 * Shortcut for checking the checked status on the inherited Ewl_CheckButton
 * fields.
 */
#define ewl_radiobutton_is_checked(r) \
	ewl_checkbutton_is_checked(EWL_CHECKBUTTON(r))
void            ewl_radiobutton_chain_set(Ewl_RadioButton * w, Ewl_RadioButton * c);

/*
 * Internally used callbacks, override at your own risk.
 */
void            ewl_radiobutton_clicked_cb(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            ewl_radiobutton_destroy_cb(Ewl_Widget * w, void *ev_data,
					   void *user_data);

/**
 * @}
 */

#endif				/* __EWL_RADIOBUTTON_H__ */
