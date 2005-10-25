#ifndef __EWL_SPACER_H__
#define __EWL_SPACER_H__

/**
 * @file ewl_spacer.h
 * @defgroup Ewl_Spacer Spacer: A Widget to Add Space Between Other Widgets
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
 * The Ewl_Spacer is a simple widget that is used to tweak spacing in an app.
 */
typedef struct Ewl_Spacer Ewl_Spacer;

/**
 * @struct Ewl_Spacer
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

#endif				/* __EWL_SPACER_H__ */
