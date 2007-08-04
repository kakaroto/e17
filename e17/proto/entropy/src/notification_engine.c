#include "entropy.h"
#include "notification_engine.h"
#include <stdlib.h>
#include <dlfcn.h>

entropy_notification_engine* entropy_notification_engine_init() {
	entropy_notification_engine* notify = entropy_malloc(sizeof(entropy_notification_engine));

	/*Init the ecore_list for operation queueing */
	notify->op_queue = ecore_list_new();
	notify->exe_queue = ecore_list_new();
	
	//printf("Initializing the notify engine..\n");

	notify->server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);
	/*notify->notify_thread_id = pthread_create(&notify->notify_thread, NULL, entropy_notify_loop, (void*)notify);*/

	return notify;
}

void entropy_notification_engine_destroy_thread(entropy_notification_engine* engine) {
	/*Request a terminate*/
	engine->terminate = 1;
	entropy_notify_event* ev;
	void* clean;
	entropy_notify_event_cb_data* cb_data;

	/*Wait for thread to finish*/
	printf("Waiting for notify thread to finish..\n");
	printf("..Terminated\n");

	ecore_list_first_goto(engine->op_queue);
	while ( (ev = ecore_list_next(engine->op_queue)) ) {
		//printf("Freeing an event..\n");
	
		if (ev->cb_list) {
			ecore_list_first_goto(ev->cb_list);
			while ( (cb_data = ecore_list_next(ev->cb_list)) ) {
				entropy_free(cb_data);
			}

			ecore_list_destroy(ev->cb_list);
		}
		
		if (ev->cleanup_list) {
                        ecore_list_first_goto(ev->cleanup_list);
                        while ( (clean = ecore_list_next(ev->cleanup_list)) ) {
                                entropy_free(clean);
                        }

			ecore_list_destroy(ev->cleanup_list);
		}

	
	}

	printf("Terminating op queue...\n");
	ecore_list_destroy(engine->op_queue);
	printf("Terminating exe queue...\n");
	ecore_list_destroy(engine->exe_queue);

	/*Return*/
	printf("..Exiting thread\n");
}

void entropy_notification_engine_destroy(entropy_notification_engine* engine) {
	printf("Destroying notify engine..");
	if (engine) {

		/*Destroy the thread*/
		entropy_notification_engine_destroy_thread(engine);

		free(engine);
	}
	printf ("..done\n");
}


/*Remove a list of events, if they are on the waiting queue still*/
void entropy_notify_request_destroy_list(entropy_notification_engine* notify, Ecore_List* list) {
	return;	
}


int entropy_notify_loop(void* data) {
	entropy_notification_engine* notify = (entropy_notification_engine*)data;
	entropy_notify_event* next;

	
	void* (*call_func)(void* arg);

	ecore_list_first_goto (notify->op_queue);
	if ( (next = ecore_list_next(notify->op_queue)) ) {
		next->processed = 1;

		
		/*printf ("*************************************** Notify loop - Processing event..\n");	*/
		/*printf("Calling function '%s'\n", next->function);*/
		call_func = dlsym(next->plugin->dl_ref, next->function);	
		next->return_struct = (*call_func)(next->data);

		/*Relock to remove next*/
		ecore_list_first_remove(notify->op_queue);

		ecore_list_append(notify->exe_queue, next);

		ecore_ipc_server_send(notify->server, ENTROPY_IPC_EVENT_CORE, 6, 0, 0, 0, NULL, 0);
	} else {
	}
		

	return 1;
}

void entropy_notify_event_expire_requestor_layout(void* requestor) 
{
	/*HACK - assume the requestor is an entropy_gui_component_instance* -
	 * safe for now - but we commit ourselves here*/

	entropy_notification_engine* engine = entropy_core_get_core()->notify;
	entropy_gui_component_instance* instance = requestor;
	entropy_gui_component_instance* layout = instance->layout_parent;
	entropy_notify_event* ev;


	ecore_list_first_goto(engine->op_queue);
	ev = ecore_list_current(engine->op_queue);
	while ( ev) {
		if (  ((entropy_gui_component_instance*)ev->requestor_data)->layout_parent == layout) {
			if (!ev->processed) {
				ecore_list_remove(engine->op_queue);
				entropy_notify_event_destroy(ev);

				printf("Removed event, unprocessed: %p\n", ev);
				
				ev = ecore_list_current(engine->op_queue);
			} else {
				printf("Left event, processed: %p\n", ev);
				
				ev = ecore_list_next(engine->op_queue);
			}
		} else {
			printf("Left event, different layout: %p\n", ev);
			
			ev = ecore_list_next(engine->op_queue);
		}
	}
}

entropy_notify_event* entropy_notify_request_register(void* requestor, int event_type, entropy_plugin* request_plugin, char* request_function, void* request_data, Ecore_List* cleanup) {
	entropy_notify_event* event = entropy_notify_event_new();

	event->requestor_data = requestor;	
	event->event_type = event_type;
	event->plugin = request_plugin;
	event->data = request_data;

	
	event->function = request_function;
	
	return event;
	
}

