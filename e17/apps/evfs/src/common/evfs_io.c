#include <evfs.h>


static pthread_mutex_t ipc_client_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t ipc_server_mutex = PTHREAD_MUTEX_INITIALIZER;


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


void evfs_event_client_id_notify(evfs_client* client) {

	/*printf("Notifying client of id %ld\n", client->id);*/
	evfs_write_ecore_ipc_client_message(client->client, ecore_ipc_message_new(EVFS_EV_NOTIFY_ID,0,0,0,0,&client->id, sizeof(long)));	
}

void evfs_write_event_file_monitor (evfs_client* client, evfs_event* event) {
	evfs_write_ecore_ipc_client_message(client->client, ecore_ipc_message_new(EVFS_EV_REPLY,EVFS_EV_PART_FILE_MONITOR_TYPE,client->id,0,0,&event->file_monitor.fileev_type, sizeof(evfs_file_monitor_type)));
	evfs_write_ecore_ipc_client_message(client->client, ecore_ipc_message_new(EVFS_EV_REPLY,EVFS_EV_PART_FILE_MONITOR_FILENAME,client->id,0,0,event->file_monitor.filename,event->file_monitor.filename_len));
	
	
}

void evfs_write_stat_event (evfs_client* client, evfs_event* event) {
	evfs_write_ecore_ipc_client_message(client->client, ecore_ipc_message_new(EVFS_EV_REPLY,EVFS_EV_PART_STAT_SIZE,client->id,0,0,&event->stat.stat_obj,sizeof(struct stat)));
	
	
}




void evfs_write_event(evfs_client* client, evfs_command* command, evfs_event* event) {
	printf("Sending event type '%d'\n", event->type);
	evfs_write_ecore_ipc_client_message(client->client, ecore_ipc_message_new(EVFS_EV_REPLY,EVFS_EV_PART_TYPE,client->id,0,0,&event->type, sizeof(evfs_eventtype)));

	/*Now write the source command, if any*/
	if (command) {
		evfs_write_command_client(client,command);
	}

	switch (event->type) {
		case EVFS_EV_FILE_MONITOR: evfs_write_event_file_monitor(client,event);
					   break;
		case EVFS_EV_STAT:	   
					   evfs_write_stat_event(client,event);
					   break;
	}

	evfs_write_ecore_ipc_client_message(client->client, ecore_ipc_message_new(EVFS_EV_REPLY,EVFS_EV_PART_END,client->id,0,0,NULL,0));	

	
}


int evfs_read_event(evfs_event* event, ecore_ipc_message* msg) {

	switch (msg->minor) {
		case EVFS_EV_PART_TYPE:
			
			memcpy(&event->type, msg->data, sizeof(evfs_eventtype));
			break;
		case EVFS_EV_PART_FILE_MONITOR_TYPE:
			memcpy(&event->file_monitor.fileev_type, msg->data, sizeof(evfs_file_monitor_type));
			break;
		case EVFS_EV_PART_FILE_MONITOR_FILENAME:
			event->file_monitor.filename = strdup(msg->data);
			event->file_monitor.filename_len = strlen(msg->data);
			break;
		case EVFS_EV_PART_STAT_SIZE:
			memcpy(&event->stat.stat_obj, msg->data, sizeof(struct stat));
			break;


		/*The pieces of the incoming command*/
		case EVFS_COMMAND_TYPE:
		case EVFS_FILE_REFERENCE:
		case EVFS_COMMAND_END:
			evfs_process_incoming_command(&event->resp_command, msg);
			break;	
			
			
		case EVFS_EV_PART_END:
			return TRUE;
			break;
	}


	return FALSE;

}




/*Writers*/
void evfs_write_ecore_ipc_server_message(Ecore_Ipc_Server* server, ecore_ipc_message* msg) {

	LOCK(&ipc_client_mutex);

	ecore_ipc_server_send(server, msg->major, msg->minor, msg->ref, msg->ref_to, msg->response,msg->data, msg->len);
        free(msg);

	UNLOCK(&ipc_client_mutex);
}

