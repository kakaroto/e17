#ifndef _EW_NOTICE_H
#define _EW_NOTICE_H

#define EW_NOTICE_OK_BUTTON -5
#define EW_NOTICE_CANCEL_BUTTON -6
#define EW_NOTICE_CLOSE_BUTTON -7
#define EW_NOTICE_YES_BUTTON -8
#define EW_NOTICE_NO_BUTTON  -9
#define EW_NOTICE_APPLY_BUTTON -10

Entrance_Dialog ew_notice_new(const char *);
void ew_notice_show(Entrance_Dialog);
Entrance_Widget ew_notice_group_add(Entrance_Dialog, const char *, int);
void ew_notice_destroy(Entrance_Dialog);

void ew_notice_button_add(Entrance_Dialog, const char*, int, void (*)(void *, int, void *), void *);
void ew_notice_close_button_add(Entrance_Dialog, void (*)(void *,  int, void *), void *);
void ew_notice_apply_button_add(Entrance_Dialog, void (*)(void *, int, void *), void *);
void ew_notice_ok_button_add(Entrance_Dialog, void (*)(void *, int, void *), void *);
void ew_notice_yes_button_add(Entrance_Dialog, void (*)(void *, int, void *), void *);
void ew_notice_no_button_add(Entrance_Dialog, void (*)(void *, int, void *), void *);
void ew_notice_cancel_button_add(Entrance_Dialog, void (*)(void *, int, void *), void *);

#endif
