
/*\
|*|
|*| The container is a sub class for each widget, Ewl_Widget structure holds it
|*| and is used to keep all subwidgets for a widget.
|*|
\*/

#ifndef __EWL_CONTAINER_H__
#define __EWL_CONTAINER_H__

typedef struct _ewl_container Ewl_Container;

#define EWL_CONTAINER(widget) ((Ewl_Container *) widget)

typedef void    (*Ewl_Child_Add) (Ewl_Container * c, Ewl_Widget * w);
typedef void    (*Ewl_Child_Remove) (Ewl_Container * c, Ewl_Widget * w);
typedef void    (*Ewl_Child_Resize) (Ewl_Container * c, Ewl_Widget * w,
				     int size, Ewl_Orientation o);

struct _ewl_container
{
	/*
	 * Inherit the basics of the widget.
	 */
	Ewl_Widget      widget;

	/*
	 * The list of children that are contained.
	 */
	Ewd_List       *children;

	/*
	 * The clip box of the container to bound the widgets of the
	 * container.
	 */
	Evas_Object    *clip_box;

	/*
	 * Addition function updates the preferred size of the container when
	 * a child is added.
	 */
	Ewl_Child_Add   child_add;

	/*
	 * Removal function updates the preferred size of the container when
	 * a child is removed.
	 */
	Ewl_Child_Add   child_remove;

	/*
	 * Changes the preferred size of the container when the preferred size
	 * of the child changes.
	 */
	Ewl_Child_Resize child_resize;
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
void            ewl_container_prefer_largest(Ewl_Container *c,
					     Ewl_Orientation o);

#endif				/* __EWL_CONTAINER_H__ */
