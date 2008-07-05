/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_MENUBAR_H
#define EWL_MENUBAR_H

#include "ewl_box.h"
#include "ewl_menu.h"

/**
 * @addtogroup Ewl_Menubar Ewl_Menubar: A menu bar widget
 * Defines a class for displaying menu bars
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Menubar_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /menubar/file
 * @themekey /menubar/group
 */

/**
 * @def EWL_MENUBAR_TYPE
 * The type name for the Ewl_Menubar widget
 */
#define EWL_MENUBAR_TYPE "menubar"

/**
 * @def EWL_MENUBAR_IS(w)
 * Returns TRUE if the widget is an Ewl_Menubar, FALSE otherwise
 */
#define EWL_MENUBAR_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_MENUBAR_TYPE))

/**
 * The Ewl_Menubar widget
 */
typedef struct Ewl_Menubar Ewl_Menubar;

/**
 * @def EWL_MENUBAR(menubar)
 * Typecasts a pointer to a Ewl_Menubar pointer
 */
#define EWL_MENUBAR(menubar) ((Ewl_Menubar *) menubar)

/**
 * @brief Inherits from the Ewl_Box class and extends it to provide for
 * a menubar
 */
struct Ewl_Menubar
{
        Ewl_Box                 outer_box;  /**< Inherit from Ewl_Box */
        Ewl_Widget         *inner_box; /**< The box to pack the widgets into */
};

/**
 * Ewl_Menubar_Info
 */
typedef struct Ewl_Menubar_Info Ewl_Menubar_Info;

/**
 * @brief Provides a simple menu structure for the menubar
 */
struct Ewl_Menubar_Info
{
        char *name;                /**< The menu name */
        Ewl_Menu_Info *menu;        /**< The menu items */
};

Ewl_Widget              *ewl_menubar_new(void);
Ewl_Widget              *ewl_hmenubar_new(void);
Ewl_Widget              *ewl_vmenubar_new(void);

int                      ewl_menubar_init(Ewl_Menubar *mb);

void                     ewl_menubar_from_info(Ewl_Menubar *mb,
                                        Ewl_Menubar_Info *info);

void                     ewl_menubar_orientation_set(Ewl_Menubar *mb,
                                        Ewl_Orientation o);
Ewl_Orientation          ewl_menubar_orientation_get(Ewl_Menubar *mb);

void                     ewl_menubar_cb_child_add(Ewl_Container *c,
                                        Ewl_Widget *w);

/**
  * @}
  */

#endif

