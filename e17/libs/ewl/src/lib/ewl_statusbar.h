#ifndef EWL_BAR_H
#define EWL_BAR_H

/**
 * @defgroup Ewl_Statusbar Ewl_Statusbar: A status bar widget
 * @brief Defines a class for displaying status bars
 *
 * @{
 */

/**
 * @themekey /statusbar/file
 * @themekey /statusbar/group
 */

#define EWL_STATUSBAR_TYPE "statusbar"

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
	Ewl_Box			 outer_box;	/**< Inherit from Ewl_Box */
	Ewl_Widget		*left;		/**< The left container **/
	Ewl_Widget		*status;	/**< The status container */
	Ewl_Widget		*right;		/**< The right container */
	Ecore_List		*stack;		/**< The stack of status' */
	Ewl_Widget		*current;	/**< The currently displayed status */
};

Ewl_Widget	*ewl_statusbar_new(void);
int		 ewl_statusbar_init(Ewl_Statusbar *sb);

void		 ewl_statusbar_left_hide(Ewl_Statusbar *sb);
void		 ewl_statusbar_left_show(Ewl_Statusbar *sb);
void		 ewl_statusbar_right_hide(Ewl_Statusbar *sb);
void		 ewl_statusbar_right_show(Ewl_Statusbar *sb);

void		 ewl_statusbar_active_set(Ewl_Statusbar *sb, Ewl_Position pos);
void		 ewl_statusbar_left_append(Ewl_Statusbar *sb, Ewl_Widget *w);
void		 ewl_statusbar_left_prepend(Ewl_Statusbar *sb, Ewl_Widget *w);
void		 ewl_statusbar_right_append(Ewl_Statusbar *sb, Ewl_Widget *w);
void		 ewl_statusbar_right_prepend(Ewl_Statusbar *sb, Ewl_Widget *w);

void		 ewl_statusbar_push(Ewl_Statusbar *sb, char *txt);
void		 ewl_statusbar_pop(Ewl_Statusbar *sb);

/**
 * @}
 */

#endif

