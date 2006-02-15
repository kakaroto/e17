#ifndef __EWL_RADIOBUTTON_H__
#define __EWL_RADIOBUTTON_H__

/**
 * @file ewl_radiobutton.h
 * @defgroup Ewl_Radiobutton Radiobutton: A Radio Button Widget and Grouping System
 * Provides for a simple radiobutton with label, and to group radio buttons
 * for selecting a single option.
 *
 * @{
 */

/**
 * @themekey /radiobutton/file
 * @themekey /radiobutton/group
 */

#define EWL_RADIOBUTTON_TYPE "radiobutton"

/**
 * The radio button provides a means for selecting a single item from a group
 * of options.
 */
typedef struct Ewl_Radiobutton Ewl_Radiobutton;

/**
 * @def EWL_RADIOBUTTON(button)
 * Typecasts a pointer to an Ewl_Radiobutton pointer.
 */
#define EWL_RADIOBUTTON(button) ((Ewl_Radiobutton *) button)

/**
 * Inherits from  Ewl_Checkbutton and extends it to provide grouping buttons
 * to limit to a single selection in a group at a given time.
 */
struct Ewl_Radiobutton
{
	Ewl_Checkbutton   button; /**< Inherit from Ewl_Checkbutton */
	Ecore_List       *chain; /**< List of members of the group */
};

Ewl_Widget     *ewl_radiobutton_new(void);
int             ewl_radiobutton_init(Ewl_Radiobutton *cb);

void            ewl_radiobutton_chain_set(Ewl_Radiobutton *w, Ewl_Radiobutton *c);

/**
 * @def ewl_radiobutton_checked_set(r, c)
 * Shortcut for setting the checked status on the inherited Ewl_Checkbutton
 * fields.
 */
#define ewl_radiobutton_checked_set(r, c) \
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(r), c)

/**
 * @def ewl_radiobutton_is_checked(r)
 * Shortcut for checking the checked status on the inherited Ewl_Checkbutton
 * fields.
 */
#define ewl_radiobutton_is_checked(r) \
	ewl_checkbutton_is_checked(EWL_CHECKBUTTON(r))

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_radiobutton_clicked_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_radiobutton_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif				/* __EWL_RADIOBUTTON_H__ */
