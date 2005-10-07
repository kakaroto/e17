#include "evfs.h"


Ecore_List* client_list = NULL;
static int libevfs_registered_callback = 0;

evfs_connection* evfs_get_connection_for_id(int id) {
	evfs_connection* conn;
	
	ecore_list_goto_first(client_list);

	while ( (conn = ecore_list_next(client_list))) {
		if (conn->id == id) return conn;
	}

	return NULL;
	
}

int evfs_server_data (void* data, int type, void* event) {
   Ecore_Ipc_Event_Server_Data *e;
   if ((e = (Ecore_Ipc_Event_Server_Data *) event)) {
	   /*Special case, if it's an id notify, we can't really id the client without it*/
	   
	   if (e->major == EVFS_EV_NOTIFY_ID) {
		   /*We have an id, assign it to the most recent client*/
		   /*This assumes we don't drop anything - this could be dangerous*/

		   evfs_connection* client;
		   ecore_list_goto_first(client_list);
		   if ( (client = ecore_list_next(client_list))) {
		
			   
			   /*We have a client, let's see if it needs an id*/
			   if (client->id == MAX_CLIENT) {
				   memcpy(&client->id, e->data, sizeof(unsigned long));
			   } else {
				   fprintf(stderr, "Error, client already has an assigned id.  Dropped packet?\n");
				   return 1;
			   }
		   }
		   
	   } else {

		   /*Get the connection associated with this ipc ref*/
		   /*we should probably use a hash here, but we aren't, for two reasons:
		    * 	1: We don't even have a key until the server assigns us one
		    * 	2: Most clients will only have one connection, giving us O(n) (worst) -> O(1) (best)
		    */

		   evfs_connection* conn = evfs_get_connection_for_id(e->ref);

		   if (conn) {
			   ecore_ipc_message *msg = ecore_ipc_message_new(e->major, e->minor, e->ref, e->ref_to, e->response, e->data, e->size);
			   
			   if (conn->prog_event == NULL) {
				   /*We haven't started an event yet - make a new one*/
				   conn->prog_event = NEW(evfs_event);
			   }

			   if (evfs_read_event(conn->prog_event, msg)) {
				   /*True return == Event fully read*/

				   /*Execute callback if registered..*/
				   if (conn->callback_func) {
					   evfs_event* ev = conn->prog_event;
					   conn->prog_event = NULL; /*Detach this event from the conn.  Client is responsible for it now*/
								      
					   (*conn->callback_func)(ev);
				   } else {
					   printf("EVFS: Alert - no callback registered for event\n");
				   }
				   
			   }
		   } else {
			   printf(stderr, "EVFS: Could not find connection for clientId\n");
		   }
	   }
   }
}

evfs_connection* evfs_connect(void (*callback_func)(void*)) {
	ecore_init();
	ecore_ipc_init();
	
	evfs_connection* connection = NEW(evfs_connection);
	connection->id = MAX_CLIENT;
	connection->prog_event = NULL;
	connection->callback_func = callback_func;

        if (!libevfs_registered_callback) {
		libevfs_registered_callback = 1;
	       ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, evfs_server_data, NULL); 
	       client_list = ecore_list_new();

		ecore_list_append(client_list, connection);
	} else {
		ecore_list_prepend(client_list, connection);
	}

	if ( !(connection->server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, EVFS_IPC_TITLE, 0, NULL)) ) {
		fprintf(stderr, "Cannot connect to evfs server with '%s'..\n", EVFS_IPC_TITLE);
		exit(1);
		
		return NULL;
	}



	

	return connection;
}


void evfs_disconnect(evfs_connection* connection) {
	ecore_ipc_server_del(connection->server);
}


/*Function to parse a uri*/
/*We should rewrite this,use a proper parser*/
evfs_file_uri_path* evfs_parse_uri(char* uri) {
	char* pos;
	int i;
	evfs_filereference* ref;
	evfs_file_uri_path* path = NEW(evfs_file_uri_path);
	
	/*for (i=0;i<strlen(uri);i++) {
		printf("URI Input: '%s'\n", uri);
	}*/

	if (!strstr(uri, "#")) {
		/*Trivial case simple file uri*/

		/*Find the uri plugin seperator (e.g. "posix':'")*/
		if ( (pos = index(uri, ':')) ) {
			i = (pos - uri);
			
			ref = NEW(evfs_filereference);
			ref->plugin_uri = strndup(uri, i);
		} else {
			fprintf(stderr, "URI did not contain a colon\n");
			free(path);
			free(ref);
			return NULL;
		}

		/*Assign the rest to the path*/
		ref->path = strdup(pos+3);

		/*Only one file, therefore just assign our ref to the path structure*/
		path->files = malloc(sizeof(evfs_filereference*));
		path->files[0] = ref;
		path->num_files = 0;

		return path;
	}
	return NULL;
}
