#include "evfs.h"

void evfs_monitor_add(evfs_connection* conn, evfs_filereference* ref) {
	evfs_command* command = NEW(evfs_command);
	
	/*printf("Adding a monitor on: '%s' using '%s'\n", ref->path, ref->plugin_uri);*/

	command->type = EVFS_CMD_STARTMON_FILE;
	command->file_command.num_files = 1;
	command->file_command.files = malloc(sizeof(evfs_filereference*));
	command->file_command.files[0] = ref;


	evfs_write_command(conn, command);

	

	free(command);	
}

void evfs_monitor_remove(evfs_connection* conn, evfs_filereference* ref) {
	evfs_command* command = NEW(evfs_command);
	

	command->type = EVFS_CMD_STOPMON_FILE;
	command->file_command.num_files = 1;
	command->file_command.files = malloc(sizeof(evfs_filereference*));
	command->file_command.files[0] = ref;

	evfs_write_command(conn, command);

	

	free(command);	
}

void evfs_client_file_remove(evfs_connection* conn, evfs_filereference* ref) {
	evfs_command* command = NEW(evfs_command);

	printf("Removing a file..\n");

	command->type = EVFS_CMD_REMOVE_FILE;
	command->file_command.num_files = 1;
	command->file_command.files = malloc(sizeof(evfs_filereference*));
	command->file_command.files[0] = ref;

	evfs_write_command(conn, command);

	free(command);	

}

void evfs_client_file_rename(evfs_connection* conn, evfs_filereference* from, evfs_filereference* to) {
	evfs_command* command = NEW(evfs_command);

	printf("Renaming a file..\n");

	command->type = EVFS_CMD_REMOVE_FILE;
	command->file_command.num_files = 2;
	command->file_command.files = malloc(sizeof(evfs_filereference*)*2);
	command->file_command.files[0] = from;
	command->file_command.files[1] = to;

	evfs_write_command(conn, command);

	free(command);	

}

void evfs_client_file_stat(evfs_connection* conn, evfs_filereference* file) {
	evfs_command* command = NEW(evfs_command);

	printf("Stat'ing a file..\n");

	command->type = EVFS_CMD_FILE_STAT;
	command->file_command.num_files = 1;
	command->file_command.files = malloc(sizeof(evfs_filereference*)*1);
	command->file_command.files[0] = file;

	evfs_write_command(conn, command);

	free(command);	

}

void evfs_client_dir_list(evfs_connection* conn, evfs_filereference* file) {
	evfs_command* command = NEW(evfs_command);

	printf("Listing a directory..\n");

	command->type = EVFS_CMD_LIST_DIR;
	command->file_command.num_files = 1;
	command->file_command.files = malloc(sizeof(evfs_filereference*)*1);
	command->file_command.files[0] = file;

	evfs_write_command(conn, command);

	free(command);	

}

void evfs_client_file_copy(evfs_connection* conn, evfs_filereference* from, evfs_filereference* to) {
	
	evfs_command* command = NEW(evfs_command);

	/*printf("Copying a file..\n");*/

	command->type = EVFS_CMD_FILE_COPY;
	command->file_command.num_files = 2;
	command->file_command.files = malloc(sizeof(evfs_filereference*)*2);
	command->file_command.files[0] = from;
	command->file_command.files[1] = to;

	evfs_write_command(conn, command);

	free(command);	
}

void evfs_client_file_open(evfs_connection* conn, evfs_filereference* file) {
	evfs_command* command = NEW(evfs_command);

	fprintf(stderr, "Opening a file..\n");

	command->type = EVFS_CMD_FILE_OPEN;
	command->file_command.num_files = 1;
	command->file_command.files = malloc(sizeof(evfs_filereference*)*1);
	command->file_command.files[0] = file;

	evfs_write_command(conn, command);

	free(command);	
}


void evfs_client_file_read(evfs_connection* conn, evfs_filereference* file, int read_size) {
	evfs_command* command = NEW(evfs_command);

	//printf("Reading a file..\n");

	command->type = EVFS_CMD_FILE_READ;
	command->file_command.num_files = 1;
	command->file_command.files = malloc(sizeof(evfs_filereference*)*1);
	command->file_command.files[0] = file;
	command->file_command.extra = read_size;

	evfs_write_command(conn, command);

	free(command);	
}




