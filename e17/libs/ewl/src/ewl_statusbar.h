#ifndef __EWL_BAR_H__
#define __EWL_BAR_H__

/**
 * @defgroup Ewl_Statusbar Bar: A status bar widget
 * @brief Defines a class for displaying status bars
 *
 * @{
 */

/**
 */
typedef struct Ewl_Statusbar Ewl_Statusbar;

/**
 * @def EWL_STATUSBAR(statusbar)
 * Typecasts a pointer to a Ewl_Statusbar pointer
 */
#define EWL_STATUSBAR(statusbar) ((Ewl_Statusbar *) statusbar)

/**
 * @struct Ewl_Statusbar
 * Inherits from the Ewl_Box class and extends it to provide for
 * a simple status bar
 */
struct Ewl_Statusbar
{
	Ewl_Box			outer_box;	/**< Inherit from Ewl_Box */
	Ewl_Widget		*left;		/**< The left container **/
	Ewl_Widget		*status;	/**< The status container */
	Ewl_Widget		*right;		/**< The right container */
	Ecore_List		*stack;		/**< The stack of status' */
	Ewl_Widget		*current;	/**< The currently displayed status */
};

Ewl_Widget     *ewl_statusbar_new(void);
int		ewl_statusbar_init(Ewl_Statusbar *sb);

void		ewl_statusbar_push(Ewl_Statusbar *sb, char *txt);
void		ewl_statusbar_pop(Ewl_Statusbar *sb);

#endif

