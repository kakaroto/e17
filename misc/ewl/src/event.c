#include "event.h"

/* EVENT QUEUE CALLS */
EwlList  *ewl_get_event_queue()
{
	static EwlList *event_queue = NULL;
	if (!event_queue)	{
		event_queue = ewl_list_new();
	}
	return event_queue;
}

char      ewl_events_pending()
{
	EwlList *event_queue = ewl_get_event_queue();
	return ewl_list_len(event_queue);
}

EwlEvent *ewl_next_event()
{
	EwlList *queue = ewl_get_event_queue();
	return EWL_EVENT(ewl_list_pop(queue));
}

void      ewl_queue(EwlEvent *event)
{
	ewl_event_queue(event);
	return;
}

void      ewl_event_queue(EwlEvent *event)
{
	EwlList *event_queue = ewl_get_event_queue();
	ewl_list_insert(event_queue, EWL_LIST_NODE(event));
	return;
}

void      ewl_event_queue_new(char *type, void *object)
{
	ewl_event_queue(ewl_event_new(type, object));
	return;
}

/* EVENT CALLBACK FUNCTIONS */
EwlCallbackData *ewl_callback_data_new(EwlCallback cb, void *data)
{
	EwlCallbackData *cb_data = malloc(sizeof(EwlCallbackData));
	cb_data->callback = cb;
	cb_data->data = data;
	return cb_data;
}


/* EVNET NEW/FREE FUNCTIONS */
EwlEvent *ewl_event_new(char *type, void *object)
{
	EwlEvent *event = malloc(sizeof(EwlEvent));
	event->node.next = NULL;
	event->node.prev = NULL;
	event->node.data = NULL;
	event->data = ewl_hash_new();
	ewl_event_set_type(event,type);
	ewl_event_set_object(event, object);
	return event;
}

void      ewl_event_free(EwlEvent *event)
{
	EwlCallback cb = ((EwlCallback) ewl_event_get_data(event,
	                                "free_callback"));
	if (cb)	cb(NULL, event,NULL);
	ewl_hash_free(event->data);
	free(event);
	return;
}

/* EVENT TYPE/OBJECT FUNCTIONS */
void      ewl_event_set_type(EwlEvent *event, char *type)
{
	ewl_event_set_data(event, "/event/type", ewl_string_dup(type));
	return;
}

char     *ewl_event_get_type(EwlEvent *event)
{
	return ewl_string_dup(ewl_event_get_data(event, "/event/type"));
}

void      ewl_event_set_object(EwlEvent *event, void *object)
{
	ewl_event_set_data(event, "/event/object", object);
	return;
}

void     *ewl_event_get_object(EwlEvent *event)
{
	return ewl_event_get_data(event, "/event/object");
}

/* EVENT DATA HASH FUCNTIONS */
void      ewl_event_set_data(EwlEvent *event, char *key, void *data)
{
	ewl_hash_set(event->data, key, data);
	return;
}

void     *ewl_event_get_data(EwlEvent *event, char *key)
{
	return ewl_hash_get(event->data, key);
}

