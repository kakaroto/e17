#include "container.h"

EwlWidget *ewl_container_new()
{
	EwlWidget *widget = EWL_WIDGET(malloc(sizeof(EwlContainer)));
	ewl_container_init(widget);
	return widget;
}

void       ewl_container_init(EwlWidget *widget)
{
	EwlContainer *container = EWL_CONTAINER(widget);
	ewl_widget_init(widget);
	ewl_set(widget, "/object/type", ewl_string_dup("EwlContainer"));
	ewl_widget_set_flag(widget, "is_container", TRUE);
	
	container->children = ewl_list_new();

	ewl_callback_add(widget, "realize",
	                 ewl_container_handle_realize, NULL);
	ewl_callback_push(widget, "unrealize",
	                  ewl_container_handle_unrealize, NULL);
	ewl_callback_add(widget, "show",
	                 ewl_container_handle_show, NULL);
	ewl_callback_add(widget, "hide",
	                 ewl_container_handle_hide, NULL);
	ewl_callback_add(widget, "move",
	                 ewl_container_handle_move, NULL);
	ewl_callback_add(widget, "resize",
	                 ewl_container_handle_resize, NULL);

	return;
}

void       ewl_container_free_cb(EwlListNode *node, void *data)
{
	UNUSED(data);
	ewl_widget_free(EWL_WIDGET(node->data));
	return;
}

void       ewl_container_free(EwlWidget *widget)
{
	EwlContainer *container = EWL_CONTAINER(widget);
	ewl_list_foreach(container->children, ewl_container_free_cb, NULL);
	ewl_list_free(container->children);
	ewl_widget_free(widget);
	return;
}


void       ewl_container_insert(EwlWidget *widget, EwlWidget *child)
{
	EwlContainer *container = EWL_CONTAINER(widget);
	ewl_list_insert(container->children, ewl_list_node_new(child));
	ewl_event_queue_new("resize", widget);
	return;
}

void       ewl_container_push(EwlWidget *widget, EwlWidget *child)
{
	EwlContainer *container = EWL_CONTAINER(widget);
	ewl_list_push(container->children, ewl_list_node_new(child));
	ewl_event_queue_new("resize", widget);
	return;
}

void       ewl_container_remove(EwlWidget *widget, EwlWidget *child)
{
	EwlContainer *container = EWL_CONTAINER(widget);
	EwlListNode *node = ewl_list_find_by_value(container->children, child);
	if (node)	{
		ewl_list_remove(container->children, node);
		ewl_event_queue_new("resize", widget);
	}
	return;
}


void       ewl_container_foreach_cb(EwlListNode *node, void *data)
{
	EwlContainerPrivateData *pd = (EwlContainerPrivateData*)data;
	pd->callback(EWL_WIDGET(node->data), pd->data);
	return;
}

void       ewl_container_foreach(EwlWidget *widget,
                                 void     (*cb)(EwlWidget *child,
                                                void      *data),
                                 void      *data)
{
	EwlContainer *container = EWL_CONTAINER(widget);
	EwlContainerPrivateData pd;
	pd.callback = cb;
	pd.data = data;

	ewl_list_foreach(container->children, ewl_container_foreach_cb, &pd);
	return;
}


void       ewl_container_handle_realize_cb(EwlWidget *widget, void *data)
{
	UNUSED(data);
	ewl_widget_realize(widget);
	return;
}

void       ewl_container_handle_realize(void      *object,
                                        EwlEvent  *event,
                                        void      *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	UNUSED(event);
	UNUSED(data);

	ewl_container_foreach(widget, ewl_container_handle_realize_cb, NULL);
	
	return;
}

void       ewl_container_handle_unrealize_cb(EwlWidget *widget, void *data)
{
	UNUSED(data);
	ewl_widget_unrealize(widget);
	return;
}

void       ewl_container_handle_unrealize(void      *object,
                                          EwlEvent  *event,
                                          void      *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	UNUSED(event);
	UNUSED(data);

	ewl_container_foreach(widget, ewl_container_handle_unrealize_cb, NULL);
	
	return;
}

void       ewl_container_handle_show_cb(EwlWidget *widget, void *data)
{
	UNUSED(data);
	ewl_widget_show(widget);
	return;
}

void       ewl_container_handle_show(void      *object,
                                     EwlEvent  *event,
                                     void      *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	UNUSED(object); 
	UNUSED(event);
	UNUSED(data);
	ewl_container_foreach(widget, ewl_container_handle_show_cb, NULL);
	return;
}

void       ewl_container_handle_hide_cb(EwlWidget *widget, void *data)
{
	UNUSED(data);
	ewl_widget_hide(widget);
	return;
}

void       ewl_container_handle_hide(void      *object,
                                     EwlEvent  *event,
                                     void      *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	UNUSED(event);
	UNUSED(data);
	ewl_container_foreach(widget, ewl_container_handle_hide_cb, NULL);
	return;
}

void       ewl_container_handle_move(void      *object,
                                     EwlEvent  *event,
                                     void      *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	UNUSED(widget);
	UNUSED(event);
	UNUSED(data);
	return;
}

void       ewl_container_handle_resize(void      *object,
                                       EwlEvent  *event,
                                       void      *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	UNUSED(widget);
	UNUSED(event);
	UNUSED(data);
	return;
}

