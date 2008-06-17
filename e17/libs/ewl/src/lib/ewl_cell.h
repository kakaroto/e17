/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_CELL_H
#define EWL_CELL_H

/**
 * @addtogroup Ewl_Cell Ewl_Cell: The Cell Container, Layout for a Single Widget
 * Defines a container to layout a single child with all of it's available space.
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Cell_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /cell/file
 * @themekey /cell/group
 */

/**
 * @def EWL_CELL_TYPE
 * The type name for the Ewl_Cell widget
 */
#define EWL_CELL_TYPE "cell"

/**
 * @def EWL_CELL_IS(w)
 * Returns TRUE if the widget is an Ewl_Cell, FALSE otherwise
 */
#define EWL_CELL_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_CELL_TYPE))

/**
 * The Ewl_Cell widget
 */
typedef struct Ewl_Cell Ewl_Cell;

/**
* @def EWL_CELL(t)
* Typecast a pointer to an Ewl_Cell pointer.
*/
#define EWL_CELL(t) ((Ewl_Cell *)t)

/**
 * @brief Inherits from Ewl_Container for packing widgets inside a cell.
 */
struct Ewl_Cell
{
        Ewl_Container container; /**< Inherit from Ewl_Container */
};

Ewl_Widget       *ewl_cell_new(void);
int               ewl_cell_init(Ewl_Cell *cell);
void              ewl_cell_state_change_cb_add(Ewl_Cell *cell);
void              ewl_cell_state_change_cb_del(Ewl_Cell *cell);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_cell_cb_configure(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_cell_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_cell_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
                                                        Ewl_Orientation o);

/**
 * @}
 */

#endif
