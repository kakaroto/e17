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
