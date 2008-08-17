#include <Etk.h>
#include "Entrance_Widgets.h"

Entrance_Widget 
ew_image_new(int w, int h)
{
	Entrance_Widget ew = ew_new();
	if(ew) 
	{
		ew->owner = etk_image_new();
		ew->box = NULL;

		etk_widget_size_request_set(ETK_WIDGET(ew->owner), w, h);
	}

	return ew;
}

void
ew_image_file_load(Entrance_Widget ew, const char *filename)
{
	etk_image_set_from_file(ETK_IMAGE(ew->owner), filename, NULL);
}

void 
ew_image_edje_load(Entrance_Widget ew, const char *edje_filename, const char *edje_group)
{
	etk_image_set_from_edje(ETK_IMAGE(ew->owner), edje_filename, edje_group);
}
