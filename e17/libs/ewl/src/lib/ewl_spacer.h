#ifndef EWL_SPACER_H
#define EWL_SPACER_H

/**
 * @addtogroup Ewl_Spacer Ewl_Spacer: A Widget to Add Space Between Other Widgets
 * Defines an Ewl_Widget to be used for adding space between widgets in a
 * layout.
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
 * The Ewl_Spacer is a simple widget that is used to tweak spacing in an app.
 */
typedef struct Ewl_Spacer Ewl_Spacer;

/**
 * Inherits from Ewl_Widget, and does not provide any further functionality,
 * it is intended to be themed as transparent, and used to tweak spacing in an
 * application.
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

Ewl_Widget	*ewl_spacer_new(void);
int		 ewl_spacer_init(Ewl_Spacer *s);

/**
 * @}
 */

#endif