void entropy_notify_event_type_set(entropy_notify_event* event, int type) {
	event->event_type =type;
}

entropy_notify_event* entropy_notify_event_new() {
	entropy_notify_event* event;
	
	event = entropy_malloc(sizeof(entropy_notify_event));
	event->return_struct = NULL;
	event->event_type = ENTROPY_NOTIFY_GENERIC;
	event->cb_list = ecore_list_new();
	event->cleanup_list = ecore_list_new();

	/*Track alloc*/
	allocated_events++;
	print_allocation();

	return event;
}

void entropy_notify_event_destroy(entropy_notify_event* eevent) {
        entropy_notify_event_cb_data* cb_data;
        void *clean;

	/*Remove any file references*/
	if (eevent->data) {
			switch (eevent->event_type) {
				case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
				case ENTROPY_NOTIFY_FILELIST_REQUEST: {
				}
				break;

				case ENTROPY_NOTIFY_THUMBNAIL_REQUEST: {
					/*Remove ref to thumbnail's file*/
					entropy_file_request* req = eevent->data;


					//printf("De-refing for thumbnail:\n");
					entropy_core_file_cache_remove_reference ( 
						req->file->md5);

				}
				break;

				case ENTROPY_NOTIFY_FILE_STAT_AVAILABLE: {
					/*Remove a reference to the calling file*/
					entropy_generic_file* file = eevent->data;


					//printf("Derefing for stat..\n");
					entropy_core_file_cache_remove_reference ( 
						file->md5);
				}
				break;			

				case ENTROPY_NOTIFY_HOVER: {
					entropy_generic_file* file = eevent->data;
					entropy_core_file_cache_remove_reference ( 
						file->md5);
				};
				break;
				case ENTROPY_NOTIFY_DEHOVER: {
					entropy_generic_file* file = eevent->data;
					entropy_core_file_cache_remove_reference ( 
						file->md5);
				};
				break;
				

			}
	}

	
	/*Cleanup the return struct*/
	if (eevent->return_struct) {
			switch (eevent->event_type) {
				case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
				case ENTROPY_NOTIFY_FILELIST_REQUEST: {
					Ecore_List* list = eevent->return_struct;
					entropy_generic_file* file;

					//printf("Freeing files at return struct for filelist..\n");
					
					/*It's a filelist request - return is an ecore list - destroy*/
					while ((file = ecore_list_first_remove(list))) {
						entropy_core_file_cache_remove_reference (file->md5);
					}
					ecore_list_destroy(eevent->return_struct);
				}
				break;
			
				case ENTROPY_NOTIFY_THUMBNAIL_REQUEST: {
				}
				break;
			
				case ENTROPY_NOTIFY_FILE_STAT_AVAILABLE: {
										 
				}
				break;			

				default:
				//printf("Event type: %d\n", eevent->event_type);
				break;
			}
	}

		/*This is potentially bad - we need some way to identify what we are freeing*/	
                ecore_list_first_goto(eevent->cb_list);
                while ( (cb_data = ecore_list_next(eevent->cb_list)) ) {
                        entropy_free(cb_data);
                }


                /*Nuke our cleanup objs*/
                ecore_list_first_goto(eevent->cleanup_list);
                while ( (clean = ecore_list_next(eevent->cleanup_list))) {
                        entropy_free(clean);
                }

                ecore_list_destroy(eevent->cleanup_list);
                ecore_list_destroy(eevent->cb_list);

                entropy_free(eevent);

}


void entropy_notify_event_callback_add(entropy_notify_event* event, void* cb, void* data) {
	entropy_notify_event_cb_data* cb_data = entropy_malloc(sizeof(entropy_notify_event_cb_data));

	cb_data->cb = cb;
	cb_data->data = data;
	
	ecore_list_append(event->cb_list, cb_data);
}

void entropy_notify_event_cleanup_add(entropy_notify_event* event, void* obj) {
	ecore_list_append(event->cleanup_list, obj);
}



void entropy_notify_event_commit(entropy_notify_event* ev) {
	entropy_notification_engine* engine = entropy_core_get_core()->notify;

	ecore_list_append(engine->op_queue, ev);

}

void entropy_notify_event_bulk_commit(entropy_notification_engine* engine, Ecore_List* list) {
	entropy_notify_event* ev;
	
	while ( (ev = ecore_list_first_remove(list))) {
		ecore_list_append(engine->op_queue, ev);
	}

	ecore_list_destroy(list);

}




void entropy_notify_lock_loop(entropy_notification_engine* notify) {

}

void entropy_notify_unlock_loop(entropy_notification_engine* notify) {

}

void entropy_notify_lock_malloc()
{
}

void entropy_notify_unlock_malloc()
{
}


