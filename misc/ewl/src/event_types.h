#ifndef _EWL_EVENT_TYPES_H_
#define _EWL_EVENT_TYPES_H_ 1

typedef unsigned int EwlModifierMask;

/* HIGER LEVEL EVENT WRAPPERS DECLARED AFTER WIDGET CORE STUFF */
typedef struct _EwlEventInit	EwlEventInit;
struct _EwlEventInit	{
	EwlEvent   event;
};

typedef struct _EwlEventMedia	EwlEventMedia;
struct _EwlEventMedia	{
	EwlEvent   event;
};

typedef EwlEvent EwlEventRealize;
typedef EwlEvent EwlEventUnrealize;

typedef struct _EwlEventShow	EwlEventShow;
struct _EwlEventShow	{
	EwlEvent event;
};

typedef struct _EwlEventHide	EwlEventHide;
struct _EwlEventHide	{
	EwlEvent event;
};

typedef struct _EwlEventResize	EwlEventResize;
struct _EwlEventResize	{
	EwlEvent event;
	int      x, y, w, h;
};

typedef struct _EwlEventMove	EwlEventMove;
struct _EwlEventMove	{
	EwlEvent event;
	int      x, y;
};

typedef struct _EwlEventMousedown	EwlEventMousedown;
struct _EwlEventMousedown	{
	EwlEvent event;
	int      x, y;
	int      button;
};

typedef struct _EwlEventMouseup	EwlEventMouseup;
struct _EwlEventMouseup	{
	EwlEvent event;
	int      x, y;
	int      button;
};

typedef struct _EwlEventMousemove	EwlEventMousemove;
struct _EwlEventMousemove	{
	EwlEvent event;
	int      x, y;
};

typedef struct _EwlEventKeydown	EwlEventKeydown;
struct _EwlEventKeydown	{
	EwlEvent         event;
	EwlModifierMask  mod_mask;
	char            *key;
};

typedef struct _EwlEventKeyup	EwlEventKeyup;
struct _EwlEventKeyup	{
	EwlEvent         event;
	EwlModifierMask  mod_mask;
	char            *key;
};

typedef struct _EwlEventEnter	EwlEventEnter;
struct _EwlEventEnter	{
	EwlEvent event;
};

typedef struct _EwlEventLeave	EwlEventLeave;
struct _EwlEventLeave	{
	EwlEvent event;
};

typedef struct _EwlEventFocusin	EwlEventFocusin;
struct _EwlEventFocusin	{
	EwlEvent event;
};

typedef struct _EwlEventFocusout	EwlEventFocusout;
struct _EwlEventFocusout	{
	EwlEvent event;
};

typedef struct _EwlEventExpose	EwlEventExpose;
struct _EwlEventExpose	{
	EwlEvent  event;
	EwlRect  *rect;
	int       count;
};

typedef struct _EwlEventVisibility	EwlEventVisibility;
struct _EwlEventVisibility	{
	EwlEvent event;
};

typedef struct _EwlEventCreate	EwlEventCreate;
struct _EwlEventCreate	{
	EwlEvent event;
};

typedef struct _EwlEventDestroy	EwlEventDestroy;
struct _EwlEventDestroy	{
	EwlEvent event;
};

typedef struct _EwlEventReparent	EwlEventReparent;
struct _EwlEventReparent	{
	EwlEvent   event;
	void      *parent; /* should be EwlWidget, but it's not declared yet */
};

typedef struct _EwlEventConfigure	EwlEventConfigure;
struct _EwlEventConfigure	{
	EwlEvent event;
	int      width;
	int      height;
};

typedef struct _EwlEventCirculate	EwlEventCirculate;
struct _EwlEventCirculate	{
	EwlEvent event;
};

typedef struct _EwlEventProperty	EwlEventProperty;
struct _EwlEventProperty	{
	EwlEvent event;
};

typedef struct _EwlEventColormap	EwlEventColormap;
struct _EwlEventColormap	{
	EwlEvent event;
};

typedef struct _EwlEventClient	EwlEventClient;
struct _EwlEventClient	{
	EwlEvent event;
};

typedef struct _EwlEventSelection	EwlEventSelection;
struct _EwlEventSelection	{
	EwlEvent event;
};

#endif /* _EWL_EVENT_TYPES_H_ */
