#ifndef EWL_SEPARATOR_H
#define EWL_SEPARATOR_H

/**
 * @addtogroup Ewl_Separator Ewl_Separator: A Visual Separator Between Widgets
 * Defines the Ewl_Separator class used for drawing lines between widgets when
 * visual separation is needed.
 *
 * @{
 */

/**
 * @themekey /hseparator/file
 * @themekey /hseparator/group
 * @themekey /vseparator/file
 * @themekey /vseparator/group
 */

/**
 * @def EWL_SEPARATOR_TYPE
 * The type name for the Ewl_Separator widget
 */
#define EWL_SEPARATOR_TYPE "separator"

/**
 * Provides an Ewl_Widget to simply act as a separator between other
 * Ewl_Widget's.
 */
typedef struct Ewl_Separator Ewl_Separator;

/**
 * @def EWL_SEPARATOR(separator)
 * Typecast a pointer to an Ewl_Separator pointer.
 */
#define EWL_SEPARATOR(separator) ((Ewl_Separator *) separator)

/**
 * @brief Inherits from Ewl_Widget to allow drawing a horizontal or vertical
 * line between widgets, depending on the orientation the Ewl_Separator.
 */
struct Ewl_Separator
{
	Ewl_Widget      widget; /**< Inherit from Ewl_Widget */
	Ewl_Orientation orientation; /**< Sets drawing horizontal or vertical */
};

Ewl_Widget     *ewl_separator_new(void);
Ewl_Widget     *ewl_hseparator_new(void);
Ewl_Widget     *ewl_vseparator_new(void);
int             ewl_separator_init(Ewl_Separator *s);
void            ewl_separator_orientation_set(Ewl_Separator *s, Ewl_Orientation o);
Ewl_Orientation ewl_separator_orientation_get(Ewl_Separator *s);

/**
 * @}
 */

#endif

