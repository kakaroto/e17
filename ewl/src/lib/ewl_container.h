/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_CONTAINER_H
#define EWL_CONTAINER_H

/**
 * @addtogroup Ewl_Container Ewl_Container: Widgets Holding Other Widgets
 * @brief Define the Ewl_Container class which inherits from Ewl_Widget and adds
 * the ability to nest Ewl_Widget's inside.
 *
 * @remarks Inherits from Ewl_Widget.
 * @if HAVE_IMAGES
 * @image html Ewl_Container_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_CONTAINER_TYPE
 * The type name for the Ewl_Container widget
 */
#define EWL_CONTAINER_TYPE "container"

/**
 * @def EWL_CONTAINER_IS(w)
 * Returns TRUE if the widget is an Ewl_Container, FALSE otherwise
 */
#define EWL_CONTAINER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_CONTAINER_TYPE))

/**
 * This class inherits from Ewl_Widget and provides the capabilities necessary
 * for nesting other widgets inside.
 */
typedef struct Ewl_Container Ewl_Container;

/**
 * @def EWL_CONTAINER(widget)
 * @brief Typecast a poiner to an Ewl_Container pointer.
 */
#define EWL_CONTAINER(widget) ((Ewl_Container *) widget)

/**
 * A typedef to shorten the definition of the child_add callbacks. This
 * callback is container specific and is triggered when an Ewl_Widget is added
 * to the Ewl_Container.
 */
typedef void (*Ewl_Child_Add) (Ewl_Container *c, Ewl_Widget *w);

/**
 * A typedef to shorten the definition of the child_remove callbacks. This
 * callback is container specific and is triggered when an Ewl_Widget is
 * removed from the Ewl_Container.
 */
typedef void (*Ewl_Child_Remove) (Ewl_Container *c, Ewl_Widget *w, int idx);

/**
 * A typedef to shorten the definition of the child_resize callbacks. This
 * callback is container specific and is triggered when an Ewl_Widget is
 * resized in the Ewl_Container.
 */
typedef void (*Ewl_Child_Resize) (Ewl_Container *c, Ewl_Widget *w,
                                        int size, Ewl_Orientation o);

/**
 * A typedef to shorten the definition of the child_show callbacks. This
 * callback is container specific and is triggered when an Ewl_Widget is shown
 * to the Ewl_Container.
 */
typedef void (*Ewl_Child_Show) (Ewl_Container *c, Ewl_Widget *w);

/**
 * A typedef to shorten the definition of the child_hide callbacks. This
 * callback is container specific and is triggered when an Ewl_Widget is hidden
 * from the Ewl_Container.
 */
typedef void (*Ewl_Child_Hide) (Ewl_Container *c, Ewl_Widget *w);

/**
 * A typedef to shorten the definition of the child iterator callbacks. This
 * callback is container specific and is usually set in the container's init
 * function. This is used to pick the next child in the list of children for
 * the container.
 */
typedef Ewl_Widget *(*Ewl_Container_Iterator) (Ewl_Container *c);

/**
 * @brief Inherits from the Ewl_Widget and expands to allow for placing child widgets
 * within the available space. Also adds notifiers for various child events.
 */
struct Ewl_Container
{
        Ewl_Widget widget; /**< Inherit the basics of the widget. */

        Ecore_DList *children; /**< List of children that are contained. */
        int visible_children; /**< the number of visible children */

        Ewl_Container *redirect; /**< Alternate parent for children */

        Ewl_Child_Add child_add; /**< Function called on child add */
        Ewl_Child_Remove child_remove; /**< Function called on child remove */
        Ewl_Child_Resize child_resize; /**< Function called on child resize */
        Ewl_Child_Show child_show; /**< Function called on child show */
        Ewl_Child_Hide child_hide; /**< Function called on child hide */

        Ewl_Container_Iterator iterator; /**< Function to find next child */
};

Ewl_Widget      *ewl_container_new(void);
int              ewl_container_init(Ewl_Container *container);

