#include <evfs.h>

void callback(evfs_event* data) {

	if (data->type == EVFS_EV_REPLY) {
		switch (data->sub_type) {
			case EVFS_EV_SUB_MONITOR_NOTIFY:
				printf("DEMO: Received a file monitor notification\n");
				printf("DEMO: For file: '%s'\n", data->data);
		}
	}

	/*TODO : Free event*/
}

int main() {
	evfs_connection* con;
	evfs_file_uri_path* path;
	evfs_file_uri_path* dir_path;
	char pathi[1024];
	
	printf("EVFS Demo system..\n");

	con = evfs_connect(&callback);

	path = evfs_parse_uri("posix:///dev/ttyS0");

	
	snprintf(pathi,1024,"posix://%s", getenv("HOME"));
	printf ("Monitoring dir: %s\n", pathi);
	dir_path = evfs_parse_uri(pathi);

	

	printf("Plugin uri is '%s', for path '%s'\n\n", dir_path->files[0]->plugin_uri, dir_path->files[0]->path);

	
	evfs_monitor_add(con, dir_path->files[0]);

	ecore_main_loop_begin();
	
	evfs_disconnect(con);
	
	return 0;
}
