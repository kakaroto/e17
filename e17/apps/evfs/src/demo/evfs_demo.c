#include <evfs.h>

int main() {
	evfs_connection* con;
	evfs_file_uri_path* path;
	evfs_file_uri_path* dir_path;
	char pathi[1024];
	
	printf("EVFS Demo system..\n");

	con = evfs_connect();

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
