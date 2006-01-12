#ifndef __EWL_MENUBAR_H__
#define __EWL_MENUBAR_H__

/**
 * @file ewl_menubar.h
 * @defgroup Ewl_Menubar Menubar: A menu bar widget
 * @brief Defines a class for displaying menu bars
 *
 * @{
 */

/**
 * @themekey /menubar/file
 * @themekey /menubar/group
 */

#define EWL_MENUBAR_TYPE "menubar"

/**
 */
typedef struct Ewl_Menubar Ewl_Menubar;

/**
 * @def EWL_MENUBAR(menubar)
 * Typecasts a pointer to a Ewl_Menubar pointer
 */
#define EWL_MENUBAR(menubar) ((Ewl_Menubar *) menubar)

/**
 * @struct Ewl_Menubar
 * Inherits from the Ewl_Box class and extends it to provide for
 * a menubar
 */
struct Ewl_Menubar
{
	Ewl_Box		 outer_box;  /**< Inherit from Ewl_Box */
	Ewl_Widget 	*inner_box; /**< The box to pack the widgets into */
};

Ewl_Widget 	*ewl_menubar_new(void);
Ewl_Widget	*ewl_hmenubar_new(void);
Ewl_Widget	*ewl_vmenubar_new(void);
int	    	 ewl_menubar_init(Ewl_Menubar *mb);

void		 ewl_menubar_orientation_set(Ewl_Menubar *mb, Ewl_Orientation o);
Ewl_Orientation	 ewl_menubar_orientation_get(Ewl_Menubar *mb);

void		ewl_menubar_cb_child_add(Ewl_Container *c, Ewl_Widget *w);

/**
  * @}
  */

#endif

