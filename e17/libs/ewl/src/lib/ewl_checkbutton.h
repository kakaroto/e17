#ifndef EWL_CHECKBUTTON_H
#define EWL_CHECKBUTTON_H

/**
 * @defgroup Ewl_Checkbutton Ewl_Checkbutton: An Ewl_Checkbutton with Label
 * @brief Defines an Ewl_Checkbutton that inherits from Ewl_Widget and
 * provides an Ewl_Check that changes value on each click.
 *
 * @{
 */

/**
 * @themekey /checkbutton/file
 * @themekey /checkbutton/group
 */

#define EWL_CHECKBUTTON_TYPE "checkbutton"

/**
 * The button class is a basic button with a label. This class is inherited by
 * the check button and radio button classes.
 */
typedef struct Ewl_Checkbutton Ewl_Checkbutton;

/**
 * @def EWL_CHECKBUTTON(button)
 * Typecasts a pointer to an Ewl_Checkbutton pointer.
 */
#define EWL_CHECKBUTTON(button) ((Ewl_Checkbutton *) button)

/**
 * Inherits from Ewl_Widget and expands to provide a stateful check button.
 */
struct Ewl_Checkbutton
{
	Ewl_Button      button; /**< Inherit the basic button properties */
	Ewl_Position    label_position; /**< Order of label and check */
	Ewl_Widget     *check; /**< Check widget represented */
};

Ewl_Widget     *ewl_checkbutton_new(void);
int             ewl_checkbutton_init(Ewl_Checkbutton *cb);

void            ewl_checkbutton_checked_set(Ewl_Checkbutton *cb, int checked);
int             ewl_checkbutton_is_checked(Ewl_Checkbutton *cb);

void            ewl_checkbutton_label_position_set(Ewl_Checkbutton *cb,
						   Ewl_Position p);

/*
 * Internally used callbacks, override at your own risk
 */
void ewl_checkbutton_clicked_cb(Ewl_Widget *w, void *ev_data,
					   void *user_data);

/**
 * @}
 */

#endif
