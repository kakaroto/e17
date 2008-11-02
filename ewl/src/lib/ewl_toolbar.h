/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TOOLBAR_H
#define EWL_TOOLBAR_H

#include "ewl_menubar.h"

/**
 * @addtogroup Ewl_Toolbar Ewl_Toolbar: A toolbar widget
 * @brief Defines a widget to contain a toolbar
 *
 * @remarks Inherits from Ewl_Menubar.
 * @if HAVE_IMAGES
 * @image html Ewl_Toolbar_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_TOOLBAR_TYPE
 * The type name for the Ewl_Toolbar widget
 */
#define EWL_TOOLBAR_TYPE "toolbar"

/**
 * @def EWL_TOOLBAR_IS(w)
 * Returns TRUE if the widget is an Ewl_Toolbar, FALSE otherwise
 */
#define EWL_TOOLBAR_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TOOLBAR_TYPE))

/**
 * Ewl_Toolbar
 */
typedef struct Ewl_Toolbar Ewl_Toolbar;

/**
 * @def EWL_TOOLBAR(toolbar)
 * Typecast a pointer to an Ewl_Toolbar pointer.
 */
#define EWL_TOOLBAR(toolbar) ((Ewl_Toolbar *) toolbar)

/**
 * @brief Inherits from Ewl_Menubar and extends to provide a toolbar layout
 */
struct Ewl_Toolbar
{
        Ewl_Menubar     menubar; /**< Inherit from Ewl_Menubar */
        Ewl_Icon_Part   hidden;  /**< which part of icons should be hidden */
};

Ewl_Widget     *ewl_htoolbar_new(void);
Ewl_Widget     *ewl_vtoolbar_new(void);

Ewl_Widget     *ewl_toolbar_new(void);
int             ewl_toolbar_init(Ewl_Toolbar *t);

void            ewl_toolbar_orientation_set(Ewl_Toolbar *t, Ewl_Orientation o);
Ewl_Orientation ewl_toolbar_orientation_get(Ewl_Toolbar *t);

void            ewl_toolbar_icon_part_hide(Ewl_Toolbar *t, Ewl_Icon_Part part);
Ewl_Icon_Part   ewl_toolbar_icon_part_hidden_get(Ewl_Toolbar *t);

/*
 * Internal functions
 */
void            ewl_toolbar_cb_child_add(Ewl_Container *c, Ewl_Widget *w);

/**
 * @}
 */

#endif

