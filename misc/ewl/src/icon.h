#ifndef _ICON_H_
#define _ICON_H_

#include "widget.h"

#define EWL_ICON(a) ((EwlIcon*)a)

typedef struct _EwlIcon EwlIcon;

struct _EwlIcon {
	EwlWidget widget;
};

EwlWidget *ewl_icon_new(char *path);
void       ewl_icon_init(EwlWidget *widget);

void       ewl_icon_set(EwlWidget *widget, char *path);
char      *ewl_icon_get(EwlWidget *widget);

void       ewl_icon_realize_callback(void     *object,
                                     EwlEvent *event,
                                     void     *data);
#endif /* _ICON_H_ */
