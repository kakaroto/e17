#ifndef _EW_DIALOG_H
#define _EW_DIALOG_H

struct __Entrance_Dialog {
	Etk_Widget *owner;
	Etk_Widget *box;
	Etk_Widget *hbox;
};

typedef struct __Entrance_Dialog _Entrance_Dialog;
typedef struct __Entrance_Dialog *Entrance_Dialog;

#define EW_DIALOG_FREE(ew) if(1) \
{ \
	if(ew) \
	{ \
		if(ew->owner) \
		{ \
			free(ew->owner); \
		} \
		if(ew->box) \
		{ \
			free(ew->box); \
		} \
		if(ew->hbox) \
		{ \
			free(ew->hbox); \
		} \
		free(ew); \
	} \
} \
else \

Entrance_Dialog ew_dialog_new(const char*, int);
void ew_dialog_show(Entrance_Dialog);
void ew_dialog_destroy(Entrance_Dialog);
void ew_dialog_add(Entrance_Dialog, Entrance_Widget);
void ew_dialog_close_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *);
void ew_dialog_apply_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *);
void ew_dialog_ok_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *);
Entrance_Widget ew_dialog_group_add(Entrance_Dialog, const char *);

#endif
