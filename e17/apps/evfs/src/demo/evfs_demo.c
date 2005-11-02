#include <evfs.h>

static int mon_current =0; /*A demo of stopping monitoring, after 10 events*/
evfs_file_uri_path* dir_path;
evfs_connection* con;

void callback(evfs_event* data) {
	if (data->type == EVFS_EV_FILE_MONITOR) {
				printf("DEMO: Received a file monitor notification\n");
				printf("DEMO: For file: '%s'\n", data->file_monitor.filename);
				mon_current++;
	} else if (data->type == EVFS_EV_STAT) {
		printf("Received stat event for file '%s'!\n", data->resp_command.file_command.files[0]->path);
		printf("File size: %ld\n", data->stat.stat_obj.st_size);
		printf("File inode: %ld\n", data->stat.stat_obj.st_ino);
		printf("File uid: %ld\n", data->stat.stat_obj.st_uid);
		printf("File gid: %ld\n", data->stat.stat_obj.st_gid);
		printf("Last access: %ld\n", data->stat.stat_obj.st_atime);
		printf("Last modify : %ld\n", data->stat.stat_obj.st_mtime);
	} else if (data->type == EVFS_EV_DIR_LIST) {
		evfs_filereference* ref;
		
		printf("Received a directory listing..\nFiles:\n\n");

		ecore_list_goto_first(data->file_list.list);
		while (  (ref= ecore_list_next(data->file_list.list)) ) {
			printf("(%s) Received file type for file: %d\n", ref->path, ref->file_type);
		}

		
	}

	/*if (mon_current == 2) {
		static char str_data[1024];
		snprintf(str_data,1024,"posix://%s/newfile", getenv("HOME"));
		
		evfs_file_uri_path* path = evfs_parse_uri(str_data);
		printf("Removing monitor...\n");
		evfs_monitor_remove(con, dir_path->files[0]);
	
		printf("DEMO: Removing HOME/newfile\n");
		evfs_client_file_remove(con, path->files[0]);
		
		
	}*/

	/*TODO : Free event*/
}

int main() {
	
	evfs_file_uri_path* path;
	
	char pathi[1024];
	
	printf("EVFS Demo system..\n");

	con = evfs_connect(&callback);

	//path = evfs_parse_uri("posix:///dev/ttyS0");

	
	//snprintf(pathi,1024,"posix://%s", getenv("HOME"));
	//snprintf(pathi,1024,"smb:///gown/MythVideos/musicvideos");
	snprintf(pathi,1024,"posix:///usr/src/linux-2.6.13.1.tar.bz2#bzip2:///#tar:///");
	
	
	printf ("Listing dir: %s\n", pathi);
	dir_path = evfs_parse_uri(pathi);

	

	printf("Plugin uri is '%s', for path '%s'\n\n", dir_path->files[0]->plugin_uri, dir_path->files[0]->path);

	
	/*evfs_monitor_add(con, dir_path->files[0]);

	evfs_client_file_copy(con, dir_path->files[0], NULL);*/

	evfs_client_dir_list(con, dir_path->files[0]);

	ecore_main_loop_begin();
	
	evfs_disconnect(con);
	
	return 0;
}
