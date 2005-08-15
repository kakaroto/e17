#include <evfs.h>

int main() {
	evfs_connection* con;
	evfs_file_uri_path* path;
	evfs_file_uri_path* dir_path;
	
	printf("EVFS Demo system..\n");

	con = evfs_connect();

	path = evfs_parse_uri("posix:///home/chaos/.bash_profile");
	dir_path = evfs_parse_uri("posix:///root/");

	printf("Plugin uri is '%s', for path '%s'\n\n", path->files[0]->plugin_uri, path->files[0]->path);


	evfs_monitor_add(con, path->files[0]);
	evfs_monitor_add(con, dir_path->files[0]);

	ecore_main_loop_begin();
	
	evfs_disconnect(con);
	
	return 0;
}
