#ifndef __EWL_MENUBAR_H__
#define __EWL_MENUBAR_H__

/**
 * @defgroup Ewl_Menubar Menubar: A menu bar widget
 * @brief Defines a class for displaying menu bars
 *
 * @{
 */

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
	Ewl_Box			outer_box;  /**< Inherit from Ewl_Box */
    Ewl_Box         *inner_box; /**< The box to pack the widgets into */
};

/**
 * @def ewl_menubar_h_new()
 * Shortcut to allocate a new horizontal Ewl_Menubar
 */
#define ewl_hmenubar_new() ewl_menubar_new(EWL_ORIENTATION_HORIZONTAL)

/**
 * @def ewl_menubar_v_new()
 * Shortcut to allocate a new vertical Ewl_Menubar
 */
#define ewl_vmenubar_new() ewl_menubar_new(EWL_ORIENTATION_VERTICAL)

Ewl_Widget *ewl_menubar_new(Ewl_Orientation orientation);
int	    	ewl_menubar_init(Ewl_Menubar *mb, Ewl_Orientation orientation);


Ewl_Widget *ewl_menubar_menu_add(Ewl_Menubar *mb, char *img, char *title);
Ewl_Widget *ewl_menubar_seperator_add(Ewl_Menubar *mb);

#endif

