#ifndef _EVENT_H_
#define _EVENT_H_

#include "includes.h"
#include "error.h"
#include "list.h"
#include "hash.h"

#define EWL_EVENT(a) ((EwlEvent*)a)
#define EWL_CALLBACK(a) ((EwlCallback) a)
#define EWL_CALLBACK_DATA(a) ((EwlCallbackData*)a)

typedef struct _EwlEvent        EwlEvent;
typedef void (*EwlCallback)(void *object, EwlEvent *event, void *data);
typedef struct _EwlCallbackData EwlCallbackData;

struct _EwlEvent	{
	EwlListNode   node;
	EwlHash      *data;
};

struct _EwlCallbackData	{
	EwlCallback  callback;
	void        *data;
};

/* EVENT QUEUE CALLS */
EwlList  *ewl_get_event_queue();
char      ewl_events_pending();
EwlEvent *ewl_next_event();
void      ewl_queue(EwlEvent *event);
void      ewl_event_queue(EwlEvent *event);
void      ewl_event_queue_new(char *type, void *object);

/* EVENT CALLBACK FUNCTIONS */
EwlCallbackData *ewl_callback_data_new(EwlCallback cb, void *data);

/* EVNET NEW/FREE FUNCTIONS */
EwlEvent *ewl_event_new(char *type, void *object);
void      ewl_event_free(EwlEvent *event);

/* EVENT TYPE/OBJECT FUNCTIONS */
void      ewl_event_set_type(EwlEvent *event, char *type);
char     *ewl_event_get_type(EwlEvent *event);

void      ewl_event_set_object(EwlEvent *event, void *object);
void     *ewl_event_get_object(EwlEvent *event);

/* EVENT DATA HASH FUCNTIONS */
void      ewl_event_set_data(EwlEvent *event, char *key, void *data);
void     *ewl_event_get_data(EwlEvent *event, char *key);

#endif /* _EVENT_H_ */
