#include "icon.h"

EwlWidget *ewl_icon_new(char *path)
{
	EwlWidget *widget = malloc(sizeof(EwlIcon));
	ewl_icon_init(widget);
	ewl_set_string(widget, "/icon/path", path);
	return widget;
}

void       ewl_icon_init(EwlWidget *widget)
{
	ewl_widget_init(widget);
	ewl_object_set_type(widget, "EwlIcon");
	
	ewl_callback_add(widget, "realize", ewl_icon_realize_callback, NULL);
	return;
}


void       ewl_icon_set(EwlWidget *widget, char *path)
{
	Evas_Object  obj;
	EwlRect     *rect;
	double       r[4];
	ewl_set_string(widget, "/icon/path", path);
	if (ewl_widget_is_realized(widget))	{
		obj = evas_add_image_from_file(ewl_widget_get_evas(widget), path);
		evas_get_geometry(ewl_widget_get_evas(widget), obj,
		                                      &r[0], &r[1], &r[2], &r[3]);
		rect = ewl_rect_new_with_values(r[0], r[1], r[2], r[3]);
		fprintf(stderr,"setting icon.\n");
		ewl_widget_set_background(widget, obj, 0);
		ewl_widget_set_requested_rect(widget, rect);
	}
	return;
}

char      *ewl_icon_get(EwlWidget *widget)
{
	return ewl_get_string(widget, "/icon/path");
}

void       ewl_icon_realize_callback(void     *object,
                                     EwlEvent *event,
                                     void     *data)
{
	EwlWidget *widget = EWL_WIDGET(object);
	char *path = ewl_get_string(object, "/icon/path");

	UNUSED(event);
	UNUSED(data);

	ewl_widget_set_flag(widget, "realized", TRUE);
	
	ewl_widget_get_theme(widget, "EwlIcon");
	if (path)
		ewl_icon_set(widget, path);
	else fprintf(stderr,"icon_realize: path is null\n");
	return;
}
