
#ifndef __EWL_CONTAINER_H__
#define __EWL_CONTAINER_H__

struct _ewl_container {
	Ewd_List				  * children;
	Ewd_Free_Cb					free_cb;
	int							recursive;
	Evas_Object					clip_box;
};

#define EWL_CONTAINER(widget) ((Ewl_Container *) widget)

void ewl_container_new(Ewl_Widget * widget);
void ewl_container_append_child(Ewl_Widget * parent,
								Ewl_Widget * child);
void ewl_container_prepend_child(Ewl_Widget * parent,
								 Ewl_Widget * child);
void ewl_container_insert_child(Ewl_Widget * parent,
								Ewl_Widget * child,
								int index);
Ewl_Widget * ewl_container_get_child_at(Ewl_Widget * widget,
										int x,
										int y);
Ewl_Widget * ewl_container_get_child_at_recursive(Ewl_Widget * widget,
												  int x,
												  int y);
void ewl_container_clip_box_create(Ewl_Widget * widget);
void ewl_container_clip_box_resize(Ewl_Widget * widget);
void ewl_container_set_clip(Ewl_Widget * widget);
void ewl_container_show_clip(Ewl_Widget * widget);
void ewl_container_hide_clip(Ewl_Widget * widget);

#endif
