#include <evfs.h>

static int mon_current =0; /*A demo of stopping monitoring, after 10 events*/
evfs_file_uri_path* dir_path;
evfs_connection* con;

void callback(evfs_event* data) {
	static char str_data[1024];
	
	if (data->type == EVFS_EV_FILE_MONITOR) {
				printf("DEMO: Received a file monitor notification\n");
				printf("DEMO: For file: '%s'\n", data->file_monitor.filename);
				mon_current++;
	}

	if (mon_current == 2) {
		snprintf(str_data,1024,"posix://%s/newfile", getenv("HOME"));
		
		evfs_file_uri_path* path = evfs_parse_uri(str_data);
		printf("Removing monitor...\n");
		evfs_monitor_remove(con, dir_path->files[0]);
	
		printf("DEMO: Removing HOME/newfile\n");
		evfs_client_file_remove(con, path->files[0]);
		
		
	}

	/*TODO : Free event*/
}

int main() {
	
	evfs_file_uri_path* path;
	
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
