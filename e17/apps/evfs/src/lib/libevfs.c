#include "evfs.h"

evfs_connection* evfs_connect() {
	ecore_init();
	ecore_ipc_init();
	
	evfs_connection* connection = NEW(evfs_connection);

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