void             ewl_container_add_notify_set(Ewl_Container *container,
                                                Ewl_Child_Add add);
void             ewl_container_remove_notify_set(Ewl_Container *container,
                                                Ewl_Child_Remove remove);
void             ewl_container_resize_notify_set(Ewl_Container *container,
                                                Ewl_Child_Resize resize);
void             ewl_container_show_notify_set(Ewl_Container *container,
                                                Ewl_Child_Show show);
void             ewl_container_hide_notify_set(Ewl_Container *container,
                                                Ewl_Child_Hide show);

void             ewl_container_child_append(Ewl_Container *parent,
                                                Ewl_Widget *child);
void             ewl_container_child_prepend(Ewl_Container *parent,
                                                Ewl_Widget *child);
void             ewl_container_child_insert(Ewl_Container *parent,
                                                Ewl_Widget *child, int index);
void             ewl_container_child_insert_internal(Ewl_Container *parent,
                                                Ewl_Widget *child, int index);
void             ewl_container_child_remove(Ewl_Container *parent,
                                                Ewl_Widget *child);
void             ewl_container_child_resize(Ewl_Widget *w, int size,
                                                Ewl_Orientation o);
Ewl_Widget      *ewl_container_child_get(Ewl_Container *parent, int index);
Ewl_Widget      *ewl_container_child_internal_get(Ewl_Container *parent,
                                                int index);
int              ewl_container_child_index_get(Ewl_Container *parent,
                                                Ewl_Widget *child);
int              ewl_container_child_index_internal_get(Ewl_Container *parent,
                                                Ewl_Widget *child);
void             ewl_container_child_iterate_begin(Ewl_Container *c);
Ewl_Widget      *ewl_container_child_next(Ewl_Container *c);
void             ewl_container_child_iterator_set(Ewl_Container *c,
                                                Ewl_Container_Iterator i);
int              ewl_container_child_count_get(Ewl_Container *c);
int              ewl_container_child_count_internal_get(Ewl_Container *c);
int              ewl_container_child_count_visible_get(Ewl_Container *c);
void             ewl_container_children_show(Ewl_Container *c);
void             ewl_container_destroy(Ewl_Container *c);
void             ewl_container_reset(Ewl_Container *c);
void             ewl_container_callback_notify(Ewl_Container *c,
                                                unsigned int t);
void             ewl_container_callback_nonotify(Ewl_Container *c,
                                                unsigned int t);
void             ewl_container_callback_intercept(Ewl_Container *c,
                                                unsigned int t);
void             ewl_container_callback_nointercept(Ewl_Container *c,
                                                unsigned int t);
Ewl_Widget      *ewl_container_child_at_get(Ewl_Container *widget,
                                                int x, int y);
Ewl_Widget      *ewl_container_child_at_recursive_get(Ewl_Container *widget,
                                                int x, int y);
void             ewl_container_largest_prefer(Ewl_Container *c,
                                                Ewl_Orientation o);
void             ewl_container_sum_prefer(Ewl_Container *c,
                                                Ewl_Orientation o);

void             ewl_container_child_add_call(Ewl_Container *c, Ewl_Widget *w);
void             ewl_container_child_remove_call(Ewl_Container *c,
                                                Ewl_Widget *w, int idx);
void             ewl_container_child_show_call(Ewl_Container *c, Ewl_Widget *w);
void             ewl_container_child_hide_call(Ewl_Container *c, Ewl_Widget *w);

Ewl_Container   *ewl_container_end_redirect_get(Ewl_Container *c);
Ewl_Container   *ewl_container_redirect_get(Ewl_Container *c);
void             ewl_container_redirect_set(Ewl_Container *c,
                                                Ewl_Container *rc);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_container_cb_obscure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_container_cb_realize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_container_cb_configure(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_container_cb_reparent(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_container_cb_unrealize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_container_cb_enable(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_container_cb_disable(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_container_cb_container_focus_out(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_container_cb_widget_focus_out(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_container_cb_widget_focus_in(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
