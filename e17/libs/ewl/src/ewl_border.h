#ifndef __EWL_BORDER_H__
#define __EWL_BORDER_H__

/**
 * @defgroup Ewl_Border Border: A container with a border and label
 * Defines the Ewl_Border class used for adding a border decoration around a
 * group of widgets.
 */

/**
 * @themekey /border/file
 * @themekey /border/group
 */

/**
 * Provides an Ewl_Widget to simply act as a separator between other
 * Ewl_Widget's.
 */
typedef struct Ewl_Border Ewl_Border;

/**
 * @def EWL_BORDER(border)
 * Typecast a pointer to an Ewl_Separator pointer.
 */
#define EWL_BORDER(border) ((Ewl_Border *) border)

/**
 * @struct Ewl_Border
 * @brief Inherits from Ewl_Container to allow drawing a border and label
 * decoration around widgets.
 */
struct Ewl_Border
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */
	Ewl_Widget     *label;     /**< Text label for the border */
	Ewl_Widget     *box;       /**< Box for holding children */
	unsigned int    flags;     /**< Flags for placing the label */
};

Ewl_Widget     *ewl_border_new(char *label);
void            ewl_border_init(Ewl_Border * b, char *label);


#endif				/* __EWL_BORDER_H__ */
