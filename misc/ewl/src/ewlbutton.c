#include "ewlbutton.h"

EwlWidget *ewl_button_new()
{
	EwlWidget *b = NULL;
	FUNC_BGN("ewl_button_new");
	b = ewl_button_new_with_image_and_label(NULL,NULL);
	FUNC_END("ewl_button_new");
	return b;
}

EwlWidget *ewl_button_new_with_label(char *label)
{
	EwlWidget *b = NULL;
	FUNC_BGN("ewl_button_new_with_label");
	b = ewl_button_new_with_image_and_label(NULL,label);
	FUNC_END("ewl_button_new_with_label");
	return  b;
}

EwlWidget *ewl_button_new_with_image(EwlImage *icon)
{
	EwlWidget *b = NULL;
	FUNC_BGN("ewl_button_new_with_image");
	b = ewl_button_new_with_image_and_label(icon, NULL);
	FUNC_END("ewl_button_new_with_image");
	return  b;
}

EwlWidget *ewl_button_new_with_image_and_label(EwlImage *icon, char *label)
{
	EwlButton *b = malloc(sizeof(EwlButton));
	FUNC_BGN("ewl_button_new_with_image_and_label");
	if (!b)	{
		ewl_debug("ewl_button_new_with_image_and_label", EWL_NULL_WIDGET_ERROR,
		          "b");
	} else {
		ewl_button_init(b,icon,label);
	}
	FUNC_END("ewl_button_new_with_image_and_label");
	return (EwlWidget*) b;
}

EwlBool _cb_ewl_button_event_handler(EwlWidget *w, EwlEvent *ev, EwlData *data)
{
	switch (ev->type)	{
	case EWL_EVENT_MOUSEDOWN:
		ewl_widget_grab(w);
		ewl_widget_set_state(w,EWL_STATE_CLICKED);
		break;
	case EWL_EVENT_MOUSEUP:
		ewl_widget_ungrab(w);
		ewl_widget_set_state(w,EWL_STATE_HILITED);
		break;
	default:
		break;
	}
	return TRUE;
}

void       ewl_button_init(EwlButton *b, EwlImage *icon, char *label)
{
	EwlWidget    *widget    = (EwlWidget*)    b;
	EwlContainer *container = (EwlContainer*) b;
	EwlImLayer   *il = NULL;
	FUNC_BGN("ewl_button_init");

	/* EwlContainer class init */
	ewl_container_init(container);
	ewl_container_set_max_children(widget, 1);
	ewl_container_set_propagate_events(widget, FALSE);
	ewl_widget_set_type(widget,EWL_BUTTON);

	ewl_button_set_flags(widget,0); /* zero out the flags */
	b->label = NULL;

	/* initialize defaults from registry/db here */
	ewl_widget_get_theme(widget, "/EwlButton");

	/* add default event handler -- one generic one cuz it's cleaner */
	ewl_widget_callback_add(widget, EWL_EVENT_MOUSEDOWN,
	                        _cb_ewl_button_event_handler, NULL);
	ewl_widget_callback_add(widget, EWL_EVENT_MOUSEUP,
	                        _cb_ewl_button_event_handler, NULL);

	/* parse user data here */
	if (icon)	{
		il = ewl_imlayer_new();
		if (!il)	{
			ewl_debug("ewl_button_init", EWL_NULL_ERROR, "il");
		} else {
			ewl_imlayer_image_insert(il, icon);
			ewl_widget_imlayer_insert(widget,il);
			ewl_button_set_flag(widget, EWL_BUTTON_HAS_IMAGE, TRUE);
		}
	}
	if (label)	{
		b->label = e_string_dup(label);
		ewl_button_set_flag(widget, EWL_BUTTON_HAS_LABEL, TRUE);
	}
		
	FUNC_END("ewl_button_init");
	return;
}

EwlBool    ewl_button_get_flag(EwlWidget *button, EwlFlag flag)
{
	EwlBool r = FALSE;
	FUNC_BGN("ewl_button_get_flag");
	if (!button)	{
		ewl_debug("ewl_button_get_flag", EWL_NULL_WIDGET_ERROR, "button");
	} else {
		r = ((EwlButton*)button)->flags & flag;
	}
	FUNC_END("ewl_button_get_flag");
	return r;
}

void       ewl_button_set_flag(EwlWidget *button, EwlFlag flag, EwlBool v)
{
	EwlButton *b = (EwlButton*)button;
	FUNC_BGN("ewl_button_get_flag");
	if (!button)	{
		ewl_debug("ewl_button_get_flag", EWL_NULL_WIDGET_ERROR, "button");
	} else {
		if (v)	{
			b->flags |= flag;
		} else if (b->flags|flag)	{
			b->flags ^= flag;
		}
	}
	FUNC_END("ewl_button_get_flag");
	return;
}

void       ewl_button_set_flags(EwlWidget *button, EwlFlag mask)
{
	EwlButton *b = (EwlButton*)button;
	FUNC_BGN("ewl_button_get_flag");
	if (!button)	{
		ewl_debug("ewl_button_get_flag", EWL_NULL_WIDGET_ERROR, "button");
	} else {
		b->flags = mask;
	}
	FUNC_END("ewl_button_get_flag");
	return;
}


