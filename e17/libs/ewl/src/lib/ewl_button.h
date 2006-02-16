#ifndef EWL_BUTTON_H
#define EWL_BUTTON_H

/**
 * @defgroup Ewl_Button Ewl_Button: The Basic Button
 * @brief The button class is a basic button with a label. This class inherits
 * from the Ewl_Box to allow for placing any other widget inside the button.
 *
 * @{
 */

/**
 * @themekey /button/file
 * @themekey /button/group
 */

#define EWL_BUTTON_TYPE "button"

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
 * @brief A simple Ewl_Widget to provide for a clickable button in the UI.
 * Provides easy facilities for adding a Ewl_Text label to the button, and
 * a Ewl_Image but allows for placing any number of Ewl_Widget's in the Ewl_Button.
 */
struct Ewl_Button
{
	Ewl_Box         box; 		/**< Inherit from the box for adding widgets */
	Ewl_Widget     *body;		/**< The body of the button */
	Ewl_Widget     *label_object;	/**< Labels are common, make it easy */
	Ewl_Widget     *image_object;	/**< Add an image to the button if needed */
	Ewl_Stock_Type	stock_type;	/**< The stock type of the button */
};

Ewl_Widget	*ewl_button_new(void);
int		 ewl_button_init(Ewl_Button *b);
void		 ewl_button_label_set(Ewl_Button *b, const char *l);
const char	*ewl_button_label_get(Ewl_Button *b);

void		 ewl_button_stock_type_set(Ewl_Button *b, Ewl_Stock_Type stock);
Ewl_Stock_Type	 ewl_button_stock_type_get(Ewl_Button *b);

void		 ewl_button_image_set(Ewl_Button *b, const char *file, const char *key);
const char	*ewl_button_image_get(Ewl_Button *b);

/*
 * Internal
 */
void ewl_button_cb_key_down(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

