/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_BUTTON_H
#define EWL_BUTTON_H

#include "ewl_stock.h"
#include "ewl_view.h"

/**
 * @addtogroup Ewl_Button Ewl_Button: The Basic Button
 * @brief The button class is a basic button with a label. This class inherits
 * from the Ewl_Box to allow for placing any other widget inside the button.
 *
 * @remarks Inherits from Ewl_Stock.
 * @if HAVE_IMAGES
 * @image html Ewl_Button_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /button/file
 * @themekey /button/group
 */

/**
 * @def EWL_BUTTON_TYPE
 * The type name for the Ewl_Button widget
 */
#define EWL_BUTTON_TYPE "button"

/**
 * @def EWL_BUTTON_IS(w)
 * Returns TRUE if the widget is an Ewl_Button, FALSE otherwise
 */
#define EWL_BUTTON_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_BUTTON_TYPE))

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
 *
 * Provides easy facilities for adding a Ewl_Label label to the button, and
 * a Ewl_Image but allows for placing any number of Ewl_Widget's in the Ewl_Button.
 */
struct Ewl_Button
{
        Ewl_Stock stock;                 /**< Inherit from the box for adding widgets */
        Ewl_Widget *body;                /**< The body of the button */
        Ewl_Widget *label_object;        /**< Labels are common, make it easy */
        Ewl_Widget *image_object;        /**< Add an image to the button if needed */
};

Ewl_Widget       *ewl_button_new(void);
int               ewl_button_init(Ewl_Button *b);
Ewl_View         *ewl_button_view_get(void);

void              ewl_button_label_set(Ewl_Button *b, const char *l);
const char       *ewl_button_label_get(Ewl_Button *b);

void              ewl_button_image_set(Ewl_Button *b, const char *file, const char *key);
const char       *ewl_button_image_get(Ewl_Button *b);

void              ewl_button_image_size_set(Ewl_Button *b, int width, int height);
void              ewl_button_image_size_get(Ewl_Button *b, int *width, int *height);

unsigned int      ewl_button_alignment_get(Ewl_Button *b);
void              ewl_button_alignment_set(Ewl_Button *b, unsigned int align);

unsigned int      ewl_button_fill_policy_get(Ewl_Button *b);
void              ewl_button_fill_policy_set(Ewl_Button *b, unsigned int fill);

/*
 * Internal
 */
void ewl_button_cb_key_down(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

