#include <Etk.h>
#include "Entrance_Widgets.h"

Entrance_Widget
ew_radio_button_new(char *label, Entrance_Widget rb, void (*function)(void *, void *), void *data) {
	Entrance_Widget ew = ew_new();
	if(!ew)
		return NULL;
	
	if(rb /*&& ETK_IS_RADIO_BUTTON(ew->owner)*/)
	/* I don't know, but this one doesn't compile..
	 * We should check whether it's really a radio button
	 */
		ew->owner = etk_radio_button_new_from_widget(ETK_RADIO_BUTTON(rb->owner));
	else
		ew->owner = etk_radio_button_new(NULL);

	ew_title_set(ew, label);

	if(label)
		ew_button_label_set(ew, label);

	if(function)
		ew_button_onclick_set(ew, function, data);

	return ew;
}

