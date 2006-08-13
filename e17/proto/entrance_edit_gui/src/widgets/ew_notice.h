#ifndef _EW_NOTICE_H
#define _EW_NOTICE_H

#define ICON_SIZE "48"
#define ENTRANCE_NOTICE_QUESTION_DIALOG ("question_" ICON_SIZE)
#define ENTRANCE_NOTICE_MESSAGE_DIALOG	("information_" ICON_SIZE)
#define ENTRANCE_NOTICE_WARNING_DIALOG	("warning_" ICON_SIZE)
#define ENTRANCE_NOTICE_ERROR_DIALOG		("error_" ICON_SIZE)

#define ENTRANCE_NOTICE_OK_BUTTON -5
#define ENTRANCE_NOTICE_CANCEL_BUTTON -6
#define ENTRANCE_NOTICE_YES_BUTTON -8
#define ENTRANCE_NOTICE_NO_BUTTON  -9

#define _EW_NOTICE_FREE(ew) if(1) \
{ \
	if(ew) \
	{ \
		if(ew->owner) \
		{ \
			free(ew->owner); \
		} \
		if(ew->box)	\
		{ \
			free(ew->box); \
		} \
		free(ew); \
	} \
} 

typedef struct _Entrance_Notice {
	Etk_Widget *owner;
	Etk_Widget *box;
} *Entrance_Notice;

Entrance_Notice ew_notice_new(const char *type, const char *title, const char *message, void (*)(Etk_Window *window));

void ew_notice_ok_button_add(Entrance_Notice ew, void (*)(Etk_Dialog *, int, void *));
void ew_notice_yes_button_add(Entrance_Notice ew, void (*)(Etk_Dialog *, int, void *));
void ew_notice_no_button_add(Entrance_Notice ew, void (*)(Etk_Dialog *, int, void *));
void ew_notice_cancel_button_add(Entrance_Notice ew, void (*)(Etk_Dialog *, int, void *));

int ew_notice_show(Entrance_Notice ew);
void ew_notice_destroy(Entrance_Notice ew);

#endif
