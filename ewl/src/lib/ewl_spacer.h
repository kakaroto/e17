/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_SPACER_H
#define EWL_SPACER_H

/**
 * @addtogroup Ewl_Spacer Ewl_Spacer: A Widget to Add Space Between Other Widgets
 * Defines an Ewl_Widget to be used for adding space between widgets in a
 * layout.
 *
 * @remarks Inherits from Ewl_Widget.
 * @if HAVE_IMAGES
 * @image html Ewl_Spacer_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /spacer/file
 * @themekey /spacer/group
 */

/**
 * @def EWL_SPACER_TYPE
 * The type name for the Ewl_Spacer widget
 */
#define EWL_SPACER_TYPE "spacer"

/**
 * @def EWL_SPACER_IS(w)
 * Returns TRUE if the widget is an Ewl_Spacer, FALSE otherwise
 */
#define EWL_SPACER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_SPACER_TYPE))

/**
 * The Ewl_Spacer is a simple widget that is used to tweak spacing in an app.
 */
typedef struct Ewl_Spacer Ewl_Spacer;

/**
 * @brief Inherits from Ewl_Widget and provides as a spacer between widgets
 */
struct Ewl_Spacer
{
        Ewl_Widget widget; /**< Inherit from Ewl_Widget */
};

/**
 * @def EWL_SPACER(spacer)
 * Typecasts a pointer to an Ewl_Spacer pointer.
 */
#define EWL_SPACER(spacer) ((Ewl_Spacer *) spacer)

Ewl_Widget      *ewl_spacer_new(void);
int              ewl_spacer_init(Ewl_Spacer *s);

/**
 * @}
 */

#endif
