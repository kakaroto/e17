#ifndef __EWL_CONTAINER_H__
#define __EWL_CONTAINER_H__

/**
 * @defgroup Ewl_Container The Base Class for Widgets Holding Other Widgets
 * @brief Define the Ewl_Container class which inherits from Ewl_Widget and adds
 * the ability to nest Ewl_Widget's inside.
 *
 * @{
 */

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
typedef void    (*Ewl_Child_Add) (Ewl_Container * c, Ewl_Widget * w);

/**
 * A typedef to shorten the definition of the child_remove callbacks. This
 * callback is container specific and is triggered when an Ewl_Widget is
 * removed from the Ewl_Container.
 */
typedef void    (*Ewl_Child_Remove) (Ewl_Container * c, Ewl_Widget * w);

/**
 * A typedef to shorten the definition of the child_resize callbacks. This
 * callback is container specific and is triggered when an Ewl_Widget is
 * resized in the Ewl_Container.
 */
typedef void    (*Ewl_Child_Resize) (Ewl_Container * c, Ewl_Widget * w,
				     int size, Ewl_Orientation o);

/**
 * @struct Ewl_Container
 * Inherits from the Ewl_Widget and expands to allow for placing child widgets
 * within the available space. Also adds notifiers for various child events.
 */
struct Ewl_Container
{
	Ewl_Widget       widget; /**< Inherit the basics of the widget. */
	Ewd_List        *children; /**< List of children that are contained. */
	Evas_Object     *clip_box; /**< Clip box to bound widgets inside. */
	Ewl_Child_Add    child_add; /**< Function called on child add */
	Ewl_Child_Add    child_remove; /**< Function called on child remove */
	Ewl_Child_Resize child_resize; /**< Function called on child resize */
};

void            ewl_container_init(Ewl_Container * widget, char *appearance,
				   Ewl_Child_Add add, Ewl_Child_Resize rs,
				   Ewl_Child_Remove remove);
void            ewl_container_add_notify(Ewl_Container * container,
					 Ewl_Child_Add add);
void            ewl_container_remove_notify(Ewl_Container * container,
					 Ewl_Child_Add add);
void            ewl_container_resize_notify(Ewl_Container * container,
					    Ewl_Child_Resize resize);
void            ewl_container_append_child(Ewl_Container * parent,
					   Ewl_Widget * child);
void            ewl_container_prepend_child(Ewl_Container * parent,
					    Ewl_Widget * child);
void            ewl_container_insert_child(Ewl_Container * parent,
					   Ewl_Widget * child, int index);
void            ewl_container_remove_child(Ewl_Container * parent,
					   Ewl_Widget * child);
void            ewl_container_resize_child(Ewl_Widget * w, int size,
					   Ewl_Orientation o);
void            ewl_container_reset(Ewl_Container * c);
void            ewl_container_notify_callback(Ewl_Container *c,
					      Ewl_Callback_Type t);
void            ewl_container_intercept_callback(Ewl_Container *c,
					      Ewl_Callback_Type t);
Ewl_Widget     *ewl_container_get_child_at(Ewl_Container * widget, int x,
					   int y);
Ewl_Widget     *ewl_container_get_child_at_recursive(Ewl_Container * widget,
						     int x, int y);
int             ewl_container_parent_of(Ewl_Widget *c, Ewl_Widget *w);
void            ewl_container_prefer_largest(Ewl_Container *c,
					     Ewl_Orientation o);
void            ewl_container_call_child_add(Ewl_Container *c, Ewl_Widget *w);

/**
 * @}
 */

#endif				/* __EWL_CONTAINER_H__ */
