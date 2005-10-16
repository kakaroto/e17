#include <evfs.h>
#include <dlfcn.h>

void evfs_handle_monitor_start_command(evfs_client* client, evfs_command* command) {
	/*First get the plugin responsible for this file*/


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


void evfs_handle_file_rename_command(evfs_client* client, evfs_command* command) {
	printf("At rename handle\n");

	evfs_plugin* plugin = evfs_get_plugin_for_uri(command->file_command.files[0]->plugin_uri);
	if (plugin) {
		printf("Pointer here: %p\n", plugin->functions->evfs_file_rename);
		(*plugin->functions->evfs_file_rename)(client,command);
	}
}

void evfs_handle_file_stat_command(evfs_client* client, evfs_command* command) {
	static struct stat file_stat;
	
	printf ("At file stat handler\n");
	printf("Looking for plugin for '%s'\n", command->file_command.files[0]->plugin_uri);
	evfs_plugin* plugin = evfs_get_plugin_for_uri(command->file_command.files[0]->plugin_uri);
	if (plugin) {
		printf("Pointer here: %p\n", plugin->functions->evfs_file_stat);
		(*(plugin->functions->evfs_file_stat))(command, &file_stat);

		evfs_stat_event_create(client, command, &file_stat);
	}
	printf("Handled event, client is %p\n", client);

}


void evfs_handle_dir_list_command(evfs_client* client, evfs_command* command) {
	printf ("At dir list handler\n");

	evfs_plugin* plugin = evfs_get_plugin_for_uri(command->file_command.files[0]->plugin_uri);
	if (plugin) {
		printf("Pointer here: %p\n", plugin->functions->evfs_dir_list);
		(*plugin->functions->evfs_dir_list)(client,command);
	}

}

void evfs_handle_file_copy(evfs_client* client, evfs_command* command) {
	evfs_plugin* plugin;
	evfs_plugin* dst_plugin;
	
	char bytes[COPY_BLOCKSIZE];
	long count;
	long read_write_bytes = 0;
	static struct stat file_stat;
	evfs_filereference* ref = NEW(evfs_filereference);

	printf ("At test handler\n");

	/*Make a dummy 'to' file for now*/
	/*ref->plugin_uri = strdup("smb");
	ref->plugin = evfs_get_plugin_for_uri(ref->plugin_uri);
	ref->parent = NULL;
	ref->path = strdup("/gown/MythVideos/musicvideos/testcopy.dat");*/
	
	

 	plugin = evfs_get_plugin_for_uri(command->file_command.files[0]->plugin_uri);
	dst_plugin = evfs_get_plugin_for_uri(command->file_command.files[1]->plugin_uri);
	
	if (plugin && dst_plugin) {
		(*dst_plugin->functions->evfs_file_create)(command->file_command.files[1]);
		(*plugin->functions->evfs_file_open)(command->file_command.files[0]);

		/*Get the source file size*/
		(*plugin->functions->evfs_file_stat)(command, &file_stat);
		printf("Source file size: %d bytes\n", file_stat.st_size);
		
		
		count = 0;
		while (count < file_stat.st_size) {
			(*plugin->functions->evfs_file_seek)(command->file_command.files[0], count, SEEK_SET);

			read_write_bytes = (file_stat.st_size > count + COPY_BLOCKSIZE) ? COPY_BLOCKSIZE : (file_stat.st_size - count);
			printf("Reading/writing %d bytes\n", read_write_bytes);
			
			(*plugin->functions->evfs_file_read)(command->file_command.files[0], bytes, read_write_bytes );

			(*dst_plugin->functions->evfs_file_write)(command->file_command.files[1], bytes, read_write_bytes );

			

			count+= COPY_BLOCKSIZE;
		}
		
		

		(*dst_plugin->functions->evfs_file_close)(command->file_command.files[1]);
		(*plugin->functions->evfs_file_close)(command->file_command.files[0]);
		
	}

}
