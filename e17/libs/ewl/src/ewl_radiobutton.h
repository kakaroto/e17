#ifndef __EWL_RADIOBUTTON_H__
#define __EWL_RADIOBUTTON_H__

/**
 * @defgroup Ewl_RadioButton A Radio Button Widget and Grouping System
 * Provides for a simple radiobutton with label, and to group radio buttons
 * for selecting a single option.
 *
 * @{
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
	Ewd_List       *chain; /**< List of members of the group */
};

Ewl_Widget     *ewl_radiobutton_new(char *l);
void            ewl_radiobutton_init(Ewl_RadioButton * cb, char *label);

/**
 * @def ewl_radiobutton_set_checked(r, c)
 * Shortcut for setting the checked status on the inherited Ewl_CheckButton
 * fields.
 */
#define ewl_radiobutton_set_checked(r, c) \
	ewl_checkbutton_set_checked(EWL_CHECKBUTTON(r), c)

/**
 * @def ewl_radiobutton_set_checked(r, c)
 * Shortcut for checking the checked status on the inherited Ewl_CheckButton
 * fields.
 */
#define ewl_radiobutton_is_checked(r) \
	ewl_checkbutton_is_checked(EWL_CHECKBUTTON(r))
void            ewl_radiobutton_set_chain(Ewl_Widget * w, Ewl_Widget * c);

/**
 * @}
 */

#endif				/* __EWL_RADIOBUTTON_H__ */
