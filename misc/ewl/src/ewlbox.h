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

/* private event callback functions */
EwlBool    ewl_box_handle_realize(EwlWidget *widget,
                                  EwlEvent  *ev,
                                  EwlData   *data);
EwlBool    ewl_box_handle_resize(EwlWidget *widget,
                                  EwlEvent  *ev,
                                  EwlData   *data);

EwlBool    ewl_hbox_resize_children_foreach(EwlLL *node, EwlData *data);
EwlBool    ewl_vbox_resize_children_foreach(EwlLL *node, EwlData *data);
EwlBool    ewl_lbox_resize_children_foreach(EwlLL *node, EwlData *data);
void       ewl_box_handle_resize_children(EwlWidget *widget);

#endif /* _EWL_BOX_H_ */
