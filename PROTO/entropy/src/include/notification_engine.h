#ifndef __NOTIFICATION_ENGINE_H_
#define __NOTIFICATION_ENGINE_H_

enum ENTROPY_NOTIFY_TYPES {
	ENTROPY_NOTIFY_GENERIC = 0,
	ENTROPY_NOTIFY_THUMBNAIL_REQUEST =1,
	ENTROPY_NOTIFY_FILELIST_REQUEST =2 ,
	ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL = 3,  /*For delayed-response dir-lists, e.g. from evfs*/
	ENTROPY_NOTIFY_FILE_CHANGE = 4,
	ENTROPY_NOTIFY_FILE_CREATE = 5,
	ENTROPY_NOTIFY_FILE_REMOVE = 6,
	ENTROPY_NOTIFY_FILE_REMOVE_DIRECTORY = 7,
	ENTROPY_NOTIFY_FILE_ACTION = 8,
	ENTROPY_NOTIFY_FILE_STAT_EXECUTED = 9,
	ENTROPY_NOTIFY_FILE_STAT_AVAILABLE = 10,
	ENTROPY_NOTIFY_FILE_PROGRESS = 11,
	ENTROPY_NOTIFY_USER_INTERACTION_YES_NO_ABORT = 12,
	ENTROPY_NOTIFY_FILE_METADATA_REQUEST = 13,
	ENTROPY_NOTIFY_FILE_METADATA_AVAILABLE = 14,
	ENTROPY_NOTIFY_EXTENDED_STAT = 15,
	ENTROPY_NOTIFY_METADATA_GROUPS = 16,
	ENTROPY_NOTIFY_COPY_REQUEST = 17,
	ENTROPY_NOTIFY_CUT_REQUEST = 18,
	ENTROPY_NOTIFY_PASTE_REQUEST = 19,
	ENTROPY_NOTIFY_HOVER = 20,
	ENTROPY_NOTIFY_DEHOVER = 21,
	ENTROPY_NOTIFY_AUTH_REQUEST =22,
	ENTROPY_NOTIFY_METADATA_ALL = 23
};

enum ENTROPY_GUI_EVENT_HINTS {
	ENTROPY_GUI_EVENT_HINT_WINDOW_NEW = 1
};

typedef struct entropy_notification_engine entropy_notification_engine;
struct entropy_notification_engine {
        pthread_t  notify_thread;
        int notify_thread_id;
	int terminate;
	Ecore_List* op_queue;
	Ecore_List* exe_queue;

	Ecore_Ipc_Server *server; /*Our ref to the IPC engine*/
	
};

typedef struct entropy_notify_event_cb_data entropy_notify_event_cb_data;
struct entropy_notify_event_cb_data {
	void (*cb)();
	void* data;
};

typedef struct entropy_notify_event entropy_notify_event;
struct entropy_notify_event {
	int processed; /*A flag to indicate that this event has begun processing*/	
	

	int event_type;
	struct entropy_plugin* plugin;
	char* function;
	void* data;
	int key;
	int hints;

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
	int hints;
};


int entropy_notify_loop(void* data);
entropy_notification_engine* entropy_notification_engine_init();
void entropy_notification_engine_destroy_thread(entropy_notification_engine* engine);
void entropy_notification_engine_destroy(entropy_notification_engine* engine);
entropy_notify_event* entropy_notify_event_new();
void entropy_notify_event_type_set(entropy_notify_event*, int);
void entropy_notify_event_destroy(entropy_notify_event* eevent);

void entropy_notify_request_destroy_list(entropy_notification_engine* notify, Ecore_List* list);
entropy_notify_event* entropy_notify_request_register(void* requestor, int event_type, struct entropy_plugin* request_plugin, char* request_function, void* request_data,  Ecore_List* cleanup);
void entropy_notify_event_callback_add(entropy_notify_event* event, void* cb, void* data);

void entropy_notify_lock_loop(entropy_notification_engine* notify);
void entropy_notify_unlock_loop(entropy_notification_engine* notify);
void entropy_notify_event_commit(entropy_notify_event* ev);
void entropy_notify_event_bulk_commit(entropy_notification_engine* engine, Ecore_List* events);
void entropy_notify_event_cleanup_add(entropy_notify_event* event, void* obj);
void entropy_notify_event_expire_requestor_layout(void*);

void entropy_notify_lock_malloc();
void entropy_notify_unlock_malloc();


#define ENTROPY_EVENT_LOCAL 0
#define ENTROPY_EVENT_GLOBAL 1

#endif
