#ifndef _EW_ENTRY_H
#define _EW_ENTRY_H

typedef struct _Entrance_Entry {
	Etk_Widget *owner;
	Etk_Widget *box;
	Etk_Widget *label;
	Etk_Widget *control;
} *Entrance_Entry;

#define EW_ENTRY_FREE(ew) if(1) \
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
		if(ew->label) \
		{ \
			free(ew->label); \
		} \
		if(ew->control) \
		{ \
			free(ew->control); \
		} \
		free(ew); \
	} \
} \
else \

Entrance_Entry ew_entry_new(const char *label, const char *text, int ispassword);
const char* ew_entry_get(Entrance_Entry);
void ew_entry_set(Entrance_Entry ew, const char *text);
void ew_entry_password_set(Entrance_Entry ew);
void ew_entry_password_clear(Entrance_Entry ew);



#endif
