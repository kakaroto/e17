#ifndef _EW_BUTTON_H
#define _EW_BUTTON_H

Entrance_Widget ew_button_new(const char *label, void (*func)(void *, void*), void *data);
void ew_button_onclick_set(Entrance_Widget ew, void (*func)(void*, void*), void *data);

#endif