void evfs_write_ecore_ipc_client_message(Ecore_Ipc_Client* client, ecore_ipc_message* msg) {

	LOCK(&ipc_server_mutex);

	ecore_ipc_client_send(client, msg->major, msg->minor, msg->ref, msg->ref_to, msg->response,msg->data, msg->len);
        free(msg);

	UNLOCK(&ipc_server_mutex);
}


/*------------------------------------------------------------------------*/
//Some ugly duplication here - maybe we should consider reworking this so it can
//be generic

void evfs_write_command(evfs_connection* conn, evfs_command* command) {

	switch (command->type) {
		case EVFS_CMD_STOPMON_FILE:
		case EVFS_CMD_STARTMON_FILE:
		case EVFS_CMD_REMOVE_FILE:
		case EVFS_CMD_RENAME_FILE:
		case EVFS_CMD_FILE_STAT:
			evfs_write_file_command(conn, command);
			break;
	}

}

void evfs_write_command_client(evfs_client* client, evfs_command* command) {
	switch (command->type) {
		case EVFS_CMD_STOPMON_FILE:
		case EVFS_CMD_STARTMON_FILE:
		case EVFS_CMD_REMOVE_FILE:
		case EVFS_CMD_RENAME_FILE:
		case EVFS_CMD_FILE_STAT:
			evfs_write_file_command_client(client, command);
			break;
	}	
}

void evfs_write_command_end(evfs_connection* conn) {
	evfs_write_ecore_ipc_server_message(conn->server, ecore_ipc_message_new(EVFS_COMMAND, EVFS_COMMAND_END, 0,0,0,NULL,0));
}

void evfs_write_command_end_client(evfs_client* client) {
	evfs_write_ecore_ipc_client_message(client->client, ecore_ipc_message_new(EVFS_COMMAND, EVFS_COMMAND_END, client->id,0,0,NULL,0));
}

void evfs_write_file_command(evfs_connection* conn, evfs_command* command) {
	int i;
	char uri[1024];


	/*Write the command type structure*/
	evfs_write_ecore_ipc_server_message(conn->server, 
			ecore_ipc_message_new(EVFS_COMMAND, EVFS_COMMAND_TYPE, 0,0,0,
			&command->type, sizeof(evfs_command_type)));

	/*Write the files*/
	/*Send them de-parsed to save time*/
	for (i=0;i<command->file_command.num_files;i++) {
		snprintf(uri, 1024, "%s://%s", 
			command->file_command.files[i]->plugin_uri, 
			command->file_command.files[i]->path);

		evfs_write_ecore_ipc_server_message(conn->server, 
			ecore_ipc_message_new(EVFS_COMMAND, 
			EVFS_FILE_REFERENCE, 0,0,0,uri, sizeof(uri)));	
	}

	/*Send a final*/
	evfs_write_command_end(conn);
	

}

void evfs_write_file_command_client(evfs_client* client, evfs_command* command) {
	int i;
	char uri[1024];


	/*Write the command type structure*/
	evfs_write_ecore_ipc_client_message(client->client, 
			ecore_ipc_message_new(EVFS_COMMAND, EVFS_COMMAND_TYPE, client->id,0,0,
			&command->type, sizeof(evfs_command_type)));

	/*Write the files*/
	/*Send them de-parsed to save time*/
	for (i=0;i<command->file_command.num_files;i++) {
		snprintf(uri, 1024, "%s://%s", 
			command->file_command.files[i]->plugin_uri, 
			command->file_command.files[i]->path);


		evfs_write_ecore_ipc_client_message(client->client, 
			ecore_ipc_message_new(EVFS_COMMAND, 
			EVFS_FILE_REFERENCE, client->id,0,0,uri, sizeof(uri)));	
	}

	/*Send a final*/
	evfs_write_command_end_client(client);
	

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

				


				free(path);
			} else {
				
				//printf("we already have %d files\n", command->file_command.num_files);
				/*TODO Handle multiple files*/

			
				command->file_command.files = realloc(command->file_command.files, sizeof(evfs_filereference*)*(command->file_command.num_files+1));
				command->file_command.files[command->file_command.num_files] = path->files[0];
				
				command->file_command.num_files++;
			}
						  
			
			
			break;
		}

		case EVFS_COMMAND_END:	
			 /*TODO cleanp ref event*/
			 
					  
			 return TRUE;
			 break;
	}

	return FALSE;
}

/*----------------------------*/
