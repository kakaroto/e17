#include "object.h"

EwlList   *ewl_get_object_list()
{
	static EwlList *object_list = NULL;
	if (!object_list)	{
		object_list = ewl_list_new();
	}
	return object_list;
}

void       ewl_add(void *object)
{
	ewl_object_list_add(EWL_OBJECT(object));
	return;
}

void       ewl_object_list_add(EwlObject *object)
{
	EwlList *list = ewl_get_object_list();
	ewl_list_insert(list,ewl_list_node_new(object));
	return;
}

void       ewl_object_list_remove(EwlObject *object)
{
	EwlList *list = ewl_get_object_list();
	ewl_list_remove(list,ewl_list_find_by_value(list,object));
	return;
}


EwlObject *ewl_object_new()
{
	EwlObject *object = malloc(sizeof(EwlObject));
	if (!object)	{
		/* FIXME */
	} else {
		ewl_object_init(object);
	} 
	return object;
}

void       ewl_object_init(EwlObject *object)
{
	ewl_object_ref(object);
	object->event_callbacks = ewl_hash_new();
	object->handlers = ewl_hash_new();
	object->data = ewl_hash_new();

	ewl_handler_set(object, "/object/callback_add",
	                        ewl_callback_add_handler);
	ewl_handler_set(object, "/object/callback_push",
	                        ewl_callback_push_handler);
	/* FIXME -- add these in the future */
	/*ewl_set_handler(object, "/object/set_data",
	                        ewl_object_set_data_handler);
	ewl_set_handler(object, "/object/get_data",
	                        ewl_object_get_data_handler);
	ewl_set_handler(object, "/object/remove_data",
	                        ewl_object_remove_data_handler);*/

	ewl_set(object, "/object/type", ewl_string_dup("EwlObject"));
	ewl_set(object, "/object/name", ewl_string_dup("Unnamed Object"));
	ewl_add(object);
	return;
}

void       ewl_object_free(EwlObject *object)
{
	if (object->ref_count<=0)	{
		ewl_hash_free(object->event_callbacks);
		ewl_hash_free(object->handlers);
		ewl_hash_free(object->data);
		free(object);
	}
	return;
}

void       ewl_callback_add_handler(void        *object,
                                    char        *handler_type,
                                    EwlHash     *params)
{
	char        *type = ewl_hash_get(params, "type");
	EwlCallback  callback = EWL_CALLBACK(ewl_hash_get(params, "callback"));
	void        *data = ewl_hash_get(params, "data");

	EwlList     *list = ewl_hash_get(EWL_OBJECT(object)->event_callbacks, type);
	EwlCallbackData *cb_data = ewl_callback_data_new(callback,data);
	UNUSED(handler_type);

	fprintf(stderr,"HOORAY, IT'S WOKRING!\n");
	if (list)	{
		ewl_list_insert(list, ewl_list_node_new(cb_data));
	} else {
		list = ewl_list_new();
		ewl_list_insert(list, ewl_list_node_new(cb_data));
		ewl_hash_set(EWL_OBJECT(object)->event_callbacks, type, list);
	}
	return;
}

void       ewl_callback_add(void        *object,
                            char        *type,
                            EwlCallback  callback,
                            void        *data)
{
	EwlHandler handler = ewl_handler_get(object, "/object/callback_add");
	EwlHash *params;
	if (handler)	{
		params = ewl_hash_new();
		ewl_hash_set(params, "type", type);
		ewl_hash_set(params, "callback", (void*) callback);
		ewl_hash_set(params, "data", data);
		handler(object, "/object/callback_add", params);
		ewl_hash_free(params);
	} else {
		fprintf(stderr,"couldn't find handler for /object/callback_add\n");
	}
}

void       ewl_callback_push_handler(void    *object,
                                     char    *handler_type,
                                     EwlHash *params)
{
	char        *type = ewl_hash_get(params, "type");
	EwlCallback  callback = EWL_CALLBACK(ewl_hash_get(params, "callback"));
	void        *data = ewl_hash_get(params, "data");

	EwlList *list = ewl_hash_get(EWL_OBJECT(object)->event_callbacks, type);
	EwlCallbackData *cb_data = ewl_callback_data_new(callback,data);
	UNUSED(handler_type);

	if (list)	{
		ewl_list_push(list, ewl_list_node_new(cb_data));
	} else {
		list = ewl_list_new();
		ewl_list_push(list, ewl_list_node_new(cb_data));
		ewl_hash_set(EWL_OBJECT(object)->event_callbacks, type, list);
	}
	return;
}

