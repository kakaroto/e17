#ifndef _BOX_H_
#define _BOX_H_

#include "object.h"
#include "widget.h"
#include "container.h"

#define EWL_BOX(a) ((EwlBox*)a)

typedef struct _EwlBox EwlBox;

struct _EwlBox {
	EwlContainer container;
};

EwlWidget *ewl_hbox_new(char homogeneous);
EwlWidget *ewl_vbox_new(char homogeneous);
EwlWidget *ewl_lbox_new(char homogeneous);

EwlWidget *ewl_box_new(char *type, char homogeneous);
void       ewl_box_init(EwlWidget *widget);
void       ewl_box_free(EwlWidget *widget);

void       ewl_box_pack_start(EwlWidget *widget, EwlWidget *child);
void       ewl_box_pack_end(EwlWidget *widget, EwlWidget *child);
void       ewl_box_remove(EwlWidget *widget, EwlWidget *child);

void       ewl_box_resize_callback(void *object, EwlEvent *event, void *data);

#endif /* _BOX_H_ */
