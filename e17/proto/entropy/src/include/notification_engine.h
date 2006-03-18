#ifndef __NOTIFICATION_ENGINE_H_
#define __NOTIFICATION_ENGINE_H_

//#include <pthread.h>

enum ENTROPY_NOTIFY_TYPES {
	ENTROPY_NOTIFY_GENERIC,
	ENTROPY_NOTIFY_THUMBNAIL_REQUEST,
	ENTROPY_NOTIFY_FILELIST_REQUEST,
	ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL,  /*For delayed-response dir-lists, e.g. from evfs*/
	ENTROPY_NOTIFY_FILE_CHANGE,
	ENTROPY_NOTIFY_FILE_CREATE,
	ENTROPY_NOTIFY_FILE_REMOVE,
	ENTROPY_NOTIFY_FILE_REMOVE_DIRECTORY,
	ENTROPY_NOTIFY_FILE_ACTION,
	ENTROPY_NOTIFY_FILE_STAT_EXECUTED,
	ENTROPY_NOTIFY_FILE_STAT_AVAILABLE,
	ENTROPY_NOTIFY_FILE_PROGRESS,
	ENTROPY_NOTIFY_USER_INTERACTION_YES_NO_ABORT
};

typedef struct entropy_notification_engine entropy_notification_engine;
struct entropy_notification_engine {
        pthread_t  notify_thread;
        int notify_thread_id;
	int terminate;
	Ecore_List* op_queue;
	Ecore_List* exe_queue;

	Ecore_Ipc_Server *server; /*Our ref to the IPC engine*/
	
	pthread_mutex_t op_queue_mutex;
	pthread_mutex_t loop_mutex; /*Block on this the whole time*/
	pthread_mutex_t exe_queue_mutex;
};

typedef struct entropy_notify_event_cb_data entropy_notify_event_cb_data;
struct entropy_notify_event_cb_data {
	void (*cb)();
	void* data;
};

typedef struct entropy_notify_event entropy_notify_event;
struct entropy_notify_event {
	
	

	int event_type;
	entropy_plugin* plugin;
	char* function;
	void* data;
	int key;

	Ecore_List* cb_list;
	Ecore_List* cleanup_list;
	

	void* return_struct;

	void* requestor_data;
};

typedef struct entropy_gui_event entropy_gui_event;
struct entropy_gui_event {
	char* event_type;
	void *data;
	int key;
};


void* entropy_notify_loop(void* data);
entropy_notification_engine* entropy_notification_engine_init();
void entropy_notification_engine_destroy_thread(entropy_notification_engine* engine);
void entropy_notification_engine_destroy(entropy_notification_engine* engine);
entropy_notify_event* entropy_notify_event_new();
void entropy_notify_event_type_set(entropy_notify_event*, int);
void entropy_notify_event_destroy(entropy_notify_event* eevent);

void entropy_notify_request_destroy_list(entropy_notification_engine* notify, Ecore_List* list);
entropy_notify_event* entropy_notify_request_register(entropy_notification_engine* notify, void* requestor, int event_type, entropy_plugin* request_plugin, char* request_function, void* request_data,  Ecore_List* cleanup);
void entropy_notify_event_callback_add(entropy_notify_event* event, void* cb, void* data);

void entropy_notify_lock_loop(entropy_notification_engine* notify);
void entropy_notify_unlock_loop(entropy_notification_engine* notify);
void entropy_notify_event_commit(entropy_notification_engine* engine, entropy_notify_event* ev);
void entropy_notify_event_bulk_commit(entropy_notification_engine* engine, Ecore_List* events);
void entropy_notify_event_cleanup_add(entropy_notify_event* event, void* obj);


#define ENTROPY_EVENT_LOCAL 0
#define ENTROPY_EVENT_GLOBAL 1

#endif
