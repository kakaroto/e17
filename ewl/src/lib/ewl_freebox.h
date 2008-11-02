/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_FREEBOX_H
#define EWL_FREEBOX_H

/**
 * @addtogroup Ewl_Freebox Ewl_Freebox: A Freelayout widget
 * @brief Defines a class to allow the user to specify the layout type for
 * the widget.
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Freebox_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_FREEBOX_TYPE
 * The type name for the Ewl_Freebox widget
 */
#define EWL_FREEBOX_TYPE "freebox"

/**
 * Callback to compare two widgets to determine which is first. If the
 * return is < 0 then a is first. If the return is > 0 then b is first. If
 * the return == 0 then they are equal
 */
typedef int (*Ewl_Freebox_Comparator)(Ewl_Widget *a, Ewl_Widget *b);

/**
 * @def EWL_FREEBOX(box)
 * Typecasts a pointer to an Ewl_Freebox pointer.
 */
#define EWL_FREEBOX(box) ((Ewl_Freebox *)box)

/**
 * @def EWL_FREEBOX_IS(w)
 * Returns TRUE if the widget is an Ewl_Freebox, FALSE otherwise
 */
#define EWL_FREEBOX_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_FREEBOX_TYPE))

/**
 * A free layout container.
 */
typedef struct Ewl_Freebox Ewl_Freebox;

/**
 * @brief Inherits from Ewl_Container and extends to allow for differnt layout
 * algorithms
 */
struct Ewl_Freebox
{
        Ewl_Container container;        /**< Inherit from Ewl_Container */

        Ewl_Freebox_Layout_Type layout; /**< The current layout type */
        Ewl_Freebox_Comparator comparator;      /**< The item comparator */
        Ewl_Orientation orientation;    /**< The orientation of the box */
        unsigned short sorted;          /**< Is the box sorted */
};

Ewl_Widget              *ewl_vfreebox_new(void);
Ewl_Widget              *ewl_hfreebox_new(void);
Ewl_Widget              *ewl_freebox_new(void);
int                      ewl_freebox_init(Ewl_Freebox *fb);

void                     ewl_freebox_orientation_set(Ewl_Freebox *fb,
                                        Ewl_Orientation orientation);
Ewl_Orientation          ewl_freebox_orientation_get(Ewl_Freebox *fb);

void                     ewl_freebox_layout_type_set(Ewl_Freebox *fb,
                                        Ewl_Freebox_Layout_Type type);
Ewl_Freebox_Layout_Type  ewl_freebox_layout_type_get(Ewl_Freebox *fb);

void                     ewl_freebox_comparator_set(Ewl_Freebox *fb,
                                        Ewl_Freebox_Comparator cmp);
Ewl_Freebox_Comparator   ewl_freebox_comparator_get(Ewl_Freebox *fb);

void                     ewl_freebox_resort(Ewl_Freebox *fb);

/*
 * Internal callbacks, override at your risk
 */
void ewl_freebox_cb_configure(Ewl_Widget *w, void *ev, void *data);

void ewl_freebox_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_freebox_cb_child_show(Ewl_Container *c, Ewl_Widget *w);

/**
 * @}
 */

#endif

