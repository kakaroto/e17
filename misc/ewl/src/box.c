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

	ewl_callback_add(widget, "resize", ewl_box_resize_callback, NULL);
	
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

void       ewl_box_resize_callback(void *object, EwlEvent *event, void *data)
{
	EwlContainer *container = EWL_CONTAINER(object);
	EwlWidget    *widget = EWL_WIDGET(object),
                 *child;
	EwlIterator *i;
	EwlRect     *min,
	            *max,
	            *req;
	int         min_w = 0, min_h = 0,
	            max_w = 0, max_h = 0,
	            sum_w = 0, sum_h = 0;
	UNUSED(event);
	UNUSED(data);
	
	/*for (i=ewl_iterator_start(container->children); i;
	     i=ewl_iterator_next(i))	{
		child = EWL_WIDGET(i->data);
		if (ewl_widget_is_visible(child))	{
			req = ewl_widget_get_requested_rect(child);
			min = ewl_widget_get_min_rect(child);
			max = ewl_widget_get_max_rect(child);
			if (req)	{
				if (max_w<req->w) max_w = req->w;
				if (max_h<req->h) max_h = req->h;
				sum_w += req->w;
				sum_h += req->h;
			} else if (min)	{
				
			}

			if (ewl_box_is_homogeneous(widget))	{
				
			} else {
			}
		}
	}*/
	return;
}


