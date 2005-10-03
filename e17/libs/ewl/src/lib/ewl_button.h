#ifndef __EWL_BUTTON_H__
#define __EWL_BUTTON_H__

/**
 * @file ewl_button.h
 * @defgroup Ewl_Button Button: The Basic Button
 * @brief The button class is a basic button with a label. This class inherits
 * from the Ewl_Box to allow for placing any other widget inside the button.
 *
 * @{
 */

/**
 * @themekey /button/file
 * @themekey /button/group
 */

/**
 * The button provides a simple wrapper for creating a clickable Ewl_Widget
 * with an Ewl_Text displayed inside.
 */
typedef struct Ewl_Button Ewl_Button;

/**
 * @def EWL_BUTTON(button)
 * Typecast a pointer to an Ewl_Button pointer.
 */
#define EWL_BUTTON(button) ((Ewl_Button *) button)

/**
 * @struct Ewl_Button
 * @brief A simple Ewl_Widget to provide for a clickable button in the UI.
 * Provides easy facilities for adding a Ewl_Text label to the button, but
 * allows for placing any number of Ewl_Widget's in the Ewl_Button.
 */
struct Ewl_Button
{
	Ewl_Box         box; /**< Inherit from the box for adding widgets */
	Ewl_Widget     *label_object; /**< Labels are common, make it easy */
};

Ewl_Widget     *ewl_button_new(void);
int             ewl_button_init(Ewl_Button * b);
void            ewl_button_label_set(Ewl_Button * b, char *l);
char           *ewl_button_label_get(Ewl_Button *b);

/**
 * @}
 */

#endif				/* __EWL_BUTTON_H__ */
