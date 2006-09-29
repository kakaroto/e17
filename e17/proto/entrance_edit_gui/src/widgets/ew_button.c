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

	ew_title_set(ew, label);

	if(label)
	{
		ew->owner = etk_button_new_with_label(label);
	}
	else
		ew->owner = etk_button_new();

	ew_button_onclick_set(ew, func, data);

	return ew;
}

void
ew_button_onclick_set(Entrance_Widget ew, void (*func)(void*, void*), void *data)
{
	if(func)
		etk_signal_connect("clicked", ETK_OBJECT(ew->owner), ETK_CALLBACK(func), data);
}

void
ew_button_label_set(Entrance_Widget ew, char *label) {
	if(!label)
		return;
	ew_title_set(ew, label);
	etk_button_label_set(ETK_BUTTON(ew->owner), label);
}

const char *
ew_button_label_get(Entrance_Widget ew) {
	return etk_button_label_get(ETK_BUTTON(ew->owner));
}
