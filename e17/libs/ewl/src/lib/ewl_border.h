/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_BORDER_H
#define EWL_BORDER_H

#include "ewl_box.h"

/**
 * @addtogroup Ewl_Border Ewl_Border: A container with a border and label
 * @brief Defines the Ewl_Border class used for adding a border decoration
 * around a group of widgets.
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Border_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /border/file
 * @themekey /border/group
 */

/**
 * @def EWL_BORDER_TYPE
 * The type name for the Ewl_Border widget
 */
#define EWL_BORDER_TYPE "border"

/**
 * @def EWL_BORDER_IS(w)
 * Returns TRUE if the widget is an Ewl_Border widget, FALSE otherwise
 */
#define EWL_BORDER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_BORDER_TYPE))

/**
 * Ewl_Border provides a container that has a labeled border.
 */
typedef struct Ewl_Border Ewl_Border;

/**
 * @def EWL_BORDER(border)
 * Typecast a pointer to an Ewl_Separator pointer.
 */
#define EWL_BORDER(border) ((Ewl_Border *) border)

/**
 * @brief Inherits from Ewl_Box to allow drawing a border and label
 * decoration around widgets.
 */
struct Ewl_Border
{
        Ewl_Box box;                         /**< Inherit from Ewl_Box */
        Ewl_Widget *label;                 /**< Text label for the border */
        Ewl_Widget *body;                /**< Box for holding children */
        Ewl_Position label_position;        /**< Flags for placing the label */
};

Ewl_Widget      *ewl_border_new(void);
int              ewl_border_init(Ewl_Border * b);

void             ewl_border_label_set(Ewl_Border *b, const char *t);
const char      *ewl_border_label_get(Ewl_Border *b);

void             ewl_border_label_position_set(Ewl_Border *b, Ewl_Position pos);
Ewl_Position     ewl_border_label_position_get(Ewl_Border *b);

void             ewl_border_label_alignment_set(Ewl_Border *b,
                                                unsigned int align);
unsigned int     ewl_border_label_alignment_get(Ewl_Border *b);

/**
 * @}
 */

#endif

