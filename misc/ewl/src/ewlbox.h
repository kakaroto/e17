#ifndef _EWL_BOX_H_
#define _EWL_BOX_H_ 1

#include "ewlcore.h"
#include "ewlcontainer.h"

typedef struct _EwlBox EwlBox;

/* box types are enumerated in types.h */

struct _EwlBox	{
	EwlContainer container;
};

EwlWidget *ewl_box_new(EwlType type, EwlBool homogeneous);
void       ewl_box_init(EwlWidget *box);

EwlWidget *ewl_hbox_new(EwlBool homogeneous);
EwlWidget *ewl_vbox_new(EwlBool homogeneous);
EwlWidget *ewl_lbox_new();

EwlType    ewl_box_get_type(EwlWidget *b);
void       ewl_box_set_type(EwlWidget *box, EwlType type);

void       ewl_box_pack_start(EwlWidget *box, EwlWidget *child);
void       ewl_box_pack_end(EwlWidget *box, EwlWidget *child);

void       ewl_box_remove(EwlWidget *box, EwlWidget *child);

EwlBool    ewl_box_handle_realize(EwlWidget *widget,
                                  EwlEvent  *ev,
                                  EwlData   *data);
/* private functions */
EwlBool _cb_ewl_box_event_handler(EwlWidget *widget, EwlEvent *ev, EwlData *d);
#endif /* _EWL_BOX_H_ */
