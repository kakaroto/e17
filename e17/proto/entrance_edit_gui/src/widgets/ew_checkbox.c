#include <Etk.h>
#include "Entrance_Widgets.h"

Entrance_Widget
ew_checkbox_new(char *label, void (*function)(void *, void *), void *data) {
	Entrance_Widget ew = ew_new();
	if(!ew)
		return NULL;

	ew->owner = etk_check_button_new();
	if(label)
		ew_button_label_set(ew, label);
	if(function)
		ew_button_onclick_set(ew, function, data);

	return ew;
}

