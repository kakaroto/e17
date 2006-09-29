#include <Etk.h>
#include "Entrance_Widgets.h"

Entrance_Widget
ew_toggle_button_new(char *label, void (*function)(void *, void *), void *data) {
	Entrance_Widget ew = ew_new();
	if(!ew)
		return NULL;
	
	ew->owner = etk_toggle_button_new();
	ew_title_set(ew, label);
	if(label)
		ew_button_label_set(ew, label);
	
	if(function)
		ew_button_onclick_set(ew, function, data);

	return ew;
}

void
ew_toggle_button_active_set(Entrance_Widget ew, int state) {
	etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(ew->owner), state);
}

int
ew_toggle_button_active_get(Entrance_Widget ew) {
	return etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(ew->owner));
}
