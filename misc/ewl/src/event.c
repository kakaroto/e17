#include "event.h"

EwlEvent *ewl_event_none_new()	{
	EwlEvent *ev = malloc(sizeof(EwlEvent));
	FUNC_BGN("ewl_event_none_new");
	if (!ev)	{
		ewl_debug("ewl_event_none_new",EWL_NULL_ERROR,"ev");
	} else {
		ev->type    = EWL_EVENT_NONE;
		ev->widget  = NULL;
		ev->data    = NULL;
		ev->time    = 0;
		ev->ll.data = NULL;
		ev->ll.next = NULL;
	}
	FUNC_END("ewl_event_none_new");
	return ev;
}

EwlEvent *ewl_event_new()	{
	EwlEvent *ev = ewl_event_none_new();
	FUNC_BGN("ewl_event_new");
	if (!ev)	{
		ewl_debug("ewl_event_new",EWL_NULL_ERROR,"ev");
	}
	FUNC_END("ewl_event_new");
	return ev;
}

EwlEvent *ewl_event_new_by_type(EwlEventType type)
{
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_event_new_by_type");
	switch(type)	{

/* INTERNAL EVENTS */
	case EWL_EVENT_NONE:
		ev = malloc(sizeof(EwlEvent));
		break;
	case EWL_EVENT_INIT:
		ev = malloc(sizeof(EwlEventInit));
		break;
	case EWL_EVENT_REALIZE:
		ev = malloc(sizeof(EwlEventRealize));
		break;
	case EWL_EVENT_UNREALIZE:
		ev = malloc(sizeof(EwlEventUnrealize));
		break;
	case EWL_EVENT_MEDIA:
		ev = malloc(sizeof(EwlEventMedia));
		break;

/* X EVENTS */
	case EWL_EVENT_SHOW:
		ev = malloc(sizeof(EwlEventShow));
		break;
	case EWL_EVENT_HIDE:
		ev = malloc(sizeof(EwlEventHide));
		break;
	case EWL_EVENT_RESIZE:
		ev = malloc(sizeof(EwlEventResize));
		break;
	case EWL_EVENT_MOVE:
		ev = malloc(sizeof(EwlEventMove));
		break;
	case EWL_EVENT_MOUSEDOWN:
		ev = malloc(sizeof(EwlEventMousedown));
		break;
	case EWL_EVENT_MOUSEUP:
		ev = malloc(sizeof(EwlEventMouseup));
		break;
	case EWL_EVENT_MOUSEMOVE:
		ev = malloc(sizeof(EwlEventMousemove));
		break;
	case EWL_EVENT_KEYDOWN:
		ev = malloc(sizeof(EwlEventKeydown));
		break;
	case EWL_EVENT_KEYUP:
		ev = malloc(sizeof(EwlEventKeyup));
		break;
	case EWL_EVENT_ENTER:
		ev = malloc(sizeof(EwlEventEnter));
		break;
	case EWL_EVENT_LEAVE:
		ev = malloc(sizeof(EwlEventLeave));
		break;
	case EWL_EVENT_FOCUSIN:
		ev = malloc(sizeof(EwlEventFocusin));
		break;
	case EWL_EVENT_FOCUSOUT:
		ev = malloc(sizeof(EwlEventFocusout));
		break;
	case EWL_EVENT_EXPOSE:
		ev = malloc(sizeof(EwlEventExpose));
		((EwlEventExpose*)ev)->rect = NULL;
		break;
	case EWL_EVENT_VISIBILITY:
		ev = malloc(sizeof(EwlEventVisibility));
		break;
	case EWL_EVENT_CREATE:
		ev = malloc(sizeof(EwlEventCreate));
		break;
	case EWL_EVENT_DESTROY:
		ev = malloc(sizeof(EwlEventDestroy));
		break;
	case EWL_EVENT_REPARENT:
		ev = malloc(sizeof(EwlEventReparent));
		break;
	case EWL_EVENT_CONFIGURE:
		ev = malloc(sizeof(EwlEventConfigure));
		break;
	case EWL_EVENT_CIRCULATE:
		ev = malloc(sizeof(EwlEventCirculate));
		break;
	case EWL_EVENT_PROPERTY:
		ev = malloc(sizeof(EwlEventProperty));
		break;
	case EWL_EVENT_COLORMAP:
		ev = malloc(sizeof(EwlEventColormap));
		break;
	case EWL_EVENT_CLIENT:
		ev = malloc(sizeof(EwlEventClient));
		break;
	case EWL_EVENT_SELECTION:
		ev = malloc(sizeof(EwlEventSelection));
		break;
		default:
			ewl_debug("ewl_event_new_by_type",EWL_OUT_OF_BOUNDS_ERROR,"type");
	}
	if (!ev) {
		ewl_debug("ewl_event_new_by_type", EWL_NULL_ERROR, "ev");
	} else {
		ev->type    = type;
		ev->widget  = NULL;
		ev->data    = NULL;
		ev->time    = 0;
		ev->ll.data = NULL;
		ev->ll.next = NULL;
	}
	FUNC_END("ewl_event_new_by_type");
	return ev;
}

EwlEvent      *ewl_event_new_by_type_with_widget(EwlEventType  type,
                                                 void         *widget)
{
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_event_new_by_type_with_widget");
	ev = ewl_event_new_by_type(type);
	if (!ev) {
		ewl_debug("ewl_event_new_by_type_with_widget",
		          EWL_NULL_ERROR, "ev");
	} else {
		ev->widget = widget;
	}
	FUNC_END("ewl_event_new_by_type_with_widget");
	return ev;
}


