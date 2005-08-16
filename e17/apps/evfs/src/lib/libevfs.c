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
	printf("Got message from server %d..\n", ((Ecore_Ipc_Event_Server_Data*)event)->major);

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
				   printf("This client is assigned id %ld\n", client->id);
			   } else {
				   printf(stderr, "Error, client already has an assigned id.  Dropped packet?\n");
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
	   }
   }
}

evfs_connection* evfs_connect() {
	ecore_init();
	ecore_ipc_init();
	
	evfs_connection* connection = NEW(evfs_connection);
	connection->id = MAX_CLIENT;
	connection->prog_event = NULL;

        if (!libevfs_registered_callback) {
		libevfs_registered_callback = 1;
		fprintf(stderr, "Registering callback at client..\n");
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