void       ewl_callback_push(void        *object,
                             char        *type,
                             EwlCallback  callback,
                             void        *data)
{
	EwlHandler handler = ewl_handler_get(object, "/object/callback_push");
	EwlHash *params;
	if (handler)	{
		params = ewl_hash_new();
		ewl_hash_set(params, "type", type);
		ewl_hash_set(params, "callback", (void*) callback);
		ewl_hash_set(params, "data", data);
		handler(object, "/object/callback_push", params);
		ewl_hash_free(params);
	} else {
		fprintf(stderr,"couldn't find handler for /object/callback_push\n");
	}
}

void       ewl_object_handle_event(void *object, EwlEvent *event)
{
	EwlList     *list = ewl_hash_get(EWL_OBJECT(object)->event_callbacks,
	                                 ewl_event_get_type(event));
	EwlIterator *i = NULL;
	EwlCallbackData *callback_data;

	if (list)	{
		for (i=ewl_iterator_start(list); i; i=ewl_iterator_next(i))	{
			callback_data = EWL_CALLBACK_DATA(i->data);
			callback_data->callback(object, event, callback_data->data);
		}
	} else {
		fprintf(stderr,"DEBUG: ewl_object_handle_event(): didn't find "
		        "callback list for type \"%s\".\n",
		         ewl_event_get_type(event));
	}
	ewl_event_free(event);
	return;
}


void       ewl_handler_set(void *object, char *type, EwlHandler handler)
{
	ewl_hash_set(EWL_OBJECT(object)->handlers, type, (void*) handler);
	return;
}

EwlHandler ewl_handler_get(void *object, char *type)
{
	return EWL_HANDLER(ewl_hash_get(EWL_OBJECT(object)->handlers, type));
}

void       ewl_handler_remove(void *object, char *type)
{
	ewl_hash_remove(EWL_OBJECT(object)->handlers, type);
}


void       ewl_set(void *object, char *key, void *data)
{
	ewl_object_set_data(EWL_OBJECT(object), key, data);
	return;
}

void      *ewl_get(void *object, char *key)
{
	return ewl_object_get_data(EWL_OBJECT(object), key);
}

void       ewl_remove(void *object, char *key)
{
	ewl_object_remove_data(EWL_OBJECT(object), key);
	return;
}


void       ewl_set_flag(void *object, char *key, char flag)
{
	char *f = ewl_get(object,key);
	if (!f) f = malloc(1);
	*f = flag;
	return;
}

char       ewl_get_flag(void *object, char *key)
{
	char *f = ewl_get(object,key);
	return f?*f:0;
}

void       ewl_set_int(void *object, char *key, int val)
{
	int *ov = ewl_get(object,key);
	if (!ov) ov = malloc(sizeof(int));
	*ov = val;
	return;
}

int        ewl_get_int(void *object, char *key)
{
	int *val = ewl_get(object,key);
	return val?*val:0;
}

void       ewl_set_double(void *object, char *key, double val)
{
	double *ov = ewl_get(object,key);
	if (!ov) ov = malloc(sizeof(double));
	*ov = val;
	return;
}

double        ewl_get_double(void *object, char *key)
{
	double *val = ewl_get(object,key);
	return val?*val:0;
}

void       ewl_set_string(void *object, char *key, char *val)
{
	char *ov = ewl_get(object, key);
	if (ov)	free(ov);
	ewl_set(object,key,ewl_string_dup(val));
	return;
}

char      *ewl_get_string(void *object, char *key)
{
	return ewl_string_dup(ewl_get(object,key));
}


void       ewl_ref(void *object)
{
	ewl_object_ref(EWL_OBJECT(object));
	return;
}

void       ewl_unref(void *object)
{
	ewl_object_unref(EWL_OBJECT(object));
	return;
}


void       ewl_object_set_data(EwlObject *object, char *key, void *data)
{
	ewl_hash_set(object->data, key, data);
	return;
}

void      *ewl_object_get_data(EwlObject *object, char *key)
{
	return ewl_hash_get(object->data, key);
}

void       ewl_object_remove_data(EwlObject *object, char *key)
{
	ewl_hash_remove(object->data, key);
	return;
}

void       ewl_object_ref(EwlObject *object)
{
	object->ref_count++;
	return;
}

void       ewl_object_unref(EwlObject *object)
{
	object->ref_count--;
	if (object->ref_count<=0)
		ewl_object_free(object);
	return;
}


