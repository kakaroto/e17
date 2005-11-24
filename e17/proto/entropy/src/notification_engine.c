#include "entropy.h"
#include "notification_engine.h"
#include <pthread.h>
#include <stdlib.h>
#include <dlfcn.h>


entropy_notification_engine* entropy_notification_engine_init() {
	entropy_notification_engine* notify = entropy_malloc(sizeof(entropy_notification_engine));

	/*Init the ecore_list for operation queueing */
	notify->op_queue = ecore_list_new();
	notify->exe_queue = ecore_list_new();
	
	notify->terminate = 0;
	pthread_mutex_init(&notify->op_queue_mutex, NULL);
	pthread_mutex_init(&notify->loop_mutex, NULL);
	pthread_mutex_init(&notify->exe_queue_mutex, NULL);
	
	//printf("Initializing the notify engine..\n");

	notify->server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);
	
	notify->notify_thread_id = pthread_create(&notify->notify_thread, NULL, entropy_notify_loop, (void*)notify);

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
	pthread_mutex_lock(&engine->loop_mutex);
	printf("..Terminated\n");

	ecore_list_goto_first(engine->op_queue);
	while ( (ev = ecore_list_next(engine->op_queue)) ) {
		//printf("Freeing an event..\n");
	
		if (ev->cb_list) {
			ecore_list_goto_first(ev->cb_list);
			while ( (cb_data = ecore_list_next(ev->cb_list)) ) {
				entropy_free(cb_data);
			}

			ecore_list_destroy(ev->cb_list);
		}
		
		if (ev->cleanup_list) {
                        ecore_list_goto_first(ev->cleanup_list);
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


void* entropy_notify_loop(void* data) {
	entropy_notification_engine* notify = (entropy_notification_engine*)data;
	entropy_notify_event* next;

	
	int rc;
	
	void* (*call_func)(void* arg);

	pthread_mutex_lock(&notify->loop_mutex);
	
	while (!notify->terminate) {

		/*Lock the mutex*/
		rc = pthread_mutex_lock(&notify->op_queue_mutex);
		if (rc) {
			perror("pthread_mutex_lock: entropy_notify_loop");
			pthread_exit(NULL);
		}
		
		ecore_list_goto_first (notify->op_queue);
		if ( (next = ecore_list_next(notify->op_queue)) ) {
			pthread_mutex_unlock(&notify->op_queue_mutex);
			
			/*printf ("*************************************** Notify loop - Processing event..\n");	
			printf("Calling function '%s'\n", next->function);*/
			call_func = dlsym(next->plugin->dl_ref, next->function);	


			next->return_struct = (*call_func)(next->data);

			/*printf ("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Called\n");*/

			/*Relock to remove next*/
			rc = pthread_mutex_lock(&notify->op_queue_mutex);
			if (rc) {
				perror("pthread_mutex_lock: entropy_notify_loop");
				pthread_exit(NULL);
			}
			ecore_list_remove_first(notify->op_queue);
			pthread_mutex_unlock(&notify->op_queue_mutex);	
			

			
			

			/*Lock the mutex for the exe queue*/
			pthread_mutex_lock(&notify->exe_queue_mutex);
			ecore_list_append(notify->exe_queue, next);
			
			ecore_ipc_server_send(notify->server, 1, 6, 0, 0, 0, NULL, 0);
			pthread_mutex_unlock(&notify->exe_queue_mutex);

			

			
		} else {
			pthread_mutex_unlock(&notify->op_queue_mutex);
			/*printf("Notify loop - nothing to do.,.\n");*/
		}
		
		
		usleep(500); /*Allow the CPU to have a rest*/
	}

	pthread_mutex_unlock(&notify->loop_mutex);
	pthread_exit(NULL);
}

entropy_notify_event* entropy_notify_request_register(entropy_notification_engine* notify, void* requestor, int event_type, entropy_plugin* request_plugin, char* request_function, void* request_data, Ecore_List* cleanup) {
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


	
                ecore_list_goto_first(eevent->cb_list);
                while ( (cb_data = ecore_list_next(eevent->cb_list)) ) {
                        entropy_free(cb_data);
                }


                /*Nuke our cleanup objs*/

                ecore_list_goto_first(eevent->cleanup_list);
                while ( (clean = ecore_list_next(eevent->cleanup_list))) {
                        entropy_free(clean);
                }

                ecore_list_destroy(eevent->cleanup_list);
                ecore_list_destroy(eevent->cb_list);

                entropy_free(eevent);

		allocated_events--;
		print_allocation();

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



void entropy_notify_event_commit(entropy_notification_engine* engine, entropy_notify_event* ev) {

	pthread_mutex_lock(&engine->op_queue_mutex);
	ecore_list_append(engine->op_queue, ev);
	pthread_mutex_unlock(&engine->op_queue_mutex);

}

void entropy_notify_event_bulk_commit(entropy_notification_engine* engine, Ecore_List* list) {
	entropy_notify_event* ev;
	
	pthread_mutex_lock(&engine->op_queue_mutex);
	while ( (ev = ecore_list_remove_first(list))) {
		ecore_list_append(engine->op_queue, ev);
	}
	pthread_mutex_unlock(&engine->op_queue_mutex);

	ecore_list_destroy(list);

}




void entropy_notify_lock_loop(entropy_notification_engine* notify) {

	pthread_mutex_lock(&notify->op_queue_mutex);
}

void entropy_notify_unlock_loop(entropy_notification_engine* notify) {

	pthread_mutex_unlock(&notify->op_queue_mutex);
}

