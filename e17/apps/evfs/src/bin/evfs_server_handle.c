#include <evfs.h>
#include <dlfcn.h>

void evfs_handle_monitor_start_command(evfs_client* client, evfs_command* command) {
	/*First get the plugin responsible for this file*/

	void (*evfs_monitor_start)(evfs_client* client, evfs_command* command);

	if (command->file_command.num_files > 0) {
		evfs_plugin* plugin = evfs_get_plugin_for_uri(command->file_command.files[0]->plugin_uri);

		if (!plugin) {
			printf("No plugin able to monitor this uri type\n");
		} else {
			printf("Requesting a file monitor from this plugin for uri type '%s'\n", command->file_command.files[0]->plugin_uri );
			(*plugin->functions->evfs_monitor_start)(client,command);
			
		}
	} else {
		printf("No files to monitor!\n");
	}
}

void evfs_handle_monitor_stop_command(evfs_client* client, evfs_command* command) {
	/*First get the plugin responsible for this file*/

	void (*evfs_monitor_start)(evfs_client* client, evfs_command* command);

	if (command->file_command.num_files > 0) {
		evfs_plugin* plugin = evfs_get_plugin_for_uri(command->file_command.files[0]->plugin_uri);

		if (!plugin) {
			printf("No plugin able to monitor this uri type\n");
		} else {
			printf("Requesting a file monitor end from this plugin for uri type '%s'\n", command->file_command.files[0]->plugin_uri );
			
			evfs_monitor_start = dlsym(plugin->dl_ref, EVFS_FUNCTION_MONITOR_STOP);
			if (evfs_monitor_start) {
				(*evfs_monitor_start)(client,command);
			}
			
		}
	} else {
		printf("No files to monitor!\n");
	}
}

void evfs_handle_file_remove_command(evfs_client* client, evfs_command* command) {
	printf("At remove handle\n");

	evfs_plugin* plugin = evfs_get_plugin_for_uri(command->file_command.files[0]->plugin_uri);
	if (plugin) {
		printf("Pointer here: %p\n", plugin->functions->evfs_file_remove);
		(*plugin->functions->evfs_file_remove)(command->file_command.files[0]->path);
	}
}
