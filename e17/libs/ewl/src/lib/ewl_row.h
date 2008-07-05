/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ROW_H
#define EWL_ROW_H

/**
 * @addtogroup Ewl_Row Ewl_Row: The Row widget
 * Defines a widget to display a row
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Row_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /row/file
 * @themekey /row/group
 */

/**
 * @def EWL_ROW_TYPE
 * The type name for the Ewl_Row widget
 */
#define EWL_ROW_TYPE "row"

/**
 * @def EWL_ROW_IS(w)
 * Returns TRUE if the widget is an Ewl_Row, FALSE otherwise
 */
#define EWL_ROW_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_ROW_TYPE))

/**
 * The Ewl_Row widget
 */
typedef struct Ewl_Row Ewl_Row;

/**
 * @def EWL_ROW(t)
 * Typecasts a pointer to an Ewl_Row pointer
 */
#define EWL_ROW(t) ((Ewl_Row *)t)

/**
 * @brief Inherits from Ewl_Container to create a tree row that
 * lays out cells in a left to right fashion.
 */
struct Ewl_Row
{
        Ewl_Container container;        /**< Inherit from Ewl_Container */
        Ewl_Container *header;                /**< The header row */
};

Ewl_Widget      *ewl_row_new(void);
int              ewl_row_init(Ewl_Row *row);

void             ewl_row_header_set(Ewl_Row *row, Ewl_Container *header);
Ewl_Widget      *ewl_row_column_get(Ewl_Row *row, short n);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_row_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_row_cb_header_configure(Ewl_Widget *w, void *ev_data,
                                                void *user_data);
void ewl_row_cb_header_destroy(Ewl_Widget *w, void *ev_data,
                                                void *user_data);

void ewl_row_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_row_cb_child_hide(Ewl_Container *c, Ewl_Widget *w);
void ewl_row_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
                                               Ewl_Orientation o);

/**
 * @}
 */

#endif
