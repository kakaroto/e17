#ifndef  _EWL_BUTTON_H_
#define _EWL_BUTTON_H_ 1

#include "ewlcore.h"
#include "ewlcontainer.h"

typedef struct _EwlButton EwlButton;

enum   _EwlButtonFlagsEnum	{
	EWL_BUTTON_HAS_LABEL    = 1<<0,
	EWL_BUTTON_HAS_IMAGE    = 1<<1
};

struct _EwlButton	{
	EwlContainer container;
	EwlFlag      flags;
	char        *label; /* make this an ewl_entry */
};

EwlWidget *ewl_button_new();
EwlWidget *ewl_button_new_with_label(char *label);
EwlWidget *ewl_button_new_with_image(EwlImage *icon);
EwlWidget *ewl_button_new_with_image_and_label(EwlImage *icon, char *label);
void       ewl_button_init(EwlButton *b, EwlImage *icon, char *label);

EwlBool    ewl_button_get_flag(EwlWidget *button, EwlFlag flag);
void       ewl_button_set_flag(EwlWidget *button, EwlFlag flag, EwlBool v);
void       ewl_button_set_flags(EwlWidget *button, EwlFlag mask);

EwlBool _cb_ewl_button_event_handler(EwlWidget *w, EwlEvent *ev, EwlData *data);
#endif /* _EWL_BUTTON_H_ */
