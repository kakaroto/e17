#ifndef _EW_LABEL_H
#define _EW_LABEL_H

Entrance_Widget ew_label_new(const char *text);
void ew_label_set(Entrance_Widget ew, const char *text);
const char* ew_label_get(Entrance_Widget ew);

#endif
