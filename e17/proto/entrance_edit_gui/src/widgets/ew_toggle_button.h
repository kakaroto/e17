#ifndef _EW_TOGGLE_BUTTON_H
#define _EW_TOGGLE_BUTTON_H

Entrance_Widget ew_toggle_button_new(char *label, void (*click_function)(void *, void *), void *data);
void ew_toggle_button_active_set(Entrance_Widget ew, int state);
int ew_toggle_button_active_get(Entrance_Widget);

#endif
