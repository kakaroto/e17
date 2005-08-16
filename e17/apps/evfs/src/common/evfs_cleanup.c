#include <evfs.h>

void evfs_cleanup_command(evfs_command* command) {
	switch (command->type) {
		case EVFS_CMD_STARTMON_FILE:
			evfs_cleanup_file_command(command);
			break;
	}

	free(command);
}

void evfs_cleanup_file_command(evfs_command* command) {
	int i;
	for (i=0;i<command->file_command.num_files;i++) {
		free(command->file_command.files[i]->path);
		free(command->file_command.files[i]->plugin_uri);
		free(command->file_command.files[i]);
	}
}



void evfs_cleanup_file_monitor(evfs_file_monitor* mon) {
	if (mon->monitor_path) free (mon->monitor_path);
	free(mon);
}
