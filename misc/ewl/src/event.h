#ifndef _EWL_EVENT_CLASS_H_
#define _EWL_EVENT_CLASS_H_ 1

#include "includes.h"
#include "debug.h"
#include "ll.h"
#include "util.h"
#include "layout.h"

typedef unsigned int                EwlEventType;
enum _EwlEventTypeEnum {
/* INTERNAL EVENTS */
	EWL_EVENT_NONE,
	EWL_EVENT_INIT,
	EWL_EVENT_MEDIA,

/* X EVENTS */
	EWL_EVENT_SHOW,
	EWL_EVENT_HIDE,
	EWL_EVENT_RESIZE,
	EWL_EVENT_MOVE,
	EWL_EVENT_MOUSEDOWN,
	EWL_EVENT_MOUSEUP,
	EWL_EVENT_MOUSEMOVE,
	EWL_EVENT_KEYDOWN,
	EWL_EVENT_KEYUP,
	EWL_EVENT_ENTER,
	EWL_EVENT_LEAVE,
	EWL_EVENT_FOCUSIN,
	EWL_EVENT_FOCUSOUT,
	EWL_EVENT_EXPOSE,
	EWL_EVENT_VISIBILITY,
	EWL_EVENT_CREATE,
	EWL_EVENT_DESTROY,
	EWL_EVENT_REPARENT,
	EWL_EVENT_CONFIGURE,
	EWL_EVENT_CIRCULATE,
	EWL_EVENT_PROPERTY,
	EWL_EVENT_COLORMAP,
	EWL_EVENT_CLIENT,
	EWL_EVENT_SELECTION,
	EWL_EVENT_LAST
};

static char *_EwlEventTypeStringEnum[] = {
/* INTERNAL EVENTS */
	"EWL_EVENT_NONE",
	"EWL_EVENT_INIT",
	"EWL_EVENT_MEDIA",

/* X EVENTS */
	"EWL_EVENT_SHOW",
	"EWL_EVENT_HIDE",
	"EWL_EVENT_RESIZE",
	"EWL_EVENT_MOVE",
	"EWL_EVENT_MOUSEDOWN",
	"EWL_EVENT_MOUSEUP",
	"EWL_EVENT_MOUSEMOVE",
	"EWL_EVENT_KEYDOWN",
	"EWL_EVENT_KEYUP",
	"EWL_EVENT_ENTER",
	"EWL_EVENT_LEAVE",
	"EWL_EVENT_FOCUSIN",
	"EWL_EVENT_FOCUSOUT",
	"EWL_EVENT_EXPOSE",
	"EWL_EVENT_VISIBILITY",
	"EWL_EVENT_CREATE",
	"EWL_EVENT_DESTROY",
	"EWL_EVENT_REPARENT",
	"EWL_EVENT_CONFIGURE",
	"EWL_EVENT_CIRCULATE",
	"EWL_EVENT_PROPERTY",
	"EWL_EVENT_COLORMAP",
	"EWL_EVENT_CLIENT",
	"EWL_EVENT_SELECTION",
	"EWL_EVENT_LAST"
};

typedef struct _EwlEvent EwlEvent;
struct _EwlEvent	{
	EwlLL             ll;
	EwlType           type;
	void             *widget; /* EwlWidget hasn't been defined yet */
	EwlData          *data;
	unsigned int      time;
};

#include "event_types.h"

EwlEvent      *ewl_event_none_new();
EwlEvent      *ewl_event_new();
EwlEvent      *ewl_event_new_by_type(EwlEventType t);
EwlEvent      *ewl_event_new_by_type_with_widget(EwlEventType  type,
                                                 void         *widget);
EwlEvent      *ewl_event_dup(EwlEvent *sev);
void           ewl_event_free(EwlEvent *ev);

EwlEventType   ewl_event_get_type(EwlEvent *ev);
/*void           ewl_event_set_type(EwlEvent *ev, EwlEventType type);*/
char          *ewl_event_get_type_string(EwlEvent *ev);

static char __depricated_eventtype_die_pedantic_die()	{
	if (!_EwlEventTypeStringEnum)	{
		return __depricated_eventtype_die_pedantic_die();
	}
	return 0;
}

#endif /* _EWL_EVENT_CLASS_H_ */
