#include "box.h"

EwlWidget *ewl_hbox_new(char homogeneous)
{
	return ewl_box_new("horizontal", homogeneous);	
}

EwlWidget *ewl_vbox_new(char homogeneous)
{
	return ewl_box_new("vertical", homogeneous);	
}

EwlWidget *ewl_lbox_new(char homogeneous)
{
	return ewl_box_new("layered", homogeneous);	
}


EwlWidget *ewl_box_new(char *type, char homogeneous)
{
	EwlWidget *widget = EWL_WIDGET(malloc(sizeof(EwlBox)));
	char      *c = malloc(1);

	ewl_box_init(widget);

	*c = homogeneous;
	ewl_set(widget, "/box/type", ewl_string_dup(type));
	ewl_set(widget, "/box/homogeneous", c);

	return widget;
}

void       ewl_box_init(EwlWidget *widget)
{
	EwlContainer *container = EWL_CONTAINER(widget);
	UNUSED(container);

	ewl_container_init(widget);
	ewl_set(widget, "/object/type", ewl_string_dup("EwlBox"));

	ewl_callback_add(widget, "resize", ewl_box_handle_resize, NULL);
	
	return;
}

void       ewl_box_free(EwlWidget *widget)
{
	ewl_widget_free(widget);
	return;
}

void       ewl_box_pack_start(EwlWidget *widget, EwlWidget *child)
{
	ewl_container_insert(widget, child);
	return;
}

void       ewl_box_pack_end(EwlWidget *widget, EwlWidget *child)
{
	ewl_container_push(widget,child);
	return;
}

void       ewl_box_remove(EwlWidget *widget, EwlWidget *child)
{
	ewl_container_remove(widget,child);
}

void       ewl_box_handle_resize(void *object, EwlEvent *event, void *data)
{
	UNUSED(object);
	UNUSED(event);
	UNUSED(data);
	return;
}