EwlEvent *ewl_event_dup(EwlEvent *sev)
{
	EwlEvent *tev = ewl_event_new();
	FUNC_BGN("ewl_event_dup");
	if (!tev)	{
		ewl_debug("ewl_event_dup",EWL_NULL_ERROR,"tev");
	} else if (!sev)	{
		ewl_debug("ewl_event_dup",EWL_NULL_ERROR,"sev");
		tev = NULL;
	} else {
		tev->type = sev->type;
		/* switch type here */
		tev->ll.next = NULL;
	}
	FUNC_END("ewl_event_dup");
	return tev;
}

void ewl_event_free(EwlEvent *ev)
{
	FUNC_BGN("ewl_event_free");
	if (!ev)	{
		ewl_debug("ewl_event_free",EWL_NULL_ERROR,"ev");
	} else {
		/* broken... switch here, cast then free */
		switch(ev->type)	{
		case EWL_EVENT_NONE:
			free((EwlEvent*)ev);
			break;
		case EWL_EVENT_INIT:
			free((EwlEventInit*)ev);
			break;
		case EWL_EVENT_REALIZE:
			free((EwlEventRealize*)ev);
			break;
		case EWL_EVENT_UNREALIZE:
			free((EwlEventUnrealize*)ev);
			break;
		case EWL_EVENT_MEDIA:
			free((EwlEventMedia*)ev);
			break;
		case EWL_EVENT_SHOW:
			free((EwlEventShow*)ev);
			break;
		case EWL_EVENT_HIDE:
			free((EwlEventHide*)ev);
			break;
		case EWL_EVENT_RESIZE:
			free((EwlEventResize*)ev);
			break;
		case EWL_EVENT_MOVE:
			free((EwlEventMove*)ev);
			break;
		case EWL_EVENT_MOUSEDOWN:
			free((EwlEventMousedown*)ev);
			break;
		case EWL_EVENT_MOUSEUP:
			free((EwlEventMouseup*)ev);
			break;
		case EWL_EVENT_MOUSEMOVE:
			free((EwlEventMousemove*)ev);
			break;
		case EWL_EVENT_KEYDOWN:
			free((EwlEventKeydown*)ev);
			break;
		case EWL_EVENT_KEYUP:
			free((EwlEventKeyup*)ev);
			break;
		case EWL_EVENT_ENTER:
			free((EwlEventEnter*)ev);
			break;
		case EWL_EVENT_LEAVE:
			free((EwlEventLeave*)ev);
			break;
		case EWL_EVENT_FOCUSIN:
			free((EwlEventFocusin*)ev);
			break;
		case EWL_EVENT_FOCUSOUT:
			free((EwlEventFocusout*)ev);
			break;
		case EWL_EVENT_EXPOSE:
			if (((EwlEventExpose*)ev)->rect)
				ewl_rect_free((((EwlEventExpose*)ev)->rect));
			free((EwlEventExpose*)ev);
			break;
		case EWL_EVENT_VISIBILITY:
			free((EwlEventVisibility*)ev);
			break;
		case EWL_EVENT_CREATE:
			free((EwlEventCreate*)ev);
			break;
		case EWL_EVENT_DESTROY:
			free((EwlEventDestroy*)ev);
			break;
		case EWL_EVENT_REPARENT:
			free((EwlEventReparent*)ev);
			break;
		case EWL_EVENT_CONFIGURE:
			free((EwlEventConfigure*)ev);
			break;
		case EWL_EVENT_CIRCULATE:
			free((EwlEventCirculate*)ev);
			break;
		case EWL_EVENT_PROPERTY:
			free((EwlEventProperty*)ev);
			break;
		case EWL_EVENT_COLORMAP:
			free((EwlEventColormap*)ev);
			break;
		case EWL_EVENT_CLIENT:
			free((EwlEventClient*)ev);
			break;
		case EWL_EVENT_SELECTION:
			free((EwlEventSelection*)ev);
			break;
			default:
				ewl_debug("ewl_event_free",
				          EWL_OUT_OF_BOUNDS_ERROR,
				          "type");
		}
		ev = NULL;
	}
	FUNC_END("ewl_event_free");
	return;
}


EwlEventType   ewl_event_get_type(EwlEvent *ev)
{
	EwlEventType t = 0;
	FUNC_BGN("ewl_event_get_type");
	if (!ev)	{
		ewl_debug("ewl_event_type_get", EWL_NULL_ERROR, "ev");
	} else {
		t = ev->type;
	}
	FUNC_BGN("ewl_event_get_type");
	return t;
}

/*void           ewl_event_set_type(EwlEvent *ev, EwlEventType type);*/
char          *ewl_event_get_type_string(EwlEvent *ev)
{
	char *type = NULL;
	FUNC_BGN("ewl_event_get_type_string");
	if (!ev)	{
		ewl_debug("ewl_event_type_string_get", EWL_NULL_ERROR, "ev");
	} else if (ev->type<0||ev->type>=EWL_EVENT_LAST)	{
		ewl_debug("ewl_event_type_string_get", EWL_OUT_OF_BOUNDS_ERROR,
		          "ev->type");
	} else {
		type = e_string_dup(_EwlEventTypeStringEnum[ev->type]);
	}
	FUNC_BGN("ewl_event_get_type_string");
	return type;
}
