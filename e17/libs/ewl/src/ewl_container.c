
#include <Ewl.h>

Ewl_Container *
ewl_container_new()
{
	Ewl_Container * container = NULL;

	container = malloc(sizeof(Ewl_Container));
	memset(container, 0, sizeof(Ewl_Container));

	container->children = ewd_list_new();

	return container;
}

void
ewl_container_append_child(Ewl_Widget * parent, Ewl_Widget * child)
{
	CHECK_PARAM_POINTER("parent", parent);
	CHECK_PARAM_POINTER("child", child);

	child->evas = EWL_WIDGET(parent)->evas;
	child->evas_window = EWL_WIDGET(parent)->evas_window;
	child->parent = parent;

	EWL_OBJECT(child)->layer = EWL_OBJECT(parent)->layer+1;

	if (!parent->container.children)
		parent->container.children = ewd_list_new();
	if (parent->container.free_cb)
		ewd_list_set_free_cb(parent->container.children,
							 parent->container.free_cb);

	ewd_list_append(parent->container.children, child);
}

void
ewl_container_prepend_child(Ewl_Widget * parent, Ewl_Widget * child)
{
	CHECK_PARAM_POINTER("parent", parent);
	CHECK_PARAM_POINTER("child", child);

	child->evas = EWL_WIDGET(parent)->evas;
	child->evas_window = EWL_WIDGET(parent)->evas_window;
	child->parent = parent;

	EWL_OBJECT(child)->layer = EWL_OBJECT(parent)->layer+5;

	if (!parent->container.children)
		parent->container.children = ewd_list_new();

	ewd_list_prepend(parent->container.children, child);
}

void
ewl_container_set_free_callback(Ewl_Widget * widget, Ewd_Free_Cb cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewd_list_set_free_cb(widget->container.children, cb);
}

void
ewl_container_insert_child(Ewl_Widget * parent, Ewl_Widget * child)
{
	ewd_list_insert(parent->container.children, child);
}

Ewl_Widget *
ewl_container_get_child_at(Ewl_Widget * widget, int x, int y)
{
	Ewl_Widget * child = NULL;

	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	if (!widget->container.children ||
			ewd_list_is_empty(widget->container.children))
		return NULL;

	ewd_list_goto_first(widget->container.children);

	while ((child = ewd_list_next(widget->container.children)) != NULL) {
		if (x >= EWL_OBJECT(child)->current.x &&
			y >= EWL_OBJECT(child)->current.y &&
			EWL_OBJECT(child)->current.x + EWL_OBJECT(child)->current.w >= x &&
			EWL_OBJECT(child)->current.y + EWL_OBJECT(child)->current.h >= y)
			return child;
	}

	return NULL;
}

Ewl_Widget *
ewl_container_get_child_at_recursive(Ewl_Widget * widget, int x, int y)
{
	Ewl_Widget * child = NULL, * child2 = NULL;

	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	if (!widget->container.children ||
		ewd_list_is_empty(widget->container.children))
		return NULL;

	ewd_list_goto_first(widget->container.children);

	child = widget;

	while ((child2 = ewl_container_get_child_at(child, x, y)) != NULL) {
		if (child2->container.recursive)
			child = ewl_container_get_child_at(child, x, y);
		else
			return child2;
	}

	return NULL;
}
