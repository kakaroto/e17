/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_HIGHLIGHT_H
#define EWL_HIGHLIGHT_H

#include "ewl_floater.h"

/**
 * @addtogroup Ewl_Highlight Ewl_Highlight: A highlight widget
 * Defines a widget to highlight other widgets
 *
 * @remarks Inherits from Ewl_Floater.
 * @image html Ewl_Highlight_inheritance.png
 *
 * @{
 */

/**
 * @def EWL_HIGHLIGHT_TYPE
 * The type name for the Ewl_Highlight widget
 */
#define EWL_HIGHLIGHT_TYPE "highlight"

/**
 * @def EWL_HIGHLIGHT
 * Typecasts a pointer to an Ewl_Highlight pointer.
 */
#define EWL_HIGHLIGHT(w) ((Ewl_Highlight *)w)

/**
 * Ewl_Highlight
 */
typedef struct Ewl_Highlight Ewl_Highlight;

/**
 * Inherit from Ewl_Floater
 */
struct Ewl_Highlight
{
	Ewl_Floater floater;		/**< Inherit from Ewl_Floater */
};

Ewl_Widget 	*ewl_highlight_new(void);
int		 ewl_highlight_init(Ewl_Highlight *h);

void		 ewl_highlight_follow_set(Ewl_Highlight *h, Ewl_Widget *w);
Ewl_Widget	*ewl_highlight_follow_get(Ewl_Highlight *h);

/**
 * @}
 */

#endif

