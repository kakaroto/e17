#include "ewlbox.h"

EwlWidget *ewl_hbox_new(EwlBool homogeneous)
{
	EwlWidget *b = ewl_box_new(EWL_HBOX, homogeneous);
	FUNC_BGN("ewl_hbox_new");
	if (!b)	{
		ewl_debug("ewl_hbox_new", EWL_NULL_WIDGET_ERROR, "b");
	}
	FUNC_BGN("ewl_hbox_new");
	return b;
}

EwlWidget *ewl_vbox_new(EwlBool homogeneous)
{
	EwlWidget *b = ewl_box_new(EWL_VBOX, homogeneous);
	FUNC_BGN("ewl_vbox_new");
	if (!b)	{
		ewl_debug("ewl_vbox_new", EWL_NULL_WIDGET_ERROR, "b");
	}
	FUNC_BGN("ewl_vbox_new");
	return b;
}

EwlWidget *ewl_lbox_new()
{
	EwlWidget *b = ewl_box_new(EWL_LBOX, FALSE);
	FUNC_BGN("ewl_lbox_new");
	if (!b)	{
		ewl_debug("ewl_lbox_new", EWL_NULL_WIDGET_ERROR, "b");
	}
	FUNC_BGN("ewl_lbox_new");
	return b;
}

EwlWidget *ewl_box_new(EwlType type, EwlBool homogeneous)
{
	EwlBox *b = malloc(sizeof(EwlBox));
	FUNC_BGN("ewl_box_new");
	if (!b)	{
		ewl_debug("ewl_box_new", EWL_NULL_WIDGET_ERROR, "b");
	} else if (type<EWL_HBOX||type>EWL_LBOX)	{
		ewl_debug("ewl_box_new", EWL_OUT_OF_BOUNDS_ERROR, "type");
	} else {
		ewl_widget_set_type((EwlWidget*)b, type);
		ewl_box_init((EwlWidget*)b);
	}
	FUNC_BGN("ewl_box_new");
	return (EwlWidget*) b;
}

void       ewl_box_init(EwlWidget *b)
{
	EwlContainer *container = (EwlContainer*) b;
	EwlType       t = ewl_widget_get_type(b);
	FUNC_BGN("ewl_box_init");
	ewl_container_init(container);
	ewl_widget_set_type(b,t);
	ewl_widget_set_flag(b, CAN_RESIZE, TRUE);

	/* LOAD DB SHIT HERE */
	ewl_widget_get_theme(b,"/EwlBox");
	switch (t)	{
	case EWL_VBOX:
		ewl_widget_get_theme(b,"/EwlVBox");
		break;
	case EWL_HBOX:
		ewl_widget_get_theme(b,"/EwlHBox");
		break;
	case EWL_LBOX:
		ewl_widget_get_theme(b,"/EwlLBox");
		break;
	default:
		break;
	}

	ewl_callback_add(b,EWL_EVENT_RESIZE,_cb_ewl_box_event_handler, NULL);

	FUNC_END("ewl_box_init");
	return;
}


EwlType    ewl_box_get_type(EwlWidget *box)
{
	EwlType t = 0;
	FUNC_BGN("ewl_box_get_type");
	if (!box)	{
		ewl_debug("ewl_box_get_type", EWL_NULL_WIDGET_ERROR, "box");
	} else {
		t = ewl_widget_get_type(box);
	}
	FUNC_END("ewl_box_get_type");
	return t;
}

void       ewl_box_set_type(EwlWidget *box, EwlType type)
{
	FUNC_BGN("ewl_box_set_type");
	if (!box)	{
		ewl_debug("ewl_box_set_type", EWL_NULL_WIDGET_ERROR, "box");
	} else if (type<EWL_HBOX||type>EWL_LBOX)	{
		ewl_debug("ewl_box_set_type", EWL_OUT_OF_BOUNDS_ERROR, "type");
	} else {
		ewl_widget_set_type(box, type);
	}
	FUNC_BGN("ewl_box_new");
	return;
}


void       ewl_box_pack_start(EwlWidget *box, EwlWidget *child)
{

	return;
}

void       ewl_box_pack_end(EwlWidget *box, EwlWidget *child)
{
	FUNC_BGN("ewl_box_pack_end");
	if (!box)	{
		ewl_debug("ewl_box_pack_end", EWL_NULL_WIDGET_ERROR, "box");
	} else if (!child)	{
		ewl_debug("ewl_box_pack_end", EWL_NULL_WIDGET_ERROR, "child");
	} else {
		ewl_container_insert(box,child);
	}
	FUNC_END("ewl_box_pack_end");
	return;
}


void       ewl_box_remove(EwlWidget *box, EwlWidget *child)
{
	FUNC_BGN("ewl_box_remove");
	if (!box)	{
		ewl_debug("ewl_box_remove", EWL_NULL_WIDGET_ERROR, "box");
	} else if (!child)	{
		ewl_debug("ewl_box_remove", EWL_NULL_WIDGET_ERROR, "child");
	} else {
		ewl_container_remove(box,child);
	}
	FUNC_END("ewl_box_remove");
	return;
}

EwlBool _cb_ewl_box_event_handler(EwlWidget *widget, EwlEvent *ev, EwlData *d)
{

	return TRUE;
}
