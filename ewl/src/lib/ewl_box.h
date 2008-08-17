/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_BOX_H
#define EWL_BOX_H

/**
 * @addtogroup Ewl_Box Ewl_Box: The Box Layout Container.
 * @brief Defines the Ewl_Box class used for laying out Ewl_Widget's in a
 * horizontal or vertical line.
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Box_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /box/file
 * @themekey /box/group
 */

/**
 * @def EWL_BOX_TYPE
 * The type name for the Ewl_Box widget
 */
#define EWL_BOX_TYPE "box"

/**
 * @def EWL_BOX_IS(w)
 * Returns TRUE if the widget is an Ewl_Box, FALSE otherwise
 */
#define EWL_BOX_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_BOX_TYPE))

/**
 * The box widget is an Ewl_Container and lays out Ewl_Widget's either
 * horizontally or vertically.
 */
typedef struct Ewl_Box Ewl_Box;

/**
 * @def EWL_BOX(box)
 * Typecast a pointer to an Ewl_Box pointer.
 */
#define EWL_BOX(box) ((Ewl_Box *) box)

/**
 * @brief Inherits from an Ewl_Container to provide layout facilities for child
 * widgets placed inside.
 *
 * Layout is done horizontally (left-to-right), or vertically (top-to-bottom).
 * There is also a flag for homogeneous layout, which gives each child Ewl_Widget
 * equal space inside the Ewl_Box.
 */
struct Ewl_Box
{
        Ewl_Container container;    /**< Inherit from Ewl_Container */

        Ewl_Orientation orientation;/**< Indicate the orientation of layout */
        int spacing;                /**< Space between each widget in the box */

        unsigned int homogeneous;   /**< Flag indicating space assignemnt */
};

Ewl_Widget      *ewl_box_new(void);
Ewl_Widget      *ewl_hbox_new(void);
Ewl_Widget      *ewl_vbox_new(void);
int              ewl_box_init(Ewl_Box *box);
void             ewl_box_orientation_set(Ewl_Box *b, Ewl_Orientation o);
Ewl_Orientation  ewl_box_orientation_get(Ewl_Box *b);
void             ewl_box_spacing_set(Ewl_Box *b, int spacing);
void             ewl_box_homogeneous_set(Ewl_Box *b, unsigned int h);
unsigned int     ewl_box_homogeneous_get(Ewl_Box *b);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_box_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
                                                        Ewl_Orientation o);
void ewl_box_cb_child_homogeneous_resize(Ewl_Container *c,
                                Ewl_Widget *w, int size, Ewl_Orientation o);
void ewl_box_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_box_cb_child_hide(Ewl_Container *c, Ewl_Widget *w);
void ewl_box_cb_child_homogeneous_show(Ewl_Container *c, Ewl_Widget *w);

void ewl_box_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_box_cb_configure_homogeneous(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);

/**
 * @}
 */

#endif
