#include <Etk.h>
#include <Entrance_Widgets.h>


Entrance_Widget
ew_button_new(const char *label, void (*func)(void *, void*), void *data)
{
	Entrance_Widget ew = ew_new();
	if(!ew)
	{
		return NULL;
	}

	ew->owner = etk_button_new_with_label(label);
	ew_button_onclick_set(ew, func, data);

	return ew;
}

void
ew_button_onclick_set(Entrance_Widget ew, void (*func)(void*, void*), void *data)
{
	if(func)
		etk_signal_connect("clicked", ETK_OBJECT(ew->owner), ETK_CALLBACK(func), data);
}

