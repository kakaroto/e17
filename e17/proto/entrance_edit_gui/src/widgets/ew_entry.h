#ifndef _EW_ENTRY_H
#define _EW_ENTRY_H

Entrance_Widget ew_entry_new(const char *, int);
const char* ew_entry_get(Entrance_Widget);
void ew_entry_set(Entrance_Widget ew, const char *text);
void ew_entry_password_set(Entrance_Widget ew);
void ew_entry_password_clear(Entrance_Widget ew);



#endif
