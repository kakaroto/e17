#ifndef _EW_DIALOG_H
#define _EW_DIALOG_H

Entrance_Widget ew_dialog_new(const char*, int);
void ew_dialog_show(Entrance_Widget);
void ew_dialog_destroy(Entrance_Widget);
void ew_dialog_add(Entrance_Widget, Entrance_Widget);
void ew_dialog_close_button_add(Entrance_Widget ew, void (*func)(void *, void*), void *);
void ew_dialog_apply_button_add(Entrance_Widget ew, void (*func)(void *, void*), void *);
void ew_dialog_ok_button_add(Entrance_Widget ew, void (*func)(void *, void*), void *);
Entrance_Widget ew_dialog_group_add(Entrance_Widget, const char *, int);

#endif
