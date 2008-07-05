/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_SHADOW_H
#define EWL_SHADOW_H

#include "ewl_box.h"

/**
 * @addtogroup Ewl_Shadow Ewl_Shadow: A container with a shadow/border
 * @brief Defines the Ewl_Shadow class used for adding a shadow decoration
 * around a group of widgets.
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Shadow_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /shadow/file
 * @themekey /shadow/group
 */

/**
 * @def EWL_SHADOW_TYPE
 * The type name for the Ewl_Shadow widget
 */
#define EWL_SHADOW_TYPE "shadow"

/**
 * @def EWL_SHADOW_IS(w)
 * Returns TRUE if the widget is an Ewl_Shadow, FALSE otherwise
 */
#define EWL_SHADOW_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_SHADOW_TYPE))

/**
 * Ewl_Shadow provides a container that has a shadowed border.
 */
typedef struct Ewl_Shadow Ewl_Shadow;

/**
 * @def EWL_SHADOW(shadow)
 * Typecast a pointer to an Ewl_Shadow pointer.
 */
#define EWL_SHADOW(shadow) ((Ewl_Shadow *) shadow)

/**
 * @brief Inherits from Ewl_Box to allow drawing a border and shadow
 * decoration around widgets.
 */
struct Ewl_Shadow
{
        Ewl_Box box;       /**< Inherit from Ewl_Box */
};

Ewl_Widget      *ewl_shadow_new(void);
int              ewl_shadow_init(Ewl_Shadow *s);

/**
 * @}
 */

#endif

