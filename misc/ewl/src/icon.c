#include "icon.h"

EwlWidget *ewl_icon_new(char *path)
{
	EwlWidget *widget = malloc(sizeof(EwlIcon));
	ewl_set_string(widget, "/icon/path", path);
	ewl_icon_init(widget);
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
	ewl_set_string(widget, "/icon/path", path);
	if (ewl_widget_is_realized(widget))	{
		ewl_widget_set_background(widget,
			evas_add_image_from_file(ewl_widget_get_evas(widget),
			                         path), 0);
	}
	return;
}

char      *ewl_icon_get(EwlWidget *widget)
{
	return ewl_string_dup(ewl_get_string(widget, "/icon/path"));
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
	return;
}
