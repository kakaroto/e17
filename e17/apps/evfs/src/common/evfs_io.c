#include <evfs.h>

static pthread_mutex_t ipc_client_mutex = PTHREAD_MUTEX_INITIALIZER;


ecore_ipc_message* ecore_ipc_message_new(int major, int minor, int ref, int ref_to, int response, void* data, int len) {
        ecore_ipc_message* msg= malloc(sizeof(ecore_ipc_message));

        msg->major = major;
        msg->minor = minor;
        msg->ref = ref;
        msg->ref_to = ref_to;
        msg->response = response;
        msg->data = data;
        msg->len = len;
        msg->client=NULL;
        msg->server=NULL;
        msg->dest=0;

        return msg;
}


/*Writers*/
void evfs_write_ecore_ipc_server_message(Ecore_Ipc_Server* server, ecore_ipc_message* msg) {

	LOCK(&ipc_client_mutex);

	ecore_ipc_server_send(server, msg->major, msg->minor, msg->ref, msg->ref_to, msg->response,msg->data, msg->len);
        free(msg);

	UNLOCK(&ipc_client_mutex);
}


void evfs_write_command(evfs_connection* conn, evfs_command* command) {

	switch (command->type) {
		case EVFS_CMD_STARTMON_FILE:
			evfs_write_file_command(conn, command);
			break;
	}

}

void evfs_write_command_end(evfs_connection* conn) {
	evfs_write_ecore_ipc_server_message(conn->server, ecore_ipc_message_new(EVFS_COMMAND, EVFS_COMMAND_END, 0,0,0,NULL,0));
}

void evfs_write_file_command(evfs_connection* conn, evfs_command* command) {
	int i;
	char uri[1024];
	
	printf("Writing a file command..\n");

	/*Write the command type structure*/
	evfs_write_ecore_ipc_server_message(conn->server, ecore_ipc_message_new(EVFS_COMMAND, EVFS_COMMAND_TYPE, 0,0,0,&command->type, sizeof(evfs_command_type)));

	/*Write the files*/
	/*Send them de-parsed to save time*/
	for (i=0;i<command->file_command.num_files;i++) {
		snprintf(uri, 1024, "%s://%s", command->file_command.files[i]->plugin_uri, command->file_command.files[i]->path);
		evfs_write_ecore_ipc_server_message(conn->server, ecore_ipc_message_new(EVFS_COMMAND, EVFS_FILE_REFERENCE, 0,0,0,uri, sizeof(uri)));	
	}

	/*Send a final*/
	evfs_write_command_end(conn);
	

}
/*----------------------------*/



/*Readers*/
int evfs_process_incoming_command(evfs_command* command, ecore_ipc_message* message) {

	switch (message->minor) {
		case EVFS_COMMAND_TYPE:

			memcpy(&command->type,message->data,sizeof(evfs_command_type));

			
			break;
		case EVFS_FILE_REFERENCE: {
			evfs_file_uri_path* path = evfs_parse_uri(message->data);
			if (command->file_command.num_files == 0) {
				command->file_command.num_files = 1;
				command->file_command.files = malloc(sizeof(evfs_filereference*));
				command->file_command.files[0] = path->files[0];

				printf("Got a file: %s %s\n", command->file_command.files[0]->plugin_uri, command->file_command.files[0]->path);
				free(path);
			} else {
				printf("we already have %d files\n", command->file_command.num_files);
				/*TODO Handle multiple files*/
			}
						  
			printf("This is a file reference\n");
			
			
			break;
		}

		case EVFS_COMMAND_END:
			 return TRUE;
			 break;
	}

	return FALSE;
}

/*----------------------------*/
