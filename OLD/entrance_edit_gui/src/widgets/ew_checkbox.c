#include <Etk.h>
#include "Entrance_Widgets.h"


Entrance_Widget
ew_checkbox_new(char *label) {
	Entrance_Widget ew = ew_new();
	if(!ew)
		return NULL;

	ew->owner = etk_check_button_new();
	ew_title_set(ew, label);
	if(label)
		ew_button_label_set(ew, label);

	return ew;
}

int
ew_checkbox_is_active(Entrance_Widget ew)
{
	return (int) etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(ew->owner));
}

void
ew_checkbox_toggle(Entrance_Widget ew)
{
	etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(ew->owner));
}
